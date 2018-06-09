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
// does not specify. (The Cuckoo paper used 12 bits for the Cuckoo filter
// fingerprints.)
// This determines what bits of the hash comprise the "remainder".
#define REMAINDER_BITS 12
#define REMAINDER_MASK 0x0fff
#define CELL_COUNT_BITS 4
#define CELL_COUNT_MAX_VAL 0xf

/*
  16 bits per cell
  4 cells per bucket
  BUCK buckets per subtable
  4 tables per dlcbf

  1 dlcbf = 1 dlcbf * 4 t/dlcbf * BUCK b/t * 4 c/b * 16 bits/c = 192MB
  256*BUCK bits = 192MB
  32*BUCK bytes = 192MB
  BUCK = 192 * 1024 * 1024 / 32
       = 6291456
  So buckets per subtable = BUCK/4 = 1572864
*/

class DLeftCountingBloomFilter {
 public:
  // To get 192MB like the Cuckoo filter paper, pass in this value:
  //   1572864
  // (See above for the calculation)
  DLeftCountingBloomFilter(uint32_t buckets_per_subtable,
                           std::shared_ptr<HashFamily> family);
  ~DLeftCountingBloomFilter();
  int insert(int data);
  bool contains(int data) const;
  void remove(int data);

 private:
  uint16_t get_targets(int data, uint32_t targets[NUM_SUBTABLES]) const;
  struct dlcbf_bucket {
    uint8_t fill_count;
    struct cell {
      uint16_t fingerprint:REMAINDER_BITS;
      uint8_t count:CELL_COUNT_BITS;
    } cells[BUCKET_HEIGHT];
    uint16_t fingerprints[BUCKET_HEIGHT];
  };
  struct subtable {
    struct dlcbf_bucket* buckets;
  };
  struct subtable subtables_[NUM_SUBTABLES];
  uint16_t num_buckets_per_subtable_;
  HashFunction hash_func_;
  // Hash functions aren't actually valid permutation functions.
  //std::vector<HashFunction> permutations_;
  // Disable copy construction.
  DLeftCountingBloomFilter(DLeftCountingBloomFilter const &) = delete;
  void operator=(DLeftCountingBloomFilter const &) = delete;
};

#endif  // DLeftCountingBloomFilter_Included
