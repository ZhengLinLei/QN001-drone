from flask import Flask, request
import cv2
import numpy as np
import io

app = Flask(__name__)

@app.route('/0', methods=['POST'])
def upload():
    print('Received image')
    # Check content type is image/jpeg
    if request.headers['Content-Type'] != 'image/jpeg':
        return 'Content-Type debe ser image/jpeg'
    
    img_data = request.data  # obtén los datos de la imagen
    nparr = np.fromstring(img_data, np.uint8)
    img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

    # Save the image
    cv2.imwrite('./tmp/image.jpg', img)

    # # Muestra la imagen
    # cv2.imshow('image', img)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()

    # Return 201
    return '', 201

if __name__ == '__main__':
    # Run server and listen on port 8000 to be accessible from the network
    app.run(host='0.0.0.0', port=8000)
