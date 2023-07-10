import cv2
import numpy as np
import utlis 


# A list to store the curvature values of the previous frames
curveList = []
# The number of frames to average over when calculating the curvature
avgVal=10

# The main function of the lane detection 
def getLaneCurve(img,display=2):
    """
    Takes an input image and returns the curvature of the lane.

    Parameters:
        img (numpy.ndarray): The input image.
        display (int, optional): The type of display to show. 
            0 - Displays nothing.
            1 - Displays only the resulting image with the lane lines and curvature information overlaid.
            2 - Displays intermediate processing steps in addition to the resulting image.

    Returns:
        float: The curvature of the lane, ranging from -1 (left curve) to 1 (right curve).
    """
    # Create a copy of the input image
    imgCopy = img.copy()
    imgResult = img.copy()

    #### STEP 1: Thresholding ####
    # Apply thresholding to the image to obtain a binary image with white pixels representing the lane lines
    imgThres = utlis.thresholding(img)

    #### STEP 2: Warping ####
    # Get the dimensions of the input image
    hT, wT, c = img.shape
    # Get the points of the trapezoidal region of interest using the trackbar values
    points = utlis.valTrackbars()
    # Warp the binary image to obtain a bird's eye view of the lane lines
    imgWarp = utlis.warpImg(imgThres,points,wT,hT)
    # Draw the points of the trapezoidal region of interest on the input image for visualization
    imgWarpPoints = utlis.drawPoints(imgCopy,points)

    #### STEP 3: Histogram ####
    # Compute a histogram of the bottom half of the warped image to find the midpoint of the lane lines
    middlePoint,imgHist = utlis.getHistogram(imgWarp,display=True,minPer=0.5,region=4)
    # Compute the average point between the two lane lines as the curve average point
    curveAveragePoint, imgHist = utlis.getHistogram(imgWarp, display=True, minPer=0.9)
    # Compute the raw curvature of the lane by subtracting the midpoint from the curve average point
    curveRaw = curveAveragePoint - middlePoint

    #### SETP 4: Smoothing ####
    # Add the raw curvature to the curve list
    curveList.append(curveRaw)
    # If the curve list is longer than the average value, remove the oldest curvature value
    if len(curveList)>avgVal:
        curveList.pop(0)
    # Compute the average curvature from the last few frames
    curve = int(sum(curveList)/len(curveList))

    #### STEP 5: Visualization ####
    # Draw the lane lines and the curve information onto the bird's eye view of the image
    
    if display != 0:
        imgInvWarp = utlis.warpImg(imgWarp, points, wT, hT, inv=True)
        imgInvWarp = cv2.cvtColor(imgInvWarp, cv2.COLOR_GRAY2BGR)
        imgInvWarp[0:hT // 3, 0:wT] = 0, 0, 0
        imgLaneColor = np.zeros_like(img)
        imgLaneColor[:] = 0, 255, 0
        imgLaneColor = cv2.bitwise_and(imgInvWarp, imgLaneColor)
        imgResult = cv2.addWeighted(imgResult, 1, imgLaneColor, 1, 0)
        midY = 450
        cv2.putText(imgResult, str(curve), (wT // 2 - 80, 85), cv2.FONT_HERSHEY_COMPLEX, 2, (255, 0, 255), 3)
        cv2.line(imgResult, (wT // 2, midY), (wT // 2 + (curve * 3), midY), (255, 0, 255), 5)
        cv2.line(imgResult, ((wT // 2 + (curve * 3)), midY - 25), (wT // 2 + (curve * 3), midY + 25), (0, 255, 0), 5)
        for x in range(-30, 30):
            w = wT // 20
            cv2.line(imgResult, (w * x + int(curve // 50), midY - 10),
                     (w * x + int(curve // 50), midY + 10), (0, 0, 255), 2)
        #fps = cv2.getTickFrequency() / (cv2.getTickCount() - timer);
        #cv2.putText(imgResult, 'FPS ' + str(int(fps)), (20, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (230, 50, 50), 3);
    if display == 2:
        imgStacked = utlis.stackImages(0.7, ([img, imgWarpPoints, imgWarp],
                                             [imgHist, imgLaneColor, imgResult]))
        cv2.imshow('ImageStack', imgStacked)
    elif display == 1:
        cv2.imshow('Resutlt', imgResult)

    # Return the curvature of the lane
    return curve/100




