#!/usr/bin/env python
import cv2
import asyncio
import websockets

delay = 10
camera_id = 0

def QR_creator():
                # let's encode this example string
    string_to_encode = " RNXG Industrial bot"

# the result image will be in minimal possible size
# which is what it should be to avoid unnecessary memory allocations
# because it would be easier to let the user to resize the result image
# (most of the time we need to resize it to fit our needs anyway, this saves one call to resize)
    minimal_qrcode = cv2.QRCodeEncoder.encode(cv2.QRCodeEncoder.create(), string_to_encode)

# for this example, we can resize it to 300x300
    qrcode = cv2.resize(minimal_qrcode, (300, 300), interpolation = cv2.INTER_AREA)
    cv2.imshow("QR" ,qrcode)
    cv2.waitKey(10)

def QR_scanner():

    cap = cv2.VideoCapture(camera_id)
    qcd = cv2.QRCodeDetector()

    while True:
        ret, frame = cap.read()

# ret_qr, decoded_info, points, _= cv2.QRCodeDetector.detectAndDecode(qrcode)
        if ret:
            ret_qr, decoded_info, points,_ = qcd.detectAndDecodeMulti(frame)
            if ret_qr:
                for s , p in zip(decoded_info, points):
                    if s:
                        print(s)
                        asyncio.run(hello(s))
                        color= (0,255,0)
                    else:
                        color= (0,0,255)
                    frame = cv2.polylines(frame, [p.astype(int)], True, color, 8)
            cv2.imshow('OpenCV QR Code', frame)

        if cv2.waitKey(delay) & 0xFF == ord('q'):
            break
    

async def hello(msg):
    async with websockets.connect("wss://InsubstantialRosyApplications.gaurav136.repl.co") as websocket:
        await websocket.send(msg)
        #await websocket.recv()
        
QR_creator()
QR_scanner()



