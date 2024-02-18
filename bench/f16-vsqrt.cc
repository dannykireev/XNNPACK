// Copyright 2022 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <algorithm>
#include <cmath>
#include <functional>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>
#include <fp16/fp16.h>
#include "bench/utils.h"

#include <xnnpack.h>
#include <xnnpack/aligned-allocator.h>
#include <xnnpack/common.h>
#include <xnnpack/microfnptr.h>
#include <xnnpack/microparams-init.h>
#include <xnnpack/vunary.h>


static void f16_vsqrt(
  benchmark::State& state,
  xnn_f16_vsqrt_ukernel_fn sqrt,
  benchmark::utils::IsaCheckFunction isa_check = nullptr)
{
  if (isa_check && !isa_check(state)) {
    return;
  }

  const size_t num_elements = state.range(0);

  std::random_device random_device;
  auto rng = std::mt19937(random_device());
  auto f32rng = std::bind(std::uniform_real_distribution<float>(), std::ref(rng));
  auto f16rng = std::bind(fp16_ieee_from_fp32_value, f32rng);

  std::vector<uint16_t, AlignedAllocator<uint16_t, 64>> x(num_elements);
  std::vector<uint16_t, AlignedAllocator<uint16_t, 64>> y(num_elements);
  std::generate(x.begin(), x.end(), std::ref(f16rng));
  std::fill(y.begin(), y.end(), UINT16_C(0x7E00) /* NaN */);

  for (auto _ : state) {
    sqrt(num_elements * sizeof(uint16_t), x.data(), y.data(), nullptr);
  }

  const uint64_t cpu_frequency = benchmark::utils::GetCurrentCpuFrequency();
  if (cpu_frequency != 0) {
    state.counters["cpufreq"] = cpu_frequency;
  }

  const size_t elements_per_iteration = num_elements;
  state.counters["elements"] =
    benchmark::Counter(uint64_t(state.iterations()) * elements_per_iteration, benchmark::Counter::kIsRate);

  const size_t bytes_per_iteration = 2 * num_elements * sizeof(uint16_t);
  state.counters["bytes"] =
    benchmark::Counter(uint64_t(state.iterations()) * bytes_per_iteration, benchmark::Counter::kIsRate);
}


#if XNN_ENABLE_ARM_FP16_VECTOR && XNN_ARCH_ARM64
  BENCHMARK_CAPTURE(f16_vsqrt, aarch64_neonfp16arith_sqrt_u8,
                    xnn_f16_vsqrt_ukernel__aarch64_neonfp16arith_sqrt_u8,
                    benchmark::utils::CheckNEONFP16ARITH)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
  BENCHMARK_CAPTURE(f16_vsqrt, aarch64_neonfp16arith_sqrt_u16,
                    xnn_f16_vsqrt_ukernel__aarch64_neonfp16arith_sqrt_u16,
                    benchmark::utils::CheckNEONFP16ARITH)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
  BENCHMARK_CAPTURE(f16_vsqrt, aarch64_neonfp16arith_sqrt_u32,
                    xnn_f16_vsqrt_ukernel__aarch64_neonfp16arith_sqrt_u32,
                    benchmark::utils::CheckNEONFP16ARITH)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
#endif  // XNN_ENABLE_ARM_FP16_VECTOR && XNN_ARCH_ARM64

#if XNN_ENABLE_ARM_FP16_VECTOR && (XNN_ARCH_ARM || XNN_ARCH_ARM64)
  BENCHMARK_CAPTURE(f16_vsqrt, neonfp16arith_nr1fma1adj_u8,
                    xnn_f16_vsqrt_ukernel__neonfp16arith_nr1fma1adj_u8,
                    benchmark::utils::CheckNEONFP16ARITH)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
  BENCHMARK_CAPTURE(f16_vsqrt, neonfp16arith_nr1fma1adj_u16,
                    xnn_f16_vsqrt_ukernel__neonfp16arith_nr1fma1adj_u16,
                    benchmark::utils::CheckNEONFP16ARITH)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
  BENCHMARK_CAPTURE(f16_vsqrt, neonfp16arith_nr1fma1adj_u32,
                    xnn_f16_vsqrt_ukernel__neonfp16arith_nr1fma1adj_u32,
                    benchmark::utils::CheckNEONFP16ARITH)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
#endif  // XNN_ENABLE_ARM_FP16_VECTOR && (XNN_ARCH_ARM || XNN_ARCH_ARM64)

#if XNN_ENABLE_ARM_FP16_SCALAR && (XNN_ARCH_ARM || XNN_ARCH_ARM64)
  BENCHMARK_CAPTURE(f16_vsqrt, fp16arith_sqrt_u1,
                    xnn_f16_vsqrt_ukernel__fp16arith_sqrt_u1,
                    benchmark::utils::CheckFP16ARITH)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
  BENCHMARK_CAPTURE(f16_vsqrt, fp16arith_sqrt_u2,
                    xnn_f16_vsqrt_ukernel__fp16arith_sqrt_u2,
                    benchmark::utils::CheckFP16ARITH)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
  BENCHMARK_CAPTURE(f16_vsqrt, fp16arith_sqrt_u4,
                    xnn_f16_vsqrt_ukernel__fp16arith_sqrt_u4,
                    benchmark::utils::CheckFP16ARITH)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
#endif  // XNN_ENABLE_ARM_FP16_SCALAR && (XNN_ARCH_ARM || XNN_ARCH_ARM64)

#if XNN_ARCH_X86 || XNN_ARCH_X86_64
  BENCHMARK_CAPTURE(f16_vsqrt, f16c_sqrt_u8,
                    xnn_f16_vsqrt_ukernel__f16c_sqrt_u8,
                    benchmark::utils::CheckF16C)
      ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
      ->UseRealTime();
  BENCHMARK_CAPTURE(f16_vsqrt, f16c_sqrt_u16,
                    xnn_f16_vsqrt_ukernel__f16c_sqrt_u16,
                    benchmark::utils::CheckF16C)
      ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
      ->UseRealTime();
  BENCHMARK_CAPTURE(f16_vsqrt, f16c_sqrt_u32,
                    xnn_f16_vsqrt_ukernel__f16c_sqrt_u32,
                    benchmark::utils::CheckF16C)
      ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
      ->UseRealTime();
  BENCHMARK_CAPTURE(f16_vsqrt, f16c_rsqrt_u8,
                    xnn_f16_vsqrt_ukernel__f16c_rsqrt_u8,
                    benchmark::utils::CheckF16C)
      ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
      ->UseRealTime();
  BENCHMARK_CAPTURE(f16_vsqrt, f16c_rsqrt_u16,
                    xnn_f16_vsqrt_ukernel__f16c_rsqrt_u16,
                    benchmark::utils::CheckF16C)
      ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
      ->UseRealTime();
  BENCHMARK_CAPTURE(f16_vsqrt, f16c_rsqrt_u32,
                    xnn_f16_vsqrt_ukernel__f16c_rsqrt_u32,
                    benchmark::utils::CheckF16C)
      ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
      ->UseRealTime();
#endif  // XNN_ARCH_X86 || XNN_ARCH_X86_64

#ifndef XNNPACK_BENCHMARK_NO_MAIN
  BENCHMARK_MAIN();
#endif
