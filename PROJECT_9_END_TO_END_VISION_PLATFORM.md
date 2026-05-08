# PROJECT 9: End-to-End Vision Platform Capstone

**Mục tiêu:** Ghép toàn bộ kiến thức từ 8 project trước thành một hệ thống CV hoàn chỉnh, có thể demo như một mini platform chứ không chỉ là một bài lab.

**Độ khó:** ⭐⭐⭐⭐⭐
**Thời gian:** 4-6 tuần
**Đầu ra tối thiểu:** một capstone có video demo, source code sạch, config, README, metrics, diagram kiến trúc và roadmap nâng cấp.

## Mục tiêu capstone

Bạn phải chọn một use case thật, ví dụ:

- Smart camera phát hiện người/xe và record event.
- Perception pipeline cho robot di chuyển trong nhà.
- Hệ multi-camera giám sát với dashboard metrics.
- AI edge vision box nhận RTSP, detect, publish ROS 2 topic, đồng thời ghi clip.

## Công nghệ bạn có thể phối hợp

- OpenCV cho preprocess/overlay.
- GStreamer cho ingest, record, streaming.
- ROS 2 cho orchestration node-based.
- ONNX Runtime/NCNN/OpenCV DNN cho inference.
- Docker cho đóng gói.
- Logging/metrics/config từ Project 5.

## Yêu cầu capstone tối thiểu

- Có ít nhất 1 input source thật.
- Có pipeline xử lý hoặc inference thật.
- Có kiến trúc module rõ ràng.
- Có config file.
- Có logging và metrics.
- Có tài liệu chạy lại.
- Có video demo hoặc ảnh minh họa kết quả.

## Kiến trúc gợi ý

Phiên bản edge camera:

`RTSP/GStreamer source -> capture buffer -> preprocess -> inference -> event logic -> overlay -> record/stream -> metrics`

Phiên bản robotics:

`camera nodes -> sync -> preprocess -> inference -> tracker -> ROS 2 topics -> visualization/logging`

## Các bước thực hiện

### Bước 1: Chọn use case và viết yêu cầu kỹ thuật

Bạn phải chốt rõ:

- input là gì
- output là gì
- latency mục tiêu bao nhiêu
- FPS mục tiêu bao nhiêu
- chạy trên máy nào
- có cần network/ROS/GStreamer không

### Bước 2: Vẽ sơ đồ kiến trúc

Ít nhất phải có:

- source
- buffer/queue
- processing stage
- inference stage
- output/record/stream
- diagnostics/metrics

### Bước 3: Chọn module từ các project trước

Ví dụ:

- buffer lấy từ Project 1
- optimization lấy từ Project 2
- IPC/network từ Project 3
- inference wrapper từ Project 4
- config/logging từ Project 5
- GStreamer ingest từ Project 6
- ROS 2 orchestration từ Project 7
- sync từ Project 8

### Bước 4: Đặt tiêu chí đánh giá

- startup time
- steady-state FPS
- average latency
- max latency
- drop count
- CPU usage
- memory footprint
- độ ổn định sau 30-60 phút chạy liên tục

### Bước 5: Xây phiên bản MVP

- chỉ làm đường đi quan trọng nhất.
- ưu tiên chạy end-to-end trước.
- chưa cần UI đẹp hoặc tính năng phụ.

### Bước 6: Thêm feature production

- config profile `dev` và `prod`
- log theo module
- dashboard metrics đơn giản hoặc file metrics
- graceful restart hoặc reconnect source

### Bước 7: Viết báo cáo kỹ thuật

Báo cáo nên trả lời được:

- hệ làm gì
- tại sao chọn stack này
- bottleneck chính là gì
- đã đo những gì
- nếu có 2 tuần nữa sẽ nâng cấp gì

## Bài tập bắt buộc

1. Chọn 1 use case cụ thể và khóa scope.
2. Tạo sơ đồ kiến trúc hệ thống.
3. Viết MVP chạy end-to-end.
4. Có benchmark hoặc metrics thật.
5. Có README chỉ cách build, run, config, debug.

## Bài tập nâng cao

1. Thêm dashboard web hoặc visualization riêng.
2. Tách service chạy đa process hoặc đa máy.
3. Thêm recorder và playback tool.
4. Thêm test case tự động cho ít nhất một module.

## Tiêu chí hoàn thành

- Người khác có thể đọc README và chạy lại.
- Bạn có thể demo trực tiếp mà không sửa tay quá nhiều.
- Kiến trúc không phải một file `main.cpp` khổng lồ.
- Có lý do kỹ thuật rõ cho từng framework được dùng.

## Câu hỏi tự kiểm tra

- Vì sao use case này cần GStreamer hoặc ROS 2, hay thực ra không cần?
- Bottleneck lớn nhất của hệ nằm ở đâu?
- Nếu scale lên nhiều camera hoặc nhiều model, kiến trúc hiện tại chịu được không?
- Nếu đem đi demo trên máy khác, bạn cần chuẩn bị gì?

## Kết quả mong đợi cuối lộ trình

Sau project này, bạn không chỉ biết viết code CV. Bạn có thể nói chuyện nghiêm túc về pipeline, latency, throughput, buffer policy, inference stack, media graph, orchestration, logging, metrics và production trade-off.
