
#include "EncoderRingBuffer.h"

EncoderRingBuffer::EncoderRingBuffer()
    : head_(0), tail_(0), overflow_count_(0) {}

bool EncoderRingBuffer::push(uint8_t value) {
    uint8_t next_head = (head_ + 1) % EncoderRingBuffer::BufferSize;
    if (next_head == tail_) {
        ++overflow_count_;
        return false;
    }
    buffer_[head_] = value;
    head_ = next_head;
    return true;
}

bool EncoderRingBuffer::pop(uint8_t& value) {
    if (tail_ == head_) return false;
    value = buffer_[tail_];
    tail_ = (tail_ + 1) % EncoderRingBuffer::BufferSize;
    return true;
}

bool EncoderRingBuffer::is_empty() const { return head_ == tail_; }

bool EncoderRingBuffer::is_full() const {
    return ((head_ + 1) % EncoderRingBuffer::BufferSize) == tail_;
}

uint32_t EncoderRingBuffer::overflow_count() const { return overflow_count_; }
