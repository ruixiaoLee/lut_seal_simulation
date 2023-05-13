# Equidistant data selection
import numpy as np
import matplotlib.pyplot as plt
import math

def sigmoid(x):
    z = np.exp(-x)
    sig = 1 / (1 + z)
    return sig

def swish(x):
    z = np.exp(-x)
    swi = 1 / (1 + z)
    return (swi*x)

def relu(x):
    if x>0:
        return x
    else:
        return 0

# X17 = np.linspace(-6.5536, 6.5536, 131072, endpoint=True)
# X16 = np.linspace(-6.5536, 6.5536, 65536, endpoint=True)
X14 = np.linspace(-6.5536, 6.5536, 16384, endpoint=True)
X12 = np.linspace(-6.5536, 6.5536, 4096, endpoint=True)
# X11 = np.linspace(-6, 6, 2048, endpoint=True)
X10 = np.linspace(-6.5536, 6.5536, 1024, endpoint=True)
X8 = np.linspace(-6.5536, 6.5536, 256, endpoint=True)
X6 = np.linspace(-6.5536, 6.5536, 64, endpoint=True)

fi = open("Table/plaintext_table/relu_input_6.txt","w")
fo = open("Table/plaintext_table/relu_output_6.txt","w")
for i in X6:
    fi.write(str(i)+'\n')
    fo.write(str(relu(i))+'\n')
fi.close()
fo.close()

# fi = open("Table/plaintext_table/swish_input_6.txt","w")
# fo = open("Table/plaintext_table/swish_output_6.txt","w")
# for i in X6:
#     fi.write(str(i)+'\n')
#     fo.write(str(swish(i))+'\n')
# fi.close()
# fo.close()

print("Done.")
