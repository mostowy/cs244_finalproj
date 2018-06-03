#include <iostream>

#include "Hashes.h"
#include "BlockedBloomFilter.h"
#include "BloomFilter.h"
#include "CuckooFilter.h"
#include "Timing.h"
#include "QuotientFilter.h"

int main() {
  /* A list of all true families of hash functions (that is, hash families
   * where we can sample as many hash functions as we need.) These hash
   * families can be used in any hash table type.
   */
  auto allHashFamilies  = {
    //twoIndependentHashFamily(),
    //threeIndependentHashFamily(),
    fiveIndependentHashFamily()
    //tabulationHashFamily()
  };
  
  /* A list of all types of hash functions available, including families with
   * just a single hash function in them. These are suitable for everything
   * except second-choice hashing and cuckoo hashing.
   */
  /*auto allHashFunctions = {
    twoIndependentHashFamily(),
    threeIndependentHashFamily(),
    fiveIndependentHashFamily(),
    tabulationHashFamily(),
    identityHash(),
    jenkinsHash()
  };*/

  std::cout << "Sanity Checks" << std::endl;

  std::cout << "  Hash functions are different:     ";
  {
    auto family = fiveIndependentHashFamily();
    HashFunction h1 = family->get();
    HashFunction h2 = family->get();
    std::cout << (h1(4) != h2(4) ? "Pass" : "fail") << std::endl;
  }

  std::cout << "  Bloom filter basic functionality: ";
  {
    BloomFilter bf(1200, fiveIndependentHashFamily());
    bf.insert(5);
    bf.insert(2400);
    if (!bf.contains(10) && bf.contains(5) && bf.contains(2400)) {
      std::cout << "Pass";
    } else {
      std::cout << "fail";
    }
    std::cout << std::endl;
  }

  std::cout << "  Blocked Bloom filter basic functionality: ";
  {
    BlockedBloomFilter bbf(1200, fiveIndependentHashFamily());
    bbf.insert(5);
    bbf.insert(2400);
    if (!bbf.contains(10) && bbf.contains(5) && bbf.contains(2400)) {
      std::cout << "Pass";
    } else {
      std::cout << "fail";
    }
    std::cout << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Correctness Tests" << std::endl;
  std::cout << "  Bloom:          " << (checkCorrectness<BloomFilter>(allHashFamilies) ? "Pass" : "fail") << std::endl;
  std::cout << "  Blocked Bloom:  " << (checkCorrectness<BlockedBloomFilter>(allHashFamilies) ? "Pass" : "fail") << std::endl;
  std::cout << "  Quotient:       " << (checkCorrectness<QuotientFilter>(allHashFamilies) ? "Pass" : "fail") << std::endl;
  //std::cout << "  Cuckoo:         " << (checkCorrectness<CuckooFilter>(allHashFamilies) ? "pass" : "fail") << std::endl;
  std::cout << std::endl;

  /* Test linear probing variants. */
  /**
  auto probingLoadFactors = {0.3, 0.5, 0.7, 0.9, 0.99};

  std::cout << "#### Timing Linear Probing ####" << std::endl;
  doAllReports<LinearProbingHashTable>(allHashFunctions, probingLoadFactors);
  std::cout << "###########################" << std::endl;
  std::cout << std::endl;
  
  std::cout << "#### Timing Robin Hood ####" << std::endl;
  doAllReports<RobinHoodHashTable>(allHashFunctions, probingLoadFactors);
  std::cout << "###########################" << std::endl;
  std::cout << std::endl;
  
  */
  /* Test chained hashing variants. */
  /*
  auto chainedLoadFactors = {0.3, 0.5, 0.7, 0.9, 0.99, 2.0, 5.00};
  
  std::cout << "#### Timing Chained ####" << std::endl;
  doAllReports<ChainedHashTable>(allHashFunctions, chainedLoadFactors);
  std::cout << "###########################" << std::endl;
  std::cout << std::endl;
  
  std::cout << "#### Timing Second-Choice ####" << std::endl;
  doAllReports<SecondChoiceHashTable>(allHashFamilies, chainedLoadFactors);
  std::cout << "###########################" << std::endl;
  std::cout << std::endl;
  */
  /* Test cuckoo hashing. */
  /*auto cuckooLoadFactors = {0.2, 0.3, 0.4, 0.45, 0.47};
  
  std::cout << "#### Timing Cuckoo Hashing ####" << std::endl;
  doAllReports<CuckooHashTable>(allHashFamilies, cuckooLoadFactors);
  std::cout << "###########################" << std::endl;
  std::cout << std::endl;
  */
  }
