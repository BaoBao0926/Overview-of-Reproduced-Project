/*
Copyright (c) MONAI Consortium
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

// Helper Macros: for internal use (see below)
#define _DO_1(TARGET) TARGET(1)
#define _DO_2(TARGET) TARGET(2) _DO_1(TARGET)
#define _DO_3(TARGET) TARGET(3) _DO_2(TARGET)
#define _DO_4(TARGET) TARGET(4) _DO_3(TARGET)
#define _DO_5(TARGET) TARGET(5) _DO_4(TARGET)
#define _DO_6(TARGET) TARGET(6) _DO_5(TARGET)
#define _DO_7(TARGET) TARGET(7) _DO_6(TARGET)
#define _DO_8(TARGET) TARGET(8) _DO_7(TARGET)
#define _DO_9(TARGET) TARGET(9) _DO_8(TARGET)
#define _DO_10(TARGET) TARGET(10) _DO_9(TARGET)
#define _DO_11(TARGET) TARGET(11) _DO_10(TARGET)
#define _DO_12(TARGET) TARGET(12) _DO_11(TARGET)
#define _DO_13(TARGET) TARGET(13) _DO_12(TARGET)
#define _DO_14(TARGET) TARGET(14) _DO_13(TARGET)
#define _DO_15(TARGET) TARGET(15) _DO_14(TARGET)
#define _DO_16(TARGET) TARGET(16) _DO_15(TARGET)
#define _DO_17(TARGET) TARGET(17) _DO_16(TARGET)
#define _DO_18(TARGET) TARGET(18) _DO_17(TARGET)
#define _DO_19(TARGET) TARGET(19) _DO_18(TARGET)
#define _DO_20(TARGET) TARGET(20) _DO_19(TARGET)
#define _DO_21(TARGET) TARGET(21) _DO_20(TARGET)
#define _DO_22(TARGET) TARGET(22) _DO_21(TARGET)
#define _DO_23(TARGET) TARGET(23) _DO_22(TARGET)
#define _DO_24(TARGET) TARGET(24) _DO_23(TARGET)
#define _DO_25(TARGET) TARGET(25) _DO_24(TARGET)
#define _DO_26(TARGET) TARGET(26) _DO_25(TARGET)
#define _DO_27(TARGET) TARGET(27) _DO_26(TARGET)
#define _DO_28(TARGET) TARGET(28) _DO_27(TARGET)
#define _DO_29(TARGET) TARGET(29) _DO_28(TARGET)
#define _DO_30(TARGET) TARGET(30) _DO_29(TARGET)
#define _DO_31(TARGET) TARGET(31) _DO_30(TARGET)
#define _DO_32(TARGET) TARGET(32) _DO_31(TARGET)

#define _DO_A_1(TARGET, A) TARGET(A, 1)
#define _DO_A_2(TARGET, A) TARGET(A, 2) _DO_A_1(TARGET, A)
#define _DO_A_3(TARGET, A) TARGET(A, 3) _DO_A_2(TARGET, A)
#define _DO_A_4(TARGET, A) TARGET(A, 4) _DO_A_3(TARGET, A)
#define _DO_A_5(TARGET, A) TARGET(A, 5) _DO_A_4(TARGET, A)
#define _DO_A_6(TARGET, A) TARGET(A, 6) _DO_A_5(TARGET, A)
#define _DO_A_7(TARGET, A) TARGET(A, 7) _DO_A_6(TARGET, A)
#define _DO_A_8(TARGET, A) TARGET(A, 8) _DO_A_7(TARGET, A)
#define _DO_A_9(TARGET, A) TARGET(A, 9) _DO_A_8(TARGET, A)
#define _DO_A_10(TARGET, A) TARGET(A, 10) _DO_A_9(TARGET, A)
#define _DO_A_11(TARGET, A) TARGET(A, 11) _DO_A_10(TARGET, A)
#define _DO_A_12(TARGET, A) TARGET(A, 12) _DO_A_11(TARGET, A)
#define _DO_A_13(TARGET, A) TARGET(A, 13) _DO_A_12(TARGET, A)
#define _DO_A_14(TARGET, A) TARGET(A, 14) _DO_A_13(TARGET, A)
#define _DO_A_15(TARGET, A) TARGET(A, 15) _DO_A_14(TARGET, A)
#define _DO_A_16(TARGET, A) TARGET(A, 16) _DO_A_15(TARGET, A)
#define _DO_A_17(TARGET, A) TARGET(A, 17) _DO_A_16(TARGET, A)
#define _DO_A_18(TARGET, A) TARGET(A, 18) _DO_A_17(TARGET, A)
#define _DO_A_19(TARGET, A) TARGET(A, 19) _DO_A_18(TARGET, A)
#define _DO_A_20(TARGET, A) TARGET(A, 20) _DO_A_19(TARGET, A)
#define _DO_A_21(TARGET, A) TARGET(A, 21) _DO_A_20(TARGET, A)
#define _DO_A_22(TARGET, A) TARGET(A, 22) _DO_A_21(TARGET, A)
#define _DO_A_23(TARGET, A) TARGET(A, 23) _DO_A_22(TARGET, A)
#define _DO_A_24(TARGET, A) TARGET(A, 24) _DO_A_23(TARGET, A)
#define _DO_A_25(TARGET, A) TARGET(A, 25) _DO_A_24(TARGET, A)
#define _DO_A_26(TARGET, A) TARGET(A, 26) _DO_A_25(TARGET, A)
#define _DO_A_27(TARGET, A) TARGET(A, 27) _DO_A_26(TARGET, A)
#define _DO_A_28(TARGET, A) TARGET(A, 28) _DO_A_27(TARGET, A)
#define _DO_A_29(TARGET, A) TARGET(A, 29) _DO_A_28(TARGET, A)
#define _DO_A_30(TARGET, A) TARGET(A, 30) _DO_A_29(TARGET, A)
#define _DO_A_31(TARGET, A) TARGET(A, 31) _DO_A_30(TARGET, A)
#define _DO_A_32(TARGET, A) TARGET(A, 32) _DO_A_31(TARGET, A)

#define _DO_1_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 1)
#define _DO_2_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 2) _DO_1_B(TARGET, B_RANGE)
#define _DO_3_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 3) _DO_2_B(TARGET, B_RANGE)
#define _DO_4_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 4) _DO_3_B(TARGET, B_RANGE)
#define _DO_5_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 5) _DO_4_B(TARGET, B_RANGE)
#define _DO_6_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 6) _DO_5_B(TARGET, B_RANGE)
#define _DO_7_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 7) _DO_6_B(TARGET, B_RANGE)
#define _DO_8_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 8) _DO_7_B(TARGET, B_RANGE)
#define _DO_9_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 9) _DO_8_B(TARGET, B_RANGE)
#define _DO_10_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 10) _DO_9_B(TARGET, B_RANGE)
#define _DO_11_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 11) _DO_10_B(TARGET, B_RANGE)
#define _DO_12_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 12) _DO_11_B(TARGET, B_RANGE)
#define _DO_13_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 13) _DO_12_B(TARGET, B_RANGE)
#define _DO_14_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 14) _DO_13_B(TARGET, B_RANGE)
#define _DO_15_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 15) _DO_14_B(TARGET, B_RANGE)
#define _DO_16_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 16) _DO_15_B(TARGET, B_RANGE)
#define _DO_17_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 17) _DO_16_B(TARGET, B_RANGE)
#define _DO_18_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 18) _DO_17_B(TARGET, B_RANGE)
#define _DO_19_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 19) _DO_18_B(TARGET, B_RANGE)
#define _DO_20_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 20) _DO_19_B(TARGET, B_RANGE)
#define _DO_21_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 21) _DO_20_B(TARGET, B_RANGE)
#define _DO_22_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 22) _DO_21_B(TARGET, B_RANGE)
#define _DO_23_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 23) _DO_22_B(TARGET, B_RANGE)
#define _DO_24_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 24) _DO_23_B(TARGET, B_RANGE)
#define _DO_25_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 25) _DO_24_B(TARGET, B_RANGE)
#define _DO_26_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 26) _DO_25_B(TARGET, B_RANGE)
#define _DO_27_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 27) _DO_26_B(TARGET, B_RANGE)
#define _DO_28_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 28) _DO_27_B(TARGET, B_RANGE)
#define _DO_29_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 29) _DO_28_B(TARGET, B_RANGE)
#define _DO_30_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 30) _DO_29_B(TARGET, B_RANGE)
#define _DO_31_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 31) _DO_30_B(TARGET, B_RANGE)
#define _DO_32_B(TARGET, B_RANGE) _DO_A_##B_RANGE(TARGET, 32) _DO_31_B(TARGET, B_RANGE)

#define _CASE_A(A) \
  case (A):        \
    CASE(A) break;
#define _CASE_AB(A, B) \
  case (A * 100 + B):  \
    CASE(A, B) break;

// Preproccessor For Loops
#define DO_FOR_A(TARGET, A_RANGE) _DO_##A_RANGE(TARGET)
#define DO_FOR_AB(TARGET, A_RANGE, B_RANGE) _DO_##A_RANGE##_B(TARGET, B_RANGE)

// Preproccessor Switch Statement Generators
#define SWITCH_A(CASE, A_RANGE, A) \
  switch (A) { DO_FOR_A(_CASE_A, A_RANGE) }
#define SWITCH_AB(CALL, A_RANGE, B_RANGE, A, B) \
  switch (A * 100 + B) { DO_FOR_AB(_CASE_AB, A_RANGE, B_RANGE) }
