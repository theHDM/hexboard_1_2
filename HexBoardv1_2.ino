#include "src/utils.h"            // helpful utility functions
#include "src/timing.h"            // helpful utility functions
#include "src/diagnostics.h"
#include "src/hexBoardHardware.h" // API to work with hexBoard hardware
#include "src/hexBoardSystem.h" // API to work with hexBoard hardware

void read_key(button_t& b) {
  b.key_state    = pinGrid.read_keypress(b.atMux, b.atCol);
  b.key_velocity = pinGrid.read_velocity(b.atMux, b.atCol);
  b.key_pressure = pinGrid.read_pressure(b.atMux, b.atCol);
}

void debug_key(button_t& b) {
  sendToLog(
    " pxl " + std::to_string(b.pixel)
    + " hw " + std::to_string(b.atMux) + "M " + std::to_string(b.atCol) + "C "
    + " hex " + std::to_string(b.coord.x) + "," + std::to_string(b.coord.y)
    + " btn " + std::to_string(b.key_state)
    + " spd " + std::to_string(b.key_velocity)
    + " prs " + std::to_string(b.key_pressure)
  );
}

void process_note(music_key_t& n) {
  switch (n.key_state) {
    case button_press:
      //tryMIDInoteOn(h);
      //trySynthNoteOn(h);
      break;
    case button_release:
      //tryMIDInoteOff(h);
      //trySynthNoteOff(h);
      break; 
  }
}

void process_command(other_cmd_t& c) {
  switch (c.key_state) {
    case button_press:
      switch (c.cmd) {
        //case CMDB + 3:
          //toggleWheel = !toggleWheel;
        //  break;
        default:
          // the rest should all be taken care of within the wheelDef structure
          break;
      }
    default: // inactive
      break;
  }
}

void process_all_keys() {
  // only do this if the pingrid object is free
  if (!(pinGrid.is_background_process_complete())) return;
  // read in the new pin state completely first
  for (auto& h : hexBoard.keys)     read_key(h);
  for (auto& h : hexBoard.commands) read_key(h);
  // then release pingrid object
  pinGrid.resume_background_process();
  // if you are in play mode
  for (auto& h : hexBoard.keys) {
    debug_key(h); // if needed
    process_note(h);
  }
  for (auto& h : hexBoard.commands) {
    debug_key(h); // if needed
    process_command(h);
  }
  // otherwise make the keys do something else
}

//  a global variable used to control the timing of setup functions between cores
int setup_phase = 0;

void setup() {
  delay(2000);
  //#if (defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040))
  //  Manual begin() is required on core without built-in support for TinyUSB 
  //  such as mbed rp2040
  //TinyUSB_Device_Init(0);  
  //#endif
  //Serial.print("survived step 1");
  //Serial.begin(115200);     //  Set serial to make uploads work without bootsel button
  //OLED_setup();               //  Start the OLED screen, in case you want a splash screen?
  //presets_littleFS_setup();   //  Set up the littleFS file system first (to pull stored user settings in v2) 
  hardware_setup();           //  set up the keyboard, rotary, and audio functions based on config constants.
    setup_phase = 1;        //  change the setup flag to let the other core know to start the background processes
  //synth_reset();              //  make sure the synth is reset so no notes are running
  //MIDI_setup();               //  Set up the USB (Serial, pin 0) and MIDI-out (Serial1, pin 1) as MIDI objects
  //gridSystem_setup();         //  Set up the hex grid object, and set the pins that will read the button states 
  //applyLayout(); // see V1.assignment.h. Based on the default layout, populate grid with notes and colors
  //LED_setup();                //  Once the grid is defined, start the LEDs
  //menu_setup();               //  Set up the menu last.
  //for (byte i = 0; i < 5 && !TinyUSBDevice.mounted(); i++) {
  //  delay(1);                //  wait until device mounted, maybe 
  //}
  //sendToLog("survived");
}


void loop() { // need to add soft loop timing.
  //timing_measure_lap();           //  get time in uS at the start of the loop, measure loop duration
  //OLED_screenSaver();           //  every 1 second. reduces wear-and-tear on OLED panel  
  process_all_keys();             //  every loop. interpret button press actions, play MIDI / synth notes
  //interface_update_wheels();    //  v1.0 firmware only. deal with the pitch/mod wheel
  //synth_arpeggiate();           //  every X millis based on user input. arpeggiate if synth mode allows it
  //animate_calculate_pixels();   //  every 17 or 33 millis, calculate the next frame of responsive animations
  //LED_update_pixels();          //  every 17 or 33 millis, send updated pixel data to LEDs
  //interface_interpret_rotary(); //  every loop. interpret rotary knob presses, send to menu object, refresh OLED
  delay(2000);
  //sendToLog(txt(lapTime - 2000000));
}

void setup1() {
  while (setup_phase < 1) {}    //  wait until the hardware objects have been set up in the other core
  hardware_start_background_process();   //  begin the background loops on this core
}
void loop1() {
  //  dedicate this core to computing the synth audio and running the background processes.
  if (audioOut.roomToWrite()) {
    //uint8_t temp = ((getTheCurrentTime() >> 12) & 0b00010000);
    //audioOut.write(temp /* synth.next_sample() */ );   //  write synth samples tnto the audio buffer.
  }
  //  and respond to all hardware task manager interrupts when they are called.
}
