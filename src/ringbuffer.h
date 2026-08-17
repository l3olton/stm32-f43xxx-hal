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

void ring_buf_init(uint8_t *buffer, size_t size);

bool ring_buf_push(uint8_t byte);

bool ring_buf_pop(uint8_t *output);

#endif
