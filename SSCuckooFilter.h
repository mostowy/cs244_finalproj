#ifndef SSCuckooFilter_Included
#define SSCuckooFilter_Included

#include "Hashes.h"
#include "SemiSortTable.h"
#include <vector>

class SSCuckooFilter {
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
   SSCuckooFilter(size_t arr_size, std::shared_ptr<HashFamily> family);
  
  /**
   * Cleans up all memory allocated by this hash table.
   */
  ~SSCuckooFilter();
  
  /**
   * Inserts the specified element into this hash table. If the element already
   * exists, this operation is a no-op.
   *
   * When performing an insertion, you may need to trigger a rehash if too many
   * displacements occur. To do so, keep track of the number of times that you
   * have displaced an element. If it ever exceeds 6 lg n, you should trigger
   * a rehash.
   */
  int insert(int key);
  
  /**
   * Returns whether the specified key is contained in this hash tasble.
   */
  bool contains(int key) const;
  
  /**
   * Removes the specified element from this hash table. If the element is not
   * present in the hash table, this operation is a no-op.
   */
  void remove(int key);

  /**
   * Runs the loop of 'cuckoos' where on each iteration if no open spots,
   * a finger print  is randomly selected and evicted.  This process just
   * loops through this
   */
  int run_cuckoo_loop(uint32_t f, int ind1, int ind2);  

private:
  
  /* Fun with C++: these next two lines disable implicitly-generated copy
   * functions that would otherwise cause weird errors if you tried to
   * implicitly copy an object of this type. You don't need to touch these
   * lines.
   */
  size_t numBucks;
  size_t num_elems;
  int num_max_cuckoos;
  SemiSortTable buckets;
  HashFunction h1;
  HashFunction finger_pointer;
  SSCuckooFilter(SSCuckooFilter const &) = delete;
  void operator=(SSCuckooFilter const &) = delete;
};

#endif
