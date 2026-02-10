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

#include <gtest/gtest.h>

#include <decaf/lang/Math.h>
#include <decaf/lang/Double.h>
#include <decaf/lang/Float.h>

using namespace std;
using namespace decaf;
using namespace decaf::lang;

    class MathTest : public ::testing::Test
    {
private:

        double HYP;
        // double OPP;
        // double ADJ;

    public:

        MathTest();
        virtual ~MathTest() {}

        virtual void test_absD();
        virtual void test_absF();
        virtual void test_absI();
        virtual void test_absJ();
//        virtual void test_acosD();
//        virtual void test_asinD();
//        virtual void test_atanD();
//        virtual void test_atan2DD();
//        virtual void test_cbrt_D();
        virtual void test_ceilD();
//        virtual void test_cosD();
//        virtual void test_cosh_D();
//        virtual void test_expD();
//        virtual void test_expm1_D();
        virtual void test_floorD();
//        virtual void test_hypot_DD();
//        virtual void test_IEEEremainderDD();
//        virtual void test_logD();
//        virtual void test_log10_D();
//        virtual void test_log1p_D();
        virtual void test_maxDD();
        virtual void test_maxFF();
        virtual void test_maxII();
        virtual void test_maxJJ();
        virtual void test_minDD();
        virtual void test_minFF();
        virtual void test_minII();
        virtual void test_minJJ();
        virtual void test_powDD();
//        virtual void test_rintD();
        virtual void test_roundD();
        virtual void test_roundF();
        virtual void test_signum_D();
        virtual void test_signum_F();
//        virtual void test_sinD();
//        virtual void test_sinh_D();
        virtual void test_sqrtD();
//        virtual void test_tanD();
//        virtual void test_tanh_D();
        virtual void test_random();
        virtual void test_toRadiansD();
        virtual void test_toDegreesD();
//        virtual void test_ulp_D();
//        virtual void test_ulp_f();

    };


////////////////////////////////////////////////////////////////////////////////
MathTest::MathTest() : HYP(Math::sqrt(2.0)) { //, OPP(1.0), ADJ(1.0) {
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_absD() {
    // Test for method double decaf.lang.Math.abs(double)

    ASSERT_TRUE(( Math::abs(-1908.8976) == 1908.8976)) << ("Incorrect double abs value");
    ASSERT_TRUE(( Math::abs(1908.8976) == 1908.8976)) << ("Incorrect double abs value");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_absF() {
    // Test for method float decaf.lang.Math::abs(float)
    ASSERT_TRUE((Math::abs(-1908.8976f) == 1908.8976f)) << ("Incorrect float abs value");
    ASSERT_TRUE((Math::abs(1908.8976f) == 1908.8976f)) << ("Incorrect float abs value");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_absI() {
    // Test for method int decaf.lang.Math::abs(int)
    ASSERT_TRUE((Math::abs(-1908897) == 1908897)) << ("Incorrect int abs value");
    ASSERT_TRUE((Math::abs(1908897) == 1908897)) << ("Incorrect int abs value");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_absJ() {
    // Test for method long decaf.lang.Math::abs(long)
    ASSERT_TRUE((Math::abs(-19088976000089LL) == 19088976000089LL)) << ("Incorrect long abs value");
    ASSERT_TRUE((Math::abs(19088976000089LL) == 19088976000089LL)) << ("Incorrect long abs value");
}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_acosD() {
//    // Test for method double decaf.lang.Math::acos(double)
//    double r = Math::cos(Math::acos(ADJ / HYP));
//    long long lr = Double::doubleToLongBits(r);
//    long long t = Double::doubleToLongBits(ADJ / HYP);
//    ASSERT_TRUE(lr == t || (lr + 1) == t
//            || (lr - 1) == t) << ("Returned incorrect arc cosine");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_asinD() {
//    // Test for method double decaf.lang.Math::asin(double)
//    double r = Math::sin(Math::asin(OPP / HYP));
//    long long lr = Double::doubleToLongBits(r);
//    long long t = Double::doubleToLongBits(OPP / HYP);
//    ASSERT_TRUE(lr == t || (lr + 1) == t
//            || (lr - 1) == t) << ("Returned incorrect arc sine");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_atanD() {
//    // Test for method double decaf.lang.Math::atan(double)
//    double answer = Math::tan(Math::atan(1.0));
//    ASSERT_TRUE(//            answer <= 1.0 && answer >= 9.9999999999999983E-1) << ("Returned incorrect arc tangent: ");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_atan2DD() {
//    // Test for method double decaf.lang.Math::atan2(double, double)
//    double answer = Math::atan(Math::tan(1.0));
//    ASSERT_TRUE(//            answer <= 1.0 && answer >= 9.9999999999999983E-1) << ("Returned incorrect arc tangent: ");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_cbrt_D() {
//    //Test for special situations
//    ASSERT_TRUE(//            Double::isNaN(Math::cbrt(Double::NaN))) << ("Should return Double::NaN");
//    ASSERT_EQ(//            Math::cbrt(Double::POSITIVE_INFINITY), Double::POSITIVE_INFINITY) << ("Should return Double::POSITIVE_INFINITY");
//    ASSERT_EQ(//            Math::cbrt(Double::NEGATIVE_INFINITY), Double::NEGATIVE_INFINITY) << ("Should return Double::NEGATIVE_INFINITY");
//    ASSERT_EQ(//            Double::doubleToLongBits(0.0), //            Double::doubleToLongBits(Math::cbrt(0.0)));
//    ASSERT_EQ(Double::doubleToLongBits(+0.0), //                          Double::doubleToLongBits(Math::cbrt(+0.0)));
//    ASSERT_EQ(Double::doubleToLongBits(-0.0), //                          Double::doubleToLongBits(Math::cbrt(-0.0)));
//
//    ASSERT_EQ(Math::cbrt(27.0), 3.0) << ("Should return 3.0");
//    ASSERT_EQ(//            5.643803094122362E102, Math::cbrt(Double::MAX_VALUE)) << ("Should return 5.643803094122362E102");
//    ASSERT_EQ(0.01, Math::cbrt(0.000001)) << ("Should return 0.01");
//
//    ASSERT_EQ(-3.0, Math::cbrt(-27.0)) << ("Should return -3.0");
//    ASSERT_EQ(//            1.7031839360032603E-108, Math::cbrt(Double::MIN_VALUE)) << ("Should return 1.7031839360032603E-108");
//    ASSERT_EQ(-0.01, Math::cbrt(-0.000001)) << ("Should return -0.01");
//}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_ceilD() {
    // Test for method double decaf.lang.Math::ceil(double)
    ASSERT_EQ(79.0, Math::ceil(78.89)) << ("Incorrect ceiling for double");
    ASSERT_EQ(-78.0, Math::ceil(-78.89)) << ("Incorrect ceiling for double");
}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_cosD() {
//    // Test for method double decaf.lang.Math::cos(double)
//    ASSERT_EQ(1.0, Math::cos(0)) << ("Incorrect answer");
//    ASSERT_EQ(0.5403023058681398, Math::cos(1)) << ("Incorrect answer");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_cosh_D() {
//    // Test for special situations
//    ASSERT_TRUE(Double::isNaN(Math::cosh(Double::NaN)));
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::cosh(Double::POSITIVE_INFINITY)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::cosh(Double::NEGATIVE_INFINITY)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(1.0, Math::cosh(+0.0)) << ("Should return 1.0");
//    ASSERT_EQ(1.0, Math::cosh(-0.0)) << ("Should return 1.0");
//
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::cosh(1234.56)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::cosh(-1234.56)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            1.0000000000005, Math::cosh(0.000001)) << ("Should return 1.0000000000005");
//    ASSERT_EQ(//            1.0000000000005, Math::cosh(-0.000001)) << ("Should return 1.0000000000005");
//    ASSERT_EQ(//            5.212214351945598, Math::cosh(2.33482)) << ("Should return 5.212214351945598");
//
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::cosh(Double::MAX_VALUE)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            1.0, Math::cosh(Double::MIN_VALUE)) << ("Should return 1.0");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_expD() {
//    // Test for method double decaf.lang.Math::exp(double)
//    ASSERT_TRUE(//            Math::abs(Math::exp(4.0) -
//            Math::E * Math::E * Math::E * Math::E) < 0.1) << ("Incorrect answer returned for simple power");
//    ASSERT_TRUE(//            Math::log( Math::abs( Math::exp(5.5) ) - 5.5) < 10.0) << ("Incorrect answer returned for larger power");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_expm1_D() {
//
//    // Test for special cases
//    ASSERT_TRUE(Double::isNaN(Math::expm1(Double::NaN))) << ("Should return NaN");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::expm1(Double::POSITIVE_INFINITY)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(-1.0, //            Math::expm1(Double::NEGATIVE_INFINITY)) << ("Should return -1.0");
//    ASSERT_EQ(Double::doubleToLongBits(0.0), //                          Double::doubleToLongBits(Math::expm1(0.0)));
//    ASSERT_EQ(Double::doubleToLongBits(+0.0), //            Double::doubleToLongBits(Math::expm1(+0.0)));
//    ASSERT_EQ(Double::doubleToLongBits(-0.0), //            Double::doubleToLongBits(Math::expm1(-0.0)));
//
//    ASSERT_EQ(//            -9.999950000166666E-6, Math::expm1(-0.00001)) << ("Should return -9.999950000166666E-6");
//    ASSERT_EQ(//            1.0145103074469635E60, Math::expm1(138.16951162)) << ("Should return 1.0145103074469635E60");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, //            Math::expm1(123456789123456789123456789.4521584223)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::expm1(Double::MAX_VALUE)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(Double::MIN_VALUE, //            Math::expm1(Double::MIN_VALUE)) << ("Should return MIN_VALUE");
//}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_floorD() {
    // Test for method double decaf.lang.Math::floor(double)
    ASSERT_EQ(78.0, Math::floor(78.89)) << ("Incorrect floor for double");
    ASSERT_EQ(-79.0, Math::floor(-78.89)) << ("Incorrect floor for double");
}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_hypot_DD() {
//    // Test for special cases
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::hypot(Double::POSITIVE_INFINITY,
//                    1.0)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::hypot(Double::NEGATIVE_INFINITY,
//                    123.324)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, //            Math::hypot(-758.2587,Double::POSITIVE_INFINITY)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, //            Math::hypot(5687.21, Double::NEGATIVE_INFINITY)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::hypot(Double::POSITIVE_INFINITY,
//                    Double::NEGATIVE_INFINITY)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::hypot(Double::NEGATIVE_INFINITY,
//                    Double::POSITIVE_INFINITY)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_TRUE(Double::isNaN(Math::hypot(Double::NaN,
//            2342301.89843))) << ("Should be NaN");
//    ASSERT_TRUE(Double::isNaN(Math::hypot(-345.2680,
//            Double::NaN))) << ("Should be NaN");
//
//    ASSERT_EQ(//            2396424.905416697, Math::hypot(12322.12, -2396393.2258)) << ("Should return 2396424.905416697");
//    ASSERT_EQ(138.16958070558556, //            Math::hypot(-138.16951162, 0.13817035864)) << ("Should return 138.16958070558556");
//    ASSERT_EQ(//            1.7976931348623157E308, Math::hypot(Double::MAX_VALUE, 211370.35)) << ("Should return 1.7976931348623157E308");
//    ASSERT_EQ(5413.7185, Math::hypot(
//            -5413.7185, Double::MIN_VALUE)) << ("Should return 5413.7185");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_IEEEremainderDD() {
//    // Test for method double decaf.lang.Math::IEEEremainder(double, double)
//    ASSERT_EQ(//            0.0, Math::IEEEremainder(1.0, 1.0)) << ("Incorrect remainder returned");
//    ASSERT_TRUE(//            Math::IEEEremainder(1.32,89.765) >= 1.4705063220631647E-2 ||
//            Math::IEEEremainder(1.32, 89.765) >= 1.4705063220631649E-2) << ("Incorrect remainder returned");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_logD() {
//    // Test for method double decaf.lang.Math::log(double)
//    for( double d = 10; d >= -10; d -= 0.5 ) {
//        double answer = Math::log( Math::exp(d) );
//
//        ASSERT_TRUE(//                Math::abs( answer - d ) <= Math::abs(d * 0.00000001)) << (//                "Answer does not equal expected answer for d");
//    }
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_log10_D() {
//    // Test for special cases
//    ASSERT_TRUE(Double::isNaN(Math::log10(Double::NaN)));
//    ASSERT_TRUE(Double::isNaN(Math::log10(-2541.05745687234187532)));
//    ASSERT_TRUE(Double::isNaN(Math::log10(-0.1)));
//    ASSERT_EQ(Double::POSITIVE_INFINITY, Math::log10(Double::POSITIVE_INFINITY));
//    ASSERT_EQ(Double::NEGATIVE_INFINITY, Math::log10(0.0));
//    ASSERT_EQ(Double::NEGATIVE_INFINITY, Math::log10(+0.0));
//    ASSERT_EQ(Double::NEGATIVE_INFINITY, Math::log10(-0.0));
//
//    ASSERT_EQ(3.0, Math::log10(1000.0));
//    ASSERT_EQ(14.0, Math::log10(Math::pow(10, 14)));
//
//    ASSERT_EQ(3738956126954LL, (long long)(Math::log10(5482.2158)*1000000000000.0));
//    ASSERT_EQ(14661551142893LL, (long long)(Math::log10(458723662312872.125782332587)*1000000000000.0));
//    ASSERT_EQ(-908382862219LL, (long long)(Math::log10(0.12348583358871)*1000000000000.0));
//    ASSERT_EQ(308254715559916LL, (long long)(Math::log10(Double::MAX_VALUE)*1000000000000.0));
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_log1p_D() {
//
//	// Test for special cases
//    ASSERT_TRUE(Double::isNaN(Math::log1p(Double::NaN))) << ("Should return NaN");
//    ASSERT_TRUE(Double::isNaN(Math::log1p(-32.0482175))) << ("Should return NaN");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::log1p(Double::POSITIVE_INFINITY)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(Double::doubleToLongBits(0.0), //                          Double::doubleToLongBits(Math::log1p(0.0)));
//    ASSERT_EQ(Double::doubleToLongBits(+0.0), //                          Double::doubleToLongBits(Math::log1p(+0.0)));
//    ASSERT_EQ(Double::doubleToLongBits(-0.0), //                          Double::doubleToLongBits(Math::log1p(-0.0)));
//
//    ASSERT_EQ(-0.2941782295312541, //            Math::log1p(-0.254856327)) << ("Should return -0.2941782295312541");
//    ASSERT_EQ(7.368050685564151, //            Math::log1p(1583.542)) << ("Should return 7.368050685564151");
//    ASSERT_EQ(0.4633708685409921, //            Math::log1p(0.5894227)) << ("Should return 0.4633708685409921");
//    ASSERT_EQ(709.782712893384, //            Math::log1p(Double::MAX_VALUE)) << ("Should return 709.782712893384");
//    ASSERT_EQ(Double::MIN_VALUE, //            Math::log1p(Double::MIN_VALUE)) << ("Should return Double::MIN_VALUE");
//}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_maxDD() {
    // Test for method double decaf.lang.Math::max(double, double)
    ASSERT_EQ(1908897.6000089, Math::max(-1908897.6000089, 1908897.6000089)) << ("Incorrect double max value");
    ASSERT_EQ(1908897.6000089, Math::max(2.0, 1908897.6000089)) << ("Incorrect double max value");
    ASSERT_EQ(-2.0, Math::max(-2.0, -1908897.6000089)) << ("Incorrect double max value");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_maxFF() {
    // Test for method float decaf.lang.Math::max(float, float)
    ASSERT_TRUE(Math::max(-1908897.600f,
            1908897.600f) == 1908897.600f) << ("Incorrect float max value");
    ASSERT_TRUE(Math::max(2.0f, 1908897.600f) == 1908897.600f) << ("Incorrect float max value");
    ASSERT_TRUE(Math::max(-2.0f, -1908897.600f) == -2.0f) << ("Incorrect float max value");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_maxII() {
    // Test for method int decaf.lang.Math::max(int, int)
    ASSERT_EQ(19088976, Math::max(-19088976, 19088976)) << ("Incorrect int max value");
    ASSERT_EQ(19088976, Math::max(20, 19088976)) << ("Incorrect int max value");
    ASSERT_EQ(-20, Math::max(-20, -19088976)) << ("Incorrect int max value");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_maxJJ() {
    // Test for method long long decaf.lang.Math::max(long long, long long)
    ASSERT_EQ(19088976000089LL, Math::max(-19088976000089LL, 19088976000089LL)) << ("Incorrect long max value");
    ASSERT_EQ(19088976000089LL, Math::max(20LL, 19088976000089LL)) << ("Incorrect long max value");
    ASSERT_EQ(-20LL, Math::max(-20LL, -19088976000089LL)) << ("Incorrect long max value");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_minDD() {
    // Test for method double decaf.lang.Math::min(double, double)
    ASSERT_EQ(-1908897.6000089, Math::min(-1908897.6000089, 1908897.6000089)) << ("Incorrect double min value");
    ASSERT_EQ(2.0, Math::min(2.0, 1908897.6000089)) << ("Incorrect double min value");
    ASSERT_EQ(-1908897.6000089, Math::min(-2.0,-1908897.6000089)) << ("Incorrect double min value");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_minFF() {
    // Test for method float decaf.lang.Math::min(float, float)
    ASSERT_TRUE(Math::min(-1908897.600f,
            1908897.600f) == -1908897.600f) << ("Incorrect float min value");
    ASSERT_TRUE(Math::min(2.0f, 1908897.600f) == 2.0f) << ("Incorrect float min value");
    ASSERT_TRUE(Math::min(-2.0f, -1908897.600f) == -1908897.600f) << ("Incorrect float min value");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_minII() {
    // Test for method int decaf.lang.Math::min(int, int)
    ASSERT_EQ(-19088976, Math::min(-19088976, 19088976)) << ("Incorrect int min value");
    ASSERT_EQ(20, Math::min(20, 19088976)) << ("Incorrect int min value");
    ASSERT_EQ(-19088976, Math::min(-20, -19088976)) << ("Incorrect int min value");

}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_minJJ() {
    // Test for method long long decaf.lang.Math::min(long long, long long)
    ASSERT_EQ(-19088976000089LL, Math::min(-19088976000089LL, 19088976000089LL)) << ("Incorrect long min value");
    ASSERT_EQ(20LL, Math::min(20LL, 19088976000089LL)) << ("Incorrect long min value");
    ASSERT_EQ(-19088976000089LL, Math::min(-20LL, -19088976000089LL)) << ("Incorrect long min value");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_powDD() {
    // Test for method double decaf.lang.Math::pow(double, double)
    ASSERT_TRUE((long) Math::pow(2, 8) == 256l) << ("pow returned incorrect value");
    ASSERT_TRUE(Math::pow(2, -8) == 0.00390625) << ("pow returned incorrect value");
    ASSERT_EQ(2, (int)Math::sqrt(Math::pow(Math::sqrt(2), 4))) << ("Incorrect root returned1");
}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_rintD() {
//    // Test for method double decaf.lang.Math::rint(double)
//    ASSERT_EQ(//            3.0, Math::rint(2.9)) << ("Failed to round properly - up to odd");
//    ASSERT_TRUE(//            Double::isNaN(Math::rint(Double::NaN))) << ("Failed to round properly - NaN");
//    ASSERT_EQ(//            2.0, Math::rint(2.1)) << ("Failed to round properly down to even");
//    ASSERT_TRUE(//            Math::rint(2.5) == 2.0) << ("Failed to round properly to even");
//}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_roundD() {
    // Test for method long decaf.lang.Math::round(double)
    ASSERT_EQ(-91LL, Math::round(-90.89)) << ("Incorrect rounding of a float");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_roundF() {
    // Test for method int decaf.lang.Math::round(float)
    ASSERT_EQ(-91, Math::round(-90.89f)) << ("Incorrect rounding of a float");
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_signum_D() {
    ASSERT_TRUE(Double::isNaN(Math::signum(Double::NaN)));
    ASSERT_TRUE(Double::isNaN(Math::signum(Double::NaN)));
    ASSERT_EQ(Double::doubleToLongBits(0.0), Double::doubleToLongBits(Math::signum(0.0)));
    ASSERT_EQ(Double::doubleToLongBits(+0.0), Double::doubleToLongBits(Math::signum(+0.0)));
    ASSERT_EQ(Double::doubleToLongBits(-0.0), Double::doubleToLongBits(Math::signum(-0.0)));

    ASSERT_EQ(1.0, Math::signum(253681.2187962));
    ASSERT_EQ(-1.0, Math::signum(-125874693.56));
    ASSERT_EQ(1.0, Math::signum(1.2587E-308));
    ASSERT_EQ(-1.0, Math::signum(-1.2587E-308));

    ASSERT_EQ(1.0, Math::signum(Double::MAX_VALUE));
    ASSERT_EQ(1.0, Math::signum(Double::MIN_VALUE));
    ASSERT_EQ(-1.0, Math::signum(-Double::MAX_VALUE));
    ASSERT_EQ(-1.0, Math::signum(-Double::MIN_VALUE));
    ASSERT_EQ(1.0, Math::signum(Double::POSITIVE_INFINITY));
    ASSERT_EQ(-1.0, Math::signum(Double::NEGATIVE_INFINITY));
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_signum_F() {
    ASSERT_TRUE(Float::isNaN(Math::signum(Float::NaN)));
    ASSERT_EQ(Float::floatToIntBits(0.0f), Float::floatToIntBits(Math::signum(0.0f)));
    ASSERT_EQ(Float::floatToIntBits(+0.0f), Float::floatToIntBits(Math::signum(+0.0f)));
    ASSERT_EQ(Float::floatToIntBits(-0.0f), Float::floatToIntBits(Math::signum(-0.0f)));

    ASSERT_EQ(1.0f, Math::signum(253681.2187962f));
    ASSERT_EQ(-1.0f, Math::signum(-125874693.56f));
    ASSERT_EQ(1.0f, Math::signum(1.2587E-11f));
    ASSERT_EQ(-1.0f, Math::signum(-1.2587E-11f));

    ASSERT_EQ(1.0f, Math::signum(Float::MAX_VALUE));
    ASSERT_EQ(1.0f, Math::signum(Float::MIN_VALUE));
    ASSERT_EQ(-1.0f, Math::signum(-Float::MAX_VALUE));
    ASSERT_EQ(-1.0f, Math::signum(-Float::MIN_VALUE));
    ASSERT_EQ(1.0f, Math::signum(Float::POSITIVE_INFINITY));
    ASSERT_EQ(-1.0f, Math::signum(Float::NEGATIVE_INFINITY));
}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_sinD() {
//    // Test for method double decaf.lang.Math::sin(double)
//    ASSERT_EQ(//        0.0, Math::sin(0)) << ("Incorrect answer");
//    ASSERT_EQ(//        0.8414709848078965, Math::sin(1)) << ("Incorrect answer");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_sinh_D() {
//    // Test for special situations
//    ASSERT_TRUE(Double::isNaN(Math::sinh(Double::NaN))) << ("Should return NaN");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::sinh(Double::POSITIVE_INFINITY)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            Double::NEGATIVE_INFINITY, Math::sinh(Double::NEGATIVE_INFINITY)) << ("Should return NEGATIVE_INFINITY");
//    ASSERT_EQ(Double::doubleToLongBits(0.0), //            Double::doubleToLongBits(Math::sinh(0.0)));
//    ASSERT_EQ(Double::doubleToLongBits(+0.0), //            Double::doubleToLongBits(Math::sinh(+0.0)));
//    ASSERT_EQ(Double::doubleToLongBits(-0.0), //            Double::doubleToLongBits(Math::sinh(-0.0)));
//
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::sinh(1234.56)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(//            Double::NEGATIVE_INFINITY, Math::sinh(-1234.56)) << ("Should return NEGATIVE_INFINITY");
//    ASSERT_EQ(//            1.0000000000001666E-6, Math::sinh(0.000001)) << ("Should return 1.0000000000001666E-6");
//    ASSERT_EQ(//            -1.0000000000001666E-6, Math::sinh(-0.000001)) << ("Should return -1.0000000000001666E-6");
//    ASSERT_EQ(//             5.11538644196386, Math::sinh( 2.33482 )) << ("Should return 5.11538644196386");
//    ASSERT_EQ(//            Double::POSITIVE_INFINITY, Math::sinh(Double::MAX_VALUE)) << ("Should return POSITIVE_INFINITY");
//    ASSERT_EQ(4.9E-324, //            Math::sinh(Double::MIN_VALUE)) << ("Should return 4.9E-324");
//}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_sqrtD() {
    // Test for method double decaf.lang.Math::sqrt(double)
    ASSERT_EQ(7.0, Math::sqrt(49.0)) << ("Incorrect root returned2");
}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_tanD() {
//    // Test for method double decaf.lang.Math::tan(double)
//    ASSERT_EQ(//            0.0, Math::tan(0)) << ("Incorrect answer");
//    ASSERT_EQ(//            1.5574077246549023, Math::tan(1)) << ("Incorrect answer");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_tanh_D() {
//    // Test for special situations
//    ASSERT_TRUE(Double::isNaN(Math::tanh(Double::NaN))) << ("Should return NaN");
//    ASSERT_EQ(+1.0, //            Math::tanh(Double::POSITIVE_INFINITY)) << ("Should return +1.0");
//    ASSERT_EQ(-1.0, //            Math::tanh(Double::NEGATIVE_INFINITY)) << ("Should return -1.0");
//    ASSERT_EQ(Double::doubleToLongBits(0.0), //            Double::doubleToLongBits(Math::tanh(0.0)));
//    ASSERT_EQ(Double::doubleToLongBits(+0.0), //            Double::doubleToLongBits(Math::tanh(+0.0)));
//    ASSERT_EQ(Double::doubleToLongBits(-0.0), //            Double::doubleToLongBits(Math::tanh(-0.0)));
//
//    ASSERT_EQ(1.0, Math::tanh(1234.56)) << ("Should return 1.0");
//    ASSERT_EQ(-1.0, Math::tanh(-1234.56)) << ("Should return -1.0");
//    ASSERT_EQ(//            9.999999999996666E-7, Math::tanh(0.000001)) << ("Should return 9.999999999996666E-7");
//    ASSERT_EQ(0.981422884124941, //            Math::tanh(2.33482)) << ("Should return 0.981422884124941");
//    ASSERT_EQ(//            1.0, Math::tanh(Double::MAX_VALUE)) << ("Should return 1.0");
//
//    ASSERT_EQ(4.9E-324, //            Math::tanh(Double::MIN_VALUE)) << ("Should return 4.9E-324");
//}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_random() {
    // There isn't a place for these tests so just stick them here
    ASSERT_EQ(4613303445314885481LL, Double::doubleToLongBits(Math::E)) << ("Wrong value E");
    ASSERT_EQ(4614256656552045848LL, Double::doubleToLongBits(Math::PI)) << ("Wrong value PI");

    for (int i = 500; i >= 0; i--) {
        double d = Math::random();
        ASSERT_TRUE(d >= 0.0 && d < 1.0) << ("Generated number is out of range: ");
    }
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_toRadiansD() {
    for (double d = 500; d >= 0; d -= 1.0) {
        double converted = Math::toDegrees(Math::toRadians(d));
        ASSERT_TRUE(converted >= d * 0.99999999 && converted <= d * 1.00000001) << ("Converted number not equal to original. d ");
    }
}

////////////////////////////////////////////////////////////////////////////////
void MathTest::test_toDegreesD() {
    for (double d = 500; d >= 0; d -= 1.0) {
        double converted = Math::toRadians(Math::toDegrees(d));
        ASSERT_TRUE(converted >= d * 0.99999999 && converted <= d * 1.00000001) << ("Converted number not equal to original. d ");
    }
}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_ulp_D() {
//    // Test for special cases
//    ASSERT_TRUE(Double::isNaN(Math::ulp(Double::NaN))) << ("Should return NaN");
//    ASSERT_EQ(Double::POSITIVE_INFINITY, //            Math::ulp(Double::POSITIVE_INFINITY)) << ("Returned incorrect value");
//    ASSERT_EQ(Double::POSITIVE_INFINITY, //            Math::ulp(Double::NEGATIVE_INFINITY)) << ("Returned incorrect value");
//    ASSERT_EQ(//            Double::MIN_VALUE, Math::ulp(0.0)) << ("Returned incorrect value");
//    ASSERT_EQ(Double::MIN_VALUE, //            Math::ulp(+0.0)) << ("Returned incorrect value");
//    ASSERT_EQ(Double::MIN_VALUE, //            Math::ulp(-0.0)) << ("Returned incorrect value");
//    ASSERT_EQ(Math::pow(2, 971), //            Math::ulp(Double::MAX_VALUE)) << ("Returned incorrect value");
//    ASSERT_EQ(Math::pow(2, 971), //            Math::ulp(-Double::MAX_VALUE)) << ("Returned incorrect value");
//
//    ASSERT_EQ(//            Double::MIN_VALUE, Math::ulp(Double::MIN_VALUE)) << ("Returned incorrect value");
//    ASSERT_EQ(//            Double::MIN_VALUE, Math::ulp(-Double::MIN_VALUE)) << ("Returned incorrect value");
//
//    ASSERT_EQ(2.220446049250313E-16, //            Math::ulp(1.0)) << ("Returned incorrect value");
//    ASSERT_EQ(2.220446049250313E-16, //            Math::ulp(-1.0)) << ("Returned incorrect value");
//    ASSERT_EQ(2.2737367544323206E-13, //            Math::ulp(1153.0)) << ("Returned incorrect value");
//}

////////////////////////////////////////////////////////////////////////////////
//void MathTest::test_ulp_f() {
//    // Test for special cases
//    ASSERT_TRUE(Float::isNaN(Math::ulp(Float::NaN))) << ("Should return NaN");
//    ASSERT_EQ(//            Float::POSITIVE_INFINITY, Math::ulp(Float::POSITIVE_INFINITY)) << ("Returned incorrect value");
//    ASSERT_EQ(//            Float::POSITIVE_INFINITY, Math::ulp(Float::NEGATIVE_INFINITY)) << ("Returned incorrect value");
//    ASSERT_EQ(//            Float::MIN_VALUE, Math::ulp(0.0f)) << ("Returned incorrect value");
//    ASSERT_EQ(Float::MIN_VALUE, Math
//            ::ulp(+0.0f)) << ("Returned incorrect value");
//    ASSERT_EQ(//            Math::ulp(-0.0f), Float::MIN_VALUE) << ("Returned incorrect value");
//    ASSERT_EQ(//            2.028241E31f, Math::ulp(Float::MAX_VALUE)) << ("Returned incorrect value");
//    ASSERT_EQ(//            2.028241E31f, Math::ulp(-Float::MAX_VALUE)) << ("Returned incorrect value");
//
//    ASSERT_EQ(//            1.4E-45f, Math::ulp( Float::MIN_VALUE )) << ("Returned incorrect value");
//    ASSERT_EQ(//            1.4E-45f, Math::ulp( -Float::MIN_VALUE )) << ("Returned incorrect value");
//
//    ASSERT_EQ(//            1.1920929E-7f, Math::ulp(1.0f)) << ("Returned incorrect value");
//    ASSERT_EQ(1.1920929E-7f, //            Math::ulp(-1.0f)) << ("Returned incorrect value");
//    ASSERT_EQ(//            1.2207031E-4f, Math::ulp(1153.0f)) << ("Returned incorrect value");
//    ASSERT_EQ(//            5.6E-45f, Math::ulp(9.403954E-38f)) << ("Returned incorrect value");
//}

TEST_F(MathTest, test_absD) { test_absD(); }
TEST_F(MathTest, test_absF) { test_absF(); }
TEST_F(MathTest, test_absI) { test_absI(); }
TEST_F(MathTest, test_absJ) { test_absJ(); }
TEST_F(MathTest, test_ceilD) { test_ceilD(); }
TEST_F(MathTest, test_floorD) { test_floorD(); }
TEST_F(MathTest, test_maxDD) { test_maxDD(); }
TEST_F(MathTest, test_maxFF) { test_maxFF(); }
TEST_F(MathTest, test_maxII) { test_maxII(); }
TEST_F(MathTest, test_maxJJ) { test_maxJJ(); }
TEST_F(MathTest, test_minDD) { test_minDD(); }
TEST_F(MathTest, test_minFF) { test_minFF(); }
TEST_F(MathTest, test_minII) { test_minII(); }
TEST_F(MathTest, test_minJJ) { test_minJJ(); }
TEST_F(MathTest, test_powDD) { test_powDD(); }
TEST_F(MathTest, test_roundD) { test_roundD(); }
TEST_F(MathTest, test_roundF) { test_roundF(); }
TEST_F(MathTest, test_signum_D) { test_signum_D(); }
TEST_F(MathTest, test_signum_F) { test_signum_F(); }
TEST_F(MathTest, test_sqrtD) { test_sqrtD(); }
TEST_F(MathTest, test_random) { test_random(); }
TEST_F(MathTest, test_toRadiansD) { test_toRadiansD(); }
TEST_F(MathTest, test_toDegreesD) { test_toDegreesD(); }
