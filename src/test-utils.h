// Copyright 2022 Cartesi Pte. Ltd.
//
// This file is part of the machine-emulator. The machine-emulator is free
// software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// The machine-emulator is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with the machine-emulator. If not, see http://www.gnu.org/licenses/.

#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "i-hasher.h"
#include "keccak-256-hasher.h"
#include "machine-c-api.h"
#include "machine-merkle-tree.h"
#include "pma-constants.h"

using hash_type = cartesi::keccak_256_hasher::hash_type;

std::array zero_keccak_hash_table{hash_type{}, hash_type{}, hash_type{},
    hash_type{0x01, 0x1b, 0x4d, 0x03, 0xdd, 0x8c, 0x01, 0xf1, 0x04, 0x91, 0x43, 0xcf, 0x9c, 0x4c, 0x81, 0x7e, 0x4b,
        0x16, 0x7f, 0x1d, 0x1b, 0x83, 0xe5, 0xc6, 0xf0, 0xf1, 0x0d, 0x89, 0xba, 0x1e, 0x7b, 0xce},
    hash_type{0x4d, 0x94, 0x70, 0xa8, 0x21, 0xfb, 0xe9, 0x01, 0x17, 0xec, 0x35, 0x7e, 0x30, 0xba, 0xd9, 0x30, 0x57,
        0x32, 0xfb, 0x19, 0xdd, 0xf5, 0x4a, 0x07, 0xdd, 0x3e, 0x29, 0xf4, 0x40, 0x61, 0x92, 0x54},
    hash_type{0xae, 0x39, 0xce, 0x85, 0x37, 0xac, 0xa7, 0x5e, 0x2e, 0xff, 0x3e, 0x38, 0xc9, 0x80, 0x11, 0xdf, 0xe9,
        0x34, 0xe7, 0x00, 0xa0, 0x96, 0x77, 0x32, 0xfc, 0x07, 0xb4, 0x30, 0xdd, 0x65, 0x6a, 0x23},
    hash_type{0x3f, 0xc9, 0xa1, 0x5f, 0x5b, 0x48, 0x69, 0xc8, 0x72, 0xf8, 0x10, 0x87, 0xbb, 0x61, 0x04, 0xb7, 0xd6,
        0x3e, 0x6f, 0x9a, 0xb4, 0x7f, 0x2c, 0x43, 0xf3, 0x53, 0x5e, 0xae, 0x71, 0x72, 0xaa, 0x7f},
    hash_type{0x17, 0xd2, 0xdd, 0x61, 0x4c, 0xdd, 0xaa, 0x4d, 0x87, 0x92, 0x76, 0xb1, 0x1e, 0x06, 0x72, 0xc9, 0x56,
        0x00, 0x33, 0xd3, 0xe8, 0x45, 0x3a, 0x1d, 0x04, 0x53, 0x39, 0xd3, 0x4b, 0xa6, 0x01, 0xb9},
    hash_type{0xc3, 0x7b, 0x8b, 0x13, 0xca, 0x95, 0x16, 0x6f, 0xb7, 0xaf, 0x16, 0x98, 0x8a, 0x70, 0xfc, 0xc9, 0x0f,
        0x38, 0xbf, 0x91, 0x26, 0xfd, 0x83, 0x3d, 0xa7, 0x10, 0xa4, 0x7f, 0xb3, 0x7a, 0x55, 0xe6},
    hash_type{0x8e, 0x7a, 0x42, 0x7f, 0xa9, 0x43, 0xd9, 0x96, 0x6b, 0x38, 0x9f, 0x4f, 0x25, 0x71, 0x73, 0x67, 0x60,
        0x90, 0xc6, 0xe9, 0x5f, 0x43, 0xe2, 0xcb, 0x6d, 0x65, 0xf8, 0x75, 0x81, 0x11, 0xe3, 0x09},
    hash_type{0x30, 0xb0, 0xb9, 0xde, 0xb7, 0x3e, 0x15, 0x5c, 0x59, 0x74, 0x0b, 0xac, 0xf1, 0x4a, 0x6f, 0xf0, 0x4b,
        0x64, 0xbb, 0x8e, 0x20, 0x1a, 0x50, 0x64, 0x09, 0xc3, 0xfe, 0x38, 0x1c, 0xa4, 0xea, 0x90},
    hash_type{0xcd, 0x5d, 0xea, 0xc7, 0x29, 0xd0, 0xfd, 0xac, 0xcc, 0x44, 0x1d, 0x09, 0xd7, 0x32, 0x5f, 0x41, 0x58,
        0x6b, 0xa1, 0x3c, 0x80, 0x1b, 0x7e, 0xcc, 0xae, 0x0f, 0x95, 0xd8, 0xf3, 0x93, 0x3e, 0xfe},
    hash_type{0xd8, 0xb9, 0x6e, 0x5b, 0x7f, 0x6f, 0x45, 0x9e, 0x9c, 0xb6, 0xa2, 0xf4, 0x1b, 0xf2, 0x76, 0xc7, 0xb8,
        0x5c, 0x10, 0xcd, 0x46, 0x62, 0xc0, 0x4c, 0xbb, 0xb3, 0x65, 0x43, 0x47, 0x26, 0xc0, 0xa0},
    hash_type{0xc9, 0x69, 0x53, 0x93, 0x02, 0x7f, 0xb1, 0x06, 0xa8, 0x15, 0x31, 0x09, 0xac, 0x51, 0x62, 0x88, 0xa8,
        0x8b, 0x28, 0xa9, 0x38, 0x17, 0x89, 0x94, 0x60, 0xd6, 0x31, 0x0b, 0x71, 0xcf, 0x1e, 0x61},
    hash_type{0x63, 0xe8, 0x80, 0x6f, 0xa0, 0xd4, 0xb1, 0x97, 0xa2, 0x59, 0xe8, 0xc3, 0xac, 0x28, 0x86, 0x42, 0x68,
        0x15, 0x9d, 0x0a, 0xc8, 0x5f, 0x85, 0x81, 0xca, 0x28, 0xfa, 0x7d, 0x2c, 0x0c, 0x03, 0xeb},
    hash_type{0x91, 0xe3, 0xee, 0xe5, 0xca, 0x7a, 0x3d, 0xa2, 0xb3, 0x05, 0x3c, 0x97, 0x70, 0xdb, 0x73, 0x59, 0x9f,
        0xb1, 0x49, 0xf6, 0x20, 0xe3, 0xfa, 0xce, 0xf9, 0x5e, 0x94, 0x7c, 0x0e, 0xe8, 0x60, 0xb7},
    hash_type{0x21, 0x22, 0xe3, 0x1e, 0x4b, 0xbd, 0x2b, 0x7c, 0x78, 0x3d, 0x79, 0xcc, 0x30, 0xf6, 0x0c, 0x62, 0x38,
        0x65, 0x1d, 0xa7, 0xf0, 0x72, 0x6f, 0x76, 0x7d, 0x22, 0x74, 0x72, 0x64, 0xfd, 0xb0, 0x46},
    hash_type{0xf7, 0x54, 0x9f, 0x26, 0xcc, 0x70, 0xed, 0x5e, 0x18, 0xba, 0xeb, 0x6c, 0x81, 0xbb, 0x06, 0x25, 0xcb,
        0x95, 0xbb, 0x40, 0x19, 0xae, 0xec, 0xd4, 0x07, 0x74, 0xee, 0x87, 0xae, 0x29, 0xec, 0x51},
    hash_type{0x7a, 0x71, 0xf6, 0xee, 0x26, 0x4c, 0x5d, 0x76, 0x13, 0x79, 0xb3, 0xd7, 0xd6, 0x17, 0xca, 0x83, 0x67,
        0x73, 0x74, 0xb4, 0x9d, 0x10, 0xae, 0xc5, 0x05, 0x05, 0xac, 0x08, 0x74, 0x08, 0xca, 0x89},
    hash_type{0x2b, 0x57, 0x3c, 0x26, 0x7a, 0x71, 0x2a, 0x52, 0xe1, 0xd0, 0x64, 0x21, 0xfe, 0x27, 0x6a, 0x03, 0xef,
        0xb1, 0x88, 0x9f, 0x33, 0x72, 0x01, 0x11, 0x0f, 0xdc, 0x32, 0xa8, 0x1f, 0x8e, 0x15, 0x24},
    hash_type{0x99, 0xaf, 0x66, 0x58, 0x35, 0xaa, 0xbf, 0xdc, 0x67, 0x40, 0xc7, 0xe2, 0xc3, 0x79, 0x1a, 0x31, 0xc3,
        0xcd, 0xc9, 0xf5, 0xab, 0x96, 0x2f, 0x68, 0x1b, 0x12, 0xfc, 0x09, 0x28, 0x16, 0xa6, 0x2f},
    hash_type{0x27, 0xd8, 0x60, 0x25, 0x59, 0x9a, 0x41, 0x23, 0x38, 0x48, 0x70, 0x2f, 0x0c, 0xfc, 0x04, 0x37, 0xb4,
        0x45, 0x68, 0x2d, 0xf5, 0x11, 0x47, 0xa6, 0x32, 0xa0, 0xa0, 0x83, 0xd2, 0xd3, 0x8b, 0x5e},
    hash_type{0x13, 0xe4, 0x66, 0xa8, 0x93, 0x5a, 0xff, 0xf5, 0x8b, 0xb5, 0x33, 0xb3, 0xef, 0x5d, 0x27, 0xfb, 0xa6,
        0x3e, 0xe6, 0xb0, 0xfd, 0x9e, 0x67, 0xff, 0x20, 0xaf, 0x9d, 0x50, 0xde, 0xee, 0x3f, 0x8b},
    hash_type{0xf0, 0x65, 0xec, 0x22, 0x0c, 0x1f, 0xd4, 0xba, 0x57, 0xe3, 0x41, 0x26, 0x1d, 0x55, 0x99, 0x7f, 0x85,
        0xd6, 0x6d, 0x32, 0x15, 0x25, 0x26, 0x73, 0x68, 0x72, 0x69, 0x3d, 0x2b, 0x43, 0x7a, 0x23},
    hash_type{0x3e, 0x23, 0x37, 0xb7, 0x15, 0xf6, 0xac, 0x9a, 0x6a, 0x27, 0x26, 0x22, 0xfd, 0xc2, 0xd6, 0x7f, 0xcf,
        0xe1, 0xda, 0x34, 0x59, 0xf8, 0xda, 0xb4, 0xed, 0x7e, 0x40, 0xa6, 0x57, 0xa5, 0x4c, 0x36},
    hash_type{0x76, 0x6c, 0x5e, 0x8a, 0xc9, 0xa8, 0x8b, 0x35, 0xb0, 0x5c, 0x34, 0x74, 0x7e, 0x65, 0x07, 0xf6, 0xb0,
        0x44, 0xab, 0x66, 0x18, 0x0d, 0xc7, 0x6a, 0xc1, 0xa6, 0x96, 0xde, 0x03, 0x18, 0x95, 0x93},
    hash_type{0xfe, 0xdc, 0x0d, 0x0d, 0xbb, 0xd8, 0x55, 0xc8, 0xea, 0xd6, 0x73, 0x54, 0x48, 0x99, 0xb0, 0x96, 0x0e,
        0x4a, 0x5a, 0x7c, 0xa4, 0x3b, 0x4e, 0xf9, 0x0a, 0xfe, 0x60, 0x7d, 0xe7, 0x69, 0x8c, 0xae},
    hash_type{0xfd, 0xc2, 0x42, 0x78, 0x8f, 0x65, 0x4b, 0x57, 0xa4, 0xfb, 0x32, 0xa7, 0x1b, 0x33, 0x5e, 0xf6, 0xff,
        0x9a, 0x4c, 0xc1, 0x18, 0xb2, 0x82, 0xb5, 0x3b, 0xdd, 0x6d, 0x61, 0x92, 0xb7, 0xa8, 0x2c},
    hash_type{0x3c, 0x51, 0x26, 0xb9, 0xc7, 0xe3, 0x3c, 0x8e, 0x5a, 0x5a, 0xc9, 0x73, 0x8b, 0x8b, 0xd3, 0x12, 0x47,
        0xfb, 0x74, 0x02, 0x05, 0x4f, 0x97, 0xb5, 0x73, 0xe8, 0xab, 0xb9, 0xfa, 0xad, 0x21, 0x9f},
    hash_type{0x4f, 0xd0, 0x85, 0xac, 0xea, 0xa7, 0xf5, 0x42, 0xd7, 0x87, 0xee, 0x41, 0x96, 0xd3, 0x65, 0xf3, 0xcc,
        0x56, 0x6e, 0x7b, 0xbc, 0xfb, 0xfd, 0x45, 0x12, 0x30, 0xc4, 0x8d, 0x80, 0x4c, 0x01, 0x7d},
    hash_type{0x21, 0xe2, 0xd8, 0xfa, 0x91, 0x4e, 0x25, 0x59, 0xbb, 0x72, 0xbf, 0x0a, 0xb7, 0x8c, 0x8a, 0xb9, 0x2f,
        0x00, 0xef, 0x0d, 0x0d, 0x57, 0x6e, 0xcc, 0xdd, 0x48, 0x6b, 0x64, 0x13, 0x8a, 0x41, 0x72},
    hash_type{0x67, 0x48, 0x57, 0xe5, 0x43, 0xd1, 0xd5, 0xb6, 0x39, 0x05, 0x8d, 0xd9, 0x08, 0x18, 0x65, 0x97, 0xe3,
        0x66, 0xad, 0x5f, 0x3d, 0x9c, 0x7c, 0xea, 0xff, 0x44, 0xd0, 0x4d, 0x15, 0x50, 0xb8, 0xd3},
    hash_type{0x3a, 0xbc, 0x75, 0x1d, 0xf0, 0x74, 0x37, 0x83, 0x4b, 0xa5, 0xac, 0xb3, 0x23, 0x28, 0xa3, 0x96, 0x99,
        0x4a, 0xeb, 0xb3, 0xc4, 0x0f, 0x75, 0x9c, 0x2d, 0x6d, 0x7a, 0x3c, 0xb5, 0x37, 0x7e, 0x55},
    hash_type{0xd5, 0xd2, 0x18, 0xef, 0x5a, 0x29, 0x6d, 0xda, 0x8d, 0xdc, 0x35, 0x5f, 0x3f, 0x50, 0xc3, 0xd0, 0xb6,
        0x60, 0xa5, 0x1d, 0xfa, 0x4d, 0x98, 0xa6, 0xa5, 0xa3, 0x35, 0x64, 0x55, 0x6c, 0xf8, 0x3c},
    hash_type{0x13, 0x73, 0xa8, 0x14, 0x64, 0x1d, 0x6a, 0x1d, 0xce, 0xf9, 0x7b, 0x88, 0x3f, 0xee, 0x61, 0xbb, 0x84,
        0xfe, 0x60, 0xa3, 0x40, 0x93, 0x40, 0x21, 0x7e, 0x62, 0x9c, 0xc7, 0xe4, 0xdc, 0xc9, 0x3b},
    hash_type{0x85, 0xd8, 0x82, 0x09, 0x21, 0xff, 0x58, 0x26, 0x14, 0x8b, 0x60, 0xe6, 0x93, 0x9a, 0xcd, 0x78, 0x38,
        0xe1, 0xd7, 0xf2, 0x05, 0x62, 0xbf, 0xf8, 0xee, 0x4b, 0x5e, 0xc4, 0xa0, 0x5a, 0xd9, 0x97},
    hash_type{0xa5, 0x7b, 0x97, 0x96, 0xfd, 0xcb, 0x2e, 0xda, 0x87, 0x88, 0x3c, 0x26, 0x40, 0xb0, 0x72, 0xb1, 0x40,
        0xb9, 0x46, 0xbf, 0xdf, 0x65, 0x75, 0xca, 0xcc, 0x06, 0x6f, 0xda, 0xe0, 0x4f, 0x69, 0x51},
    hash_type{0xe6, 0x36, 0x24, 0xcb, 0xd3, 0x16, 0xa6, 0x77, 0xca, 0xd5, 0x29, 0xbb, 0xe4, 0xe9, 0x7b, 0x91, 0x44,
        0xe4, 0xbc, 0x06, 0xc4, 0xaf, 0xd1, 0xde, 0x55, 0xdd, 0x3e, 0x11, 0x75, 0xf9, 0x04, 0x23},
    hash_type{0x84, 0x7a, 0x23, 0x0d, 0x34, 0xdf, 0xb7, 0x1e, 0xd5, 0x6f, 0x29, 0x65, 0xa7, 0xf6, 0xc7, 0x2e, 0x6a,
        0xa3, 0x3c, 0x24, 0xc3, 0x03, 0xfd, 0x67, 0x74, 0x5d, 0x63, 0x26, 0x56, 0xc5, 0xef, 0x90},
    hash_type{0xbe, 0xc8, 0x0f, 0x4f, 0x5d, 0x1d, 0xaa, 0x25, 0x19, 0x88, 0x82, 0x6c, 0xef, 0x37, 0x5c, 0x81, 0xc3,
        0x6b, 0xf4, 0x57, 0xe0, 0x96, 0x87, 0x05, 0x6f, 0x92, 0x46, 0x77, 0xcb, 0x0b, 0xcc, 0xf9},
    hash_type{0x8d, 0xff, 0x81, 0xe0, 0x14, 0xce, 0x25, 0xf2, 0xd1, 0x32, 0x49, 0x79, 0x23, 0xe2, 0x67, 0x36, 0x39,
        0x63, 0xcd, 0xf4, 0x30, 0x2c, 0x50, 0x49, 0xd6, 0x31, 0x31, 0xdc, 0x03, 0xfd, 0x95, 0xf6},
    hash_type{0x5d, 0x8b, 0x6a, 0xa5, 0x93, 0x4f, 0x81, 0x72, 0x52, 0xc0, 0x28, 0xc9, 0x0f, 0x56, 0xd4, 0x13, 0xb9,
        0xd5, 0xd1, 0x0d, 0x89, 0x79, 0x07, 0x07, 0xda, 0xe2, 0xfa, 0xbb, 0x24, 0x9f, 0x64, 0x99},
    hash_type{0x29, 0x92, 0x7c, 0x21, 0xdd, 0x71, 0xe3, 0xf6, 0x56, 0x82, 0x6d, 0xe5, 0x45, 0x1c, 0x5d, 0xa3, 0x75,
        0xaa, 0xde, 0xcb, 0xd5, 0x9d, 0x5e, 0xbf, 0x3a, 0x31, 0xfa, 0xe6, 0x5a, 0xc1, 0xb3, 0x16},
    hash_type{0xa1, 0x61, 0x1f, 0x1b, 0x27, 0x6b, 0x26, 0x53, 0x0f, 0x58, 0xd7, 0x24, 0x7d, 0xf4, 0x59, 0xce, 0x1f,
        0x86, 0xdb, 0x1d, 0x73, 0x4f, 0x6f, 0x81, 0x19, 0x32, 0xf0, 0x42, 0xce, 0xe4, 0x5d, 0x0e},
    hash_type{0x45, 0x53, 0x06, 0xd0, 0x10, 0x81, 0xbc, 0x33, 0x84, 0xf8, 0x2c, 0x5f, 0xb2, 0xaa, 0xca, 0xa1, 0x9d,
        0x89, 0xcd, 0xfa, 0x46, 0xcc, 0x91, 0x6e, 0xac, 0x61, 0x12, 0x14, 0x75, 0xba, 0x2e, 0x61},
    hash_type{0x91, 0xb4, 0xfe, 0xec, 0xbe, 0x17, 0x89, 0x71, 0x70, 0x21, 0xa1, 0x58, 0xac, 0xe5, 0xd0, 0x67, 0x44,
        0xb4, 0x0f, 0x55, 0x10, 0x76, 0xb6, 0x7c, 0xd6, 0x3a, 0xf6, 0x00, 0x07, 0xf8, 0xc9, 0x98},
    hash_type{0x76, 0xe1, 0x42, 0x48, 0x83, 0xa4, 0x5e, 0xc4, 0x9d, 0x49, 0x7d, 0xda, 0xf8, 0x08, 0xa5, 0x52, 0x1c,
        0xa7, 0x4a, 0x99, 0x9a, 0xb0, 0xb3, 0xc7, 0xaa, 0x9c, 0x80, 0xf8, 0x5e, 0x93, 0x97, 0x7e},
    hash_type{0xc6, 0x1c, 0xe6, 0x8b, 0x20, 0x30, 0x7a, 0x1a, 0x81, 0xf7, 0x1c, 0xa6, 0x45, 0xb5, 0x68, 0xfc, 0xd3,
        0x19, 0xcc, 0xbb, 0x5f, 0x65, 0x1e, 0x87, 0xb7, 0x07, 0xd3, 0x7c, 0x39, 0xe1, 0x5f, 0x94},
    hash_type{0x5e, 0xa6, 0x9e, 0x2f, 0x7c, 0x7d, 0x2c, 0xcc, 0x85, 0xb7, 0xe6, 0x54, 0xc0, 0x7e, 0x96, 0xf0, 0x63,
        0x6a, 0xe4, 0x04, 0x4f, 0xe0, 0xe3, 0x85, 0x90, 0xb4, 0x31, 0x79, 0x5a, 0xd0, 0xf8, 0x64},
    hash_type{0x7b, 0xdd, 0x61, 0x37, 0x13, 0xad, 0xa4, 0x93, 0xcc, 0x17, 0xef, 0xd3, 0x13, 0x20, 0x63, 0x80, 0xe6,
        0xa6, 0x85, 0xb8, 0x19, 0x84, 0x75, 0xbb, 0xd0, 0x21, 0xc6, 0xe9, 0xd9, 0x4d, 0xaa, 0xb2},
    hash_type{0x21, 0x49, 0x47, 0x12, 0x75, 0x06, 0x07, 0x3e, 0x44, 0xd5, 0x40, 0x8b, 0xa1, 0x66, 0xc5, 0x12, 0xa0,
        0xb8, 0x68, 0x05, 0xd0, 0x7f, 0x5a, 0x44, 0xd3, 0xc4, 0x17, 0x06, 0xbe, 0x2b, 0xc1, 0x5e},
    hash_type{0x71, 0x2e, 0x55, 0x80, 0x52, 0x48, 0xb9, 0x2e, 0x86, 0x77, 0xd9, 0x0f, 0x6d, 0x28, 0x4d, 0x1d, 0x6f,
        0xfa, 0xff, 0x2c, 0x43, 0x06, 0x57, 0x04, 0x2a, 0x0e, 0x82, 0x62, 0x4f, 0xa3, 0x71, 0x7b},
    hash_type{0x06, 0xcc, 0x0a, 0x6f, 0xd1, 0x22, 0x30, 0xea, 0x58, 0x6d, 0xae, 0x83, 0x01, 0x9f, 0xb9, 0xe0, 0x60,
        0x34, 0xed, 0x28, 0x03, 0xc9, 0x8d, 0x55, 0x4b, 0x93, 0xc9, 0xa5, 0x23, 0x48, 0xca, 0xff},
    hash_type{0xf7, 0x5c, 0x40, 0x17, 0x4a, 0x91, 0xf9, 0xae, 0x6b, 0x86, 0x47, 0x85, 0x4a, 0x15, 0x60, 0x29, 0xf0,
        0xb8, 0x8b, 0x83, 0x31, 0x66, 0x63, 0xce, 0x57, 0x4a, 0x49, 0x78, 0x27, 0x7b, 0xb6, 0xbb},
    hash_type{0x27, 0xa3, 0x10, 0x85, 0x63, 0x4b, 0x6e, 0xc7, 0x88, 0x64, 0xb6, 0xd8, 0x20, 0x1c, 0x7e, 0x93, 0x90,
        0x3d, 0x75, 0x81, 0x50, 0x67, 0xe3, 0x78, 0x28, 0x9a, 0x3d, 0x07, 0x2a, 0xe1, 0x72, 0xda},
    hash_type{0xfa, 0x6a, 0x45, 0x24, 0x70, 0xf8, 0xd6, 0x45, 0xbe, 0xbf, 0xad, 0x97, 0x79, 0x59, 0x4f, 0xc0, 0x78,
        0x4b, 0xb7, 0x64, 0xa2, 0x2e, 0x3a, 0x81, 0x81, 0xd9, 0x3d, 0xb7, 0xbf, 0x97, 0x89, 0x3c},
    hash_type{0x41, 0x42, 0x17, 0xa6, 0x18, 0xcc, 0xb1, 0x4c, 0xaa, 0x9e, 0x92, 0xe8, 0xc6, 0x16, 0x73, 0xaf, 0xc9,
        0x58, 0x36, 0x62, 0xe8, 0x12, 0xad, 0xba, 0x1f, 0x87, 0xa9, 0xc6, 0x82, 0x02, 0xd6, 0x0e},
    hash_type{0x90, 0x9e, 0xfa, 0xb4, 0x3c, 0x42, 0xc0, 0xcb, 0x00, 0x69, 0x5f, 0xc7, 0xf1, 0xff, 0xe6, 0x7c, 0x75,
        0xca, 0x89, 0x4c, 0x3c, 0x51, 0xe1, 0xe5, 0xe7, 0x31, 0x36, 0x01, 0x99, 0xe6, 0x00, 0xf6},
    hash_type{0xce, 0xd9, 0xa8, 0x7b, 0x2a, 0x6a, 0x87, 0xe7, 0x0b, 0xf2, 0x51, 0xbb, 0x50, 0x75, 0xab, 0x22, 0x21,
        0x38, 0x28, 0x81, 0x64, 0xb2, 0xed, 0xa7, 0x27, 0x51, 0x5e, 0xa7, 0xde, 0x12, 0xe2, 0x49},
    hash_type{0x6d, 0x4f, 0xe4, 0x2e, 0xa8, 0xd1, 0xa1, 0x20, 0xc0, 0x3c, 0xf9, 0xc5, 0x06, 0x22, 0xc2, 0xaf, 0xe4,
        0xac, 0xb0, 0xda, 0xd9, 0x8f, 0xd6, 0x2d, 0x07, 0xab, 0x4e, 0x82, 0x8a, 0x94, 0x49, 0x5f},
    hash_type{0x6d, 0x1a, 0xb9, 0x73, 0x98, 0x2c, 0x7c, 0xcb, 0xe6, 0xc1, 0xfa, 0xe0, 0x27, 0x88, 0xe4, 0x42, 0x2a,
        0xe2, 0x22, 0x82, 0xfa, 0x49, 0xcb, 0xdb, 0x04, 0xba, 0x54, 0xa7, 0xa2, 0x38, 0xc6, 0xfc},
    hash_type{0x41, 0x18, 0x74, 0x51, 0x38, 0x34, 0x60, 0x76, 0x2c, 0x06, 0xd1, 0xc8, 0xa7, 0x2b, 0x9c, 0xd7, 0x18,
        0x86, 0x6a, 0xd4, 0xb6, 0x89, 0xe1, 0x0c, 0x9a, 0x8c, 0x38, 0xfe, 0x5e, 0xf0, 0x45, 0xbd},
    hash_type{0x78, 0x5b, 0x01, 0xe9, 0x80, 0xfc, 0x82, 0xc7, 0xe3, 0x53, 0x2c, 0xe8, 0x18, 0x76, 0xb7, 0x78, 0xdd,
        0x9f, 0x1c, 0xee, 0xba, 0x44, 0x78, 0xe8, 0x64, 0x11, 0xfb, 0x6f, 0xdd, 0x79, 0x06, 0x83},
    hash_type{0x91, 0x6c, 0xa8, 0x32, 0x59, 0x24, 0x85, 0x09, 0x36, 0x44, 0xe8, 0x76, 0x0c, 0xd7, 0xb4, 0xc0, 0x1d,
        0xba, 0x1c, 0xcc, 0x82, 0xb6, 0x61, 0xbf, 0x13, 0xf0, 0xe3, 0xf3, 0x4a, 0xcd, 0x6b, 0x88}};

#define LOG2_WORD_SIZE 3

struct incremental_merkle_tree_of_pages {
    std::vector<hash_type> m_tree;
    int m_page_log2_size = 0;
    int m_tree_log2_size = 0;
    uint64_t m_page_count = 0;
    uint64_t m_max_pages = 0;

    incremental_merkle_tree_of_pages(int page_log2_size, int tree_log2_size) :
        m_page_log2_size(page_log2_size),
        m_tree_log2_size(tree_log2_size),
        m_page_count(0),
        m_max_pages(1 << (tree_log2_size - page_log2_size)) {
        m_tree.resize(2 * m_max_pages);
    }

    void add_page(const hash_type &new_page_hash) {
        cartesi::keccak_256_hasher h;
        hash_type right = new_page_hash;
        if (m_page_count >= m_max_pages) {
            throw std::out_of_range("Page count must be smaller than max pages");
        }
        int depth = m_tree_log2_size - m_page_log2_size;
        for (int i = 0; i <= depth; ++i) {
            if (m_page_count & (0x01 << i)) {
                hash_type left = m_tree[i];
                get_concat_hash(h, left, right, right);
            } else {
                m_tree[i] = right;
                break;
            }
        }
        ++m_page_count;
    }

    hash_type get_root_hash() const {
        if (m_page_count > m_max_pages) {
            throw std::out_of_range("Page count must be smaller or equal than max pages");
        }
        cartesi::keccak_256_hasher h;
        int depth = m_tree_log2_size - m_page_log2_size;
        if (m_page_count < m_max_pages) {
            hash_type root = zero_keccak_hash_table[m_page_log2_size];
            for (int i = 0; i < depth; ++i) {
                if (m_page_count & (0x01 << i)) {
                    auto left = m_tree[i];
                    get_concat_hash(h, left, root, root);
                } else {
                    auto right = zero_keccak_hash_table[m_page_log2_size + i];
                    get_concat_hash(h, root, right, root);
                }
            }
            return root;
        } else {
            return m_tree[depth];
        }
    }
};

// Calculate root hash for data buffer of log2_size
hash_type calculate_root_hash(const std::vector<uint8_t> &data, int log2_size) {
    cartesi::keccak_256_hasher h;
    hash_type result;
    if (log2_size < LOG2_WORD_SIZE) {
        throw std::invalid_argument("Wrong data size");
    } else if (log2_size > LOG2_WORD_SIZE) {
        --log2_size;
        uint64_t sz = data.size() / 2 + data.size() % 2;
        auto child1 = calculate_root_hash(std::vector<uint8_t>{data.cbegin(), data.cbegin() + sz}, log2_size);
        auto child2 = calculate_root_hash(std::vector<uint8_t>{data.cbegin() + sz, data.cend()}, log2_size);
        get_concat_hash(h, child1, child2, result);
    } else {
        h.add_data(data.data(), data.size());
        h.end(result);
    }
    return result;
}

// Taking memory region in buffer data_buffer, and occuping data_number_of_pages
// of page size page_log2_size
// calculate merke hash for region of up to tree_log2_size,
// using zero sibling hashes where needed
static hash_type calculate_region_hash(const std::vector<uint8_t> &data_buffer, int data_number_of_pages,
    int page_log2_size, int tree_log2_size) {
    int page_size = 1 << page_log2_size;
    auto incremental_tree = incremental_merkle_tree_of_pages(page_log2_size, tree_log2_size);

    for (int i = 0; i < data_number_of_pages; ++i) {
        auto current_page_data =
            std::vector<uint8_t>(data_buffer.begin() + i * page_size, data_buffer.begin() + (i + 1) * page_size);
        auto current_page_hash = calculate_root_hash(current_page_data, page_log2_size);
        incremental_tree.add_page(current_page_hash);
    }
    return incremental_tree.get_root_hash();
}

// Take data hash of some region and extend it with pristine space
// up to tree_log2_size, calculating target hash
static hash_type extend_region_hash(hash_type data_hash, uint64_t data_address, int data_log2_size,
    int tree_log2_size) {
    auto result_hash = data_hash;
    auto result_address = data_address;
    for (int n = data_log2_size + 1; n <= tree_log2_size; ++n) {
        cartesi::keccak_256_hasher h;
        if (result_address % (1 << n) == 0) {
            auto child1 = result_hash;
            auto child2 = zero_keccak_hash_table[n - 1];
            get_concat_hash(h, child1, child2, result_hash);
        } else {
            auto child1 = zero_keccak_hash_table[n - 1];
            auto child2 = result_hash;
            get_concat_hash(h, child1, child2, result_hash);
#pragma GCC diagnostic ignored "-Wshift-negative-value"
            result_address = result_address & (~0x01 << (n - 1));
        }
    }

    return result_hash;
}

// Taking memory region with starting data_address and log2_data_size
// calculate merke hash for region of up to log2_result_address_space,
// using zero sibling hashes where needed. Data_address may not be aligned
// to the beginning of the log2_result_address_space
static hash_type calculate_region_hash_2(uint64_t data_address, const std::vector<uint8_t> data_buffer,
    int log2_data_size, int log2_result_address_space) {
#pragma GCC diagnostic ignored "-Wshift-negative-value"
    data_address = data_address & (~0x01 << (log2_data_size - 1));
    auto data_hash = calculate_root_hash(data_buffer, log2_data_size);

    auto result_hash = data_hash;
    auto result_address = data_address;
    for (int n = log2_data_size + 1; n <= log2_result_address_space; ++n) {
        cartesi::keccak_256_hasher h;
        if (result_address % (1 << n) == 0) {
            auto child1 = result_hash;
            auto child2 = zero_keccak_hash_table[n - 1];
            get_concat_hash(h, child1, child2, result_hash);
        } else {
            auto child1 = zero_keccak_hash_table[n - 1];
            auto child2 = result_hash;
            get_concat_hash(h, child1, child2, result_hash);
#pragma GCC diagnostic ignored "-Wshift-negative-value"
            result_address = result_address & (~0x01 << (n - 1));
        }
    }
    return result_hash;
}

static hash_type calculate_proof_root_hash(const cm_merkle_tree_proof *proof) {
    hash_type hash;
    memcpy(hash.data(), proof->target_hash, sizeof(cm_hash));
    for (int log2_size = static_cast<int>(proof->log2_target_size); log2_size < static_cast<int>(proof->log2_root_size);
         ++log2_size) {
        cartesi::keccak_256_hasher h;
        auto bit = (proof->target_address & (1 << log2_size));
        hash_type first, second;
        if (bit) {
            memcpy(first.data(), proof->sibling_hashes.entry[proof->log2_root_size - log2_size - 1], sizeof(cm_hash));
            second = hash;
        } else {
            first = hash;
            memcpy(second.data(), proof->sibling_hashes.entry[proof->log2_root_size - log2_size - 1], sizeof(cm_hash));
        }
        get_concat_hash(h, first, second, hash);
    }
    return hash;
}

static std::vector<uint8_t> parse_pma_file(const std::string &path) {
    std::streampos size;
    std::ifstream file(path, std::ios::binary);

    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char *>(&data[0]), size);
    return data;
}

static int ceil_log2(uint64_t x) {
    return static_cast<int>(std::ceil(std::log2(static_cast<double>(x))));
}

static hash_type calculate_emulator_hash(const std::array<const char *, 7> &pmas_files) {
    using namespace cartesi;
    cartesi::keccak_256_hasher h;
    auto shadow_state = parse_pma_file(pmas_files[0]);
    auto rom = parse_pma_file(pmas_files[1]);
    auto shadow_pmas = parse_pma_file(pmas_files[2]);
    auto shadow_tlb = parse_pma_file(pmas_files[3]);
    auto clint = parse_pma_file(pmas_files[4]);
    auto htif = parse_pma_file(pmas_files[5]);
    auto ram = parse_pma_file(pmas_files[6]);

    std::vector<uint8_t> shadow_rom;
    shadow_rom.reserve(shadow_state.size() + rom.size() + shadow_pmas.size());
    shadow_rom.insert(shadow_rom.end(), shadow_state.begin(), shadow_state.end());
    shadow_rom.insert(shadow_rom.end(), rom.begin(), rom.end());
    shadow_rom.insert(shadow_rom.end(), shadow_pmas.begin(), shadow_pmas.end());

    hash_type shadow_rom_tlb_space_hash;
    hash_type shadow_rom_tlb_clint_hash;
    hash_type left;
    hash_type used_space_hash;

    int shadow_rom_hash_size_log2 = ceil_log2(PMA_SHADOW_STATE_LENGTH + PMA_ROM_LENGTH + PMA_SHADOW_PMAS_LENGTH);
    auto shadow_rom_space_hash = calculate_region_hash(shadow_rom,
        (shadow_rom.size() + PMA_PAGE_SIZE - 1) / PMA_PAGE_SIZE, PMA_PAGE_SIZE_LOG2, shadow_rom_hash_size_log2);
    shadow_rom_space_hash = extend_region_hash(shadow_rom_space_hash, 0, shadow_rom_hash_size_log2, 17);

    auto tlb_size_log2 = ceil_log2(PMA_SHADOW_TLB_LENGTH);
    auto tlb_space_hash = calculate_region_hash(shadow_tlb, (shadow_tlb.size() + PMA_PAGE_SIZE - 1) / PMA_PAGE_SIZE,
        PMA_PAGE_SIZE_LOG2, tlb_size_log2);
    tlb_space_hash = extend_region_hash(tlb_space_hash, PMA_SHADOW_TLB_START, tlb_size_log2, 17);

    get_concat_hash(h, shadow_rom_space_hash, tlb_space_hash, shadow_rom_tlb_space_hash); // 18
    shadow_rom_tlb_space_hash = extend_region_hash(shadow_rom_tlb_space_hash, 0, 18, 25);

    auto clint_size_log2 = ceil_log2(PMA_CLINT_LENGTH);
    auto clint_space_hash = calculate_region_hash(clint, (clint.size() + PMA_PAGE_SIZE - 1) / PMA_PAGE_SIZE,
        PMA_PAGE_SIZE_LOG2, clint_size_log2);
    clint_space_hash = extend_region_hash(clint_space_hash, PMA_CLINT_START, clint_size_log2, 25);

    get_concat_hash(h, shadow_rom_tlb_space_hash, clint_space_hash, shadow_rom_tlb_clint_hash); // 26
    shadow_rom_tlb_clint_hash = extend_region_hash(shadow_rom_tlb_clint_hash, 0, 26, 30);

    uint64_t htif_size_log2 = ceil_log2(htif.size());
    auto htif_space_hash = calculate_region_hash_2(PMA_HTIF_START, htif, htif_size_log2, 30);
    get_concat_hash(h, shadow_rom_tlb_clint_hash, htif_space_hash, left); // 31

    uint64_t ram_size_log2 = ceil_log2(ram.size());
    auto ram_space_hash = calculate_region_hash_2(PMA_RAM_START, ram, ram_size_log2, 31);
    get_concat_hash(h, left, ram_space_hash, used_space_hash); // 32

    return extend_region_hash(used_space_hash, 0, 32, 64);
}
