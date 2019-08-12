/*
BEGIN_JUCE_MODULE_DECLARATION
    ID:               WusikWavetable
    vendor:           Wusik Dot Com
    version:          1.0.0
    name:             WusikWavetable
    description:      Wusik Wavetable
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
#define MAX_WAVETABLE_VOICES 32
#define WAVE_SAMPLE_LENGTH 1200
#define WAVETABLE_SIZE_SIMD 1200.0f
#define MAX_OVERSAMPLE 64
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WusikWaveform
{
public:
	DECLARE_ALIGNED(32);
	//
	WusikWaveform() 
	{ 
		tune = 0;
		fine = 0.0f;
		zeromem(wavetable, sizeof(wavetable));
	};
	//
	Image waveform;
	JUCE_ALIGN(32) float wavetable[WAVE_SAMPLE_LENGTH + 12];
	char tune;
	float fine;
};
//
// -------------------------------------------------------------------------------------------------------------------------------
class WusikWavetable
{
public:
	DECLARE_ALIGNED(32);
	//
	class WusikWavetableObject
	{
	public:
		// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		WusikWavetableObject(WusikWavetable& _owner, float* _initialWaves, bool _addThisLayer, float* _pitchEnvelopeValue, float _boostVolume) : owner(_owner),
			rateUpdateCounter(0), pitchEnvelopeValue(_pitchEnvelopeValue), addThisLayerSignal(_addThisLayer), boostVolume(_boostVolume)
		{ 
			previousSampleRate = 0.0f;
			isSequenced = false;
			zeromem(filterCoeficients, sizeof(filterCoeficients));
			memset(panL, 1.0f, sizeof(panL));
			memset(panR, 1.0f, sizeof(panR));
			//
			for (int xv = 0; xv < MAX_WAVETABLE_VOICES; xv++)
			{
				waves[xv] = _initialWaves;
			}
			//
			reset(); 
		};
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		void reset()
		{
			for (int xv = 0; xv < MAX_WAVETABLE_VOICES; xv++)
			{
				fade[xv] = 0.0f;
				fadeRate[xv] = 0.0f;
				currentRate[xv] = 0.0f;
				targetRate[xv] = 0.0f;
				volume[xv] = 0.0f;
				position[xv] = 0.0f;
				note[xv] = 60;
				fine[xv] = 0.5f;
				fineUnison[xv] = 0.5f;
				samples[0][xv] = 0.0f;
				samples[1][xv] = 0.0f;
				samples[2][xv] = 0.0f;
				samples[3][xv] = 0.0f;
				rateFrequency[xv] = 100.0f;
				//
				for (int x = 0; x < MAX_OVERSAMPLE; x++)
				{
					output[kLeft][x][xv] = 0.0f;
					output[kRight][x][xv] = 0.0f;
				}
				//
				filterBuffers[0][0][xv] = filterBuffers[0][1][xv] = filterBuffers[0][2][xv] = 0.0f;
				filterBuffers[1][0][xv] = filterBuffers[1][1][xv] = filterBuffers[1][2][xv] = 0.0f;
				filterBuffers[0][3][xv] = filterBuffers[0][4][xv] = filterBuffers[0][5][xv] = 0.0f;
				filterBuffers[1][3][xv] = filterBuffers[1][4][xv] = filterBuffers[1][5][xv] = 0.0f;
			}
		}
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		forcedinline void updateRate(float* pitchEnvelope)
		{
			if (rateUpdateCounter == 0)
			{
				for (int voice = 0; voice < owner.maxVoices; voice++)
				{
					lastPitchEnvelope[voice] = pitchEnvelope[voice];
					updateRate(voice);
				}
			}
			//
			rateUpdateCounter++;
			if (rateUpdateCounter > 100) rateUpdateCounter = 0;
		}
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		forcedinline void updateRate(int voice)
		{
			double xSampRate = owner.sampleRate * owner.overSample;
			double cyclesPerSecond = MidiMessage::getMidiNoteInHertz(note[voice]);
			//
			double fineTuneCycles = 0;
			double xFine = juce::jlimit(-1.0, 1.0, ((fine[voice] * 2.0) - 1.0) + ((fineUnison[voice] * 2.0) - 1.0) + ((*owner.fineTune * 2.0) - 1.0));
			if (xFine > 0) fineTuneCycles = xFine * (MidiMessage::getMidiNoteInHertz(note[voice] + 1) - cyclesPerSecond);
			else if (xFine < 0) fineTuneCycles = xFine * (cyclesPerSecond - MidiMessage::getMidiNoteInHertz(note[voice] - 1));
			//
			double pitchBendCycles = 0;
			if (*owner.pitchBend != 0.0f && *owner.pitchBendValue != 0.0f)
			{
				if (*owner.pitchBendValue > 0.0f) pitchBendCycles = *owner.pitchBendValue * (MidiMessage::getMidiNoteInHertz(note[voice] + int(*owner.pitchBend * 48.0f)) - cyclesPerSecond);
				else pitchBendCycles = *owner.pitchBendValue * (cyclesPerSecond - MidiMessage::getMidiNoteInHertz(note[voice] - int(*owner.pitchBend * 48.0f)));
			}
			//
			double envelopeCycles = 0;
			if (*pitchEnvelopeValue != 0.5f && lastPitchEnvelope[voice] != 0.0f)
			{
				if (*pitchEnvelopeValue > 0.0f) envelopeCycles = lastPitchEnvelope[voice] * (MidiMessage::getMidiNoteInHertz(note[voice] + int((*pitchEnvelopeValue * 96.0f) - 48.0f)) - cyclesPerSecond);
				else envelopeCycles = lastPitchEnvelope[voice] * (cyclesPerSecond - MidiMessage::getMidiNoteInHertz(note[voice] - int((*pitchEnvelopeValue * 96.0f) - 48.0f)));
			}
			//
			rateFrequency[voice] = cyclesPerSecond + fineTuneCycles + envelopeCycles + pitchBendCycles;
			targetRate[voice] = ((rateFrequency[voice]) / xSampRate) * WAVETABLE_SIZE_SIMD;
			if (targetRate[voice] > WAVETABLE_SIZE_SIMD) targetRate[voice] = WAVETABLE_SIZE_SIMD * 0.5f;
			if (targetRate[voice] < 0.0f) targetRate[voice] = 0.0f;
			//
			if (*owner.glide == 0.0f) copyGlideRate(voice);
		}
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		void copyGlideRate(int voice)
		{
			currentRate[voice] = targetRate[voice];
		}
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		void changeWaveform(int voice, MidiMessage message, float _volume, float pan, float* _waves, float _fine, float initialPosition, bool reStart, float _unisonFine)
		{
			fine[voice] = _fine;
			fineUnison[voice] = _unisonFine;
			volume[voice] = _volume;
			waves[voice] = _waves;
			if (reStart)
			{
				fade[voice] = 1.0f;
				if (fadeRate[voice] < 1.0f) fade[voice] = 0.0f;
				position[voice] = initialPosition * (WAVETABLE_SIZE_SIMD - 1.0f);
			}
			note[voice] = message.getNoteNumber();
			if (message.isNoteOn() && message.getFloatVelocity() > 0.0f) lastVelocity[voice] = message.getFloatVelocity();
			//
			panR[voice] = jlimit(0.0f, 1.0f, pan);
			panL[voice] = jlimit(0.0f, 1.0f, 1.0f - pan);
			//
			if (!isSequenced)
			{
				fade[voice] = 1.0f; 
				fadeRate[voice] = 0.0f;
			}
			//
			updateRate(voice);
		}
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		forcedinline void processVoices_SSE()
		{
			for (int voice = 0; voice < owner.maxVoices; voice += 4)
			{
				if (*owner.glide > 0.0f)
				{
					__m128 smoothBufferTemp = _mm_load_ps(currentRate + voice);
					__m128 smoothBuffer = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_load_ps(targetRate + voice),
						smoothBufferTemp), _mm_set1_ps(owner.smooth_frequency)), smoothBufferTemp);
					_mm_store_ps(currentRate + voice, smoothBuffer);
				}
				//
				for (int x = 0; x < owner.overSample; x++)
				{
					JUCE_ALIGN(16) uint32_t posInt[4];
					__m128i xPosInt = _mm_cvtps_epi32(_mm_load_ps(position + voice));
					__m128 sampInterpolation = _mm_sub_ps(_mm_load_ps(position + voice), _mm_cvtepi32_ps(xPosInt));
					_mm_store_si128((__m128i *) posInt, xPosInt);
					//
					#define v0 _mm_load_ps(waves[voice + 0] + posInt[0])
					#define v1 _mm_load_ps(waves[voice + 1] + posInt[1])
					#define v2 _mm_load_ps(waves[voice + 2] + posInt[2])
					#define v3 _mm_load_ps(waves[voice + 3] + posInt[3])
					//
					__m128 v01 = _mm_movelh_ps(v0, v1); // V0S0 V0S1 V1S0 V1S1
					__m128 v23 = _mm_movelh_ps(v2, v3); // V2S0 V2S1 V3S0 V3S1
					//
					_mm_store_ps(samples[0] + voice, _mm_shuffle_ps(v01, v23, _MM_SHUFFLE(2, 0, 2, 0)));
					_mm_store_ps(samples[1] + voice, _mm_shuffle_ps(v01, v23, _MM_SHUFFLE(3, 1, 3, 1)));
					//
					__m128 v01l = _mm_movehl_ps(v1, v0); // V0S2 V0S3 V1S2 V1S3
					__m128 v23l = _mm_movehl_ps(v3, v2); // V2S2 V2S3 V3S2 V3S3
					//
					_mm_store_ps(samples[2] + voice, _mm_shuffle_ps(v01l, v23l, _MM_SHUFFLE(2, 0, 2, 0)));
					_mm_store_ps(samples[3] + voice, _mm_shuffle_ps(v01l, v23l, _MM_SHUFFLE(3, 1, 3, 1)));
					//
					// Hermite Interpolation (4 point) //
					//
					__m128 y0 = _mm_load_ps(samples[0] + voice);
					__m128 y1 = _mm_load_ps(samples[1] + voice);
					__m128 y2 = _mm_load_ps(samples[2] + voice);
					//
					__m128 c1 = _mm_mul_ps(_mm_set1_ps(0.5f), _mm_sub_ps(y2, y0));
					__m128 c3 = _mm_add_ps(_mm_mul_ps(_mm_set1_ps(1.5f), _mm_sub_ps(y1, y2)),
								_mm_mul_ps(_mm_set1_ps(0.5f), _mm_sub_ps(_mm_load_ps(samples[3] + voice), y0)));
					__m128 c2 = _mm_sub_ps(_mm_sub_ps(y0, _mm_add_ps(y1, c1)), c3);
					__m128 hermi = _mm_add_ps(_mm_mul_ps(_mm_add_ps(_mm_mul_ps(_mm_add_ps(_mm_mul_ps(c3, sampInterpolation), c2), sampInterpolation), c1), sampInterpolation), y1);
					//
					__m128 signalMono = _mm_mul_ps(hermi, _mm_mul_ps(_mm_load_ps(volume + voice), _mm_load_ps(fade + voice)));
					//
					_mm_store_ps(output[kLeft][x] + voice, _mm_mul_ps(signalMono, _mm_load_ps(panL + voice)));
					_mm_store_ps(output[kRight][x] + voice, _mm_mul_ps(signalMono, _mm_load_ps(panR + voice)));
					//
					__m128 pos = _mm_load_ps(position + voice);
					__m128 sizeWave = _mm_set1_ps(WAVETABLE_SIZE_SIMD - 1.0f);
					//
					pos = _mm_add_ps(pos, _mm_load_ps(currentRate + voice));
					pos = _mm_sub_ps(pos, _mm_and_ps(_mm_cmpge_ps(pos, sizeWave), sizeWave));
					_mm_store_ps(position + voice, pos);
				}
				//
				if (isSequenced) processSequencedVoice<wSSE>(voice);
			}
		}
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		#if JUCE_WINDOWS
		forcedinline void processVoices_AVX()
		{
			for (int voice = 0; voice < owner.maxVoices; voice += 8)
			{
				if (*owner.glide > 0.0f)
				{
					__m256 smoothBufferTemp = _mm256_load_ps(currentRate + voice);
					__m256 smoothBuffer = _mm256_add_ps(_mm256_mul_ps(_mm256_sub_ps(_mm256_load_ps(targetRate + voice),
						smoothBufferTemp), _mm256_set1_ps(owner.smooth_frequency)), smoothBufferTemp);
					_mm256_store_ps(currentRate + voice, smoothBuffer);
				}
				//
				for (int x = 0; x < owner.overSample; x++)
				{
					JUCE_ALIGN(32)  uint32_t posInt[8];
					__m256i xPosInt = _mm256_cvtps_epi32(_mm256_load_ps(position + voice));
					__m256 sampInterpolation = _mm256_sub_ps(_mm256_load_ps(position + voice), _mm256_cvtepi32_ps(xPosInt));
					_mm256_store_si256((__m256i *) posInt, xPosInt);
					//
					#define v0x _mm_load_ps(waves[voice + 0] + posInt[0])
					#define v1x _mm_load_ps(waves[voice + 1] + posInt[1])
					#define v2x _mm_load_ps(waves[voice + 2] + posInt[2])
					#define v3x _mm_load_ps(waves[voice + 3] + posInt[3])
					#define v4x _mm_load_ps(waves[voice + 4] + posInt[4])
					#define v5x _mm_load_ps(waves[voice + 5] + posInt[5])
					#define v6x _mm_load_ps(waves[voice + 6] + posInt[6])
					#define v7x _mm_load_ps(waves[voice + 7] + posInt[7])
					//
					__m128 v01 = _mm_movelh_ps(v0x, v1x); // V0S0 V0S1 V1S0 V1S1
					__m128 v23 = _mm_movelh_ps(v2x, v3x); // V2S0 V2S1 V3S0 V3S1
					__m128 v45 = _mm_movelh_ps(v4x, v5x);
					__m128 v67 = _mm_movelh_ps(v6x, v7x);
					//
					_mm_store_ps(samples[0] + voice, _mm_shuffle_ps(v01, v23, _MM_SHUFFLE(2, 0, 2, 0)));
					_mm_store_ps(samples[1] + voice, _mm_shuffle_ps(v01, v23, _MM_SHUFFLE(3, 1, 3, 1)));
					//
					_mm_store_ps(samples[0] + voice + 4, _mm_shuffle_ps(v45, v67, _MM_SHUFFLE(2, 0, 2, 0)));
					_mm_store_ps(samples[1] + voice + 4, _mm_shuffle_ps(v45, v67, _MM_SHUFFLE(3, 1, 3, 1)));
					//
					__m128 v01l = _mm_movehl_ps(v1x, v0x);
					__m128 v23l = _mm_movehl_ps(v3x, v2x);
					__m128 v45l = _mm_movehl_ps(v5x, v4x);
					__m128 v67l = _mm_movehl_ps(v7x, v6x);
					//
					_mm_store_ps(samples[2] + voice, _mm_shuffle_ps(v01l, v23l, _MM_SHUFFLE(2, 0, 2, 0)));
					_mm_store_ps(samples[3] + voice, _mm_shuffle_ps(v01l, v23l, _MM_SHUFFLE(3, 1, 3, 1)));
					//
					_mm_store_ps(samples[2] + voice + 4, _mm_shuffle_ps(v45l, v67l, _MM_SHUFFLE(2, 0, 2, 0)));
					_mm_store_ps(samples[3] + voice + 4, _mm_shuffle_ps(v45l, v67l, _MM_SHUFFLE(3, 1, 3, 1)));
					//
					// Hermite Interpolation (4 point) //
					//
					__m256 y0 = _mm256_load_ps(samples[0] + voice);
					__m256 y1 = _mm256_load_ps(samples[1] + voice);
					__m256 y2 = _mm256_load_ps(samples[2] + voice);
					//
					__m256 c1 = _mm256_mul_ps(_mm256_set1_ps(0.5f), _mm256_sub_ps(y2, y0));
					__m256 c3 = _mm256_fmadd_ps(_mm256_set1_ps(1.5f), _mm256_sub_ps(y1, y2),
						_mm256_mul_ps(_mm256_set1_ps(0.5f), _mm256_sub_ps(_mm256_load_ps(samples[3] + voice), y0)));
					__m256 c2 = _mm256_sub_ps(_mm256_sub_ps(y0, _mm256_add_ps(y1, c1)), c3);
					__m256 hermi = _mm256_fmadd_ps(_mm256_fmadd_ps(_mm256_fmadd_ps(c3, sampInterpolation, c2), sampInterpolation, c1), sampInterpolation, y1);
					//
					__m256 signalMono = _mm256_mul_ps(hermi, _mm256_mul_ps(_mm256_load_ps(volume + voice), _mm256_load_ps(fade + voice)));
					//
					_mm256_store_ps(output[kLeft][x] + voice, _mm256_mul_ps(signalMono, _mm256_load_ps(panL + voice)));
					_mm256_store_ps(output[kRight][x] + voice, _mm256_mul_ps(signalMono, _mm256_load_ps(panR + voice)));
					//
					__m256 pos = _mm256_load_ps(position + voice);
					__m256 sizeWave = _mm256_set1_ps(WAVETABLE_SIZE_SIMD - 1.0f);
					//
					pos = _mm256_add_ps(pos, _mm256_load_ps(currentRate + voice));
					pos = _mm256_sub_ps(pos, _mm256_and_ps(_mm256_cmp_ps(pos, sizeWave, _CMP_GE_OQ), sizeWave));
					_mm256_store_ps(position + voice, pos);
				}
				//
				if (isSequenced) processSequencedVoice<wAVXFMA3>(voice);
			}
		}
		#endif
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		#define V(value) (value + voice)
		template <class T>
		forcedinline T filterProcess(const T xInput, const int voice, const int pole)
		{
			T xOut = T::multAdd(filterCoeficients[0], xInput, V(filterBuffers[0][pole]));
			T(T::multSubs(filterCoeficients[1], xInput, T(filterCoeficients[3]) * xOut) + V(filterBuffers[1][pole])).write(V(filterBuffers[0][pole]));
			T::multSubs(filterCoeficients[2], xInput, (T(filterCoeficients[4]) * xOut)).write(V(filterBuffers[1][pole]));
			return xOut;
		}
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		/*forcedinline float hermite(float x, float y0, float y1, float y2, float y3)
		{
			// 4-point, 3rd-order Hermite (x-form)
			float c0 = y1;
			float c1 = 0.5f * (y2 - y0);
			float c3 = (1.5f * (y1 - y2)) + (0.5f * (y3 - y0));
			float c2 = (y0 - (y1 + c1)) - c3;
			//
			return (((((c3 * x) + c2) * x) + c1) * x) + c0;
		}*/
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		template <class T>
		forcedinline void downSample(const int voice, float* theInputL, float* theInputR)
		{
			if (previousSampleRate != owner.sampleRate)
			{
				// ===================================== //
				// Set Filter's Coeficients for Low Pass //
				// ===================================== //
				//
				previousSampleRate = owner.sampleRate;
				float filterFrequency = 22000.0f;
				float n = float(1.0) / tan(double_Pi * jmax(100.0f, filterFrequency) / (owner.sampleRate * float(owner.overSample)));
				float nSquared = n * n;
				filterCoeficients[0] = float(1.0) / (float(1.0) + std::sqrt(float(2.0)) * n + nSquared);
				filterCoeficients[2] = filterCoeficients[0];
				filterCoeficients[1] = filterCoeficients[0] * float(2.0);
				filterCoeficients[3] = filterCoeficients[1] * (float(1.0) - nSquared);
				filterCoeficients[4] = filterCoeficients[0] * (float(1.0) - std::sqrt(float(2.0)) * n + nSquared);
			}
			//
			T xOutL = filterProcess(T(V(theInputL)), voice, pole1L);
			T xOutL2 = filterProcess(xOutL, voice, pole2L);
			T xOutL3 = filterProcess(xOutL2, voice, pole3L);
			//
			T xOutR = filterProcess(T(V(theInputR)), voice, pole1R);
			T xOutR2 = filterProcess(xOutR, voice, pole2R);
			T xOutR3 = filterProcess(xOutR2, voice, pole3R);
			//
			xOutL3.write(V(theInputL));
			xOutR3.write(V(theInputR));
		}
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		template <class T>
		forcedinline void processSequencedVoice(const int voice)
		{
			T Fade(V(fade));
			Fade += T(V(fadeRate));
			Fade.write(V(fade)); // this was the fade value can go < 0 and > 1
			Fade.boundsThisZeroOne(); // but the value used for the multiplication is bounder to 0 to +1
			Fade.write(V(fade));
		}
		//
		// -------------------------------------------------------------------------------------------------------------------------------
		//
		JUCE_ALIGN(32) float fade[MAX_WAVETABLE_VOICES];
		JUCE_ALIGN(32) float fadeRate[MAX_WAVETABLE_VOICES];
		JUCE_ALIGN(32) float currentRate[MAX_WAVETABLE_VOICES];
		JUCE_ALIGN(32) float targetRate[MAX_WAVETABLE_VOICES];
		JUCE_ALIGN(32) float volume[MAX_WAVETABLE_VOICES];
		JUCE_ALIGN(32) float output[2][MAX_OVERSAMPLE][MAX_WAVETABLE_VOICES];
		JUCE_ALIGN(32) float panL[MAX_WAVETABLE_VOICES];
		JUCE_ALIGN(32) float panR[MAX_WAVETABLE_VOICES];
		JUCE_ALIGN(32) float position[MAX_WAVETABLE_VOICES];
		JUCE_ALIGN(32) float fine[MAX_WAVETABLE_VOICES];
		JUCE_ALIGN(32) float fineUnison[MAX_WAVETABLE_VOICES];
		JUCE_ALIGN(32) float samples[4][MAX_WAVETABLE_VOICES];
		float rateFrequency[MAX_WAVETABLE_VOICES];
		float lastPitchEnvelope[MAX_WAVETABLE_VOICES];
		int note[MAX_WAVETABLE_VOICES];
		float* pitchEnvelopeValue;
		float* waves[MAX_WAVETABLE_VOICES];
		int rateUpdateCounter;
		float lastVelocity[MAX_WAVETABLE_VOICES];
		bool isSequenced;
		bool addThisLayerSignal;
		float boostVolume;
		float previousSampleRate;
		//
		// Low Pass Filter
		JUCE_ALIGN(32) float filterBuffers[2][3 * 2][MAX_WAVETABLE_VOICES]; // 3 POLES 2 CHANNELS
		JUCE_ALIGN(32) float filterCoeficients[5]; // Coeficients
		//
		WusikWavetable& owner;
	};
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------------------------------
	// -------------------------------------------------------------------------------------------------------------------------------
	//
	WusikWavetable() : sampleRate(44100.0f), overSample(8), maxVoices(MAX_WAVETABLE_VOICES), smooth_frequency(0.0f)
	{ 
		reset(); 
	};
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	~WusikWavetable()
	{
		wavetableObjects.clear();
	}
	//
	// ------------------------------------------------------------------------------------------------------------------------------- 
	void startProcess(float _sampleRate, float *_fineTune, float *_pitchBend, float *_pitchBendValue, float _overSample, int _maxVoices, float* _glide)
	{
		if (sampleRate != _sampleRate)
		{
			sampleRate = _sampleRate;
			reset();
		}
		//
		if (maxVoices != _maxVoices)
		{
			maxVoices = _maxVoices;
			reset();
		}
		//
		glide = _glide;
		fineTune = _fineTune;
		pitchBend = _pitchBend;
		pitchBendValue = _pitchBendValue;
		//
		switch (int(_overSample * 3.0f))
		{
			case 0: overSample = 8; break;
			case 1: overSample = 16; break;
			case 2: overSample = 32; break;
			case 3: overSample = 64; break;
		}
		//
		if (*glide > 0.0f)
		{
			double theFreq = 1.0f - ((0.9 + (double(*glide) * 0.1)) - 0.001);
			smooth_frequency = float((theFreq * theFreq) * (6.28319 / double(sampleRate)) * 8000.0);
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void reset()
	{
		ARRAY_Iterator(wavetableObjects)
		{
			wavetableObjects[index]->reset();
		}
	};
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	float sampleRate, *fineTune, *pitchBend, *pitchBendValue, *glide, smooth_frequency;
	OwnedArray<WusikWavetableObject> wavetableObjects;
	int overSample, maxVoices;
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	enum
	{
		pole1L, pole2L, pole3L,
		pole1R, pole2R, pole3R,
		kLeft = 0, kRight
	};
};