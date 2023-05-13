# Input data-aware selection
from sklearn.cluster import KMeans
import numpy as np
import matplotlib.pyplot as plt
from decimal import Decimal

def swish(x):
    z = np.exp(-x)
    swi = 1 / (1 + z)
    return (swi*x)

def relu(x):
    if x>0:
        return x
    else:
        return 0

inFile = open("randomInput/train_normal.txt",'r')
inputData = inFile.readlines()
inFile.close()

train = [float(i.strip("\n")) for i in inputData]
print('Training dataset points:',len(train))

train = [[i] for i in train]
kmeans = KMeans(n_clusters=16384, random_state=0).fit(train)

center_clu = []
for i in kmeans.cluster_centers_:
    center_clu.append(i[0])
sorted_clu_center = sorted(center_clu)
# print(sorted_clu_center)

wi = open("Table/plaintext_table/relu_input_14_freq.txt","w")
wo = open("Table/plaintext_table/relu_output_14_freq.txt","w")
print('table size:',len(sorted_clu_center))
for i in sorted_clu_center:
    wi.write(str(i)+'\n')
    # temp_out = swish(i)
    temp_out = relu(i)
    wo.write(str(temp_out)+'\n')
wi.close()
wo.close()
