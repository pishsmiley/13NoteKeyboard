#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// Pin assignments
const int incBtnPin = 7;               // Button for increasing the program change value
const int decBtnPin = 6;               // Button for decreasing the program change value
const int cc64BtnPin = 8;              // Button for toggling Control Change 64 (Sustain) on/off
const int aftertouchBtnPin = 9;        // Button to toggle aftertouch transmission on/off
const int potPin = A0;                 // Potentiometer for aftertouch value
const int expression2BtnPin = 10;      // Button to toggle Expression2 on/off
const int expression2PotPin = A1;      // Potentiometer for Expression2 value
const int expression3BtnPin = 11;      // Button to toggle Expression3 on/off
const int expression3PotPin = A2;      // Potentiometer for Expression3 value
const int expression4BtnPin = 12;      // Button to toggle Expression4 on/off
const int expression4PotPin = A3;      // Potentiometer for Expression4 value
const int numBanks = 3;                // How many MIDI Patch Banks does the synth have?
const int maxProgramsPerBank = 128;    // Maximum number of programs per bank
const int maxPrograms = numBanks * maxProgramsPerBank;  // Total maximum number of programs

// MIDI settings
int midiChannel = 1;                   // Set MIDI channel for both program change and note buttons
int currentBank = 3;                   // Initial MIDI Patch Bank
int currentProgram = 63;               // Initial MIDI Program (Bank 3, 65 is first User patch on Microbrute)
int currentOctave = 0;                 // Initial Octave on Power Up
int expression2MidiChannel = 1;        // Set the MIDI channel for Expression2
int expression3MidiChannel = 1;        // Set the MIDI channel for Expression3
int expression4MidiChannel = 1;        // Set the MIDI channel for Expression4

// Button state tracking
bool cc64ButtonState = false;          // Track the state of the CC64 button (Sustain)
bool secondaryFunctionEnabled = false; // Declare the secondary function state
bool aftertouchEnabled = false;        // State of aftertouch transmission, change to true to enable on startup
bool aftertouchButtonPressed = false;  // Track aftertouch button press state
bool expression2Enabled = false;       // State of Expression2 transmission, change to true to enable on startup
bool expression2ButtonPressed = false; // Track Expression2 button press state
bool expression3Enabled = false;       // State of Expression3 transmission, change to true to enable on startup
bool expression3ButtonPressed = false; // Track Expression3 button press state
bool expression4Enabled = false;       // State of Expression4 transmission, change to true to enable on startup
bool expression4ButtonPressed = false; // Track Expression4 button press state

// Button assignments
byte Switch0 = 41;
byte Switch1 = 42;
byte Switch2 = 43;
byte Switch3 = 44;
byte Switch4 = 45;
byte Switch5 = 46;
byte Switch6 = 47;
byte Switch7 = 48;
byte Switch8 = 49;
byte Switch9 = 50;
byte Switch10 = 51;
byte Switch11 = 52;
byte Switch12 = 53;
byte MinusOctaveButton = 4;
byte PlusOctaveButton = 5;
byte SecondaryFunctionButton = 2;      // Button to enable the secondary function
byte x;
byte LedPin = 13;
byte note;
byte velocity = 100;
byte currentSwitch = 0;
int startNote = 36;

byte switches[13] = {Switch0, Switch1, Switch2, Switch3, Switch4, Switch5, Switch6, Switch7, Switch8, Switch9, Switch10, Switch11, Switch12};
byte switchState[13] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
byte lastSwitchState[13] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
bool minusButtonPressed = false;
bool plusButtonPressed = false;
bool incButtonPressed = false;
bool decButtonPressed = false;
bool cc64ButtonPressed = false;
unsigned long lastDebounceTime[2] = {0, 0};
unsigned long debounceDelay = 50;      // Debounce delay for the CC64 Sustain button
unsigned long debounceDelayOctave = 140;  // Debounce delay for the octave buttons
unsigned long debounceDelayCC64 = 140;   // Debounce delay for the CC64 Sustain button

void setup() {
  MIDI.begin(MIDI_CHANNEL_OFF);
  pinMode(incBtnPin, INPUT_PULLUP);
  pinMode(decBtnPin, INPUT_PULLUP);
  pinMode(cc64BtnPin, INPUT_PULLUP);   // Set Control Change 64 button as input
  pinMode(aftertouchBtnPin, INPUT_PULLUP); // Set aftertouch button as input
  pinMode(expression2BtnPin, INPUT_PULLUP); // Set Expression2 button as input
  pinMode(expression3BtnPin, INPUT_PULLUP); // Set Expression3 button as input
  pinMode(expression4BtnPin, INPUT_PULLUP); // Set Expression4 button as input
  pinMode(Switch0, INPUT_PULLUP);
  pinMode(Switch1, INPUT_PULLUP);
  pinMode(Switch2, INPUT_PULLUP);
  pinMode(Switch3, INPUT_PULLUP);
  pinMode(Switch4, INPUT_PULLUP);
  pinMode(Switch5, INPUT_PULLUP);
  pinMode(Switch6, INPUT_PULLUP);
  pinMode(Switch7, INPUT_PULLUP);
  pinMode(Switch8, INPUT_PULLUP);
  pinMode(Switch9, INPUT_PULLUP);
  pinMode(Switch10, INPUT_PULLUP);
  pinMode(Switch11, INPUT_PULLUP);
  pinMode(Switch12, INPUT_PULLUP);
  pinMode(MinusOctaveButton, INPUT_PULLUP);
  pinMode(PlusOctaveButton, INPUT_PULLUP);
  pinMode(SecondaryFunctionButton, INPUT_PULLUP);  // Set Secondary Function Button as input
  pinMode(LedPin, OUTPUT);
  for (x = 1; x <= 4; x++) {
    digitalWrite(LedPin, HIGH);
    delay(300);
    digitalWrite(LedPin, LOW);
    delay(300);
  }
  Serial.begin(31250);
  Serial.flush();
}

void loop() {
  // Check if the Secondary Function Button is pressed
  if (digitalRead(SecondaryFunctionButton) == LOW) {
    // Enable secondary function (changing MIDI channel with octave buttons)
    cc64ButtonState = false;  // Ensure CC64 is off when secondary function is enabled
    secondaryFunctionEnabled = true;  // Enable the secondary function
  } else {
    secondaryFunctionEnabled = false;  // Disable the secondary function
  }

  // Handle the CC64 button press
  if (digitalRead(cc64BtnPin) == LOW && !cc64ButtonPressed) {
    cc64ButtonPressed = true;
    // Toggle the CC64 state
    cc64ButtonState = !cc64ButtonState;
    // Send Control Change 64 (Sustain) message
    MIDI.sendControlChange(64, cc64ButtonState ? 127 : 0, midiChannel);
    delay(debounceDelayCC64);  // Debounce delay for CC64 button
  } else if (digitalRead(cc64BtnPin) == HIGH) {
    cc64ButtonPressed = false;
  }

  // Handle the aftertouch button press
  if (digitalRead(aftertouchBtnPin) == LOW && !aftertouchButtonPressed) {
    aftertouchButtonPressed = true;
    aftertouchEnabled = !aftertouchEnabled;  // Toggle aftertouch state
    delay(debounceDelay);  // Debounce delay for aftertouch button
  } else if (digitalRead(aftertouchBtnPin) == HIGH) {
    aftertouchButtonPressed = false;
  }

  // Handle the Expression2 button press
  if (digitalRead(expression2BtnPin) == LOW && !expression2ButtonPressed) {
    expression2ButtonPressed = true;
    expression2Enabled = !expression2Enabled;  // Toggle Expression2 state
    delay(debounceDelay);  // Debounce delay for Expression2 button
  } else if (digitalRead(expression2BtnPin) == HIGH) {
    expression2ButtonPressed = false;
  }

  // Handle the Expression3 button press
  if (digitalRead(expression3BtnPin) == LOW && !expression3ButtonPressed) {
    expression3ButtonPressed = true;
    expression3Enabled = !expression3Enabled;  // Toggle Expression3 state
    delay(debounceDelay);  // Debounce delay for Expression3 button
  } else if (digitalRead(expression3BtnPin) == HIGH) {
    expression3ButtonPressed = false;
  }

  // Handle the Expression4 button press
  if (digitalRead(expression4BtnPin) == LOW && !expression4ButtonPressed) {
    expression4ButtonPressed = true;
    expression4Enabled = !expression4Enabled;  // Toggle Expression4 state
    delay(debounceDelay);  // Debounce delay for Expression4 button
  } else if (digitalRead(expression4BtnPin) == HIGH) {
    expression4ButtonPressed = false;
  }

  // Read the potentiometer value and send aftertouch message if enabled
  if (aftertouchEnabled) {
    int potValue = analogRead(potPin);
    int aftertouchValue = map(potValue, 0, 884, 0, 127); // 0,884 are the analog pot range, measure pot with AnalogExpA0set to set range up to 1023. 127,0 is Aftertouch range 0,127 will reverse it.
    MIDI.sendAfterTouch(aftertouchValue, midiChannel);
  }

  // Read the potentiometer value and send Expression2 message if enabled
  if (expression2Enabled) {
    int potValue = analogRead(expression2PotPin);
    int expression2Value = map(potValue, 0, 1023, 0, 127); // 0,884 are the analog pot range, measure pot with AnalogExpA0set to set range up to 1023. 127,0 is Aftertouch range 0,127 will reverse it.
    MIDI.sendControlChange(23, expression2Value, expression2MidiChannel);  // Change 23 to desired CC number
  }

  // Read the potentiometer value and send Expression3 message if enabled
  if (expression3Enabled) {
    int potValue = analogRead(expression3PotPin);
    int expression3Value = map(potValue, 0, 1023, 0, 127); // 0,884 are the analog pot range, measure pot with AnalogExpA0set to set range up to 1023. 127,0 is Aftertouch range 0,127 will reverse it.
    MIDI.sendControlChange(83, expression3Value, expression3MidiChannel);  // Change 83 to desired CC number
  }

  // Read the potentiometer value and send Expression4 message if enabled
  if (expression4Enabled) {
    int potValue = analogRead(expression4PotPin);
    int expression4Value = map(potValue, 0, 1023, 0, 127); // 0,884 are the analog pot range, measure pot with AnalogExpA0set to set range up to 1023. 127,0 is Aftertouch range 0,127 will reverse it.
    MIDI.sendControlChange(106, expression4Value, expression4MidiChannel);  // Change 106 to desired CC number
  }

  // Handle note buttons
  for (int n = 0; n < 13; n++) {
    currentSwitch = digitalRead(switches[n]);
    if (currentSwitch == LOW and lastSwitchState[n] == HIGH) {
      note = n;
      // Send note-on message with the adjusted MIDI channel or octave
      if (secondaryFunctionEnabled) {
        MIDI.sendNoteOn(note + startNote, velocity, adjustMIDIChannel(midiChannel));
      } else {
        MIDI.sendNoteOn(note + startNote + (currentOctave * 12), velocity, midiChannel);
      }
    }
    if (currentSwitch == HIGH and lastSwitchState[n] == LOW) {
      note = n;
      // Send note-off message with the adjusted MIDI channel or octave
      if (secondaryFunctionEnabled) {
        MIDI.sendNoteOff(note + startNote, 0, adjustMIDIChannel(midiChannel));
      } else {
        MIDI.sendNoteOff(note + startNote + (currentOctave * 12), 0, midiChannel);
      }
    }
    lastSwitchState[n] = currentSwitch;
  }

  // Handle Minus Octave Button
  if (digitalRead(MinusOctaveButton) == LOW and !minusButtonPressed) {
    if (secondaryFunctionEnabled) {
      midiChannel = adjustMIDIChannel(midiChannel - 1);
    } else {
      if (currentOctave > -3) {  // Set the lower bound for the octave
        currentOctave--;
      }
    }
    minusButtonPressed = true;
    delay(debounceDelayOctave);  // Debounce delay for MinusOctaveButton
  }
  if (digitalRead(MinusOctaveButton) == HIGH) {
    minusButtonPressed = false;
  }

  // Handle Plus Octave Button
  if (digitalRead(PlusOctaveButton) == LOW and !plusButtonPressed) {
    if (secondaryFunctionEnabled) {
      midiChannel = adjustMIDIChannel(midiChannel + 1);
    } else {
      if (currentOctave < 7) {  // Set the upper bound for the octave
        currentOctave++;
      }
    }
    plusButtonPressed = true;
    delay(debounceDelayOctave);  // Debounce delay for PlusOctaveButton
  }
  if (digitalRead(PlusOctaveButton) == HIGH) {
    plusButtonPressed = false;
  }

  // Handle program change buttons
  int incButtonState = digitalRead(incBtnPin);
  int decButtonState = digitalRead(decBtnPin);

  if (incButtonState == LOW and !incButtonPressed) {
    currentProgram++;
    if (currentProgram >= maxPrograms) {
      currentProgram = 0;
      currentBank++;
      if (currentBank > numBanks) {
        currentBank = 1;
      }
    }
    MIDI.sendControlChange(0, currentBank - 1, midiChannel);
    MIDI.sendProgramChange(currentProgram, midiChannel);
    incButtonPressed = true;
    delay(debounceDelay);
  }
  if (incButtonState == HIGH) {
    incButtonPressed = false;
  }

  if (decButtonState == LOW and !decButtonPressed) {
    currentProgram--;
    if (currentProgram < 0) {
      currentProgram = maxPrograms - 1;
      currentBank--;
      if (currentBank < 1) {
        currentBank = numBanks;
      }
    }
    MIDI.sendControlChange(0, currentBank - 1, midiChannel);
    MIDI.sendProgramChange(currentProgram, midiChannel);
    decButtonPressed = true;
    delay(debounceDelay);
  }
  if (decButtonState == HIGH) {
    decButtonPressed = false;
  }
}

// Function to adjust MIDI channel and ensure it wraps around
int adjustMIDIChannel(int channel) {
  if (channel < 1) {
    return 16;
  } else if (channel > 16) {
    return 1;
  } else {
    return channel;
  }
}
