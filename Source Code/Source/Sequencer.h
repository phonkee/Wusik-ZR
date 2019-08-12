// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

	Created by William Kalfelz @ Wusik Dot Com (c) 2019

*/

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "PluginProcessor.h"
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WusikSequencerStep
{
public:
	WusikSequencerStep()
	{
		volume = 1.0f;
		pan = 0.5f;
		fine = 0.5f;
		tune = 0.5f;
		waveform = 0.0f;
		filter = 0.5f;
		time = 1.0f / 128.0f;
		crossFade = 0.0f;
		reset();
	};
	//
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void reset()
	{
		for (int xv = 0; xv < MAX_WAVETABLE_VOICES; xv++)
		{
			timeCounter[xv] = 0;
		}
	}
	//
	float volume, pan, fine, crossFade, tune, time, waveform, filter;
	int timeCounter[MAX_WAVETABLE_VOICES];
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WusikSequencer
{
public:
	WusikSequencer(WusikWaveform *_wavetables) : globalBPM(120.0), sampleRate(44100.0f), wavetables(_wavetables)
	{
		wavetableObjects[0] = wavetableObjects[1] = nullptr;
		xRandom.setSeedRandomly();
		reset();
		//
		theMode = kForward;
		mode = 0.0f;
		isStatic = true;
		time1 = time2 = isBPM = 1.0f;
		loopTo = 0.0f;
		loopToThis = 0;
		layer_mute = layer_solo = layer_volume = 1.0f;
		layer_pan = 0.5f;
		layer_tune = 0.5f;
		layer_fine = 0.5f;
		layer_addMultiply = 0.0f;
		layer_pitchEnvelope = 0.5f;
		stepsSize = 1;
	};
	//
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void reset()
	{
		isUnison = false;
		//
		for (int xv = 0; xv < MAX_WAVETABLE_VOICES; xv++)
		{
			pingPongWavefom[xv] = 0; // used to go from one waveform to the next one //
			position[xv] = rate[xv] = 0.0f;
			currentStep[xv] = 0;
			pingPongModeForward[xv] = true;
			unisonFine[xv] = 0.5f;
			unisonVolume[xv] = 1.0f;
			unisonPan[xv] = 0.5f;
			unisonPosition[xv] = 0.0f;
		}
	}
	//
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void updateRate(int voice)
	{
		if (isBPM > 0.0f)
		{
			float value1 = int32(jlimit(1.0f, 128.0f, (time1 * 127.0f) + 1));
			float value2 = int32(jlimit(1.0f, 128.0f, (time2 * 127.0f) + 1));
			rate[voice] = (globalBPM / (((60.0f * value1) / value2)) / sampleRate);
		}
		else
		{
			rate[voice] = (time1 * float(int32(jlimit(1.0f, 1000.0f, time2 * 1000.0f))) / sampleRate);
		}
	}
	// 
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	forcedinline void processNext(int maxVoices, int BPM)
	{
		if (!isStatic && stepsSize > 0)
		{
			globalBPM = BPM;
			//
			for (int xv = 0; xv < maxVoices; xv++)
			{
				position[xv] += rate[xv];
				if (position[xv] > 1.0f)
				{
					position[xv] -= 1.0f;
					steps[currentStep[xv]].timeCounter[xv]++;
					//
					if (steps[currentStep[xv]].timeCounter[xv] >= int(((steps[currentStep[xv]].time * 127.0f) + 1.0f)))
					{
						if (theMode == kForward)
						{
							currentStep[xv]++;
							if (currentStep[xv] >= stepsSize) currentStep[xv] = loopToThis;
						}
						else if (theMode == kReverse)
						{
							currentStep[xv]--;
							if (currentStep[xv] < 0) currentStep[xv] = loopToThis;
						}
						else if (theMode == kPingPong)
						{
							if (pingPongModeForward[xv])
							{
								currentStep[xv]++;
								if (currentStep[xv] >= (stepsSize - 1))
								{
									currentStep[xv] = stepsSize - 1;
									pingPongModeForward[xv] = !pingPongModeForward[xv];
								}
							}
							else
							{
								currentStep[xv]--;
								if (currentStep[xv] < 0)
								{
									currentStep[xv] = 0;
									pingPongModeForward[xv] = !pingPongModeForward[xv];
								}
							}
						}
						else //if (theMode == kRandom)
						{
							if (stepsSize > 2)
							{
								int xCounter = 0;
								int nextStep = -1;
								while (nextStep == -1 && nextStep != currentStep[xv])
								{
									nextStep = jlimit(0, stepsSize - 1,  int(xRandom.nextFloat() * float(stepsSize)));
									xCounter++;
									if (xCounter > 99) break;
								}
								currentStep[xv] = nextStep;
								//ShowValue(currentStep[xv]);
							}
							else currentStep[xv] = jlimit(0, stepsSize - 1, int(xRandom.nextFloat() * float(stepsSize)));
						}
						//
						steps[currentStep[xv]].timeCounter[xv] = 0;
						pingPongWavefom[xv] = 1 - pingPongWavefom[xv];
						setWaveform(xv, true);
						calculateCrossfade(xv, false);
					}
				}
			}
		}
	}
	//
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void calculateCrossfade(int voice, bool isFirst)
	{
		if (isFirst)
		{
			wavetableObjects[0]->fade[voice] = 1.0f;
			wavetableObjects[0]->fadeRate[voice] = 0.0f;
			wavetableObjects[1]->fade[voice] = 0.0f;
			wavetableObjects[1]->fadeRate[voice] = 0.0f;
		}
		else
		{
			if (steps[currentStep[voice]].crossFade == 1.0f) // Complete Crossfade //
			{
				wavetableObjects[1 - pingPongWavefom[voice]]->fade[voice] = 1.0f; // Previous Waveform
				wavetableObjects[1 - pingPongWavefom[voice]]->fadeRate[voice] = -1.0f * (rate[voice] * (steps[currentStep[voice]].time + 1)); // going down (negative) //
				//
				wavetableObjects[pingPongWavefom[voice]]->fade[voice] = 0.0f; // Current Waveform
				wavetableObjects[pingPongWavefom[voice]]->fadeRate[voice] = rate[voice] * (steps[currentStep[voice]].time + 1);

			}
			else if (steps[currentStep[voice]].crossFade > 0.0f) // Parcial Crossfade //
			{
				// !!!! Needs to be fixed ///
				wavetableObjects[1 - pingPongWavefom[voice]]->fade[voice] = 1.0f; // Previous Waveform
				wavetableObjects[1 - pingPongWavefom[voice]]->fadeRate[voice] = 
					-1.0f * 
					((1.0f - steps[currentStep[voice]].crossFade) +
					(steps[currentStep[voice]].crossFade * (rate[voice] * (steps[currentStep[voice]].time + 1))));
				//
				wavetableObjects[pingPongWavefom[voice]]->fade[voice] = 0.0f; // Current Waveform
				wavetableObjects[pingPongWavefom[voice]]->fadeRate[voice] = 
					(1.0f - steps[currentStep[voice]].crossFade) +
					(steps[currentStep[voice]].crossFade * (rate[voice] * (steps[currentStep[voice]].time + 1)));
			}
			else
			{
				wavetableObjects[1 - pingPongWavefom[voice]]->fade[voice] = 1.0f; // Previous Waveform
				wavetableObjects[1 - pingPongWavefom[voice]]->fadeRate[voice] = -0.04f; // Very Fast //
				//
				wavetableObjects[pingPongWavefom[voice]]->fade[voice] = 1.0f; // Current Waveform
				wavetableObjects[pingPongWavefom[voice]]->fadeRate[voice] = 0.0f;
			}
		}
	}
	//
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void startSequence(MidiMessage message, float _unisonFine, float _unisonVolume, float _unisonPan, float _unisonPosition, bool _reStart, bool _isUnison)
	{
		#if 0
			File xDebug("c:\\temp\\debug.txt");
			xDebug.appendText("-----------------\n");
			xDebug.appendText("Start Sequence\n");
			if (_reStart) xDebug.appendText("DO Start Sequence\n"); else xDebug.appendText("NOT Start Sequence\n");
		#endif
		//
		theMode = int(mode * 3.0f);
		isUnison = _isUnison;
		int voice = int(message.getTimeStamp());
		if (_reStart) currentStep[voice] = 0;
		//
		initialMidiMessage[voice] = message;
		int note = jlimit(0, 127, message.getNoteNumber() + int((layer_tune * (48.0f * 2.0f)) - 48.0f));
		initialMidiMessage[voice].setNoteNumber(note);
		//
		if (_reStart)
		{
			unisonFine[voice] = _unisonFine;
			unisonVolume[voice] = _unisonVolume;
			unisonPan[voice] = _unisonPan;
			unisonPosition[voice] = _unisonPosition;
		}
		//
		loopToThis = jlimit(0, (stepsSize - 1), int(loopTo * 127.0f));
		wavetableObjects[0]->isSequenced = false;
		//
		if (_reStart)
		{
			position[voice] = 1.0f;
			steps[0].timeCounter[voice] = 0;
			pingPongWavefom[voice] = 0;
		}
		//
		setWaveform(voice, _reStart);
		//
		if (!isStatic)
		{
			wavetableObjects[0]->isSequenced = wavetableObjects[1]->isSequenced = true;
			//
			if (_reStart)
			{
				if (theMode == kRandom) currentStep[voice] = jlimit(0, stepsSize - 1, int(xRandom.nextFloat() * float(stepsSize)));
				else if (theMode == kReverse) currentStep[voice] = stepsSize - 1;
				//
				updateRate(voice);
				calculateCrossfade(voice, true);
				if (steps[0].crossFade == 0.0f) position[voice] = 0.0f;
			}
		}
	}
	//
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void setWaveform(int voice, bool restart)
	{
		if (wavetableObjects[pingPongWavefom[voice]] == nullptr || stepsSize == 0) return;
		//
		MidiMessage message = initialMidiMessage[voice];
		message.setNoteNumber(jlimit(0, 127, message.getNoteNumber() + int((steps[currentStep[voice]].tune * (48.0f * 2.0f)) - 48.0f)));
		if (message.getVelocity() == 0) message.setVelocity(1.0f);
		int nextWaveform = jlimit(0, 47, int(steps[currentStep[voice]].waveform * 47.0f));
		//
		float theVolume = steps[currentStep[voice]].volume * layer_volume * layer_mute * layer_solo;
		float thePan = (steps[currentStep[voice]].pan + layer_pan) * 0.5f;
		float theFine = (steps[currentStep[voice]].fine + layer_fine) * 0.5f;
		//
		if (isUnison)
		{
			theVolume = unisonVolume[voice];
			thePan = unisonPan[voice];
		}
		//
		wavetableObjects[pingPongWavefom[voice]]->changeWaveform(voice, message, theVolume, thePan, wavetables[nextWaveform].wavetable, theFine, unisonPosition[voice], restart, unisonFine[voice]);
		//
		#if 0
			File xDebug("c:\\temp\\debug.txt");
			xDebug.appendText("-----------------\n");
			xDebug.appendText("Voice: " + String(voice) + "\n");
			xDebug.appendText("Note: " + String(message.getNoteNumber()) + "\n");
			xDebug.appendText("Vel: " + String(message.getVelocity()) + "\n");
			xDebug.appendText("Vol: " + String(theVolume, 4) + "\n");
			xDebug.appendText("Fine: " + String(theFine, 4) + "\n");
			xDebug.appendText("Pan: " + String(thePan, 4) + "\n");
			xDebug.appendText("Unison Pos: " + String(unisonPosition[voice], 4) + "\n");
			xDebug.appendText("Unison Fine: " + String(unisonFine[voice], 4) + "\n");
			xDebug.appendText("Waveform: " + String(nextWaveform) + "\n");
			xDebug.appendText("pingPongWavefom: " + String(pingPongWavefom[voice]) + "\n");
			if (restart) xDebug.appendText("DO Start\n"); else xDebug.appendText("NOT Start\n");
		#endif
	}
	//
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void setWavetableObjects(WusikWavetable::WusikWavetableObject* wavetableObjects1, WusikWavetable::WusikWavetableObject* wavetableObjects2 = nullptr)
	{
		wavetableObjects[0] = wavetableObjects1;
		wavetableObjects[1] = wavetableObjects2;
	};
	//
	bool isStatic, isUnison;
	int loopToThis;
	int pingPongWavefom[MAX_WAVETABLE_VOICES], currentStep[MAX_WAVETABLE_VOICES];
	float position[MAX_WAVETABLE_VOICES], rate[MAX_WAVETABLE_VOICES], sampleRate, globalBPM, pingPongModeForward[MAX_WAVETABLE_VOICES];
	float unisonFine[MAX_WAVETABLE_VOICES], unisonVolume[MAX_WAVETABLE_VOICES], unisonPan[MAX_WAVETABLE_VOICES], unisonPosition[MAX_WAVETABLE_VOICES];
	WusikSequencerStep steps[128];
	int stepsSize, theMode;
	WusikWavetable::WusikWavetableObject* wavetableObjects[2]; // one is for regular playback, the other is for the next sequencer step waveform //
	Random xRandom;
	MidiMessage initialMidiMessage[MAX_WAVETABLE_VOICES];
	WusikWaveform *wavetables;
	float layer_volume, layer_pan, layer_fine, layer_mute, layer_solo, layer_tune, loopTo, mode, isBPM, time1, time2, layer_addMultiply, layer_pitchEnvelope;
	//
	enum
	{
		kForward, kReverse, kPingPong, kRandom
	};
};