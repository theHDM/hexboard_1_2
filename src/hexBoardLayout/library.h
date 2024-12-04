#pragma once
#include <vector>
#include "tuningSystem.h"
#include "hexagon.h"

// here is a library of tunings, palettes, etc.
// that we use to populate presets.


/**
  * The Scale is the representation of an .SCL, or Scala, file. 
  * It contains several key features. 
  * Most importantly it has a count and a vector of Tones.
  * You can build a scale either by passing a string/file
  * formatted in the SCL way, or you can initialize the
  * class yourself.
  */

Tunings::Scale scale_12_edo  = Tunings::evenDivisionOfSpanByM(2, 12);
Tunings::Scale scale_31_edo  = Tunings::evenDivisionOfSpanByM(2, 31);
Tunings::Scale bohlen_pierce = Tunings::evenDivisionOfSpanByM(3, 13);
Tunings::Scale carlos_alpha  = Tunings::evenDivisionOfCentsByM(701.684905896, 9, "Carlos Alpha");
Tunings::Scale mos_3L_5s = Tunings::parseSCLData(R"SCL(!
MOS_3L_5s_generator=757c
8
129.0
258.0
443.0
572.0
701.0
886.0
1015.0
1200.0
)SCL");

	/**
	 * The KeyboardMapping class represents a KBM file. In most cases, the salient
	 * features are the tuningConstantNote and tuningFrequency, which allow you to
	 * pick a fixed note in the midi keyboard when retuning. The KBM file can also
	 * remap individual keys to individual points in a scale, which kere is done with the
	 * keys vector.
	 *
	 * Just as with Scale, the rawText member contains the text of the KBM file used.
	 */
Tunings::KeyboardMapping kbm_A440_root_C = Tunings::startScaleOnAndTuneNoteTo(60, 69, 440.0);

Tunings::Tuning default_12_edo(scale_12_edo, kbm_A440_root_C, false);

// struct sub_scale

struct hex_value_pair {
  hex_t hex;
  int value;
};

struct key_layout {
  hex_t root_location;
  int steps_per_2x;
  int steps_per_2y;
  int OLED_orientation;
  std::vector<hex_value_pair> overrides;
  key_layout(hex_t root, hex_t sm_hex, int sm_val, hex_t lg_hex, int lg_val)
  : root_location(root)
  {
    int d = (sm_hex.x * lg_hex.y) - (sm_hex.y * lg_hex.x);
    steps_per_2x = 2 * ((sm_val * lg_hex.y) - (lg_val * sm_hex.y)) / d;
    steps_per_2y = 2 * ((lg_val * sm_hex.x) - (sm_val * lg_hex.x)) / d;
  }
};

key_layout wicki_hayden_12({0, 0}, unitHex[dir_e], 2, unitHex[dir_nw], 5);