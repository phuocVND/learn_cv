# PROJECT 2: Multi-threaded SIMD Processor

**Mục tiêu:** Tăng tốc pipeline ở Project 1 bằng profiling, chia việc hợp lý, tận dụng vectorization và giảm copy không cần thiết.

**Độ khó:** ⭐⭐
**Thời gian:** 2-3 tuần
**Đầu ra tối thiểu:** một pipeline xử lý ảnh có benchmark trước/sau tối ưu và biểu đồ hoặc log số liệu rõ ràng.

## Bạn sẽ học gì

- Cách xác định `bottleneck` thay vì tối ưu theo cảm giác.
- Khác nhau giữa tối ưu thuật toán, tối ưu memory access và tối ưu instruction level.
- SIMD là gì, vì sao NEON/AVX2 nhanh hơn loop scalar trong một số tác vụ.
- Khi nào nên xử lý theo frame-level parallelism, khi nào theo tile-level parallelism.
- Cách xây dựng thread pool hoặc worker pool đơn giản.

## Công nghệ nên dùng

- C++17.
- OpenCV `imgproc`.
- `std::chrono` hoặc benchmark tool đơn giản.
- Thread pool tự viết hoặc `std::async` phiên bản đầu.
- Tùy chọn:
  - Apple Silicon: NEON.
  - x86_64 Linux/Windows: SSE/AVX2.

## Bài toán bạn đang giải

Pipeline đã chạy được nhưng vẫn chậm khi thêm bước xử lý như blur, Sobel, Canny, resize, color conversion hoặc overlay. Bạn cần biết tối ưu chỗ nào trước, và tối ưu bằng công cụ gì.

## Hướng kỹ thuật nên bám

- Tối ưu sau khi có số đo baseline.
- Ưu tiên giảm copy và giảm cấp phát trước khi viết intrinsics.
- Dùng OpenCV optimized path trước khi tự viết SIMD tay.
- Chỉ thêm nhiều luồng khi chắc rằng không bị nghẽn bởi lock hoặc memory bandwidth.

## Các bước thực hiện

### Bước 1: Khóa baseline

- Lấy pipeline Project 1 làm nền.
- Chọn 2-3 tác vụ xử lý ảnh cụ thể, ví dụ:
  - grayscale
  - gaussian blur
  - canny edge
- Đo:
  - thời gian capture
  - thời gian preprocess
  - thời gian display
  - tổng FPS

### Bước 2: Tách pipeline thành stage rõ ràng

Ví dụ:

`capture -> preprocess -> heavy filter -> overlay -> display`

Mỗi stage phải có timer riêng để biết bottleneck nằm ở đâu.

### Bước 3: Tối ưu memory trước

- Reuse `cv::Mat` output buffer nếu có thể.
- Tránh tạo object mới trong mỗi frame nếu không cần.
- Hạn chế `clone()` dư thừa.
- So sánh `copy`, `move`, `reference`, `pre-allocated buffer`.

### Bước 4: Tối ưu bằng OpenCV trước

- Dùng các hàm chuẩn của OpenCV thay vì loop pixel thủ công ở phiên bản đầu.
- Kiểm tra xem bật optimization build (`Release`) chưa.
- So sánh build `Debug` và `Release` để thấy chênh lệch.

### Bước 5: Thêm thread pool hoặc worker pool

Có 2 hướng:

- **Frame parallelism:** mỗi worker xử lý một frame khác nhau.
- **Tile parallelism:** chia cùng một frame thành nhiều vùng và xử lý song song.

**Khuyến nghị:** với video streaming, bắt đầu từ frame parallelism dễ hơn.

### Bước 6: Viết một kernel tự tối ưu

Chọn một tác vụ nhỏ để thử viết tay, ví dụ:

- brightness/contrast adjustment
- threshold
- grayscale conversion
- alpha blending

Làm 3 phiên bản:

- scalar
- OpenCV built-in
- SIMD hoặc parallelized

### Bước 7: Benchmark và phân tích

Bạn phải có bảng số liệu kiểu này:

| Phiên bản | Độ phân giải | FPS | CPU usage | Ghi chú |
| --- | --- | --- | --- | --- |
| baseline | 1280x720 | ... | ... | 1 thread |
| optimized OpenCV | 1280x720 | ... | ... | Release |
| pool + optimized | 1280x720 | ... | ... | 2-4 worker |
| SIMD thử nghiệm | 1280x720 | ... | ... | kernel riêng |

## Bài tập bắt buộc

1. Thêm ít nhất 2 bước xử lý ảnh realtime vào pipeline.
2. Đo tốc độ trước và sau khi tối ưu.
3. Dùng `Release` build và ghi rõ chênh lệch với `Debug`.
4. Thử song song hóa ít nhất một phần pipeline.
5. Viết báo cáo ngắn: bottleneck đầu tiên là gì, bạn xử lý nó ra sao.

## Bài tập nâng cao

1. Viết một kernel SIMD cơ bản cho Apple Silicon bằng NEON hoặc x86_64 bằng AVX2.
2. So sánh lock-based queue với lock-free hoặc double buffering.
3. Thêm CLI để đổi filter runtime.
4. Dùng heatmap thời gian xử lý hoặc rolling average latency.

## Nên dùng công nghệ gì

- Nếu muốn đi nhanh: OpenCV + thread pool đơn giản.
- Nếu muốn học tối ưu phần cứng: intrinsics NEON/AVX2.
- Nếu muốn benchmark bài bản hơn: thêm Google Benchmark hoặc custom harness.

## Tiêu chí hoàn thành

- Có bảng benchmark trước/sau tối ưu.
- Giải thích được bottleneck lớn nhất nằm ở stage nào.
- Có ít nhất một thay đổi giúp FPS tăng rõ rệt.
- Không làm pipeline kém ổn định hơn Project 1.

## Câu hỏi tự kiểm tra

- Tại sao tối ưu memory allocation nhiều khi hiệu quả hơn viết SIMD sớm?
- Vì sao nhiều thread có thể làm chậm hơn nếu lock quá nhiều?
- Khi nào tile-based processing hợp lý hơn frame-based processing?
- Tại sao phải luôn benchmark bằng `Release` build?

## Lỗi thường gặp

- So benchmark ở `Debug` rồi kết luận sai.
- Dùng quá nhiều lock giữa các stage.
- Viết intrinsics sớm khi chưa biết bottleneck.
- Đo FPS trung bình nhưng không đo latency peak.

## Sau project này bạn sẽ sẵn sàng làm gì

- Đưa frame sang process khác ở Project 3.
- Gắn inference vào pipeline ở Project 4.
- Hiểu rõ khái niệm low-latency trước khi sang GStreamer ở Project 6.
