// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument
#include <cmath>     // std::abs


// ---------
// Allocator
// ---------

template <typename T, std::size_t N>
class Allocator {
    public:
        // --------
        // typedefs
        // --------

        typedef T                 value_type;

        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;

        typedef       value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef       value_type&       reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&) {
            return true;}                                              // this is correct

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
            return !(lhs == rhs);}

    private:

        // ----
        // data
        // ----

        char a[N];

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         */
        bool valid () const {
            int curIndex = 0;
            while(curIndex < N) {
                const int* head = &((*this)[curIndex]);
                int size = std::abs(*head);
                if(size < sizeof(T))
                    return false;
                const int* tail = head + size/4 + 1;
                int nextIndex = curIndex + 8 + size;
                if(nextIndex > N)
                    return false;
                if(*head != *tail)
                    return false;
                if(*head > 0) {
                    if(curIndex > 0 && (*this)[curIndex - 4] > 0)
                        return false;  
                } 
                curIndex = nextIndex; 
            }
            if(curIndex  != N )
                return false;
            return true;}

        /**
         * O(1) in space
         * O(1) in time
         * 
         * Overloads the array access operator for Allocator
         * param i an integer argument that specifies the index to dereference
         * return the address of an index within Allocator
         *
         * https://code.google.com/p/googletest/wiki/AdvancedGuide#Private_Class_Members
         */
        FRIEND_TEST(TestAllocator2, index); 
        FRIEND_TEST(TestAllocator4, test_1);
        FRIEND_TEST(TestAllocator4, test_2);
        FRIEND_TEST(TestAllocator4, test_3);
        FRIEND_TEST(TestAllocator4, test_4);
        FRIEND_TEST(TestAllocator4, test_5);
        FRIEND_TEST(TestAllocator5, test_1);
        FRIEND_TEST(TestAllocator5, test_2);
        FRIEND_TEST(TestAllocator5, test_3);
        FRIEND_TEST(TestAllocator5, test_4);
        FRIEND_TEST(TestAllocator5, test_5);
        FRIEND_TEST(TestAllocator5, test_6);
        FRIEND_TEST(TestAllocator5, test_7);
        FRIEND_TEST(TestAllocator6, test_1);
        FRIEND_TEST(TestAllocator6, test_2);
        FRIEND_TEST(TestAllocator6, test_3);
        FRIEND_TEST(TestAllocator6, test_4);
        FRIEND_TEST(TestAllocator6, test_5);
        int& operator [] (int i) {
            return *reinterpret_cast<int*>(&a[i]);}

    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         *
         * Overloads the constructor Allocator. Initializes the beginning and end sentinels.
         *
         * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
         */
        Allocator () {
            if(N < sizeof(T) + 8) {
                throw std::bad_alloc();
            }
            (*this)[0] = N-8;
            (*this)[N-4] = N-8;
            
        
            assert(valid());}  

        // Default copy, destructor, and copy assignment
        // Allocator  (const Allocator&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate
        // --------

        /**
         * O(1) in space
         * O(n) in time
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         * throw a bad_alloc exception, if n is invalid
         */
        pointer allocate (size_type n) {
            int size = n * sizeof(T);
            int* current = &((*this)[0]);
            while(size > *current) {
                char* next = (char*)current;
                next += 8+std::abs(*current);
                if(next >= (char*)(&((*this)[0])) + N) {
                    throw std::bad_alloc();
                }
                current = (int*)next; 
            }
            if(*current - size - 8 < (int)sizeof(T)) {
                size = *current;
                *current = -size;
                *(current + size/4 + 1) = -size;
            } else {
                int temp = *current;
                *current = -size;
                *(current + size/4 + 1) = -size;
                int rem = temp - size - 8;
                *(current + size/4 + 2) = rem;
                *(current + size/4 + 3 + rem/4) = rem;
            }
            

            assert(valid());
            return (pointer)(current+1);}             

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                               // this is correct and exempt
            assert(valid());}                           // from the prohibition of new

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * after deallocation adjacent free blocks must be coalesced
         * throw an invalid_argument exception, if p is invalid
         * param p the pointer to the data that needs to be deallocated
         * param size_type unused variable. Necessary by interface.
         */
        void deallocate (pointer p, size_type) {
            //print();
            if((int*)p < &(*this)[4] || (int*)p > &(*this)[N-4])
                throw std::invalid_argument("Argument out of bounds!");

            int* head = ((int*)p - 1);
            if(*head > 0)
                throw std::invalid_argument("Argument not a given pointer!");
            int curSize = std::abs(*head);
            if(head + curSize/4  > &(*this)[N-4])
                throw std::invalid_argument("Argument not a given pointer!");
            int* tail = (int*)((char*)p + curSize);
            if(*head != *tail)
                throw std::invalid_argument("Argument not a given pointer!");

            *head = std::abs(*head);
            *tail = std::abs(*tail);
            if(head > &(*this)[8]) {
                if(*(head -1) > 0) {
                    //if previous segment is also free, coalesce
                    int* prevTail = head-1;
                    int prevSize = *prevTail;
                    int* prevHead = (int*)((char*)prevTail - prevSize - 4);

                    head = prevHead;
                    curSize = prevSize + 8 + curSize;
                    *head = curSize;
                    *tail = curSize;
                    
                }
            }
            if(tail < &(*this)[N-8]) {
                if(*(tail+1) > 0) {
                    //if next segment is also free, coalesce
                    int* nextHead = tail+1;
                    int nextSize  = *nextHead;
                    int* nextTail = (int*)((char*)nextHead + nextSize + 4);

                    tail = nextTail;
                    curSize = curSize + 8 + nextSize;
                    *head = curSize;
                    *tail = curSize;

                }
            }
            //print();
            assert(valid());}


        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         */
        void destroy (pointer p) {
            p->~T();               // this is correct
            assert(valid());}

        /**
         * O(1) in space
         * O(1) in time
         * Overrides the const array access operator to return a const address to a location with Allocator.
         * param i the index of Allocator that will be returned
         * return the const address of the index
         */
        const int& operator [] (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};
    


#endif // Allocator_h
