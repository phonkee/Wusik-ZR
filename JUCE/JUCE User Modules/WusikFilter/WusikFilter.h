/*
BEGIN_JUCE_MODULE_DECLARATION
    ID:               WusikFilter
    vendor:           Wusik Dot Com
    version:          1.0.0
    name:             WusikFilter
    description:      Wusik Filter
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
#define MAX_VOICES 32
//
//=========================================================================
class WusikFilter
{
public:
	DECLARE_ALIGNED(32)
	//
	WusikFilter() { init(); };
	//
	void process_SSE(float* input, int xvoice)
	{
		__m128 smoothBufferTemp = _mm_load_ps(smooth_dBuffer + xvoice);
		__m128 smoothBuffer = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_load_ps(Frequency + xvoice),
			smoothBufferTemp), _mm_set1_ps(smooth_frequency)), smoothBufferTemp);
		_mm_store_ps(smooth_dBuffer + xvoice, smoothBuffer);
		//
		__m128 buff0 = _mm_load_ps(dBuffer0 + xvoice);
		__m128 dinput = _mm_load_ps(input + xvoice);
		__m128 orgInput = dinput;
		//
		__m128 muldrive = _mm_mul_ps(_mm_load_ps(drive + xvoice), _mm_set1_ps(Rezonance));
		__m128 drv = _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(muldrive, buff0), buff0), buff0);
		__m128 notchOut = _mm_sub_ps(dinput, _mm_mul_ps(buff0, _mm_load_ps(damp + xvoice)));
		__m128 lowOut = _mm_add_ps(_mm_load_ps(dBuffer1 + xvoice), _mm_mul_ps(buff0, smoothBuffer));
		__m128 highOut = _mm_sub_ps(notchOut, lowOut);
		__m128 bandOut = _mm_sub_ps(_mm_add_ps(_mm_mul_ps(highOut, smoothBuffer), buff0), drv);
		//
		bandOut = _mm_min_ps(_mm_set1_ps(4.9f), _mm_max_ps(_mm_set1_ps(-4.9f), bandOut));
		lowOut = _mm_min_ps(_mm_set1_ps(4.9f), _mm_max_ps(_mm_set1_ps(-4.9f), lowOut));
		//
		_mm_store_ps(dBuffer0 + xvoice, bandOut);
		_mm_store_ps(dBuffer1 + xvoice, lowOut);
		//
		if (Pole == 0)
		{
			switch (Type)
			{
				case 0: _mm_store_ps(input + xvoice, lowOut); break;
				case 1: _mm_store_ps(input + xvoice, bandOut); break;
				case 2: _mm_store_ps(input + xvoice, highOut); break;
				case 3: _mm_store_ps(input + xvoice, notchOut); break;
			}
		}
		else
		{
			switch (Type)
			{
				case 0: dinput = lowOut; break;
				case 1: dinput = bandOut; break;
				case 2: dinput = highOut; break;
				case 3: dinput = notchOut; break;
			}
			//
			__m128 buff2 = _mm_load_ps(dBuffer2 + xvoice);
			//
			drv = _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(muldrive, buff2), buff2), buff2);
			__m128 notchOut2 = _mm_sub_ps(dinput, _mm_mul_ps(buff2, _mm_load_ps(damp + xvoice)));
			__m128 lowOut2 = _mm_add_ps(_mm_load_ps(dBuffer3 + xvoice), _mm_mul_ps(buff2, smoothBuffer));
			__m128 highOut2 = _mm_sub_ps(notchOut2, lowOut2);
			__m128 bandOut2 = _mm_sub_ps(_mm_add_ps(_mm_mul_ps(highOut2, smoothBuffer), buff2), drv);
			//
			bandOut2 = _mm_min_ps(_mm_set1_ps(4.9f), _mm_max_ps(_mm_set1_ps(-4.9f), bandOut2));
			lowOut2 = _mm_min_ps(_mm_set1_ps(4.9f), _mm_max_ps(_mm_set1_ps(-4.9f), lowOut2));
			//
			_mm_store_ps(dBuffer2 + xvoice, bandOut2);
			_mm_store_ps(dBuffer3 + xvoice, lowOut2);
			//
			switch (Type)
			{
				case 0: _mm_store_ps(input + xvoice, lowOut2); break;
				case 1: _mm_store_ps(input + xvoice, bandOut2); break;
				case 2: _mm_store_ps(input + xvoice, highOut2); break;
				case 3: _mm_store_ps(input + xvoice, notchOut2); break;
			}
		}
	};
	//
	#if JUCE_WINDOWS
	void process_AVX(float* input, int xvoice)
	{
		__m256 smoothBufferTemp = _mm256_load_ps(smooth_dBuffer + xvoice);
		__m256 smoothBuffer = _mm256_add_ps(_mm256_mul_ps(_mm256_sub_ps(_mm256_load_ps(Frequency + xvoice),
			smoothBufferTemp), _mm256_set1_ps(smooth_frequency)), smoothBufferTemp);
		_mm256_store_ps(smooth_dBuffer + xvoice, smoothBuffer);
		//
		__m256 buff0 = _mm256_load_ps(dBuffer0 + xvoice);
		__m256 dinput = _mm256_load_ps(input + xvoice);
		__m256 orgInput = dinput;
		//
		__m256 muldrive = _mm256_mul_ps(_mm256_load_ps(drive + xvoice), _mm256_set1_ps(Rezonance));
		__m256 drv = _mm256_mul_ps(_mm256_mul_ps(_mm256_mul_ps(muldrive, buff0), buff0), buff0);
		__m256 notchOut = _mm256_sub_ps(dinput, _mm256_mul_ps(buff0, _mm256_load_ps(damp + xvoice)));
		__m256 lowOut = _mm256_add_ps(_mm256_load_ps(dBuffer1 + xvoice), _mm256_mul_ps(buff0, smoothBuffer));
		__m256 highOut = _mm256_sub_ps(notchOut, lowOut);
		__m256 bandOut = _mm256_sub_ps(_mm256_add_ps(_mm256_mul_ps(highOut, smoothBuffer), buff0), drv);
		//
		bandOut = _mm256_min_ps(_mm256_set1_ps(4.9f), _mm256_max_ps(_mm256_set1_ps(-4.9f), bandOut));
		lowOut = _mm256_min_ps(_mm256_set1_ps(4.9f), _mm256_max_ps(_mm256_set1_ps(-4.9f), lowOut));
		//
		_mm256_store_ps(dBuffer0 + xvoice, bandOut);
		_mm256_store_ps(dBuffer1 + xvoice, lowOut);
		//
		if (Pole == 0)
		{
			switch (Type)
			{
				case 0: _mm256_store_ps(input + xvoice, lowOut); break;
				case 1: _mm256_store_ps(input + xvoice, bandOut); break;
				case 2: _mm256_store_ps(input + xvoice, highOut); break;
				case 3: _mm256_store_ps(input + xvoice, notchOut); break;
			}
		}
		else
		{
			switch (Type)
			{
				case 0: dinput = lowOut; break;
				case 1: dinput = bandOut; break;
				case 2: dinput = highOut; break;
				case 3: dinput = notchOut; break;
			}
			//
			__m256 buff2 = _mm256_load_ps(dBuffer2 + xvoice);
			//
			drv = _mm256_mul_ps(_mm256_mul_ps(_mm256_mul_ps(muldrive, buff2), buff2), buff2);
			__m256 notchOut2 = _mm256_sub_ps(dinput, _mm256_mul_ps(buff2, _mm256_load_ps(damp + xvoice)));
			__m256 lowOut2 = _mm256_add_ps(_mm256_load_ps(dBuffer3 + xvoice), _mm256_mul_ps(buff2, smoothBuffer));
			__m256 highOut2 = _mm256_sub_ps(notchOut2, lowOut2);
			__m256 bandOut2 = _mm256_sub_ps(_mm256_add_ps(_mm256_mul_ps(highOut2, smoothBuffer), buff2), drv);
			//
			bandOut2 = _mm256_min_ps(_mm256_set1_ps(4.9f), _mm256_max_ps(_mm256_set1_ps(-4.9f), bandOut2));
			lowOut2 = _mm256_min_ps(_mm256_set1_ps(4.9f), _mm256_max_ps(_mm256_set1_ps(-4.9f), lowOut2));
			//
			_mm256_store_ps(dBuffer2 + xvoice, bandOut2);
			_mm256_store_ps(dBuffer3 + xvoice, lowOut2);
			//
			switch (Type)
			{
				case 0: _mm256_store_ps(input + xvoice, lowOut2); break;
				case 1: _mm256_store_ps(input + xvoice, bandOut2); break;
				case 2: _mm256_store_ps(input + xvoice, highOut2); break;
				case 3: _mm256_store_ps(input + xvoice, notchOut2); break;
			}
		}
	};
	#endif
	//
	void setFrequency(float aFrequency, int32 voice)
	{
		Frequency[voice] = (jmin(0.125f, jmax(0.0f,
			((abs(aFrequency * aFrequency) * 10945.0f) + 80.0f) / (SampleRateFloat * 2.0f)))) * 3.1415926f;
		Frequency[voice] = ((((Frequency[voice] * -1.0f) * Frequency[voice]) / 6.0f) + Frequency[voice]) * 2.0f;
		if (!doSmooth) smooth_dBuffer[voice] = Frequency[voice];
		//
		float xFreqx = jmax(0.0001f, Frequency[voice]);
		float ddamp = (2.0f / xFreqx) - (xFreqx * 0.5f);
		damp[voice] = jmin(Rezonance, (jmin(2.0f, ddamp)));
	};
	//
	void setRezonance(float aRezonance, int voices)
	{
		Rezonance = jmax(0.0f, (1.0f - float(pow(((jmax(0.0f, aRezonance) * 0.999f) + 0.001f), 0.25f))) * 2.0f);
		//
		for (int voice = 0; voice < voices; voice++)
		{
			float xFreqx = jmax(0.0001f, Frequency[voice]);
			float ddamp = (2.0f / xFreqx) - (xFreqx * 0.5f);
			damp[voice] = jmin(Rezonance, (jmin(2.0f, ddamp)));
		}
	};
	//
	void setPole(int aPole) { Pole = aPole; };
	//
	void setType(int aType) { Type = aType; };
	//
	void setLimit(float value)
	{
		drive[0] = drive[1] = drive[2] = drive[3] = ((value*value) * 0.2f) + 0.002f;
	}
	//
	void init()
	{
		for (int xv = 0; xv < MAX_VOICES; xv++)
		{
			drive[xv] = float(0.002f);
			Frequency[xv] = 0.001f;
		}
		doSmooth = false;
		reset();
		Rezonance = float(1.0);
		Pole = 0;
		Type = 0;
		smooth_frequency = float(1.0);
		SampleRateFloat = 44100.0f;
	};
	//
	void reset()
	{
		for (int xv = 0; xv < MAX_VOICES; xv++)
		{
			smooth_dBuffer[xv] = Frequency[xv];
		}
		//
		zeromem(dBuffer0, sizeof(dBuffer0));
		zeromem(dBuffer1, sizeof(dBuffer1));
		zeromem(dBuffer2, sizeof(dBuffer2));
		zeromem(dBuffer3, sizeof(dBuffer3));
	};
	//
	void reset(int voice)
	{
		smooth_dBuffer[voice] = Frequency[voice];
		dBuffer0[voice] = 0.0f;
		dBuffer1[voice] = 0.0f;
		dBuffer2[voice] = 0.0f;
		dBuffer3[voice] = 0.0f;
	}
	//
	void setSmoothfrequency(float aFrequency)
	{
		if (aFrequency == 0.0f) doSmooth = false; else doSmooth = true;
		double theFeq = 1.0f - ((0.9 + (double(aFrequency) * 0.1)) - 0.001);
		smooth_frequency = float(theFeq * (6.28319 / double(SampleRateFloat)) * 8000.0);
	};
	//
	JUCE_ALIGN(32) float Frequency[MAX_VOICES];
	JUCE_ALIGN(32) float drive[MAX_VOICES];
	JUCE_ALIGN(32) float damp[MAX_VOICES];
	float SampleRateFloat;
	int32 Pole, v;
	int32 Type;
	bool doSmooth;
	JUCE_ALIGN(32) float smooth_dBuffer[MAX_VOICES];
	float smooth_frequency;
	float xFreqSamp, xFreqSampMin;
	float Rezonance;
	//
	JUCE_ALIGN(32) float dBuffer0[MAX_VOICES];
	JUCE_ALIGN(32) float dBuffer1[MAX_VOICES];
	JUCE_ALIGN(32) float dBuffer2[MAX_VOICES];
	JUCE_ALIGN(32) float dBuffer3[MAX_VOICES];
};
