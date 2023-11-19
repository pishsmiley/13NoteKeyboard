//# 13NoteKeyboard
//Arduino based 13 note keyboard, to make DIY Midi Organ Bass Pedals

//Simple project, to diy a simple midi keyboard. I did this using my limited Electronics knowledge and ChatGPT to write the Arduino Code.
//I have tried to make it so that it is adaptable to other peoples needs. The code is full of comments to help make adjustments.
//Right now the code is setup for my needs, so that it works with an Arturia MicroFreak. For example this has 3 banks of 128 patches,
//if your needs are different, look at the code and there are comments on what to adjust to fit your needs.
//I have done this project on an Arduino Mega 2560 Micro as it has plenty of pins for expansion.
//Dont want the feature, dont wire it up and itll be fine

//FEATURES
//13 notes C-c
//Octave Up/Octave down buttons
//Program Up/Program down buttons
//A button which toggles Sustain CC#64. (Press for on, press again for off)
//Secondary function button, which when held changes the Octave Up/Down buttons into Midi Channel Change buttons, and also leaves opportunity for expansion
//Midi Din Out

//All switches are none latching spdt buttons with one side connected to an Arduino Pin and the other ground

#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// Pin assignments
const int incBtnPin = 7;               // Button for increasing the program change value
const int decBtnPin = 6;               // Button for decreasing the program change value
const int cc64BtnPin = 8;              // Button for toggling Control Change 64 (Sustain) on/off
const int numBanks = 3;                // Number of MIDI Patch Banks
const int maxProgramsPerBank = 128;    // Maximum number of programs per bank
const int maxPrograms = numBanks * maxProgramsPerBank;  // Total maximum number of programs

// MIDI settings
int midiChannel = 1;                    // Set MIDI channel for both program change and note buttons
int currentBank = 1;                    // Initial MIDI Patch Bank
int currentProgram = 0;                 // Initial MIDI Program
int currentOctave = 0;                  // Initial Octave on Power Up

// Button state tracking
bool cc64ButtonState = false;           // Track the state of the CC64 button
bool secondaryFunctionEnabled = false;  // Declare the secondary function state

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
