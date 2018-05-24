#include "BucketsTable.h"


BucketsTable::BucketsTable(size_t numBuckets) {
    num_buckets = numBuckets;
    num_elems = 0;
}

BucketsTable::~BucketsTable() {
    // TODO Implement this
}

bool BucketsTable::has(int f) {
    // TODO: Implement this
    return false;
}

bool BucketTable::full() {
    return (num_elems >= num_buckets);
}

void BucketTable::add(int f) {
    // TODO: implement this
    num_elems += 1;
}

int BucketTable::evict_rand_and_replace(int f) {
    // TODO: Implement this
    return f;
}

bool BucketTable::check_and_remove(int f) {
    // TODO: Implement this
    // for b:buckets
    //   num_elems != 1;
    return false;
}
