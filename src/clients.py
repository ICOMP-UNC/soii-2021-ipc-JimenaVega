import os
#import numpy as np

port = input("Server port: ")
cli  = int(input("Amount of clients: "))
out = os.system('cat /proc/sys/kernel/hostname')
print(out)

for i in range (cli):
    os.system("../bin/cli {0} {1}&".format(out,port))






