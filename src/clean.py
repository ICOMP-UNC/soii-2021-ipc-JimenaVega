import os

#kill producers
for i in range(3):
    os.system("killall -9 ../bin/p{0}".format(i+1))

#kill clients
os.system("killall -9 ../bin/cli")

#kill CLI
os.system("killall -9 ../bin/CLI")

#kill server
os.system("killall -9 ../bin/serv")
