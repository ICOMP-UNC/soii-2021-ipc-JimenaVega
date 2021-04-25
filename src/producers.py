#This script allows to execute 3 producers at the same time
import os

PRODUCERS = 3

for i in range (PRODUCERS):
    os.system("../bin/p{0} &".format(i+1))