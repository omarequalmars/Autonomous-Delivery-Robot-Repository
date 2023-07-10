import cv2
import face_recognition

# Load the image
image_path = 'Face_Recognition/group.png'
image = cv2.imread(image_path)

# Convert the image to RGB (required by the face_recognition library)
image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

# Perform face detection
face_locations = face_recognition.face_locations(image_rgb)

# Draw rectangles around the detected faces
for top, right, bottom, left in face_locations:
    cv2.rectangle(image, (left, top), (right, bottom), (0, 255, 0), 2)

# Display the annotated image
cv2.imshow('Face Detection', image)
cv2.waitKey(0)
cv2.destroyAllWindows()