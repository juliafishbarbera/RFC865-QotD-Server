/**
 * Rate limiting implementation
 * MIT License (c)  2026 Julia Barbera <julia@fishcat.fish>
 */

#include "qotd.h"

rate_entry_t rate_table[TABLE_SIZE];

static uint64_t monotonic_time_ms() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
}

int rate_allow(uint32_t ip) {
  uint32_t hash = ip % TABLE_SIZE;
  uint64_t current_time = monotonic_time_ms();
  int allowed = 0;

  if (rate_table[hash].ip == ip) {
    uint64_t time_diff = current_time - rate_table[hash].last_refill;
    if (time_diff > 1000) {
      uint64_t seconds_passed = time_diff / 1000;
      uint64_t tokens_to_add = seconds_passed * tokens_per_second;
      rate_table[hash].tokens = (rate_table[hash].tokens + tokens_to_add > (uint64_t)burst_size) ? burst_size : rate_table[hash].tokens + tokens_to_add;
      rate_table[hash].last_refill = current_time;
    }

    if (rate_table[hash].tokens > 0) {
      rate_table[hash].tokens--;
      allowed = 1;
    }
  } else {
    rate_table[hash].ip = ip;
    rate_table[hash].tokens = burst_size - 1;
    rate_table[hash].last_refill = current_time;
    allowed = 1;
  }

  return allowed;
}
