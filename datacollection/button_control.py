from flask import Flask, render_template, redirect, url_for
import serial
import time

app = Flask(__name__)
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(3)
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

@app.route('/')
def index():
    status = get_arduino_status()
    return render_template('index.html', **status)

@app.route('/toggle_fan')
def toggle_fan():
    ser.write(b'FAN_TOGGLE\n')
    time.sleep(1.5)  
    return redirect(url_for('index'))

@app.route('/toggle_mode')
def toggle_mode():
    ser.write(b'MODE_TOGGLE\n')
    time.sleep(1.5)  
    return redirect(url_for('index'))

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000, debug=True)
