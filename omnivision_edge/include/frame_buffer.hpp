#pragma once
// Milestone 2: MatPool + SPSCRingBuffer
// BẠN IMPLEMENT: src/frame_buffer.cpp (hoặc header-only cho template)

#include <opencv2/core.hpp>
#include <atomic>
#include <vector>
#include <mutex>
#include <optional>
#include <cassert>

// ---------------------------------------------------------------------------
// MatPool — pre-allocated cv::Mat slots, O(1) acquire/release
// ---------------------------------------------------------------------------
//
// Tại sao không dùng new/delete trực tiếp?
// → Heap allocation trong hot path (30fps) gây fragmentation và latency spike.
// → Pool cấp phát trước N slot, acquire() chỉ là array index lookup.
//
// Design:
// - N slot cv::Mat pre-allocated khi khởi tạo
// - free_indices_ là stack của slot index còn trống
// - mutex bảo vệ free_indices_ (pool được chia sẻ bởi nhiều thread)
//
class MatPool {
public:
    // Khởi tạo pool với n_slots frame slots, mỗi slot kích thước (rows, cols, type)
    // VD: MatPool pool(8, 480, 640, CV_8UC3);
    MatPool(int n_slots, int rows, int cols, int type);

    // Lấy 1 slot từ pool — trả nullptr nếu pool cạn
    // CALLER phải gọi release() sau khi dùng xong
    // THREAD SAFETY: safe từ nhiều threads
    cv::Mat* acquire();

    // Trả slot về pool
    // PRECONDITION: ptr phải là pointer lấy từ acquire() của pool này
    // THREAD SAFETY: safe từ nhiều threads
    void release(cv::Mat* ptr);

    // Số slot hiện đang free (để debug/metrics)
    int freeCount() const;

private:
    // TODO (Milestone 2): implement
    // - slots_: vector<cv::Mat> với N phần tử pre-allocated
    // - free_indices_: vector<int> ban đầu = {0, 1, ..., N-1}
    // - base_ptr_: slots_.data() để tính index khi release
    // - mtx_: mutex bảo vệ free_indices_

    std::vector<cv::Mat> slots_;         // N pre-allocated Mats
    std::vector<int>     free_indices_;  // stack của index còn trống
    cv::Mat*             base_ptr_;      // = slots_.data()
    mutable std::mutex   mtx_;
};

// ---------------------------------------------------------------------------
// SPSCRingBuffer — lock-free single-producer single-consumer queue
// ---------------------------------------------------------------------------
//
// Tại sao lock-free?
// → SPSC pattern (1 producer + 1 consumer) có thể dùng atomic head/tail
//   mà không cần mutex → không có context switch, zero kernel involvement.
// → Chỉ safe với ĐÚNG 1 producer thread và ĐÚNG 1 consumer thread.
//
// N phải là lũy thừa 2 để dùng bitmask (head_ & (N-1)) thay vì modulo.
// → Modulo (%) là division instruction → ~20-40 cycles. Bitmask = 1 cycle.
//
// alignas(64): tách head_ và tail_ ra cache line riêng để tránh false sharing.
// → Nếu chúng cùng cache line, CPU phải invalidate cache line cho cả 2 thread.
//
template<typename T, size_t N>
class SPSCRingBuffer {
    static_assert((N & (N - 1)) == 0, "N phải là lũy thừa của 2");

public:
    SPSCRingBuffer() : head_(0), tail_(0) {}

    // Push 1 item — gọi từ producer thread
    // Trả false nếu buffer đầy (caller quyết định drop hay block)
    bool push(T item) {
        // TODO (Milestone 2): implement
        // 1. Đọc head_ với memory_order_relaxed (producer thread tự đọc head mình write)
        // 2. Tính next_head = (head + 1) & (N - 1)
        // 3. Nếu next_head == tail_.load(memory_order_acquire) → buffer đầy, return false
        // 4. buf_[head] = std::move(item)
        // 5. head_.store(next_head, memory_order_release)
        // 6. return true
        (void)item;
        return false; // placeholder
    }

    // Pop 1 item — gọi từ consumer thread
    // Trả nullopt nếu buffer trống
    std::optional<T> pop() {
        // TODO (Milestone 2): implement
        // 1. Đọc tail_ với memory_order_relaxed
        // 2. Nếu tail == head_.load(memory_order_acquire) → trống, return nullopt
        // 3. result = std::move(buf_[tail])
        // 4. tail_.store((tail + 1) & (N - 1), memory_order_release)
        // 5. return result
        return std::nullopt; // placeholder
    }

    // Số item hiện có trong buffer (approximate — chỉ dùng để monitor, không để sync)
    size_t size() const {
        size_t h = head_.load(std::memory_order_acquire);
        size_t t = tail_.load(std::memory_order_acquire);
        return (h - t + N) & (N - 1);
    }

    bool empty() const { return size() == 0; }
    bool full()  const { return size() == N - 1; }

private:
    // alignas(64): mỗi field nằm ở cache line riêng để tránh false sharing
    alignas(64) std::atomic<size_t> head_;
    alignas(64) std::atomic<size_t> tail_;
    T buf_[N];   // ring storage — T phải moveable
};

// ---------------------------------------------------------------------------
// TODO Checklist Milestone 2:
// [ ] Implement MatPool constructor: pre-allocate N slots bằng Mat::create()
// [ ] Implement MatPool::acquire(): lock, pop index, return &slots_[idx]
// [ ] Implement MatPool::release(): tính index từ ptr-base_ptr_, push về free_indices_
// [ ] Implement SPSCRingBuffer::push() với đúng memory_order
// [ ] Implement SPSCRingBuffer::pop() với đúng memory_order
// [ ] Viết test: 1 thread push 1000 items, 1 thread pop → verify không mất item
// [ ] Dùng Valgrind Helgrind hoặc ThreadSanitizer để verify no data race
// ---------------------------------------------------------------------------
