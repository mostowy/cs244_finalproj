#ifndef BloomFilter_Included
#define BloomFilter_Included

#include "Hashes.h"
#include <vector>

class BitVector {
 public:
  BitVector(size_t size_in_bits);
  ~BitVector();
  bool get(size_t bit_index) const;
  void set(size_t bit_index, bool value);
  size_t size() const { return size_in_bits_; }

 private:
  size_t size_in_bits_;
  unsigned char* vector_;
  // Disable copy construction.
  BitVector(BitVector const &) = delete;
  void operator=(BitVector const &) = delete;
};

class BloomFilter {
 public:
  BloomFilter(size_t size_in_bits, std::shared_ptr<HashFamily> family,
              int num_hash_funcs = 8);
  ~BloomFilter();
  int insert(int data);
  bool contains(int data) const;

 private:
  BitVector bit_vector_;
  std::vector<HashFunction> hash_funcs_;
  // This is an artificial way of saying that the bloom filter is "full" for
  // the purposes of the testing infrastructure.
  int num_inserted_;
  // Disable copy construction.
  BloomFilter(BloomFilter const &) = delete;
  void operator=(BloomFilter const &) = delete;
};

#endif  // BloomFilter_Included
