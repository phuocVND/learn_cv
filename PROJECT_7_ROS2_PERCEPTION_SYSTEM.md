# PROJECT 7: ROS 2 Perception System

**Mục tiêu:** Biến pipeline CV của bạn thành một hệ perception dạng robotics với `node`, `topic`, `QoS`, `launch file` và luồng dữ liệu rõ ràng giữa nhiều module.

**Độ khó:** ⭐⭐⭐⭐
**Thời gian:** 2-3 tuần
**Đầu ra tối thiểu:** một workspace ROS 2 có camera publisher, processing node, inference node và visualization node.

## Vì sao project này quan trọng

ROS 2 không thay OpenCV hay GStreamer. Nó giải quyết bài toán orchestration giữa nhiều sensor, nhiều node và nhiều máy trong robotics/perception stack. Nếu bạn muốn làm robot, autonomous system, smart camera network hoặc sensor fusion, ROS 2 gần như là ngôn ngữ chung.

## Bạn sẽ học gì

- Cách tổ chức workspace, package, node trong ROS 2.
- Cách publish/subscribe ảnh bằng `sensor_msgs/Image`.
- Cách dùng `cv_bridge` và `image_transport`.
- Cách chọn `QoS` phù hợp cho video stream.
- Cách tách node capture, preprocess, inference, visualization.

## Công nghệ nên dùng

- ROS 2 Humble hoặc Jazzy.
- `rclcpp`.
- `sensor_msgs`, `std_msgs`.
- `cv_bridge`.
- `image_transport`.
- OpenCV.
- Tùy chọn: RViz2.

## Bài toán bạn đang giải

Khi hệ thống có nhiều sensor và nhiều module, architecture kiểu một executable lớn trở nên khó kiểm soát. ROS 2 cho bạn chuẩn message, scheduler, topic, launch, QoS để tách hệ thống thành node.

## Kiến trúc mục tiêu

`camera_node -> preprocess_node -> inference_node -> overlay_node -> visualization`

Có thể thêm:

- `recorder_node`
- `diagnostics_node`
- `parameter_server`

## Các bước thực hiện

### Bước 1: Tạo ROS 2 workspace

- Tạo `colcon` workspace.
- Tạo package C++ cho perception.
- Build và chạy node mẫu kiểu hello world.

### Bước 2: Viết `camera_node`

- Publish frame thành `sensor_msgs/Image`.
- Publish thêm timestamp và frame id.
- Cho phép chọn source camera hoặc video file.

### Bước 3: Viết `preprocess_node`

- Subscribe topic ảnh.
- Convert sang `cv::Mat` bằng `cv_bridge`.
- Xử lý nhẹ như resize/grayscale/blur.
- Publish ảnh đã xử lý ra topic khác.

### Bước 4: Viết `inference_node`

- Subscribe ảnh đã preprocess.
- Chạy model từ Project 4.
- Publish detection result hoặc ảnh overlay.

### Bước 5: Cấu hình QoS

So sánh:

- `best_effort`
- `reliable`
- depth queue khác nhau

**Mục đích:** hiểu vì sao video stream không phải lúc nào cũng nên `reliable`.

### Bước 6: Tạo launch file

- Start nhiều node cùng lúc.
- Truyền parameter như source path, topic name, model path.
- Chạy một lệnh là lên cả stack.

### Bước 7: Debug bằng ROS tools

- `ros2 topic list`
- `ros2 topic hz`
- `ros2 topic echo`
- `rqt_graph`
- RViz2 nếu cần visualization

## Bài tập bắt buộc

1. Tạo tối thiểu 3 node riêng.
2. Truyền ảnh giữa các node bằng ROS 2 topic.
3. Dùng `cv_bridge` đúng cách.
4. Thêm launch file để khởi động toàn bộ pipeline.
5. Đo tần số topic và so sánh QoS.

## Bài tập nâng cao

1. Dùng `image_transport` để thử compressed image.
2. Thêm dynamic parameter cho threshold hoặc model path.
3. Viết diagnostics node báo FPS, latency, queue depth.
4. Tách node inference ra máy khác trong cùng ROS domain.

## Khi nào nên dùng ROS 2

- Nhiều node, nhiều sensor, nhiều module cần phối hợp.
- Làm robotics, AMR, drone, autonomous perception.
- Cần chuẩn message và tooling hệ sinh thái.

## Tiêu chí hoàn thành

- Workspace ROS 2 build và launch được.
- Ảnh đi qua ít nhất 3 node.
- Có inference hoặc processing thật chứ không chỉ relay topic.
- Bạn giải thích được vai trò của `QoS`, `cv_bridge`, `launch`.

## Lỗi thường gặp

- Dùng QoS không phù hợp với stream video.
- Convert ảnh qua `cv_bridge` sai encoding.
- Nhồi quá nhiều logic vào một node.
- Không timestamp frame ngay từ source.
