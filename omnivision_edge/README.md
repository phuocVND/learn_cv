# OmniVision-Edge — Reading Guide

## Cách sử dụng package này

### Thứ tự đọc

1. **SPEC.md** — đọc trước để hiểu big picture, protocol, và memory strategy
2. **ROADMAP.md** — kế hoạch 5 milestone, mỗi milestone có "The Why" + self-check quiz
3. **include/*.hpp** — skeleton headers với TODO comments — đây là nơi bạn bắt đầu code
4. **CMakeLists.txt** — build config, uncomment dần khi bạn thêm .cpp files

### Quy trình làm việc mỗi milestone

```
Đọc ROADMAP.md#MilestoneN → Đọc include/X.hpp → Tạo src/X.cpp → Implement TODO → Build & test → Quiz
```

### Cấu trúc dự kiến khi hoàn thành

```
omnivision_edge/
├── SPEC.md                      ← System blueprint + protocol spec
├── ROADMAP.md                   ← 5 milestones với theory + quiz
├── CMakeLists.txt               ← Build config (uncomment src/ dần)
├── README.md                    ← File này
├── include/
│   ├── packet_protocol.hpp      ← Binary protocol (Milestone 1) — skeleton done
│   ├── network_manager.hpp      ← Boost.Asio UDP/TCP (Milestone 1) — skeleton done
│   ├── frame_buffer.hpp         ← MatPool + SPSCRingBuffer (Milestone 2) — skeleton done
│   ├── camera_interface.hpp     ← Camera abstraction (Milestone 1) — skeleton done
│   └── processor.hpp            ← WorkerPool + CV pipeline (Milestone 3) — skeleton done
└── src/                         ← BẠN TẠO — implement từng .cpp theo TODO
    ├── packet_protocol.cpp      ← Milestone 1
    ├── network_manager.cpp      ← Milestone 1
    ├── local_camera.cpp         ← Milestone 1
    ├── frame_buffer.cpp         ← Milestone 2
    ├── processor.cpp            ← Milestone 3
    └── main.cpp                 ← Wire everything together (Milestone 3+)
```

### Build từng milestone

```bash
# Tạo thư mục build
mkdir -p build && cd build

# Configure (Release mặc định)
cmake .. -DCMAKE_BUILD_TYPE=Debug   # Debug để dùng ASan/UBSan

# Build (sau khi uncomment executable trong CMakeLists.txt)
cmake --build . -j4
```

### Debug tools khuyến nghị

| Công cụ | Mục đích | Khi dùng |
|---|---|---|
| AddressSanitizer | Phát hiện buffer overflow, use-after-free | Luôn bật khi develop |
| ThreadSanitizer | Phát hiện data race | Khi test lock-free code (M2) |
| Valgrind Massif | Profiling heap allocation | Verify MatPool không leak (M2) |
| perf / Instruments | CPU profiling, flamegraph | Tìm bottleneck (M4) |
| Wireshark | Capture UDP/TCP packet | Debug protocol (M1) |

### Tips quan trọng

1. **Đọc "The Why" trước khi code** — Hiểu lý do tồn tại của mỗi design decision.
2. **Quiz là bắt buộc** — Không qua quiz = chưa thực sự hiểu.
3. **Không copy code từ AI** cho phần TODO — mục đích là bạn tự implement.
4. **Dùng ASan từ đầu** — lỗi memory dễ tìm hơn nhiều khi có tool hỗ trợ.
5. **Test từng component riêng lẻ** trước khi wire vào main().
