#include <stdint.h>
#include <iostream>
#include "SSPerm.h"
#include <bitset>

SSPerm::SSPerm() {
    uint16_t index = 0;
    for(uint16_t i = 0; i < 16; i++){
        for(uint16_t j = 0; j <= i; j++){
            for(uint16_t k = 0; k <= j; k++) {
                for(uint16_t l = 0; l <=k; l++){
                    uint16_t ent = 0;
                    ent = ent | (i << 12);
                    ent = ent | (j << 8);
                    ent = ent | (k << 4);
                    ent = ent | l;
                    enc_table[index] = ent;
                    dec_table[ent] = index;
                    index += 1;
                }
            }
        }
    }
}

SSPerm::~SSPerm() {
    // Nothing to clean
}

uint16_t SSPerm::encode(const uint8_t entries[2]) {
    uint16_t ent = 0;
    ent = ent | (entries[0] << 8);
    ent = ent | entries[1];
    return dec_table[ent];
}

uint16_t SSPerm::decode(uint16_t index) const{
    return enc_table[index];

}
