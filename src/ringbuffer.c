#include "ringbuffer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// TODO: move instance into usart.c
static RingBuffer ring_buffer = {0};

void ring_buf_init(uint8_t *buffer, size_t size)
{
    ring_buffer.buffer = buffer;
    // memset(rb->buffer, 0, size);
    ring_buffer.write_index = 0;
    ring_buffer.read_index = 0;
    ring_buffer.size = size;
}

static bool ring_buf_is_full(const RingBuffer *rb)
{
    return (rb->write_index + 1) % rb->size == rb->read_index;
}

static bool ring_buf_is_empty(const RingBuffer *rb)
{
    return rb->write_index == rb->read_index;
}

bool ring_buf_push(uint8_t byte)
{
    if (ring_buf_is_full(&ring_buffer)) return false;
    ring_buffer.buffer[ring_buffer.write_index] = byte;
    ring_buffer.write_index = (ring_buffer.write_index + 1) % ring_buffer.size;
    return true;
}

bool ring_buf_pop(uint8_t *output)
{
    if (ring_buf_is_empty(&ring_buffer)) return false;
    *output = ring_buffer.buffer[ring_buffer.read_index];
    ring_buffer.read_index = (ring_buffer.read_index + 1) % ring_buffer.size;
    return true;
}
