import matplotlib.pyplot as plt
import numpy as np

i=0
with open('/home/dominik/Documents/pingplotter/time.txt') as fp:
    for line in fp:
        Y1 = float(line)/1000
        i=i+1
        if Y1 >= 100:
            plt.bar(i, +Y1, facecolor='#ff0000', edgecolor='white')
        else:
            plt.bar(i, +Y1, facecolor='#00ff00', edgecolor='white')
fp.close()
plt.ylabel('Ping in ms')
plt.xlabel('Time in sec')
plt.show()
