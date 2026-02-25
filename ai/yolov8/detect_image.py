from ultralytics import YOLO

# Load custom model
model = YOLO("runs/detect/train3/weights/best.pt")

# Define path to the image file
source = "test/hard_hat_workers.png"

# Run inference on the source
results = model(source)  # list of Results objects

results[0].show()  # display results
results[0].save("test/hard_hat_workers_output.png")  # save results 