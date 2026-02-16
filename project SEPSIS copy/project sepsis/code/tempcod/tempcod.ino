#include <Servo.h>

// Define servo objects
Servo base, arm1, arm2, picker;

void setup() {
  // Attach servos to Arduino pins
  base.attach(3);    // Base connected to pin D3
  arm1.attach(5);    // Arm 1 connected to pin D5
  arm2.attach(6);    // Arm 2 connected to pin D6
  picker.attach(9);  // Picker connected to pin D9

  // Initialize all servos to their starting positions
  base.write(90);    // Base neutral
  arm1.write(90);    // Arm 1 neutral
  arm2.write(90);    // Arm 2 neutral
  picker.write(90);  // Picker neutral
  delay(3000);       // Allow time for setup
}

void loop() {
  // Sequence of movements

  // Step 1: Base - 10 degrees
  base.write(10);
  delay(3000);

  // Step 2: Arm1 - 150 degrees
  arm1.write(100);
  delay(3000);

  // Step 3: Arm2 - 10 degrees
  arm2.write(10);
  delay(3000);

  // Step 4: Picker - 100 degrees
  picker.write(100);
  delay(3000);

  // Step 5: Arm2 - 40 degrees
  arm2.write(50);
  delay(3000);

  // Step 6: Arm1 - 50 degrees
  arm1.write(25);
  delay(3000);

  // Step 7: Picker - 10 degrees
  picker.write(10);
  delay(3000);

  // Step 8: Arm2 - 10 degrees
  arm2.write(10);
  delay(3000);

  // Step 9: Arm1 - 150 degrees
  arm1.write(100);
  delay(3000);

  // Step 10: Base - 150 degrees
  base.write(150);
  delay(3000);

  // Step 11: Arm1 - 50 degrees
  arm1.write(25);
  delay(3000);

  // Step 12: Arm2 - 40 degrees
  arm2.write(50);
  delay(3000);


  // Step 13: Picker - 100 degrees
  picker.write(100);
  delay(3000);

  // Optional: Reset to neutral before looping
  base.write(90);
  arm1.write(90);
  arm2.write(90);
  picker.write(90);
  delay(3000);
}
