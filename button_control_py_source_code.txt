from flask import Flask, render_template, redirect, url_for, request
import pymysql
import serial
import time
from datetime import datetime

app = Flask(__name__)

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(5)

DB_CONFIG = {
    'host': 'localhost',
    'user': 'xinzhe',
    'password': '',
    'database': 'individual_assignment_db'
}

def get_arduino_status():
    ser.reset_input_buffer()
    time.sleep(0.5)
    start_time = time.time()
    while time.time() - start_time < 1.0:
        line = ser.readline().decode(errors='ignore').strip()
        if line.startswith("DATA:"):
            try:
                parts = line[5:].split(",")
                temperature = float(parts[0])
                mode = parts[1]
                fan = parts[2]
                return {"temperature": temperature, "mode": mode, "fan": fan}
            except:
                continue
    return {"temperature": "--", "mode": "--", "fan": "--"}

def get_fan_changes():
    conn = pymysql.connect(**DB_CONFIG)
    cursor = conn.cursor()
    cursor.execute("""
        SELECT timestamp, temperature, fan_state
        FROM temperature_log
        WHERE id IN (
            SELECT id FROM (
                SELECT id, fan_state,
                    LAG(fan_state) OVER (ORDER BY timestamp) AS prev_state
                FROM temperature_log
            ) AS t
            WHERE fan_state != prev_state OR prev_state IS NULL
        )
        ORDER BY timestamp DESC
        LIMIT 10
    """)
    rows = cursor.fetchall()
    conn.close()
    return rows

@app.route('/')
def index():
    status = get_arduino_status()
    fan_data = get_fan_changes()
    return render_template('index.html', fan_data=fan_data, **status)

@app.route('/delete_all', methods=['POST'])
def delete_all():
    conn = pymysql.connect(**DB_CONFIG)
    cursor = conn.cursor()
    cursor.execute("DELETE FROM temperature_log")
    conn.commit()
    conn.close()
    return redirect(url_for('index'))

@app.route('/toggle_fan')
def toggle_fan():
    ser.write(b'FAN_TOGGLE\n')
    time.sleep(3)
    return redirect(url_for('index'))

@app.route('/toggle_mode')
def toggle_mode():
    ser.write(b'MODE_TOGGLE\n')
    time.sleep(3)
    return redirect(url_for('index'))

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000, debug=True)
