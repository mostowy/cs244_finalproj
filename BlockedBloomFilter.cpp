#include "BlockedBloomFilter.h"
#include <cstring>

BlockedBloomFilter::BlockedBloomFilter(size_t unused_size_in_bits,
                                       std::shared_ptr<HashFamily> family,
                                       int num_hash_funcs)
    : bits_per_bloom_filter_(CACHE_LINE_BYTES * 8),
      total_size_in_bits_(bits_per_bloom_filter_ * NUM_BLOOM_FILTERS),
      num_inserted_(0) {
  memset(bloom_filter_array_, 0, total_size_in_bits_ / 8);
  for (int i = 0; i < num_hash_funcs; i++) {
    hash_funcs_.push_back(family->get());
  }
}

BlockedBloomFilter::~BlockedBloomFilter() {
}

bool BlockedBloomFilter::get(size_t bloom_filter_index, size_t bit_index)
    const {
  const size_t byte_index = bit_index / 8;
  const unsigned char offset = bit_index % 8;
  return (bloom_filter_array_[bloom_filter_index][byte_index] & (1 << offset))
      > 0;
}

void BlockedBloomFilter::set(size_t bloom_filter_index, size_t bit_index,
    bool value) {
  const size_t byte_index = bit_index / 8;
  const unsigned char offset = bit_index % 8;
  const unsigned char mask = 1 << offset;
  if (value) {
    bloom_filter_array_[bloom_filter_index][byte_index] |= mask;
  } else {
    bloom_filter_array_[bloom_filter_index][byte_index] &= ~mask;
  }
}

size_t BlockedBloomFilter::get_filter_index_for(int data) const {
  return hash_funcs_[0](data) % NUM_BLOOM_FILTERS;
}

int BlockedBloomFilter::insert(int data) {
  // Arbitrary "fullness" threshold, done just for the testing infrastructure.
  // Bloom filters don't really reach fullness.
  if (num_inserted_ >= total_size_in_bits_ / hash_funcs_.size() * 2) return -1;

  const size_t filter_index = get_filter_index_for(data);
  for (auto const& hash : hash_funcs_) {
    const size_t bit_index = hash(data) % bits_per_bloom_filter_;
    set(filter_index, bit_index, true);
  }
  num_inserted_++;
  return 1;
}

bool BlockedBloomFilter::contains(int data) const {
  const size_t filter_index = get_filter_index_for(data);
  for (auto const& hash : hash_funcs_) {
    const size_t bit_index = hash(data) % bits_per_bloom_filter_;
    if (!get(filter_index, bit_index)) {
      return false;
    }
  }
  return true;
}
