#pragma once
#include <vector>
#include <stdint.h>
#include <map>
#include "hexBoardSystem/hexagon.h"

// Effective Nov 15, 2024, the portion of the code related to setting the key pins
// is moved to "hwKeys.h". This section now focuses on the grid object in a theoretical sense.

// grandparent structure for a mux/pin input
// a shorted (hardwired) pin is the minimal example
struct switch_t {
  unsigned atMux;
  unsigned atCol;
  //unsigned hwKey; // column x 2^muxPins + muxValue
  unsigned type;  // see enum in config.h
  switch_t(unsigned m, unsigned c, unsigned t) : atMux(m), atCol(c), type(t) {}
};

// parent structure for a physical button
// in the rare case a button on a mux/pin grid
// does NOT have an associated LED pixel,
// set pixel negative and the LED routine will
// skip this object. pixel values should still be
// unique among physical button switches.
// there is not a error check for this so be careful
enum {
  button_off = 0,
  button_press = 1,
  button_release = 2,
  button_hold = 3,
};
struct button_t : switch_t {
  hex_t coord; // physical location
  unsigned pixel; // associated pixel
  unsigned index; // location within its array

  int key_state; // down, up, press, release?
  double key_velocity; // key press velocity in ticks per millisecond
  double key_pressure; // percentage pressure from 0 to 1
  unsigned long long int timePressed; // store time that press occurred

  unsigned zero = 0;
  uint32_t LEDcodeAnim;     // calculate it once and store value, to make LED playback snappier 
  uint32_t LEDcodePlay;     // calculate it once and store value, to make LED playback snappier
  uint32_t LEDcodeRest;     // calculate it once and store value, to make LED playback snappier
  uint32_t LEDcodeOff;      // calculate it once and store value, to make LED playback snappier
  uint32_t LEDcodeDim;      // calculate it once and store value, to make LED playback snappier
  // base color
  // gradient rule
  int animate; // store value to track animations

  button_t(switch_t sw, hex_t x, unsigned p) : switch_t(sw.atMux, sw.atCol, sw.type), coord(x), pixel(p) {}
  button_t(unsigned m, unsigned c, unsigned t, hex_t x, unsigned p) : switch_t(m, c, t), coord(x), pixel(p) {}
};

// child structure for buttons that 
// play a musical note.
struct music_key_t : button_t {
/*
  int      stepsFromC;      // number of steps from C4 (semitones in 12EDO; microtones if >12EDO)
  float    centsFromAnchor;
  bool     inScale;         // 0 if it's not in the selected scale; 1 if it is
  unsigned     note;  // MIDI note or control parameter corresponding to this hex
  int16_t  bend;            // in microtonal mode, the pitch bend for this note needed to be tuned correctly
  float    pitch;
  unsigned     MIDIch;          // what MIDI channel this note is playing on
  unsigned     synthCh;         // what synth polyphony ch this is playing on
  float    frequency;     // what frequency to ring on the synther
*/
  double frequency;         // equivalent pitch in Hz
  uint8_t midiNote;         // nearest MIDI pitch, 0 to 128
  uint8_t midiBend;         // pitch bend for MPE purposes
  uint8_t midiCh;          // what channel (if not MPE mode)
  uint8_t midiTuningTable; // assigned MIDI note (if MTS mode)
  uint8_t midiChPlaying;          // what midi channel is there a note-on
  unsigned synthChPlaying;         // what synth channel is there a note-on
  unsigned scaleDegree;     // order in scale relative to equave
  bool inScale; // for scale-lock purposes
  music_key_t(button_t btn) : button_t(btn.atMux, btn.atCol, btn.type, btn.coord, btn.pixel) {}
};

// child structure for buttons that
// do any other command.
struct other_cmd_t : button_t {
  unsigned  cmd;  // control parameter corresponding to this hex
  // possibly more structure, pending
  other_cmd_t(button_t btn) : button_t(btn.atMux, btn.atCol, btn.type, btn.coord, btn.pixel) {}
};

// structure to collect all inputs from 
// the grid, and groups the switches by type.
struct switchboard_t {
  std::vector<music_key_t> keys;
  std::vector<other_cmd_t> commands;
  std::vector<switch_t> hardwired_switches;
//  maps to navigate the list of buttons
  //    index: location in button vector
  //    coord: physical hex location
  //    pixel: corresponding pixel number
  //    atMux: mux state to read this key
  //    atCol: column index of pin this key is on
  //  these are derived in the grid setup based on config.h constants.
  std::map<hex_t, int>   coord_to_pixel; // e.g. hex(0,-6) -> pixel 5
  std::map<int, bool>    pixel_is_cmd;  // e.g. pixel 5 -> false, pixel 80 -> true
  std::map<int, unsigned>    pixel_to_index; // e.g. pixel 5 -> 4 (key), pixel 80 -> 4 (cmd)
  music_key_t& key_at_pixel(const int pxl) {
    return keys[pixel_to_index.at(pxl)];
  }
  button_t& button_at_pixel(int pxl) {
    int ind = pixel_to_index.at(pxl);
    if (pixel_is_cmd.at(pxl)) {
      return commands[ind];
    } else {
      return keys[ind];
    }
  }
  button_t& button_at_coord(hex_t coord) {
    return button_at_pixel(coord_to_pixel.at(coord));
  }
  bool in_bounds(hex_t coord) {
    return (coord_to_pixel.find(coord) != coord_to_pixel.end());
  }
};

// THIS IS THE OBJECT WHERE ALL THE MAGIC HAPPENS
switchboard_t hexBoard;
const std::vector<unsigned> assignCmd = {0,20,40,60,80,100,120};

// when you run the setup routine, you should then have
// arrays of key definitions that are ready to be loaded
// with instructions and states.

void gridSystem_setup() {
  for (auto& eachRow : config_hexboard_layout) {
    switch_t tempSwitch(
      eachRow.multiplexer_value,
      eachRow.column_pin_index,
      eachRow.switch_type
    );
    switch (tempSwitch.type) {
      case unused_pin:
        break;
      case hardwired:
        hexBoard.hardwired_switches.emplace_back(tempSwitch);
        break;
      default:
        button_t tempButton(
          tempSwitch,
          {eachRow.hex_coordinate_x,eachRow.hex_coordinate_y},
          eachRow.associated_pixel
        );
        hexBoard.coord_to_pixel[tempButton.coord] = tempButton.pixel; // add to the lookup map
        // in v2 this will be part of the predefined layout definition
        // for now using a very dirty method:
        auto findCmd = std::find(assignCmd.begin(),assignCmd.end(),tempButton.pixel);
        bool isCmd = (findCmd != assignCmd.end());
        hexBoard.pixel_is_cmd[tempButton.pixel] = isCmd; // add to the lookup map
        if (isCmd) {
          other_cmd_t tempCmd(tempButton);
          tempCmd.cmd = findCmd - assignCmd.begin();
          hexBoard.pixel_to_index[tempCmd.pixel] = hexBoard.commands.size();
          hexBoard.commands.emplace_back(tempCmd);
        } else {
          music_key_t tempKey(tempButton);
          hexBoard.pixel_to_index[tempKey.pixel] = hexBoard.keys.size();
          hexBoard.keys.emplace_back(tempKey);
        }
        break;
    }
  }
}