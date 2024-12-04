#pragma once
#include <stdint.h>
#include "hexBoardLayout/buttonGrid.h"
/*
  This section of the code handles all
  things related to MIDI messages.
*/
#include <Adafruit_TinyUSB.h>   // library of code to get the USB port working
Adafruit_USBD_MIDI usb_midi;
#include <MIDI.h>               // library of code to send and receive MIDI messages
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, UMIDI);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, SMIDI);

enum {
  MIDID_NONE = 0,
  MIDID_USB = 1,
  MIDID_SER = 2,
  MIDID_BOTH = 3
};
unsigned midiD = MIDID_USB | MIDID_SER; // make part of settings
#define ALL_MIDI_DEVICES(command, args...) if (midiD & MIDID_USB) UMIDI.command(args); if (midiD & MIDID_SER) SMIDI.command(args);

enum {
  NAIVE_MIDI_mode = 0,
  MTS_mode = 1,
  MPE_mode = 2
};
unsigned MIDI_mode = MPE_mode; // make part of settings

unsigned MPE_pitch_bend_range = 48; // make part of settings
std::deque<uint8_t> MPE_channel_queue;

uint8_t note_to_send(music_key_t h) {
  if (MIDI_mode == MTS_mode) return h.midiTuningTable;
  return h.midiNote;
}

void midi_note_on(music_key_t& h) {
  // determine channel
  if (MIDI_mode == MPE_mode) {
    if (MPE_channel_queue.empty()) return;
    h.midiChPlaying = MPE_channel_queue.front();
    ALL_MIDI_DEVICES(sendPitchBend, h.midiBend, h.midiChPlaying);    
    MPE_channel_queue.pop_front();
  } else {
    h.midiChPlaying = h.midiCh;
  } 
  ALL_MIDI_DEVICES(sendNoteOn, note_to_send(h), 64, h.midiChPlaying);
}

void midi_note_off(music_key_t& h) {
  ALL_MIDI_DEVICES(sendNoteOff, note_to_send(h), 64, h.midiChPlaying);
  if (MIDI_mode == MPE_mode) {  
    MPE_channel_queue.push_back(h.midiChPlaying);
  }
  h.midiChPlaying = 0;
}

void midi_set_pb_range(uint8_t c, uint8_t semitones) {
  ALL_MIDI_DEVICES(beginRpn, 0, c);
  ALL_MIDI_DEVICES(sendRpnValue, semitones << 7, c);
  ALL_MIDI_DEVICES(endRpn, c);
}

void midi_set_MPE_zone(uint8_t masterCh, uint8_t sizeOfZone) {
  ALL_MIDI_DEVICES(beginRpn, 6, masterCh);
  ALL_MIDI_DEVICES(sendRpnValue, sizeOfZone << 7, masterCh);
  ALL_MIDI_DEVICES(endRpn, masterCh);
}

void midi_reset_mode() {
  for (auto& h : hexBoard.keys) {
    midi_note_off(h);
  }
  if (MIDI_mode == MPE_mode) {
    midi_set_MPE_zone(1, 15);   // make a variable
    MPE_channel_queue.clear();
    for (uint8_t i = 2; i <= 16; i++) MPE_channel_queue.push_back(i);
  } else {
    midi_set_MPE_zone(1, 0);
  }
  if (MIDI_mode == MTS_mode) {
    // if MTS send bulk tuning dump based on layout
  } else {
    // else send tuning dump that resets
  }
  // recalculate MIDI values for all notes
}
