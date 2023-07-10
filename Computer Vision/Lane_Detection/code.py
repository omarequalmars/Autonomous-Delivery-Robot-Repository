from LDM import getLaneCurve 
import cv2
import serial
import time
import utlis

def write(x):
    x = str(x)
    print('curves= ' + x)
    ArduinoSerial.write(x.encode('utf-8'))
    print(x)
    time.sleep(0.05)

cap = cv2.VideoCapture(0)
initialTrackBarVals = [95,175,0,255]
utlis.initializeTrackbars(initialTrackBarVals)

ArduinoSerial = serial.Serial('COM5',9600, timeout = 0.1) 
time.sleep(1)


while True:
    _,img = cap.read()
    img = cv2.resize(img, (480,240)) 
    curves = getLaneCurve(img)
    # curves = 0.09
    write(curves)

    cv2.waitKey(1)



    






