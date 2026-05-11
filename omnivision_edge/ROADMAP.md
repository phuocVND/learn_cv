# OmniVision-Edge — Learning Roadmap

## Overview

```
Milestone 1 → Protocol + Network Transport
Milestone 2 → Memory Pool + Lock-free Ring Buffer
Milestone 3 → CV Processing Worker Pool
Milestone 4 → SIMD Optimization + Profiling
Milestone 5 → Config / Logging / Metrics / Docker
```

---

## Milestone 1: Binary Protocol & Network Transport

### Objective
Implement `NetworkManager` nhận frame từ ESP32-Cam (UDP) và laptop (TCP). Parse `FramePacketHeader` đúng, không mất bytes, không parse sai offset.

### The "Why" — Theory

**Struct padding problem:**
Compiler thêm bytes vào struct để căn chỉnh theo alignment tự nhiên. Ví dụ:
```
struct Bad { uint8_t a; uint32_t b; }; // sizeof = 8, không phải 5
struct Good { uint32_t b; uint8_t a; }; // sizeof = 5 nếu pack(1)
```
Nếu ESP32 gửi 24 bytes và server parse struct 28 bytes (vì padding) → decode sai hoàn toàn.

**Endianness:**
ESP32 là little-endian. x86/ARM64 đều little-endian. Tuy nhiên mạng (TCP/IP) theo convention dùng big-endian (network byte order). Bạn cần quyết định convention rõ ràng và dùng `htole32`/`le32toh` consistently.

**async_read vs read_some:**
`boost::asio::read_some` có thể trả về ít bytes hơn yêu cầu nếu socket buffer chưa đủ. Phải dùng `boost::asio::async_read` với `transfer_exactly(N)` để đảm bảo đọc đúng N bytes.

### Skeleton Code → Bạn implement `.cpp`

Xem file: `include/packet_protocol.hpp`, `include/network_manager.hpp`

### Self-Check Quiz

1. `#pragma pack(push, 1)` ảnh hưởng gì tới CPU alignment fault trên ARM? Trong điều kiện nào nó có thể gây crash thay vì chỉ chậm?
2. Nếu ESP32 gửi `frame_id` theo big-endian nhưng server đọc theo little-endian, kết quả `frame_id` sẽ sai thế nào? Viết ví dụ số cụ thể.
3. Tại sao `boost::asio::async_read` an toàn hơn `read_some` trong production? Điều gì xảy ra nếu TCP segment bị fragmented ở network layer?

---

## Milestone 2: Memory Pool & Lock-free Ring Buffer

### Objective
Implement `MatPool` (pre-allocated slots) và `SPSCRingBuffer<FrameHandle, N>`. Đảm bảo không có `new cv::Mat` trong hot path (sau init). Verify bằng Valgrind Massif.

### The "Why" — Theory

**Memory fragmentation:**
Heap allocator theo thời gian tạo ra các "lỗ hổng" nhỏ giữa các block đã free. Dù còn tổng memory, không thể cấp phát một block liên tục lớn → `std::bad_alloc` hoặc latency spike. Memory Pool tránh hoàn toàn vì chỉ dùng vùng pre-allocated.

**`std::atomic` memory ordering:**
- `memory_order_relaxed`: không đảm bảo thứ tự với các thao tác khác — chỉ dùng cho counter đơn giản.
- `memory_order_acquire`: đảm bảo mọi read sau đây thấy write từ thread khác trước đó.
- `memory_order_release`: đảm bảo mọi write trước đây visible cho thread khác sau khi acquire.
- SPSC ring chỉ cần `release` khi write head, `acquire` khi read head.

**Cache line false sharing:**
Nếu `head_` và `tail_` nằm cùng cache line (64 bytes), CPU phải invalidate cache line mỗi lần một trong hai thay đổi → "ping-pong" giữa cores. Dùng `alignas(64)` để tách chúng ra cache line riêng.

### Skeleton Code → Bạn implement `.cpp`

Xem file: `include/mat_pool.hpp`, `include/frame_buffer.hpp`

### Self-Check Quiz

1. `SPSCRingBuffer` chỉ an toàn với 1 producer và 1 consumer. Nếu bạn có 2 producer threads, cần thay đổi gì? Hint: research ABA problem.
2. Tại sao `N` của ring buffer phải là lũy thừa của 2? Thay vào đó có thể dùng modulo `% N` không, và chi phí là gì?
3. Sau khi `MatPool::release(mat*)`, thread khác gọi `acquire()` ngay lập tức — có race condition không? Tại sao?

---

## Milestone 3: CV Processing Worker Pool

### Objective
Implement `WorkerPool` với N threads, nhận `FrameHandle` từ `FrameBuffer`, xử lý (grayscale → blur → canny), push kết quả vào output queue. Display chỉ chạy trên main thread.

### The "Why" — Theory

**Frame-level vs tile-level parallelism:**
- Frame-level: mỗi worker xử lý 1 frame khác nhau. Đơn giản, ít đồng bộ, tốt cho throughput.
- Tile-level: chia 1 frame thành vùng nhỏ, xử lý song song. Giảm latency per-frame nhưng cần barrier đồng bộ ở cuối.
- Với streaming realtime (30fps), frame-level thường tốt hơn vì latency per-frame không critical bằng throughput tổng thể.

**Ordering problem:**
Worker pool xử lý song song → frame ra không theo thứ tự. Nếu cần ghi video, phải thêm `frame_id` và reorder buffer (priority queue theo `frame_id`). Nếu chỉ display realtime, drop ordering không quan trọng.

**Thread count tradeoff:**
N workers quá nhiều → context switch overhead > gain từ parallelism. Rule of thumb: N = logical_cores - 1 (giữ 1 core cho capture + display). Profile để xác định N tối ưu.

### Skeleton Code → Bạn implement `.cpp`

Xem file: `include/processor.hpp`

### Self-Check Quiz

1. Nếu worker xử lý frame 5ms và capture rate 30fps (33ms/frame), cần bao nhiêu worker để không tụt hậu? Tính toán cụ thể.
2. `std::function<void()>` trong queue có hidden heap allocation không? Thay bằng gì nếu muốn zero-alloc task queue?
3. Khi `WorkerPool` destructor gọi, tại sao phải notify_all trước khi join? Điều gì xảy ra nếu quên notify?

---

## Milestone 4: SIMD Optimization & Profiling

### Objective
Profile pipeline bằng `perf`/Instruments để xác định bottleneck. Bật OpenCV transparent API (UMat). Implement 1 kernel NEON thủ công (brightness adjustment) và so sánh với OpenCV built-in.

### The "Why" — Theory

**SIMD — Tại sao nhanh hơn loop scalar?**
CPU hiện đại có SIMD registers (NEON: 128-bit, AVX2: 256-bit). Một instruction xử lý 16 bytes (NEON uint8x16) thay vì 1 byte → throughput tăng lý thuyết 16x. Thực tế thấp hơn do overhead load/store và pipeline stall.

**OpenCV Transparent API:**
`cv::UMat` tự động chọn backend: CPU (SIMD), OpenCL, hoặc CUDA nếu available. Nhanh hơn loop tự viết mà không cần viết intrinsics thủ công. Dùng trước, chỉ xuống intrinsics nếu UMat vẫn không đủ.

**Profiling rule:**
Không tối ưu mà không đo. Flamegraph cho thấy chính xác % CPU time ở hàm nào. Nếu bottleneck là I/O (camera capture), SIMD không giúp gì.

**Memory alignment:**
SIMD load/store yêu cầu address align 16/32 bytes. `cv::Mat` allocate với `alignas(32)` đảm bảo điều này. Unaligned load dùng `vld1q_u8` vẫn hoạt động trên ARM nhưng có penalty.

### Skeleton Code → Bạn implement

Không có skeleton — nhiệm vụ của bạn là:
1. Viết benchmark harness riêng để đo scalar vs OpenCV vs NEON cho grayscale.
2. Dùng `std::chrono` đo p50/p95/p99 latency (không chỉ mean).
3. Chạy `perf stat` để xem instructions per cycle (IPC) của từng version.

### Self-Check Quiz

1. `vqaddq_u8` (saturated add) khác `vaddq_u8` (wrapping add) thế nào? Khi nào dùng cái nào cho image processing?
2. Compiler `-O3 -march=native` đôi khi tự vector hóa loop. Làm sao verify xem compiler đã auto-vectorize chưa mà không dùng profiler?
3. OpenCV `GaussianBlur` đã dùng SIMD bên trong. Vậy khi nào viết SIMD thủ công mới có ý nghĩa?

---

## Milestone 5: Config / Logging / Metrics / Docker

### Objective
Tách toàn bộ tham số ra `config.yaml`. Thêm `spdlog` async logging. Expose metrics (FPS, latency p50/p95, queue depth, drop count) ra log mỗi 5 giây. Đóng gói bằng Docker.

### The "Why" — Theory

**Production vs demo:**
Demo code hard-code `width=640, port=5000`. Production code đọc từ config → có thể đổi hành vi mà không rebuild. Không có config file = không thể vận hành bởi người khác.

**Async logging:**
`spdlog` sync mode: mỗi log call = ghi file = I/O wait → block thread xử lý. Async mode: log call push vào queue, background thread ghi → không block hot path. Flush explicit khi shutdown để không mất log.

**Metrics vs Logging:**
- Log: narrative ("frame dropped", "camera disconnected") — cho humans khi debug.
- Metrics: time-series numbers (FPS, latency) — cho dashboard, alerting, trend analysis.
- Không lẫn lộn: không dùng log để tính metrics, không dùng metrics để thay thế log.

### Skeleton Code → Bạn implement

Xem file: `include/app_controller.hpp`, `config/default.yaml`

### Self-Check Quiz

1. `spdlog` async mode dùng ring buffer nội bộ. Nếu ring đầy (log quá nhanh), hành vi mặc định là gì? Có thể thay đổi không?
2. Tại sao p95/p99 latency quan trọng hơn mean latency khi đánh giá realtime system?
3. Docker image cho OpenCV thường nặng 1-2GB. Làm sao giảm size? Hint: multi-stage build, strip symbols, chọn opencv module.
