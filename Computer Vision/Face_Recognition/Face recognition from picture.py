import cv2
import face_recognition

# Load known faces and their names
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

# Initialize the video capture object
video_capture = cv2.VideoCapture(0)  # Use 0 for the default camera

while True:
    # Capture a frame from the video feed
    ret, frame = video_capture.read()
    
    # Convert the frame to RGB
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    
    # Perform face detection
    face_locations = face_recognition.face_locations(frame_rgb)
    face_encodings = face_recognition.face_encodings(frame_rgb, face_locations)
    
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
        cv2.rectangle(frame, (left, top), (right, bottom), (0, 255, 0), 2)
        cv2.putText(frame, name, (left, top - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

    # Display the frame with annotations
    cv2.imshow('Face Recognition', frame)

    # Check for 'q' key press to exit
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the video capture object and close windows
video_capture.release()
cv2.destroyAllWindows()
