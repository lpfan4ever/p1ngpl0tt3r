import matplotlib.pyplot as plt
import numpy as np
import os

i=0
liste=[]
fileDir = os.path.dirname(os.path.realpath('__file__'))
filename = os.path.join(fileDir,'time.txt')
with open(filename) as fp:
    for line in fp:
        ping,time = line.split(" ",1)
        Y1 = float(ping)/1000
        i=i+1
        if Y1 >= 100:
            plt.bar(i, +Y1, facecolor='#ff0000', edgecolor='black')
        else:
            plt.bar(i, +Y1, facecolor='#00ff00', edgecolor='black')
        liste.append(time)

fp.close()
plt.ylabel('Ping in ms')
plt.xlabel('Time in sec')
plt.xticks(range(1,i+1),liste, rotation=45)
figManager = plt.get_current_fig_manager()
figManager.resize(*figManager.window.maxsize())
plt.show()
