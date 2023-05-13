import numpy as np
import matplotlib.pyplot as plt
import math
import random
from time import process_time

def swish(x):
    z = np.exp(-x)
    swi = 1 / (1 + z)
    return (swi*x)

def relu(x):
    if x>0:
        return x
    else:
        return 0
# LUT accuracy ctxt
def errorCom(input):
    inFile = open("randomInput/test_normal.txt",'r') # normal_distribution
    inputTest = inFile.readlines()
    inFile.close()
    testData = [float(i.strip("\n")) for i in inputTest]
    input_int = [int(i*pow(10,4)) for i in input] # swish 10^4, relu 10^4, convert the decimals to integers
    output = []
    for i in input:
        temp_out = relu(i)
        # temp_out = swish(i)
        temp_out = int(temp_out * pow(10,4)) # convert the decimals to integers
        output.append(float(temp_out/pow(10,4)))
    all_acc=0.0
    err = 0.0
    for i in testData:
        iter=0
        real_output = relu(i)
        # real_output = swish(i)
        for k in input_int:
            tab_intermediate = int(i*pow(10,4)) - k
            iter += 1
            if tab_intermediate <= 0:
                break
        if real_output!=0:
            acc = abs((real_output-output[iter-1])/real_output)
        else:
            acc=0.0
        all_acc += acc
        err += abs(real_output-output[iter-1])
    print("Percentage Error:")
    print(all_acc*100.0/len(testData))
    print("Absolute Avg Error:")
    print(err/len(testData))

# LUT accuracy ptxt
# def errorCom(input, output):
#     inFile = open("randomInput/test_normal.txt",'r') # normal_distribution
#     inputTest = inFile.readlines()
#     inFile.close()
#     testData = [float(i.strip("\n")) for i in inputTest]
#     print('number of test input:',len(testData))
#
#     all_acc=0.0
#     err = 0.0
#     print("Testing")
#     t_start = process_time()
#     for i in testData:
#         iter=0
#         # real_output = swish(i)
#         real_output = relu(i)
#         for k in input:
#             tab_intermediate = i - k
#             iter += 1
#             if tab_intermediate <= 0:
#                 break
#         if abs(i-input[iter-1])<i-input[iter-2]:
#             table_out=output[iter-1]
#         else:
#             table_out=output[iter-2]
#         if real_output!=0:
#             acc = abs((real_output-table_out)/real_output)*100
#         else:
#             acc=0.0
#         all_acc += acc
#         err += abs(real_output-table_out)
#     print("Percentage Error:")
#     print(all_acc/len(testData))
#     print("All Absolute Error:", err)
#     print("Absolute Avg Error:")
#     print(err/len(testData))
#     t_stop = process_time()
#     print("Elapsed time:", t_stop, t_start)
#     print("Elapsed time during the whole program in seconds:",
#                                          t_stop-t_start)

# read the input LUT
inFile = open("Table/plaintext_table/relu_input_12_freq.txt",'r')
tab_in = inFile.readlines()
inFile.close()
# read the output LUT
# inFile2 = open("Table/plaintext_table/relu_output_6.txt",'r')
# tab_out = inFile2.readlines()
# inFile2.close()

tab_input = [float(i.strip("\n")) for i in tab_in]
# tab_output = [float(i.strip("\n")) for i in tab_out]
print('The len of input table: ', len(tab_input))
# print('The len of output table: ', len(tab_output))

errorCom(tab_input)
# errorCom(tab_input,tab_out)
