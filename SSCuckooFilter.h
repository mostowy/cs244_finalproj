#ifndef SSCuckooFilter_Included
#define SSCuckooFilter_Included

#include "Hashes.h"
#include "SemiSortTable.h"
#include <vector>

class SSCuckooFilter {
 public:
  /**
   * Constructs a semi sort cuckoo filter
   */
   SSCuckooFilter(size_t arr_size, std::shared_ptr<HashFamily> family);
  
  /**
   * Cleans up all memory allocated
   */
  ~SSCuckooFilter();
  
  /**
   * Inserts the specified element into the filter. If the element already
   * exists, this operation is a no-op.
   */
  int insert(uint64_t data);
  
  /**
   * Returns whether the specified key is contained in this filter.
   */
  bool contains(uint64_t data) const;
  
  /**
   * Removes the specified element from this filter. If the element is not
   * present, this operation is a no-op.
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
  
  /* Fun with C++: these next two lines disable implicitly-generated copy
   * functions that would otherwise cause weird errors if you tried to
   * implicitly copy an object of this type. You don't need to touch these
   * lines.
   */
  size_t numBucks;
  size_t num_elems;
  int num_max_cuckoos;
  int f_bit_size;
  uint64_t f_mask;
  SemiSortTable buckets;
  HashFunction h1;
  HashFunction finger_pointer;
  SSCuckooFilter(SSCuckooFilter const &) = delete;
  void operator=(SSCuckooFilter const &) = delete;
};

#endif
