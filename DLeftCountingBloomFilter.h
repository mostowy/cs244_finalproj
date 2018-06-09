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
#define REMAINDER_BITS 8
#define REMAINDER_MASK 0x00ff
#define CELL_COUNT_BITS 3
#define CELL_COUNT_MAX_VAL 0x7

/*
  Cuckoo filter paper:
  105M inserts in 192MB memory
  Assume 75% cell occupancy. Then:
  # cells = 105M / .75 = 140M
  # buckets = 140M / 4 = 35M
  # buckets/table = 35M / 4 = 9M

  # bits/cell = 192MB / 140M = 11
  That means that fingerprints are pretty small!
  Probably 11 bits = 8-bit fingerprint + 3-bit counter.


  Mirror the above calculations for our own impl

  8+3 bits per cell
  4 cells per bucket
  BUCK buckets per subtable
  4 tables per dlcbf

  1 dlcbf = 1 dlcbf * 4 t/dlcbf * BUCK b/t * 4 c/b * 11 bits/c = 192MB
  176*BUCK bits = 192MB
  22*BUCK bytes = 192MB
  BUCK = 192 * 1024 * 1024 / 22
       = 9151208
*/

class DLeftCountingBloomFilter {
 public:
  // To get 192MB like the Cuckoo filter paper, pass in this value:
  //   9151208
  // (See above for the calculation)
  DLeftCountingBloomFilter(uint32_t buckets_per_subtable,
                           std::shared_ptr<HashFamily> family);
  ~DLeftCountingBloomFilter();
  int insert(uint64_t data);
  bool contains(uint64_t data) const;
  void remove(uint64_t data);

 private:
  uint16_t get_targets(uint64_t data, uint32_t targets[NUM_SUBTABLES]) const;
  void dump_occupancy() const;
  struct dlcbf_bucket {
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
  uint32_t num_buckets_per_subtable_;
  HashFunction hash_func_;
  uint64_t num_inserted_;
  // Disable copy construction.
  DLeftCountingBloomFilter(DLeftCountingBloomFilter const &) = delete;
  void operator=(DLeftCountingBloomFilter const &) = delete;
};

#endif  // DLeftCountingBloomFilter_Included
