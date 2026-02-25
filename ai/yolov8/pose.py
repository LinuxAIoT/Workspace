import cv2
from ultralytics import YOLO

# Load a pretrained YOLO26n-pose Pose model
model = YOLO("yolo26n-pose.pt")
# or
# model = YOLO("runs/pose/train/weights/best.pt")  #! load a custom model，need to train first
# Initialize the cam
cap = cv2.VideoCapture(0)
try:
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        # Perform pose estimation with a confidence threshold of 0.7
        results = model(frame, conf=0.7)
        # Visualize the results
        annotated_frame = results[0].plot()
        # Display the results
        cv2.imshow('YOLOv8 Pose Estimation', annotated_frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
finally:
    cap.release()
    cv2.destroyAllWindows()