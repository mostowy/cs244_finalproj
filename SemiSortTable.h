#ifndef SemiSortTable_Included
#define SemiSortTable_Included

#include <stddef.h>
#include <vector>
#include "SSPerm.h"

/* TWO BIG ASSUMPTIONS:
 *      1) b = 4.  As in 4 values per bucket.
 *      2) 0 is not hashed.  If a bucket has
 *         a value of 0, it is considered
 *         empty.
 * */

class SemiSortTable{
    public:
        /**
         * Constructor
         */
        SemiSortTable(size_t numBuckets, size_t f_size);

        /**
         * Deconstructor
         */
        ~SemiSortTable();

        /**
         * Function returns true if table has finger print
         * false otherwise. 
         */
        bool has(uint32_t f, int i1, int i2) const;

        /**
         * Function returns if buckets table is full
         */
        bool full(int i1);

        /**
         * Function adds given finger print
         */
        bool add(uint32_t f, int i1, uint32_t &oldf);

        /**
         * Function picks a random element, replaces it 
         * with the given finger print, and returns the
         * evicted finger print
         */
        int evict_rand_and_replace(uint32_t f);

        /**
         * Function checks if given element is in the table,
         * removes it if it is, and returns whether or not
         * the element was removed.
         */
        bool check_and_remove(uint32_t f, int i1);

        /**
         * Function checks if the value f is in the index
         * i.
         */
        bool check_val(uint32_t f, int i) const;


        /**
         * Function reads in a bucket index and places
         * the finger prints in the entries array passed in.
         */
        void readInEntries(int i1, uint32_t entries[4]) const;

        /**
         * Function writes out the finger print to the given
         * bucket.  It evicts the bucket index given.
         */
        void writeOutEntry(int i1, uint32_t entries[4]);
        void sortEntries(uint32_t* entries);
        void swapEntries(uint32_t& e1, uint32_t& e2);
        uint8_t getLowOrderBits(uint32_t e);

        struct buck_struct{
            unsigned index:12;
            unsigned e1:8;
            unsigned e2:8;
            unsigned e3:8;
            unsigned e4:8;
        };

    private:
        size_t num_buckets;      // Number of buckets in the table
        size_t bits_per_entry;   // Number of bits per entry
        size_t bits_per_bucket;  // Number of bits per bucket
        size_t bytes_per_bucket; // Number of bytes per bucket
        buck_struct* buckets;           // Pointer to buckets array
        size_t len_bytes;        // Length of array in bytes
        SSPerm perm;             // Perm table for semi sort
        //uint32_t bits_mask;      // used to mask of high bits 
        
        
        //SemiSortTable(SemiSortTable const &) = delete;
        void operator=(SemiSortTable const &) = delete;
};

#endif
