import os

PRODUCERS = 3

for i in range (PRODUCERS):
    os.system("../bin/p{0} &".format(i+1))