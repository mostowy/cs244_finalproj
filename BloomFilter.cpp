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

size_t BitVector::size() const {
  return size_in_bits_;
}

BloomFilter::BloomFilter(size_t size_in_bits, std::shared_ptr<HashFamily> family,
                         int num_hash_funcs)
    : bit_vector_(size_in_bits),
      num_inserted_(0) {
  for (int i = 0; i < num_hash_funcs; i++) {
    hash_funcs_.push_back(family->get());
  }
}

BloomFilter::~BloomFilter() {
}

int BloomFilter::insert(int data) {
  // Arbitrary "fullness" threshold, done just for the testing infrastructure.
  // Bloom filters don't really reach fullness.
  if (num_inserted_ >= bit_vector_.size() * hash_funcs_.size()) return -1;

  for (auto const& hash : hash_funcs_) {
    const size_t bit_index = hash(data) % bit_vector_.size();
    bit_vector_.set(bit_index, true);
  }
  num_inserted_++;
  return 1;
}

bool BloomFilter::contains(int data) const {
  for (auto const& hash : hash_funcs_) {
    const size_t bit_index = hash(data) % bit_vector_.size();
    if (!bit_vector_.get(bit_index)) {
      return false;
    }
  }
  return true;
}
