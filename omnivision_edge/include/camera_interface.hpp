#pragma once
// Milestone 1: CameraInterface — abstract interface cho mọi nguồn frame
// Milestone 1: LocalCamera  — wrapper quanh cv::VideoCapture
// BẠN IMPLEMENT: src/local_camera.cpp, src/remote_camera.cpp

#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <string>
#include <optional>
#include <memory>

// ---------------------------------------------------------------------------
// CameraInterface — abstract base class
// ---------------------------------------------------------------------------
//
// Tại sao cần interface thay vì dùng trực tiếp cv::VideoCapture?
// → Dependency Inversion Principle: Processor không quan tâm nguồn frame là
//   camera thật, file video, hay remote stream — nó chỉ cần gọi read().
// → Dễ test: mock CameraInterface trả frame giả mà không cần camera thật.
// → Runtime polymorphism: AppController chọn LocalCamera hay RemoteCamera
//   dựa vào config, không cần if/else rải rác trong code xử lý.
//
class CameraInterface {
public:
    virtual ~CameraInterface() = default;

    // Mở camera/file/stream. source = device index ("0"), file path, RTSP URL
    // Trả false nếu không mở được
    virtual bool open(const std::string& source) = 0;

    // Đọc 1 frame. Trả nullopt nếu hết stream hoặc lỗi
    // CALLER không giữ Mat sau khi gọi open() hoặc read() lần tiếp theo
    virtual std::optional<cv::Mat> read() = 0;

    // Giải phóng tài nguyên
    virtual void release() = 0;

    // Metadata
    virtual double getFPS()          const = 0;
    virtual cv::Size getResolution() const = 0;
    virtual bool isOpened()          const = 0;

    // Factory method — tạo đúng subclass dựa vào source string
    // "rtsp://" hoặc "udp://" → RemoteCamera, còn lại → LocalCamera
    static std::unique_ptr<CameraInterface> create(const std::string& source);
};

// ---------------------------------------------------------------------------
// LocalCamera — đọc từ webcam hoặc file video
// ---------------------------------------------------------------------------
class LocalCamera : public CameraInterface {
public:
    bool open(const std::string& source) override;
    std::optional<cv::Mat> read() override;
    void release() override;
    double getFPS()          const override;
    cv::Size getResolution() const override;
    bool isOpened()          const override;

private:
    // TODO (Milestone 1): implement trong local_camera.cpp
    // - cap_.open(source) — source "0" → device 0, "video.mp4" → file
    // - cap_.read(frame_) — trả nullopt nếu !ret
    // - Dùng frame_ = cv::Mat() để reset giữa các lần read nếu cần

    cv::VideoCapture cap_;
    cv::Mat          frame_;  // reuse buffer để tránh alloc mỗi frame
};

// ---------------------------------------------------------------------------
// TODO Checklist Milestone 1 (camera):
// [ ] Implement LocalCamera::open(): cap_.open(atoi(source)) nếu source là số,
//     cap_.open(source) nếu là file/URL
// [ ] Implement LocalCamera::read(): cap_.read(frame_), return frame_ nếu !empty()
// [ ] Implement LocalCamera::getFPS(): cap_.get(cv::CAP_PROP_FPS)
// [ ] Implement LocalCamera::getResolution(): Size(cap_.get(CAP_PROP_FRAME_WIDTH), ...)
// [ ] Implement CameraInterface::create() factory (milestone 1 basic: chỉ trả LocalCamera)
// [ ] Test: open("0"), đọc 100 frames, verify không crash, FPS > 0
// ---------------------------------------------------------------------------
