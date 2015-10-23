// ------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------------

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator

#include "gtest/gtest.h"

#include "Allocator.h"

// --------------
// TestAllocator1
// --------------

template <typename A>
struct TestAllocator1 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);

TYPED_TEST(TestAllocator1, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator1, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type  x;
    const size_type       s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

// --------------
// TestAllocator2
// --------------

TEST(TestAllocator2, const_index) {
    const Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

TEST(TestAllocator2, index) {
    const Allocator<int, 400> x;
    ASSERT_EQ(x[0], 392);
    ASSERT_EQ(x[396], 392);
}

// --------------
// TestAllocator3
// --------------

template <typename A>
struct TestAllocator3 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_2;

TYPED_TEST_CASE(TestAllocator3, my_types_2);

TYPED_TEST(TestAllocator3, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator3, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 10;
    const value_type     v = 2;
    const pointer        b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

// --------------
// Allocate tests
// --------------

TEST(TestAllocator4, test_1) {
    Allocator<int, 100> x;
    int* p = x.allocate(1);
    *p = 5;
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    ASSERT_EQ(x[4], 5);
}

TEST(TestAllocator4, test_2) {
    Allocator<int, 100> x;
    int* p = x.allocate(1);
    *p = 5;
    int* p2 = x.allocate(1);
    *p2 = 6;


    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    ASSERT_EQ(x[4], 5);

    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);
    ASSERT_EQ(x[16], 6);
}

TEST(TestAllocator4, test_3) {
    Allocator<int, 12> x;
    
    try{
        x.allocate(2);
        FAIL();
    } catch (std::bad_alloc& e) {
        SUCCEED();
    }
}

TEST(TestAllocator4, test_4) {
    Allocator<double, 100> x;
    double* p = x.allocate(11);
    *(p+1) = 1.1;
    ASSERT_EQ(x[0], -92);
    ASSERT_EQ(x[96], -92);
    ASSERT_EQ(*(double*)(&(x[12])), 1.1); 
}

TEST(TestAllocator4, test_5) {
    Allocator<int, 100> x;
    int* p = x.allocate(1);
    int* p2 = x.allocate(1);

std::cout <<"blah" << (long)p << std::endl;
std::cout << (long)p2 << std::endl;
    x.deallocate(p, 1);
    int* p3 = x.allocate(1);    
std::cout << (long)p3 << std::endl;

    ASSERT_EQ(p, p3);
}

// --------------
// Deallocate tests
// --------------

TEST(TestAllocator5, test_1) {
    Allocator<int, 100> x;
    int* p = x.allocate(1);
    x.deallocate(p, 1);
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

TEST(TestAllocator5, test_2) {
    Allocator<int, 100> x;
    x.allocate(1);
    int* p2 = x.allocate(1);
    x.allocate(1);  
    
    x.deallocate(p2, 1);
    ASSERT_EQ(x[12], 4);
    ASSERT_EQ(x[20], 4);
}

TEST(TestAllocator5, test_3) {
    Allocator<int, 100> x;
    int* p = x.allocate(1);
    int* p2 = x.allocate(1);
    x.allocate(1);  
    
    x.deallocate(p2, 1);
    x.deallocate(p, 1);
    ASSERT_EQ(x[0], 16);
    ASSERT_EQ(x[20], 16);
}

TEST(TestAllocator5, test_4) {
    Allocator<int, 100> x;
    x.allocate(1);
    int* p2 = x.allocate(1);
    int* p3 = x.allocate(1);  
    
    x.deallocate(p2, 1);
    x.deallocate(p3, 1);
    ASSERT_EQ(x[12], 80);
    ASSERT_EQ(x[96], 80);
}

TEST(TestAllocator5, test_5) {
    Allocator<int, 100> x;
    int* p = x.allocate(1);
    int* p2 = p + 7;


    //////////ASSERT_THROW(x.deallocate(p2,1), std::invalid_argument);
    try {
        x.deallocate(p2, 1);
        FAIL();
    } catch (std::invalid_argument& e) {
        SUCCEED();
    }
}

TEST(TestAllocator5, test_6) {
    Allocator<int, 100> x;
    int* p = x.allocate(1);
    int* p2 = p-4;

    try {
        x.deallocate(p2, 1);
        FAIL();
    } catch (std::invalid_argument& e) {
        SUCCEED();
    }
    
}

TEST(TestAllocator5, test_7) {
    Allocator<int, 100> x;
    int* p = x.allocate(1);
    int* p3 = p+100;

    
    try {
        x.deallocate(p3, 1);
        FAIL();
    } catch (std::invalid_argument& e) {
        SUCCEED();
    }
}

// --------------
// Valid tests
// --------------

TEST(TestAllocator6, test_1) {
    Allocator<int, 100> x;
    x[0] = 8;
    x[12] = 9;
    ASSERT_FALSE(x.valid());
}

TEST(TestAllocator6, test_2) {
    Allocator<int, 100> x;
    x[0] = 8;
    x[12] = 8;
    x[16] = 4;
    x[24] = 4;
    ASSERT_FALSE(x.valid());
}

TEST(TestAllocator6, test_3) {
    Allocator<int, 100> x;
    x[0] = -8;
    x[12] = -8;
    x[16] = 84;
    ASSERT_FALSE(x.valid());
}

TEST(TestAllocator6, test_4) {
    Allocator<double, 100> x;
    x[0] = 4;
    x[12] = 4;
    x[16] = 80;
    x[96] = 80;
    ASSERT_FALSE(x.valid());
}

TEST(TestAllocator6, test_5) {
    Allocator<int, 100> x;
    x[0] = -8;
    x[4] = 2346427;
    x[12] = -8;
    x[16] = 8;
    x[28] = 8;
    x[32] = -40;
    x[48] = 346236;
    x[52] = 235;
    x[76] = -40;
    x[80] = -12;
    x[96] = -12;
    ASSERT_TRUE(x.valid());
}