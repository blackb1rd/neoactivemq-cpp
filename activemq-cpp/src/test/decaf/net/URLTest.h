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

#ifndef _DECAF_NET_URLTEST_H_
#define _DECAF_NET_URLTEST_H_

#include <gtest/gtest.h>
namespace decaf {
namespace net {

    class URLTest : public ::testing::Test {
public:

        URLTest();
        virtual ~URLTest();

        void testConstructor1();
        void testConstructor2();
        void testConstructor3();
        void testConstructor4();
        void testEquals();
        void testSameFile();
        void testToString();
        void testToExternalForm();
        void testGetFile();
        void testGetHost();
        void testGetPort();
        void testGetDefaultPort();
        void testGetProtocol();
        void testGetRef();
        void testGetAuthority();
        void testToURI();
        void testURLStreamHandlerParseURL();
        void testUrlParts();
        void testFileEqualsWithEmptyHost();
        void testOmittedHost();
        void testNoHost();
        void testNoPath();
        void testEmptyHostAndNoPath();
        void testNoHostAndNoPath();
        void testAtSignInUserInfo();
        void testUserNoPassword();
        void testUserNoPasswordExplicitPort();
        void testUserPasswordHostPort();
        void testUserPasswordEmptyHostPort();
        void testUserPasswordEmptyHostEmptyPort();
        void testPathOnly();
        void testQueryOnly();
        void testFragmentOnly();
        void testAtSignInPath();
        void testColonInPath();
        void testSlashInQuery();
        void testQuestionMarkInQuery();
        void testAtSignInQuery();
        void testColonInQuery();
        void testQuestionMarkInFragment();
        void testColonInFragment();
        void testSlashInFragment();
        void testSlashInFragmentCombiningConstructor();
        void testHashInFragment();
        void testEmptyPort();
        void testNonNumericPort();
        void testNegativePort();
        void testNegativePortEqualsPlaceholder();
        void testRelativePathOnQuery();
        void testRelativeFragmentOnQuery();
        void testPathContainsRelativeParts();
        void testRelativePathAndFragment();
        void testRelativeParentDirectory();
        void testRelativeChildDirectory();
        void testRelativeRootDirectory();
        void testRelativeFullUrl();
        void testRelativeDifferentScheme();
        void testRelativeDifferentAuthority();
        void testRelativeWithScheme();
        void testMalformedUrlsRefusedByFirefoxAndChrome();
        void testRfc1808NormalExamples();
        void testRfc1808AbnormalExampleTooManyDotDotSequences();
        void testRfc1808AbnormalExampleRemoveDotSegments();
        void testRfc1808AbnormalExampleNonsensicalDots();
        void testRfc1808AbnormalExampleRelativeScheme();
        void testRfc1808AbnormalExampleQueryOrFragmentDots();
        void testSquareBracketsInUserInfo();
        void testComposeUrl();
        void testComposeUrlWithNullHost();
        void testFileUrlExtraLeadingSlashes();
        void testFileUrlWithAuthority();
        void testEmptyAuthority();
        void testHttpUrlExtraLeadingSlashes();
        void testFileUrlRelativePath();
        void testFileUrlDottedPath();
        void testParsingDotAsHostname();
        void testSquareBracketsWithIPv4();
        void testSquareBracketsWithHostname();
        void testIPv6WithoutSquareBrackets();
        void testIpv6WithSquareBrackets();
        void testEqualityWithNoPath();
        void testUrlDoesNotEncodeParts();
        void testSchemeCaseIsCanonicalized();
        void testEmptyAuthorityWithPath();
        void testEmptyAuthorityWithQuery();
        void testEmptyAuthorityWithFragment();
        void testCombiningConstructorsMakeRelativePathsAbsolute();
        void testCombiningConstructorsDoNotMakeEmptyPathsAbsolute();
        void testPartContainsSpace();
        void testUnderscore();

    };

}}

#endif /* _DECAF_NET_URLTEST_H_ */
