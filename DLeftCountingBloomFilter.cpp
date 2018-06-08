#include "DLeftCountingBloomFilter.h"
//#include <iostream>

DLeftCountingBloomFilter::DLeftCountingBloomFilter(size_t unused_size_in_bits,
                           std::shared_ptr<HashFamily> family)
    : hash_func_(family->get()) {
  subtables_ = (struct subtable*) calloc(sizeof(struct subtable),
                                         NUM_SUBTABLES);
}

DLeftCountingBloomFilter::~DLeftCountingBloomFilter() {
  free(subtables_);
}

// Populates `targets` (bucket indexes) and returns the fingerprint (what the
// dlcbf paper calls the "remainder").
uint16_t DLeftCountingBloomFilter::get_targets(
    int data, uint16_t targets[NUM_SUBTABLES]) const {
  uint32_t true_fingerprint = hash_func_(data);
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    targets[i] = (true_fingerprint * (2*i+1)) % NUM_BUCKETS_PER_SUBTABLE;
  }
  uint16_t fingerprint = true_fingerprint & REMAINDER_MASK;
  //std::cout << "Data " << data << " hashes to fp " << +fingerprint
  //          << ", slots: ";
  //for (int i = 0; i < NUM_SUBTABLES; i++) {
  //  std::cout << +targets[i] << " ";
  //}
  //std::cout << "." << std::endl;
  return fingerprint;
}

int DLeftCountingBloomFilter::insert(int data) {
  uint16_t targets[NUM_SUBTABLES];
  uint16_t fingerprint = get_targets(data, &targets[0]);
  uint8_t min_fill_count = BUCKET_HEIGHT;
  uint8_t best_subtable = 0;
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    auto *next_bucket = &subtables_[i].buckets[targets[i]];
    for (int j = 0; j < next_bucket->fill_count; j++) {
      if (next_bucket->fingerprints[j] == fingerprint) {
        // Already included in the table.
        return 1;
      }
    }
    if (i == 0 || next_bucket->fill_count < min_fill_count) {
      min_fill_count = next_bucket->fill_count;
      best_subtable = i;
    }
  }
  if (min_fill_count >= BUCKET_HEIGHT) {
    return -1;
  }
  //std::cout << "Placing data " << data << " in subtable " << +best_subtable
  //          << " bucket " << +targets[best_subtable] << " which was "
  //          << +min_fill_count << " full." << std::endl;
  auto* best_bucket =
      &subtables_[best_subtable].buckets[targets[best_subtable]];
  best_bucket->fingerprints[min_fill_count] = fingerprint;
  best_bucket->fill_count++;
  return 1;
}

bool DLeftCountingBloomFilter::contains(int data) const {
  uint16_t targets[NUM_SUBTABLES];
  uint16_t fingerprint = get_targets(data, &targets[0]);
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    auto *bucket = &subtables_[i].buckets[targets[i]];
    for (int j = 0; j < bucket->fill_count; j++) {
      if (bucket->fingerprints[j] == fingerprint) {
        //std::cout << "Found data " << data << " in subtable " << i
        //    << " bucket " << targets[i] << " slot "
        //    << j << " (" << +bucket->fill_count << " full)." << std::endl;
        return true;
      }
    }
  }
  return false;
}

void DLeftCountingBloomFilter::remove(int data) {
  uint16_t targets[NUM_SUBTABLES];
  uint16_t fingerprint = get_targets(data, &targets[0]);
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    auto *bucket = &subtables_[i].buckets[targets[i]];
    for (int j = 0; j < bucket->fill_count; j++) {
      if (bucket->fingerprints[j] == fingerprint) {
        // Shift the rest of the bucket over.
        for (int k = j + 1; k < bucket->fill_count; k++) {
          bucket->fingerprints[k-1] = bucket->fingerprints[k];
        }
        bucket->fill_count--;
        return;
      }
    }
  }
}
