#ifndef CuckooFilter_Included
#define CuckooFilter_Included

#include "Hashes.h"
#include "BucketsTable.h"
#include <vector>

class CuckooFilter {
 public:
  /**
   * Constructs a new cuckoo hash table with the specified number of buckets,
   * using hash functions drawn from the indicated family of hash functions.
   *
   * Since cuckoo hashing requires two tables, you should create two tables
   * of size numBuckets / 2. Because our testing harness attempts to exercise
   * a number of different load factors, you should not change the number of
   * buckets once the hash table has initially be created.
   *
   * You can choose a hash function out of the family of hash functions by
   * declaring a variable of type HashFunction and assigning it the value
   * family->get(). For example:
   *
   *    HashFunction h;
   *    h = family->get();
   *
   * Because cuckoo hashing may require a rehash if elements can't be placed
   * into the table, you will need to store the hash family for later use.
   * You can do so by declaring a data member of type
   *
   *    std::shared_ptr<HashFamily>
   *
   * and assigning 'family' to it.
   */
   CuckooFilter(size_t arr_size, std::shared_ptr<HashFamily> family);
  
  /**
   * Cleans up all memory allocated by this filter.
   */
  ~CuckooFilter();
  
  /**
   * Inserts the specified element into this hash table. If the element already
   * exists, this operation is a no-op.
   *
   * When performing an insertion, you may need to trigger a rehash if too many
   * displacements occur. To do so, keep track of the number of times that you
   * have displaced an element. If it ever exceeds 6 lg n, you should trigger
   * a rehash.
   */
  int insert(uint64_t data);
  
  /**
   * Returns whether the specified key is contained in this hash tasble.
   */
  bool contains(uint64_t data) const;
  
  /**
   * Removes the specified element from this hash table. If the element is not
   * present in the hash table, this operation is a no-op.
   */
  void remove(uint64_t data);

  /**
   * Runs the loop of 'cuckoos' where on each iteration if no open spots,
   * a finger print  is randomly selected and evicted.  This process just
   * loops through this
   */
  int run_cuckoo_loop(uint32_t f, int ind1, int ind2);  


  /**
   * Function takes the data and gets the finger print.  After looking
   * at the implementation in the paper, this is done by hashing the 
   * data, then taking the finger pring from the f least significant
   * bits of the 32 most significant bits of the hash.  So, in mem
   * it looks like:
   * |-- 32-f --|-- finger print --|----_ 32 least significant bits --------|
   */
  uint32_t get_finger_print(uint64_t data) const;

private:
  size_t numBucks;       // Number of buckets in the table
  size_t num_elems;      // Number of elements in the table
  int num_max_cuckoos;   // Max Cuckoo loops
  int f_bit_size;        // Size in bits of finger print
  uint64_t f_mask;       // Used to mask the the finger print
  BucketsTable buckets;  // Buckets Table 
  HashFunction h1;       // Hash function for the index
  HashFunction finger_pointer;  // Hash Function for the finger print
  CuckooFilter(CuckooFilter const &) = delete;
  void operator=(CuckooFilter const &) = delete;
};

#endif
