import matplotlib.pyplot as plt
import numpy as np
import os

i=0
fileDir = os.path.dirname(os.path.realpath('__file__'))
filename = os.path.join(fileDir,'time.txt')
with open(filename) as fp:
    for line in fp:
        Y1 = float(line)/1000
        i=i+1
        if Y1 >= 100:
            plt.bar(i, +Y1, facecolor='#ff0000', edgecolor='black')
        else:
            plt.bar(i, +Y1, facecolor='#00ff00', edgecolor='black')
fp.close()
plt.ylabel('Ping in ms')
plt.xlabel('Time in sec')
plt.show()
