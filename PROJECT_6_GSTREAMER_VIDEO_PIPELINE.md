# PROJECT 6: GStreamer Video Pipeline

**Mục tiêu:** Học cách dùng GStreamer để xây media pipeline chuyên nghiệp, low-latency và dễ mở rộng hơn OpenCV `VideoCapture` thuần.

**Độ khó:** ⭐⭐⭐
**Thời gian:** 2-3 tuần
**Đầu ra tối thiểu:** một pipeline GStreamer có thể nhận camera/video, decode, convert, hiển thị, record hoặc đưa frame sang OpenCV.

## Vì sao project này quan trọng

OpenCV rất tốt để xử lý ảnh, nhưng phần ingest/decode/stream media của nó không phải lúc nào cũng đủ linh hoạt. GStreamer là framework chuẩn công nghiệp cho media graph: source, decode, convert, queue, encode, stream, sink.

## Bạn sẽ học gì

- Khái niệm `element`, `pad`, `caps`, `pipeline`, `bin`.
- Cách prototype bằng `gst-launch-1.0` trước khi code C++.
- Cách dùng `appsink` để lấy frame sang OpenCV.
- Cách dùng `appsrc` để đẩy frame từ OpenCV ngược lại pipeline.
- Cách tối ưu low-latency bằng `queue`, `sync=false`, `drop=true`, `max-buffers`.

## Công nghệ nên dùng

- GStreamer core.
- Plugins `base`, `good`, `bad` nếu cần.
- OpenCV cho phần xử lý ảnh.
- C++17.
- Tùy chọn: RTSP hoặc RTP test stream.

## Bài toán bạn đang giải

Bạn cần hiểu media path đúng nghĩa:

`source -> decode -> convert -> queue -> appsink -> OpenCV -> appsrc -> encode -> sink`

Project này giúp bạn biết lúc nào nên dùng OpenCV thuần và lúc nào nên giao phần media cho GStreamer.

## Các bước thực hiện

### Bước 1: Làm quen với `gst-launch`

Thử các lệnh kiểu:

- webcam -> autovideosink
- file -> decodebin -> videoconvert -> autovideosink
- file -> tee -> display + fakesink

**Mục đích:** hiểu graph trước khi viết code.

### Bước 2: Đọc từ camera bằng GStreamer

Pipeline ví dụ về mặt ý tưởng:

`source -> videoconvert -> queue -> appsink`

Sau đó trong code C++:

- kéo frame từ `appsink`
- map buffer sang dữ liệu ảnh
- chuyển thành `cv::Mat`

### Bước 3: Xử lý ảnh bằng OpenCV

- grayscale
- blur
- canny
- overlay text

Sau đó hiển thị bằng OpenCV hoặc đẩy ngược về GStreamer.

### Bước 4: Đẩy frame ngược lại pipeline bằng `appsrc`

Graph ví dụ:

`appsrc -> videoconvert -> encoder -> filesink`

Mục tiêu là ghi video output hoặc stream ra network.

### Bước 5: Thêm branch bằng `tee`

Ví dụ:

- branch 1: display
- branch 2: record file
- branch 3: network stream

**Đây là điểm GStreamer mạnh hơn OpenCV thuần rất nhiều.**

### Bước 6: Tối ưu low-latency

Bạn nên thử các cấu hình:

- `queue`
- `max-size-buffers`
- `drop=true`
- `sync=false`
- `appsink max-buffers=1 drop=true`

So sánh latency khi bật/tắt từng cấu hình.

### Bước 7: Benchmark và phân tích

Đo tối thiểu:

- ingest FPS
- appsink to OpenCV processing time
- appsrc push time
- end-to-end latency

## Bài tập bắt buộc

1. Prototype ít nhất 3 pipeline bằng `gst-launch`.
2. Viết 1 app C++ lấy frame từ `appsink` sang OpenCV.
3. Ghi output processed video bằng `appsrc`.
4. Tạo pipeline có `tee` để vừa display vừa record.
5. So sánh OpenCV-only ingest với GStreamer ingest.

## Bài tập nâng cao

1. Nhận RTSP stream và đưa vào OpenCV.
2. Stream kết quả ra RTP/UDP.
3. Thử hardware decode nếu máy hỗ trợ.
4. Thêm dynamic pipeline reconfiguration.

## Khi nào nên dùng GStreamer

- Nguồn video phức tạp.
- Cần nhiều nhánh media cùng lúc.
- Cần encode/decode/stream chuẩn low-latency.
- Muốn tích hợp camera/codec/network tốt hơn `VideoCapture` thuần.

## Tiêu chí hoàn thành

- Bạn hiểu rõ vai trò của `appsink` và `appsrc`.
- Bạn build được một media graph có ít nhất 2 branch.
- Có số liệu latency/FPS rõ ràng.
- Bạn giải thích được vì sao GStreamer mạnh hơn OpenCV ở phần media orchestration.

## Lỗi thường gặp

- Code C++ trước khi hiểu pipeline `gst-launch`.
- Không kiểm soát `caps`, dẫn đến format mismatch.
- Quên xử lý ownership khi map buffer sang `cv::Mat`.
- Không dùng `queue` ở pipeline có nhiều nhánh.
