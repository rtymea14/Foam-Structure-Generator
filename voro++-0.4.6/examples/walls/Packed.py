import numpy as np
import matplotlib.pyplot as plt
import math
import random
import csv
#time = range(1,1365,50)
#time = range(1,1365,200)
time = range(1200,1201)
for t in range(0,len(time)):
    f = 'Clipped.z.' + str(time[t]) + ".csv"
    x = np.genfromtxt(f, skip_header=1, skip_footer=0, delimiter = ',', unpack=True, usecols=13)
    y = np.genfromtxt(f, skip_header=1, skip_footer=0, delimiter = ',', unpack=True, usecols=14)
    #z = np.genfromtxt(f, skip_header=1, skip_footer=0, delimiter = ',', unpack=True, usecols=15)
    idd = range(1,len(x))
    z = 0.5*np.ones(len(x))
    f3 = 'Coord' + str(time[t]) + ".csv"
    with open(f3, 'w') as fr:
        writer = csv.writer(fr, delimiter=' ')
        writer.writerows(zip(idd,x,y,z))
    with open('Cut1200.csv', 'w') as fd:
        writer = csv.writer(fd, delimiter=' ')
        writer.writerows(zip(x,y))

quit()
