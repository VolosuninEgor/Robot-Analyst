from flask import Flask, render_template, request, Response, jsonify
from picamera2 import Picamera2
import cv2
import serial
import threading
import time
import queue
import psutil

# Настраиваем Flask
app = Flask(__name__)

# Подключаем Arduino
arduino_sensor = serial.Serial('/dev/ttyUSB0', 9600)
arduino_platform = serial.Serial('/dev/ttyUSB1', 9600)

# Очередь для данных с Arduino
data_queue = queue.Queue()
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

# Читаем данные с Arduino
def read_from_arduino():
    global data
    while True:
        if arduino_sensor.in_waiting > 0:
            try:
                line = arduino_sensor.readline().decode('utf-8').strip()
                parsed_data = line.split(',')
                if len(parsed_data) == 8:
                    distance1, distance2, distance3, distance4, humidity, temperature, light, motion = parsed_data
                    new_data = {
                        "distance1": float(distance1),
                        "distance2": float(distance2),
                        "distance3": float(distance3),
                        "distance4": float(distance4),
                        "humidity": float(humidity),
                        "temperature": float(temperature),
                        "light": str(float(light)) if float(light) > 5 else "Темно, хоть глаз выколи!",
                        "motion": "О нет, кто-то позарился на мои сочные булочки!" if int(motion) == 1 else "Пронесло, попка в безопасности"
                    }
                    data_queue.put(new_data)
            except Exception as e:
                print(f"Ошибка чтения данных: {e}")
        time.sleep(0.5)  # Уменьшаем нагрузку на CPU

# Настраиваем камеру
picam2 = Picamera2()
video_config = picam2.create_video_configuration(main={"size": (640, 480)})
picam2.configure(video_config)
picam2.set_controls({"FrameRate": 10})  # Ограничиваем FPS (ставь 10-15, если лаги)
picam2.start()


# Переменная для хранения последнего кадра
frame_lock = threading.Lock()
current_frame = None

# Обновление кадров в фоне
def update_frame():
    global current_frame
    while True:
        with frame_lock:
            frame = picam2.capture_array()
            _, buffer = cv2.imencode('.jpg', frame)
            current_frame = buffer.tobytes()
        time.sleep(0.1)  # Ограничиваем FPS

# Генерация кадров
def generate_frames():
    while True:
        with frame_lock:
            if current_frame:
                yield (b'--frame\r\n'
                       b'Content-Type: image/jpeg\r\n\r\n' + current_frame + b'\r\n')
        time.sleep(0.1)

# Запускаем потоки
threading.Thread(target=read_from_arduino, daemon=True).start()
threading.Thread(target=update_frame, daemon=True).start()

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route("/")
def index():
    return render_template("index1.html")

@app.route("/control", methods=["POST"])
def control():
    command = request.form.get("command")
    if command:
        try:
            arduino_platform.write(command.encode())
            print(f"Отправлено на Arduino: {command}")
        except Exception as e:
            print(f"Ошибка отправки команды: {e}")
    return "OK"

@app.route("/data")
def get_data():
    global data
    while not data_queue.empty():
        data = data_queue.get()
    return jsonify(data)


if __name__ == "__main__":
    print("Сервер запущен на порту 5000")
    app.run(host="0.0.0.0", port=5000, threaded=True)
