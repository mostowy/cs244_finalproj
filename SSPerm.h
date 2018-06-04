#ifndef SSPerm_Included
#define SSPerm_Included

#include <stdint.h>
#include <stddef.h>

class SSPerm{
    public:
        /**
         * Constructor
         */
        SSPerm();

        /**
         * Deconstructor
         */
        ~SSPerm();

        /**
         * Function encodes the two chars and returns
         * the index.  The chars will store the 16
         * bits of the four tags, the first two in
         * the first char and second two in the second
         * char.
         */
        uint16_t encode(const uint8_t entries[2]);


        /**
         * Function takes in an index and returns
         * the 16 bit representation as a uint16_t.
         */ 
        uint16_t decode(uint16_t index) const;


    private:

        uint16_t enc_table[3876];
        uint16_t dec_table[1<<16];

        //SSPerm(SemiSortTable const &) = delete;
        void operator=(SSPerm const &) = delete;
};

#endif
