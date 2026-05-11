#pragma once
// Milestone 3: Processor — CV pipeline với WorkerPool
// BẠN IMPLEMENT: src/processor.cpp

#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <string>

// ---------------------------------------------------------------------------
// StageTiming — lưu thời gian xử lý của từng stage trong pipeline
// ---------------------------------------------------------------------------
struct StageTiming {
    double grayscale_ms = 0.0;
    double blur_ms      = 0.0;
    double canny_ms     = 0.0;
    double total_ms     = 0.0;
};

// ---------------------------------------------------------------------------
// WorkerPool — thread pool nhận task queue, xử lý song song
// ---------------------------------------------------------------------------
//
// Tại sao thread pool thay vì tạo thread mới mỗi frame?
// → Thread creation tốn ~10-100µs (kernel must allocate stack, TLS, etc.)
//   Với 30fps, đây là 300-3000µs/giây overhead thuần túy.
// → Pool pre-creates N threads và giữ chúng sống, task chỉ là function push vào queue.
//
// Design:
// - N worker threads luôn chạy, wait trên condition_variable khi queue trống
// - push_task(): push std::function vào queue, notify_one() để wake worker
// - Destructor: set stop_=true, notify_all(), join tất cả threads
//
class WorkerPool {
public:
    // Tạo pool với n_threads workers
    // Khuyến nghị: n_threads = std::thread::hardware_concurrency() - 1
    explicit WorkerPool(size_t n_threads);

    ~WorkerPool();

    // Push task vào queue — non-blocking
    // task sẽ được gọi bởi 1 worker thread nào đó
    void push_task(std::function<void()> task);

    // Số task đang chờ trong queue (để monitor queue depth)
    size_t pending() const;

private:
    // Vòng lặp mỗi worker thread chạy
    // TODO (Milestone 3): implement
    // - Loop: lock(mtx_), wait cv_ until (stop_ || !tasks_.empty())
    // - Nếu stop_ và tasks_ trống → return
    // - Lấy task từ queue, unlock, gọi task()
    void workerLoop();

    std::vector<std::thread>          threads_;
    std::queue<std::function<void()>> tasks_;
    mutable std::mutex                mtx_;
    std::condition_variable           cv_;
    std::atomic<bool>                 stop_{false};
};

// ---------------------------------------------------------------------------
// ProcessorConfig — tham số CV pipeline (đọc từ config.yaml ở Milestone 5)
// ---------------------------------------------------------------------------
struct ProcessorConfig {
    // GaussianBlur params
    int    blur_kernel_size = 5;   // phải lẻ: 3, 5, 7, ...
    double blur_sigma       = 1.5;

    // Canny params
    double canny_low        = 50.0;
    double canny_high       = 150.0;

    // Display mode: "grayscale", "blur", "canny", "bgr"
    std::string display_mode = "canny";
};

// Callback khi 1 frame được xử lý xong
// Tham số: frame kết quả (BGR 3-channel), timing metrics
using ProcessCallback = std::function<void(cv::Mat result, StageTiming timing)>;

// ---------------------------------------------------------------------------
// Processor — điều phối CV pipeline trên WorkerPool
// ---------------------------------------------------------------------------
class Processor {
public:
    explicit Processor(size_t n_workers, ProcessorConfig cfg = {});
    ~Processor();

    // Submit 1 frame để xử lý
    // frame: cv::Mat BGR input (sẽ được copy internally, caller có thể reuse)
    // THREAD SAFETY: safe gọi từ mọi thread
    void submit(cv::Mat frame);

    // Set callback nhận kết quả — gọi trước start()
    // THREAD SAFETY: callback chạy trên worker thread, không phải main thread
    void setOutputCallback(ProcessCallback cb);

    // Số frame đang queue (để monitor backpressure)
    size_t queueDepth() const;

private:
    // Xử lý 1 frame — gọi bởi worker thread
    // TODO (Milestone 3): implement
    // 1. Đo thời gian từng stage bằng steady_clock
    // 2. Grayscale: cv::cvtColor(input, gray, COLOR_BGR2GRAY)
    // 3. Blur:      cv::GaussianBlur(gray, blurred, Size(k,k), sigma)
    // 4. Canny:     cv::Canny(blurred, edges, low, high)
    // 5. Convert kết quả về BGR nếu display_mode != "bgr"
    // 6. Gọi output_cb_(result, timing)
    void processFrame(cv::Mat input);

    WorkerPool      pool_;
    ProcessorConfig cfg_;
    ProcessCallback output_cb_;
    std::atomic<size_t> queue_depth_{0};
};

// ---------------------------------------------------------------------------
// TODO Checklist Milestone 3:
// [ ] Implement WorkerPool constructor: khởi tạo N threads, mỗi thread chạy workerLoop()
// [ ] Implement WorkerPool::workerLoop(): wait-task-execute loop
// [ ] Implement WorkerPool destructor: stop_=true, notify_all(), join all threads
// [ ] Implement Processor::processFrame(): grayscale→blur→canny với timing
// [ ] Implement Processor::submit(): push_task([this, frame]{ processFrame(frame); })
// [ ] Test: submit 100 frames, verify callback được gọi 100 lần với timing hợp lý
// [ ] Verify: không có crash khi Processor destructor gọi khi còn task đang chạy
// ---------------------------------------------------------------------------
