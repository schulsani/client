import mysql.connector
import time
import drivers
unfall_old = "g"
raum_old = "G"
lcd = drivers.Lcd()
while True:
    mydb = mysql.connector.connect(
    host="localhost",
    user="admin",
    password="admin",
    database="schulsani"
    )
    mycursor = mydb.cursor()

# Select the last row for the "unfall" column
    mycursor.execute("SELECT unfall FROM college ORDER BY ID DESC LIMIT 1;")

    myresult = mycursor.fetchone()
   
    unfall = myresult
    mydb = mysql.connector.connect(
    host="localhost",
    user="admin",
    password="admin",
    database="schulsani"
    )
    mycursor = mydb.cursor()

# Select the last row for the "unfall" column
    mycursor.execute("SELECT raum FROM college ORDER BY ID DESC LIMIT 1;")

    myresult = mycursor.fetchone()
   
    raum = myresult
    
    mycursor.close
    unfall_str = str(unfall)
    unfall_str = unfall_str.replace('(', '')
    unfall_str = unfall_str.replace(')', '')
    unfall_str = unfall_str.replace(',', '')
    unfall_str = unfall_str.replace("'", '')
    raum_str = str(raum)
    raum_str = raum_str.replace('(', '')
    raum_str = raum_str.replace(')', '')
    raum_str = raum_str.replace(',', '')
    raum_str = raum_str.replace("'", '')
    if unfall_str != unfall_old:
       print(1)
       unfall_old = unfall_str
       lcd.lcd_clear()
    if raum_str != raum_old:
       print(2)
       raum_old = raum_str
       lcd.lcd_clear()
    
    
    lcd.lcd_display_string(time.strftime("Schulsanialarm %H:%M"), 1)
    lcd.lcd_display_string("Raum:" + unfall_str, 2)
    lcd.lcd_display_string("Unfall:" + raum_str, 3)
    lcd.lcd_display_string("", 4)
                                   
