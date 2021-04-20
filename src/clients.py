import os
import random
#import numpy as np

port = input("Server port: ")
#serverIP = input("ServerIP")
cli  = int(input("Amount of clients: "))

#out = os.system('cat /proc/sys/kernel/hostname')

serverIP = "192.168.100.7"

for i in range (cli):
    clientIP = ".".join(map(str, (random.randint(0, 255) for _ in range(4))))
    print(clientIP)
    os.system("../bin/cli {0} {1} {2}&".format(clientIP, serverIP, port))




