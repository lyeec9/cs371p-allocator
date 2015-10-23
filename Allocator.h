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
         * <your documentation>
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
        int& operator [] (int i) {
            return *reinterpret_cast<int*>(&a[i]);}

    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
         */
        Allocator () {
            if(N < sizeof(T) + 8) {
                throw std::bad_alloc();
            }
            (*this)[0] = N-8;
            (*this)[N-4] = N-8;
            
        
            assert(valid());}


        void print() {
            for(int i = 0; i < N/4; i++) {
                std::cout << (long)(&((*this)[4*i])) << " " <<  (*this)[4*i]<< std::endl;
            }

            std::cout << std::endl << std::endl;
        }

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
            //print();
            int size = n * sizeof(T);
            int* current = &((*this)[0]);
            while(size > *current) {
                char* next = (char*)current;
                next += 8+std::abs(*current);
/*std::cout << "current:: " << (long)current << std::endl;
std::cout << "next:: " << (long)next << std::endl;*/
                if(next >= (char*)(&((*this)[0])) + N) {
                    throw std::bad_alloc();
                }
                current = (int*)next; 
            }

//std::cout << "if statement " << *current - size - 8 << " - " << sizeof(T) << std::endl;
            if(*current - size - 8 < (int)sizeof(T)) {
//std::cout << "I'm here!!" << std::endl;
//                std::cout << "size " << size << std::endl;
                size = *current;
//                std::cout << "size " << size << std::endl;
                *current = -size;
                *(current + size/4 + 1) = -size;
            } else {
                int temp = *current;
                *current = -size;
                *(current + size/4 + 1) = -size;
                //char* nextBlock = ((char*)current) + 8;
                //*((int*)nextBlock) = -size;
                /*std::cout << (long)nextBlock << std::endl;
                std::cout << (long)(current + size/4 + 1) << std::endl;
                std::cout << *(current + size/4 + 1) << std::endl;*/
                int rem = temp - size - 8;
                /*std::cout << "temp " << temp<< std::endl;
                std::cout << "rem " << rem << std::endl;*/
                *(current + size/4 + 2) = rem;
                *(current + size/4 + 3 + rem/4) = rem;
            }
            

            assert(valid());
            //print();
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
         * <your documentation>
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
            //if not the head of the array
            if(head > &(*this)[8]) {
                if(*(head -1) > 0) {
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
/*std::cout << "Gets somewhere!" << std::endl;
std::cout << "Current tail pointer " << (long)(tail)<< std::endl;
std::cout << "Next pointer" << (long)(tail+1)<< std::endl;
std::cout << "Next head " << *(tail+1) << std::endl;*/
                if(*(tail+1) > 0) {
                    int* nextHead = tail+1;
                    int nextSize  = *nextHead;
                    int* nextTail = (int*)((char*)nextHead + nextSize + 4);

                    tail = nextTail;
                    curSize = curSize + 8 + nextSize;
                    *head = curSize;
                    *tail = curSize;
/*std::cout << "size " << curSize <<std::endl;
std::cout << "head " << *head <<std::endl;
std::cout << "tail " << *tail <<std::endl;*/

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
         * <your documentation>
         */
        const int& operator [] (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};
    


#endif // Allocator_h
