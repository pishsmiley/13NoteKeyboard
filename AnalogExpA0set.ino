// Upload this to your Arduino to help set values for your analog inputs AKA expression pedals
// Set the analog pin to be read on the line with the stared comment
// So in the code it will be either A0 A1 A2 A3
// Stereo jack wired T5V RAnalog Pin
// Upload
// Open up Serial Monitor in tools
// Move the expression pedal and write down the  min and max values. They'll be enetered in the main program. The line is marked up.
// Do this with each expression pedal, they'll all have different travel. Mine are DIY ones made from Crybaby Wahs which go from 0 to 800 or so. You may need to tweak for a prefered response.

int potPin = A0;       // Analog pin A0*********
int potValue;          // Variable to store the potentiometer value
int minValue = 1023;   // Initialize to maximum possible value
int maxValue = 0;      // Initialize to minimum possible value

void setup() {
  Serial.begin(9600);  // Initialize serial communication at 9600 bits per second
}

void loop() {
  potValue = analogRead(potPin);  // Read the potentiometer value
  
  // Update the minimum and maximum values
  if (potValue < minValue) {
    minValue = potValue;
  }
  if (potValue > maxValue) {
    maxValue = potValue;
  }
  
  // Print the current, minimum, and maximum values to the Serial Monitor
  Serial.print("Current Value: ");
  Serial.print(potValue);
  Serial.print(" | Min Value: ");
  Serial.print(minValue);
  Serial.print(" | Max Value: ");
  Serial.println(maxValue);
  
  delay(100);  // Small delay to avoid flooding the Serial Monitor
}
