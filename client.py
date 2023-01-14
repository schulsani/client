import requests
import time
while True:
    raum = requests.get('http://schulsaniserver.local/raum.txt')



    unfall = requests.get('http://schulsaniserver.local/unfall.txt')

    alarm = requests.get('http://schulsaniserver.local/alarm.txt')

    f = open("raum.txt", "w")
    f.write(raum.text)
    f.close()

    f = open("unfall.txt", "w")
    f.write(unfall.text)
    f.close()
    
    f = open("alarm.txt", "w")
    f.write(alarm.text)
    f.close()
    time.sleep(1.5)