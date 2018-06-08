#include "BucketsTable.h"
#include <stddef.h>
#include <stdlib.h>
#include <set>
#include <vector>
#include <iostream>

BucketsTable::BucketsTable(size_t numBuckets){
    num_buckets = numBuckets;
    num_elems = 0;
    buckets = new struct buck_struct[num_buckets];
    f_size = 12;
    // Set to all zeros
    for(size_t i = 0; i < num_buckets; i++){
        buckets[i].e1 = 0;
        buckets[i].e2 = 0;
        buckets[i].e3 = 0;
        buckets[i].e4 = 0;
    }
}

BucketsTable::~BucketsTable() {
    delete buckets;
}

bool BucketsTable::has(uint32_t f, int ind1, int ind2) const{
    return check_val(f, ind1) || check_val(f, ind2);
}

bool BucketsTable::full(int ind) {
    //std::cout<<buckets[ind].e1<<std::endl;
    return (buckets[ind].e1 != 0 &&
            buckets[ind].e2 != 0 &&
            buckets[ind].e3 != 0 &&
            buckets[ind].e4 != 0);
}

void BucketsTable::add(uint32_t f, int ind) {
    buck_struct b = buckets[ind];
    if(b.e1 == 0){
        buckets[ind].e1 = f;
    }else if(b.e2 == 0) {
        buckets[ind].e2 = f;
    }else if(b.e3 == 0) {
        buckets[ind].e3 = f;
    }else{
        buckets[ind].e4 = f;
    }
    num_elems += 1;
}

uint32_t BucketsTable::evict_rand_and_replace(uint32_t f, int ind) {
    int b_ind = rand() % 4;
    uint32_t f_ret = 0;
    switch(b_ind) {
        case 0: f_ret = buckets[ind].e1;
                buckets[ind].e1 = f;
                break;
        case 1: f_ret = buckets[ind].e2;
                buckets[ind].e2 = f;
                break;
        case 2: f_ret = buckets[ind].e3;
                buckets[ind].e3 = f;
                break;
        case 3: f_ret = buckets[ind].e4;
                buckets[ind].e4 = f;
                break;

    }
    return f_ret;
}

bool BucketsTable::check_and_remove(uint32_t f, int ind) {
    buck_struct b = buckets[ind];
    if(b.e1 == f){
        buckets[ind].e1 = 0;
    }else if(b.e2 == f) {
        buckets[ind].e2 = 0;
    }else if(b.e3 == f) {
        buckets[ind].e3 = 0;
    }else if(b.e4 == f){
        buckets[ind].e4 = 0;
    }else {
        return false;
    }
    return true;
}

bool BucketsTable::check_val(uint32_t f, int ind) const{
    buck_struct b = buckets[ind];
    return ((b.e1 == f) ||
            (b.e2 == f) ||
            (b.e3 == f) ||
            (b.e4 == f));
} 
