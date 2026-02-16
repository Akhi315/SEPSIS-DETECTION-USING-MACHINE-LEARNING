from flask import Flask, render_template, request, jsonify
import joblib
import numpy as np

app = Flask(__name__)

# -------------------------
# Load models (ensure these .pkl files exist)
# -------------------------
models = {
    "Logistic Regression": joblib.load("Logistic_Regression_model.pkl"),
    "Random Forest": joblib.load("Random_Forest_model.pkl"),
    "Decision Tree": joblib.load("Decision_Tree_model.pkl"),
    "SVM": joblib.load("SVM_model.pkl"),
    "KNN": joblib.load("KNN_model.pkl")
}

# -------------------------
# Shared storage for latest sensor values
# -------------------------
latest_sensor_data = {
    "Heart_Rate": 0.0,
    "SpO2": 0.0,
    "Body_Temperature": 0.0,
    "Respiratory_Rate": 0.0
}

# -------------------------
# Main page (form + live values)
# -------------------------
@app.route("/", methods=["GET", "POST"])
def index():
    prediction_results = {}
    final_prediction = None

    if request.method == "POST":
        Age = float(request.form.get("Age", 0))
        Sex = 1 if request.form.get("Sex", "Male") == "Male" else 0
        Heart_Rate = float(request.form.get("Heart_Rate", latest_sensor_data["Heart_Rate"] or 0))
        SpO2 = float(request.form.get("SpO2", latest_sensor_data["SpO2"] or 0))
        Body_Temperature = float(request.form.get("Body_Temperature", latest_sensor_data["Body_Temperature"] or 0))
        Respiratory_Rate = float(request.form.get("Respiratory_Rate", latest_sensor_data["Respiratory_Rate"] or 0))

        features = np.array([Age, Sex, Heart_Rate, SpO2, Body_Temperature, Respiratory_Rate]).reshape(1, -1)

        selected_models = request.form.getlist("models")
        votes = []
        for model_name in selected_models:
            model = models[model_name]
            result = model.predict(features)[0]
            prediction_results[model_name] = "🚨 Sepsis Detected" if result == 1 else "✅ No Sepsis"
            votes.append(result)

        if votes:
            final_prediction = "🚨 Sepsis Detected" if sum(votes) > len(votes) / 2 else "✅ No Sepsis"

    return render_template("form.html",
                           prediction_results=prediction_results,
                           final_prediction=final_prediction,
                           sensor_data=latest_sensor_data)

# -------------------------
# Endpoint for ESP32 to POST sensor data
# -------------------------
@app.route("/data", methods=["POST"])
@app.route("/upload", methods=["POST"])
def receive_data():
    try:
        data = request.get_json(force=True)
        if not data:
            return jsonify({"status": "error", "message": "No JSON"}), 400

        # Accept different key names just in case
        hr = data.get("heartRate") or data.get("heart_rate") or data.get("HR") or data.get("Heart_Rate")
        spo2 = data.get("spo2") or data.get("SpO2")
        temp = data.get("temperature") or data.get("temp") or data.get("Body_Temperature")
        rr = data.get("respiratoryRate") or data.get("respiratory_rate") or data.get("Respiratory_Rate")

        latest_sensor_data["Heart_Rate"] = float(hr) if hr is not None else latest_sensor_data["Heart_Rate"]
        latest_sensor_data["SpO2"] = float(spo2) if spo2 is not None else latest_sensor_data["SpO2"]
        latest_sensor_data["Body_Temperature"] = float(temp) if temp is not None else latest_sensor_data["Body_Temperature"]
        latest_sensor_data["Respiratory_Rate"] = float(rr) if rr is not None else latest_sensor_data["Respiratory_Rate"]

        print(f"📩 Received → HR: {latest_sensor_data['Heart_Rate']} | SpO2: {latest_sensor_data['SpO2']} | Temp: {latest_sensor_data['Body_Temperature']} | RR: {latest_sensor_data['Respiratory_Rate']}")

        return jsonify({"status": "success"}), 200
    except Exception as e:
        print("❌ Error receiving data:", e)
        return jsonify({"status": "error", "message": str(e)}), 500

# Optional: return latest data as JSON for AJAX if you want later
@app.route("/latest", methods=["GET"])
def latest():
    return jsonify(latest_sensor_data)

if __name__ == "__main__":
    # Run on LAN so ESP32 can hit it
    app.run(host="0.0.0.0", port=5000, debug=True)
