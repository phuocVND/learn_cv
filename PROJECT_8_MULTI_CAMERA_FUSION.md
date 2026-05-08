# PROJECT 8: Multi-Camera Fusion and Synchronization

**Mục tiêu:** Xây hệ thống nhiều camera có đồng bộ thời gian, calibration cơ bản, recording và hợp nhất dữ liệu đủ dùng cho perception stack nghiêm túc.

**Độ khó:** ⭐⭐⭐⭐⭐
**Thời gian:** 3-4 tuần
**Đầu ra tối thiểu:** một hệ 2 camera trở lên có timestamp rõ ràng, đồng bộ gần đúng, và pipeline xử lý/ghi lại dữ liệu.

## Vì sao project này quan trọng

Một camera thì đơn giản. Nhiều camera mới là nơi hệ CV thật sự phức tạp: lệch timestamp, trôi FPS, bandwidth tăng mạnh, calibration khó, storage nặng, debug cực khó nếu thiếu log và metadata.

## Bạn sẽ học gì

- Cách timestamp frame đúng từ lúc capture.
- Cách đồng bộ nhiều nguồn bằng exact hoặc approximate sync.
- Cách hiệu chuẩn cơ bản và lưu intrinsic/extrinsic.
- Cách thiết kế recorder cho nhiều stream cùng lúc.
- Cách đánh giá drift, dropped frame, out-of-sync.

## Công nghệ nên dùng

Bạn có thể chọn 1 trong 2 đường:

- OpenCV + custom pipeline + GStreamer.
- ROS 2 + `message_filters` + recorder.

Các thành phần hỗ trợ:

- OpenCV calibration tools.
- YAML/JSON lưu calibration.
- GStreamer hoặc FFmpeg path cho recording.
- ROS 2 nếu muốn topic orchestration.

## Bài toán bạn đang giải

Mục tiêu không chỉ là mở 2 camera cùng lúc. Mục tiêu là biết 2 frame này có thực sự thuộc cùng một thời điểm hay không, có trôi theo thời gian không, và làm sao ghi lại chúng để còn debug sau này.

## Kiến trúc mục tiêu

`cam0 + cam1 (+ cam2...) -> timestamp -> sync -> process/fuse -> record -> diagnostics`

## Các bước thực hiện

### Bước 1: Mở nhiều camera độc lập

- Mỗi camera có thread hoặc source riêng.
- Ghi rõ resolution, metadata FPS, timestamp capture.
- Lưu frame id cho từng camera.

### Bước 2: Chuẩn hóa timestamp

- Timestamp ngay khi frame đi vào hệ thống.
- Dùng cùng một clock source cho tất cả camera.
- Lưu timestamp cùng frame packet.

### Bước 3: Đồng bộ frame

Thử hai kiểu:

- `exact sync`: chỉ ghép khi timestamp khớp chặt.
- `approximate sync`: ghép frame gần nhau trong một khoảng tolerance.

**Khuyến nghị:** bắt đầu từ approximate sync.

### Bước 4: Calibration cơ bản

- Intrinsic cho từng camera.
- Nếu có 2 camera, thêm extrinsic giữa chúng.
- Lưu file calibration để pipeline dùng lại.

### Bước 5: Xây recorder nhiều luồng

Ghi tối thiểu:

- raw hoặc encoded stream
- timestamp
- frame id
- calibration metadata

### Bước 6: Fusion hoặc view ghép

Chọn một output dễ đánh giá:

- side-by-side synchronized display
- stitched preview đơn giản
- disparity hoặc depth toy example nếu là stereo setup

### Bước 7: Diagnostics

Đo:

- sync error trung bình
- max timestamp skew
- FPS mỗi camera
- dropped frame count mỗi nguồn
- recording throughput

## Bài tập bắt buộc

1. Chạy được tối thiểu 2 camera hoặc 2 video source giả lập.
2. Gắn timestamp và frame id cho từng frame.
3. Viết module ghép frame theo approximate sync.
4. Hiển thị side-by-side frame đồng bộ.
5. Ghi log skew theo thời gian.

## Bài tập nâng cao

1. Stereo calibration cơ bản.
2. Record cả raw stream và metadata vào cùng session.
3. Tạo viewer playback để kiểm tra sync sau khi record.
4. Thêm ROS 2 `message_filters` hoặc GStreamer multi-branch graph.

## Tiêu chí hoàn thành

- Hệ chạy được với hơn một nguồn hình ảnh.
- Có metric rõ về sync quality.
- Có recorder hoặc session log đủ để debug offline.
- Bạn giải thích được vì sao multi-camera khó hơn single-camera ở cả timing lẫn bandwidth.

## Lỗi thường gặp

- Lấy timestamp quá muộn, sau nhiều bước xử lý.
- Tin vào metadata FPS mà không đo drift thực tế.
- Đồng bộ theo frame index thay vì timestamp.
- Ghi video nhưng quên lưu metadata session.
