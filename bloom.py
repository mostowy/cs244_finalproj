#!/usr/bin/env python2.7

import hashlib


class BitVector(object):
  def __init__(self, size_in_bits):
    self._size_in_bits = size_in_bits
    full_bytes_used, leftover = self._decode_index(size_in_bits)
    size_in_bytes = full_bytes_used + (1 if leftover else 0)
    self._vector = bytearray(size_in_bytes)

  def getitem(self, bit_index):
    byte_index, offset = self._decode_index(bit_index)
    return (self._vector[byte_index] & (1 << offset)) and 1 or 0
    
  def setitem(self, bit_index, value):
    byte_index, offset = self._decode_index(bit_index)
    if value:
      self._vector[byte_index] |= (1 << offset)
    else:
      self._vector[byte_index] &= 255 - (1 << offset)
    
  def _decode_index(self, bit_index):
    byte_index = int(bit_index / 8)
    offset = int(bit_index % 8)
    return byte_index, offset

  def __getitem__(self, bit_index):
    return self.getitem(bit_index)

  def __setitem__(self, bit_index, value):
    return self.setitem(bit_index, value)

  def __len__(self):
    return self._size_in_bits


class BloomFilter(object):
  def __init__(self, bits=2**20, hashes=8):
    self.bit_vector = BitVector(bits)
    self.hash_funcs = [_gen_hash_func(i) for i in range(hashes)]

  def add(self, item):
    for h in self.hash_funcs:
      index = h(item) % len(self.bit_vector)
      self.bit_vector[index] = 1

  def contains(self, item):
    for h in self.hash_funcs:
      index = h(item) % len(self.bit_vector)
      if self.bit_vector[index] == 0:
        return False
    return True


class CountingBloomFilter(object):
  def __init__(self, bits=2**20, hashes=8, bits_per_entry=4):
    self._bits_per_entry = bits_per_entry
    self._num_entries = bits / bits_per_entry
    self.bit_vector = BitVector(bits)
    self.hash_funcs = [_gen_hash_func(i) for i in range(hashes)]

  def add(self, item):
    # Bits in an entry are in little endian order.
    for h in self.hash_funcs:
      entry_index = h(item) % self._num_entries
      for i in range(self._bits_per_entry):
        bit_vector_index = entry_index * self._bits_per_entry + i
        if self.bit_vector[bit_vector_index] == 0:
          self.bit_vector[bit_vector_index] = 1
          break
        self.bit_vector[bit_vector_index] = 0
      else:
        raise FilterException('Overflow while adding item: ' + str(item))

  def remove(self, item):
    # Bits in an entry are in little endian order.
    for h in self.hash_funcs:
      entry_index = h(item) % self._num_entries
      for i in range(self._bits_per_entry):
        bit_vector_index = entry_index * self._bits_per_entry + i
        if self.bit_vector[bit_vector_index] == 1:
          self.bit_vector[bit_vector_index] = 0
          break
        self.bit_vector[bit_vector_index] = 1
      else:
        raise FilterException('Underflow while removing item: ' + str(item))

  def contains(self, item):
    for h in self.hash_funcs:
      entry_index = h(item) % self._num_entries
      for i in range(self._bits_per_entry):
        bit_vector_index = entry_index * self._bits_per_entry + i
        if self.bit_vector[bit_vector_index] == 1:
          break
      else:
        return False
    return True


class FilterException(Exception):
  pass


def _gen_hash_func(seed):
  return lambda input: int(
      hashlib.sha256(str(input) + str(seed)).hexdigest(), 16)
