
#pragma once
#include <atomic>
#include <cstddef>  // Add this line
#include <cstdint>

class EncoderRingBuffer {
    public:
    static constexpr size_t BufferSize = 64;

    EncoderRingBuffer();

    bool push(uint8_t value);
    bool pop(uint8_t& value);

    bool is_empty() const;
    bool is_full() const;

    uint32_t overflow_count() const;

    private:
    volatile uint8_t buffer_[BufferSize];
    volatile uint8_t head_;
    volatile uint8_t tail_;
    std::atomic<uint32_t> overflow_count_;
};
