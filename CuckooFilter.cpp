#include "CuckooFilter.h"
#include "BucketsTable.h"
#include <math.h>
#include <vector>
#include <functional>
#include <stdlib.h>
#include <iostream>

CuckooFilter::CuckooFilter(size_t num_buckets, std::shared_ptr<HashFamily> family):
                           buckets(num_buckets){
  numBucks = num_buckets;
  h1 = family->get();
  finger_pointer = family->get();
  num_max_cuckoos = 500;
  num_elems = 0;
  f_bit_size = 12;
  f_mask = (1ULL << f_bit_size) - 1;
}

CuckooFilter::~CuckooFilter() {
}

uint32_t CuckooFilter::get_finger_print(uint64_t data) const{
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


int CuckooFilter::run_cuckoo_loop(uint32_t f, int ind1, int ind2) {
  //std::cout<<"Running Cuckoo Loop"<<std::endl;
  int counter = rand() % 2;
  int ind = (counter % 2 == 0) ? ind2 : ind1;
  //std::cout<<"ENTERING CUCKOO LOOP"<<std::endl;
  for(size_t i = 0; i < num_max_cuckoos; i++) {
    //std::cout<<"CC Loop Iter num: "<<i<<std::endl;
    if(!buckets.full(ind)){
        buckets.add(f, ind);
        return 1;
    }
    f = buckets.evict_rand_and_replace(f, ind);
    ind = (ind ^ h1(f)) % numBucks; 
  }
  return -1;
}

int CuckooFilter::insert(uint64_t data) {
  uint32_t f = get_finger_print(data);
  int ind1 = h1(data) % numBucks;
  int ind2 = (ind1 ^ h1(f)) % numBucks;
 // std::cout<<"Ind 1: "<<ind1<<" Ind 2: "<<ind2<<std::endl;
  if(buckets.has(f, ind1, ind2)){
      //std::cout<<"ADDED IN HAS"<<std::endl;
      //if(b1[ind1].has(f)){
      //  std::cout<<"Has B1"<<std::endl;
      //}else {
      //  std::cout<<"Has B2"<<std::endl;
      //}
      
      return 1;
  }
  if(!buckets.full(ind1)){
    //std::cout<<"ADDED IN FULL B1"<<std::endl;
    buckets.add(f, ind1);
    return 1;
  }
  if(!buckets.full(ind2)){
    //std::cout<<"ADDED IN FULL B2"<<std::endl;
    buckets.add(f, ind2);
    return 1;
  }
  return run_cuckoo_loop(f, ind1, ind2);
}

bool CuckooFilter::contains(uint64_t data) const {
  uint32_t f = get_finger_print(data);
  int ind1 = h1(data) % numBucks;
  int ind2 = (ind1 ^ h1(f)) % numBucks;
  return buckets.has(f, ind1, ind2);
}

void CuckooFilter::remove(uint64_t data) {
  uint32_t f = get_finger_print(data);
  int ind1 = h1(f);
  int ind2 = ind1 ^ f;
  if(buckets.check_and_remove(f, ind1))
      return;
  buckets.check_and_remove(f, ind2);
}
