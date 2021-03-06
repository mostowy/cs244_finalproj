#ifndef QuotientFilter_Included
#define QuotientFilter_Included

#include "Hashes.h"
#include <vector>
class QuotientFilter {
public:
  /**
   * Constructs a new Quotient Filter with the specified number of buckets,
   * using hash functions drawn from the indicated family of hash functions.
   */
  QuotientFilter(size_t numBuckets, std::shared_ptr<HashFamily> family);
  
  /**
   * Cleans up all memory allocated by this filter.
   */
  ~QuotientFilter();
 
  /**
   * Function given a fingerprint returns either the q
   * or r value depending on the is_q value specified
   */
  uint64_t getqr(uint64_t f, bool is_q) const;

  /**
   * Inserts the specified element into filter. If the element already
   * exists, this operation is a no-op.
   */
  int insert(uint64_t data);

  /**
   * Function sets the three indicator bits for the given
   * index to the given value.
   */
  void set_3_bit(uint64_t ind, bool occ, bool cont, bool shift);
  
  /**
   * Returns whether the specified key is contained in the filter.
   */
  bool contains(uint64_t data) const;
  
  /**
   * Removes the specified element from the filter. If the element is not
   * present in the hash table, this operation is a no-op.
   */
  void remove(uint64_t data);
  
  /**
   * Function perfroms a linear scan starting from the given bucket.
   * The LinScan function is only used when the array is full.
   */
  bool linscan(uint64_t data, size_t bucket) const;  
  
  /**
   * Function takes in a bucket index and
   * decrements it, accounting for wrap around.
   */
  uint64_t decrement(uint64_t bucket) const;
  
  
  /**
   * Funciton returns whether or not a given
   * index is filled (true) or empty (false)
   */
  bool isFilled(uint64_t ind) const;
  
  /**
   * Function increments the given bucket according to the
   * wrap around rule.
   */
  uint64_t increment(size_t numBucks, uint64_t bucket) const;
  
  /**
   * Function returns whether or not the given bucket
   * is the beginning of a cluster.
   */
  bool isClusterStart(uint64_t bucket) const;
  
  /**
   * Finds the run for the given bucket.
   */
  uint64_t find_run(uint64_t bucket) const;

  struct b_struct {
    unsigned r:9;
  };

private:
  // Array of buckets containing data
  //std::vector<uint16_t>  buckets;
  struct b_struct* buckets;
  size_t numBucks;
  size_t num_elems;
  HashFunction fp;
  int r;
  int q;
  std::vector<bool> stat_arr; // Use Bool array cause it only stores one bit 
  // Only caviate with bitsets is all of them 
  QuotientFilter(QuotientFilter const &) = delete;
  void operator=(QuotientFilter const &) = delete;
};

#endif
