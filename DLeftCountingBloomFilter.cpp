#include "DLeftCountingBloomFilter.h"
#include <iostream>

// Arbitrary prime number, used as a linear permutation function.
#define PRIME1 9199
#define PRIME2 2221

DLeftCountingBloomFilter::DLeftCountingBloomFilter(
    uint32_t buckets_per_subtable, std::shared_ptr<HashFamily> family)
    : num_buckets_per_subtable_(buckets_per_subtable),
      hash_func_(family->get()),
      num_inserted_(0) {
  for (int i = 0; i < NUM_SUBTABLES; i++) {
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
    uint64_t data, uint32_t targets[NUM_SUBTABLES]) const {
  uint64_t true_fingerprint = hash_func_(data);
  uint16_t fingerprint = ((uint16_t) true_fingerprint) & REMAINDER_MASK;
  uint32_t true_target = true_fingerprint >> (8 * sizeof(uint32_t));

  for (uint32_t i = 0; i < NUM_SUBTABLES; i++) {
    //targets[i] = (true_fingerprint * (2*i+1)) % num_buckets_per_subtable_;
    //targets[i] = (hidden_fingerprint * PRIME1 * (i+1) + PRIME2)
    //    % num_buckets_per_subtable_;
    targets[i] =
        (true_target + (i*2+1) * fingerprint) % num_buckets_per_subtable_;
  }

  //std::cout << "Data " << data << " hashes to fp " << +fingerprint
  //          << ", slots: ";
  //for (int i = 0; i < NUM_SUBTABLES; i++) {
  //  std::cout << +targets[i] << " ";
  //}
  //std::cout << "." << std::endl;
  return fingerprint;
}

int DLeftCountingBloomFilter::insert(uint64_t data) {
  uint32_t targets[NUM_SUBTABLES];
  uint16_t fingerprint = get_targets(data, targets);
  uint8_t min_fill_count = BUCKET_HEIGHT;
  uint8_t best_subtable = 0;
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    auto *next_bucket = &subtables_[i].buckets[targets[i]];
    uint8_t next_fill_count = 0;
    for (int j = 0;
         j < num_buckets_per_subtable_ && next_bucket->cells[j].count > 0;
         j++) {
      if (next_bucket->cells[j].fingerprint == fingerprint) {
        // Already included in the table.
        if (next_bucket->cells[j].count < CELL_COUNT_MAX_VAL) {
          next_bucket->cells[j].count++;
          //std::cout << +next_bucket->cells[j].count << std::endl;
          num_inserted_++;
          return 1;
        } else {
          //std::cout << "STOP: Cell count overflow." << std::endl;
          //std::cout << "(was inserting data " << data << ", fp " << +fingerprint
          //          << " b-indexes";
          //for (int i = 0; i < NUM_SUBTABLES; i++) {
          //  std::cout << " " << targets[i];
          //}
          //std::cout << ")" << std::endl;
          //dump_occupancy();
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
    //std::cout << "STOP: All target buckets full of other fps." << std::endl;
    //std::cout << "(was inserting data " << data << ", fp " << +fingerprint
    //          << " b-indexes";
    //for (int i = 0; i < NUM_SUBTABLES; i++) {
    //  std::cout << " " << targets[i];
    //}
    //std::cout << ")" << std::endl;
    //dump_occupancy();
    return -1;
  }
  //std::cout << "Placing data " << data << " in subtable " << +best_subtable
  //          << " bucket " << +targets[best_subtable] << " which was "
  //          << +min_fill_count << " full." << std::endl;
  auto* best_bucket =
      &subtables_[best_subtable].buckets[targets[best_subtable]];
  best_bucket->cells[min_fill_count].fingerprint = fingerprint;
  best_bucket->cells[min_fill_count].count = 1;
  //std::cout << "1" << std::endl;
  num_inserted_++;
  return 1;
}

bool DLeftCountingBloomFilter::contains(uint64_t data) const {
  uint32_t targets[NUM_SUBTABLES];
  uint16_t fingerprint = get_targets(data, targets);
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    auto *bucket = &subtables_[i].buckets[targets[i]];
    for (size_t j = 0;
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

void DLeftCountingBloomFilter::remove(uint64_t data) {
  uint32_t targets[NUM_SUBTABLES];
  uint16_t fingerprint = get_targets(data, targets);
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    auto *bucket = &subtables_[i].buckets[targets[i]];
    for (size_t j = 0;
         j < num_buckets_per_subtable_ && bucket->cells[j].count > 0;
         j++) {
      if (bucket->cells[j].fingerprint == fingerprint) {
        if (bucket->cells[j].count >= 2) {
          bucket->cells[j].count--;
          return;
        }
        // Shift the rest of the bucket over.
        for (size_t k = j + 1; k < num_buckets_per_subtable_; k++) {
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

void DLeftCountingBloomFilter::dump_occupancy() const {
  std::cout << "Successful insert() calls: " << num_inserted_ << std::endl;
  std::cout << "Overall cell occupancy: "
            << (1.0 * num_inserted_ /
                (NUM_SUBTABLES * num_buckets_per_subtable_ * BUCKET_HEIGHT))
            << std::endl;
  for (int i = 0; i < NUM_SUBTABLES; i++) {
    for (int j = 0; j < num_buckets_per_subtable_; j++) {
      auto *bucket = &subtables_[i].buckets[j];
      uint8_t counts[BUCKET_HEIGHT];
      uint8_t count = 0;
      for (int k = 0; k < BUCKET_HEIGHT; k++) {
        counts[k] = bucket->cells[k].count;
        if (counts[k] > 0) count++;
      }
      std::cout << "t" << i << " b" << j << ": " << +count;
      for (int k = 0; k < BUCKET_HEIGHT; k++) {
        if (counts[k] > 0) {
          std::cout << " " << +(bucket->cells[k].fingerprint)
                    << "x" << +(bucket->cells[k].count) << ",";
        }
      }
      std::cout << std::endl;
    }
  }
}
