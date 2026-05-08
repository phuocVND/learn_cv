# PROJECT 4: AI Inference Pipeline

**Mục tiêu:** Đưa model AI vào pipeline video realtime, hiểu toàn bộ luồng `preprocess -> inference -> postprocess -> overlay`, và đo được latency/FPS thực tế.

**Độ khó:** ⭐⭐⭐⭐
**Thời gian:** 2-4 tuần
**Đầu ra tối thiểu:** 1 ứng dụng nhận frame, chạy model detection hoặc classification, vẽ kết quả lên màn hình.

## Bạn sẽ học gì

- Cách chọn runtime inference phù hợp.
- Cách chuẩn hóa input đúng với model.
- Cách tách inference ra thành stage riêng trong pipeline.
- Khác nhau giữa synchronous inference và asynchronous inference.
- Cách đo `model latency`, `end-to-end latency`, `throughput`.

## Công nghệ nên dùng

Chọn 1 stack chính, 1 stack phụ để so sánh:

- Ưu tiên dễ tiếp cận:
  - OpenCV DNN
  - ONNX Runtime
- Tùy platform:
  - NCNN cho edge/mobile nhẹ
  - OpenVINO nếu làm trên Intel

Các thư viện phụ:

- OpenCV cho preprocess/postprocess/overlay.
- C++17.
- CMake.

## Bài toán bạn đang giải

Khi thêm AI vào pipeline, thứ giết realtime không chỉ là model nặng, mà còn là preprocess sai, copy ảnh quá nhiều, postprocess không tối ưu hoặc inference block cả luồng hiển thị.

## Kiến trúc mục tiêu

`capture -> preprocess -> inference -> postprocess -> overlay -> display`

Phiên bản tốt hơn:

`capture -> queue -> inference worker -> result queue -> overlay/display`

## Các bước thực hiện

### Bước 1: Chọn model nhỏ, dễ debug

Gợi ý:

- classification: MobileNetV2
- detection: YOLOv5n, YOLOv8n, NanoDet hoặc model ONNX nhẹ

**Khuyến nghị:** bắt đầu từ model nhỏ để chắc pipeline đúng trước khi tối ưu.

### Bước 2: Xác định chuẩn input của model

Bạn phải ghi rõ:

- input shape
- color order (`BGR` hay `RGB`)
- normalize rule
- mean/std nếu có
- output tensor format

### Bước 3: Viết preprocess chuẩn hóa

Ví dụ:

- resize
- letterbox nếu detection model yêu cầu
- convert BGR -> RGB
- normalize về `[0, 1]`
- chuyển `HWC` sang `CHW` nếu runtime yêu cầu

### Bước 4: Viết inference wrapper

Tạo class kiểu:

- `InferenceEngine`
- `loadModel()`
- `run(const FramePacket&)`
- `postprocess()`

**Mục đích:** sau này đổi runtime không phải sửa khắp code.

### Bước 5: Vẽ output ra frame

Tối thiểu:

- bbox
- label
- confidence
- inference time
- total FPS

### Bước 6: Tách inference thành worker riêng

- Dùng queue giữa capture/process và inference.
- So sánh sync với async.
- Nếu model nặng, thêm frame skipping policy.

### Bước 7: Benchmark

Đo ít nhất:

- preprocess time
- inference time
- postprocess time
- total frame latency
- stable FPS trong 30-60 giây

## Bài tập bắt buộc

1. Chạy được ít nhất 1 model trên video hoặc webcam.
2. Hiển thị kết quả AI lên frame.
3. Ghi log thời gian preprocess/inference/postprocess.
4. So sánh synchronous và worker-based inference.
5. Thử 2 độ phân giải đầu vào khác nhau và ghi lại kết quả.

## Bài tập nâng cao

1. Thêm batching mini-size nếu runtime hỗ trợ.
2. Thêm tracking đơn giản sau detection.
3. Thêm ROI inference để tăng tốc.
4. Cho phép đổi model bằng file config.

## Kinh nghiệm chọn công nghệ

- Dễ bắt đầu nhất: OpenCV DNN.
- Cân bằng tốt giữa ease-of-use và production path: ONNX Runtime.
- Nếu nhắm edge nhẹ: NCNN.
- Nếu mục tiêu robotics/ROS 2: chuẩn hóa interface output ngay từ bây giờ.

## Tiêu chí hoàn thành

- Model chạy ổn định trên luồng video.
- Overlay đúng, không bị sai scale hoặc sai label.
- Có số liệu rõ cho từng stage.
- Bạn giải thích được model chậm vì preprocessing, inference hay postprocess.

## Câu hỏi tự kiểm tra

- Tại sao preprocess sai có thể làm model chạy nhưng kết quả vô nghĩa?
- Vì sao inference latency và end-to-end latency không giống nhau?
- Khi nào nên skip frame để giữ realtime?

## Lỗi thường gặp

- Resize sai tỉ lệ hoặc normalize sai.
- Vẽ bbox theo tọa độ model output nhưng quên scale về ảnh gốc.
- Block luồng chính trong khi inference chạy lâu.
- Chỉ nhìn FPS mà quên kiểm tra accuracy cơ bản.

## Sau project này bạn sẽ sẵn sàng làm gì

- Bọc thành node/service để dùng trong ROS 2 ở Project 7.
- Streaming kết quả qua process hoặc network ở Project 3 và 9.
- Đưa toàn bộ hệ vào production pipeline ở Project 5.
