#include "ButtonDefinitions.h"
#include "LEDControl.h"
#include "ButtonLogic.h"
#include "MIDIOut.h"
#include "MessageConfigurations.h"
#include "PotentiometerControl.h"

#include "usb_rename.h" // https://github.com/mon/Arduino-USB-Rename

// Product name, Manufacturer, serial
// Any argument can be left off, or NULL to keep the original Arduino name
USBRename dummy = USBRename("OpenMIDIStomper", "Nerd Musician", "0002");

void setup() {
  Serial.begin(115200); // For debugging

  setupButtons();
  setupLEDs();
  initMessageArrays();  // Initialize message arrays with default values
  configureMessages();  // Set specific configurations for messages

  // Initialize the responsive pots
  for (int i = 0; i < N_POTS; i++) {
    responsivePot[i] = ResponsiveAnalogRead(POT_ARDUINO_PIN[i], true, snapMultiplier);
    responsivePot[i].setAnalogResolution(1023);  // sets the resolution
  }
}

void loop() {
  scanButtonMatrix();

  if (isAnyButtonPressed == false) {
    handleLEDs();
  }

  potentiometers();
  delay(10);
}
