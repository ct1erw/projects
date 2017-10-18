import numpy as np
import matplotlib.pyplot as plt
import os

fileName = "./cap2.bin"

offset = 0xb8
f = open(fileName, mode='rb')
f.seek(offset, os.SEEK_SET)

data = np.fromfile(f, dtype= 'H')
time = np.linspace(-1,1, num = len(data))

print("Data: ", len(data))

plt.plot(time,data)
plt.show()
