#include "SemiSortTable.h"
#include <stddef.h>
#include <stdlib.h>
#include <set>
#include <vector>
#include <iostream>
#include <math.h>
#include <bitset>
#include "SSPerm.h"



SemiSortTable::SemiSortTable(size_t numBuckets, size_t f_size): perm() {
    
    num_buckets = numBuckets;
    bits_per_entry = (f_size - 4);
    bits_per_bucket = (4 * bits_per_entry) + 12;
    bytes_per_bucket = (bits_per_bucket + 7) >> 3;
    len_bytes = bytes_per_bucket * numBuckets;
    buckets = new buck_struct[num_buckets];
    //entry_mask = pow(2, bits_per_entry) - 1; 
    for(size_t i = 0; i < num_buckets; i++) {
        buckets[i].index = 0;
        buckets[i].e1 = 0;
        buckets[i].e2 = 0;
        buckets[i].e3 = 0;
        buckets[i].e4 = 0;
    }
}

SemiSortTable::~SemiSortTable() {
    delete buckets;
}

bool SemiSortTable::has(uint32_t f, int i1, int i2) const{
    return check_val(f, i1) || check_val(f, i2);

}

bool SemiSortTable::full(int i1) {
    uint32_t entries[4];
    readInEntries(i1, entries);
    for(size_t i = 0; i < 4; i++){
        if(entries[i] == 0)
            return false;
    }
    return true;
}

bool SemiSortTable::add(uint32_t f, int i1, uint32_t &oldf) {
    uint32_t entries[4];
    readInEntries(i1, entries);
    //std::cout<<"ADDING: "<<f<<std::endl;
    //for(int i = 0; i < 4; i++){
    //    std::cout<<"Entry "<<i<<": "<<entries[i]<<std::endl;
    //}
    
    for(size_t i = 0; i < 4; i++) {
      if(entries[i] == 0){
        entries[i] = f;
        writeOutEntry(i1, entries);
        return true;
      }
    }
    //std::cout<<"Outta loop"<<std::endl;
    int ind = rand() % 4;
    oldf = entries[ind];
    entries[ind] = f;
    writeOutEntry(i1, entries);
    return false;
}

int SemiSortTable::evict_rand_and_replace(uint32_t f) {
    return 0;
}

bool SemiSortTable::check_and_remove(uint32_t f, int i1) {
    uint32_t entries[4];
    readInEntries(i1, entries);
    for(int i = 0; i < 4; i++) {
        if(entries[i] == f){
            entries[i] = 0;
            writeOutEntry(i1, entries);
            return true;
        }
    }
    return false;
}


// Helper methods
bool SemiSortTable::check_val(uint32_t f, int i) const{
    uint32_t entries[4];
    readInEntries(i, entries);
    //std::cout<<"IN CHECK VAL"<<std::endl;
    for(int j = 0; j < 4; j++) {
      //cstd::cout<<"Entry Number"<<j<<": "<<entries[j]<<std::endl;
      if(f == entries[j])
          return true;
    }
    return false;
}

void SemiSortTable::readInEntries(int i1, uint32_t entries[4]) const{
    buck_struct b = buckets[i1];
    uint16_t index = b.index;
    //std::cout<<"Read in index: "<<index<<std::endl;
    uint16_t nums = perm.decode(index);
    //std::cout<<"Read in nums val: "<<nums<<std::endl;
    //std::bitset<16> x(nums);
    //std::cout<<"Read in nums bits: "<<x<<std::endl;
    
    uint32_t mask = 0x0000000F;
    for(size_t j = 0; j < 4; j++) {
        uint32_t val = 0;

        // First get low bits
        val = val + (mask & (nums>>((3 - j)*4)));
        //std::cout<<"VAL: "<<val<<std::endl; 
        uint32_t high_bits = 0;
        switch(j){
            case 0: high_bits = b.e1;
                    break;
            case 1: high_bits = b.e2;
                    break;
            case 2: high_bits = b.e3;
                    break;
            case 3: high_bits = b.e4;
                    break;
        }
        high_bits = high_bits << 4;
        val = val + high_bits;
        entries[j] = val;
        /*// Now get high bits
        size_t added_bytes = 0;

        // Number of bits to shift forward by.  Must be less than
        size_t forward_shift = 12 + (bits_per_entry * j);
        while((forward_shift + bits_per_entry) > 64){
            added_bytes += 1;
            forward_shift -= 8;
        }

        uint64_t high_num = bits_mask & *(buckets + (bytes_per_bucket*i1) + added_bytes);
        // First shift bits forward to get high bits at end of num
        high_num = high_num << forward_shift;
        // Now shift bits back to get bits at beginning of num
        high_num = high_num >> (64 - bits_per_entry);
        // Now mask off leading 32 bits
        uint32_t t_high_num = high_num & 0xffffffff;
        // Now shift forward by 4
        val = val + (t_high_num << 4);
        entries[j] = val;
        */
    }
    //std::cout<<"IN READ"<<std::endl;
    //for(int i = 0; i < 4; i++) {
    //    std::cout<<"Entry Number"<<i<<": "<<entries[i]<<std::endl;
    //}
}

void SemiSortTable::swapEntries(uint32_t& e1, uint32_t& e2){
    if((e1 & 0x0000000F) < (e2 & 0x0000000F)){
        std::swap(e1, e2);
    }
}

void SemiSortTable::sortEntries(uint32_t* entries) {
    swapEntries(entries[0], entries[1]);
    swapEntries(entries[2], entries[3]);
    swapEntries(entries[0], entries[2]);
    swapEntries(entries[1], entries[3]);
    swapEntries(entries[1], entries[2]);
}

uint8_t SemiSortTable::getLowOrderBits(uint32_t e) {
    uint8_t temp =  e & 0x0F;
    return temp;
}


void SemiSortTable::writeOutEntry(int i1, uint32_t entries[4]) {
    // First sort the entries
    sortEntries(entries);
    //std::cout<<"IN WRITE"<<std::endl;
    //for(int i = 0; i < 4; i++) {
    //    std::cout<<"Entry Number"<<i<<": "<<entries[i]<<std::endl;
    //}
    // Then write out low bits
    uint8_t lows[2];
    //std::cout<<"Low bits of first: "<<(entries[0] & 0x0F)<<std::endl;
    uint8_t first = 0;
    uint8_t second = 0;
    first = first | ((entries[0] & 0x0F) << 4);
    first = first | ((entries[1] & 0x0F));
    second = second | ((entries[2] & 0x0F) << 4);
    second = second | ((entries[3] & 0x0F));
    lows[0] = first;
    lows[1] = second;
    std::bitset<8> x((first));
    //std::bitset<8> y(second);
    //std::cout<<"First: "<<first<<std::endl;
    //std::cout<<"Second: "<<second<<std::endl;
    //std::cout<<"Bitset first: "<<x<<std::endl;
    //std::cout<<"Bitset second: "<<y<<std::endl;
    //std::cout<<"IN WRITE"<<std::endl;
    
    uint16_t low_index = perm.encode(lows);
    //std::bitset<16> b(low_index);
    //std::cout<<"index bitset: "<<b<<std::endl;
    buckets[i1].index = low_index;
    //std::cout<<"Low index: "<<low_index<<std::endl;
    //std::cout<<"Low index: "<<buckets[i1].index<<std::endl;
    for(int i = 0; i < 4; i++) {
        //uint32_t high_bits = (entries[i] >> 4);
        switch(i) {
            case 0: buckets[i1].e1 = ((entries[i] >> 4));
                    break;
            case 1: buckets[i1].e2 = ((entries[i] >> 4));
                    break;
            case 2: buckets[i1].e3 = ((entries[i] >> 4));
                    break;
            case 3: buckets[i1].e4 = ((entries[i] >> 4));
                    break;
            default: continue;
        }
    }
    //std::cout<<"High Order Bits: "<<buckets[i1].e1<<std::endl;
    //std::cout<<"High Order Bits Shifted: "<<(buckets[i1].e1 << 4)<<std::endl;
    
    /*char writeouts[bytes_per_bucket];
    int index = 3;
    uint32_t curr_entry = entries[index];
    size_t bits_left_in_entry = bits_per_entry;
    size_t overhang = bits_per_bucket -(8*bytes_per_bucket);*/
    /*for(int i = bytes_per_bucket - 1; i >= 0; i--){
        char buf = 0;
        size_t buf_written = 0;
        if(overhang > 0) {
            buf_written = overhang;
            overhang = 0;
        }
        while(buf_written < 8) {
            if(buf_written + bits_left_in_entry <= 8) {
                char temp = ((curr_entry << buf_written) & 0xFF);
                buf = buf | temp;
                buf_written += bits_left_in_entry;
                index -= 1;
                if(index < 0) {
                    curr_entry = low_index;
                    bits_left_in_entry = 12;
                } else {
                    curr_entry = entries[index];
                    bits_left_in_entry = bits_per_entry;
                }
            } else {
                // First write out remaining buf
                size_t write_out = 8 - buf_written;
                char temp = ((curr_entry << buf_written) & 0xFF);
                buf = buf | temp;
                buf_written += write_out;

                // Now update curr entry to take off written bits
                curr_entry = (curr_entry >> write_out);
                bits_left_in_entry -= write_out;
            }
        }
        writeouts[i] = buf;
    }
    for(int i = 0; i < bytes_per_bucket; i++) {
        //std::bitset<8> z(writeouts[i]);
        //std::cout<<"Writeouts: "<<z<<std::endl;
        memcpy(buckets + (bytes_per_bucket*i1) + i, &writeouts[i],1);
        //std::bitset<8>a(*(buckets + (bytes_per_bucket*i1) + i));
        //std::cout<<"In Mem: "<<a<<std::endl;
    }

    uint32_t temp_ents[4];
    std::cout<<"In Read In Entries"<<std::endl;
    readInEntries(i1, temp_ents);
    std::cout<<"Entries: "<<std::endl;
    for(int i = 0; i < 4; i++) {
        std::cout<<entries[i]<<std::endl;
    } 
    std::cout<<"Temp Entries: "<<std::endl;
    for(int i = 0; i < 4; i++) {
        std::cout<<temp_ents[i]<<std::endl;
    }*/
}
