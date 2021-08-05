import numpy as np
#import matplotlib.pyplot as plt
import math
import random
#import sys
import os

#def my_print(text):
#    sys.stdout.write(str(text))
#    sys.stdout.flush()


file1 = "pack_ten_cube" + ".gnu"
x = []
y = []
z = []

with open(file1) as f:
    for line in f:
        data = line.strip().split()
        if len(data) != 0:
            x.append(float(data[0]))
            y.append(float(data[1]))
            z.append(float(data[2]))
f.close
Size=len(x)
frontfile = open('xfront.dat', "w")
for i in range(0,Size):
    if x[i] < (-5+0.0001):
        if x[i].is_integer():
            fsx = str(int(x[i]))
        else:
            fsx = str(x[i])
        if y[i].is_integer():
            fsy = str(int(y[i]))
        else:
            fsy = str(y[i])
        if z[i].is_integer():
            fsz = str(int(z[i]))
        else:
            fsz = str(z[i])
        fs =  fsx + "," + fsy + "," + fsz
        if i != (Size-1):
            fs = fs + "\n"
        frontfile.write(fs)
        #print fs

frontfile.close()

file1 = "xfront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('xfrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "xfrontnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("pack_ten_cube_v.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)

outfile = open('xnofront.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                for j in range(i+1,size1):
                    if contain[j] in Lines[l]:
                        outfile.write(Lines[l]+"\n")
                        #print Lines[l]

outfile.close()

#os.system('sed \'$d\' xnofront.dat')

file1 = "xnofront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('xnofrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()
#os.system('grep -xvf xnofrontnodup.dat pack_ten_cube_v.dat > noxfront.pov')


file1 = "pack_ten_cube" + ".gnu"
x = []
y = []
z = []

with open(file1) as f:
    for line in f:
        data = line.strip().split()
        if len(data) != 0:
            x.append(float(data[0]))
            y.append(float(data[1]))
            z.append(float(data[2]))
f.close
Size=len(x)
frontfile = open('xbot.dat', "w")
for i in range(0,Size):
    if x[i] > (5-0.0001):
        if x[i].is_integer():
            fsx = str(int(x[i]))
        else:
            fsx = str(x[i])
        if y[i].is_integer():
            fsy = str(int(y[i]))
        else:
            fsy = str(y[i])
        if z[i].is_integer():
            fsz = str(int(z[i]))
        else:
            fsz = str(z[i])
        fs =  fsx + "," + fsy + "," + fsz
        if i != (Size-1):
            fs = fs + "\n"
        frontfile.write(fs)
        #print fs

frontfile.close()

file1 = "xbot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('xbotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "xbotnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("pack_ten_cube_v.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)

outfile = open('xnobot.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                for j in range(i+1,size1):
                    if contain[j] in Lines[l]:
                        outfile.write(Lines[l]+"\n")
                        #print Lines[l]

outfile.close()

#os.system('sed \'$d\' xnofront.dat')

file1 = "xnobot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('xnobotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()
#os.system('grep -xvf xnofrontnodup.dat pack_ten_cube_v.dat > noxfront.dat')
#os.system('grep -xvf xnobotnodup.dat noxfront.dat > nox.pov')

file1 = "pack_ten_cube" + ".gnu"
x = []
y = []
z = []

with open(file1) as f:
    for line in f:
        data = line.strip().split()
        if len(data) != 0:
            x.append(float(data[0]))
            y.append(float(data[1]))
            z.append(float(data[2]))
f.close
Size=len(x)
frontfile = open('yfront.dat', "w")
for i in range(0,Size):
    if y[i] < (-5+0.0001):
        if x[i].is_integer():
            fsx = str(int(x[i]))
        else:
            fsx = str(x[i])
        if y[i].is_integer():
            fsy = str(int(y[i]))
        else:
            fsy = str(y[i])
        if z[i].is_integer():
            fsz = str(int(z[i]))
        else:
            fsz = str(z[i])
        fs =  fsx + "," + fsy + "," + fsz
        if i != (Size-1):
            fs = fs + "\n"
        frontfile.write(fs)
        #print fs

frontfile.close()

file1 = "yfront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('yfrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "yfrontnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("pack_ten_cube_v.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)

outfile = open('ynofront.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                for j in range(i+1,size1):
                    if contain[j] in Lines[l]:
                        outfile.write(Lines[l]+"\n")
                        #print Lines[l]

outfile.close()

#os.system('sed \'$d\' ynofront.dat')

file1 = "ynofront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('ynofrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()
#os.system('grep -xvf xnofrontnodup.dat pack_ten_cube_v.dat > noxfront.pov')


file1 = "pack_ten_cube" + ".gnu"
x = []
y = []
z = []

with open(file1) as f:
    for line in f:
        data = line.strip().split()
        if len(data) != 0:
            x.append(float(data[0]))
            y.append(float(data[1]))
            z.append(float(data[2]))
f.close
Size=len(x)
frontfile = open('ybot.dat', "w")
for i in range(0,Size):
    if y[i] > (5-0.0001):
        if x[i].is_integer():
            fsx = str(int(x[i]))
        else:
            fsx = str(x[i])
        if y[i].is_integer():
            fsy = str(int(y[i]))
        else:
            fsy = str(y[i])
        if z[i].is_integer():
            fsz = str(int(z[i]))
        else:
            fsz = str(z[i])
        fs =  fsx + "," + fsy + "," + fsz
        if i != (Size-1):
            fs = fs + "\n"
        frontfile.write(fs)
        #print fs

frontfile.close()

file1 = "ybot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('ybotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "ybotnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("pack_ten_cube_v.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)

outfile = open('ynobot.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                for j in range(i+1,size1):
                    if contain[j] in Lines[l]:
                        outfile.write(Lines[l]+"\n")
                        #print Lines[l]

outfile.close()

#os.system('sed \'$d\' ynofront.dat')

file1 = "ynobot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('ynobotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

#os.system('grep -xvf xnofrontnodup.dat pack_ten_cube_v.dat > noxfront.dat')
#os.system('grep -xvf xnobotnodup.dat noxfront.dat > nox.dat')
#os.system('grep -xvf ynofrontnodup.dat nox.dat > noxyfront.dat')
#os.system('grep -xvf ynobotnodup.dat noxyfront.dat > noxy.pov')


file1 = "pack_ten_cube" + ".gnu"
x = []
y = []
z = []

with open(file1) as f:
    for line in f:
        data = line.strip().split()
        if len(data) != 0:
            x.append(float(data[0]))
            y.append(float(data[1]))
            z.append(float(data[2]))
f.close
Size=len(x)
frontfile = open('zfront.dat', "w")
for i in range(0,Size):
    if z[i] < (0.0001):
        if x[i].is_integer():
            fsx = str(int(x[i]))
        else:
            fsx = str(x[i])
        if y[i].is_integer():
            fsy = str(int(y[i]))
        else:
            fsy = str(y[i])
        if z[i].is_integer():
            fsz = str(int(z[i]))
        else:
            fsz = str(z[i])
        fs =  fsx + "," + fsy + "," + fsz
        if i != (Size-1):
            fs = fs + "\n"
        frontfile.write(fs)
        #print fs

frontfile.close()

file1 = "zfront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('zfrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "zfrontnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("pack_ten_cube_v.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)

outfile = open('znofront.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                for j in range(i+1,size1):
                    if contain[j] in Lines[l]:
                        outfile.write(Lines[l]+"\n")
                        #print Lines[l]

outfile.close()

#os.system('sed \'$d\' znofront.dat')

file1 = "znofront" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('znofrontnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()
#os.system('grep -xvf xnofrontnodup.dat pack_ten_cube_v.dat > noxfront.pov')


file1 = "pack_ten_cube" + ".gnu"
x = []
y = []
z = []

with open(file1) as f:
    for line in f:
        data = line.strip().split()
        if len(data) != 0:
            x.append(float(data[0]))
            y.append(float(data[1]))
            z.append(float(data[2]))
f.close
Size=len(x)
frontfile = open('zbot.dat', "w")
for i in range(0,Size):
    if z[i] > (10-0.0001):
        if x[i].is_integer():
            fsx = str(int(x[i]))
        else:
            fsx = str(x[i])
        if y[i].is_integer():
            fsy = str(int(y[i]))
        else:
            fsy = str(y[i])
        if z[i].is_integer():
            fsz = str(int(z[i]))
        else:
            fsz = str(z[i])
        fs =  fsx + "," + fsy + "," + fsz
        if i != (Size-1):
            fs = fs + "\n"
        frontfile.write(fs)
        #print fs

frontfile.close()

file1 = "zbot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('zbotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

file1 = "zbotnodup" + ".dat"

contain = []
f = open(file1,'r')
for line in f:
    contain.append(str(line.strip()))

f.close

Lines = []        
with open ("pack_ten_cube_v.pov", "r") as myfile:
        for line in myfile:
                 Lines.append(str(line.strip())) 
myfile.close()
size1 = len(contain)
size2 = len(Lines)

outfile = open('znobot.dat', "w")
for i in range(0,size1):
    for l in range(0,size2):
        if contain[i] in Lines[l]:
            if "cylinder" in Lines[l]:
                for j in range(i+1,size1):
                    if contain[j] in Lines[l]:
                        outfile.write(Lines[l]+"\n")
                        #print Lines[l]

outfile.close()

#os.system('sed \'$d\' znofront.dat')

file1 = "znobot" + ".dat"
lines_seen = set() # holds lines already seen
outfile = open('znobotnodup.dat', "w")
for line in open(file1, "r"):
        if line not in lines_seen: # not a duplicate
                    outfile.write(line)
                    lines_seen.add(line)
outfile.close()

os.system('grep -xvf xnofrontnodup.dat pack_ten_cube_v.pov > noxfront.dat')
os.system('grep -xvf xnobotnodup.dat noxfront.dat > nox.dat')
os.system('grep -xvf ynofrontnodup.dat nox.dat > noxyfront.dat')
os.system('grep -xvf ynobotnodup.dat noxyfront.dat > noxy.dat')
os.system('grep -xvf znofrontnodup.dat noxy.dat > noxyzfront.dat')
os.system('grep -xvf znobotnodup.dat noxyzfront.dat > noxyz.pov')

quit()
