#include "BlockedBloomFilter.h"
#include <cstring>
#include <stdlib.h>

BlockedBloomFilter::BlockedBloomFilter(size_t num_bloom_filters,
                                       std::shared_ptr<HashFamily> family,
                                       uint8_t num_hash_funcs)
    : num_bloom_filters_(num_bloom_filters),
      bits_per_bloom_filter_(CACHE_LINE_BYTES * 8),
      //total_size_in_bits_(bits_per_bloom_filter_ * num_bloom_filters_),
      hash_func_(family->get()),
      num_simulated_hash_funcs_(num_hash_funcs),
      num_inserted_(0) {
  const size_t total_size_bytes =
      sizeof(struct cache_line) * num_bloom_filters_;
  // aligned_alloc is the C11 standard, but it doesn't exist on my machine for
  // some reason.
  //bloom_filter_array_ = (struct cache_line *) aligned_alloc(
  //    CACHE_LINE_BYTES, total_size_bytes);
  posix_memalign((void **)&bloom_filter_array_, CACHE_LINE_BYTES,
      total_size_bytes);
  memset(bloom_filter_array_, 0, total_size_bytes);
  bits_flipped_ = (uint16_t*) calloc(sizeof(uint16_t), num_bloom_filters_);
}

BlockedBloomFilter::~BlockedBloomFilter() {
  free(bloom_filter_array_);
  free(bits_flipped_);
}

bool BlockedBloomFilter::get(size_t bloom_filter_index, size_t bit_index)
    const {
  const size_t byte_index = bit_index / 8;
  const unsigned char offset = bit_index % 8;
  const struct cache_line* line =
      &bloom_filter_array_[bloom_filter_index];
  return (line->filter[byte_index] & (1 << offset)) > 0;
}

void BlockedBloomFilter::set(size_t bloom_filter_index, size_t bit_index,
    bool value) {
  const size_t byte_index = bit_index / 8;
  const unsigned char offset = bit_index % 8;
  const unsigned char mask = 1 << offset;
  if (value) {
    bloom_filter_array_[bloom_filter_index].filter[byte_index] |= mask;
  } else {
    bloom_filter_array_[bloom_filter_index].filter[byte_index] &= ~mask;
  }
}

// This is an optimization specified in the Cuckoo filter paper. Each hash
// needs to be 32 bits. CityHash64 produces 64 bits, so the first 32 bits count
// as the first hash (h1), the second 32 bits count as the second (h2), and then
// every hash i after that is set to h1 + h2 * i, reducing the number of calls
// to CityHash64.
size_t BlockedBloomFilter::hash_data_and_get_filter_index(
    int data, uint32_t* hashes) const {
  const uint64_t hash = hash_func_(data);
  //uint32_t hashes[num_simulated_hash_funcs_];
  const uint32_t h1 = (uint32_t) hash;
  const uint32_t h2 = (uint32_t) (hash >> sizeof(uint32_t));
  hashes[0] = h1;
  if (num_simulated_hash_funcs_ > 1) {
    hashes[1] = h2;
  }
  for (uint8_t i = 2; i < num_simulated_hash_funcs_; i++) {
    hashes[i] = h1 + h2 * i;
  }
  return (h1 >> 8) % num_bloom_filters_;
}

int BlockedBloomFilter::insert(int data) {

  uint32_t hashes[num_simulated_hash_funcs_];
  const size_t filter_index = hash_data_and_get_filter_index(data, &hashes[0]);

  // Arbitrary "fullness" threshold, done just for the testing infrastructure.
  // Bloom filters don't really reach fullness.
  //if (num_inserted_ >= total_size_in_bits_ / num_simulated_hash_funcs_ * 2) {
  //  return -1;
 // }
  float frac_bits_flipped = bits_flipped_[filter_index];
  frac_bits_flipped /= (CACHE_LINE_BYTES * 8 * 1.0);
  if (frac_bits_flipped >= 0.9) {
    return -1;
  }

  for (uint8_t i = 0; i < num_simulated_hash_funcs_; i++) {
    const size_t bit_index = hashes[i] % bits_per_bloom_filter_;
    if (!get(filter_index, bit_index)) {
      set(filter_index, bit_index, true);
      bits_flipped_[filter_index]++;
    }
  }
  num_inserted_++;
  return 1;
}

bool BlockedBloomFilter::contains(int data) const {
  uint32_t hashes[num_simulated_hash_funcs_];
  const size_t filter_index = hash_data_and_get_filter_index(data, &hashes[0]);
  for (uint8_t i = 0; i < num_simulated_hash_funcs_; i++) {
    const size_t bit_index = hashes[i] % bits_per_bloom_filter_;
    if (!get(filter_index, bit_index)) {
      return false;
    }
  }
  return true;
}
