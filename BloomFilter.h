#ifndef BloomFilter_Included
#define BloomFilter_Included

#include "Hashes.h"

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
  // To get 192MB like the Cuckoo filter paper, pass in this number of bits:
  //   192 * 1024 * 1024 * 8 = 1610612736
  BloomFilter(size_t size_in_bits, std::shared_ptr<HashFamily> family,
              uint8_t num_hash_funcs = 9);
  ~BloomFilter();
  int insert(int data);
  bool contains(int data) const;

 private:
  void hash_data(int data, uint32_t* hashes) const;
  BitVector bit_vector_;
  HashFunction hash_func_;
  uint8_t num_simulated_hash_funcs_;
  uint64_t num_bits_flipped_;
  // This is an artificial way of saying that the bloom filter is "full" for
  // the purposes of the testing infrastructure.
  size_t num_inserted_;
  // Disable copy construction.
  BloomFilter(BloomFilter const &) = delete;
  void operator=(BloomFilter const &) = delete;
};

#endif  // BloomFilter_Included
