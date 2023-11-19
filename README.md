// 13NoteKeyboard
// Arduino based 13 note keyboard, to make DIY Midi Organ Bass Pedals

//Simple project, to diy a simple midi keyboard. I did this using my limited Electronics knowledge and ChatGPT to write the Arduino Code.
//I have tried to make it so that it is adaptable to other peoples needs. The code is full of comments to help make adjustments.
//Right now the code is setup for my needs, so that it works with an Arturia MicroFreak. For example this has 3 banks of 128 patches,
//if your needs are different, look at the code and there are comments on what to adjust to fit your needs.
//I have done this project on an Arduino Mega 2560 Micro as it has plenty of pins for expansion.
//Dont want the feature, dont wire it up and itll be fine

//FEATURES
//13 notes C-c
//Octave Up,Octave down buttons
//Program Up,Program down buttons
//A button which toggles Sustain CC#64. Press for on, press again for off
//Secondary function button, which when held changes the Octave Up,Down buttons into Midi Channel Change buttons, and also leaves opportunity for expansion
//Midi Din Out

//All switches are none latching spdt buttons with one side connected to an Arduino Pin and the other ground

//When drilling for stomps, i lined mine up with keys as someone else has done the calculations for those. Dont bunch them up or theyll be useless. See pictures.
