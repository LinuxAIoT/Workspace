import cv2
import logging
from ultralytics import YOLO

# 关闭 ultralytics 的调试输出
logging.getLogger('ultralytics').setLevel(logging.WARNING)

# 初始化摄像头
try:
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        raise ValueError("Unable to open the camera")
except Exception as e:
    print(f"Camera initialization failed: {e}")
    exit()

# 加载模型
try:
    model = YOLO('runs/detect/train/weights/best.pt')
except Exception as e:
    print(f"Model loading failed: {e}")
    exit()
try:
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        # https://docs.ultralytics.com/zh/modes/predict/#boxes    
        # 目标检测,并设置置信度阈值为0.7（置信度阈值：当检测的概率高于0.7时，将目标框出来）
        results = model(frame, conf=0.7)
        # 结果绘制和可视化
        annotated_frame = results[0].plot()
        # 输出检测信息
        for box in results[0].boxes:
            # 获取边界框坐标
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            # 获取置信度和类别信息
            conf = box.conf[0]
            # 获取类别ID
            class_id = int(box.cls[0])
            # 获取类别名称
            class_name = model.names[class_id]
            print(f"Detected: {class_name}, Confidence={conf:.2f}, Coordinates=({x1}, {y1}, {x2}, {y2})")
        # 显示结果
        cv2.imshow('YOLOv8 Detection', annotated_frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
finally:
    cap.release()
    cv2.destroyAllWindows()