import pymysql
import RPi.GPIO as GPIO
import time
# Connect to the database
connection = pymysql.connect(
    host='localhost',
    user='admin',
    password='admin',
    db='schulsani'
)
PIN_A = 22
GPIO.setmode(GPIO.BOARD)
GPIO.setup(PIN_A, GPIO.OUT)
print("Programm wird mit STRG+C beendet")
done = False
state = False

# Create a cursor object
cursor = connection.cursor()




while True:
    connection = pymysql.connect(
        host='localhost',
        user='admin',
        password='admin',
        db='schulsani'
    )
    cursor = connection.cursor()
    cursor.execute("select alarm from alarm ORDER BY id DESC LIMIT 1;")
    result = cursor.fetchone()

    if result == ('on',):
        
        cursor.execute("select alarm from alarm ORDER BY id DESC LIMIT 1;")
        result = cursor.fetchone()
        GPIO.output(PIN_A, state)
        if state:
            state = False
        else:
            state = True
        time.sleep(0.1)
        print(result)
        print("alarm is on")
        cursor.close()
        connection.close()
    if result == ('off',):
        GPIO.output(PIN_A, done)
        cursor.execute("select alarm from alarm ORDER BY id DESC LIMIT 1;")
        result = cursor.fetchone()
        print(result)
        print("alarm is off")
        cursor.close()
        connection.close()
        
    time.sleep(1)
    
# Close the cursor and connection
cursor.close()
connection.close()

