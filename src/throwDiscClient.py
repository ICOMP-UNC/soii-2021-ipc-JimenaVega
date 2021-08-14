import os
import time as tim
import random

port = input("Server port: ")
serverIP =input("Server IP: ")
cli  = int(input("Amount of clients: "))
clientIP = ".".join(map(str, (random.randint(0, 255) for _ in range(4))))

os.system("../bin/dummy {0} {1} {2} &".format(clientIP, serverIP, port))
os.system("killall -9 ../bin/dummy")

tim.sleep(2)

os.system("../bin/dummy {0} {1} {2} &".format(clientIP, serverIP, port))