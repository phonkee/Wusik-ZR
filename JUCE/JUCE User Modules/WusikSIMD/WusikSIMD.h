/*
BEGIN_JUCE_MODULE_DECLARATION
    ID:               WusikSIMD
    vendor:           Wusik Dot Com
    version:          1.0.0
    name:             WusikSIMD
    description:      Wusik SIMD SSE, AVX and FMA3 Parallel Processing
    website:          https://www.Wusik.com
    dependencies:     
	license:          Open-Source
END_JUCE_MODULE_DECLARATION 
*/
//
#ifndef WUTILSSIMD_H_INCLUDED
#define WUTILSSIMD_H_INCLUDED
//
// -------------------------------------------------------------------------------------------------------------------------------
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_data_structures/juce_data_structures.h> 
//
using namespace juce;
//
JUCE_ALIGN(16) class wSSE
{
public:
	__m128 v;
	//
	forcedinline wSSE() { v = _mm_setzero_ps(); };
	forcedinline wSSE(float x) : v(_mm_set1_ps(x)) { };
	forcedinline wSSE(float *px) : v(_mm_load_ps(px)) { };
	forcedinline wSSE(__m128 v) : v(v) { }; 
	forcedinline void write(float* target) { _mm_store_ps(target, v); };
	forcedinline void load(float* load) { v = _mm_load_ps(load); };
	forcedinline void set(wSSE& value) { v = value.v; };
	forcedinline void operator = (wSSE& _v2) { v = _v2.v; };
	forcedinline wSSE topThis(const wSSE& topValue) { v = _mm_min_ps(v, topValue.v); return v; };
	forcedinline wSSE bottomThis(const wSSE& topValue) { v = _mm_max_ps(v, topValue.v); return v; };
	forcedinline wSSE boundsThis(const wSSE& lowValue, const wSSE topValue) { v = _mm_min_ps(_mm_max_ps(v, lowValue.v), topValue.v); return v; };
	forcedinline wSSE boundsThisZeroOne() { v = _mm_min_ps(_mm_max_ps(v, _mm_setzero_ps()), _mm_set1_ps(1.0f)); return v; };
	forcedinline wSSE overIncrementThis(const wSSE& value, const wSSE compare, const wSSE increment) { v = _mm_add_ps(v, _mm_and_ps(_mm_cmpgt_ps(value.v, compare.v), increment.v)); return v; };
	forcedinline bool anyOverZero() { return _mm_movemask_ps(_mm_cmpeq_ps(v, _mm_setzero_ps())) != 0xff; };
	forcedinline wSSE multiply_MultAddThis(const wSSE& mult1, const wSSE& mult2, const wSSE& add) { v = _mm_mul_ps(v, _mm_add_ps(_mm_mul_ps(mult1.v, mult2.v), add.v)); return v; }
	forcedinline wSSE invert() { return _mm_sub_ps(_mm_set1_ps(1.0f), v); };
	forcedinline void addWrite(float *variable) { v = _mm_add_ps(v, _mm_load_ps(variable)); _mm_store_ps(variable, v); };
	static forcedinline void addVectorsAndWrite(wSSE input, float* output) { _mm_store_ps(output, _mm_add_ps(input.v, _mm_load_ps(output))); };
	forcedinline void multWrite(float *variable) { v = _mm_mul_ps(v, _mm_load_ps(variable)); _mm_store_ps(variable, v); };
	static forcedinline void setZero(float *variable) { _mm_store_ps(variable, _mm_setzero_ps()); };
	static forcedinline wSSE multAdd(const wSSE& mult1, const wSSE& mult2, const wSSE& add) { return _mm_add_ps(_mm_mul_ps(mult1.v, mult2.v), add.v); }
	static forcedinline wSSE multSubs(const wSSE& mult1, const wSSE& mult2, const wSSE& subs) { return _mm_sub_ps(_mm_mul_ps(mult1.v, mult2.v), subs.v); }
	forcedinline wSSE addGreaterEqualSubs(const wSSE& addFirst, const wSSE& valueCheckAndSubs) 
	{ 
		v = _mm_add_ps(v, addFirst.v);
		v = _mm_sub_ps(v, _mm_and_ps(_mm_cmpge_ps(v, valueCheckAndSubs.v), valueCheckAndSubs.v));
		return v; 
	};
	//
	forcedinline wSSE equal(const wSSE& value2, const wSSE& valueA) { return _mm_and_ps(_mm_cmpeq_ps(v, value2.v), valueA.v); };
	forcedinline wSSE equal(const wSSE& value2, const wSSE& valueA, const wSSE& valueB)
	{
		__m128 result1 = _mm_cmpeq_ps(v, value2.v);
		return _mm_add_ps(_mm_and_ps(result1, valueA.v),_mm_andnot_ps(result1, valueB.v));
	};
	//
	forcedinline wSSE equalAdd(const wSSE& value1, const wSSE& value2, const wSSE& valueA, const wSSE& valueB)
	{
		__m128 result1 = _mm_cmpeq_ps(value1.v, value2.v);
		v = _mm_add_ps(v, _mm_add_ps(_mm_and_ps(result1, valueA.v), _mm_andnot_ps(result1, valueB.v)));
		return v;
	};
	//
	forcedinline wSSE greater(const wSSE& value2, const wSSE& valueA) { return _mm_and_ps(_mm_cmpgt_ps(v, value2.v), valueA.v); };
	forcedinline wSSE greater(const wSSE& value2, const wSSE& valueA, const wSSE& valueB)
	{
		__m128 result1 = _mm_cmpgt_ps(v, value2.v);
		return _mm_add_ps(_mm_and_ps(result1, valueA.v),_mm_andnot_ps(result1, valueB.v));
	};
	//
	forcedinline wSSE lower(const wSSE& value2, const wSSE& valueA, const wSSE& valueB)
	{
		__m128 result1 = _mm_cmplt_ps(v, value2.v);
		return _mm_add_ps(_mm_and_ps(result1, valueA.v), _mm_andnot_ps(result1, valueB.v));	
	};
};
//
forcedinline wSSE operator + (const wSSE &l, const wSSE &r) { return wSSE(_mm_add_ps(l.v, r.v)); };
forcedinline wSSE operator - (const wSSE &l, const wSSE &r) { return wSSE(_mm_sub_ps(l.v, r.v)); };
forcedinline wSSE operator * (const wSSE &l, const wSSE &r) { return wSSE(_mm_mul_ps(l.v, r.v)); };
forcedinline wSSE operator / (const wSSE &l, const wSSE &r) { return wSSE(_mm_div_ps(l.v, r.v)); };
//
forcedinline void operator += (wSSE &l, const wSSE &r) { l.v = _mm_add_ps(l.v, r.v); };
forcedinline void operator *= (wSSE &l, const wSSE &r) { l.v = _mm_mul_ps(l.v, r.v); };
//
// -------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------
//
#if JUCE_WINDOWS
	JUCE_ALIGN(32) class wAVXFMA3
	{
	public:
		__m256 v;
		//
		forcedinline wAVXFMA3() { v = _mm256_setzero_ps(); };
		forcedinline wAVXFMA3(const float x) : v(_mm256_set1_ps(x)) { };
		forcedinline wAVXFMA3(const float *px) : v(_mm256_load_ps(px)) { };
		forcedinline wAVXFMA3(const __m256 v) : v(v) { };
		forcedinline void write(float* target) { _mm256_store_ps(target, v); };
		forcedinline void load(float* load) { v = _mm256_load_ps(load); };
		forcedinline void set(const wAVXFMA3& value) { v = value.v; };
		forcedinline void operator = (const wAVXFMA3& _v2) { v = _v2.v; };
		forcedinline void operator = (const float* _v2) { v = _mm256_load_ps(_v2); };
		forcedinline wAVXFMA3 topThis(const wAVXFMA3& topValue) { v = _mm256_min_ps(v, topValue.v); return v; };
		forcedinline wAVXFMA3 bottomThis(const wAVXFMA3& topValue) { v = _mm256_max_ps(v, topValue.v); return v; };
		forcedinline wAVXFMA3 boundsThis(const wAVXFMA3& lowValue, const wAVXFMA3 topValue) { v = _mm256_min_ps(_mm256_max_ps(v, lowValue.v), topValue.v); return v; };
		forcedinline wAVXFMA3 boundsThisZeroOne() { v = _mm256_min_ps(_mm256_max_ps(v, _mm256_setzero_ps()), _mm256_set1_ps(1.0f)); return v; };
		forcedinline wAVXFMA3 overIncrementThis(const wAVXFMA3& value, const wAVXFMA3 compare, const wAVXFMA3 increment) { v = _mm256_add_ps(v, _mm256_and_ps(_mm256_cmp_ps(value.v, compare.v, _CMP_GT_OQ), increment.v)); return v; };
		forcedinline bool anyOverZero() { return _mm256_movemask_ps(_mm256_cmp_ps(v, _mm256_setzero_ps(), _CMP_EQ_OQ)) != 0xff; };
		forcedinline wAVXFMA3 multiply_MultAddThis(const wAVXFMA3& mult1, const wAVXFMA3& mult2, const wAVXFMA3& add) { v = _mm256_mul_ps(v, _mm256_fmadd_ps(mult1.v, mult2.v, add.v)); return v; }
		static forcedinline wAVXFMA3 multAdd(const wAVXFMA3& mult1, const wAVXFMA3& mult2, const wAVXFMA3& add) { return _mm256_fmadd_ps(mult1.v, mult2.v, add.v); }
		static forcedinline wAVXFMA3 multSubs(const wAVXFMA3& mult1, const wAVXFMA3& mult2, const wAVXFMA3& subs) { return _mm256_fmsub_ps(mult1.v, mult2.v, subs.v); }
		//
		forcedinline wAVXFMA3 invert() { return _mm256_sub_ps(_mm256_set1_ps(1.0f), v); };
		forcedinline void addWrite(float *variable) { v = _mm256_add_ps(v , _mm256_load_ps(variable)); _mm256_store_ps(variable, v); };
		static forcedinline void addVectorsAndWrite(wAVXFMA3 input, float* output) { _mm256_store_ps(output, _mm256_add_ps(input.v, _mm256_load_ps(output))); };
		forcedinline void multWrite(float *variable) { v = _mm256_mul_ps(v, _mm256_load_ps(variable)); _mm256_store_ps(variable, v); };
		static forcedinline void setZero(float *variable) { _mm256_store_ps(variable, _mm256_setzero_ps()); };
		forcedinline wAVXFMA3 addGreaterEqualSubs(const wAVXFMA3& addFirst, const wAVXFMA3& valueCheckAndSubs)
		{ 
			v = _mm256_add_ps(v, addFirst.v);
			v = _mm256_sub_ps(v, _mm256_and_ps(_mm256_cmp_ps(v, valueCheckAndSubs.v, _CMP_GE_OQ), valueCheckAndSubs.v)); 
			return v; 
		};
		//
		forcedinline wAVXFMA3 equal(const wAVXFMA3& value2, const wAVXFMA3& valueA) { return _mm256_and_ps(_mm256_cmp_ps(v, value2.v, _CMP_EQ_OQ), valueA.v); };
		forcedinline wAVXFMA3 equal(const wAVXFMA3& value2, const wAVXFMA3& valueA, const wAVXFMA3& valueB)
		{
			__m256 result1 = _mm256_cmp_ps(v, value2.v, _CMP_EQ_OQ);
			return  _mm256_add_ps(_mm256_and_ps(result1, valueA.v),_mm256_andnot_ps(result1, valueB.v));
		};
		//
		forcedinline wAVXFMA3 equalAdd(const wAVXFMA3& value1, const wAVXFMA3& value2, const wAVXFMA3& valueA, const wAVXFMA3& valueB)
		{
			__m256 result1 = _mm256_cmp_ps(value1.v, value2.v, _CMP_EQ_OQ);
			v = _mm256_add_ps(v, _mm256_add_ps(_mm256_and_ps(result1, valueA.v),_mm256_andnot_ps(result1, valueB.v)));
			return v;
		};
		//
		forcedinline wAVXFMA3 greater(const wAVXFMA3& value2, const wAVXFMA3& valueA) { return _mm256_and_ps(_mm256_cmp_ps(v, value2.v, _CMP_GT_OQ), valueA.v); };
		forcedinline wAVXFMA3 greater(const wAVXFMA3& value2, const wAVXFMA3& valueA, const wAVXFMA3& valueB)
		{
			__m256 result1 = _mm256_cmp_ps(v, value2.v, _CMP_GT_OQ);
			return  _mm256_add_ps(_mm256_and_ps(result1, valueA.v),_mm256_andnot_ps(result1, valueB.v));
		};
		//
		forcedinline wAVXFMA3 lower(const wAVXFMA3& value2, const wAVXFMA3& valueA, const wAVXFMA3& valueB)
		{
			__m256 result1 = _mm256_cmp_ps(v, value2.v, _CMP_LT_OQ);
			return  _mm256_add_ps(_mm256_and_ps(result1, valueA.v), _mm256_andnot_ps(result1, valueB.v));
		};
	};
	//
	forcedinline wAVXFMA3 operator + (const wAVXFMA3 &l, const wAVXFMA3 &r) { return wAVXFMA3(_mm256_add_ps(l.v, r.v)); };
	forcedinline wAVXFMA3 operator - (const wAVXFMA3 &l, const wAVXFMA3 &r) { return wAVXFMA3(_mm256_sub_ps(l.v, r.v)); };
	forcedinline wAVXFMA3 operator * (const wAVXFMA3 &l, const wAVXFMA3 &r) { return wAVXFMA3(_mm256_mul_ps(l.v, r.v)); };
	forcedinline wAVXFMA3 operator / (const wAVXFMA3 &l, const wAVXFMA3 &r) { return wAVXFMA3(_mm256_div_ps(l.v, r.v)); };
	//
	forcedinline void operator += (wAVXFMA3 &l, const wAVXFMA3 &r) { l.v = _mm256_add_ps(l.v, r.v); };
	forcedinline void operator *= (wAVXFMA3 &l, const wAVXFMA3 &r) { l.v = _mm256_mul_ps(l.v, r.v); };
	#endif
#endif