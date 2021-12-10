// Auto-generated file. Do not edit!
//   Template: src/qs8-f32-vcvt/wasmsimd.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <wasm_simd128.h>

#include <xnnpack/common.h>
#include <xnnpack/intrinsics-polyfill.h>
#include <xnnpack/vcvt.h>


void xnn_qs8_f32_vcvt_ukernel__wasmsimd_x16(
    size_t n,
    const int8_t* x,
    float* y,
    const union xnn_qs8_f32_cvt_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_DISABLE_TSAN XNN_DISABLE_MSAN
{
  assert(n != 0);
  assert(n % sizeof(int8_t) == 0);
  assert(x != NULL);
  assert(y != NULL);

  const v128_t vminus_zero_point = wasm_v128_load64_splat(params->wasmsimd.minus_zero_point);
  const v128_t vscale = wasm_v128_load64_splat(params->wasmsimd.scale);
  for (; n >= 16 * sizeof(int8_t); n -= 16 * sizeof(int8_t)) {
    v128_t vx01234567 = wasm_i16x8_load8x8(x);
    v128_t vx89ABCDEF = wasm_i16x8_load8x8(x + 8);
    x += 16;

    vx01234567 = wasm_i16x8_add(vx01234567, vminus_zero_point);
    vx89ABCDEF = wasm_i16x8_add(vx89ABCDEF, vminus_zero_point);

    v128_t vy0123 = wasm_i32x4_extend_low_i16x8(vx01234567);
    v128_t vy4567 = wasm_i32x4_extend_high_i16x8(vx01234567);
    v128_t vy89AB = wasm_i32x4_extend_low_i16x8(vx89ABCDEF);
    v128_t vyCDEF = wasm_i32x4_extend_high_i16x8(vx89ABCDEF);

    vy0123 = wasm_f32x4_convert_i32x4(vy0123);
    vy4567 = wasm_f32x4_convert_i32x4(vy4567);
    vy89AB = wasm_f32x4_convert_i32x4(vy89AB);
    vyCDEF = wasm_f32x4_convert_i32x4(vyCDEF);

    vy0123 = wasm_f32x4_mul(vy0123, vscale);
    vy4567 = wasm_f32x4_mul(vy4567, vscale);
    vy89AB = wasm_f32x4_mul(vy89AB, vscale);
    vyCDEF = wasm_f32x4_mul(vyCDEF, vscale);

    wasm_v128_store(y, vy0123);
    wasm_v128_store(y + 4, vy4567);
    wasm_v128_store(y + 8, vy89AB);
    wasm_v128_store(y + 12, vyCDEF);
    y += 16;
  }
  for (; n >= 8 * sizeof(int8_t); n -= 8 * sizeof(int8_t)) {
    v128_t vx = wasm_i16x8_load8x8(x);
    vx = wasm_i16x8_add(vx, vminus_zero_point);
    x += 8;

    v128_t vy_lo = wasm_i32x4_extend_low_i16x8(vx);
    v128_t vy_hi = wasm_i32x4_extend_high_i16x8(vx);

    vy_lo = wasm_f32x4_convert_i32x4(vy_lo);
    vy_hi = wasm_f32x4_convert_i32x4(vy_hi);

    vy_lo = wasm_f32x4_mul(vy_lo, vscale);
    vy_hi = wasm_f32x4_mul(vy_hi, vscale);

    wasm_v128_store(y, vy_lo);
    wasm_v128_store(y + 4, vy_hi);
    y += 8;
  }
  if XNN_UNLIKELY(n != 0) {
    assert(n >= 1 * sizeof(int8_t));
    assert(n <= 7 * sizeof(int8_t));

    v128_t vx = wasm_i16x8_load8x8(x);
    vx = wasm_i16x8_add(vx, vminus_zero_point);
    x += 8;

    v128_t vy = wasm_i32x4_extend_low_i16x8(vx);
    vy = wasm_f32x4_convert_i32x4(vy);
    vy = wasm_f32x4_mul(vy, vscale);

    if (n & (4 * sizeof(int8_t))) {
      wasm_v128_store(y, vy); y += 4;
      vy = wasm_i32x4_extend_high_i16x8(vx);
      vy = wasm_f32x4_convert_i32x4(vy);
      vy = wasm_f32x4_mul(vy, vscale);
    }
    if (n & (2 * sizeof(int8_t))) {
      *((double*) y) = wasm_f64x2_extract_lane(vy, 0);
      vy = wasm_v64x2_shuffle(vy, vy, 1, 1);
      y += 2;
    }
    if (n & (1 * sizeof(int8_t))) {
      *y = wasm_f32x4_extract_lane(vy, 0);
    }
  }
}
