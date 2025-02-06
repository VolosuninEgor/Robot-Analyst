from flask import Flask, render_template, request, Response, jsonify
from picamera2 import Picamera2
import cv2
import serial
import threading
import time

# Настраиваем Flask
app = Flask(__name__)

# Настраиваем последовательный порт для общения с Arduino
arduino_sensor = serial.Serial('/dev/ttyUSB0', 9600)
arduino_platform = serial.Serial('/dev/ttyUSB1', 9600)

# Глобальные данные для сенсоров
data = {
    "distance1": 0,
    "distance2": 0,
    "distance3": 0,
    "distance4": 0,
    "humidity": 0,
    "temperature": 0,
    "light": "0",
    "motion": "Попка в безопасности"
}

# Поток для чтения данных с Arduino
def read_from_arduino():
    global data
    while True:
        if arduino_sensor.in_waiting > 0:
            try:
                line = arduino_sensor.readline().decode('utf-8').strip()
                parsed_data = line.split(',')
                if len(parsed_data) == 8:
                    distance1, distance2, distance3, distance4, humidity, temperature, light, motion = parsed_data
                    data["distance1"] = float(distance1)
                    data["distance2"] = float(distance2)
                    data["distance3"] = float(distance3)
                    data["distance4"] = float(distance4)
                    data["humidity"] = float(humidity)
                    data["temperature"] = float(temperature)
                    data["light"] = str(float(light)) if float(light) > 5 else "Темно, хоть глаз выколи!"
                    data["motion"] = "О нет, кто-то позарился на мои сочные булочки!" if int(motion) == 1 else "Пронесло, попка в безопасности"
            except Exception as e:
                print(f"Ошибка чтения данных: {e}")
        time.sleep(0.1)

picam2 = Picamera2()
video_config = picam2.create_video_configuration(main={"size": (1296, 972)})  # Укажите нужное разрешение
picam2.configure(video_config)
picam2.start()

def generate_frames():
    while True:
        try:
            frame = picam2.capture_array()
            _, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')
        except Exception as e:
            print(f"Ошибка генерации кадров: {e}")
            break

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route("/")
def index():
    return render_template("index1.html")

@app.route("/control", methods=["POST"])
def control():
    command = request.form.get("command")
    print(f"Получена команда: {command}")
    if command:
        try:
            arduino_platform.write(command.encode())
            print(f"Отправлено на Arduino: {command}")
        except Exception as e:
            print(f"Ошибка отправки команды: {e}")
    else:
        print("Пустая команда.")
    return "OK"

@app.route("/data")
def get_data():
    return jsonify(data)

if __name__ == "__main__":
    threading.Thread(target=read_from_arduino, daemon=True).start()
    app.run(host="0.0.0.0", port=5000)
