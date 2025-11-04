# LinkedHashMap Implementation - Solution Summary

## Problem Overview
- **ACMOJ Problem ID**: 1866
- **Problem**: Implement a LinkedHashMap data structure in C++
- **Requirements**: 
  - Combine HashMap functionality with insertion-order iteration
  - All operations must have O(1) expected time complexity
  - Only allowed libraries: `<cstdio>`, `<cstring>`, `<iostream>`, `<cmath>`, `<string>`

## Solution Approach

### Data Structure Design
The implementation uses two main components:

1. **Hash Table**: Array of buckets for O(1) key lookup
   - Each bucket is a linked list of entries with the same hash
   - Dynamic resizing with configurable load factor

2. **Doubly-Linked List**: Maintains insertion order
   - Each node contains the key-value pair
   - Dummy head and tail nodes for easier iteration

### Key Implementation Details

#### Node Structure
```cpp
struct Node {
    value_type *data;  // Stores pair<const Key, T>
    Node *prev;        // Previous node in insertion order
    Node *next;        // Next node in insertion order
};
```

#### Bucket Structure
```cpp
struct Bucket {
    Node *node;        // Points to the actual data node
    Bucket *next;      // Next bucket in collision chain
};
```

### Critical Operations

1. **Insert**: O(1) expected
   - Check if key exists using hash table
   - If not, create new node
   - Add to end of linked list
   - Add to hash table bucket
   - Rehash if load factor exceeded

2. **Find**: O(1) expected
   - Hash the key to find bucket
   - Linear search in bucket's collision chain

3. **Erase**: O(1) expected
   - Remove from hash table bucket
   - Remove from linked list
   - Delete node

4. **Iteration**: O(1) per step
   - Follow linked list pointers
   - Forward: head->next to tail
   - Backward: tail->prev to head

### Optimization Journey

#### Initial Submission (Score: 80/100)
- **Issue**: Time Limit Exceeded on test 1 (508ms vs 400ms limit)
- **Cause**: Load factor of 0.75 caused too many rehash operations
- **Test 1 characteristics**: 100,000 insertions + 120 copy operations

#### Final Submission (Score: 100/100)
- **Fix**: Increased load factor from 0.75 to 1.5
- **Result**: Test 1 completed in 289ms (well under 400ms limit)
- **Trade-off**: Slightly longer collision chains, but fewer rehashes

### Performance Results

| Test Group | Result | Time (ms) | Memory (MB) |
|------------|--------|-----------|-------------|
| one | Accepted | 289 | 18.9 |
| one.memcheck | Accepted | 6,546 | 214.7 |
| two | Accepted | 192 | 4.4 |
| two.memcheck | Accepted | 10,290 | 438.1 |
| three | Accepted | 42 | 7.2 |
| three.memcheck | Accepted | 2,037 | 119.8 |
| four | Accepted | 22 | 4.9 |
| four.memcheck | Accepted | 1,289 | 63.9 |
| five | Accepted | 1,944 | 129.0 |
| five.memcheck | Accepted | 3,639 | 132.0 |
| six | Accepted | 878 | 4.2 |
| six.memcheck | Accepted | 18,301 | 243.7 |

**Total Score**: 100/100

## Key Challenges Solved

1. **Clear Operation Bug**: Initially forgot to clear hash table buckets in `clear()`, causing segmentation faults
   - Fixed by clearing both linked list and hash table buckets

2. **Performance Optimization**: Balanced load factor to minimize rehashing while maintaining O(1) lookup
   - Load factor of 1.5 proved optimal for the test cases

3. **Iterator Implementation**: Proper handling of edge cases
   - Decrementing from end() iterator
   - Incrementing from last element
   - Empty container checks

## Submission History

1. **Submission 706978**: Score 80/100 (TLE on test 1)
2. **Submission 706981**: Score 100/100 ✓

## Files Modified

- `linked_hashmap.hpp`: Complete implementation (643 lines)
- `submit_acmoj/acmoj_client.py`: Added `submit_code` method

## Conclusion

Successfully implemented a LinkedHashMap that:
- Maintains O(1) expected time complexity for all operations
- Preserves insertion order for iteration
- Passes all test cases including memory leak checks
- Achieves 100/100 score on ACMOJ

