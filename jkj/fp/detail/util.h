// Copyright 2020 Junekey Jeon
//
// The contents of this file may be used under the terms of
// the Apache License v2.0 with LLVM Exceptions.
//
//    (See accompanying file LICENSE-Apache or copy at
//     https://llvm.org/foundation/relicensing/LICENSE.txt)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.

#ifndef JKJ_HEADER_FP_UTIL
#define JKJ_HEADER_FP_UTIL

#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

namespace jkj::fp {
	namespace detail {
		template <class T>
		constexpr std::size_t physical_bits = sizeof(T) * std::numeric_limits<unsigned char>::digits;

		template <class T>
		constexpr std::size_t value_bits =
			std::numeric_limits<std::enable_if_t<std::is_unsigned_v<T>, T>>::digits;

		template <int k, class Int>
		constexpr Int compute_power(Int a) noexcept {
			static_assert(k >= 0);
			Int p = 1;
			for (int i = 0; i < k; ++i) {
				p *= a;
			}
			return p;
		}

		template <int a, class UInt>
		constexpr int count_factors(UInt n) noexcept {
			static_assert(a > 1);
			int c = 0;
			while (n % a == 0) {
				n /= a;
				++c;
			}
			return c;
		}

		template <class UInt, UInt a, std::size_t starting_power, class PowerList>
		struct pow_table_impl;

		template <class UInt, UInt a, std::size_t starting_power, std::size_t... offsets>
		struct pow_table_impl<UInt, a, starting_power, std::index_sequence<offsets...>> {
			static_assert(std::is_unsigned_v<UInt>);
			static constexpr UInt table[] = { compute_power<starting_power + offsets>(a)... };
		};

		template <class UInt, UInt a, std::size_t table_size, std::size_t starting_power = 0>
		using pow_table = pow_table_impl<UInt, a, starting_power, std::make_index_sequence<table_size>>;

		// C++20 std::remove_cvref_t
		template <class T>
		struct remove_cvref {
			using type = std::remove_cv_t<std::remove_reference_t<T>>;
		};
		template <class T>
		using remove_cvref_t = typename remove_cvref<T>::type;

		// A simple utility class
		template <class... T>
		struct typelist {};
	}
}

#endif