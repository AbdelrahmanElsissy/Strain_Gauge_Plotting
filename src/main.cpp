#include <Arduino.h>
#include "HX711.h"
#include "materialconstants.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 32;
const int LOADCELL_SCK_PIN = 15;

HX711 scale;
const int numReadings = 120; // Number of readings in one minute (assuming 2 readings per second)
float readings[numReadings];
int readingIndex = 0;
float maxValue = 0;
float thirtyPercentValue = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 Demo");

  Serial.println("Initializing the scale");

  // Initialize library with data output pin, clock input pin and gain factor.
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read()); // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20)); // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5)); // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1); // print the average of 5 readings from the ADC minus tare weight (not set) divided
                                         // by the SCALE parameter (not set yet)

  scale.set_scale(YZC516CFAKTOR); // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();                   // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read()); // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20)); // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5)); // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1); // print the average of 5 readings from the ADC minus tare weight, divided
                                         // by the SCALE parameter set with set_scale

  Serial.println("Readings:");
}

void loop() {
  unsigned long startTime = millis();
  while (millis() - startTime < 60000) { // Collect data for 1 minute
    if (readingIndex < numReadings) {
      float currentReading = scale.get_units();
      readings[readingIndex] = currentReading;
      Serial.print(currentReading, 1); // Print current reading for the Serial Plotter
      Serial.print("\t"); // Separate columns with a tab
      Serial.print(maxValue);
      Serial.print("\t"); // Separate columns with a tab
      Serial.print(thirtyPercentValue);
      Serial.println();
      readingIndex++;
    }
    scale.power_down(); // put the ADC in sleep mode
    delay(500);
    scale.power_up();
  }

  // Find the maximum value
  maxValue = readings[0];
  for (int i = 1; i < numReadings; i++) {
    if (readings[i] > maxValue) {
      maxValue = readings[i];
    }
  }

  // Calculate 30% of the maximum value
  thirtyPercentValue = 0.3 * maxValue;

  // Reset reading index for the next loop
  readingIndex = 0;

  // Continuously plot the max and 30% value even after the initial 1-minute period
  while (true) {
    // Print the most recent reading, max value, and 30% value
    float currentReading = scale.get_units();
    Serial.print(currentReading, 1);
    Serial.print("\t"); // Separate columns with a tab
    Serial.print(maxValue);
    Serial.print("\t"); // Separate columns with a tab
    Serial.print(thirtyPercentValue);
    Serial.println();
    
    scale.power_down(); // put the ADC in sleep mode
    delay(500);
    scale.power_up();
  }
}
