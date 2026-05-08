# PROJECT 3: Network Frame Server

**Mục tiêu:** Tách capture và processing thành nhiều process hoặc service, sau đó truyền frame hoặc metadata bằng IPC/network đúng kiểu hệ thống thực tế.

**Độ khó:** ⭐⭐⭐
**Thời gian:** 2-3 tuần
**Đầu ra tối thiểu:** 2 process chạy riêng, một bên capture/publish, một bên receive/process/display.

## Bạn sẽ học gì

- Vì sao nhiều hệ CV thực tế không để mọi thứ trong một process.
- Khác nhau giữa shared memory, Unix domain socket và TCP socket.
- Cách định nghĩa `frame header` và protocol cơ bản.
- Cách làm reconnect, timeout, cleanup và backpressure.
- Cách cân bằng giữa throughput, latency và độ phức tạp hệ thống.

## Công nghệ nên dùng

- C++17.
- POSIX socket.
- Unix domain socket cho local IPC.
- Shared memory cho local low-copy path.
- TCP socket cho remote hoặc kiến trúc client/server.
- OpenCV cho encode/decode hoặc kiểm thử payload.

## Bài toán bạn đang giải

Khi pipeline lớn dần, capture, processing, inference, streaming không nên nằm cùng một process. Một lỗi ở module này có thể kéo sập cả chương trình. Bạn cần biết cách tách module mà vẫn giữ tốc độ chấp nhận được.

## Kiến trúc mục tiêu

Phiên bản đơn giản:

`Capture Process -> IPC Transport -> Processing Process`

Phiên bản mở rộng:

`Capture Service -> Frame Router -> Multiple Consumers`

## Các bước thực hiện

### Bước 1: Chọn dữ liệu sẽ truyền

Bắt đầu bằng một trong hai lựa chọn:

- chỉ truyền metadata nhỏ, ví dụ frame id, timestamp, width, height.
- truyền frame nén JPEG để giảm công truyền.

**Khuyến nghị:** làm metadata trước, rồi tới JPEG frame, cuối cùng mới tới raw/shared memory.

### Bước 2: Thiết kế `FrameMessage`

Ví dụ các trường:

- `frame_id`
- `timestamp`
- `width`
- `height`
- `pixel_format`
- `payload_size`

Nếu truyền frame, thêm `payload` phía sau header.

### Bước 3: Viết local IPC bản đầu

Ưu tiên:

- Unix domain socket nếu chạy cùng máy.
- Gửi header + JPEG bytes.
- Process nhận giải mã và hiển thị.

**Mục đích:** hiểu rõ framing, protocol và reconnect logic.

### Bước 4: So sánh với shared memory

- Tạo vùng shared memory kích thước cố định.
- Dùng ring buffer hoặc slot buffer trong shared memory.
- Dùng socket nhỏ hoặc semaphore/condition để báo có frame mới.

**Mục đích:** hiểu vì sao shared memory nhanh hơn copy qua socket nhưng phức tạp hơn nhiều.

### Bước 5: Thêm TCP server

- Tạo server nhận kết nối từ client.
- Truyền frame nén hoặc metadata.
- Thêm timeout, mất kết nối, reconnect.

### Bước 6: Xử lý lỗi hệ thống

Bạn phải xử lý tối thiểu:

- client ngắt đột ngột
- header lỗi hoặc payload thiếu
- process thoát bất thường
- cleanup shared memory khi crash

### Bước 7: Đo số liệu

Đo tối thiểu:

- latency từ capture tới receive
- throughput theo FPS
- kích thước payload trung bình
- CPU usage khi encode/decode

## Bài tập bắt buộc

1. Tạo 2 executable riêng.
2. Dùng Unix domain socket để gửi frame JPEG local.
3. In timestamp ở cả bên gửi và bên nhận.
4. Thêm reconnect hoặc graceful shutdown.
5. So sánh local socket và shared memory theo latency.

## Bài tập nâng cao

1. Hỗ trợ nhiều client đọc cùng một nguồn frame.
2. Thêm frame drop policy khi consumer chậm.
3. Hỗ trợ stream qua TCP sang máy khác trong mạng LAN.
4. Thêm checksum hoặc basic integrity check cho payload.

## Khi nào dùng cái gì

- `Unix domain socket`: local IPC, dễ làm, dễ debug.
- `TCP socket`: khi có khả năng scale ra nhiều máy.
- `Shared memory`: khi local throughput cao là ưu tiên số một.

## Tiêu chí hoàn thành

- Hai process có thể chạy và dừng độc lập.
- Khi process nhận chậm, hệ không bị chết cứng.
- Có số liệu rõ về latency/truyền frame.
- Bạn giải thích được vì sao shared memory nhanh nhưng khó hơn socket.

## Câu hỏi tự kiểm tra

- Khi nào nên truyền frame raw, khi nào nên truyền JPEG?
- Vì sao shared memory vẫn cần cơ chế signaling riêng?
- Nếu server có nhiều client chậm, policy backpressure nên là gì?

## Lỗi thường gặp

- Không define rõ header dẫn đến parse payload sai.
- Chỉ test happy path, không test disconnect.
- Cleanup shared memory không triệt để sau crash.
- Dùng TCP cho local use case đơn giản mà không có lý do.

## Sau project này bạn sẽ sẵn sàng làm gì

- Gắn AI inference thành service ở Project 4.
- Nâng cấp đường media bằng GStreamer ở Project 6.
- Hiểu cách chia service trong kiến trúc production ở Project 5 và 9.
