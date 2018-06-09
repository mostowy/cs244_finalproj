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
// does not specify, except to say that each "entry" (i.e. cell) is 12 bits.
// Most likely, this means that they used 8 bits per fingerprint.
// (Interestingly, the Cuckoo paper used 12 bits per fingerprint for the
// Cuckoo filter fingerprints.)
// This determines what bits of the hash comprise the "remainder".
#define REMAINDER_BITS 8
#define REMAINDER_MASK 0x00ff
#define CELL_COUNT_BITS 4
#define CELL_COUNT_MAX_VAL 0xf

/*
  Cuckoo filter paper:
  2^25 total buckets
  4 entries per bucket
  12 bits per entry
  ... so probably 8 bits per fingerprint (+/- 2)
  ... so probably 4 bits per counter (+/- 2)
  This yields 192MB total memory footprint.

  2^25 total buckets / dlcbf / (4 subtables / dlcbf)
  = 2^23 buckets / subtable
  = 8388608 buckets / subtable
*/

class DLeftCountingBloomFilter {
 public:
  // To get 192MB like the Cuckoo filter paper, pass in (2^25 / 4), or:
  //   8388608
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
