// 13NoteKeyboard
// Arduino based 13 note keyboard, to make DIY Midi Organ Bass Pedals, but could be used for other things too

//Simple project, to diy a simple midi keyboard. I did this using my limited Electronics knowledge and ChatGPT to write the Arduino Code.

//To get going, look at all the files, this will help with wiring up and inspiring you how to mount stuff etc. There are both descriptions and pictures.

//I have tried to make it so that it is adaptable to other peoples needs. The code is full of comments to help make adjustments.
//Right now the code is setup for my needs, so that it works with an Arturia MicroFreak. For example this has 3 banks of 128 patches,
//if your needs are different, look at the code and there are comments on what to adjust to fit your needs.
//I have done this project on an Arduino Mega 2560 Micro as it has plenty of pins for expansion.
//Dont want the feature, dont wire it up and it will be fine
//You could use it to just make a box that sends Program changes or just Midi expression pedals. 
//I've tried to make it as easy as possible. The main code is full of comments.

//FEATURES
//13 notes C-c
//Octave Up,Octave down buttons
//Program Up,Program down buttons
//A button which toggles Sustain CC#64. Press for on, press again for off
//Secondary function button, which when held changes the Octave Up,Down buttons into Midi Channel Change buttons, and also leaves opportunity for expansion
//Midi Din Out
//Aftertouch Expression Input
//Expression 2/3/4 set as 23Cutoff/83Resonance/106EvelopeDecay but can be changed to any midi CC in the code


//All switches are none latching spdt buttons with one side connected to an Arduino Pin and the other ground

//Expression pedal inputs need to be set with the expression pedal you want to use. Using the AnalogExpA0set arduino code.
//This is to find your min and max values for each expression pedal, to enter into the main program, the lines are marked up.
// Tip is 5v/Ring is Analog pin/Sleeve is ground
// You could use these with a stomp switch of your choice in a box or something like a EHX Cntl Knob pedal
// Why is Expression 1 controlling Aftertouch? 
// My reasoning is that on the Microfreak and other modern synths you have the ability to set per patch what Aftertouch does. (That is in the synth not this controller)
// The other Expressions can be changed in the code just by changing the CC number
// I would advise powering down to plug in Expression pedal(s)

//When drilling for stomps, i lined mine up with keys as someone else has done the calculations for those. Dont bunch them up or theyll be useless. See pictures.

//Ive used ChatGPT to get this all working, if you dont know what you are doing like myself and want to add features, copy the code into ChatGPT and ask it to help you make changes and send you the fully edited code for you to test. This may take a few back and forths but youll eventually get there, describe results and faults, etc
