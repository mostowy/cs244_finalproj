#include "DLeftCountingBloomFilter.h"
#include <iostream>

// Arbitrary prime number, used as a linear permutation function.
#define PRIME1 9199
#define PRIME2 2221

DLeftCountingBloomFilter::DLeftCountingBloomFilter(
    uint32_t buckets_per_subtable, std::shared_ptr<HashFamily> family)
    : num_buckets_per_subtable_(buckets_per_subtable),
      hash_func_(family->get()) {
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    //permutations_.push_back(family->get());
    subtables_[i].buckets = (struct dlcbf_bucket*)
        calloc(sizeof(struct dlcbf_bucket), num_buckets_per_subtable_);
  }
}

DLeftCountingBloomFilter::~DLeftCountingBloomFilter() {
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    free(subtables_[i].buckets);
  }
}

// Populates `targets` (bucket indexes) and returns the fingerprint (what the
// dlcbf paper calls the "remainder").
uint16_t DLeftCountingBloomFilter::get_targets(
    int data, uint32_t targets[NUM_SUBTABLES]) const {
  uint64_t true_fingerprint = hash_func_(data);
  uint16_t fingerprint = ((uint16_t) true_fingerprint) & REMAINDER_MASK;
  uint32_t true_target = true_fingerprint >> sizeof(uint32_t);
  for (unsigned i = 0; i < NUM_SUBTABLES; i++) {
    //targets[i] = (true_fingerprint * (2*i+1)) % num_buckets_per_subtable_;
    //targets[i] = (hidden_fingerprint * PRIME1 * (i+1) + PRIME2)
    //    % num_buckets_per_subtable_;
    targets[i] = (true_target + i * fingerprint) % num_buckets_per_subtable_;
    // This is actually supposed to be a permutation function...
    //targets[i] = permutations_[i](true_fingerprint) % num_buckets_per_subtable_;
  }
  //uint16_t fingerprint = true_fingerprint & REMAINDER_MASK;

  //std::cout << "Data " << data << " hashes to fp " << +fingerprint
  //          << ", slots: ";
  //for (int i = 0; i < NUM_SUBTABLES; i++) {
  //  std::cout << +targets[i] << " ";
  //}
  //std::cout << "." << std::endl;
  return fingerprint;
}

int DLeftCountingBloomFilter::insert(int data) {
  uint32_t targets[NUM_SUBTABLES];
  std::cout<<"Getting fingerprint"<<std::endl;
  uint16_t fingerprint = get_targets(data, &targets[0]);
  uint8_t min_fill_count = BUCKET_HEIGHT;
  uint8_t best_subtable = 0;
  std::cout<<"Before For loop"<<std::endl;
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    auto *next_bucket = &subtables_[i].buckets[targets[i]];
    uint8_t next_fill_count = 0;
    std::cout<<"Before Second loop"<<std::endl;
    for (int j = 0;
         j < num_buckets_per_subtable_ && next_bucket->cells[j].count > 0;
         j++) {
      if (next_bucket->cells[j].fingerprint == fingerprint) {
        // Already included in the table.
        if (next_bucket->cells[j].count < CELL_COUNT_MAX_VAL) {
          next_bucket->cells[j].count++;
          //std::cout << +next_bucket->cells[j].count << std::endl;
          return 1;
        } else {
          //std::cout << "Cell count overflow." << std::endl;
          return -1;
        }
      }
      next_fill_count++;
    }
    if (i == 0 || next_fill_count < min_fill_count) {
      min_fill_count = next_fill_count;
      best_subtable = i;
    }
  }
  if (min_fill_count >= BUCKET_HEIGHT) {
    //std::cout << "All target buckets full of other fps." << std::endl;
    return -1;
  }
  //std::cout << "Placing data " << data << " in subtable " << +best_subtable
  //          << " bucket " << +targets[best_subtable] << " which was "
  //          << +min_fill_count << " full." << std::endl;
  std::cout<<"Placing data"<<std::endl;
  auto* best_bucket =
      &subtables_[best_subtable].buckets[targets[best_subtable]];
  best_bucket->cells[min_fill_count].fingerprint = fingerprint;
  best_bucket->cells[min_fill_count].count = 1;
  //std::cout << "1" << std::endl;
  return 1;
}

bool DLeftCountingBloomFilter::contains(int data) const {
  uint32_t targets[NUM_SUBTABLES];
  uint16_t fingerprint = get_targets(data, &targets[0]);
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    auto *bucket = &subtables_[i].buckets[targets[i]];
    for (int j = 0;
         j < num_buckets_per_subtable_ && bucket->cells[j].count > 0;
         j++) {
      if (bucket->cells[j].fingerprint == fingerprint) {
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
  uint32_t targets[NUM_SUBTABLES];
  uint16_t fingerprint = get_targets(data, &targets[0]);
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    auto *bucket = &subtables_[i].buckets[targets[i]];
    for (int j = 0;
         j < num_buckets_per_subtable_ && bucket->cells[j].count > 0;
         j++) {
      if (bucket->cells[j].fingerprint == fingerprint) {
        if (bucket->cells[j].count >= 2) {
          bucket->cells[j].count--;
          return;
        }
        // Shift the rest of the bucket over.
        for (int k = j + 1; k < num_buckets_per_subtable_; k++) {
          bucket->cells[k-1].fingerprint = bucket->cells[k].fingerprint;
          bucket->cells[k-1].count = bucket->cells[k].count;
          if (bucket->cells[k].count == 0) break;
          bucket->cells[k].count = 0;
        }
        return;
      }
    }
  }
}
