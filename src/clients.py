import os
import random


# port = input("Server port: ")
# serverIP = input("Server IP: ")
port = 2021
serverIP = "192.168.100.6"
cli  = int(input("Amount of clients: "))

print("---------------------------------------\n")
# "192.168.100.7"

for i in range (cli):
    clientIP = ".".join(map(str, (random.randint(0, 255) for _ in range(4))))
    print("client ip : {0}".format(clientIP))
    
    os.system("../bin/cli {0} {1} {2} &".format(clientIP, serverIP, port))




