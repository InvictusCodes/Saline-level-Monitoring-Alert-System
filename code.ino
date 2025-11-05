/*
  Saline Level Monitoring System (Non-Blocking)
  - Monitors 3 IR sensors.
  - Blinks corresponding LED for a bed if saline is low.
  - Assumes IR sensor outputs HIGH when saline is absent (no reflection).
*/

// --- Pin Definitions ---
const int irSensorPins[] = {2, 3, 4}; // Sensors for Bed 1, 2, 3
const int ledPins[] = {8, 9, 10};     // LEDs for Bed 1, 2, 3
const int numBeds = 3;

// --- Blink Without Delay Timing ---
const long blinkInterval = 250; // Blink interval (milliseconds)
unsigned long previousMillis[numBeds] = {0, 0, 0};
int ledState[numBeds] = {LOW, LOW, LOW};
int previousSensorState[numBeds] = {LOW, LOW, LOW}; // Track sensor state to print alert only once

void setup() {
  Serial.begin(9600);
  Serial.println("Saline Monitoring System Initialized.");

  for (int i = 0; i < numBeds; i++) {
    pinMode(irSensorPins[i], INPUT);
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // Get the current time once per loop
  unsigned long currentMillis = millis();
  
  // Check all beds in every loop
  for (int i = 0; i < numBeds; i++) {
    checkSalineLevel(currentMillis, i);
  }
  // No delay() here, so the loop runs as fast as possible
}

/**
 * @brief Checks saline level for one bed and manages LED blinking without delay.
 * @param currentMillis The current time from millis().
 * @param bedIndex The index (0-2) of the bed to check.
 */
void checkSalineLevel(unsigned long currentMillis, int bedIndex) {
  // Read the state of the IR sensor
  int sensorState = digitalRead(irSensorPins[bedIndex]);

  // Check if the saline is absent (sensor output is HIGH)
  if (sensorState == HIGH) {
    
    // If the state just changed from LOW to HIGH, print the alert once.
    if (previousSensorState[bedIndex] == LOW) {
      Serial.print("Alert! Bed No. ");
      Serial.print(bedIndex + 1); // Add 1 for human-readable bed number
      Serial.println(" has low saline level!");
    }

    // Handle the non-blocking blink
    if (currentMillis - previousMillis[bedIndex] >= blinkInterval) {
      previousMillis[bedIndex] = currentMillis; // Save the last blink time
      ledState[bedIndex] = !ledState[bedIndex]; // Toggle the state
      digitalWrite(ledPins[bedIndex], ledState[bedIndex]);
    }
    
    previousSensorState[bedIndex] = HIGH; // Record the current state

  } else {
    // Saline level is normal (sensor is LOW)
    digitalWrite(ledPins[bedIndex], LOW); // Ensure the LED is off
    ledState[bedIndex] = LOW;             // Reset the blink state
    previousSensorState[bedIndex] = LOW;  // Record the current state
  }
}
