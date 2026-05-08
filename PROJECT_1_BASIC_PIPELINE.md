# PROJECT 1: Basic Streaming Pipeline

**Mục tiêu:** Xây pipeline ảnh/video cơ bản theo kiểu realtime, có `producer/consumer`, có `circular buffer`, có đo `FPS`, và không rò rỉ tài nguyên.

**Độ khó:** ⭐
**Thời gian:** 1-2 tuần
**Đầu ra tối thiểu:** 1 ứng dụng đọc camera hoặc video file, hiển thị frame, vẽ FPS, tắt an toàn.

## Bạn sẽ học gì

- Vì sao pipeline realtime cần tách luồng đọc và luồng xử lý.
- Cách dùng `std::thread`, `std::mutex`, `std::condition_variable`.
- Cách thiết kế `bounded buffer` hoặc `circular buffer` cho frame.
- Cách đo `capture FPS`, `processing FPS`, thời gian xử lý mỗi frame.
- Cách dùng RAII để quản lý `VideoCapture`, thread, buffer và file handle.

## Công nghệ nên dùng

- C++17 hoặc mới hơn.
- OpenCV (`core`, `highgui`, `videoio`, `imgproc`).
- STL threading (`thread`, `atomic`, `mutex`, `condition_variable`, `chrono`).
- CMake.
- Sanitizer hoặc tool check leak nếu có điều kiện.

## Bài toán bạn đang giải

Bạn có một nguồn dữ liệu đến liên tục, ví dụ webcam. Nếu bạn vừa đọc frame vừa xử lý trực tiếp trong một vòng lặp duy nhất, hệ thống sẽ nhanh nghẽn khi khối xử lý nặng lên. Project này buộc bạn tách riêng phần đọc và phần tiêu thụ dữ liệu, sau đó dùng buffer cố định để giữ nhịp.

Luồng tổng thể:

`capture -> buffer -> process/display -> shutdown`

## Kiến trúc mục tiêu

- `CaptureThread`: chỉ đọc frame từ camera hoặc file.
- `FrameBuffer`: circular buffer kích thước cố định, ví dụ 4 hoặc 8 frame.
- `Consumer/MainThread`: lấy frame ra, xử lý nhẹ, vẽ FPS và hiển thị.
- `StopSignal`: cờ dừng chung để thoát an toàn.

## Cấu trúc thư mục gợi ý

```text
project1/
  include/
    core/
      frame_buffer.hpp
      app_config.hpp
  src/
    main.cpp
    frame_buffer.cpp
    capture_worker.cpp
  CMakeLists.txt
  README.md
```

## Các bước thực hiện

### Bước 1: Viết bản baseline 1 luồng

- Mở camera bằng `cv::VideoCapture`.
- Hiển thị frame bằng `cv::imshow`.
- Đo FPS đơn giản bằng `std::chrono`.
- In ra metadata cơ bản: width, height, `CAP_PROP_FPS`.

**Mục đích:** Có baseline để so với phiên bản nhiều luồng.

### Bước 2: Tách luồng capture

- Tạo 1 thread chỉ làm việc đọc frame.
- Mỗi frame đọc xong phải được copy hoặc `clone()` đúng lúc trước khi đẩy đi.
- Không xử lý ảnh nặng trong thread capture.

**Mục đích:** Luồng đọc luôn ổn định, ít bị chặn bởi phần xử lý.

### Bước 3: Tự cài circular buffer

- Dùng một mảng hoặc `std::vector<cv::Mat>` có kích thước cố định.
- Quản lý `head`, `tail`, `count`.
- `push()` khi buffer chưa đầy.
- `pop()` khi buffer chưa rỗng.
- Nếu buffer đầy thì chọn 1 trong 2 chiến lược:
  - block producer.
  - drop frame cũ hoặc frame mới.

**Khuyến nghị Project 1:** block producer trước, drop policy làm bài nâng cao.

### Bước 4: Đồng bộ đa luồng đúng cách

- Dùng `std::mutex` để bảo vệ buffer.
- Dùng `std::condition_variable` để chờ khi buffer rỗng hoặc đầy.
- Dùng `std::atomic<bool>` hoặc cờ dừng có lock để thông báo shutdown.

**Mục đích:** Tránh busy waiting và race condition.

### Bước 5: Viết consumer loop

- Luồng chính hoặc thread thứ hai `pop()` frame từ buffer.
- Vẽ FPS, frame index, timestamp.
- Hiển thị bằng OpenCV window.
- Nhấn `q` hoặc `ESC` để dừng.

### Bước 6: Đo số liệu

Bạn nên đo ít nhất 3 số:

- `Capture FPS`: tốc độ thread capture đọc được frame.
- `Display/Process FPS`: tốc độ thread tiêu thụ xử lý xong frame.
- `Buffer occupancy`: số frame đang tồn tại trong buffer.

Nếu muốn tốt hơn, đo thêm:

- thời gian frame nằm trong buffer.
- frame drop count.
- average latency.

### Bước 7: Dọn tài nguyên đúng RAII

- Thread phải `join()` trước khi thoát.
- Window phải được `destroyAllWindows()`.
- Capture phải `release()` hoặc để destructor tự lo.
- Không dùng `new/delete` thủ công nếu không bắt buộc.

## Bài tập bắt buộc

1. Chạy với webcam và hiển thị FPS thực tế.
2. Chạy với 1 file video và so sánh `CAP_PROP_FPS` với FPS xử lý thực tế.
3. Cho phép cấu hình kích thước buffer bằng hằng số hoặc CLI.
4. In cảnh báo khi buffer đầy hoặc rỗng liên tục.
5. Thêm một bước xử lý nhẹ như grayscale hoặc blur để thấy tác động lên FPS.

## Bài tập nâng cao

1. Thêm chế độ `drop oldest` khi buffer đầy.
2. Ghi timestamp cho từng frame ngay khi capture.
3. Tạo struct `FramePacket` gồm `cv::Mat`, `frame_id`, `capture_time`.
4. So sánh 3 mode: không buffer, queue thường, circular buffer.

## Công nghệ và thư viện liên quan

- OpenCV đủ dùng cho Project 1, chưa cần GStreamer hay ROS 2.
- Nếu muốn debug leak trên macOS, có thể dùng Instruments hoặc AddressSanitizer.
- Nếu muốn kiểm tra race condition sau này, đọc thêm về ThreadSanitizer.

## Tiêu chí hoàn thành

- Ứng dụng chạy ổn định ít nhất vài phút không crash.
- FPS hiển thị đúng và cập nhật theo thời gian thực.
- Buffer có giới hạn kích thước rõ ràng.
- Khi thoát, không bị treo thread.
- Bạn giải thích được vì sao `condition_variable` cần thiết.

## Câu hỏi tự kiểm tra

- Vì sao `std::queue` động chưa phải lựa chọn tốt nhất cho realtime?
- Vì sao đọc camera và xử lý ảnh nên tách luồng?
- Frame có cần `clone()` không, và clone ở đâu là hợp lý?
- Khi buffer đầy, vì sao chính sách block khác với chính sách drop?

## Lỗi thường gặp

- Gọi `imshow` trước khi vẽ overlay.
- Chia sẻ cùng một `cv::Mat` giữa nhiều luồng mà không clone đúng lúc.
- Chạy vòng lặp chờ thủ công thay vì dùng `condition_variable`.
- Quên `join()` thread khi tắt chương trình.
- Đo FPS nhưng chỉ đo tốc độ hiển thị, không đo tốc độ capture.

## Sau project này bạn sẽ sẵn sàng làm gì

- Tối ưu tốc độ thật sự ở Project 2.
- Tách process hoặc truyền frame qua IPC ở Project 3.
- Hiểu nền tảng để sau này chuyển sang GStreamer hoặc ROS 2 mà không mơ hồ.
