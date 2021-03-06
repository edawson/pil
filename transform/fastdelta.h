/*
* Copyright (c) 2016 Daniel Lemire
* Author(s): Daniel Lemire
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
* KIND, either express or implied.  See the License for the
* specific language governing permissions and limitations
* under the License.
*/
/***
* These functions compute fast successive differences, and recover the original
* values from the fast successive differences (i.e., they compute a prefix sum)
* using fast SIMD instructions.
*
* Reference :
* Daniel Lemire, Nathan Kurz, Leonid Boytsov, SIMD Compression and the Intersection of Sorted
* Integers, Software: Practice and Experience (to appear)
* http://arxiv.org/abs/1401.6399
*/
#ifndef FASTDELTA_H_
#define FASTDELTA_H_

#include <stdint.h>
#include <stddef.h>

namespace pil {

// write to output the successive differences of input (input[0]-starting_point, input[1]-input[2], ...)
// there are "length" values in input and output
// input and output must be distinct
// it can make sense to set to zero by default
void compute_deltas(const uint32_t * __restrict__ input, size_t length, uint32_t * __restrict__ output, uint32_t starting_point);

// write to buffer the successive differences of buffer (buffer[0]-starting_point, buffer[1]-buffer[2], ...)
// there are "length" values in buffer
// it can make sense to set to zero by default
void compute_deltas_inplace(uint32_t * buffer, size_t length, uint32_t starting_point);

// write to output the successive differences of input (input[0]-starting_point, input[1]-input[2], ...)
// there are "length" values in input and output
// input and output must be distinct
void compute_prefix_sum(const uint32_t * __restrict__ input, size_t length, uint32_t * __restrict__ output, uint32_t starting_point);

// write to buffer the successive differences of buffer (buffer[0]-starting_point, buffer[1]-buffer[2], ...)
// there are "length" values in buffer
void compute_prefix_sum_inplace(uint32_t * buffer, size_t length, uint32_t starting_point);

}

#endif /* FASTDELTA_H_ */
