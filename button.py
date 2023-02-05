from gpiozero import Button
import time
button = Button(18)
import mysql.connector
while True:
    if button.is_pressed:
        connection = mysql.connector.connect(host='localhost',
                                         database='schulsani',
                                         user='admin',
                                         password='admin')

        mySql_insert_query = """INSERT INTO alarm (alarm) 
                           VALUES 
                           ('off') """

        cursor = connection.cursor()
        cursor.execute(mySql_insert_query)
        connection.commit()
        print( "Button pressend")
    
        cursor.close()
        cnx = mysql.connector.connect(user='admin', password='admin', host='localhost', database='schulsani')
        cursor = cnx.cursor()

# SQL-Befehl vorbereiten
        query = "INSERT INTO status (status) VALUES (%s)"
        value = ('Wartet auf Bestätigung ... <br> ... Alarm wurde bestätigt',)

# SQL-Befehl ausführen
        cursor.execute(query, value)

# Änderungen in der Datenbank speichern
        cnx.commit()

# Verbindung zur Datenbank schließen
        cursor.close()
        cnx.close()

        time.sleep(2)
    else:
             print("Button is not pressed")
             time.sleep(2)