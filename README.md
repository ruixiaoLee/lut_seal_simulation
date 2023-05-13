# lut_seal_simulation
Revise the `CMakeList.txt` to compile the code you need.
### Construction of codes
```
src --- demo.hpp
      | keyGeneration.cpp
      | makeTable.cpp
      | checkResult.cpp

      (For single-input:)
      | makeInput.cpp
      | searchInput.cpp
      | makeQuery.cpp
      | extractOutput.cpp

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

      - randomInput --- test_normal.txt (test dataset)
                     |_ train_normal.txt (training dataset)
      - Table (store LUTs)
      - Result (store results)
      - compare_lut --- comError.py (compute the accuracy of LUT)
                     |_ genPoly.py (generate polynomial approximation)
                     |_ lut_ptxt_eds.py (generate plaintext LUT by eds)
                     |_ lut_ptxt_idas.py (generate plaintext LUT by idas)
```
## Single-Input Function Evaluation with LUT

## Multi-Input Function Evaluation with LUT

## Comparison with Polynomial Approximation
