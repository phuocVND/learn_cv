# PROJECT 5: Production System Integration

**Mục tiêu:** Biến pipeline nghiên cứu thành hệ thống có thể vận hành: cấu hình được, log được, monitor được, đóng gói được, restart được.

**Độ khó:** ⭐⭐⭐⭐⭐
**Thời gian:** 2-3 tuần
**Đầu ra tối thiểu:** một hệ CV có config file, logging, metrics cơ bản, Dockerfile và tài liệu chạy rõ ràng.

## Bạn sẽ học gì

- Khác nhau giữa code demo và code có thể vận hành lâu dài.
- Cách đưa config ra file thay vì hard-code.
- Cách log đúng mức và đủ ngữ cảnh.
- Cách theo dõi health, FPS, latency, queue size, reconnect count.
- Cách đóng gói bằng Docker và viết runbook ngắn.

## Công nghệ nên dùng

- C++17.
- `spdlog` hoặc logging library tương đương.
- `yaml-cpp` hoặc JSON config.
- Docker.
- CMake.
- Tùy chọn: Prometheus text endpoint hoặc metrics file đơn giản.

## Bài toán bạn đang giải

Nhiều project CV dừng ở mức "chạy được trên máy dev". Nhưng production yêu cầu khác: khi cấu hình thay đổi thì không sửa code, khi lỗi phải có log, khi chậm phải có metrics, khi chuyển máy phải chạy lại được.

## Thành phần production tối thiểu

- `config`: camera source, resolution, model path, buffer size, thresholds.
- `logging`: startup config, frame stats, warning, error.
- `metrics`: FPS, latency, queue depth, drop count.
- `packaging`: Dockerfile hoặc script build/run.
- `docs`: README và known issues.

## Các bước thực hiện

### Bước 1: Tách config khỏi code

Tạo file config chứa:

- source type
- source path hoặc camera id
- buffer size
- display enable/disable
- model path
- logging level

**Mục đích:** đổi hành vi hệ thống mà không cần sửa `main.cpp`.

### Bước 2: Chuẩn hóa logging

Log tối thiểu các mốc:

- start app
- loaded config
- camera opened
- model loaded
- pipeline started
- warnings khi queue đầy hoặc inference chậm
- graceful shutdown

### Bước 3: Thêm metrics runtime

Có thể bắt đầu rất đơn giản:

- rolling FPS
- average latency
- max latency trong 10 giây gần nhất
- buffer occupancy
- frame drop count

### Bước 4: Tách module rõ ràng

Ví dụ:

- `CaptureService`
- `FrameBuffer`
- `InferenceService`
- `OverlayRenderer`
- `AppController`

**Mục đích:** code dễ test và dễ thay module hơn.

### Bước 5: Viết health checks nội bộ

- kiểm tra source còn sống không
- kiểm tra worker thread có đang chạy không
- kiểm tra model load thành công không

### Bước 6: Đóng gói bằng Docker

Docker image tối thiểu cần:

- dependency OpenCV/runtime liên quan
- binary
- config mẫu
- command chạy mặc định

### Bước 7: Viết README vận hành

README tối thiểu phải có:

- build command
- run command
- config sample
- cách đổi input source
- ý nghĩa metrics/log chính
- cách xử lý lỗi thường gặp

## Bài tập bắt buộc

1. Chuyển ít nhất 5 tham số hard-code ra config file.
2. Thêm logging có mức `info`, `warn`, `error`.
3. In metrics mỗi 5 giây hoặc expose ra file/text endpoint.
4. Tạo Dockerfile build và run được.
5. Viết README ngắn cho người khác chạy lại hệ thống.

## Bài tập nâng cao

1. Tách config theo môi trường `dev` và `prod`.
2. Thêm log rotation hoặc file sink.
3. Thêm endpoint health check nhỏ bằng HTTP.
4. Cho phép bật/tắt module bằng config.

## Tiêu chí hoàn thành

- Chạy lại được trên máy khác với ít thay đổi nhất.
- Không phải sửa source code khi đổi source video hoặc model path.
- Có log đủ để debug lỗi phổ biến.
- Có metrics đủ để trả lời câu hỏi "hệ đang chậm ở đâu".

## Câu hỏi tự kiểm tra

- Vì sao hard-code source path là dấu hiệu code chưa production-ready?
- Log nào là bắt buộc khi pipeline khởi động?
- Metrics nào hữu ích nhất khi debugging realtime lag?

## Lỗi thường gặp

- Log quá nhiều nhưng không có cấu trúc.
- Có config file nhưng không validate giá trị đầu vào.
- Docker chạy được nhưng không mount config/video source hợp lý.
- Không phân biệt log cho startup, runtime warning và fatal error.

## Sau project này bạn sẽ sẵn sàng làm gì

- Chuyển media ingestion sang GStreamer ở Project 6.
- Chuyển pipeline sang node-based architecture ở ROS 2 trong Project 7.
- Làm capstone thực thụ ở Project 9.
