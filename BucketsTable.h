#ifndef BucketsTable_Included
#define BucketsTable_Included

#include <stddef.h>
#include <vector>

class BucketsTable{
    public:
        /**
         * Constructor
         */
        BucketsTable(size_t numBuckets);

        /**
         * Deconstructor
         */
        ~BucketsTable();

        /**
         * Function returns true if table has finger print
         * false otherwise. 
         */
        bool has(uint32_t f, int ind1, int ind2) const;

        /**
         * Function returns if bucket at given index is
         * full
         */
        bool full(int ind);

        /**
         * Function adds given finger print to given
         * index
         */
        void add(uint32_t f, int ind);

        /**
         * Function picks a random element, replaces it 
         * with the given finger print, and returns the
         * evicted finger print
         */
        uint32_t evict_rand_and_replace(uint32_t f, int ind);

        /**
         * Function checks if given element is in the table,
         * removes it if it is, and returns whether or not
         * the element was removed.
         */
        bool check_and_remove(uint32_t f, int ind);

        /**
         * Function returns true if the given index has
         * the element f in it.
         */
        bool check_val(uint32_t f, int ind) const;
        
        /**
         * Yes I know this is kind of hacky but this will
         * be a bucket.  Set the bit field to be the
         * finger print size.
         */
        struct buck_struct{
            unsigned e1:12;
            unsigned e2:12;
            unsigned e3:12;
            unsigned e4:12;
        };

    private:
        size_t num_elems;
        size_t num_buckets;
        int f_size;
        struct buck_struct* buckets;
        //BucketsTable(BucketsTable const &) = delete;
        void operator=(BucketsTable const &) = delete;
};

#endif
