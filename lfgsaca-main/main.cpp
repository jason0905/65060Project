/*
 * main.cpp for FGSACA
 * Copyright (c) 2022 Jannik Olbrich All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


// compile with
//   g++ main.cpp -o main -std=c++20 -fopenmp -lpthread -I nearest-smaller-suffixes/include

#include "fgsaca.hpp"
#include "fgsaca_util.hpp"

#include <iostream>
#include <vector>

template<typename T>
std::ostream& operator<<(std::ostream&out, const std::vector<T>& v) {
	out << "[";
	for(size_t i = 0; i < v.size(); i++) {
		if (i > 0) out << ", ";
		if constexpr (std::is_same_v<T, uint8_t>)
			out << (uint32_t)v[i];
		else
			out << v[i];
	}
	return out << "]";
}

using Ix = uint32_t;
void test_SA(std::string_view S) {
	const size_t n = S.size();
	std::cout << "computing SA of \"" << S << '\"' << std::endl;
	std::vector<Ix> SA(n);

	fgsaca<Ix, uint8_t>((const uint8_t*) S.data(), SA.data(), n, 256);

	std::cout << "SA: " << SA << std::endl;
	if (auto e = fgsaca_internal::validateSA<Ix>((const uint8_t*) S.data(), n, 256, SA.data()); e != std::nullopt) {
		std::cerr << "SA validation failed: " << *e << std::endl;
		exit(1);
	}
}
void test_BBWT(std::string_view S) {
	const size_t n = S.size();
	std::cout << "computing BBWT of \"" << S << '\"' << std::endl;
	std::string bbwt(n, '?');

	fbbwt<uint8_t>((const uint8_t*) S.data(), (uint8_t*) bbwt.data(), n, 256);

	std::cout << "BBWT: \"" << bbwt << '\"'<< std::endl;
	if (not fgsaca_internal::check_bbwt((const uint8_t*) S.data(), n, (const uint8_t*) bbwt.data())) {
		std::cerr << "BBWT validation failed" << std::endl;
		exit(1);
	}
}
void test_eBWT(const std::vector<std::string>& S) {
	const size_t k = S.size();
	std::cout << "computing eBWT of " << S << std::endl;
	const size_t n = [&] {
			size_t r = 0;
			for (const auto& s : S) r += s.size();
			return r;
		}();
	std::string ebwt(n, '?');
	std::vector<Ix> idx(k);

	febwt<Ix>(S.data(), ebwt.data(), idx.data(), k, 256);

	std::cout << "eBWT: \"" << ebwt << '\"' << std::endl;
	std::cout << "I: " << idx << std::endl;
}

int main() {
	test_SA("acedcebceece");
	test_BBWT("acedcebceece");
	test_eBWT({"b", "bcbc", "b", "bc", "abcbc"});

	return 0;
}
