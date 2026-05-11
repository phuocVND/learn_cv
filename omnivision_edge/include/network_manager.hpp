#pragma once
// Milestone 1: NetworkManager — nhận FramePacket qua UDP và TCP
// BẠN IMPLEMENT: src/network_manager.cpp

#include "packet_protocol.hpp"
#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include <opencv2/core.hpp>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using udp = asio::ip::udp;

// ---------------------------------------------------------------------------
// FrameHandle: kết quả parse thành công của 1 packet
// ---------------------------------------------------------------------------
struct FrameHandle {
    FramePacketHeader header;
    cv::Mat           frame;   // frame data, borrow từ MatPool trong M2

    // Convenience accessors
    uint32_t frameId()      const { return header.frame_id; }
    uint64_t timestampUs()  const { return header.timestamp_us; }
    bool     isDropOk()     const { return header.flags & FLAG_DROP_OK; }
};

// Callback type: gọi khi parse xong 1 frame hợp lệ
using FrameCallback = std::function<void(FrameHandle)>;

// ---------------------------------------------------------------------------
// NetworkManager skeleton — Milestone 1
// ---------------------------------------------------------------------------
class NetworkManager {
public:
    explicit NetworkManager();
    ~NetworkManager();

    // Khởi động UDP listener (non-blocking, chạy trên io thread riêng)
    // source_port: server lắng nghe trên port này
    void startUDP(uint16_t source_port);

    // Khởi động TCP acceptor
    void startTCP(uint16_t listen_port);

    // Stop tất cả listener, join io thread
    void stop();

    // Set callback được gọi mỗi khi parse xong 1 frame hợp lệ
    // THREAD SAFETY: callback được gọi từ io thread, không phải main thread
    void setFrameCallback(FrameCallback cb);

private:
    // ---------------------------------------------------------------------------
    // Internal — bạn tự implement trong network_manager.cpp
    // ---------------------------------------------------------------------------

    // Bắt đầu async_receive_from loop cho UDP
    // HINT: Dùng boost::asio::async_receive_from với buffer recv_buf_
    void beginUDPReceive();

    // Handler được gọi khi UDP nhận đủ bytes
    // HINT: Kiểm tra ec, parse header, validate, đọc payload, gọi callback
    void onUDPReceive(const boost::system::error_code& ec, size_t bytes_received);

    // Bắt đầu async_accept loop cho TCP
    void beginTCPAccept();

    // Handler khi TCP connection mới đến
    void onTCPAccept(const boost::system::error_code& ec,
                     tcp::socket socket);

    // Parse buffer thô → FrameHandle nếu hợp lệ
    // HINT: Cast đầu N bytes thành FramePacketHeader, validate, rồi copy payload
    std::optional<FrameHandle> parsePacket(const uint8_t* data, size_t len);

    // ---------------------------------------------------------------------------
    // Members (bạn có thể thêm fields khác khi cần)
    // ---------------------------------------------------------------------------
    asio::io_context      ioc_;
    udp::socket           udp_sock_;
    tcp::acceptor         tcp_acc_;
    udp::endpoint         remote_ep_;       // UDP: endpoint của sender
    std::vector<uint8_t>  recv_buf_;        // UDP receive buffer (max frame size)
    FrameCallback         frame_cb_;        // callback khi parse xong
    std::thread           io_thread_;       // thread chạy ioc_.run()
    std::atomic<bool>     running_{false};
};

// ---------------------------------------------------------------------------
// TODO Checklist Milestone 1:
// [ ] Implement computeXOR16() và validateHeader() trong packet_protocol.hpp
// [ ] NetworkManager constructor: khởi tạo udp_sock_, tcp_acc_, recv_buf_(65536)
// [ ] startUDP(): bind socket, set running_=true, start io_thread_, beginUDPReceive()
// [ ] beginUDPReceive(): gọi udp_sock_.async_receive_from(...)
// [ ] onUDPReceive(): check ec, parse header từ recv_buf_, validate, copy payload, gọi frame_cb_
// [ ] parsePacket(): implement đầy đủ theo spec
// [ ] stop(): ioc_.stop(), io_thread_.join()
// [ ] Test: viết producer giả gửi UDP packet, verify callback nhận đúng
// ---------------------------------------------------------------------------
