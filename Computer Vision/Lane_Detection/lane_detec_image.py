import cv2
import numpy as np
import utlis

cap = cv2.VideoCapture(0)  # access the webcam
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

# Define the lower and upper bounds of the white color in HSV
lower_white = np.array([65, 0, 0])
upper_white = np.array([255, 160, 255])

while True:
    ret, frame = cap.read()  # Read a frame from the video

    # Convert the image to HSV
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Threshold the image to detect white lanes
    mask_white = cv2.inRange(hsv, lower_white, upper_white)

    # Apply Gaussian blur to smooth the image and reduce noise
    blur = cv2.GaussianBlur(mask_white, (5, 5), 0)

    # Apply Canny edge detection to extract the edges
    edges = cv2.Canny(blur, 50, 150)

    # Define a region of interest (ROI) where the lanes should be detected
    roi = np.array([[(0, 480), (0, 300), (640, 300), (640, 480)]], dtype=np.int32)
    masked_edges = cv2.fillPoly(edges, roi, 255)

    # Detect lines using the Hough transform
    lines = cv2.HoughLinesP(masked_edges, 1, np.pi/180, 50, minLineLength=50, maxLineGap=200)

    # Draw the detected lines on the original image
    if lines is not None:
        for line in lines:
            x1, y1, x2, y2 = line[0]
            cv2.line(frame, (x1, y1), (x2, y2), (0, 0, 255), 2)

    # Display the resulting image
    cv2.imshow('Lane detection', frame)
    
    cv2.waitKey(1)

cap.release()  # Release the capture device
cv2.destroyAllWindows()  # Close all windows
