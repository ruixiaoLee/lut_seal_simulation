# lut_seal_simulation
Our program is only tested on 64-bit platform.<br>
Revise the `CMakeList.txt` to compile the code you need.<br>
When you make LUT use the source code `makeTable.cpp`, please only available the part of code you need.<br>

## Prerequisites
Microsoft SEAL version 4.0.0, OpenFHE 1.0.3, CMake and OpenMP is needed.
[Microsoft/SEAL](https://github.com/microsoft/SEAL)
[OpenFHE](https://github.com/openfheorg/openfhe-development)

## Running

### Single-Input Function Evaluation with LUT
For one-input functions, you need to revise the LUT path in `searchInput.cpp` and `extractOutput.cpp`.
```
bin/keyGeneration
bin/makeTable
bin/makeInput
bin/searchInput
bin/makeQuery
bin/extractOutput
bin/checkResult
```
## Multi-Input Function Evaluation with LUT
For two-input functions, you need to revise the LUT path in `searchInputTwo.cpp` and `extractOutputTwo.cpp`.
```
bin/keyGeneration
bin/makeTable
bin/makeInputTwo
bin/searchInputTwo
bin/makeQueryTwo
bin/extractOutputTwo
bin/checkResult
```
For three-input functions, you need to revise the LUT path in `searchInputThree.cpp` and `extractOutputThree.cpp`.
```
bin/keyGeneration
bin/makeTable
bin/makeInputThree
bin/searchInputThree
bin/makeQueryThree
bin/extractOutputThree
bin/checkResult
```
## Comparison with Polynomial Approximation
To check the average abs/per error and runtime of polynomial approximation method, run one of the code you need.
```
bin/ckks_2(4/6/8)_relu(swish)
```
## Comparison with Naive Bit-Wise LUT
Check the directory `naivebitwiseLUT`.

### Construction of codes
```
src --- demo.hpp
      | keyGeneration.cpp
      | makeTable.cpp
      | checkResult.cpp(Step6)

      (For single-input:)
      | makeInput.cpp(Step1)
      | searchInput.cpp(Step2)
      | makeQuery.cpp(Step3)
      | extractOutput.cpp(Step4-5)

      (For two-input:)
      | makeInputTwo.cpp
      | searchInputTwo.cpp
      | makeQueryTwo.cpp
      | extractOutputTwo.cpp

      (For three-input:)
      | makeInputThree.cpp
      | searchInputThree.cpp
      | makeQueryThree.cpp
      | extractOutputThree.cpp

      (For polynomial approximation:)
      | ckks_2_relu.cpp
      | ckks_4_relu.cpp
      | ckks_6_relu.cpp
      | ckks_8_relu.cpp
      | ckks_2_swish.cpp
      | ckks_4_swish.cpp
      | ckks_6_swish.cpp
      | ckks_8_swish.cpp

      (For bit-wise FHE)
      | naivebitwiseLUT --- bitwiselut.cpp
                         |_ CMakeList.txt

      - randomInput --- test_normal.txt (test dataset)
                     |_ train_normal.txt (training dataset)
      - Table (store LUTs)
      - Result (store results)
      - compare_lut --- comError.py (compute the accuracy of LUT)
                     |_ genPoly.py (generate polynomial approximation)
                     |_ lut_ptxt_eds.py (generate plaintext LUT by eds)
                     |_ lut_ptxt_idas.py (generate plaintext LUT by idas)
```
