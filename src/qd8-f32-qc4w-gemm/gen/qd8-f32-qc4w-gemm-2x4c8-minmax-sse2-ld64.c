// Auto-generated file. Do not edit!
//   Template: src/qs8-gemm/MRx4c8-sse.c.in
//   Generator: tools/xngen
//
// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <emmintrin.h>

#include <xnnpack/gemm.h>
#include <xnnpack/math.h>
#include <xnnpack/unaligned.h>


void xnn_qd8_f32_qc4w_gemm_minmax_ukernel_2x4c8__sse2_ld64(
    size_t mr,
    size_t nc,
    size_t kc,
    const int8_t* restrict a,
    size_t a_stride,
    const void* restrict w,
    float* restrict c,
    size_t cm_stride,
    size_t cn_stride,
    const union xnn_f32_qc4w_minmax_params params[restrict XNN_MIN_ELEMENTS(1)],
    const struct xnn_qd8_quantization_params quantization_params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(mr != 0);
  assert(mr <= 2);
  assert(nc != 0);
  assert(kc != 0);
  assert(kc % sizeof(int8_t) == 0);
  assert(a != NULL);
  assert(w != NULL);
  assert(c != NULL);

  kc = round_up_po2(kc, 8 * sizeof(int8_t));
  const int8_t* a0 = a;
  float* c0 = c;
  const struct xnn_qd8_quantization_params* qp0 = quantization_params;
  const int8_t* a1 = (const int8_t*) ((uintptr_t) a0 + a_stride);
  float* c1 = (float*) ((uintptr_t) c0 + cm_stride);
  const struct xnn_qd8_quantization_params* qp1 = &quantization_params[1];
  if XNN_UNPREDICTABLE(mr != 2) {
    a1 = a0;
    c1 = c0;
    qp1 = qp0;
  }

  const __m128i vmask = _mm_load_si128((const __m128i*) params->sse.mask);  // 0xF0
  do {
    const __m128i vksum = _mm_load_si128((const __m128i*) w);
    const __m128i vinput_zero_point01 = _mm_loadu_si128((const __m128i*)qp0);
    const __m128i vinput_zero_point0 = _mm_shuffle_epi32(vinput_zero_point01, _MM_SHUFFLE(0, 0, 0, 0));
    const __m128i vinput_zero_point1 = _mm_shuffle_epi32(vinput_zero_point01, _MM_SHUFFLE(2, 2, 2, 2));
    const __m128i vzero = _mm_setzero_si128();
    const __m128i vksum_lo = _mm_srli_epi32(_mm_slli_epi32(vksum, 16), 16);
    const __m128i vksum_hi = _mm_srli_epi32(vksum, 16);

    __m128i vzpprodksumhi0 = _mm_mulhi_epu16(vinput_zero_point0, vksum_lo);
    const __m128i vzpprodksumlo0 = _mm_mullo_epi16(vinput_zero_point0, vksum_lo);
    __m128i vzpprodksumhi1 = _mm_mulhi_epu16(vinput_zero_point1, vksum_lo);
    const __m128i vzpprodksumlo1 = _mm_mullo_epi16(vinput_zero_point1, vksum_lo);

    vzpprodksumhi0 = _mm_add_epi16(vzpprodksumhi0, _mm_mullo_epi16(vinput_zero_point0, vksum_hi));
    vzpprodksumhi0 = _mm_sub_epi16(vzpprodksumhi0, _mm_and_si128(_mm_srai_epi16(vinput_zero_point0, 15), vksum_lo));
    vzpprodksumhi1 = _mm_add_epi16(vzpprodksumhi1, _mm_mullo_epi16(vinput_zero_point1, vksum_hi));
    vzpprodksumhi1 = _mm_sub_epi16(vzpprodksumhi1, _mm_and_si128(_mm_srai_epi16(vinput_zero_point1, 15), vksum_lo));

    vzpprodksumhi0 = _mm_slli_si128(vzpprodksumhi0, 2);
    vzpprodksumhi1 = _mm_slli_si128(vzpprodksumhi1, 2);

    const __m128i vksumzp0 = _mm_or_si128(vzpprodksumhi0, vzpprodksumlo0);
    const __m128i vksumzp1 = _mm_or_si128(vzpprodksumhi1, vzpprodksumlo1);

    const __m128i vksum010 = _mm_unpacklo_epi32(vksumzp0, vzero);
    const __m128i vksum230 = _mm_unpackhi_epi32(vksumzp0, vzero);
    const __m128i vksum011 = _mm_unpacklo_epi32(vksumzp1, vzero);
    const __m128i vksum231 = _mm_unpackhi_epi32(vksumzp1, vzero);

    __m128i vacc0x0 = _mm_unpacklo_epi64(vksum010, vzero);
    __m128i vacc0x1 = _mm_unpackhi_epi64(vksum010, vzero);
    __m128i vacc1x0 = _mm_unpacklo_epi64(vksum011, vzero);
    __m128i vacc1x1 = _mm_unpackhi_epi64(vksum011, vzero);

    __m128i vacc0x2 = _mm_unpacklo_epi64(vksum230, vzero);
    __m128i vacc0x3 = _mm_unpackhi_epi64(vksum230, vzero);
    __m128i vacc1x2 = _mm_unpacklo_epi64(vksum231, vzero);
    __m128i vacc1x3 = _mm_unpackhi_epi64(vksum231, vzero);
    w = (const int32_t*) w + 4;

    size_t k = kc;

    while (k >= 16 * sizeof(int8_t)) {
      const __m128i va0c0 = _mm_loadl_epi64((const __m128i*) a0);
      const __m128i vxa0c0 = _mm_srai_epi16(_mm_unpacklo_epi8(va0c0, va0c0), 8);
      a0 += 8;
      const __m128i va1c0 = _mm_loadl_epi64((const __m128i*) a1);
      const __m128i vxa1c0 = _mm_srai_epi16(_mm_unpacklo_epi8(va1c0, va1c0), 8);
      a1 += 8;

      const __m128i vb0c01 = _mm_loadl_epi64((const __m128i*) w);
      const __m128i vbs0c0 = _mm_slli_epi32(vb0c01, 4);
      const __m128i vb0c0 = _mm_and_si128(vbs0c0, vmask);
      const __m128i vxb0c0 = _mm_srai_epi16(_mm_unpacklo_epi8(vb0c0, vb0c0), 8);

      vacc0x0 = _mm_add_epi32(vacc0x0, _mm_madd_epi16(vxa0c0, vxb0c0));
      vacc1x0 = _mm_add_epi32(vacc1x0, _mm_madd_epi16(vxa1c0, vxb0c0));
      const __m128i vb1c01 = _mm_loadl_epi64((const __m128i*) ((const int8_t*) w + 8));
      const __m128i vbs1c0 = _mm_slli_epi32(vb1c01, 4);
      const __m128i vb1c0 = _mm_and_si128(vbs1c0, vmask);
      const __m128i vxb1c0 = _mm_srai_epi16(_mm_unpacklo_epi8(vb1c0, vb1c0), 8);

      vacc0x1 = _mm_add_epi32(vacc0x1, _mm_madd_epi16(vxa0c0, vxb1c0));
      vacc1x1 = _mm_add_epi32(vacc1x1, _mm_madd_epi16(vxa1c0, vxb1c0));
      const __m128i vb2c01 = _mm_loadl_epi64((const __m128i*) ((const int8_t*) w + 16));
      const __m128i vbs2c0 = _mm_slli_epi32(vb2c01, 4);
      const __m128i vb2c0 = _mm_and_si128(vbs2c0, vmask);
      const __m128i vxb2c0 = _mm_srai_epi16(_mm_unpacklo_epi8(vb2c0, vb2c0), 8);

      vacc0x2 = _mm_add_epi32(vacc0x2, _mm_madd_epi16(vxa0c0, vxb2c0));
      vacc1x2 = _mm_add_epi32(vacc1x2, _mm_madd_epi16(vxa1c0, vxb2c0));
      const __m128i vb3c01 = _mm_loadl_epi64((const __m128i*) ((const int8_t*) w + 24));
      const __m128i vbs3c0 = _mm_slli_epi32(vb3c01, 4);
      const __m128i vb3c0 = _mm_and_si128(vbs3c0, vmask);
      const __m128i vxb3c0 = _mm_srai_epi16(_mm_unpacklo_epi8(vb3c0, vb3c0), 8);

      vacc0x3 = _mm_add_epi32(vacc0x3, _mm_madd_epi16(vxa0c0, vxb3c0));
      vacc1x3 = _mm_add_epi32(vacc1x3, _mm_madd_epi16(vxa1c0, vxb3c0));

      const __m128i va0c1 = _mm_loadl_epi64((const __m128i*) a0);
      const __m128i vxa0c1 = _mm_srai_epi16(_mm_unpacklo_epi8(va0c1, va0c1), 8);
      a0 += 8;
      const __m128i va1c1 = _mm_loadl_epi64((const __m128i*) a1);
      const __m128i vxa1c1 = _mm_srai_epi16(_mm_unpacklo_epi8(va1c1, va1c1), 8);
      a1 += 8;

      const __m128i vb0c1 = _mm_and_si128(vb0c01, vmask);
      const __m128i vxb0c1 = _mm_srai_epi16(_mm_unpacklo_epi8(vb0c1, vb0c1), 8);

      vacc0x0 = _mm_add_epi32(vacc0x0, _mm_madd_epi16(vxa0c1, vxb0c1));
      vacc1x0 = _mm_add_epi32(vacc1x0, _mm_madd_epi16(vxa1c1, vxb0c1));
      const __m128i vb1c1 = _mm_and_si128(vb1c01, vmask);
      const __m128i vxb1c1 = _mm_srai_epi16(_mm_unpacklo_epi8(vb1c1, vb1c1), 8);

      vacc0x1 = _mm_add_epi32(vacc0x1, _mm_madd_epi16(vxa0c1, vxb1c1));
      vacc1x1 = _mm_add_epi32(vacc1x1, _mm_madd_epi16(vxa1c1, vxb1c1));
      const __m128i vb2c1 = _mm_and_si128(vb2c01, vmask);
      const __m128i vxb2c1 = _mm_srai_epi16(_mm_unpacklo_epi8(vb2c1, vb2c1), 8);

      vacc0x2 = _mm_add_epi32(vacc0x2, _mm_madd_epi16(vxa0c1, vxb2c1));
      vacc1x2 = _mm_add_epi32(vacc1x2, _mm_madd_epi16(vxa1c1, vxb2c1));
      const __m128i vb3c1 = _mm_and_si128(vb3c01, vmask);
      const __m128i vxb3c1 = _mm_srai_epi16(_mm_unpacklo_epi8(vb3c1, vb3c1), 8);

      vacc0x3 = _mm_add_epi32(vacc0x3, _mm_madd_epi16(vxa0c1, vxb3c1));
      vacc1x3 = _mm_add_epi32(vacc1x3, _mm_madd_epi16(vxa1c1, vxb3c1));


      w = (const int8_t*) w + 32;
      k -= 16 * sizeof(int8_t);
    }

    while (k >= 8 * sizeof(int8_t)) {
      const __m128i va0 = _mm_loadl_epi64((const __m128i*) a0);
      const __m128i vxa0 = _mm_srai_epi16(_mm_unpacklo_epi8(va0, va0), 8);
      a0 += 8;
      const __m128i va1 = _mm_loadl_epi64((const __m128i*) a1);
      const __m128i vxa1 = _mm_srai_epi16(_mm_unpacklo_epi8(va1, va1), 8);
      a1 += 8;

      __m128i vb0 = _mm_loadl_epi64((const __m128i*) w);
      vb0 = _mm_slli_epi32(vb0, 4);
      vb0 = _mm_and_si128(vb0, vmask);

      const __m128i vxb0 = _mm_srai_epi16(_mm_unpacklo_epi8(vb0, vb0), 8);

      vacc0x0 = _mm_add_epi32(vacc0x0, _mm_madd_epi16(vxa0, vxb0));
      vacc1x0 = _mm_add_epi32(vacc1x0, _mm_madd_epi16(vxa1, vxb0));
      __m128i vb1 = _mm_loadl_epi64((const __m128i*) ((const int8_t*) w + 8));
      vb1 = _mm_slli_epi32(vb1, 4);
      vb1 = _mm_and_si128(vb1, vmask);

      const __m128i vxb1 = _mm_srai_epi16(_mm_unpacklo_epi8(vb1, vb1), 8);

      vacc0x1 = _mm_add_epi32(vacc0x1, _mm_madd_epi16(vxa0, vxb1));
      vacc1x1 = _mm_add_epi32(vacc1x1, _mm_madd_epi16(vxa1, vxb1));
      __m128i vb2 = _mm_loadl_epi64((const __m128i*) ((const int8_t*) w + 16));
      vb2 = _mm_slli_epi32(vb2, 4);
      vb2 = _mm_and_si128(vb2, vmask);

      const __m128i vxb2 = _mm_srai_epi16(_mm_unpacklo_epi8(vb2, vb2), 8);

      vacc0x2 = _mm_add_epi32(vacc0x2, _mm_madd_epi16(vxa0, vxb2));
      vacc1x2 = _mm_add_epi32(vacc1x2, _mm_madd_epi16(vxa1, vxb2));
      __m128i vb3 = _mm_loadl_epi64((const __m128i*) ((const int8_t*) w + 24));
      vb3 = _mm_slli_epi32(vb3, 4);
      vb3 = _mm_and_si128(vb3, vmask);

      const __m128i vxb3 = _mm_srai_epi16(_mm_unpacklo_epi8(vb3, vb3), 8);

      vacc0x3 = _mm_add_epi32(vacc0x3, _mm_madd_epi16(vxa0, vxb3));
      vacc1x3 = _mm_add_epi32(vacc1x3, _mm_madd_epi16(vxa1, vxb3));

      w = (const int8_t*) w + 32;
      k -= 8 * sizeof(int8_t);
    }

    const __m128i vacc0x02 = _mm_add_epi32(_mm_unpacklo_epi32(vacc0x0, vacc0x2), _mm_unpackhi_epi32(vacc0x0, vacc0x2));
    const __m128i vacc0x13 = _mm_add_epi32(_mm_unpacklo_epi32(vacc0x1, vacc0x3), _mm_unpackhi_epi32(vacc0x1, vacc0x3));
    const __m128i vacc1x02 = _mm_add_epi32(_mm_unpacklo_epi32(vacc1x0, vacc1x2), _mm_unpackhi_epi32(vacc1x0, vacc1x2));
    const __m128i vacc1x13 = _mm_add_epi32(_mm_unpacklo_epi32(vacc1x1, vacc1x3), _mm_unpackhi_epi32(vacc1x1, vacc1x3));

    __m128i vacc0x0123 = _mm_add_epi32(_mm_unpacklo_epi32(vacc0x02, vacc0x13), _mm_unpackhi_epi32(vacc0x02, vacc0x13));
    __m128i vacc1x0123 = _mm_add_epi32(_mm_unpacklo_epi32(vacc1x02, vacc1x13), _mm_unpackhi_epi32(vacc1x02, vacc1x13));

    vacc0x0123 = _mm_srai_epi32(vacc0x0123, 4);
    vacc1x0123 = _mm_srai_epi32(vacc1x0123, 4);
    __m128 vout0x0123 = _mm_cvtepi32_ps(vacc0x0123);
    __m128 vout1x0123 = _mm_cvtepi32_ps(vacc1x0123);

    const __m128i vinput_scale01 = _mm_loadu_si128((const __m128i*) qp0);
    const __m128 vinput_scale0 = _mm_castsi128_ps(_mm_shuffle_epi32(vinput_scale01, _MM_SHUFFLE(1, 1, 1, 1)));
    const __m128 vinput_scale1 = _mm_castsi128_ps(_mm_shuffle_epi32(vinput_scale01, _MM_SHUFFLE(3, 3, 3, 3)));

    vout0x0123 = _mm_mul_ps(vout0x0123, vinput_scale0);
    vout1x0123 = _mm_mul_ps(vout1x0123, vinput_scale1);

    const __m128 vfilter_output_scale0123 = _mm_load_ps((const float*) w);
    vout0x0123 = _mm_mul_ps(vout0x0123, vfilter_output_scale0123);
    vout1x0123 = _mm_mul_ps(vout1x0123, vfilter_output_scale0123);

    const __m128 vbias0123 = _mm_load_ps((const float*) w + 4);
    w = (const float*) w + 8;
    vout0x0123 = _mm_add_ps(vout0x0123, vbias0123);
    vout1x0123 = _mm_add_ps(vout1x0123, vbias0123);

    const __m128 vmin = _mm_load_ps(params->sse.min);
    vout0x0123 = _mm_max_ps(vout0x0123, vmin);
    vout1x0123 = _mm_max_ps(vout1x0123, vmin);

    const __m128 vmax = _mm_load_ps(params->sse.max);
    vout0x0123 = _mm_min_ps(vout0x0123, vmax);
    vout1x0123 = _mm_min_ps(vout1x0123, vmax);

    if XNN_LIKELY(nc >= 4) {
      _mm_storeu_ps(c0, vout0x0123);
      _mm_storeu_ps(c1, vout1x0123);

      a0 = (const int8_t*) ((uintptr_t) a0 - kc);
      a1 = (const int8_t*) ((uintptr_t) a1 - kc);

      c0 = (float*) ((uintptr_t) c0 + cn_stride);
      c1 = (float*) ((uintptr_t) c1 + cn_stride);

      nc -= 4;
    } else {
      if (nc & 2) {
        _mm_storel_pi((__m64*) c0, vout0x0123);
        vout0x0123 = _mm_unpackhi_ps(vout0x0123, vout0x0123);
        c0 += 2;
        _mm_storel_pi((__m64*) c1, vout1x0123);
        vout1x0123 = _mm_unpackhi_ps(vout1x0123, vout1x0123);
        c1 += 2;
      }
      if (nc & 1) {
        _mm_store_ss(c0, vout0x0123);
        _mm_store_ss(c1, vout1x0123);
      }
      nc = 0;
    }
  } while (nc != 0);
}
