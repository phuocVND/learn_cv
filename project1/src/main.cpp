#include <boost/circular_buffer.hpp>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>
#include <iostream>
#include <chrono>
#include <atomic>

std::atomic<bool> running{true};
boost::circular_buffer<cv::Mat> buffer(30); // buffer 60 frame
std::mutex mtx;
std::condition_variable frame_cv;

void captureFrames()
{
    cv::VideoCapture cap(0); // Open the default camera
    cv::Mat frame;
    if (!cap.isOpened()) return;
    while (running) {
        cap >> frame; // Capture a new frame
        if (frame.empty()) continue;
        {
            std::lock_guard<std::mutex> lock(mtx);
            buffer.push_back(std::move(frame)); // Add frame to buffer
        }
        frame_cv.notify_one(); // Báo cho consumer có frame mới
    }
}

void displayFrames()
{
    cv::Mat frame;
    int disp_count = 0;
    auto last_disp = std::chrono::steady_clock::now();
    int display_fps = 0;

    while (running) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            frame_cv.wait(lock, [] { return !buffer.empty() || !running; });
            if (!running && buffer.empty()) break;
            frame = buffer.front();
            buffer.pop_front();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // Giả lập xử lý nặng
        // cập nhật counter và tính FPS mỗi giây
        ++disp_count;
        auto now = std::chrono::steady_clock::now();
        if (now - last_disp >= std::chrono::seconds(1)) {
            display_fps = disp_count;
            disp_count = 0;
            last_disp = now;
        }

        // vẽ FPS lên frame
        std::string txt = "DISPLAY FPS: " + std::to_string(display_fps);
        cv::putText(frame, txt, cv::Point(10, 20),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);

        cv::imshow("Camera Feed", frame);
        if (cv::waitKey(1) >= 0) break;
    }

    running = false;
    frame_cv.notify_all();
}

int main() {
    std::thread producer(captureFrames);
    displayFrames();            // chạy trên main (imshow/WaitKey an toàn)
    running = false;            // khi displayFrames() return, yêu cầu dừng producer
    producer.join();
    cv::destroyAllWindows();
    return 0;
}
