// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include "seal/util/croots.h"
#include <complex>
#include <iostream>
using namespace std;

namespace seal
{
    namespace util
    {
        ComplexRoots::ComplexRoots(size_t degree_of_roots, MemoryPoolHandle pool) : pool_(std::move(pool)), degree_of_roots_(degree_of_roots)
        {
#ifdef SEAL_DEBUG
            int power = util::get_power_of_two(degree_of_roots_);
            if (power < 0)
            {
                throw std::invalid_argument("degree_of_roots must be a power of two");
            }
            else if (power < 3)
            {
                throw std::invalid_argument("degree_of_roots must be at least 8");
            }
#endif
            roots_ = allocate<complex<double>>(degree_of_roots_ / 8 + 1, pool_);

            // Generate 1/8 of all roots.
            // Alternatively, choose from precomputed high-precision roots in files.
            for (size_t i = 0; i <= degree_of_roots_ / 8; i++)
            {
                roots_[i] = std::polar<double>(1.0, 2 * PI_ * static_cast<double>(i) / degree_of_roots_);
            }
        }

        SEAL_NODISCARD complex<double> ComplexRoots::get_root(size_t index) const
        {
            index &= degree_of_roots_ - 1;
            auto mirror = [](complex<double> a) { return complex<double>{a.imag(), a.real()}; };

            // This express the 8-fold symmetry of all n-th roots.
            if (index <= degree_of_roots_ / 8)
            {
                return roots_[index];
            }
            else if (index <= degree_of_roots_ / 4)
            {
                return mirror(roots_[degree_of_roots_ / 4 - index]);
            }
            else if (index <= degree_of_roots_ / 2)
            {
                return mirror(conj(get_root(index - degree_of_roots_ / 4)));
            }
            else {
                return -get_root(index - degree_of_roots_ / 2);
            }
        }
    } // namespace util
} // namespace seal
