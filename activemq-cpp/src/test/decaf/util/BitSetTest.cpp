/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "BitSetTest.h"

#include <decaf/util/BitSet.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/System.h>

#include <decaf/lang/exceptions/OutOfMemoryError.h>
#include <decaf/lang/exceptions/NegativeArraySizeException.h>
#include <decaf/lang/exceptions/IndexOutOfBoundsException.h>

using namespace std;
using namespace decaf;
using namespace decaf::util;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

////////////////////////////////////////////////////////////////////////////////
namespace {

    BitSet eightbs;

    void printBitset(const BitSet& bs) {
        std::cout << std::endl;
        for (int i = bs.size() - 1; i >= 0; i--) {
            if (bs.get(i)) {
                std::cout << "1";
            } else {
                std::cout << "0";
            }
        }
    }

}

////////////////////////////////////////////////////////////////////////////////
BitSetTest::BitSetTest() {
}

////////////////////////////////////////////////////////////////////////////////
BitSetTest::~BitSetTest() {
}


////////////////////////////////////////////////////////////////////////////////
void BitSetTest::SetUp() {

    eightbs = BitSet(1);

    for (int i = 0; i < 8; i++) {
        eightbs.set(i);
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testConstructor() {

    BitSet bs;

    // Default size for a BitSet should be 64 elements;
    ASSERT_EQ(64, bs.size()) << ("Created BitSet of incorrect size");
    ASSERT_EQ(std::string("{}"), bs.toString()) << ("New BitSet had invalid string representation");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testConstructorI() {

    BitSet bs(128);
    // Default size for a BitSet should be 64 elements;

    ASSERT_EQ(128, bs.size()) << ("Created BitSet of incorrect size");
    ASSERT_EQ(std::string("{}"), bs.toString()) << ("New BitSet had invalid string representation: " + bs.toString());

    // All BitSets are created with elements of multiples of 64

    BitSet bs1(89);
    ASSERT_EQ(128, bs1.size()) << ("Failed to round BitSet element size");

    try {
        BitSet bs(-9);
        FAIL() << ("Failed to throw exception when creating a new BitSet with negative elements value");
    } catch (NegativeArraySizeException& e) {
        // Correct behavior
    }

    // Regression test for HARMONY-4147
    try {
        BitSet bs(Integer::MAX_VALUE);
    } catch (OutOfMemoryError& e) {
        // Ignore if no room for this size.
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testCopy() {
    BitSet bs(eightbs);
    ASSERT_TRUE(bs.equals(eightbs)) << ("copy failed to return equal BitSet");

    BitSet assigned;
    assigned = eightbs;
    ASSERT_TRUE(assigned.equals(eightbs)) << ("copy failed to return equal BitSet");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testEquals() {
    BitSet bs = eightbs;

    ASSERT_TRUE(eightbs.equals(eightbs)) << ("Same BitSet returned false");
    ASSERT_TRUE(bs.equals(eightbs)) << ("Identical BitSet returned false");
    bs.clear(6);
    ASSERT_TRUE(!eightbs.equals(bs)) << ("Different BitSets returned true");
    // Grow the BitSet
    bs = eightbs;
    bs.set(128);
    ASSERT_TRUE(!eightbs.equals(bs)) << ("Different sized BitSet with higher bit set returned true");
    bs.clear(128);
    ASSERT_TRUE(eightbs.equals(bs)) << ("Different sized BitSet with higher bits not set returned false");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testClear() {
    eightbs.clear();
    for (int i = 0; i < 8; i++) {
        ASSERT_TRUE(!eightbs.get(i)) << ("Clear didn't clear bit " + Integer::toString(i));
    }
    ASSERT_EQ(0, eightbs.length()) << ("Test1: Wrong length");

    BitSet bs(3400);
    bs.set(0, bs.size() - 1); // ensure all bits are 1's
    bs.set(bs.size() - 1);
    bs.clear();
    ASSERT_EQ(0, bs.length()) << ("Test2: Wrong length");
    ASSERT_TRUE(bs.isEmpty()) << ("Test2: isEmpty() returned incorrect value");
    ASSERT_EQ(0, bs.cardinality()) << ("Test2: cardinality() returned incorrect value");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testClearI() {

    eightbs.clear(7);
    ASSERT_TRUE(!eightbs.get(7)) << ("Failed to clear bit");

    // Check to see all other bits are still set
    for (int i = 0; i < 7; i++) {
        ASSERT_TRUE(eightbs.get(i)) << ("Clear cleared incorrect bits");
    }

    eightbs.clear(165);
    ASSERT_TRUE(!eightbs.get(165)) << ("Failed to clear bit");

    // Try out of range
    try {
        eightbs.clear(-1);
        FAIL() << ("Failed to throw expected out of bounds exception");
    } catch (IndexOutOfBoundsException& e) {
        // Correct behaviour
    }

    BitSet bs(0);
    ASSERT_EQ(0, bs.length()) << ("Test1: Wrong length,");
    ASSERT_EQ(0, bs.size()) << ("Test1: Wrong size,");
    bs.clear(0);
    ASSERT_EQ(0, bs.length()) << ("Test2: Wrong length,");
    ASSERT_EQ(0, bs.size()) << ("Test2: Wrong size,");

    bs.clear(60);
    ASSERT_EQ(0, bs.length()) << ("Test3: Wrong length,");
    ASSERT_EQ(0, bs.size()) << ("Test3: Wrong size,");

    bs.clear(120);
    ASSERT_EQ(0, bs.size()) << ("Test4: Wrong size,");
    ASSERT_EQ(0, bs.length()) << ("Test4: Wrong length,");

    bs.set(25);
    ASSERT_EQ(64, bs.size()) << ("Test5: Wrong size,");
    ASSERT_EQ(26, bs.length()) << ("Test5: Wrong length,");

    bs.clear(80);
    ASSERT_EQ(64, bs.size()) << ("Test6: Wrong size,");
    ASSERT_EQ(26, bs.length()) << ("Test6: Wrong length,");

    bs.clear(25);
    ASSERT_EQ(64, bs.size()) << ("Test7: Wrong size,");
    ASSERT_EQ(0, bs.length()) << ("Test7: Wrong length,");

    BitSet bs1;
    try {
        bs1.clear(-1);
        FAIL() << ("Should throw IndexOutOfBoundsException");
    } catch (IndexOutOfBoundsException& e) {
        // expected
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testClearII() {
    BitSet bitset;
    for (int i = 0; i < 20; i++) {
        bitset.set(i);
    }
    bitset.clear(10, 10);

    // pos1 and pos2 are in the same bitset element
    BitSet bs(16);
    int initialSize = bs.size();
    bs.set(0, initialSize);
    bs.clear(5);
    bs.clear(15);
    bs.clear(7, 11);
    for (int i = 0; i < 7; i++) {
        if (i == 5) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        } else {
            ASSERT_TRUE(bs.get(i)) << ("Shouldn't have cleared bit " + Integer::toString(i));
        }
    }
    for (int i = 7; i < 11; i++) {
        ASSERT_TRUE(!bs.get(i)) << ("Failed to clear bit " + Integer::toString(i));
    }

    for (int i = 11; i < initialSize; i++) {
        if (i == 15) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        } else {
            ASSERT_TRUE(bs.get(i)) << ("Shouldn't have cleared bit " + Integer::toString(i));
        }
    }

    for (int i = initialSize; i < bs.size(); i++) {
        ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
    }

    // pos1 and pos2 is in the same bitset element, boundary testing
    {
        BitSet bs(16);
        initialSize = bs.size();
        bs.set(0, initialSize);
        bs.clear(7, 64);
        ASSERT_EQ(64, bs.size()) << ("Failed to grow BitSet");
        for (int i = 0; i < 7; i++) {
            ASSERT_TRUE(bs.get(i)) << ("Shouldn't have cleared bit " + Integer::toString(i));
        }
        for (int i = 7; i < 64; i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Failed to clear bit " + Integer::toString(i));
        }
        for (int i = 64; i < bs.size(); i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        }
    }
    {
        // more boundary testing
        BitSet bs(32);
        initialSize = bs.size();
        bs.set(0, initialSize);
        bs.clear(0, 64);
        for (int i = 0; i < 64; i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Failed to clear bit " + Integer::toString(i));
        }
        for (int i = 64; i < bs.size(); i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        }
    }
    {
        BitSet bs(32);
        initialSize = bs.size();
        bs.set(0, initialSize);
        bs.clear(0, 65);
        for (int i = 0; i < 65; i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Failed to clear bit " + Integer::toString(i));
        }
        for (int i = 65; i < bs.size(); i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        }
    }
    {
        // pos1 and pos2 are in two sequential bitset elements
        BitSet bs(128);
        initialSize = bs.size();
        bs.set(0, initialSize);
        bs.clear(7);
        bs.clear(110);
        bs.clear(9, 74);
        for (int i = 0; i < 9; i++) {
            if (i == 7) {
                ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
            } else {
                ASSERT_TRUE(bs.get(i)) << ("Shouldn't have cleared bit " + Integer::toString(i));
            }
        }
        for (int i = 9; i < 74; i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Failed to clear bit " + Integer::toString(i));
        }
        for (int i = 74; i < initialSize; i++) {
            if (i == 110) {
                ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
            } else {
                ASSERT_TRUE(bs.get(i)) << ("Shouldn't have cleared bit " + Integer::toString(i));
            }
        }
        for (int i = initialSize; i < bs.size(); i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        }
    }
    {
        // pos1 and pos2 are in two non-sequential bitset elements
        BitSet bs(256);
        bs.set(0, 256);
        bs.clear(7);
        bs.clear(255);
        bs.clear(9, 219);
        for (int i = 0; i < 9; i++) {
            if (i == 7) {
                ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
            } else {
                ASSERT_TRUE(bs.get(i)) << ("Shouldn't have cleared bit " + Integer::toString(i));
            }
        }

        for (int i = 9; i < 219; i++) {
            ASSERT_TRUE(!bs.get(i)) << ("CPPUNIT_FAILed to clear bit " + Integer::toString(i));
        }

        for (int i = 219; i < 255; i++) {
            ASSERT_TRUE(bs.get(i)) << ("Shouldn't have cleared bit " + Integer::toString(i));
        }

        for (int i = 255; i < bs.size(); i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        }
    }
    {
        // test illegal args
        BitSet bs(10);
        try {
            bs.clear(-1, 3);
            FAIL() << ("Test1: Attempt to flip with  negative index CPPUNIT_FAILed to generate exception");
        } catch (IndexOutOfBoundsException& e) {
            // excepted
        }

        try {
            bs.clear(2, -1);
            FAIL() << ("Test2: Attempt to flip with negative index CPPUNIT_FAILed to generate exception");
        } catch (IndexOutOfBoundsException& e) {
            // excepted
        }

        bs.set(2, 4);
        bs.clear(2, 2);
        ASSERT_TRUE(bs.get(2)) << ("Bit got cleared incorrectly ");
        try {
            bs.clear(4, 2);
            FAIL() << ("Test4: Attempt to flip with illegal args CPPUNIT_FAILed to generate exception");
        } catch (IndexOutOfBoundsException& e) {
            // excepted
        }
    }
    {
        BitSet bs(0);
        ASSERT_EQ(0, bs.length()) << ("Test1: Wrong length,");
        ASSERT_EQ(0, bs.size()) << ("Test1: Wrong size,");

        bs.clear(0, 2);
        ASSERT_EQ(0, bs.length()) << ("Test2: Wrong length,");
        ASSERT_EQ(0, bs.size()) << ("Test2: Wrong size,");

        bs.clear(60, 64);
        ASSERT_EQ(0, bs.length()) << ("Test3: Wrong length,");
        ASSERT_EQ(0, bs.size()) << ("Test3: Wrong size,");

        bs.clear(64, 120);
        ASSERT_EQ(0, bs.length()) << ("Test4: Wrong length,");
        ASSERT_EQ(0, bs.size()) << ("Test4: Wrong size,");

        bs.set(25);
        ASSERT_EQ(26, bs.length()) << ("Test5: Wrong length,");
        ASSERT_EQ(64, bs.size()) << ("Test5: Wrong size,");

        bs.clear(60, 64);
        ASSERT_EQ(26, bs.length()) << ("Test6: Wrong length,");
        ASSERT_EQ(64, bs.size()) << ("Test6: Wrong size,");

        bs.clear(64, 120);
        ASSERT_EQ(64, bs.size()) << ("Test7: Wrong size,");
        ASSERT_EQ(26, bs.length()) << ("Test7: Wrong length,");

        bs.clear(80);
        ASSERT_EQ(64, bs.size()) << ("Test8: Wrong size,");
        ASSERT_EQ(26, bs.length()) << ("Test8: Wrong length,");

        bs.clear(25);
        ASSERT_EQ(64, bs.size()) << ("Test9: Wrong size,");
        ASSERT_EQ(0, bs.length()) << ("Test9: Wrong length,");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testGetI() {

    BitSet bs;
    bs.set(8);
    ASSERT_TRUE(!eightbs.get(99)) << ("Get returned true for index out of range");
    ASSERT_TRUE(eightbs.get(3)) << ("Get returned false for set value");
    ASSERT_TRUE(!bs.get(0)) << ("Get returned true for a non set value");

    try {
        bs.get(-1);
        FAIL() << ("Attempt to get at negative index CPPUNIT_FAILed to generate exception");
    } catch (IndexOutOfBoundsException& e) {
        // Correct behaviour
    }

    BitSet bs1;
    ASSERT_TRUE(!bs1.get(64)) << ("Access greater than size");

    BitSet bs2;
    bs2.set(63);
    ASSERT_TRUE(bs2.get(63)) << ("Test highest bit");

    BitSet bs3(0);
    ASSERT_EQ(0, bs3.length()) << ("Test1: Wrong length,");
    ASSERT_EQ(0, bs3.size()) << ("Test1: Wrong size,");

    bs3.get(2);
    ASSERT_EQ(0, bs3.length()) << ("Test2: Wrong length,");
    ASSERT_EQ(0, bs3.size()) << ("Test2: Wrong size,");

    bs3.get(70);
    ASSERT_EQ(0, bs3.length()) << ("Test3: Wrong length,");
    ASSERT_EQ(0, bs3.size()) << ("Test3: Wrong size,");

    bs3.set(70);
    ASSERT_EQ(71, bs3.length()) << ("Test3: Wrong length,");
    ASSERT_EQ(128, bs3.size()) << ("Test3: Wrong size,");
    ASSERT_EQ(true, bs3.get(70)) << ("Test3: Wrong value,");

    BitSet bs4;
    try {
        bs4.get(Integer::MIN_VALUE);
        FAIL() << ("Should throw IndexOutOfBoundsException");
    } catch (IndexOutOfBoundsException& e) {
        // expected
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testGetII() {

    BitSet bitset(30);
    bitset.get(3, 3);

    BitSet bs(512);
    BitSet resultbs;

    bs.set(3, 9);
    bs.set(10, 20);
    bs.set(60, 75);
    bs.set(121);
    bs.set(130, 140);

    {
        // pos1 and pos2 are in the same bitset element, at index0
        resultbs = bs.get(3, 6);
        BitSet correctbs(3);
        correctbs.set(0, 3);
        ASSERT_TRUE(correctbs.equals(resultbs)) << ("Test1: Returned incorrect BitSet");
    }
    {
        // pos1 and pos2 are in the same bitset element, at index 1
        resultbs = bs.get(100, 125);
        BitSet correctbs(25);
        correctbs.set(21);
        ASSERT_TRUE(correctbs.equals(resultbs)) << ("Test2: Returned incorrect BitSet");
    }
    {
        // pos1 in bitset element at index 0, and pos2 in bitset element at index 1
        resultbs = bs.get(15, 125);
        BitSet correctbs(25);
        correctbs.set(0, 5);
        correctbs.set(45, 60);
        correctbs.set(121 - 15);
        ASSERT_TRUE(correctbs.equals(resultbs)) << ("Test3: Returned incorrect BitSet");
    }
    {
        // pos1 in bitset element at index 1, and pos2 in bitset element at index 2
        resultbs = bs.get(70, 145);
        BitSet correctbs(75);
        correctbs.set(0, 5);
        correctbs.set(51);
        correctbs.set(60, 70);
        ASSERT_TRUE(correctbs.equals(resultbs)) << ("Test4: Returned incorrect BitSet");
    }
    {
        // pos1 in bitset element at index 0, and pos2 in bitset element at index 2
        resultbs = bs.get(5, 145);
        BitSet correctbs(140);
        correctbs.set(0, 4);
        correctbs.set(5, 15);
        correctbs.set(55, 70);
        correctbs.set(116);
        correctbs.set(125, 135);
        ASSERT_TRUE(correctbs.equals(resultbs)) << ("Test5: Returned incorrect BitSet");
    }
    {
        // pos1 in bitset element at index 0, and pos2 in bitset element at index 3
        resultbs = bs.get(5, 250);
        BitSet correctbs(200);
        correctbs.set(0, 4);
        correctbs.set(5, 15);
        correctbs.set(55, 70);
        correctbs.set(116);
        correctbs.set(125, 135);
        ASSERT_TRUE(correctbs.equals(resultbs)) << ("Test5: Returned incorrect BitSet");

        ASSERT_TRUE(bs.get(0, bs.size()).equals(bs)) << ("equality principle 1 ");
    }
    {
        // more tests
        BitSet bs2(129);
        bs2.set(0, 20);
        bs2.set(62, 65);
        bs2.set(121, 123);
        resultbs = bs2.get(1, 124);
        BitSet correctbs(129);
        correctbs.set(0, 19);
        correctbs.set(61, 64);
        correctbs.set(120, 122);
        ASSERT_TRUE(correctbs.equals(resultbs)) << ("Test7: Returned incorrect BitSet");
    }
    {
        // equality principle with some boundary conditions
        BitSet bs2(128);
        bs2.set(2, 20);
        bs2.set(62);
        bs2.set(121, 123);
        bs2.set(127);
        resultbs = bs2.get(0, bs2.size());
        ASSERT_TRUE(bs2.equals(resultbs)) << ("equality principle");
    }
    {
        BitSet bs2(128);
        bs2.set(2, 20);
        bs2.set(62);
        bs2.set(121, 123);
        bs2.set(127);
        bs2.flip(0, 128);
        resultbs = bs2.get(0, bs.size());
        ASSERT_TRUE(resultbs.equals(bs2)) << ("equality principle 3 ");
    }
    {
        BitSet bs(0);
        ASSERT_EQ(0, bs.length()) << ("Test1: Wrong length,");
        ASSERT_EQ(0, bs.size()) << ("Test1: Wrong size,");

        bs.get(0, 2);
        ASSERT_EQ(0, bs.length()) << ("Test2: Wrong length,");
        ASSERT_EQ(0, bs.size()) << ("Test2: Wrong size,");

        bs.get(60, 64);
        ASSERT_EQ(0, bs.length()) << ("Test3: Wrong length,");
        ASSERT_EQ(0, bs.size()) << ("Test3: Wrong size,");

        bs.get(64, 120);
        ASSERT_EQ(0, bs.length()) << ("Test4: Wrong length,");
        ASSERT_EQ(0, bs.size()) << ("Test4: Wrong size,");

        bs.set(25);
        ASSERT_EQ(26, bs.length()) << ("Test5: Wrong length,");
        ASSERT_EQ(64, bs.size()) << ("Test5: Wrong size,");

        bs.get(60, 64);
        ASSERT_EQ(26, bs.length()) << ("Test6: Wrong length,");
        ASSERT_EQ(64, bs.size()) << ("Test6: Wrong size,");

        bs.get(64, 120);
        ASSERT_EQ(64, bs.size()) << ("Test7: Wrong size,");
        ASSERT_EQ(26, bs.length()) << ("Test7: Wrong length,");

        bs.get(80);
        ASSERT_EQ(64, bs.size()) << ("Test8: Wrong size,");
        ASSERT_EQ(26, bs.length()) << ("Test8: Wrong length,");

        bs.get(25);
        ASSERT_EQ(64, bs.size()) << ("Test9: Wrong size,");
        ASSERT_EQ(26, bs.length()) << ("Test9: Wrong length,");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testFlipI() {
    BitSet bs;
    bs.clear(8);
    bs.clear(9);
    bs.set(10);
    bs.flip(9);
    ASSERT_TRUE(!bs.get(8)) << ("Failed to flip bit");
    ASSERT_TRUE(bs.get(9)) << ("Failed to flip bit");
    ASSERT_TRUE(bs.get(10)) << ("Failed to flip bit");

    bs.set(8);
    bs.set(9);
    bs.clear(10);
    bs.flip(9);
    ASSERT_TRUE(bs.get(8)) << ("Failed to flip bit");
    ASSERT_TRUE(!bs.get(9)) << ("Failed to flip bit");
    ASSERT_TRUE(!bs.get(10)) << ("Failed to flip bit");

    try {
        bs.flip(-1);
        FAIL() << ("Attempt to flip at negative index CPPUNIT_FAILed to generate exception");
    } catch (IndexOutOfBoundsException& e) {
        // Correct behaviour
    }

    // Try setting a bit on a 64 boundary
    bs.flip(128);
    ASSERT_EQ(192, bs.size()) << ("Failed to grow BitSet");
    ASSERT_TRUE(bs.get(128)) << ("Failed to flip bit");

    BitSet bs1(64);
    for (int i = bs1.size(); --i >= 0;) {
        bs1.flip(i);
        ASSERT_TRUE(bs1.get(i)) << ("Test1: Incorrectly flipped bit" + Integer::toString(i));
        ASSERT_EQ(i+1, bs1.length()) << ("Incorrect length");
        for (int j = bs1.size(); --j > i;) {
            ASSERT_TRUE(!bs1.get(j)) << ("Test2: Incorrectly flipped bit" + Integer::toString(j));
        }
        for (int j = i; --j >= 0;) {
            ASSERT_TRUE(!bs1.get(j)) << ("Test3: Incorrectly flipped bit" + Integer::toString(j));
        }
        bs1.flip(i);
    }

    BitSet bs0(0);
    ASSERT_EQ(0, bs0.size()) << ("Test1: Wrong size");
    ASSERT_EQ(0, bs0.length()) << ("Test1: Wrong length");

    bs0.flip(0);
    ASSERT_EQ(64, bs0.size()) << ("Test2: Wrong size");
    ASSERT_EQ(1, bs0.length()) << ("Test2: Wrong length");

    bs0.flip(63);
    ASSERT_EQ(64, bs0.size()) << ("Test3: Wrong size");
    ASSERT_EQ(64, bs0.length()) << ("Test3: Wrong length");

    eightbs.flip(7);
    ASSERT_TRUE(!eightbs.get(7)) << ("Failed to flip bit 7");

    // Check to see all other bits are still set
    for (int i = 0; i < 7; i++) {
        ASSERT_TRUE(eightbs.get(i)) << ("Flip flipped incorrect bits");
    }

    eightbs.flip(127);
    ASSERT_TRUE(eightbs.get(127)) << ("Failed to flip bit 127");

    eightbs.flip(127);
    ASSERT_TRUE(!eightbs.get(127)) << ("Failed to flip bit 127");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testFlipII() {
    BitSet bitset;
    for (int i = 0; i < 20; i++) {
        bitset.set(i);
    }
    bitset.flip(10, 10);

    // Test for method void java.util.BitSet.flip(int, int)
    // pos1 and pos2 are in the same bitset element
    BitSet bs(16);
    bs.set(7);
    bs.set(10);
    bs.flip(7, 11);
    for (int i = 0; i < 7; i++) {
        ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
    }
    ASSERT_TRUE(!bs.get(7)) << ("Failed to flip bit 7");
    ASSERT_TRUE(bs.get(8)) << ("Failed to flip bit 8");
    ASSERT_TRUE(bs.get(9)) << ("Failed to flip bit 9");
    ASSERT_TRUE(!bs.get(10)) << ("Failed to flip bit 10");
    for (int i = 11; i < bs.size(); i++) {
        ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
    }

    {
        // pos1 and pos2 is in the same bitset element, boundry testing
        BitSet bs(16);
        bs.set(7);
        bs.set(10);
        bs.flip(7, 64);
        ASSERT_EQ(64, bs.size()) << ("Failed to grow BitSet");
        for (int i = 0; i < 7; i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(7)) << ("Failed to flip bit 7");
        ASSERT_TRUE(bs.get(8)) << ("Failed to flip bit 8");
        ASSERT_TRUE(bs.get(9)) << ("Failed to flip bit 9");
        ASSERT_TRUE(!bs.get(10)) << ("Failed to flip bit 10");
        for (int i = 11; i < 64; i++) {
            ASSERT_TRUE(bs.get(i)) << ("CPPUNIT_FAILed to flip bit " + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(64)) << ("Shouldn't have flipped bit 64");
    }
    {
        // more boundary testing
        BitSet bs(32);
        bs.flip(0, 64);
        for (int i = 0; i < 64; i++) {
            ASSERT_TRUE(bs.get(i)) << ("Failed to flip bit " + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(64)) << ("Shouldn't have flipped bit 64");
    }
    {
        BitSet bs(32);
        bs.flip(0, 65);
        for (int i = 0; i < 65; i++) {
            ASSERT_TRUE(bs.get(i)) << ("Failed to flip bit " + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(65)) << ("Shouldn't have flipped bit 65");
    }
    {
        // pos1 and pos2 are in two sequential bitset elements
        BitSet bs(128);
        bs.set(7);
        bs.set(10);
        bs.set(72);
        bs.set(110);
        bs.flip(9, 74);
        for (int i = 0; i < 7; i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        }
        ASSERT_TRUE(bs.get(7)) << ("Shouldn't have flipped bit 7");
        ASSERT_TRUE(!bs.get(8)) << ("Shouldn't have flipped bit 8");
        ASSERT_TRUE(bs.get(9)) << ("Failed to flip bit 9");
        ASSERT_TRUE(!bs.get(10)) << ("Failed to flip bit 10");
        for (int i = 11; i < 72; i++) {
            ASSERT_TRUE(bs.get(i)) << ("CPPUNIT_FAILed to flip bit " + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(72)) << ("Failed to flip bit 72");
        ASSERT_TRUE(bs.get(73)) << ("Failed to flip bit 73");
        for (int i = 74; i < 110; i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        }
        ASSERT_TRUE(bs.get(110)) << ("Shouldn't have flipped bit 110");
        for (int i = 111; i < bs.size(); i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        }
    }
    {
        // pos1 and pos2 are in two non-sequential bitset elements
        BitSet bs(256);
        bs.set(7);
        bs.set(10);
        bs.set(72);
        bs.set(110);
        bs.set(181);
        bs.set(220);
        bs.flip(9, 219);
        for (int i = 0; i < 7; i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        }
        ASSERT_TRUE(bs.get(7)) << ("Shouldn't have flipped bit 7");
        ASSERT_TRUE(!bs.get(8)) << ("Shouldn't have flipped bit 8");
        ASSERT_TRUE(bs.get(9)) << ("Failed to flip bit 9");
        ASSERT_TRUE(!bs.get(10)) << ("Failed to flip bit 10");
        for (int i = 11; i < 72; i++) {
            ASSERT_TRUE(bs.get(i)) << ("CPPUNIT_FAILed to flip bit " + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(72)) << ("Failed to flip bit 72");
        for (int i = 73; i < 110; i++) {
            ASSERT_TRUE(bs.get(i)) << ("CPPUNIT_FAILed to flip bit " + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(110)) << ("Failed to flip bit 110");
        for (int i = 111; i < 181; i++) {
            ASSERT_TRUE(bs.get(i)) << ("CPPUNIT_FAILed to flip bit " + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(181)) << ("Failed to flip bit 181");
        for (int i = 182; i < 219; i++) {
            ASSERT_TRUE(bs.get(i)) << ("CPPUNIT_FAILed to flip bit " + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(219)) << ("Shouldn't have flipped bit 219");
        ASSERT_TRUE(bs.get(220)) << ("Shouldn't have flipped bit 220");
        for (int i = 221; i < bs.size(); i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
        }
    }
    {
        // test illegal args
        BitSet bs(10);
        try {
            bs.flip(-1, 3);
            FAIL() << ("Test1: Attempt to flip with  negative index CPPUNIT_FAILed to generate exception");
        } catch (IndexOutOfBoundsException& e) {
            // correct behavior
        }

        try {
            bs.flip(2, -1);
            FAIL() << ("Test2: Attempt to flip with negative index CPPUNIT_FAILed to generate exception");
        } catch (IndexOutOfBoundsException& e) {
            // correct behavior
        }

        try {
            bs.flip(4, 2);
            FAIL() << ("Test4: Attempt to flip with illegal args CPPUNIT_FAILed to generate exception");
        } catch (IndexOutOfBoundsException& e) {
            // correct behavior
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testSetI() {

    BitSet bs;
    bs.set(8);
    ASSERT_TRUE(bs.get(8)) << ("Failed to set bit");

    try {
        bs.set(-1);
        FAIL() << ("Attempt to set at negative index CPPUNIT_FAILed to generate exception");
    } catch (IndexOutOfBoundsException& e) {
        // Correct behavior
    }

    {
        // Try setting a bit on a 64 boundary
        bs.set(128);
        ASSERT_EQ(192, bs.size()) << ("Failed to grow BitSet");
        ASSERT_TRUE(bs.get(128)) << ("Failed to set bit");
    }
    {
        BitSet bs(64);
        for (int i = bs.size(); --i >= 0;) {
            bs.set(i);
            ASSERT_TRUE(bs.get(i)) << ("Incorrectly set");
            ASSERT_EQ(i+1, bs.length()) << ("Incorrect length");
            for (int j = bs.size(); --j > i;)
                ASSERT_TRUE(!bs.get(j)) << ("Incorrectly set bit " + Integer::toString(j));
            for (int j = i; --j >= 0;)
                ASSERT_TRUE(!bs.get(j)) << ("Incorrectly set bit " + Integer::toString(j));
            bs.clear(i);
        }
    }
    {
        BitSet bs(0);
        ASSERT_EQ(0, bs.length()) << ("Test1: Wrong length");
        bs.set(0);
        ASSERT_EQ(1, bs.length()) << ("Test2: Wrong length");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testSetIB() {
    eightbs.set(5, false);
    ASSERT_TRUE(!eightbs.get(5)) << ("Should have set bit 5 to true");

    eightbs.set(5, true);
    ASSERT_TRUE(eightbs.get(5)) << ("Should have set bit 5 to false");

    try {
        BitSet bs;
        bs.set(-2147483648, false);
        FAIL() << ("Should throw IndexOutOfBoundsException");
    } catch (IndexOutOfBoundsException& e) {
        // expected
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testSetII() {

    BitSet bitset(30);
    bitset.set(29, 29);

    {
        // pos1 and pos2 are in the same bitset element
        BitSet bs(16);
        bs.set(5);
        bs.set(15);
        bs.set(7, 11);
        for (int i = 0; i < 7; i++) {
            if (i == 5) {
                ASSERT_TRUE(bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
            } else {
                ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have set bit " + Integer::toString(i));
            }
        }
        for (int i = 7; i < 11; i++) {
            ASSERT_TRUE(bs.get(i)) << ("Failed to set bit " + Integer::toString(i));
        }
        for (int i = 11; i < bs.size(); i++) {
            if (i == 15) {
                ASSERT_TRUE(bs.get(i)) << (std::string("Shouldn't have flipped bit ") + Integer::toString(i));
            } else {
                ASSERT_TRUE(!bs.get(i)) << (std::string("Shouldn't have set bit ") + Integer::toString(i));
            }
        }
    }
    {
        // pos1 and pos2 is in the same bitset element, boundary testing
        BitSet bs(16);
        bs.set(7, 64);
        ASSERT_EQ(64, bs.size()) << ("Failed to grow BitSet");
        for (int i = 0; i < 7; i++) {
            ASSERT_TRUE(!bs.get(i)) << (std::string("Shouldn't have set bit ") + Integer::toString(i));
        }
        for (int i = 7; i < 64; i++) {
            ASSERT_TRUE(bs.get(i)) << (std::string("Failed to set bit ") + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(64)) << ("Shouldn't have set bit 64");
    }
    {
        // more boundary testing
        BitSet bs(32);
        bs.set(0, 64);
        for (int i = 0; i < 64; i++) {
            ASSERT_TRUE(bs.get(i)) << ("Failed to set bit " + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(64)) << ("Shouldn't have set bit 64");
    }
    {
        BitSet bs(32);
        bs.set(0, 65);
        for (int i = 0; i < 65; i++) {
            ASSERT_TRUE(bs.get(i)) << ("Failed to set bit " + Integer::toString(i));
        }
        ASSERT_TRUE(!bs.get(65)) << ("Shouldn't have set bit 65");
    }
    {
        // pos1 and pos2 are in two sequential bitset elements
        BitSet bs(128);
        bs.set(7);
        bs.set(110);
        bs.set(9, 74);
        for (int i = 0; i < 9; i++) {
            if (i == 7) {
                ASSERT_TRUE(bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
            } else {
                ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have set bit " + Integer::toString(i));
            }
        }
        for (int i = 9; i < 74; i++) {
            ASSERT_TRUE(bs.get(i)) << ("Failed to set bit " + Integer::toString(i));
        }
        for (int i = 74; i < bs.size(); i++) {
            if (i == 110) {
                ASSERT_TRUE(bs.get(i)) << ("Shouldn't have flipped bit " + Integer::toString(i));
            } else {
                ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have set bit " + Integer::toString(i));
            }
        }
    }
    {
        // pos1 and pos2 are in two non-sequential bitset elements
        BitSet bs(256);
        bs.set(7);
        bs.set(255);
        bs.set(9, 219);
        for (int i = 0; i < 9; i++) {
            if (i == 7) {
                ASSERT_TRUE(bs.get(i)) << ("Shouldn't have set flipped " + Integer::toString(i));
            } else {
                ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have set bit " + Integer::toString(i));
            }
        }

        for (int i = 9; i < 219; i++) {
            ASSERT_TRUE(bs.get(i)) << ("CPPUNIT_FAILed to set bit " + Integer::toString(i));
        }

        for (int i = 219; i < 255; i++) {
            ASSERT_TRUE(!bs.get(i)) << ("Shouldn't have set bit " + Integer::toString(i));
        }

        ASSERT_TRUE(bs.get(255)) << ("Shouldn't have flipped bit 255");
    }
    {
        // test illegal args
        BitSet bs(10);
        try {
            bs.set(-1, 3);
            FAIL() << ("Test1: Attempt to flip with  negative index CPPUNIT_FAILed to generate exception");
        } catch (IndexOutOfBoundsException& e) {
            // Correct behavior
        }

        try {
            bs.set(2, -1);
            FAIL() << ("Test2: Attempt to flip with negative index CPPUNIT_FAILed to generate exception");
        } catch (IndexOutOfBoundsException& e) {
            // Correct behavior
        }

        bs.set(2, 2);
        ASSERT_TRUE(!bs.get(2)) << ("Bit got set incorrectly ");

        try {
            bs.set(4, 2);
            FAIL() << ("Test4: Attempt to flip with illegal args CPPUNIT_FAILed to generate exception");
        } catch (IndexOutOfBoundsException& e) {
            // Correct behavior
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testSetIIB() {
    eightbs.set(3, 6, false);
    ASSERT_TRUE(!eightbs.get(3) && !eightbs.get(4) && !eightbs.get(5)) << ("Should have set bits 3, 4, and 5 to false");

    eightbs.set(3, 6, true);
    ASSERT_TRUE(eightbs.get(3) && eightbs.get(4) && eightbs.get(5)) << ("Should have set bits 3, 4, and 5 to true");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testNotModified() {
    // BitSet shouldn't be modified by any of the operations below,
    // since the affected bits for these methods are defined as inclusive of
    // pos1, exclusive of pos2.
    try {
        eightbs.flip(0, 0);
        ASSERT_TRUE(eightbs.get(0)) << ("Bit got flipped incorrectly ");

        BitSet bsnew = eightbs.get(2, 2);
        ASSERT_EQ(0, bsnew.cardinality()) << ("BitSet retrieved incorrectly ");

        eightbs.set(10, 10);
        ASSERT_TRUE(!eightbs.get(10)) << ("Bit got set incorrectly ");

        eightbs.clear(3, 3);
        ASSERT_TRUE(eightbs.get(3)) << ("Bit cleared incorrectly ");
    } catch (IndexOutOfBoundsException& e) {
        FAIL() << ("Unexpected IndexOutOfBoundsException when pos1 ==pos2");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testIntersects() {
    BitSet bs(500);
    bs.set(5);
    bs.set(63);
    bs.set(64);
    bs.set(71, 110);
    bs.set(127, 130);
    bs.set(192);
    bs.set(450);

    BitSet bs2(8);
    ASSERT_TRUE(!bs.intersects(bs2)) << ("Test1: intersects() returned incorrect value");
    ASSERT_TRUE(!bs2.intersects(bs)) << ("Test1: intersects() returned incorrect value");

    bs2.set(4);
    ASSERT_TRUE(!bs.intersects(bs2)) << ("Test2: intersects() returned incorrect value");
    ASSERT_TRUE(!bs2.intersects(bs)) << ("Test2: intersects() returned incorrect value");

    bs2.clear();
    bs2.set(5);
    ASSERT_TRUE(bs.intersects(bs2)) << ("Test3: intersects() returned incorrect value");
    ASSERT_TRUE(bs2.intersects(bs)) << ("Test3: intersects() returned incorrect value");

    bs2.clear();
    bs2.set(63);
    ASSERT_TRUE(bs.intersects(bs2)) << ("Test4: intersects() returned incorrect value");
    ASSERT_TRUE(bs2.intersects(bs)) << ("Test4: intersects() returned incorrect value");

    bs2.clear();
    bs2.set(80);
    ASSERT_TRUE(bs.intersects(bs2)) << ("Test5: intersects() returned incorrect value");
    ASSERT_TRUE(bs2.intersects(bs)) << ("Test5: intersects() returned incorrect value");

    bs2.clear();
    bs2.set(127);
    ASSERT_TRUE(bs.intersects(bs2)) << ("Test6: intersects() returned incorrect value");
    ASSERT_TRUE(bs2.intersects(bs)) << ("Test6: intersects() returned incorrect value");

    bs2.clear();
    bs2.set(192);
    ASSERT_TRUE(bs.intersects(bs2)) << ("Test7: intersects() returned incorrect value");
    ASSERT_TRUE(bs2.intersects(bs)) << ("Test7: intersects() returned incorrect value");

    bs2.clear();
    bs2.set(450);
    ASSERT_TRUE(bs.intersects(bs2)) << ("Test8: intersects() returned incorrect value");
    ASSERT_TRUE(bs2.intersects(bs)) << ("Test8: intersects() returned incorrect value");

    bs2.clear();
    bs2.set(500);
    ASSERT_TRUE(!bs.intersects(bs2)) << ("Test9: intersects() returned incorrect value");
    ASSERT_TRUE(!bs2.intersects(bs)) << ("Test9: intersects() returned incorrect value");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testAnd() {
    BitSet bs(128);

    // Initialize the bottom half of the BitSet
    for (int i = 64; i < 128; i++) {
        bs.set(i);
    }

    eightbs.AND(bs);
    ASSERT_TRUE(!eightbs.equals(bs)) << ("AND CPPUNIT_FAILed to clear bits");
    eightbs.set(3);
    bs.set(3);
    eightbs.AND(bs);
    ASSERT_TRUE(bs.get(3)) << ("AND CPPUNIT_FAILed to maintain set bits");
    bs.AND(eightbs);

    for (int i = 64; i < 128; i++) {
        ASSERT_TRUE(!bs.get(i)) << ("Failed to clear extra bits in the receiver BitSet");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testAndNot() {

    BitSet bs = eightbs;
    bs.clear(5);
    BitSet bs2;
    bs2.set(2);
    bs2.set(3);
    bs.andNot(bs2);
    ASSERT_EQ(std::string("{0, 1, 4, 6, 7}"), bs.toString()) << ("Incorrect bitset after andNot");

    BitSet bs3(0);
    bs3.andNot(bs2);
    ASSERT_EQ(0, bs3.size()) << ("Incorrect size");

    {
        BitSet bs(256);
        BitSet bs2(256);
        bs.set(97);
        bs2.set(37);
        bs.andNot(bs2);
        ASSERT_TRUE(bs.get(97)) << ("Incorrect value at 97 pos");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testOR() {
    {
        BitSet bs(128);
        bs.OR(eightbs);
        for (int i = 0; i < 8; i++) {
            ASSERT_TRUE(bs.get(i)) << ("OR CPPUNIT_FAILed to set bits");
        }
    }
    {
        BitSet bs(0);
        bs.OR(eightbs);
        for (int i = 0; i < 8; i++) {
            ASSERT_TRUE(bs.get(i)) << ("OR(0) CPPUNIT_FAILed to set bits");
        }
    }
    {
        eightbs.clear(5);
        BitSet bs(128);
        bs.OR(eightbs);
        ASSERT_TRUE(!bs.get(5)) << ("OR set a bit which should be off");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testXOR() {

    {
        BitSet bs = eightbs;
        bs.XOR(eightbs);
        for (int i = 0; i < 8; i++) {
            ASSERT_TRUE(!bs.get(i)) << ("XOR CPPUNIT_FAILed to clear bits");
        }

        bs.XOR(eightbs);
        for (int i = 0; i < 8; i++) {
            ASSERT_TRUE(bs.get(i)) << ("XOR CPPUNIT_FAILed to set bits");
        }
    }
    {
        BitSet bs(0);
        bs.XOR(eightbs);
        for (int i = 0; i < 8; i++) {
            ASSERT_TRUE(bs.get(i)) << ("XOR(0) CPPUNIT_FAILed to set bits");
        }
    }
    {
        BitSet bs;
        bs.set(63);
        ASSERT_EQ(std::string("{63}"), bs.toString()) << ("Test highest bit");
    }
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testSize() {
    ASSERT_EQ(64, eightbs.size()) << ("Returned incorrect size");
    eightbs.set(129);
    ASSERT_TRUE(eightbs.size() >= 129) << ("Returned incorrect size");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testToString() {
    ASSERT_EQ(std::string("{0, 1, 2, 3, 4, 5, 6, 7}"), eightbs.toString()) << ("Returned incorrect string representation");
    eightbs.clear(2);
    ASSERT_EQ(std::string("{0, 1, 3, 4, 5, 6, 7}"), eightbs.toString()) << ("Returned incorrect string representation");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testLength() {
    BitSet bs;
    ASSERT_EQ(0, bs.length()) << ("BitSet returned wrong length");
    bs.set(5);
    ASSERT_EQ(6, bs.length()) << ("BitSet returned wrong length");
    bs.set(10);
    ASSERT_EQ(11, bs.length()) << ("BitSet returned wrong length");
    bs.set(432);
    ASSERT_EQ(433, bs.length()) << ("BitSet returned wrong length");
    bs.set(300);
    ASSERT_EQ(433, bs.length()) << ("BitSet returned wrong length");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testNextSetBitI() {
    BitSet bs(500);
    bs.set(5);
    bs.set(32);
    bs.set(63);
    bs.set(64);
    bs.set(71, 110);
    bs.set(127, 130);
    bs.set(193);
    bs.set(450);
    try {
        bs.nextSetBit(-1);
        FAIL() << ("Expected IndexOutOfBoundsException for negative index");
    } catch (IndexOutOfBoundsException& e) {
        // correct behavior
    }
    ASSERT_EQ(5, bs.nextSetBit(0)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(5, bs.nextSetBit(5)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(32, bs.nextSetBit(6)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(32, bs.nextSetBit(32)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(63, bs.nextSetBit(33)) << ("nextSetBit() returned the wrong value");

    // boundary tests
    ASSERT_EQ(63, bs.nextSetBit(63)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(64, bs.nextSetBit(64)) << ("nextSetBit() returned the wrong value");

    // at bitset element 1
    ASSERT_EQ(71, bs.nextSetBit(65)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(71, bs.nextSetBit(71)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(72, bs.nextSetBit(72)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(127, bs.nextSetBit(110)) << ("nextSetBit() returned the wrong value");

    // boundary tests
    ASSERT_EQ(127, bs.nextSetBit(127)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(128, bs.nextSetBit(128)) << ("nextSetBit() returned the wrong value");

    // at bitset element 2
    ASSERT_EQ(193, bs.nextSetBit(130)) << ("nextSetBit() returned the wrong value");

    ASSERT_EQ(193, bs.nextSetBit(191)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(193, bs.nextSetBit(192)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(193, bs.nextSetBit(193)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(450, bs.nextSetBit(194)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(450, bs.nextSetBit(255)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(450, bs.nextSetBit(256)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(450, bs.nextSetBit(450)) << ("nextSetBit() returned the wrong value");

    ASSERT_EQ(-1, bs.nextSetBit(451)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(-1, bs.nextSetBit(511)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(-1, bs.nextSetBit(512)) << ("nextSetBit() returned the wrong value");
    ASSERT_EQ(-1, bs.nextSetBit(800)) << ("nextSetBit() returned the wrong value");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testNextClearBitI() {
    BitSet bs(500);

    // ensure all the bits from 0 to bs.size() - 1 are set to true
    bs.set(0, bs.size() - 1);
    bs.set(bs.size() - 1);
    bs.clear(5);
    bs.clear(32);
    bs.clear(63);
    bs.clear(64);
    bs.clear(71, 110);
    bs.clear(127, 130);
    bs.clear(193);
    bs.clear(450);
    try {
        bs.nextClearBit(-1);
        FAIL() << ("Expected IndexOutOfBoundsException for negative index");
    } catch (IndexOutOfBoundsException& e) {
        // correct behavior
    }

    ASSERT_EQ(5, bs.nextClearBit(0)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(5, bs.nextClearBit(5)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(32, bs.nextClearBit(6)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(32, bs.nextClearBit(32)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(63, bs.nextClearBit(33)) << ("nextClearBit() returned the wrong value");

    // boundary tests
    ASSERT_EQ(63, bs .nextClearBit(63)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(64, bs .nextClearBit(64)) << ("nextClearBit() returned the wrong value");

    // at bitset element 1
    ASSERT_EQ(71, bs .nextClearBit(65)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(71, bs .nextClearBit(71)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(72, bs .nextClearBit(72)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(127, bs .nextClearBit(110)) << ("nextClearBit() returned the wrong value");

    // boundary tests
    ASSERT_EQ(127, bs .nextClearBit(127)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(128, bs .nextClearBit(128)) << ("nextClearBit() returned the wrong value");

    // at bitset element 2
    ASSERT_EQ(193, bs .nextClearBit(130)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(193, bs .nextClearBit(191)) << ("nextClearBit() returned the wrong value");

    ASSERT_EQ(193, bs .nextClearBit(192)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(193, bs .nextClearBit(193)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(450, bs .nextClearBit(194)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(450, bs .nextClearBit(255)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(450, bs .nextClearBit(256)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(450, bs .nextClearBit(450)) << ("nextClearBit() returned the wrong value");

    // bitset has 1 still the end of bs.size() -1, but calling nextClearBit
    // with any index value after the last true bit should return bs.size()
    ASSERT_EQ(512, bs .nextClearBit(451)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(512, bs .nextClearBit(511)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(512, bs .nextClearBit(512)) << ("nextClearBit() returned the wrong value");

    // if the index is larger than bs.size(), nextClearBit should return index
    ASSERT_EQ(513, bs .nextClearBit(513)) << ("nextClearBit() returned the wrong value");
    ASSERT_EQ(800, bs .nextClearBit(800)) << ("nextClearBit() returned the wrong value");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testIsEmpty() {
    BitSet bs(500);
    ASSERT_TRUE(bs.isEmpty()) << ("Test: isEmpty() returned wrong value");

    // at bitset element 0
    bs.set(3);
    ASSERT_TRUE(!bs.isEmpty()) << ("Test0: isEmpty() returned wrong value");

    // at bitset element 1
    bs.clear();
    bs.set(12);
    ASSERT_TRUE(!bs.isEmpty()) << ("Test1: isEmpty() returned wrong value");

    // at bitset element 2
    bs.clear();
    bs.set(128);
    ASSERT_TRUE(!bs.isEmpty()) << ("Test2: isEmpty() returned wrong value");

    // boundary testing
    bs.clear();
    bs.set(459);
    ASSERT_TRUE(!bs.isEmpty()) << ("Test3: isEmpty() returned wrong value");

    bs.clear();
    bs.set(511);
    ASSERT_TRUE(!bs.isEmpty()) << ("Test4: isEmpty() returned wrong value");
}

////////////////////////////////////////////////////////////////////////////////
void BitSetTest::testCardinality() {

    BitSet bs(500);
    bs.set(5);
    bs.set(32);
    bs.set(63);
    bs.set(64);
    bs.set(71, 110);
    bs.set(127, 130);
    bs.set(193);
    bs.set(450);

    ASSERT_EQ(48, bs.cardinality()) << ("cardinality() returned wrong value");

    bs.flip(0, 500);
    ASSERT_EQ(452, bs.cardinality()) << ("cardinality() returned wrong value");

    bs.clear();
    ASSERT_EQ(0, bs.cardinality()) << ("cardinality() returned wrong value");

    bs.set(0, 500);
    ASSERT_EQ(500, bs.cardinality()) << ("cardinality() returned wrong value");

    bs.clear();
    bs.set(0, 64);
    ASSERT_EQ(64, bs.cardinality()) << ("cardinality() returned wrong value");
}
