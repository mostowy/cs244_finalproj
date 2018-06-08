#ifndef BlockedBloomFilter_Included
#define BlockedBloomFilter_Included

#include <vector>
#include "Hashes.h"

// The cuckoo paper had 64 bytes as the size of a cache line in their testbed.
#define CACHE_LINE_BYTES 64

class BlockedBloomFilter {
 public:
  // To get 192MB like the Cuckoo filter paper, pass in this value:
  //   192 * 1024 * 1024 / CACHE_LINE_BYTES = 3145728
  BlockedBloomFilter(size_t num_bloom_filters,
                     std::shared_ptr<HashFamily> family,
                     uint8_t num_hash_funcs = 9);
  ~BlockedBloomFilter();
  int insert(int data);
  bool contains(int data) const;

 private:
  bool get(size_t bloom_filter_index, size_t bit_index) const;
  void set(size_t bloom_filter_index, size_t bit_index, bool value);
  size_t hash_data_and_get_filter_index(int data, uint32_t* hashes) const;
  struct cache_line {
    unsigned char filter[CACHE_LINE_BYTES];
  };
  struct cache_line* bloom_filter_array_;
  uint16_t* bits_flipped_;
  size_t num_bloom_filters_;
  size_t bits_per_bloom_filter_;
  //size_t total_size_in_bits_;
  HashFunction hash_func_;
  uint8_t num_simulated_hash_funcs_;
  size_t num_inserted_;
  // Disable copy construction.
  BlockedBloomFilter(BlockedBloomFilter const &) = delete;
  void operator=(BlockedBloomFilter const &) = delete;
};

#endif  // BlockedBloomFilter_Included
