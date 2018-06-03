#ifndef BlockedBloomFilter_Included
#define BlockedBloomFilter_Included

#include <vector>
#include "Hashes.h"

#define NUM_BLOOM_FILTERS 8
#define CACHE_LINE_BYTES 128

class BlockedBloomFilter {
 public:
  BlockedBloomFilter(size_t unused_size_in_bits,
                     std::shared_ptr<HashFamily> family,
                     int num_hash_funcs = 8);
  ~BlockedBloomFilter();
  int insert(int data);
  bool contains(int data) const;

 private:
  bool get(size_t bloom_filter_index, size_t bit_index) const;
  void set(size_t bloom_filter_index, size_t bit_index, bool value);
  size_t get_filter_index_for(int data) const;
  alignas(CACHE_LINE_BYTES) unsigned char
      bloom_filter_array_[NUM_BLOOM_FILTERS][CACHE_LINE_BYTES];
  size_t bits_per_bloom_filter_;
  size_t total_size_in_bits_;
  std::vector<HashFunction> hash_funcs_;
  int num_inserted_;
  // Disable copy construction.
  BlockedBloomFilter(BlockedBloomFilter const &) = delete;
  void operator=(BlockedBloomFilter const &) = delete;
};

#endif  // BlockedBloomFilter_Included
