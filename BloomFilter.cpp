#include "BloomFilter.h"
#include <stdlib.h>

BitVector::BitVector(size_t size_in_bits)
    : size_in_bits_(size_in_bits) {
  const bool need_extra_byte = size_in_bits % 8 > 0;
  const size_t size_in_bytes = size_in_bits / 8 + (need_extra_byte ? 1 : 0);
  vector_ = (unsigned char *)calloc(size_in_bytes, 1);
}

BitVector::~BitVector() {
  free(vector_);
}

bool BitVector::get(size_t bit_index) const {
  const size_t byte_index = bit_index / 8;
  const unsigned char offset = bit_index % 8;
  return (vector_[byte_index] & (1 << offset)) > 0;
}

void BitVector::set(size_t bit_index, bool value) {
  const size_t byte_index = bit_index / 8;
  const unsigned char offset = bit_index % 8;
  const unsigned char mask = 1 << offset;
  if (value) {
    vector_[byte_index] |= mask;
  } else {
    vector_[byte_index] &= ~mask;
  }
}

BloomFilter::BloomFilter(size_t size_in_bits, std::shared_ptr<HashFamily> family,
                         uint8_t num_hash_funcs)
    : bit_vector_(size_in_bits),
      hash_func_(family->get()),
      num_simulated_hash_funcs_(num_hash_funcs),
      num_inserted_(0) {
  if (num_simulated_hash_funcs_ < 1) num_simulated_hash_funcs_ = 1;
}

BloomFilter::~BloomFilter() {
}

// This is an optimization specified in the Cuckoo filter paper. Each hash
// needs to be 32 bits. CityHash64 produces 64 bits, so the first 32 bits count
// as the first hash (h1), the second 32 bits count as the second (h2), and then
// every hash i after that is set to h1 + h2 * i, reducing the number of calls
// to CityHash64.
void BloomFilter::hash_data(int data, uint32_t* hashes) const {
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
}

int BloomFilter::insert(int data) {
  // Arbitrary "fullness" threshold, done just for the testing infrastructure.
  // Bloom filters don't really reach fullness.
  if (num_inserted_ >= bit_vector_.size() / num_simulated_hash_funcs_ * 2) {
    return -1;
  }

  uint32_t hashes[num_simulated_hash_funcs_];
  hash_data(data, &hashes[0]);

  for (uint8_t i = 0; i < num_simulated_hash_funcs_; i++) {
    const size_t bit_index = hashes[i] % bit_vector_.size();
    bit_vector_.set(bit_index, true);
  }
  num_inserted_++;
  return 1;
}

bool BloomFilter::contains(int data) const {
  uint32_t hashes[num_simulated_hash_funcs_];
  hash_data(data, &hashes[0]);
  for (uint8_t i = 0; i < num_simulated_hash_funcs_; i++) {
    const size_t bit_index = hashes[i] % bit_vector_.size();
    if (!bit_vector_.get(bit_index)) {
      return false;
    }
  }
  return true;
}
