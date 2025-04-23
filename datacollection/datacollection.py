import serial
import mysql.connector
import time
from datetime import datetime

# setting up serial connection
ser = serial.Serial('/dev/cu.usbmodem142101', 9600, timeout=1)
time.sleep(3)  # give Arduino time to prepare

# database connection
db = mysql.connector.connect(
    host="feenix-mariadb.swin.edu.au",
    user="s105385294",
    password="311292",
    database="s105385294_db"
)
cursor = db.cursor()

# create table if not exists
cursor.execute("""
CREATE TABLE IF NOT EXISTS temperature_log (
    id INT AUTO_INCREMENT PRIMARY KEY,
    temperature FLOAT,
    system_state VARCHAR(10),
    fan_state VARCHAR(10),
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)
""")

# initialize log file
log_file = "over26_log.txt"

try:
    while True:
        line = ser.readline().decode(errors='ignore').strip()
        if not line:
            continue

        if line.startswith("DATA:"):
            try:
                data = line[5:].split(",")
                temperature = float(data[0])
                mode = data[1]
                fan_state = data[2]

                # insert into table 
                cursor.execute("INSERT INTO temperature_log (temperature, system_state, fan_state) VALUES (%s, %s, %s)",
                               (temperature, mode, fan_state))
                db.commit()
                print(f"✔️ Saved to DB: {temperature:.1f}°C | {mode} | Fan {fan_state}")

                # write into log file if temperature > 26.0
                if temperature > 26.0:
                    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                    with open(log_file, "a") as file:
                        file.write(f"{timestamp} - Temp: {temperature:.1f}°C\n")
                    print(f"write into TXT(>26°C)：{temperature:.1f}°C")

            except Exception as e:
                print("Data extracting error:", line, "→", e)

except KeyboardInterrupt:
    print("\nprepare to exit...")

finally:
    ser.close()
    cursor.close()
    db.close()
    print("Exiting successfully")