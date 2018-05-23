#!/usr/bin/env python2.y
import random
from operator import xor


class entry(object):
    def __init__(self, size):
        self.size = size
        self.num_elems = 0
        self.bucket = []

    def full(self):
        return self.num_elems == self.size

    def add(self, fp):
        self.bucket.append(fp)

    def is_in(self, fp):
        return fp in self.bucket

    def evict_rand_and_replace(self, fp):
        ind = random.randint(0, self.size - 1)
        elem = self.bucket[ind]
        self.bucket[ind] = fp
        return elem

class CuckooFilter(object):
    def __init__(self, num_buckets, hash_family, finger_print, max_iters, buck_size):
        self.b1 = [entry(buck_size) for i in range(num_buckets)]
        self.b2 = [entry(buck_size) for i in range(num_buckets)]
        self.h  = hash_family
        self.num_elems = 0
        self.finger_print = finger_print
        self.max_iters = max_iters

    def run_cuckoo_loop(self, f, ind1, ind2):
        counter = random.choice([1, 2])
        ind = ind1 if counter % 2 != 0 else ind2
        for j in range(self.max_iters):
            if(counter % 2 == 0):
                if !self.b1[ind].full():
                    self.b1[ind].add(f)
                    return 1
                f = self.b1[ind].evict_rand_and_repace(f)
            else:
                if !self.b2[ind].full():
                    self.b2[ind].add(f)
                    return 1
                f = self.b2[ind].evict_rand_and_repace(f)
            ind = mod(ind, self.h(f))
            counter += 1
       return -1
    
    def add(self, data):
        f = self.finger_print(data)
        ind1 = self.h(data)
        ind2 = xor(ind1, self.h(f))
        self.num_elems += 1
        if self.b1[ind1].is_in(f) or self.b2[ind2].is_in(f):
            return 1
        if not self.b1[ind1].full():
            self.b1[ind1].add(f)
            return 1
        if not self.b2[ind2].full():
            self.b2[ind2].add(f)
            return 1
        return self.run_cuckoo_loop(f, ind1, ind2)


    def contains(self, data):
        f = self.finger_print(data)
        ind1 = self.h(data)
        ind2 = xor(ind1, f)
        if data in self.b1[ind1]:
            return True
        return (data in self.b2[ind2])

    def remove(self, data):
        f = self.finger_print(data)
        ind1 = self.h(data)
        ind2 = xor(ind1, f)
        if self.b1[ind1] == data:
            self.b1[ind1].index(data) = 0
        elif self.b2[ind2] == data:
            self.i2[ind2].index(data) = 0
