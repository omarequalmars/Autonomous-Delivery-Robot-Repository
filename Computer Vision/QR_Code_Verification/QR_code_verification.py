import cv2
from pyzbar.pyzbar import decode

cap = cv2.VideoCapture(0)

while True:
    _, frame = cap.read()

    decoded_objs = decode(frame)
    

    for obj in decoded_objs:
        qr_data = obj.data.decode('utf-8')
        print(qr_data)
        if qr_data == "valid":
            print("True")
        else:
             print("False")
        
    cv2.imshow("Frame", frame)
    if cv2.waitKey(1) == ord('q'):
        break
cap.release()
cv2.destroyAllWindows()