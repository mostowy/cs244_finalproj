#ifndef Timing_Included
#define Timing_Included

#include "Hashes.h"
#include "Timer.h"
#include <climits>
#include <random>
#include <tuple>
#include <memory>
#include <initializer_list>
#include <unordered_set>
#include <iostream>
#include <iomanip>

/* The random seed used throughout the run. */
static const size_t kRandomSeed = 138;

/* Multiplier used to determine the maximum integer that can be queried on an
 * operation. Higher numbers make for more likely misses in lookups.
 */
static const size_t kSpread = 4;

static const size_t bucket_val = 4;

uint64_t* keys = NULL;
const size_t num_rows = (1 << 25);
const size_t num_keys = num_rows * 4;

void init_keys() {

  keys = new uint64_t[num_keys];

  /* Seed */
  std::random_device rd;

  /* Random number generator */
  std::default_random_engine generator(rd());

  /* Distribution on which to apply the generator */
  std::uniform_int_distribution<long long unsigned> distribution(0,0xFFFFFFFFFFFFFFFF);

  for (int i = 0; i < num_keys; i++) {
      keys[i] = distribution(generator);
  }
  std::cout<<"Randomly generated "<<num_keys<<" keys."<<std::endl;  
}


/**
 * Run insert benchmark test.  Filter type indicates the type of filter.
 * By default it is cuckoo, set 1 for Quotient Filter, set 2 for bloom.
 */
template <typename HT>
void time_inserting(int filter_type, std::shared_ptr<HashFamily> family) {
  std::cout<<"Getting insert performance"<<std::endl;
  init_keys();
  size_t bucket_size = num_rows;
  switch(filter_type) {
    case 1: bucket_size = (1 << 27);
            break;
    default: break;
  }
 
  HT table(bucket_size, family);
  size_t i = 0;
  Timer insertTimer;
  insertTimer.start();
  while((i < num_keys)) {
    if(table.insert(keys[i]) == -1)
        break;
    i++;
  }
  insertTimer.stop();
  auto time = insertTimer.elapsed() * 1000000;
  std::cout<<"Inserting done after "<<i<<" elems in "<<time<<" seconds "<<std::endl;
  // do info stuf
  time_lookup(filter_type, family, table);
}

/**
 * Run lookup benchmark test.  Filter type indicates the type of filter.
 * By default it is cuckoo, set 1 for Quotient Filter, set 2 for bloom.
 */
template <typename HT>
void time_lookup(int filter_type, std::shared_ptr<HashFamily> family, HT table){
    std::cout<<"Getting lookup performance"<<std::endl;
    float pos_frac = .5;
    size_t max_queries = 10000000;
    auto queries = new uint64_t[max_queries];
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<long long unsigned> distribution(0,0xFFFFFFFFFFFFFFFF);
    for (size_t i = 0; i < max_queries; i++) {
        float  r = (double) rand() / RAND_MAX;
        if (r  > pos_frac) {
            queries[i] = distribution(generator);
        } else {
            // this is a positive query
            size_t j = rand() % num_keys;
            queries[i] = keys[j];
        }
    }

    size_t false_ops  = 0;
    size_t total_ops = 0;
    double total_time = 0;
    while (total_time < 10) 
    {

        Timer querytimer;
        querytimer.start();
        for (size_t j = 0; j < max_queries; j++) {
            int res = (table.contains(queries[j]) == true);
            false_ops += res;
        }
        querytimer.stop();

        auto time = querytimer.elapsed();
        total_time += time * 1000000;
        total_ops += max_queries;
    }

    std::cout <<  "[lookup] complete querying " << total_ops << " keys in " << total_time <<"sec\n";
    printf("[lookup] %.2f M queries / second,  %.2f ns,  f.p.r. = %f\n",
           ((double)(total_ops)/(total_time *1000000)),
           ((double) total_time) * 1000000000 / total_ops,
           1.0 * false_ops / total_ops);
}


/**
 * Check correctness, using C++'s unordered_set type as an oracle
 */
template <typename HT>
bool checkCorrectness(size_t buckets, std::shared_ptr<HashFamily> family, size_t numActions) {
  //std::default_random_engine engine;
  std::mt19937 engine;
  engine.seed((uint64_t) rand());
  //engine.seed(kRandomSeed);
  auto gen = std::uniform_int_distribution<int>(0, INT_MAX);//numActions * kSpread);
  HT table(buckets, family);
  std::unordered_set<int> reference;
  
  double total = 0;
  double true_negs = 0;
  double false_pos = 0;
  std::cout<<"Num Buckets: "<<buckets<<std::endl;
  while(true) {
    if(std::fmod(total, 1000) == 0)
        std::cout<<"Elem number: "<<total<<std::endl;
    uint32_t value = gen(engine);
    //std::cout<<"Inserting value: "<<value<<std::endl;
    int val = table.insert(value);
    if(val == -1){
        break;
    }
    reference.insert(value);
    if ((reference.count(value) > 0) && !table.contains(value)) {
      true_negs += 1;
      //std::cout<<"TRUE NEG"<<std::endl;
      //exit(0);
    }
    value = gen(engine);
    if((reference.count(value) <= 0) && table.contains(value)) {
      false_pos += 1;
    }
    total += 1;
    //exit(0);
  }
  std::cout<<"Filter Full After "<< total << " Elems."<<std::endl;
  std::cout<<"Filter had "<<false_pos<<" false positive and "<<true_negs<<" true negatives."<<std::endl;
  std::cout<<"False Positive Rate: "<<(false_pos/ total)<<std::endl;
  std::cout<<"True Negative  Rate: "<<(true_negs/ total)<<std::endl;
  //exit(0);
  return true;
}

template <typename HT>
bool checkCorrectness(std::initializer_list<std::tuple<int, std::shared_ptr<HashFamily>, int>> params) {
  for (auto param : params) {
    if (!checkCorrectness<HT>(std::get<0>(param), std::get<1>(param), std::get<2>(param))) {
      return false;
    }
  }
  return true;
}

template <typename HT>
bool checkCorrectness(std::initializer_list<std::shared_ptr<HashFamily>> families) {
  for (auto family: families) {
    if (!checkCorrectness<HT>({
          std::make_tuple(12, family, 5),
            std::make_tuple(120, family, 50),
            std::make_tuple(1200, family, 5000)
            })) {
      return false;
    }
  }
  return true;
}

#endif
