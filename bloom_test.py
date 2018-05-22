#!/usr/bin/env python2.7

import bloom

def test_bit_vector():
  bv16 = bloom.BitVector(16)
  assert bv16[7] == 0, 'BitVector must be initialized to 0'
  assert bv16[8] == 0, 'BitVector must be initialized to 0'
  assert bv16[9] == 0, 'BitVector must be initialized to 0'
  assert bv16[10] == 0, 'BitVector must be initialized to 0'
  assert bv16[11] == 0, 'BitVector must be initialized to 0'
  bv16[9] = 1
  assert bv16[7] == 0, 'Untouched BitVector values must be 0'
  assert bv16[8] == 0, 'Untouched BitVector values must be 0'
  assert bv16[9] == 1, 'BitVector value at 9 was not set correctly'
  assert bv16[10] == 0, 'Untouched BitVector values must be 0'
  assert bv16[11] == 0, 'Untouched BitVector values must be 0'
  bv16[10] = 1
  assert bv16[7] == 0, 'Untouched BitVector values must be 0'
  assert bv16[8] == 0, 'Untouched BitVector values must be 0'
  assert bv16[9] == 1, 'BitVector value at 9 was clobbered by setting index 10'
  assert bv16[10] == 1, 'BitVector value at 10 was not set correctly'
  assert bv16[11] == 0, 'Untouched BitVector values must be 0'
  bv16[9] = 0
  assert bv16[7] == 0, 'Untouched BitVector values must be 0'
  assert bv16[8] == 0, 'Untouched BitVector values must be 0'
  assert bv16[9] == 0, 'BitVector value at 9 was not set correctly'
  assert bv16[10] == 1, 'BitVector value at 10 was clobbered by setting index 9'
  assert bv16[11] == 0, 'Untouched BitVector values must be 0'

def test_bloom_filter():
  bf = bloom.BloomFilter(bits=20, hashes=3)
  bf.add('hello')
  assert bf.contains('hello'), 'BloomFilter failed to add item "hello"'
  assert not bf.contains('hi'), 'BloomFilter failed to deny item "hi"'
  bf.add('hi')
  assert bf.contains('hello'), 'BloomFilter failed to add item "hello"'
  assert bf.contains('hi'), 'BloomFilter failed to add item "hi"'

def test_counting_bloom_filter():
  cbf = bloom.CountingBloomFilter(bits=20, hashes=3, bits_per_entry=2)
  cbf.add('hello')
  assert cbf.contains('hello'), (
      'CountingBloomFilter failed to add item "hello"')
  assert not cbf.contains('hi'), 'CountingBloomFilter failed to deny item "hi"'
  cbf.add('hi')
  assert cbf.contains('hello'), 'CountingBloomFilter failed to add item "hello"'
  assert cbf.contains('hi'), 'CountingBloomFilter failed to add item "hi"'
  cbf.add('hi')
  assert cbf.contains('hello'), (
      'CountingBloomFilter clobbered added item "hello"')
  assert cbf.contains('hi'), 'CountingBloomFilter failed to add item "hi" twice'
  try:
    cbf.add('hi')
  except bloom.FilterException:
    pass
  else:
    assert False, ('Adding "hi" a third time should have overflowed '
                   'CountingBloomFilter')

  cbf2 = bloom.CountingBloomFilter(bits=64, hashes=3, bits_per_entry=8)
  try:
    cbf2.remove('hello')
  except bloom.FilterException:
    pass
  else:
    assert False, ('Removing nonexistent member "hello" should have underflowed'
                   'CountingFloomFilter')

def main():
  try:
    test_bit_vector()
    test_bloom_filter()
    test_counting_bloom_filter()
  except:
    print('FAIL')
    raise
  else:
    print('SUCCESS')

if __name__ == '__main__':
  main()
