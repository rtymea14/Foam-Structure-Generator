import numpy as np
#import matplotlib.pyplot as plt
import math
import random
#import sys
import os

#def my_print(text):
#    sys.stdout.write(str(text))
#    sys.stdout.flush()



file1 = "xfrontnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('spherexfront.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #sphere.append(contain[i])

file1 = "spherexfront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('spherexfrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "spherexfrontnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nospherexfront.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #nos.append(i)
outfile.close()

file1 = "nospherexfront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nospherexfrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()
os.system('grep -xvf nospherexfrontnodup.dat spherexfrontnodup.dat > nospherexf.dat')

file1 = "nospherexf" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nospherexf2.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(Lines[l]+"\n")
                #nos.append(i)
outfile.close()
file1 = "nospherexf2" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nospherexf3.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

#os.system('grep -xvf nospherexf3.dat noxyz.pov > nospherex_yz.pov')

file1 = "xbotnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('spherexbot.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #sphere.append(contain[i])

file1 = "spherexbot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('spherexbotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "spherexbotnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nospherexbot.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #nos.append(i)
outfile.close()

file1 = "nospherexbot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nospherexbotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()
os.system('grep -xvf nospherexbotnodup.dat spherexbotnodup.dat > nospherexb.dat')

file1 = "nospherexb" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nospherexb2.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(Lines[l]+"\n")
                #nos.append(i)
outfile.close()
file1 = "nospherexb2" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nospherexb3.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

#os.system('grep -xvf nospherexf3.dat noxyz.pov > nospherex_yz.dat')
#os.system('grep -xvf nospherexb3.dat nospherex_yz.dat > nosphereXyz.pov')

file1 = "yfrontnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('sphereyfront.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #sphere.append(contain[i])

file1 = "sphereyfront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('sphereyfrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "sphereyfrontnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nosphereyfront.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #nos.append(i)
outfile.close()

file1 = "nosphereyfront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nosphereyfrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()
os.system('grep -xvf nosphereyfrontnodup.dat sphereyfrontnodup.dat > nosphereyf.dat')

file1 = "nosphereyf" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nosphereyf2.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(Lines[l]+"\n")
                #nos.append(i)
outfile.close()
file1 = "nosphereyf2" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nosphereyf3.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

#os.system('grep -xvf nospherexf3.dat noxyz.pov > nospherex_yz.pov')

file1 = "ybotnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('sphereybot.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #sphere.append(contain[i])

file1 = "sphereybot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('sphereybotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "sphereybotnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nosphereybot.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #nos.append(i)
outfile.close()

file1 = "nosphereybot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nosphereybotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()
os.system('grep -xvf nosphereybotnodup.dat sphereybotnodup.dat > nosphereyb.dat')

file1 = "nosphereyb" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nosphereyb2.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(Lines[l]+"\n")
                #nos.append(i)
outfile.close()
file1 = "nosphereyb2" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nosphereyb3.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

#os.system('grep -xvf nospherexf3.dat noxyz.pov > nospherex_yz.dat')
#os.system('grep -xvf nospherexb3.dat nospherex_yz.dat > nosphereXyz.dat')
#os.system('grep -xvf nosphereyf3.dat nosphereXyz.dat > nospherexy_z.dat')
#os.system('grep -xvf nosphereyb3.dat nospherexy_z.dat > nosphereXYz.pov')

file1 = "zfrontnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('spherezfront.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #sphere.append(contain[i])

file1 = "spherezfront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('spherezfrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "spherezfrontnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nospherezfront.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #nos.append(i)
outfile.close()

file1 = "nospherezfront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nospherezfrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()
os.system('grep -xvf nospherezfrontnodup.dat spherezfrontnodup.dat > nospherezf.dat')

file1 = "nospherezf" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nospherezf2.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(Lines[l]+"\n")
                #nos.append(i)
outfile.close()
file1 = "nospherezf2" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nospherezf3.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

#os.system('grep -xvf nospherezf3.dat noxyz.pov > nospherez_yz.pov')

file1 = "zbotnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('spherezbot.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #sphere.append(contain[i])

file1 = "spherezbot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('spherezbotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "spherezbotnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nospherezbot.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                outfile.write(contain[i]+"\n")
                #nos.append(i)
outfile.close()

file1 = "nospherezbot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nospherezbotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()
os.system('grep -xvf nospherezbotnodup.dat spherezbotnodup.dat > nospherezb.dat')

file1 = "nospherezb" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("noxyz.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)
outfile = open('nospherezb2.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "sphere" in Lines[l]:
                outfile.write(Lines[l]+"\n")
                #nos.append(i)
outfile.close()
file1 = "nospherezb2" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('nospherezb3.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

os.system('grep -xvf nospherexf3.dat noxyz.pov > nospherex_yz.dat')
os.system('grep -xvf nospherexb3.dat nospherex_yz.dat > nosphereXyz.dat')
os.system('grep -xvf nosphereyf3.dat nosphereXyz.dat > nospherexy_z.dat')
os.system('grep -xvf nosphereyb3.dat nospherexy_z.dat > nosphereXYz.dat')
os.system('grep -xvf nospherezf3.dat nosphereXYz.dat > nospherexyz.dat')
os.system('grep -xvf nospherezb3.dat nospherexyz.dat > nosphereXYZ.pov')


quit()
