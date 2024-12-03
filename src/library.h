#pragma once
#include <tuningSystem.h>

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
Tunings::Scale carlos_alpha  = Tunings::evenDivisionOfCentsByM(701.684905896, 9);
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
Tunings::KeyboardMapping default_kbm = Tunings::startScaleOnAndTuneNoteTo(60, 69, 440.0);


Tunings::Tuning default_12_edo(scale_12_edo, default_kbm, false);
