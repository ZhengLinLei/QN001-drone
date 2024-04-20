import cv2
import requests
import numpy as np

# Captura la imagen de la cámara
cap = cv2.VideoCapture(0)

ret, frame = cap.read()

# Asegúrate de liberar la cámara después de capturar la imagen
cap.release()

# Codifica la imagen en formato JPEG
_, img_encoded = cv2.imencode('.jpg', frame)
img_data = img_encoded.tobytes()

# Envía la imagen a través de una solicitud POST
url = 'http://127.0.0.1:8000/0'
response = requests.post(url, data=img_data, headers={'Content-Type': 'image/jpeg'})

print(response.text)
