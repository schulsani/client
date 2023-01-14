import time
import requests
import subprocess
while True:
    alarm = requests.get('http://schulsaniserver.local/alarm.txt')
    f = open("alarm.txt", "w")
    f.close()
    print(2)
    time.sleep(1.5)

    if alarm.text == "on":
     subprocess.call("alarm.py", shell=True)
     time.sleep(120)