# Computer Vision Learning Path

Lộ trình này được mở rộng thành 9 project để bạn đi từ nền tảng C++/OpenCV tới hệ thống CV hoàn chỉnh có streaming, inference, ROS 2, GStreamer và production mindset.

## Cách dùng bộ tài liệu này

- Học theo thứ tự từ Project 1 đến Project 9.
- Mỗi project đều có mục tiêu kỹ thuật, công nghệ nên dùng, bài tập bắt buộc, tiêu chí hoàn thành và hướng mở rộng.
- Không cần làm hoàn hảo ngay từ đầu. Mục tiêu là mỗi project giải quyết đúng một lớp vấn đề mới.
- Nếu đang code song song trong thư mục `project1/`, hãy dùng Project 1 làm chuẩn, sau đó tách ra thành `project2/`, `project3/`... khi cần.

## Tổng quan 9 project

| Project | Chủ đề | Trọng tâm | Công nghệ chính | Kết quả đầu ra |
| --- | --- | --- | --- | --- |
| 1 | Basic Streaming Pipeline | producer/consumer, circular buffer, FPS | C++17, OpenCV, STL threading | pipeline đọc camera/video ổn định |
| 2 | SIMD and Realtime Processor | profiling, vectorization, thread pool | OpenCV, NEON/AVX2, benchmark | bản xử lý ảnh realtime tối ưu hơn |
| 3 | Network Frame Server | IPC, shared memory, TCP/Unix socket | POSIX IPC, socket, OpenCV | capture/process tách process |
| 4 | AI Inference Pipeline | preprocess, inference, postprocess | OpenCV DNN, ONNX Runtime hoặc NCNN | pipeline có AI chạy realtime |
| 5 | Production System | config, logging, monitoring, Docker | spdlog, yaml/json config, Docker | hệ thống dễ vận hành và debug |
| 6 | GStreamer Video Pipeline | decode graph, appsink/appsrc, low latency | GStreamer, OpenCV | hiểu pipeline media chuẩn công nghiệp |
| 7 | ROS 2 Perception System | node, topic, QoS, image transport | ROS 2, rclcpp, cv_bridge | perception stack dạng robot |
| 8 | Multi-Camera Fusion | đồng bộ nhiều camera, calibration, recording | OpenCV, ROS 2 hoặc GStreamer | hệ nhiều camera ổn định |
| 9 | End-to-End Vision Platform | ghép toàn bộ thành capstone | toàn bộ stack trên | demo hệ CV gần production |

## Nên học theo nhịp nào

- Tuần 1-2: Project 1
- Tuần 3-4: Project 2
- Tuần 5-7: Project 3 và 4
- Tuần 8-9: Project 5
- Tuần 10-11: Project 6
- Tuần 12-13: Project 7
- Tuần 14-15: Project 8
- Tuần 16+: Project 9

## Khi nào được coi là xong cả lộ trình

- Bạn tự build được pipeline capture -> buffer -> process -> inference -> output.
- Bạn hiểu vì sao phải giới hạn buffer, đo FPS, đo latency và log metrics.
- Bạn biết lúc nào nên dùng OpenCV thuần, lúc nào nên thêm GStreamer, lúc nào nên chuyển sang ROS 2.
- Bạn có ít nhất 1 capstone có video demo, README, config và cách chạy rõ ràng.

## Gợi ý chọn stack theo mục tiêu

- Nếu mục tiêu là học nền tảng CV C++: ưu tiên Project 1, 2, 4.
- Nếu mục tiêu là hệ thống camera realtime: ưu tiên Project 1, 3, 6, 8.
- Nếu mục tiêu là robotics/perception: ưu tiên Project 1, 4, 7, 8, 9.
- Nếu mục tiêu là deploy như sản phẩm: ưu tiên Project 5 và 9.

## Thứ tự đọc file

1. `PROJECT_1_BASIC_PIPELINE.md`
2. `PROJECT_2_SIMD_PROCESSOR.md`
3. `PROJECT_3_NETWORK_SERVER.md`
4. `PROJECT_4_INFERENCE_PIPELINE.md`
5. `PROJECT_5_PRODUCTION_SYSTEM.md`
6. `PROJECT_6_GSTREAMER_VIDEO_PIPELINE.md`
7. `PROJECT_7_ROS2_PERCEPTION_SYSTEM.md`
8. `PROJECT_8_MULTI_CAMERA_FUSION.md`
9. `PROJECT_9_END_TO_END_VISION_PLATFORM.md`

## Ghi nhớ

- OpenCV là điểm khởi đầu rất tốt, nhưng không phải framework duy nhất bạn cần biết.
- GStreamer mạnh ở media graph và low-latency streaming.
- ROS 2 mạnh ở orchestration nhiều node, sensor, robot middleware.
- Production không chỉ là code chạy được, mà là code đo được, log được, restart được và đóng gói được.
