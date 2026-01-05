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
    if ((current_time - rate_table[hash].last_refill) > 1000) {
      rate_table[hash].tokens = BURST_SIZE;
      rate_table[hash].last_refill = current_time;
    }

    if (rate_table[hash].tokens > 0) {
      rate_table[hash].tokens--;
      allowed = 1;
    }
  } else {
    rate_table[hash].ip = ip;
    rate_table[hash].tokens = BURST_SIZE - 1;
    rate_table[hash].last_refill = current_time;
    allowed = 1;
  }

  return allowed;
}
