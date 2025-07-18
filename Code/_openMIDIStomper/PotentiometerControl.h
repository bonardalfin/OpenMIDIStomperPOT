#ifndef POTENTIOMETERCONTROL_H
#define POTENTIOMETERCONTROL_H

#include <MIDIUSB.h>                    // Add this to send MIDI
#include <ResponsiveAnalogRead.h>      // https://github.com/dxinteractive/ResponsiveAnalogRead

// Potentiometers
const int N_POTS = 1;                                 //* total numbers of pots (slide & rotary). Default = 2;
const int POT_ARDUINO_PIN[N_POTS] = { 10 };       //* pins of each pot connected straight to the Arduino. Default = { 10, A0}

// Responsive Analog Read
float snapMultiplier = 0.01;                       // (0.0 - 1.0) - Increase for faster, but less smooth reading
ResponsiveAnalogRead responsivePot[N_POTS] = {};  // creates an array for the responsive pots. It gets filled in the Setup.

// Potentiometer states
int potCState[N_POTS] = { 0 };        // Current state of the pot
int potPState[N_POTS] = { 0 };        // Previous state of the pot
int midiCState[N_POTS] = { 0 };       // Current state of the midi value
int midiPState[N_POTS] = { 0 };       // Previous state of the midi value
boolean potMoving = true;             // If the potentiometer is moving
unsigned long PTime[N_POTS] = { 0 };  // Previously stored time
unsigned long timer[N_POTS] = { 0 };  // Stores the time that has elapsed since the timer was reset

// Potentiometer configuration
const int TIMEOUT = 300;      //* Amount of time the potentiometer will be read after it exceeds the varThreshold
const int varThreshold = 10;  //* Threshold for the potentiometer signal variation

/////////////////////////////////////////////
// Potentiometer Functions

void potentiometers() {
  for (int i = 0; i < N_POTS; i++) {
    int reading = analogRead(POT_ARDUINO_PIN[i]);
    responsivePot[i].update(reading);
    potCState[i] = responsivePot[i].getValue();         // Use responsive analog read value
 midiCState[i] = map(potCState[i], 0, 1023, 0, 127);  // Maps the reading of the potCState to a value usable in MIDI
    int potVar = abs(potCState[i] - potPState[i]);      // Calculates the absolute value between the difference between the current and previous state of the pot

    if (potVar > varThreshold) {  // Opens the gate if the potentiometer variation is greater than the threshold
      PTime[i] = millis();        // Stores the previous time
    }

    timer[i] = millis() - PTime[i];  // Resets the timer
    potMoving = (timer[i] < TIMEOUT);

    if (potMoving && midiPState[i] != midiCState[i]) {

#ifdef DEBUG
      Serial.print("Pot: ");
      Serial.print(i);
      Serial.print(" ");
      Serial.println(midiCState[i]);
#endif

      // --- Send MIDI CC ---
      midiEventPacket_t event = {0x0B, 0xB0 | (1 - 1), i, midiCState[i]};
      MidiUSB.sendMIDI(event);
      MidiUSB.flush();
      // ----------------------

      potPState[i] = potCState[i];
      midiPState[i] = midiCState[i];
    }
  }
}

#endif
