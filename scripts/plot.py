import sys
import numpy as np
import matplotlib.pyplot as plt

fname = sys.argv[1]

data = np.genfromtxt(fname, delimiter=',', names=['h', 'l', 's', 'time'])

plt.plot(data['time'], data['h'], 'b,')
plt.plot(data['time'], data['l'], 'g,')
plt.plot(data['time'], data['s'], 'r,')

plt.show()
