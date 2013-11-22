import sys
import numpy as np
import matplotlib.pyplot as plt

fname = sys.argv[1]

data = np.genfromtxt(fname, delimiter=',', names=['time', 'b', 'g', 'r'])

plt.plot(data['time'], data['b'], 'b,')
plt.plot(data['time'], data['g'], 'g,')
plt.plot(data['time'], data['r'], 'r,')
plt.show()
