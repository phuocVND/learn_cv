#pragma once
// Milestone 1: Binary frame packet protocol
// BẠN KHÔNG SỬA FILE NÀY — đây là contract đã định nghĩa trong SPEC.md
// Chỉ implement logic parsing ở network_manager.cpp

#include <cstdint>
#include <cstring>   // memcpy
#include <optional>
#include <span>

// ---------------------------------------------------------------------------
// Header layout: 24 bytes cố định, không padding
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct FramePacketHeader {
    uint32_t magic;           // offset 0  — phải bằng FRAME_MAGIC để accept
    uint32_t frame_id;        // offset 4  — monotonic counter từ nguồn
    uint64_t timestamp_us;    // offset 8  — microseconds since Unix epoch
    uint16_t width;           // offset 16
    uint16_t height;          // offset 18
    uint8_t  pixel_format;    // offset 20 — xem PixelFormat enum bên dưới
    uint8_t  flags;           // offset 21 — xem FrameFlags enum bên dưới
    uint16_t checksum;        // offset 22 — XOR16 của bytes [0..21]
};
#pragma pack(pop)

// Kiểm tra tại compile-time: struct phải đúng 24 bytes
static_assert(sizeof(FramePacketHeader) == 24,
    "FramePacketHeader phải đúng 24 bytes — kiểm tra #pragma pack");

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------
inline constexpr uint32_t FRAME_MAGIC = 0xCAFEF00D;

enum class PixelFormat : uint8_t {
    GRAY  = 0,   // 1 channel — payload_size = width * height
    BGR   = 1,   // 3 channel — payload_size = width * height * 3
    JPEG  = 2,   // compressed — dùng jpeg_size từ field phụ (ngoài scope M1)
    H264  = 3,   // NAL unit — advanced, ngoài scope roadmap này
};

enum FrameFlags : uint8_t {
    FLAG_KEYFRAME   = 0x01,  // bit 0: frame này là keyframe (with H264)
    FLAG_DROP_OK    = 0x02,  // bit 1: có thể drop frame này nếu buffer đầy
    FLAG_COMPRESSED = 0x04,  // bit 2: payload đã compress (JPEG/H264)
};

// ---------------------------------------------------------------------------
// Checksum helpers
// ---------------------------------------------------------------------------

// Tính XOR16 checksum: XOR tất cả byte pairs trong [data, data+len)
// HINT: len phải chẵn — với 22 bytes header bytes [0..21], len=22
inline uint16_t computeXOR16(const uint8_t* data, size_t len) {
    // TODO (Milestone 1): implement XOR16 checksum
    // - Loop qua từng cặp 2 bytes, cast thành uint16_t, XOR lại
    // - Handle odd len nếu cần (thêm 0x00 padding cuối)
    (void)data; (void)len;
    return 0; // placeholder
}

// Kiểm tra header có hợp lệ không (magic + checksum)
inline bool validateHeader(const FramePacketHeader& hdr) {
    // TODO (Milestone 1): implement validation
    // 1. Check hdr.magic == FRAME_MAGIC
    // 2. Compute checksum trên bytes [0..21] của hdr
    // 3. So sánh với hdr.checksum
    (void)hdr;
    return false; // placeholder
}

// Tính payload size từ header (chỉ hỗ trợ GRAY và BGR)
inline std::optional<size_t> payloadSize(const FramePacketHeader& hdr) {
    // TODO (Milestone 1): implement
    // - PixelFormat::GRAY  → width * height
    // - PixelFormat::BGR   → width * height * 3
    // - Các format khác → std::nullopt (chưa support)
    (void)hdr;
    return std::nullopt; // placeholder
}
