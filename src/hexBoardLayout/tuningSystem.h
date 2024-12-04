#pragma once
// this is a modification of Tunings.h by Paul Walker to the HexBoard.
// most importantly, the members of the Tunings::Tuning class are made
// public to allow access by the Hexboard system as needed.
//
// -*-c++-*-
/**
 * Tunings.h
 * Copyright Paul Walker, 2019-2020
 * Released under the MIT License. See LICENSE.md
 */
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
//#include <memory>
//#include <array>
//#include <iomanip>
//#include <cstdlib>
//#include <math.h>
//#include <cctype>
//#include <cmath>

namespace Tunings {
	static constexpr double MIDI_0_FREQ = 8.17579891564371; // or 440.0 * pow( 2.0, - (69.0/12.0 ) )
	/**
	 * A Tone is a single entry in an SCL file. It is expressed either in cents or in
	 * a ratio, as described in the SCL documentation.
	 *
	 * In most normal use, you will not use this class, and it will be internal to a Scale
	 */
	struct Tone {
		typedef enum Type {
			kToneCents, // An SCL representation like "133.0"
			kToneRatio  // An SCL representation like "3/7"
		} Type;
		Type type;
		double cents;
		int64_t ratio_d, ratio_n;
		std::string stringRep;
		double floatValue; // cents / 1200 + 1.
		int lineno; // which line of the SCL does this tone appear on?
		Tone() : type(kToneRatio), cents(0), ratio_d(1), ratio_n(1), stringRep("1/1"), floatValue(1.0) {}
	};

	// Thank you to: https://gist.github.com/josephwb/df09e3a71679461fc104
	inline std::istream& getlineEndingIndependent(std::istream &is, std::string &t) {
		t.clear();
		std::istream::sentry se(is, true);
		if (!se) return is;
		std::streambuf *sb = is.rdbuf();
		for (;;) {
			int c = sb->sbumpc();
			switch (c) {
				case '\n': 
          return is;
				case '\r': 
          if (sb->sgetc() == '\n') sb->sbumpc(); 
          return is;
				case EOF:  
          is.setstate(std::ios::eofbit); 
          if (t.empty()) is.setstate(std::ios::badbit); 
          return is;
				default:	 
          t += (char)c;
      }
		}
	}

	inline double locale_atof(const char *s) {
		double result = 0;
		std::istringstream istr(s);
		istr.imbue(std::locale("C"));
		istr >> result;
		return result;
	}

	/**
	 * Given an SCL string like "100.231" or "3/7" set up a Tone
	 */
	inline Tone toneFromString(const std::string &line, int lineno) {
		Tone t;
		t.stringRep = line;
		t.lineno = lineno;
		if (line.find('.') != std::string::npos) {
			t.type = Tone::kToneCents;
			t.cents = locale_atof(line.c_str());
		}	else {
			t.type = Tone::kToneRatio;
			auto slashPos = line.find('/');
			if (slashPos == std::string::npos) {
				t.ratio_n = atoll(line.c_str());
				t.ratio_d = 1;
			}	else {
				t.ratio_n = atoll(line.substr(0, slashPos).c_str());
				t.ratio_d = atoll(line.substr(slashPos + 1).c_str());
			}
			if (t.ratio_n == 0 || t.ratio_d == 0)	{
        // throw error
			}
			// 2^(cents/1200) = n/d
			// cents = 1200 * log(n/d) / log(2)
			t.cents = 1200 * log(1.0 * t.ratio_n / t.ratio_d) / log(2.0);
		}
		t.floatValue = t.cents / 1200.0 + 1.0;
		return t;
	}

	/**
	 * The Scale is the representation of the SCL file. It contains several key
	 * features. Most importantly it has a count and a vector of Tones.
	 *
	 * In most normal use, you will simply pass around instances of this class
	 * to a Tunings::Tuning instance, but in some cases you may want to create
	 * or inspect this class yourself. Especially if you are displaying this
	 * class to your end users, you may want to use the rawText or count methods.
	 */
	struct Scale {
		std::string name;        // The name in the SCL file. Informational only
		std::string description; // The description in the SCL file. Informational only
		std::string rawText;     // The raw text of the SCL file used to create this Scale
		int count;               // The number of tones.
		std::vector<Tone> tones; // The tones

		Scale() : name("empty scale"), description(""), rawText(""), count(0) {}
	};

	/**
	 * readSCLStream returns a Scale from the SCL input stream
	 */
	inline Scale readSCLStream(std::istream &inf)	{
		std::string line;
		const int read_header = 0, read_count = 1, read_note = 2, trailing = 3;
		int state = read_header;
		Scale res;
		std::ostringstream rawOSS;
		int lineno = 0;
		while (getlineEndingIndependent(inf, line)) {
			rawOSS << line << "\n";
			lineno++;
			if ((state == read_note && line.empty()) || line[0] == '!') {
				continue;
			}
			switch (state) {
				case read_header:
					res.description = line;
					state = read_count;
					break;
				case read_count:
					res.count = atoi(line.c_str());
					if (res.count < 1) {
            // throw error
					}
					state = read_note;
					break;
				case read_note:
					auto t = toneFromString(line, lineno);
					res.tones.push_back(t);
					if ((int)res.tones.size() == res.count)
						state = trailing;
					break;
			}
		}
		if (!(state == read_note || state == trailing)) {
      // throw error
		}
		if ((int)res.tones.size() != res.count) {
      // throw error
		}
		res.rawText = rawOSS.str();
		return res;
	}

	/**
	 * readSCLFile returns a Scale from the SCL File in fname
	 */
	inline Scale readSCLFile(std::string fname)	{
		std::ifstream inf;
		inf.open(fname);
		if (!inf.is_open())	{
      // throw error
		}
		auto res = readSCLStream(inf);
		res.name = fname;
		return res;
	}

	/**
	 * parseSCLData returns a scale from the SCL file contents in memory
	 */
	inline Scale parseSCLData(const std::string &d) {
		std::istringstream iss(d);
		auto res = readSCLStream(iss);
		res.name = "Scale from patch";
		return res;
	}

	/**
	 * evenTemperament12NoteScale provides a utility scale which is
	 * the "standard tuning" scale
	 */
	inline Scale evenTemperament12NoteScale() {
		std::string data = R"SCL(! 12 Tone Equal Temperament.scl
!
12 Tone Equal Temperament | ED2-12 - Equal division of harmonic 2 into 12 parts
12
!
100.00000
200.00000
300.00000
400.00000
500.00000
600.00000
700.00000
800.00000
900.00000
1000.00000
1100.00000
2/1
)SCL";
		return parseSCLData(data);
	}

	/**
	 * evenDivisionOfSpanByM provides a scale referd to as "ED2-17" or
	 * "ED3-24" by dividing the Span into M points. eventDivisionOfSpanByM(2,12)
	 * should be the evenTemperament12NoteScale
	 */
	inline Scale evenDivisionOfSpanByM(int Span, int M) {
		if (Span <= 0) {
      // throw error
    }
		if (M <= 0) {
      // throw error
    }
		std::ostringstream oss;
		oss.imbue(std::locale("C"));
		oss << "! Automatically generated ED" << Span << "-" << M << " scale\n";
		oss << "Automatically generated ED" << Span << "-" << M << " scale\n";
		oss << M << "\n";
		oss << "!\n";
		double topCents = 1200.0 * log(1.0 * Span) / log(2.0);
		double dCents = topCents / M;
		for (int i = 1; i < M; ++i)
			oss << std::fixed << dCents * i << "\n";
		oss << Span << "/1\n";
		return parseSCLData(oss.str());
	}

	/**
	 * evenDivisionOfCentsByM provides a scale which divides Cents into M
	 * steps. It is less frequently used than evenDivisionOfSpanByM for obvious
	 * reasons. If you want the last cents label labeled differently than the cents
	 * argument, pass in the associated optional label
	 */
	inline Scale evenDivisionOfCentsByM(float Cents, int M, const std::string &lastLabel) {
		if (Cents <= 0) {
      // throw error
    }
		if (M <= 0) {
      // throw error
    }
		std::ostringstream oss;
		oss.imbue(std::locale("C"));
		oss << "! Automatically generated Even Division of " << Cents << " ct into " << M << " scale\n";
		oss << "Automatically generated Even Division of " << Cents << " ct into " << M << " scale\n";
		oss << M << "\n";
		oss << "!\n";
		double topCents = Cents;
		double dCents = topCents / M;
		for (int i = 1; i < M; ++i)
			oss << std::fixed << dCents * i << "\n";
		if (lastLabel.empty())
			oss << Cents << "\n";
		else
			oss << lastLabel << "\n";
		return parseSCLData(oss.str());
	}

	/**
	 * The KeyboardMapping class represents a KBM file. In most cases, the salient
	 * features are the tuningConstantNote and tuningFrequency, which allow you to
	 * pick a fixed note in the midi keyboard when retuning. The KBM file can also
	 * remap individual keys to individual points in a scale, which kere is done with the
	 * keys vector.
	 *
	 * Just as with Scale, the rawText member contains the text of the KBM file used.
	 */
	struct KeyboardMapping {
		int count;
		int firstMidi, lastMidi;
		int middleNote;
		int tuningConstantNote;
		double tuningFrequency, tuningPitch; // pitch = frequency / MIDI_0_FREQ
		int octaveDegrees;
		std::vector<int> keys; // rather than an 'x' we use a '-1' for skipped keys

		std::string rawText;
		std::string name;

		inline KeyboardMapping()
		: count(0)
		, firstMidi(0)
		, lastMidi(127)
		, middleNote(60)
		, tuningConstantNote(60)
		,	tuningFrequency(MIDI_0_FREQ * 32.0)
		, tuningPitch(32.0)
		, octaveDegrees(0)
		, rawText("")
		,	name("") {
			std::ostringstream oss;
			oss.imbue(std::locale("C"));
			oss << "! Default KBM file\n";
			oss << count << "\n"
					<< firstMidi << "\n"
					<< lastMidi << "\n"
					<< middleNote << "\n"
					<< tuningConstantNote << "\n"
					<< tuningFrequency << "\n"
					<< octaveDegrees << "\n";
			rawText = oss.str();
		}
	};

	/**
	 * readKBMStream returns a KeyboardMapping from a KBM input stream
	 */
	inline KeyboardMapping readKBMStream(std::istream &inf) {
		std::string line;
		KeyboardMapping res;
		std::ostringstream rawOSS;
		res.keys.clear();
		enum parsePosition {
				map_size = 0,
				first_midi,
				last_midi,
				middle,
				reference,
				freq,
				degree,
				keys,
				trailing
		};
		parsePosition state = map_size;
		int lineno = 0;
		while (getlineEndingIndependent(inf, line)) {
			rawOSS << line << "\n";
			lineno++;
			if (line[0] == '!') {
				continue;
			}
			if (line == "x")
					line = "-1";
			else if (state != trailing) {
				const char *lc = line.c_str();
				bool validLine = line.length() > 0;
				char badChar = '\0';
				while (validLine && *lc != '\0') {
					if (!(
						   *lc == ' ' || std::isdigit(*lc) 
						|| *lc == '.' || *lc == (char)13 
						|| *lc == '\n')
					) {
						validLine = false;
						badChar = *lc;
					}
					lc++;
				}
				if (!validLine) {
          // throw error
				}
			}
			int i = std::atoi(line.c_str());
			double v = locale_atof(line.c_str());
			switch (state) {
				case map_size:
					res.count = i;
					break;
				case first_midi:
					res.firstMidi = i;
					break;
				case last_midi:
					res.lastMidi = i;
					break;
				case middle:
					res.middleNote = i;
					break;
				case reference:
					res.tuningConstantNote = i;
					break;
				case freq:
					res.tuningFrequency = v;
					res.tuningPitch = res.tuningFrequency / 8.17579891564371;
					break;
				case degree:
					res.octaveDegrees = i;
					break;
				case keys:
					res.keys.push_back(i);
					if ((int)res.keys.size() == res.count)
							state = trailing;
					break;
				case trailing:
					break;
			}
			if (!(state == keys || state == trailing))
				state = (parsePosition)(state + 1);
			if (state == keys && res.count == 0)
				state = trailing;
		}
		if (!(state == keys || state == trailing)) {
      // throw error
		}
		if ((int)res.keys.size() != res.count) {
      // throw error
		}
		res.rawText = rawOSS.str();
		return res;
	}

	/**
	 * readKBMFile returns a KeyboardMapping from a KBM file name
	 */
	inline KeyboardMapping readKBMFile(std::string fname) {
		std::ifstream inf;
		inf.open(fname);
		if (!inf.is_open())	{
      // throw error
		}
		auto res = readKBMStream(inf);
		res.name = fname;
		return res;
	}

	/**
	 * parseKBMData returns a KeyboardMapping from a KBM data in memory
	 */
	inline KeyboardMapping parseKBMData(const std::string &d)	{
		std::istringstream iss(d);
		auto res = readKBMStream(iss);
		res.name = "Mapping from patch";
		return res;
	}

	/**
	 * startScaleOnAndTuneNoteTo generates a KBM where scaleStart is the note 0
	 * of the scale, where midiNote is the tuned note, and where feq is the frequency
	 */
	inline KeyboardMapping startScaleOnAndTuneNoteTo(int scaleStart, int midiNote, double freq) {
		std::ostringstream oss;
		oss.imbue(std::locale("C"));
		oss << "! Automatically generated mapping, tuning note " << midiNote << " to " << freq
				<< " Hz\n"
				<< "!\n"
				<< "! Size of map\n"
				<< 0 << "\n"
				<< "! First and last MIDI notes to map - map the entire keyboard\n"
				<< 0 << "\n"
				<< 127 << "\n"
				<< "! Middle note where the first entry in the scale is mapped.\n"
				<< scaleStart << "\n"
				<< "! Reference note where frequency is fixed\n"
				<< midiNote << "\n"
				<< "! Frequency for MIDI note " << midiNote << "\n"
				<< freq << "\n"
				<< "! Scale degree for formal octave. This is an empty mapping, so:\n"
				<< 0 << "\n"
				<< "! Mapping. This is an empty mapping so list no keys\n";
		return parseKBMData(oss.str());
	}

	/**
	 * tuneNoteTo creates a KeyboardMapping which keeps the midi note given is set
	 * to a constant frequency, given
	 */
	inline KeyboardMapping tuneNoteTo(int midiNote, double freq) {
		return startScaleOnAndTuneNoteTo(60, midiNote, freq);
	}
	/**
	 * tuneA69To creates a KeyboardMapping which keeps the midi note 69 (A4) set
	 * to a constant frequency, given
	 */
	inline KeyboardMapping tuneA69To(double freq) { return tuneNoteTo(69, freq); }

	/**
	 * The Tuning class is the primary place where you will interact with this library.
	 * It is constructed for a scale and mapping and then gives you the ability to
	 * determine frequencies across and beyond the midi keyboard. Since modulation
	 * can force key number well outside the [0,127] range in some of our synths we
	 * support a midi note range from -256 to + 256 spanning more than the entire frequency
	 * space reasonable.
	 *
	 * To use this class, you construct a fresh instance every time you want to use a
	 * different Scale and Keyboard. If you want to tune to a different scale or mapping,
	 * just construct a new instance.
	 */
	class Tuning {
		public:
			// The number of notes we pre-compute
			constexpr static int N = 512;
			std::array<double, N> ptable, lptable;
			std::array<int, N> scalepositiontable;
      std::array<int, N> equaves;
			bool allowTuningCenterOnUnmapped{false};
			// For convenience, the scale and mapping used to construct this are kept as public copies
			Scale scale;
			KeyboardMapping keyboardMapping;

			inline Tuning(const Scale &s_, const KeyboardMapping &k_, bool allowTuningCenterOnUnmapped_)
			: allowTuningCenterOnUnmapped(allowTuningCenterOnUnmapped_) {
        /*
        ** For purposes of this tuning table,
        ** IGNORE the firstMIDI and lastMIDI range.
        ** Attempt to assign pitches and degrees to
        ** all possible keys to allow for modulation.
        */
        this->scale = s_;
        if (s_.count <= 0) {
          // throw error
        }
        Scale s = s_; // in case we need to modify a copy as we go
        this->keyboardMapping = k_;
        KeyboardMapping k = k_; // in case we need to modify a copy as we go
        if (k_.count == 0) 
        {
          // default empty map = linear map against scale, formal octave = scale equave
          k.count = s.count;
          k.octaveDegrees = s.count;
          k.keys.clear();
          for (int i = 0; i < k.count; ++i)
            k.keys.push_back(i);
        }
        // int entryZero = (N / 2); // i.e. if 512 notes, start storing map at 256
        int entryZero = 256;   
        int entryMiddle = entryZero + k.middleNote;
        int entryTCN = entryZero + k.tuningConstantNote;
        bool TCNfoundInMap = false;
        int justBelowTCN = 0;
        int justAboveTCN = N;
        /*
        ** Assign scale degrees to each key
        */
        
        for (int i = 0; i < N; ++i) 
        {
          /*
          ** Keep track of which mapped keys are closest to
          ** tuning constant note
          */
          if (!TCNfoundInMap) 
          {
            TCNfoundInMap = (i == entryTCN);
            if ((i < entryTCN) && (i > justBelowTCN)) justBelowTCN = i;
            if ((i > entryTCN) && (i < justAboveTCN)) justAboveTCN = i;
          }
          int notes_from_middle = i - entryMiddle;
          int k_mod = (((notes_from_middle % k.count) + k.count) % k.count);
          int map_lookup = k.keys[k_mod];
          if (map_lookup < 0) 
          {
            equaves[i] = 0;
            scalepositiontable[i] = -1;
          } 
          else 
          {  
            int map_period = (notes_from_middle - k_mod) / k.count;
            int map_degrees = map_lookup + map_period * k.octaveDegrees;
            scalepositiontable[i] = (((map_degrees % s.count) + s.count) % s.count);
            equaves[i] = (map_degrees - scalepositiontable[i]) / s.count;  
          }
        }
        /*
        ** Use the tuning anchor note to set the pitch of the middle key
        ** If tuning center is on an unmapped note (and if that's allowed),
        ** pretend the nearest mapped notes are tuned so that the
        ** unmapped note would interpolate to the assigned frequency
        */
        if (!TCNfoundInMap && !allowTuningCenterOnUnmapped)
        {
          // error throw
        }
        int mappedTCN = entryTCN;     // i.e. 256 + k.TuningConstantNote
        double TCN_position_in_span;
        if (!TCNfoundInMap) 
        {
          mappedTCN = justBelowTCN;
          TCN_position_in_span = (double)(entryTCN - justBelowTCN) / (double)(justAboveTCN - justBelowTCN);
        }
        /*
        ** the tuning frequency is for a note that is X.xxx octaves above
        ** the root of the scale in the same octave as the middle note of the layout
        ** ...which is not always the same as the 1st key of the layout, if the first
        ** note of the mapping is not zero. the floatValue here returns the value of X.xxx
        */
        double equave_lp = s.tones[s.count - 1].floatValue - 1.0;
        double TCN_lp = (equave_lp * equaves[mappedTCN]) 
          + (s.tones[scalepositiontable[mappedTCN]].floatValue - 1.0);
        if (!TCNfoundInMap) 
        { 
          double justAbove_lp = (equave_lp * equaves[justAboveTCN]) 
            + (s.tones[scalepositiontable[justAboveTCN]].floatValue - 1.0);
          TCN_lp += TCN_position_in_span * (justAbove_lp - TCN_lp);
        }
        double middleRoot_lp = (log(k.tuningPitch) / log(2)) - TCN_lp;
        /*
        ** we can now assign a pitch to all other mapped keys.
        */
        for (int i = 0; i < N; ++i) 
        {
          if (scalepositiontable[i] < 0) 
          {
            lptable[i] = 0;
            ptable[i] = 0;
          } 
          else 
          {
            lptable[i] = middleRoot_lp + (equave_lp * equaves[i]) 
              + (s.tones[scalepositiontable[i]].floatValue - 1.0);
            ptable[i] = pow(2.0, lptable[i]);
          }
        }

			}
			/*
			 * Skipped notes can either have nonsense values or interpolated values.
			 * The old API made the bad choice to have nonsense values which we retain
			 * for compatability, but this method will return a new tuning with correctly
			 * interpolated skipped notes.
			 */
			inline Tuning withSkippedNotesInterpolated() const {
				Tuning res = *this;
				for (int i = 1; i < N - 1; ++i)	{
					if (scalepositiontable[i] < 0) {
						int nxt = i + 1;
						int prv = i - 1;
						while (prv >= 0 && scalepositiontable[prv] < 0)
							prv--;
						while (nxt < N && scalepositiontable[nxt] < 0)
							nxt++;
						float dist = (float)(nxt - prv);
						float frac = (float)(i - prv) / dist;
						res.lptable[i] = (1.0 - frac) * lptable[prv] + frac * lptable[nxt];
						res.ptable[i] = pow(2.0, res.lptable[i]);
					}
				}
				return res;
			}

			/**
			 * These three related functions provide you the information you
			 * need to use this tuning.
			 *
			 * frequencyForMidiNote returns the Frequency in HZ for a given midi
			 * note. In standard tuning, FrequencyForMidiNote(69) will be 440
			 * and frequencyForMidiNote(60) will be 261.62 - the standard frequencies
			 * for A and middle C.
			 *
			 * frequencyForMidiNoteScaledByMidi0 returns the frequency but with the
			 * standard frequency of midi note 0 divided out. So in standard tuning
			 * frequencyForMidiNoteScaledByMidi0(0) = 1 and frequencyForMidiNoteScaledByMidi0(60) = 32
			 *
			 * Finally logScaledFrequencyForMidiNote returns the log base 2 of the scaled frequency.
			 * So logScaledFrequencyForMidiNote(0) = 0 and logScaledFrequencyForMidiNote(60) = 5.
			 *
			 * Both the frequency measures have the feature of doubling when frequency doubles
			 * (or when a standard octave is spanned), whereas the log one increase by 1 per frequency
			 * double.
			 *
			 * Depending on your internal pitch model, one of these three methods should allow you
			 * to calibrate your oscillators to the appropriate frequency based on the midi note
			 * at hand.
			 *
			 * The scalePositionForMidiNote returns the space in the logical scale. Note 0 is the root.
			 * It has a maxiumum value of count-1. Note that SCL files omit the root internally and so
			 * this logical scale position is off by 1 from the index in the tones array of the Scale data.
			 */
			inline double frequencyForMidiNote(int mn) const {
				auto mni = std::min(std::max(0, mn + 256), N - 1);
				return ptable[mni] * MIDI_0_FREQ;
			}

			inline double frequencyForMidiNoteScaledByMidi0(int mn) const {
				auto mni = std::min(std::max(0, mn + 256), N - 1);
				return ptable[mni];
			}

			inline double logScaledFrequencyForMidiNote(int mn) const {
				auto mni = std::min(std::max(0, mn + 256), N - 1);
				return lptable[mni];
			}

			inline double retuningFromEqualInCentsForMidiNote(int mn) const	{
				return retuningFromEqualInSemitonesForMidiNote(mn) * 100.0;
			}

			inline double retuningFromEqualInSemitonesForMidiNote(int mn) const {
				return logScaledFrequencyForMidiNote(mn) * 12 - mn;
			}

			inline int scalePositionForMidiNote(int mn) const	{
				auto mni = std::min(std::max(0, mn + 256), N - 1);
				return scalepositiontable[mni];
			}
			inline int equaveForMidiNote(int mn) const	{
				auto mni = std::min(std::max(0, mn + 256), N - 1);
				return equaves[mni];
			}
			inline bool isMidiNoteMapped(int mn) const {
				auto mni = std::min(std::max(0, mn + 256), N - 1);
				return scalepositiontable[mni] >= 0;
			}
	};
} // namespace Tunings