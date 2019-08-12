/*
BEGIN_JUCE_MODULE_DECLARATION
    ID:               WusikADSREnvelope
    vendor:           Wusik Dot Com
    version:          1.0.0
    name:             WusikADSREnvelope
    description:      Wusik ADSR Envelope using Template ready for the SIMD Instructions
    website:          https://www.Wusik.com
    dependencies:     WusikSIMD
	license:          Open-Source
END_JUCE_MODULE_DECLARATION 
*/
//
// -------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "WusikUtils/WusikUtils.h"
//
#define MAX_ENVELOPE_VOICES 32
#define ADD_EXTRA_CONTROLLER(controller) midiMessagesPlay.add(MidiMessage::controllerEvent(1, 100, controller))
//
enum
{
	kEnv_FirstNote = 42,
	kEnv_NotFirstNote,
	kEnv_Reset_Unison
};
//
// -------------------------------------------------------------------------------------------------------------------------------
class WusikADSREnvelope
{
public:
	#define envAttack 0.0f
	#define envRelease 1.0f
	//
	DECLARE_ALIGNED(32);
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	WusikADSREnvelope() : isMonophonic(false), sampleRate(44100.0f), maxVoices(MAX_ENVELOPE_VOICES), prevFloatVoices(5.0f), prevUnison(0.0f), isLooped(0.0f)
	{ 
		reset(); 
	};
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void reset()
	{
		for (int xw = 0; xw < MAX_ENVELOPE_VOICES; xw++)
		{
			value[xw] = 0.0f;
			output[xw] = 0.0f;
			velocity[xw] = 0.0f;
			sustain[xw] = 0.0f;
			position[xw] = envRelease;
			rate[0][xw] = 0.0f;
			rate[1][xw] = 0.0f;
			isRelease[xw] = true;
			keyCounter[xw] = 0;
			voice[xw] = 0;
			isStealingNote[xw] = false;
			sumValues[xw] = 0.0f;
			loopedRelease[xw] = 0.0f;
			loopedReleaseRate[xw] = 0.0f;
			resetFilter[xw] = true;
		}
		//
		holdingNotes.clear();
		midiMessagesPlay.clear();
		hasStealingVoice = false;
		globalKeyCounter = 0;
		isIdle = true;
	};
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void changeMaxVoices(float _maxVoices)
	{
		if (prevFloatVoices != _maxVoices)
		{
			prevFloatVoices = _maxVoices;
			//
			switch (int(_maxVoices * 5.0f))
			{
				case 0: maxVoices = 1; break;
				case 1: maxVoices = 4; break;
				case 2: maxVoices = 8; break;
				case 3: maxVoices = 16; break;
				case 4: maxVoices = 24; break;
				case 5: maxVoices = 32; break;
				default: maxVoices = 32; break;
			}
			//
			reset();
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void startVoice(int xVoice, MidiMessage message)
	{
		value[xVoice] = 0.00000001f;
		position[xVoice] = envAttack;
		//
		if (*attackVal == 0.0f) rate[0][xVoice] = 1.0f; else rate[0][xVoice] = 1.0f / (sampleRate * 12.0f * (*attackVal * *attackVal));
		if (*decayVal == 0.0f) rate[1][xVoice] = -1.0f; else rate[1][xVoice] = -1.0f / (sampleRate * 12.0f * (*decayVal * *decayVal));
		//
		sustain[xVoice] = *sustainVal;
		velocity[xVoice] = float(message.getVelocity()) / 127.f;
		if (*velocityVal < 1.0f) velocity[xVoice] = (velocity[xVoice] * *velocityVal) + (1.0f - *velocityVal);
		//
		if (isLooped == 1.0f)
		{
			loopedRelease[xVoice] = 1.0f;
			loopedReleaseRate[xVoice] = 0.0f;
			if (sustain[xVoice] == 0.0f) sustain[xVoice] = 0.00000001f;
		}
		//
		isRelease[xVoice] = false;
		resetFilter[xVoice] = true;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void endVoice(int xVoice)
	{
		if (isLooped == 0.0f)
		{
			position[xVoice] = envRelease;
			if (*releaseVal == 0.0f) rate[1][xVoice] = -1.0f; else rate[1][xVoice] = -1.0f / (sampleRate * 12.0f * (*releaseVal * *releaseVal));
			sustain[xVoice] = 0.0f;
			isRelease[xVoice] = true;
			isStealingNote[xVoice] = false;
		}
		else
		{
			loopedRelease[xVoice] = 1.0f;
			if (*releaseVal == 0.0f) loopedReleaseRate[xVoice] = -1.0f; else loopedReleaseRate[xVoice] = -1.0f / (sampleRate * 12.0f * (*releaseVal * *releaseVal));
			isRelease[xVoice] = true;
			isStealingNote[xVoice] = false;
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void endVoiceMonoSteal(int xVoice)
	{
		if (isLooped == 0.0f)
		{
			position[xVoice] = envRelease;
			rate[1][xVoice] = -1.0f / (sampleRate * 0.004f); // Quick Release
			sustain[xVoice] = 0.0f;
			isRelease[xVoice] = true;
			isStealingNote[xVoice] = false;
		}
		else
		{
			loopedRelease[xVoice] = 1.0f;
			loopedReleaseRate[xVoice] = -1.0f / (sampleRate * 0.004f); // Quick Release
			isRelease[xVoice] = true;
			isStealingNote[xVoice] = false;
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	int noteOn(MidiMessage message, int forceVoice = -1)
	{
		isIdle = false;
		//
		if (maxVoices == 1) // MONOPHONIC //
		{
			if (holdingNotes.size() == 0) // First Note //
			{
				ADD_EXTRA_CONTROLLER(kEnv_FirstNote);
				//
				midiMessage[0] = message;
				startVoice(0, message);
				message.setTimeStamp(0.0);
				midiMessagesPlay.add(message);
				holdingNotes.addUsingDefaultSort(message.getNoteNumber());
				
			}
			else
			{
				ADD_EXTRA_CONTROLLER(kEnv_NotFirstNote);
				//
				holdingNotes.addUsingDefaultSort(message.getNoteNumber());
				message.setTimeStamp(0.0);
				midiMessagesPlay.add(message);
			}
			//
			return 0;
		}
		else // POLYPHONIC //
		{
			if (*unison > 0.0f)
			{
				// UNISON All Voices Mode //
				//
				ADD_EXTRA_CONTROLLER(kEnv_Reset_Unison);
				//
				if (holdingNotes.size() == 0) // First Note //
				{
					ADD_EXTRA_CONTROLLER(kEnv_FirstNote);
					holdingNotes.addUsingDefaultSort(message.getNoteNumber());
					//
					for (int xv = 0; xv < maxVoices; xv++)
					{
						message.setTimeStamp(double(xv));
						midiMessage[xv] = message;
						startVoice(xv, message);
						midiMessagesPlay.add(message);
					}
				}
				else
				{
					ADD_EXTRA_CONTROLLER(kEnv_NotFirstNote);
					holdingNotes.addUsingDefaultSort(message.getNoteNumber());
					//
					for (int xv = 0; xv < maxVoices; xv++)
					{
						message.setTimeStamp(double(xv));
						midiMessagesPlay.add(message);
					}
				}
				//
				return 0;
			}
			else
			{
				// NO UNISON POLY MODE //
				// First Find Voices That Are Not Playing //
				int xVoice = forceVoice;
				bool isStealingNow = false;
				holdingNotes.addUsingDefaultSort(message.getNoteNumber());
				//
				if (xVoice == -1)
				{
					for (int xenv = 0; xenv < maxVoices; xenv++)
					{
						if (isLooped == 0.0f)
						{
							if (position[xenv] == envRelease && value[xenv] == 0.0f)
							{
								xVoice = xenv;
								break;
							}
						}
						else
						{
							if (loopedRelease[xenv] == 0.0f)
							{
								xVoice = xenv;
								break;
							}
						}
					}
				}
				//
				if (xVoice == -1)
				{
					// No FREE voices were found, try to find the oldest voice that has no holding notes //
					int xCounterOldVoice = 2147483200;
					for (int xenv = 0; xenv < maxVoices; xenv++)
					{
						if (value[xenv] != 0.0f && isRelease[xenv])
						{
							if (keyCounter[xenv] <= xCounterOldVoice)
							{
								xCounterOldVoice = keyCounter[xenv];
								xVoice = xenv;
							}
						}
					}
					//
					if (xVoice == -1)
					{
						// No voices found, STEAL the oldest note //
						isStealingNow = true;
						xCounterOldVoice = 2147483200;
						for (int xenv = 0; xenv < maxVoices; xenv++)
						{
							if (value[xenv] != 0.0f && !isRelease[xenv])
							{
								if (keyCounter[xenv] <= xCounterOldVoice)
								{
									xCounterOldVoice = keyCounter[xenv];
									xVoice = xenv;
								}
							}
						}
					}
					//
					if (xVoice == -1) xVoice = 0;
				}
				//
				globalKeyCounter++;
				if (globalKeyCounter > 2147483200)
				{
					globalKeyCounter -= 2147483200;
					//
					for (int xenv = 0; xenv < maxVoices; xenv++)
					{
						keyCounter[xenv] -= 2147483200;
					}
				}
				//
				keyCounter[xVoice] = globalKeyCounter;
				voice[xVoice] = xVoice;
				midiMessage[xVoice] = message;
				//
				if (isStealingNow)
				{
					isStealingNote[xVoice] = true;
					position[xVoice] = envRelease;
					rate[1][xVoice] = -1.0f / (sampleRate * 0.004f); // Quick Release
					sustain[xVoice] = 0.0f;
					isRelease[xVoice] = true;
					hasStealingVoice = true;
				}
				else
				{
					if (holdingNotes.size() == 1)
					{
						ADD_EXTRA_CONTROLLER(kEnv_FirstNote);
					}
					else
					{
						ADD_EXTRA_CONTROLLER(kEnv_NotFirstNote);
					}
					//
					startVoice(xVoice, message);
					message.setTimeStamp(double(xVoice));
					midiMessagesPlay.add(message);
				}
				//
				return xVoice;
			}
		}
		//
		return 0;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void noteOff(MidiMessage message)
	{
		isIdle = false;
		//
		if (maxVoices == 1) // MONOPHONIC //
		{
			if (holdingNotes.size() == 1)
			{
				holdingNotes.clear();
				endVoice(0);
			}
			else
			{
				int note = message.getNoteNumber();
				holdingNotes.removeAllInstancesOf(note);
				if (note > holdingNotes.getFirst()) note = holdingNotes.getLast(); else note = holdingNotes.getFirst();
				//
				message.setTimeStamp(0.0);
				message.setNoteNumber(note);
				midiMessagesPlay.add(message);
			}
		}
		else // POLYPHONIC //
		{
			if (*unison > 0.0f)
			{
				// UNISON All Voices Mode //
				//
				if (holdingNotes.size() == 1)
				{
					for (int xv = 0; xv < maxVoices; xv++)
					{
						endVoice(xv);
					}
					holdingNotes.clear();
				}
				else
				{
					ADD_EXTRA_CONTROLLER(kEnv_Reset_Unison);
					//
					int note = message.getNoteNumber();
					holdingNotes.removeAllInstancesOf(note);
					if (note > holdingNotes.getFirst()) note = holdingNotes.getLast(); else note = holdingNotes.getFirst();
					//
					message.setNoteNumber(note);
					for (int xv = 0; xv < maxVoices; xv++)
					{
						message.setTimeStamp(double(xv));
						midiMessagesPlay.add(message);
					}
				}
			}
			else // POLY NO UNISON //
			{
				if (holdingNotes.size() == 1) holdingNotes.clear(); else holdingNotes.removeAllInstancesOf(message.getNoteNumber());
				//
				for (int xenv = 0; xenv < maxVoices; xenv++)
				{
					if (midiMessage[xenv].getNoteNumber() == message.getNoteNumber() && !isRelease[xenv] && value[xenv] != 0.0f)
					{
						endVoice(xenv);
					}
				}
			}
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void checkStealingVoices()
	{
		if (hasStealingVoice)
		{
			for (int xenv = 0; xenv < maxVoices; xenv++)
			{
				if (isStealingNote[xenv] && value[xenv] == 0.0f)
				{
					MidiMessage message = midiMessage[xenv];
					startVoice(xenv, message);
					isStealingNote[xenv] = false;
					message.setTimeStamp(double(xenv));
					midiMessagesPlay.add(message);
				}
			}
			//
			hasStealingVoice = false;
			for (int xenv = 0; xenv < maxVoices; xenv++)
			{
				if (isStealingNote[xenv])
				{
					hasStealingVoice = true;
					isIdle = false;
					break;
				}
			}
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	#define V(value) (value + voice)
	#define rateAttack V(rate[kRate_Attack])
	#define rateDecayRelease V(rate[kRate_DecayRelease])
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void startProcess(float *_velocityVal, float* _curve, float* _clip, float *_attackVal, float *_decayVal, float *_sustainVal, float *_releaseVal, float _sampleRate, float* _unison, float _isLooped)
	{
		unison = _unison;
		curve = _curve;
		clip = _clip;
		attackVal = _attackVal;
		sustainVal = _sustainVal;
		releaseVal = _releaseVal;
		decayVal = _decayVal;
		sampleRate = _sampleRate;
		velocityVal = _velocityVal;
		//
		if (prevUnison != *unison)
		{
			prevUnison = *unison;
			reset();
		}
		//
		if (isLooped != _isLooped)
		{
			isLooped = _isLooped;
			reset();
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	template <class T>
	forcedinline void processVoices(const int voices)
	{
		if (isLooped == 0.0f)
		{
			T::setZero(sumValues);
			//
			for (int voice = 0; voice < maxVoices; voice += voices)
			{
				T currentPosition = V(position);
				T currentValue = V(value);
				//
				currentValue.equalAdd(currentPosition, envAttack, rateAttack, rateDecayRelease);
				currentPosition.overIncrementThis(currentValue, 1.0f, 1.0f).write(V(position));
				//
				currentValue.boundsThis(currentPosition.equal(envRelease, V(sustain)), 1.0f).write(V(value));
				currentValue.multiply_MultAddThis(clip[0], 10.0f, 1.0f);
				currentValue.topThis(1.0f);
				//
				T::addVectorsAndWrite(currentValue, sumValues);
				//
				T curveV = curve[0];
				T theVelocity = V(velocity);
				T currValExp = (currentValue * currentValue);
				theVelocity.multiply_MultAddThis(currValExp * currValExp, curveV, currentValue * curveV.invert()).write(V(output));
			}
		}
		else
		{
			T::setZero(sumValues);
			//
			for (int voice = 0; voice < maxVoices; voice += voices)
			{
				T currentPosition = V(position);
				T currentValue = V(value);
				T xSustain = V(sustain);
				T xLoopedRelease = V(loopedRelease);
				//
				currentValue.equalAdd(currentPosition, envAttack, rateAttack, rateDecayRelease);
				currentValue.write(V(value));
				//
				for (int xx = 0; xx < voices; xx++)
				{
					if (value[voice + xx] >= 1.0f)
					{
						position[voice + xx] = envRelease;
						value[voice + xx] = 1.0f;
					}
					else if (value[voice + xx] <= sustain[voice + xx])
					{
						position[voice + xx] = envAttack;
						value[voice + xx] = sustain[voice + xx];
					}
				}
				//
				currentValue.load(V(value));
				currentPosition.load(V(position));
				//
				currentValue.boundsThis(currentPosition.equal(envRelease, xSustain), 1.0f).write(V(value));
				currentValue.multiply_MultAddThis(clip[0], 10.0f, 1.0f);
				currentValue.topThis(1.0f);
				currentValue *= xLoopedRelease;
				//
				T::addVectorsAndWrite(currentValue, sumValues);
				//
				xLoopedRelease += T(V(loopedReleaseRate));
				xLoopedRelease.bottomThis(0.0f);
				xLoopedRelease.write(V(loopedRelease));
				//
				T curveV = curve[0];
				T theVelocity = V(velocity);
				T currValExp = (currentValue * currentValue);
				theVelocity.multiply_MultAddThis(currValExp * currValExp, curveV, currentValue * curveV.invert()).write(V(output));
			}
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	forcedinline void checkIdle(int voices) // Always Do Last //
	{
		isIdle = true;
		//
		int xV = 0;
		while (isIdle && xV < voices)
		{
			if (sumValues[xV] > 0.0f)
			{
				isIdle = false;
				break;
			}
			xV++;
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	JUCE_ALIGN(32) float value[MAX_ENVELOPE_VOICES];
	JUCE_ALIGN(32) float sumValues[8];
	JUCE_ALIGN(32) float output[MAX_ENVELOPE_VOICES];
	JUCE_ALIGN(32) float loopedRelease[MAX_ENVELOPE_VOICES];
	JUCE_ALIGN(32) float loopedReleaseRate[MAX_ENVELOPE_VOICES];
	JUCE_ALIGN(32) float velocity[MAX_ENVELOPE_VOICES];
	JUCE_ALIGN(32) float sustain[MAX_ENVELOPE_VOICES];
	JUCE_ALIGN(32) float rate[2][MAX_ENVELOPE_VOICES]; // + = attack, - = decay & release, 0 = idle // rate[0] = attack, rate[1] = decay or = release
	JUCE_ALIGN(32) float position[MAX_ENVELOPE_VOICES]; // 0 = attack, 1 = decay, 2 = release
	MidiMessage midiMessage[MAX_ENVELOPE_VOICES];
	Array<MidiMessage> midiMessagesPlay;
	Array<int> holdingNotes;
	bool isRelease[MAX_ENVELOPE_VOICES];
	bool isStealingNote[MAX_ENVELOPE_VOICES];
	bool resetFilter[MAX_ENVELOPE_VOICES];
	int keyCounter[MAX_ENVELOPE_VOICES];
	int voice[MAX_ENVELOPE_VOICES];
	int globalKeyCounter;
	bool isMonophonic;
	bool hasStealingVoice;
	bool isIdle;
	float *curve, *clip, *attackVal, *decayVal, *sustainVal, *releaseVal, sampleRate, *velocityVal, prevFloatVoices, *unison, prevUnison, isLooped;
	int maxVoices;
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	enum
	{
		kRate_Attack,
		kRate_DecayRelease = 1
	};
};