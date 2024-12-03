#pragma once
#include <vector>

// wiring switch states for each pin
enum {
  unused_pin = 0,  // ignored, even if it does something
  hardwired = 1,   // read only once at startup 
  hex_button = 2, // monitor during operation
};
// placeholder value
enum {
  N_A = -128
};

struct key_identification {
  unsigned column_pin_index;  // 0..x, x = # of colPins
  unsigned multiplexer_value; // 0..(2^n)-1, n = # of muxPins
  unsigned switch_type = unused_pin;   // use enum above
  int hex_coordinate_x = N_A;   // physical location on board
  int hex_coordinate_y = N_A;   // physical location on board
  int associated_pixel = N_A;  // as counted on neoPixel strip 
};

// Hardware pin constants
// If you rewire the HexBoard then change these pin values
const std::vector<unsigned> muxPins = {4,5,2,3}; // 8bit 4bit 2bit 1bit
const std::vector<unsigned> colPins = {6,7,8,9,10,11,12,13,14,15};
// 1 if analog (firmware 2.0), 0 if digital
const std::vector<bool> analogPins  = {0,0,0,0,0, 0, 0, 0, 0, 0};
const int default_analog_calibration_up = 480;
const int default_analog_calibration_down = 280;

// and this big data table, too, while you're at it:
const std::vector<key_identification> config_hexboard_layout = {
  //col mux switch type  x   y  pxl
  { 0,  0, hex_button, -10,  0,   0 },
  { 0,  1, hex_button,  -9, -5,  10 },
  { 0,  2, hex_button, -11,  1,  20 },
  { 0,  3, hex_button,  -9, -3,  30 },
  { 0,  4, hex_button, -10,  2,  40 },
  { 0,  5, hex_button,  -9, -1,  50 },
  { 0,  6, hex_button, -11,  3,  60 },
  { 0,  7, hex_button,  -9,  1,  70 },
  { 0,  8, hex_button, -10,  4,  80 },
  { 0,  9, hex_button,  -9,  3,  90 },
  { 0, 10, hex_button, -11,  5, 100 },
  { 0, 11, hex_button,  -9,  5, 110 },
  { 0, 12, hex_button, -10,  6, 120 },
  { 0, 13, hex_button,  -9,  7, 130 },
  { 0, 14, hardwired,  N_A,N_A, N_A }, // off = FW 1.0/1.1, on = FW 1.2
  { 0, 15, unused_pin, N_A,N_A, N_A },
  //col mux switch type  x   y  pxl
  { 1,  0, hex_button,  -8, -6,   1 },
  { 1,  1, hex_button,  -7, -5,  11 },
  { 1,  2, hex_button,  -8, -4,  21 },
  { 1,  3, hex_button,  -7, -3,  31 },
  { 1,  4, hex_button,  -8, -2,  41 },
  { 1,  5, hex_button,  -7, -1,  51 },
  { 1,  6, hex_button,  -8,  0,  61 },
  { 1,  7, hex_button,  -7,  1,  71 },
  { 1,  8, hex_button,  -8,  2,  81 },
  { 1,  9, hex_button,  -7,  3,  91 },
  { 1, 10, hex_button,  -8,  4, 101 },
  { 1, 11, hex_button,  -7,  5, 111 },
  { 1, 12, hex_button,  -8,  6, 121 },
  { 1, 13, hex_button,  -7,  7, 131 },
  { 1, 14, unused_pin, N_A,N_A, N_A },
  { 1, 15, unused_pin, N_A,N_A, N_A },
  //col mux switch type  x   y  pxl
  { 2,  0, hex_button,  -6, -6,   2 },
  { 2,  1, hex_button,  -5, -5,  12 },
  { 2,  2, hex_button,  -6, -4,  22 },
  { 2,  3, hex_button,  -5, -3,  32 },
  { 2,  4, hex_button,  -6, -2,  42 },
  { 2,  5, hex_button,  -5, -1,  52 },
  { 2,  6, hex_button,  -6,  0,  62 },
  { 2,  7, hex_button,  -5,  1,  72 },
  { 2,  8, hex_button,  -6,  2,  82 },
  { 2,  9, hex_button,  -5,  3,  92 },
  { 2, 10, hex_button,  -6,  4, 102 },
  { 2, 11, hex_button,  -5,  5, 112 },
  { 2, 12, hex_button,  -6,  6, 122 },
  { 2, 13, hex_button,  -5,  7, 132 },
  { 2, 14, unused_pin, N_A,N_A, N_A },
  { 2, 15, unused_pin, N_A,N_A, N_A },
  //col mux switch type  x   y  pxl
  { 3,  0, hex_button,  -4, -6,   3 },
  { 3,  1, hex_button,  -3, -5,  13 },
  { 3,  2, hex_button,  -4, -4,  23 },
  { 3,  3, hex_button,  -3, -3,  33 },
  { 3,  4, hex_button,  -4, -2,  43 },
  { 3,  5, hex_button,  -3, -1,  53 },
  { 3,  6, hex_button,  -4,  0,  63 },
  { 3,  7, hex_button,  -3,  1,  73 },
  { 3,  8, hex_button,  -4,  2,  83 },
  { 3,  9, hex_button,  -3,  3,  93 },
  { 3, 10, hex_button,  -4,  4, 103 },
  { 3, 11, hex_button,  -3,  5, 113 },
  { 3, 12, hex_button,  -4,  6, 123 },
  { 3, 13, hex_button,  -3,  7, 133 },
  { 3, 14, unused_pin, N_A,N_A, N_A },
  { 3, 15, unused_pin, N_A,N_A, N_A },
  //col mux switch type  x   y  pxl
  { 4,  0, hex_button,  -2, -6,   4 },
  { 4,  1, hex_button,  -1, -5,  14 },
  { 4,  2, hex_button,  -2, -4,  24 },
  { 4,  3, hex_button,  -1, -3,  34 },
  { 4,  4, hex_button,  -2, -2,  44 },
  { 4,  5, hex_button,  -1, -1,  54 },
  { 4,  6, hex_button,  -2,  0,  64 },
  { 4,  7, hex_button,  -1,  1,  74 },
  { 4,  8, hex_button,  -2,  2,  84 },
  { 4,  9, hex_button,  -1,  3,  94 },
  { 4, 10, hex_button,  -2,  4, 104 },
  { 4, 11, hex_button,  -1,  5, 114 },
  { 4, 12, hex_button,  -2,  6, 124 },
  { 4, 13, hex_button,  -1,  7, 134 },
  { 4, 14, unused_pin, N_A,N_A, N_A },
  { 4, 15, unused_pin, N_A,N_A, N_A },
  //col mux switch type  x   y  pxl
  { 5,  0, hex_button,   0, -6,   5 },
  { 5,  1, hex_button,   1, -5,  15 },
  { 5,  2, hex_button,   0, -4,  25 },
  { 5,  3, hex_button,   1, -3,  35 },
  { 5,  4, hex_button,   0, -2,  45 },
  { 5,  5, hex_button,   1, -1,  55 },
  { 5,  6, hex_button,   0,  0,  65 },
  { 5,  7, hex_button,   1,  1,  75 },
  { 5,  8, hex_button,   0,  2,  85 },
  { 5,  9, hex_button,   1,  3,  95 },
  { 5, 10, hex_button,   0,  4, 105 },
  { 5, 11, hex_button,   1,  5, 115 },
  { 5, 12, hex_button,   0,  6, 125 },
  { 5, 13, hex_button,   1,  7, 135 },
  { 5, 14, unused_pin, N_A,N_A, N_A },
  { 5, 15, unused_pin, N_A,N_A, N_A },
  //col mux switch type  x   y  pxl
  { 6,  0, hex_button,   2, -6,   6 },
  { 6,  1, hex_button,   3, -5,  16 },
  { 6,  2, hex_button,   2, -4,  26 },
  { 6,  3, hex_button,   3, -3,  36 },
  { 6,  4, hex_button,   2, -2,  46 },
  { 6,  5, hex_button,   3, -1,  56 },
  { 6,  6, hex_button,   2,  0,  66 },
  { 6,  7, hex_button,   3,  1,  76 },
  { 6,  8, hex_button,   2,  2,  86 },
  { 6,  9, hex_button,   3,  3,  96 },
  { 6, 10, hex_button,   2,  4, 106 },
  { 6, 11, hex_button,   3,  5, 116 },
  { 6, 12, hex_button,   2,  6, 126 },
  { 6, 13, hex_button,   3,  7, 136 },
  { 6, 14, unused_pin, N_A,N_A, N_A },
  { 6, 15, unused_pin, N_A,N_A, N_A },
  //col mux switch type  x   y  pxl
  { 7,  0, hex_button,   4, -6,   7 },
  { 7,  1, hex_button,   5, -5,  17 },
  { 7,  2, hex_button,   4, -4,  27 },
  { 7,  3, hex_button,   5, -3,  37 },
  { 7,  4, hex_button,   4, -2,  47 },
  { 7,  5, hex_button,   5, -1,  57 },
  { 7,  6, hex_button,   4,  0,  67 },
  { 7,  7, hex_button,   5,  1,  77 },
  { 7,  8, hex_button,   4,  2,  87 },
  { 7,  9, hex_button,   5,  3,  97 },
  { 7, 10, hex_button,   4,  4, 107 },
  { 7, 11, hex_button,   5,  5, 117 },
  { 7, 12, hex_button,   4,  6, 127 },
  { 7, 13, hex_button,   5,  7, 137 },
  { 7, 14, unused_pin, N_A,N_A, N_A },
  { 7, 15, unused_pin, N_A,N_A, N_A },
  //col mux switch type  x   y  pxl
  { 8,  0, hex_button,   6, -6,   8 },
  { 8,  1, hex_button,   7, -5,  18 },
  { 8,  2, hex_button,   6, -4,  28 },
  { 8,  3, hex_button,   7, -3,  38 },
  { 8,  4, hex_button,   6, -2,  48 },
  { 8,  5, hex_button,   7, -1,  58 },
  { 8,  6, hex_button,   6,  0,  68 },
  { 8,  7, hex_button,   7,  1,  78 },
  { 8,  8, hex_button,   6,  2,  88 },
  { 8,  9, hex_button,   7,  3,  98 },
  { 8, 10, hex_button,   6,  4, 108 },
  { 8, 11, hex_button,   7,  5, 118 },
  { 8, 12, hex_button,   6,  6, 128 },
  { 8, 13, hex_button,   7,  7, 138 },
  { 8, 14, unused_pin, N_A,N_A, N_A },
  { 8, 15, unused_pin, N_A,N_A, N_A },
  //col mux switch type  x   y  pxl
  { 9,  0, hex_button,   8, -6,   9 },
  { 9,  1, hex_button,   9, -5,  19 },
  { 9,  2, hex_button,   8, -4,  29 },
  { 9,  3, hex_button,   9, -3,  39 },
  { 9,  4, hex_button,   8, -2,  49 },
  { 9,  5, hex_button,   9, -1,  59 },
  { 9,  6, hex_button,   8,  0,  69 },
  { 9,  7, hex_button,   9,  1,  79 },
  { 9,  8, hex_button,   8,  2,  89 },
  { 9,  9, hex_button,   9,  3,  99 },
  { 9, 10, hex_button,   8,  4, 109 },
  { 9, 11, hex_button,   9,  5, 119 },
  { 9, 12, hex_button,   8,  6, 129 },
  { 9, 13, hex_button,   9,  7, 139 },
  { 9, 14, unused_pin, N_A,N_A, N_A },
  { 9, 15, unused_pin, N_A,N_A, N_A },
};


const unsigned keyboard_pin_reset_period_in_uS = 16;
const unsigned keyboard_reads_to_retain        = 10;
#include "hexBoardHardware/keys.h"
pinGrid_obj pinGrid;

const unsigned rotaryPinA = 20;
const unsigned rotaryPinB = 21;
const unsigned rotaryPinC = 24;
const unsigned rotary_pin_fire_period_in_uS = 768;
#include "hexBoardHardware/rotary.h"
rotary_obj rotary;

const unsigned piezoPin = 23;
const unsigned audioJackPin = 25;
const unsigned target_audio_sample_rate_in_Hz = 31'250;
#include "hexBoardHardware/audio.h"
audioOut_obj audioOut;

const unsigned ledCount = 140;
const unsigned ledPin = 22;
const unsigned OLED_sdaPin = 16;
const unsigned OLED_sclPin = 17;
const unsigned target_LED_frame_rate_in_Hz = 60;
// #include "hexBoardHardware/LED.h"

constexpr unsigned target_audio_sample_halfperiod_in_uS = 500'000 / target_audio_sample_rate_in_Hz;
constexpr unsigned hardware_tick_period_in_uS = 
  ( target_audio_sample_halfperiod_in_uS > keyboard_pin_reset_period_in_uS
  ? target_audio_sample_halfperiod_in_uS : keyboard_pin_reset_period_in_uS ); 
constexpr unsigned actual_audio_sample_period_in_uS = 2 * hardware_tick_period_in_uS;
constexpr unsigned actual_audio_sample_rate_in_Hz  = 1'000'000 / actual_audio_sample_period_in_uS;
#include "hexBoardHardware/taskmgr.h"

// global, call this on setup() i.e. the 1st core
void hardware_setup() {
  // note that the audio pins can be set by the user, and that routine can live outside
  // of the setup. in other words, it is safe to run audioOut in the background even
  // if there are no audio outputs selected.
  audioOut.setup(actual_audio_sample_rate_in_Hz);
  // based on settings, enable whichever audio out puns
  audioOut.set_pin(piezoPin, true);
  //  rotary should define a pin A and B. pin C is the center click button if it exists
  rotary.setup(rotaryPinA,rotaryPinB,rotaryPinC);
  //  first T/F: are the column pins analog?
  //  second T/F: iterate thru the multiplex pins before the column pins?
  pinGrid.setup(colPins, analogPins, muxPins, true, keyboard_reads_to_retain, 
    default_analog_calibration_up, default_analog_calibration_down
  );
}

// global, call this on setup1() i.e. the 2nd core
void hardware_start_background_process() {
    task_mgr.init(hardware_tick_period_in_uS);
    // audio sample update - highest priority (stable period needed)
    task_mgr.add_task(actual_audio_sample_period_in_uS, std::bind(&audioOut_obj::poll, &audioOut));
    // rotary knob - 2nd highest priority (input drop risk)
    task_mgr.add_task(rotary_pin_fire_period_in_uS,     std::bind(&rotary_obj::poll,   &rotary));
    // keyboard - lowest priority (timing requirements are loose)
    task_mgr.add_task(keyboard_pin_reset_period_in_uS,  std::bind(&pinGrid_obj::poll,  &pinGrid));
    // start receiving input and processing audio output
    task_mgr.begin();
}