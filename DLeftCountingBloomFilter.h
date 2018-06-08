#ifndef DLeftCountingBloomFilter_Included
#define DLeftCountingBloomFilter_Included

#include <cstdlib>
#include <vector>
#include "Hashes.h"

// Allowed range: 1 to 255. The dlcbf paper used 4; the cuckoo paper used 4.
#define NUM_SUBTABLES 4
// Allowed range: 1 to 255. The dlcbf paper used 8; the cuckoo paper used 4.
#define BUCKET_HEIGHT 4
// Allowed range: 1 to 16 bits. The dlcbf paper used 14; the cuckoo paper
// does not specify.
// This sets what bits of the hash count as the remainder (stored in the table).
#define REMAINDER_MASK 0x5fffffff

class DLeftCountingBloomFilter {
 public:
  DLeftCountingBloomFilter(uint16_t buckets_per_subtable,
                           std::shared_ptr<HashFamily> family);
  ~DLeftCountingBloomFilter();
  int insert(int data);
  bool contains(int data) const;
  void remove(int data);

 private:
  uint16_t get_targets(int data, uint16_t targets[NUM_SUBTABLES]) const;
  struct dlcbf_bucket {
    uint8_t fill_count;
    uint16_t fingerprints[BUCKET_HEIGHT];
  };
  struct subtable {
    struct dlcbf_bucket* buckets;
  };
  struct subtable subtables_[NUM_SUBTABLES];
  uint16_t num_buckets_per_subtable_;
  HashFunction hash_func_;
  std::vector<HashFunction> permutations_;
  // Disable copy construction.
  DLeftCountingBloomFilter(DLeftCountingBloomFilter const &) = delete;
  void operator=(DLeftCountingBloomFilter const &) = delete;
};

#endif  // DLeftCountingBloomFilter_Included
