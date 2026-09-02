#include "ringbuffer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void ring_buf_init(RingBuffer *rb, uint8_t *buffer, const size_t size)
{
    rb->buffer = buffer;
    rb->write_index = 0;
    rb->read_index = 0;
    rb->size = size;
}

static bool ring_buf_is_full(const RingBuffer *rb)
{
    return (rb->write_index + 1) % rb->size == rb->read_index;
}

static bool ring_buf_is_empty(const RingBuffer *rb)
{
    return rb->write_index == rb->read_index;
}

bool ring_buf_push(RingBuffer *rb, const uint8_t byte)
{
    if (ring_buf_is_full(rb)) return false;
    rb->buffer[rb->write_index] = byte;
    rb->write_index = (rb->write_index + 1) % rb->size;
    return true;
}

bool ring_buf_pop(RingBuffer *rb, uint8_t *output)
{
    if (ring_buf_is_empty(rb)) return false;
    *output = rb->buffer[rb->read_index];
    rb->read_index = (rb->read_index + 1) % rb->size;
    return true;
}

void ring_buf_reset(RingBuffer *rb)
{
    rb->write_index = 0;
    rb->read_index = 0;
}