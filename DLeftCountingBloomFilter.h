#ifndef DLeftCountingBloomFilter_Included
#define DLeftCountingBloomFilter_Included

#include <cstdlib>
#include "Hashes.h"

// Allowed range: 1 to 255
#define NUM_SUBTABLES 4
// Allowed range: 1 to 65535
#define NUM_BUCKETS_PER_SUBTABLE 2048
// Allowed range: 1 to 255
#define BUCKET_HEIGHT 8
// Allowed range: 1 to 16 bits
// This sets what bits of the hash count as the remainder (stored in the table).
#define REMAINDER_MASK 0x5fffffff

class DLeftCountingBloomFilter {
 public:
  DLeftCountingBloomFilter(size_t unused_size_in_bits,
                           std::shared_ptr<HashFamily> family);
  ~DLeftCountingBloomFilter();
  int insert(int data);
  bool contains(int data) const;
  void remove(int data);

 private:
  uint16_t get_targets(int data, uint16_t targets[NUM_SUBTABLES]) const;
  struct subtable {
    struct bucket {
      uint8_t fill_count;
      uint16_t fingerprints[BUCKET_HEIGHT];
    } buckets[NUM_BUCKETS_PER_SUBTABLE];
  };
  struct subtable* subtables_;
  HashFunction hash_func_;
  // Disable copy construction.
  DLeftCountingBloomFilter(DLeftCountingBloomFilter const &) = delete;
  void operator=(DLeftCountingBloomFilter const &) = delete;
};

#endif  // DLeftCountingBloomFilter_Included
