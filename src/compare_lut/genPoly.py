# this code is for create the polynomial approximation
import numpy as np
import matplotlib.pyplot as plt

def relu(x):
    return(np.maximum(0.0, x))

def sigmoid(x):
  return 1.0 / (1.0 + np.exp(-x))

def swish(x):
  return x * sigmoid(x)

x_value = np.arange(-3.0, 3.0, 0.01) # generate the random numbers within the range
y_value = swish(x_value)
print(x_value)
print(y_value)

z = np.polyfit(x_value, y_value, 10)
p = np.poly1d(z)
print(p)

cur = p(x_value)
plt.plot(x_value, y_value, '*', label = 'Original value')
plt.plot(x_value, cur, 'r', label='Poly fit')
plt.xlabel('x axis')
plt.ylabel('y axis')
plt.show()
