#include "SSCuckooFilter.h"
#include "SemiSortTable.h"
#include <math.h>
#include <vector>
#include <functional>
#include <stdlib.h>
#include <iostream>

SSCuckooFilter::SSCuckooFilter(size_t arr_size, std::shared_ptr<HashFamily> family) 
:buckets(arr_size, 12){
  numBucks = arr_size;
  h1 = family->get();
  finger_pointer = family->get();
  num_max_cuckoos = 500;
  num_elems = 0;
  f_bit_size = 12;
  f_mask = (1ULL << f_bit_size) - 1;  
}

SSCuckooFilter::~SSCuckooFilter() {
}

uint32_t SSCuckooFilter::get_finger_print(uint64_t data) const{
    // Problem here is that hash is 64 but we want
    // a specific finger print size.  For us, that
    // size is 12 for the experiment
    uint64_t large_finger = finger_pointer(data);
    
    // Get 32 most significant bits, mask off f
    uint32_t f = (large_finger >> 32) & f_mask;
    
    // Make sure finger print is not zero
    f += (f == 0);
    return f;
}

int SSCuckooFilter::run_cuckoo_loop(uint32_t f, int ind1, int ind2) {
  int counter = rand() % 2;
  int ind = (counter % 2 == 0) ? ind2 : ind1;
  uint32_t placeholder = 0;

  for(size_t i = 0; i < num_max_cuckoos; i++) {
    if(buckets.add(f, ind, placeholder)){
        return 1; 
    }
    f = placeholder;
    ind = (ind ^ h1(f)) % numBucks;
  }
  return -1;
}

int SSCuckooFilter::insert(uint64_t data) {
  uint32_t f = get_finger_print(data);
  int ind1 = h1(data) % numBucks;
  int ind2 = (ind1 ^ h1(f)) % numBucks;
  uint32_t placeholder = 0;
 // std::cout<<"Ind 1: "<<ind1<<" Ind 2: "<<ind2<<std::endl;
  if(buckets.has(f, ind1, ind2)){
      //std::cout<<"ADDED IN HAS"<<std::endl;
      return 1;
  }
  if(!buckets.full(ind1)){
    //std::cout<<"ADDED IN FULL B1: "<<f<<std::endl;
    buckets.add(f, ind1, placeholder);
    return 1;
  }
  if(!buckets.full(ind2)){
    //std::cout<<"ADDED IN FULL B2"<<std::endl;
    buckets.add(f, ind2, placeholder);
    return 1;
  }
  //std::cout<<"Running Cuckoo Loop"<<std::endl;
  return run_cuckoo_loop(f, ind1, ind2);
}

bool SSCuckooFilter::contains(uint64_t data) const {
  uint32_t f =  get_finger_print(data);
  int ind1 = h1(data) % numBucks;
  int ind2 = (ind1 ^ h1(f)) % numBucks;
  return buckets.has(f, ind1, ind2);
}

void SSCuckooFilter::remove(uint64_t data) {
  uint32_t f =  get_finger_print(data);
  int ind1 = h1(f);
  int ind2 = ind1 ^ f;
  if(buckets.check_and_remove(f, ind1))
      return;
  buckets.check_and_remove(f, ind2);
}
