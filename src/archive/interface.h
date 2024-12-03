#pragma once

// Effective Nov 15, 2024, the portion of the code related to setting the key and rotary pins
// is moved to "hwRotary.h" and "hwKeys.h". This section is solely for interpreting
// input presses.

/*
void setupHardware() {
  if (Hardware_Version == HARDWARE_V1_2) {
      midiD = MIDID_USB | MIDID_SER;
      audioD = AUDIO_PIEZO | AUDIO_AJACK;
      menuPageSynth.addMenuItem(menuItemAudioD, 2);
      globalBrightness = BRIGHT_DIM;
      setLEDcolorCodes();
      rotary.invertDirection(true);
  }
}
*/

/*
void interface_update_wheels() {  
  velWheel.setTargetValue();
  bool upd = velWheel.updateValue(runTime);
  if (upd) {
    sendToLog("vel became " + std::to_string(velWheel.curValue));
  }
  if (toggleWheel) {
    pbWheel.setTargetValue();
    upd = pbWheel.updateValue(runTime);
    if (upd) {
      sendMIDIpitchBendToCh1();
      updateSynthWithNewFreqs();
    }
  } else {
    modWheel.setTargetValue();
    upd = modWheel.updateValue(runTime);
    if (upd) {
      sendMIDImodulationToCh1();
    }
  }
}

void interface_interpret_rotary() {
  if (menu.readyForKey()) {
    if (rotary.getClickFromBuffer()) {
      menu.registerKeyPress(GEM_KEY_OK);
      screenTime = 0;
    }
    int getTurn = rotary.getTurnFromBuffer();
    if (getTurn != 0) {
      menu.registerKeyPress((getTurn > 0) ? GEM_KEY_UP : GEM_KEY_DOWN);
      screenTime = 0;
    }
  }
}
*/