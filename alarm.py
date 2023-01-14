
def funktion_1():
    import requests
    from threading import Thread
    from playsound import playsound
    i = 0
    ifstop = 2
    print("Script wurde gestartet")

    def funktion_alarm():
        while True:
        
         playsound("alarm.mp3")
    def funktion_WAITFOR():
        ifstop = input("")
        def funktion_alarm():        
            print("")
    Thread1 = Thread(target=funktion_alarm)
    Thread2 = Thread(target=funktion_WAITFOR)
    Thread1.start()
    Thread2.start()
funktion_1()








