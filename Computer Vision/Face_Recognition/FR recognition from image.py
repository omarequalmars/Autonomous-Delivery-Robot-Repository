import cv2
import face_recognition

# Load the image
image_path = 'Face_Recognition/group.png'
image = cv2.imread(image_path)

# Convert the image to RGB (required by the face_recognition library)
image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

# Perform face detection
face_locations = face_recognition.face_locations(image_rgb)
face_encodings = face_recognition.face_encodings(image_rgb, face_locations)

# Define known faces and their names
known_faces = [

    face_recognition.face_encodings(cv2.cvtColor(cv2.imread("Face_Recognition/amr.png"), cv2.COLOR_BGR2RGB))[0],
    "amr",
    face_recognition.face_encodings(cv2.cvtColor(cv2.imread('Face_Recognition/essam.png'), cv2.COLOR_BGR2RGB))[0],
    "essam",
    face_recognition.face_encodings(cv2.cvtColor(cv2.imread('Face_Recognition/omar.png'), cv2.COLOR_BGR2RGB))[0],
    "omar",
    face_recognition.face_encodings(cv2.cvtColor(cv2.imread('Face_Recognition/joseph.png'), cv2.COLOR_BGR2RGB))[0],
    "joseph",

]

# Iterate over detected faces
for face_location, face_encoding in zip(face_locations, face_encodings):
    top, right, bottom, left = face_location
    
    # Compare face encoding with known faces
    matches = face_recognition.compare_faces(known_faces[::2], face_encoding)
    name = "Unknown"
    
    # Check for any matches
    if True in matches:
        matched_index = matches.index(True)
        name = known_faces[matched_index * 2 + 1]
    
    # Draw rectangles and label the faces
    cv2.rectangle(image, (left, top), (right, bottom), (0, 255, 0), 2)
    cv2.putText(image, name, (left, top - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

# Display the annotated image
cv2.imshow('Face Recognition', image)
cv2.waitKey(0)
cv2.destroyAllWindows()
