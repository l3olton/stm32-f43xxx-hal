#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *buffer;
    size_t write_index;
    size_t read_index;
    size_t size;
} RingBuffer;

void ring_buf_init(RingBuffer *rb, uint8_t *buffer, size_t size);

bool ring_buf_push(RingBuffer *rb, uint8_t byte);

bool ring_buf_pop(RingBuffer *rb, uint8_t *output);

#endif
