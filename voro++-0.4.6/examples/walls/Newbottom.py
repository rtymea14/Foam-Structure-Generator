import numpy as np
import matplotlib.pyplot as plt
import math
import random
import csv
import warnings
#time = range(1,1365,50)
time = range(1,1365,200)
for t in range(0,len(time)):
    f = 'Clipped.z.' + str(time[t]) + ".csv"
    x = np.genfromtxt(f, skip_header=1, skip_footer=0, delimiter = ',', unpack=True, usecols=13)
    y = np.genfromtxt(f, skip_header=1, skip_footer=0, delimiter = ',', unpack=True, usecols=14)
    z = np.genfromtxt(f, skip_header=1, skip_footer=0, delimiter = ',', unpack=True, usecols=15)
    rr = []
    for i in range(0,len(x)):
        xij = x[i] - 150 #200
    	yij = y[i] - 150 #200
        rr.append(math.sqrt(xij*xij+yij*yij))
    cutoff = np.max(rr) #L/2.0
    dr = cutoff/10
    nB = int(cutoff/dr) + 1
    H = np.zeros(nB)
    for i in range(0,len(x)):
        #if rr <= cutoff:
    	binn = int(rr[i]/dr)
    	H[binn] = H[binn] + 1	
    	#if binn > 99:
    	#   binn = 99
    RR = []
    for i in range (0,nB):
    	RR.append(i*dr)
    N = 0.0
    gr = []
    density = []
    Rc = []
    #density = np.zeros(len(R))
    gr.append(0.0)
    density.append(0.0)
    Rc.append(0.0)
    dens = np.sum(H)/(math.pi*np.max(RR)*np.max(RR))
    for i in range(1,len(RR)):
        N = H[i] 
    	gr.append(N/(dens*2*math.pi*RR[i]*(RR[i]-RR[i-1])))
    	density.append(N/(2*math.pi*RR[i]*(RR[i]-RR[i-1])))
    	Rc.append(RR[i])
        #density[i] = H[i]/(2*(R[i]*R[i]-R[i-1]*R[i-1]))
        #density[i] = H[i]/((2./3.)*3.14159*(R[i]*R[i]*R[i]-R[i-1]*R[i-1]*R[i-1]))
        #density[i] = N
    f3 = 'Dense' + str(time[t]) + ".csv"
    with open(f3, 'w') as fr:
        writer = csv.writer(fr, delimiter=' ')
        writer.writerows(zip(Rc,gr,density))
    #z = np.polyfit(Rc, density, 11)
    #p = np.poly1d(z)
    #xp = np.linspace(0, np.max(RR), 200)
    plt.figure(1)
    #plt.xlim((0,35))
    #plt.ylim((0,1.01))
    plt.rcParams["font.family"] = "serif"
    lable = 'Time = ' + str(time[t]-1) +"ms"
    plt.plot(Rc,gr, 'b-', markersize = 1,label = lable)
    #plt.plot(Rc,density, 'b-', markersize = 1,label = '')
    #plt.plot(xp, p(xp), 'g-')
    plt.ylabel('Particle density per micrometer-square', fontsize = 12)
    plt.xlabel('Distance from center, micrometer', fontsize = 12)
    plt.legend()
    plt.tight_layout()
    file4 = 'Dense' + str(time[t]) +".png"
    #file4 = "Density" +".png"
    #plt.savefig(file4)
    plt.show()



quit()
