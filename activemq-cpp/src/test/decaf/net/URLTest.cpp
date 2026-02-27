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

#include <decaf/lang/Boolean.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/exceptions/SecurityException.h>
#include <decaf/lang/exceptions/StringIndexOutOfBoundsException.h>
#include <decaf/net/URI.h>
#include <decaf/net/URL.h>
#include <decaf/net/URLStreamHandler.h>
#include <decaf/net/URLStreamHandlerFactory.h>

using namespace std;
using namespace decaf;
using namespace decaf::net;
using namespace decaf::lang;
using namespace decaf::lang::exceptions;

class URLTest : public ::testing::Test
{
public:
    URLTest();
    virtual ~URLTest();

    void testOmittedHost();
};

////////////////////////////////////////////////////////////////////////////////
namespace
{

class MyURLStreamHandler : public URLStreamHandler
{
protected:
    virtual URLConnection* openConnection(const URL& url)
    {
        return NULL;
    }

    using URLStreamHandler::openConnection;

public:
    void parse(URL& url, const String& spec, int start, int end)
    {
        parseURL(url, spec, start, end);
    }
};

class MyURLStreamHandlerFactory : public URLStreamHandlerFactory
{
public:
    virtual URLStreamHandler* createURLStreamHandler(const std::string& protocol)
    {
        return new MyURLStreamHandler();
    }
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
URLTest::URLTest()
{
}

////////////////////////////////////////////////////////////////////////////////
URLTest::~URLTest()
{
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testConstructor1)
{
    // Tests for multiple URL instantiation basic parsing test
    URL a("http://www.yahoo1.com:8080/dir1/dir2/test.cgi?point1.html#anchor1");
    ASSERT_EQ(String("http"), a.getProtocol())
        << ("a returns a wrong protocol");
    ASSERT_EQ(String("www.yahoo1.com"), a.getHost())
        << ("a returns a wrong host");
    ASSERT_EQ(8080, a.getPort()) << ("a returns a wrong port");
    ASSERT_EQ(String("/dir1/dir2/test.cgi?point1.html"), a.getFile())
        << ("a returns a wrong file");
    ASSERT_EQ(String("anchor1"), a.getRef()) << ("a returns a wrong anchor");

    // test for no file
    URL b("http://www.yahoo2.com:9999");
    ASSERT_EQ(String("http"), b.getProtocol())
        << ("b returns a wrong protocol");
    ASSERT_EQ(String("www.yahoo2.com"), b.getHost())
        << ("b returns a wrong host");
    ASSERT_EQ(9999, b.getPort()) << ("b returns a wrong port");
    ASSERT_TRUE(b.getFile().equals("")) << ("b returns a wrong file");
    ASSERT_TRUE(b.getRef().isEmpty()) << ("b returns a wrong anchor");

    // test for no port
    URL c("http://www.yahoo3.com/dir1/dir2/test.cgi?point1.html#anchor1");
    ASSERT_EQ(String("http"), c.getProtocol())
        << ("c returns a wrong protocol");
    ASSERT_EQ(String("www.yahoo3.com"), c.getHost())
        << ("c returns a wrong host");
    ASSERT_EQ(-1, c.getPort()) << ("c returns a wrong port");
    ASSERT_EQ(String("/dir1/dir2/test.cgi?point1.html"), c.getFile())
        << ("c returns a wrong file");
    ASSERT_EQ(String("anchor1"), c.getRef()) << ("c returns a wrong anchor");

    // test for no port
    URL d("file://www.yahoo3.com/dir1/dir2/test.cgi#anchor1");
    ASSERT_EQ(String("file"), d.getProtocol())
        << ("d returns a wrong protocol");
    ASSERT_EQ(String("www.yahoo3.com"), d.getHost())
        << ("d returns a wrong host");
    ASSERT_EQ(-1, d.getPort()) << ("d returns a wrong port");
    ASSERT_EQ(String("/dir1/dir2/test.cgi"), d.getFile())
        << ("d returns a wrong file");
    ASSERT_EQ(String("anchor1"), d.getRef()) << ("d returns a wrong anchor");

    // test for no file, no port
    URL e("http://www.yahoo4.com/");
    ASSERT_EQ(String("http"), e.getProtocol())
        << ("e returns a wrong protocol");
    ASSERT_EQ(String("www.yahoo4.com"), e.getHost())
        << ("e returns a wrong host");
    ASSERT_EQ(-1, e.getPort()) << ("e returns a wrong port");
    ASSERT_EQ(String("/"), e.getFile()) << ("e returns a wrong file");
    ASSERT_TRUE(e.getRef().isEmpty()) << ("e returns a wrong anchor");

    // test for no file, no port
    URL f("file://www.yahoo4.com/");
    ASSERT_EQ(String("file"), f.getProtocol())
        << ("f returns a wrong protocol");
    ASSERT_EQ(String("www.yahoo4.com"), f.getHost())
        << ("f returns a wrong host");
    ASSERT_EQ(-1, f.getPort()) << ("f returns a wrong port");
    ASSERT_EQ(String("/"), f.getFile()) << ("f returns a wrong file");
    ASSERT_TRUE(f.getRef().isEmpty()) << ("f returns a wrong anchor");

    // test for no file, no port
    URL g("file://www.yahoo4.com");
    ASSERT_EQ(String("file"), g.getProtocol())
        << ("g returns a wrong protocol");
    ASSERT_EQ(String("www.yahoo4.com"), g.getHost())
        << ("g returns a wrong host");
    ASSERT_EQ(-1, g.getPort()) << ("g returns a wrong port");
    ASSERT_TRUE(g.getFile().equals("")) << ("g returns a wrong file");
    ASSERT_TRUE(g.getRef().isEmpty()) << ("g returns a wrong anchor");

    // test for non-port ":" and wierd characters occurrences
    URL h("http://www.yahoo5.com/di!@$%^&*()_+r1/di:::r2/"
          "test.cgi?point1.html#anchor1");
    ASSERT_EQ(String("http"), h.getProtocol())
        << ("h returns a wrong protocol");
    ASSERT_EQ(String("www.yahoo5.com"), h.getHost())
        << ("h returns a wrong host");
    ASSERT_EQ(-1, h.getPort()) << ("h returns a wrong port");
    ASSERT_EQ(String("/di!@$%^&*()_+r1/di:::r2/test.cgi?point1.html"),
              h.getFile())
        << ("h returns a wrong file");
    ASSERT_EQ(String("anchor1"), h.getRef()) << ("h returns a wrong anchor");

    URL i("file:/testing.tst");
    ASSERT_EQ(String("file"), i.getProtocol())
        << ("i returns a wrong protocol");
    ASSERT_TRUE(i.getHost().equals("")) << ("i returns a wrong host");
    ASSERT_EQ(-1, i.getPort()) << ("i returns a wrong port");
    ASSERT_EQ(String("/testing.tst"), i.getFile())
        << ("i returns a wrong file");
    ASSERT_TRUE(i.getRef().isEmpty()) << ("i returns a wrong anchor");

    URL j("file:testing.tst");
    ASSERT_EQ(String("file"), j.getProtocol())
        << ("j returns a wrong protocol");
    ASSERT_TRUE(j.getHost().equals("")) << ("j returns a wrong host");
    ASSERT_EQ(-1, j.getPort()) << ("j returns a wrong port");
    ASSERT_EQ(String("testing.tst"), j.getFile()) << ("j returns a wrong file");
    ASSERT_TRUE(j.getRef().isEmpty()) << ("j returns a wrong anchor");

    URL l("http://host:/file");
    ASSERT_EQ(-1, l.getPort()) << ("l return a wrong port");

    URL m("file:../../file.txt");
    ASSERT_TRUE(m.getFile().equals("../../file.txt"))
        << (std::string("m returns a wrong file: ") + m.getFile().toString());

    URL n("http://[fec0::1:20d:60ff:fe24:7410]:35/file.txt");
    ASSERT_TRUE(n.getProtocol().equals("http"))
        << ("n returns a wrong protocol ");
    ASSERT_TRUE(n.getHost().equals("[fec0::1:20d:60ff:fe24:7410]"))
        << ("n returns a wrong host ");
    ASSERT_TRUE(n.getPort() == 35) << ("n returns a wrong port ");
    ASSERT_TRUE(n.getFile().equals("/file.txt")) << ("n returns a wrong file ");
    ASSERT_TRUE(n.getRef().isEmpty()) << ("n returns a wrong anchor ");

    URL o("file://[fec0::1:20d:60ff:fe24:7410]/file.txt#sogood");
    ASSERT_TRUE(o.getProtocol().equals("file"))
        << ("o returns a wrong protocol ");
    ASSERT_TRUE(o.getHost().equals("[fec0::1:20d:60ff:fe24:7410]"))
        << ("o returns a wrong host ");
    ASSERT_TRUE(o.getPort() == -1) << ("o returns a wrong port ");
    ASSERT_TRUE(o.getFile().equals("/file.txt")) << ("o returns a wrong file ");
    ASSERT_TRUE(o.getRef().equals("sogood")) << ("o returns a wrong anchor ");

    URL p("file://[fec0::1:20d:60ff:fe24:7410]");
    ASSERT_TRUE(p.getProtocol().equals("file"))
        << ("p returns a wrong protocol ");
    ASSERT_TRUE(p.getHost().equals("[fec0::1:20d:60ff:fe24:7410]"))
        << ("p returns a wrong host ");
    ASSERT_TRUE(p.getPort() == -1) << ("p returns a wrong port ");

    // TODO internal representation store '//' authority as "" which can't be
    // distinguished
    //      from no authority versions.
    //    URL q("file:////file.txt");
    //    ASSERT_TRUE(q.getAuthority().isEmpty()) << ("q returns a wrong
    //    authority "); ASSERT_EQ(String("////file.txt"), q.getFile()) << ("q
    //    returns a wrong file ");

    URL r("file:///file.txt");
    ASSERT_TRUE(r.getAuthority().equals("")) << ("r returns a wrong authority");
    ASSERT_TRUE(r.getFile().equals("/file.txt")) << ("r returns a wrong file ");

    ASSERT_THROW(URL("http://www.yahoo5.com::22/dir1/di:::r2/"
                     "test.cgi?point1.html#anchor1"),
                 MalformedURLException)
        << ("Should have thrown MalformedURLException");

    // unknown protocol
    ASSERT_THROW(URL("myProtocol://www.yahoo.com:22"), MalformedURLException)
        << ("Should have thrown MalformedURLException");

    // no protocol
    ASSERT_THROW(URL("www.yahoo.com"), MalformedURLException)
        << ("Should have thrown MalformedURLException");

    try
    {
        // No leading or trailing spaces.
        URL u1("file:/some/path");
        ASSERT_EQ(10, u1.getFile().length()) << ("5 got wrong file length1");

        // Leading spaces.
        URL u2("  file:/some/path");
        ASSERT_EQ(10, u2.getFile().length()) << ("5 got wrong file length2");

        // Trailing spaces.
        URL u3("file:/some/path  ");
        ASSERT_EQ(10, u3.getFile().length()) << ("5 got wrong file length3");

        // Leading and trailing.
        URL u4("  file:/some/path ");
        ASSERT_EQ(10, u4.getFile().length()) << ("5 got wrong file length4");

        // in-place spaces.
        URL u5("  file:  /some/path ");
        ASSERT_EQ(12, u5.getFile().length()) << ("5 got wrong file length5");
    }
    catch (MalformedURLException& e)
    {
        FAIL() << ("5 Did not expect the exception ");
    }
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testConstructor2)
{
    URL u("http", "www.yahoo.com", "test.html#foo");
    ASSERT_TRUE(u.getProtocol().equals("http"));
    ASSERT_TRUE(u.getHost().equals("www.yahoo.com"));
    ASSERT_TRUE(-1 == u.getPort());
    // TODO
    //    ASSERT_EQ(String("test.html"), u.getFile());
    ASSERT_TRUE(u.getRef().equals("foo"));

    // Strange behavior in reference, the hostname contains a ':' so it gets
    // wrapped in '[', ']'
    URL testURL("http", "www.apache.org:8080", "test.html#anch");
    ASSERT_EQ(String("http"), testURL.getProtocol()) << ("wrong protocol");
    ASSERT_EQ(String("[www.apache.org:8080]"), testURL.getHost())
        << ("wrong host");
    ASSERT_EQ(-1, testURL.getPort()) << ("wrong port");
    // TODO
    // ASSERT_EQ(String("test.html"), testURL.getFile()) << ("wrong file");
    ASSERT_EQ(String("anch"), testURL.getRef()) << ("wrong anchor");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testConstructor3)
{
    URL u("http", "www.yahoo.com", 8080, "test.html#foo");
    ASSERT_EQ(String("http"), u.getProtocol())
        << ("SSIS returns a wrong protocol");
    ASSERT_EQ(String("www.yahoo.com"), u.getHost())
        << ("SSIS returns a wrong host");
    ASSERT_EQ(8080, u.getPort()) << ("SSIS returns a wrong port");
    // TODO
    //    ASSERT_EQ(String("test.html"), u.getFile()) << ("SSIS returns a wrong
    //    file");
    ASSERT_TRUE(u.getRef().equals("foo")) << ("SSIS returns a wrong anchor: ");

    ASSERT_NO_THROW(URL("http", "apache.org", 123456789, "file"));

    ASSERT_THROW(URL("http", "apache.org", -123, "file"), MalformedURLException)
        << ("Should have thrown MalformedURLException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testConstructor4)
{
    URL context("http://www.yahoo.com");

    // basic ones
    URL a(context, "file.java", new MyURLStreamHandler);
    ASSERT_EQ(String("http"), a.getProtocol())
        << ("1 returns a wrong protocol");
    ASSERT_EQ(String("www.yahoo.com"), a.getHost())
        << ("1 returns a wrong host");
    ASSERT_EQ(-1, a.getPort()) << ("1 returns a wrong port");
    ASSERT_EQ(String("/file.java"), a.getFile()) << ("1 returns a wrong file");
    ASSERT_EQ(String(), a.getRef()) << ("1 returns a wrong anchor");

    URL b(context, "systemresource:/+/FILE0/test.java", new MyURLStreamHandler);
    ASSERT_EQ(String("systemresource"), b.getProtocol())
        << ("2 returns a wrong protocol");
    ASSERT_TRUE(b.getHost().equals("")) << ("2 returns a wrong host");
    ASSERT_EQ(-1, b.getPort()) << ("2 returns a wrong port");
    ASSERT_EQ(String("/+/FILE0/test.java"), b.getFile())
        << ("2 returns a wrong file");
    ASSERT_TRUE(b.getRef().equals("")) << ("2 returns a wrong anchor");

    URL c(context, "dir1/dir2/../file.java", NULL);
    ASSERT_EQ(String("http"), c.getProtocol())
        << ("3 returns a wrong protocol");
    ASSERT_EQ(String("www.yahoo.com"), c.getHost())
        << ("3 returns a wrong host");
    ASSERT_EQ(-1, c.getPort()) << ("3 returns a wrong port");
    ASSERT_EQ(String("/dir1/dir2/../file.java"), c.getFile())
        << ("3 returns a wrong file");
    ASSERT_TRUE(c.getRef().equals("")) << ("3 returns a wrong anchor");

    // test for question mark processing
    URL d("http://www.foo.com/d0/d1/d2/cgi-bin?foo=bar/baz");

    // test for relative file and out of bound "/../" processing
    URL e(d, "../dir1/dir2/../file.java", new MyURLStreamHandler);
    ASSERT_EQ(String("/d0/d1/dir1/file.java"), e.getFile())
        << ("A) returns a wrong file: ");

    // TODO
    // test for absolute and relative file processing
    //    URL f(d, "/../dir1/dir2/../file.java", NULL);
    //    ASSERT_EQ(String("/../dir1/dir2/../file.java"), f.getFile()) << ("B)
    //    returns a wrong file");

    ASSERT_NO_THROW(URL("http://www.ibm.com"));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testEquals)
{
    URL u("http://www.apache.org:8080/dir::23??????????test.html");
    URL u1("http://www.apache.org:8080/dir::23??????????test.html");
    ASSERT_TRUE(u.equals(u1))
        << ("A) equals returns false for two identical URLs");

    URL u2("file://www.apache.org:8080/dir::23??????????test.html");
    ASSERT_TRUE(!u2.equals(u1)) << ("Returned true for non-equal URLs");

    URL a("file", "", 0, "/test.txt");
    URL b("file", "", 0, "/test.txt");
    ASSERT_TRUE(a.equals(b));

    URL c("file", "first.invalid", 0, "/test.txt");
    URL d("file", "second.invalid", 0, "/test.txt");
    ASSERT_TRUE(!c.equals(d));

    // TODO
    //    URL e("file", "harmony.apache.org", 0, "/test.txt");
    //    URL f("file", "www.apache.org", 0, "/test.txt");
    //    ASSERT_TRUE(e.equals(f));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testSameFile)
{
    URL a("http://www.yahoo.com");
    URL b("http", "www.yahoo.com", "");
    ASSERT_TRUE(a.sameFile(b)) << ("Should be the same1");

    URL c("http://www.yahoo.com/dir1/dir2/test.html#anchor1");
    URL d("http://www.yahoo.com/dir1/dir2/test.html#anchor2");
    ASSERT_TRUE(c.sameFile(d)) << ("Should be the same ");

    // TODO
    //    URL e("file", "", -1, "/d:/somedir/");
    //    URL f("file:/d:/somedir/");
    //    ASSERT_TRUE(!e.sameFile(f));

    URL g("file:///anyfile");
    URL h("file://localhost/anyfile");
    ASSERT_TRUE(g.sameFile(h));

    URL i("http:///anyfile");
    URL j("http://localhost/anyfile");
    ASSERT_TRUE(!i.sameFile(j));

    // TODO
    //    URL k("ftp:///anyfile");
    //    URL l("ftp://localhost/anyfile");
    //    ASSERT_TRUE(!k.sameFile(l));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testToString)
{
    URL a("http://www.yahoo2.com:9999");
    URL b("http://www.yahoo1.com:8080/dir1/dir2/test.cgi?point1.html#anchor1");

    ASSERT_EQ(std::string("http://www.yahoo1.com:8080/dir1/dir2/"
                          "test.cgi?point1.html#anchor1"),
              b.toString())
        << ("a) Does not return the right url string");

    ASSERT_EQ(std::string("http://www.yahoo2.com:9999"), a.toString())
        << ("b) Does not return the right url string");

    ASSERT_TRUE(a.equals(URL(a.toString())))
        << ("c) Does not return the right url string");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testToExternalForm)
{
    URL b("http://www.yahoo2.com:9999");
    URL a("http://www.yahoo1.com:8080/dir1/dir2/test.cgi?point1.html#anchor1");

    ASSERT_EQ(String("http://www.yahoo1.com:8080/dir1/dir2/"
                     "test.cgi?point1.html#anchor1"),
              a.toExternalForm())
        << ("a) Does not return the right url string");

    ASSERT_EQ(String("http://www.yahoo2.com:9999"), b.toExternalForm())
        << ("b) Does not return the right url string");

    ASSERT_TRUE(a.equals(URL(a.toExternalForm())))
        << ("c) Does not return the right url string");

    URL c("http:index");
    ASSERT_EQ(String("http:index"), c.toExternalForm())
        << ("2 wrong external form");

    URL d("http", "", "index");
    ASSERT_EQ(String("http:index"), d.toExternalForm())
        << ("2 wrong external form");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testGetFile)
{
    // TODO
    //    URL a("http", "www.yahoo.com:8080", 1233,
    //    "test/!@$%^&*/test.html#foo");
    //    ASSERT_EQ(String("test/!@$%^&*/test.html"), a.getFile()) << ("returns
    //    a wrong file");
    URL b("http", "www.yahoo.com:8080", 1233, "");
    ASSERT_TRUE(b.getFile().equals("")) << ("returns a wrong file");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testGetHost)
{
    String ipv6Host = "FEDC:BA98:7654:3210:FEDC:BA98:7654:3210";
    URL    url("http", ipv6Host, -1, "myfile");
    ASSERT_EQ((String("[") + ipv6Host + "]"), url.getHost());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testGetPort)
{
    URL a("http://member12.c++.com:9999");
    ASSERT_TRUE(a.getPort() == 9999) << ("return wrong port number");
    URL b("http://member12.c++.com:9999/");
    ASSERT_EQ(9999, b.getPort()) << ("return wrong port number");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testGetDefaultPort)
{
    URL a("http://member12.c++.com:9999");
    ASSERT_EQ(80, a.getDefaultPort());

    URL b("http://www.google.com:80/example?language[id]=2");
    ASSERT_EQ(String("www.google.com"), b.getHost());
    ASSERT_EQ(80, b.getPort());

    // TODO
    //    URL b("ftp://member12.c++.com:9999/");
    //    ASSERT_EQ(21, b.getDefaultPort());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testGetProtocol)
{
    URL a("http://www.yahoo2.com:9999");
    ASSERT_TRUE(a.getProtocol().equals("http"))
        << ("u returns a wrong protocol: ");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testGetRef)
{
    URL b("http://www.yahoo2.com:9999");
    URL a("http://www.yahoo1.com:8080/dir1/dir2/test.cgi?point1.html#anchor1");

    ASSERT_EQ(String("anchor1"), a.getRef()) << ("returns a wrong anchor1");

    ASSERT_EQ(String(), b.getRef()) << ("returns a wrong anchor2");
    URL c("http://www.yahoo2.com#ref");
    ASSERT_EQ(String("ref"), c.getRef()) << ("returns a wrong anchor3");
    URL d("http://www.yahoo2.com/file#ref1#ref2");
    ASSERT_EQ(String("ref1#ref2"), d.getRef()) << ("returns a wrong anchor4");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testGetAuthority)
{
    URL a("http", "hostname", 80, "/java?q1#ref");
    ASSERT_EQ(String("hostname:80"), a.getAuthority());
    ASSERT_EQ(String("hostname"), a.getHost());
    ASSERT_EQ(String(), a.getUserInfo());
    ASSERT_EQ(String("/java?q1"), a.getFile());
    ASSERT_EQ(String("/java"), a.getPath());
    ASSERT_EQ(String("q1"), a.getQuery());
    ASSERT_EQ(String("ref"), a.getRef());

    URL b("http", "u:p@home", 80, "/java?q1#ref");
    ASSERT_EQ(String("[u:p@home]:80"), b.getAuthority());
    ASSERT_EQ(String("[u:p@home]"), b.getHost());
    ASSERT_EQ(String(""), b.getUserInfo());
    ASSERT_EQ(String("/java?q1"), b.getFile());
    ASSERT_EQ(String("/java"), b.getPath());
    ASSERT_EQ(String("q1"), b.getQuery());
    ASSERT_EQ(String("ref"), b.getRef());

    URL c("http", "home", -1, "/java");
    ASSERT_EQ(String("home"), c.getAuthority()) << ("wrong authority2");
    ASSERT_EQ(String(), c.getUserInfo()) << ("wrong userInfo2");
    ASSERT_EQ(String("home"), c.getHost()) << ("wrong host2");
    ASSERT_EQ(String("/java"), c.getFile()) << ("wrong file2");
    ASSERT_EQ(String("/java"), c.getPath()) << ("wrong path2");
    ASSERT_EQ(String(), c.getQuery()) << ("wrong query2");
    ASSERT_EQ(String(), c.getRef()) << ("wrong ref2");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testToURI)
{
    URL a("http://www.apache.org");
    URI uri = a.toURI();
    ASSERT_TRUE(a.equals(uri.toURL()));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testURLStreamHandlerParseURL)
{
    URL                url("http://localhost");
    MyURLStreamHandler handler;

    ASSERT_THROW(handler.parse(url, "//", 0, Integer::MIN_VALUE),
                 StringIndexOutOfBoundsException)
        << ("Should have thrown an StringIndexOutOfBoundsException");

    ASSERT_THROW(handler.parse(url, "1234//", 4, Integer::MIN_VALUE),
                 StringIndexOutOfBoundsException)
        << ("Should have thrown an StringIndexOutOfBoundsException");

    ASSERT_THROW(handler.parse(url, "1", -1, 0), StringIndexOutOfBoundsException)
        << ("Should have thrown an StringIndexOutOfBoundsException");

    ASSERT_THROW(handler.parse(url, "1", 3, 2), SecurityException)
        << ("Should have thrown an SecurityException");

    ASSERT_THROW(handler.parse(url, "11", 1, Integer::MIN_VALUE),
                 SecurityException)
        << ("Should have thrown an SecurityException");

    ASSERT_THROW(handler.parse(url, "any", 10, Integer::MIN_VALUE),
                 StringIndexOutOfBoundsException)
        << ("Should have thrown an StringIndexOutOfBoundsException");

    ASSERT_THROW(handler.parse(url, "any", 10, Integer::MIN_VALUE + 1),
                 StringIndexOutOfBoundsException)
        << ("Should have thrown an StringIndexOutOfBoundsException");

    ASSERT_THROW(
        handler.parse(url, "any", Integer::MIN_VALUE, Integer::MIN_VALUE),
        StringIndexOutOfBoundsException)
        << ("Should have thrown an StringIndexOutOfBoundsException");

    ASSERT_THROW(handler.parse(url, "any", Integer::MIN_VALUE, 2),
                 StringIndexOutOfBoundsException)
        << ("Should have thrown an StringIndexOutOfBoundsException");

    ASSERT_THROW(handler.parse(url, "any", -1, 2),
                 StringIndexOutOfBoundsException)
        << ("Should have thrown an StringIndexOutOfBoundsException");

    ASSERT_THROW(handler.parse(url, "any", -1, -1), SecurityException)
        << ("Should have thrown an SecurityException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testUrlParts)
{
    URL url("http://username:password@host:8080/directory/file?query#ref");
    ASSERT_EQ(String("http"), url.getProtocol());
    ASSERT_EQ(String("username:password@host:8080"), url.getAuthority());
    ASSERT_EQ(String("username:password"), url.getUserInfo());
    ASSERT_EQ(String("host"), url.getHost());
    ASSERT_EQ(8080, url.getPort());
    ASSERT_EQ(80, url.getDefaultPort());
    ASSERT_EQ(String("/directory/file?query"), url.getFile());
    ASSERT_EQ(String("/directory/file"), url.getPath());
    ASSERT_EQ(String("query"), url.getQuery());
    ASSERT_EQ(String("ref"), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testFileEqualsWithEmptyHost)
{
    URL a("file", "", -1, "/a/");
    URL b("file:/a/");
    ASSERT_TRUE(a.equals(b));
}

////////////////////////////////////////////////////////////////////////////////
void URLTest::testOmittedHost()
{
    URL url("http:///path");
    ASSERT_EQ(String(""), url.getHost());
    ASSERT_EQ(String("/path"), url.getFile());
    ASSERT_EQ(String("/path"), url.getPath());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testNoHost)
{
    URL url("http:/path");
    ASSERT_EQ(String("http"), url.getProtocol());
    ASSERT_EQ(String(), url.getAuthority());
    ASSERT_EQ(String(), url.getUserInfo());
    ASSERT_EQ(String(), url.getHost());
    ASSERT_EQ(-1, url.getPort());
    ASSERT_EQ(80, url.getDefaultPort());
    ASSERT_EQ(String("/path"), url.getFile());
    ASSERT_EQ(String("/path"), url.getPath());
    ASSERT_EQ(String(), url.getQuery());
    ASSERT_EQ(String(), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testNoPath)
{
    URL url("http://host");
    ASSERT_EQ(String("host"), url.getHost());
    ASSERT_EQ(String(), url.getFile());
    ASSERT_EQ(String(), url.getPath());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testEmptyHostAndNoPath)
{
    URL url("http://");
    ASSERT_EQ(String("http"), url.getProtocol());
    ASSERT_EQ(String(), url.getAuthority());
    ASSERT_EQ(String(), url.getUserInfo());
    ASSERT_EQ(String(), url.getHost());
    ASSERT_EQ(-1, url.getPort());
    ASSERT_EQ(80, url.getDefaultPort());
    ASSERT_EQ(String(), url.getFile());
    ASSERT_EQ(String(), url.getPath());
    ASSERT_EQ(String(), url.getQuery());
    ASSERT_EQ(String(), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testNoHostAndNoPath)
{
    URL url("http:");
    ASSERT_EQ(String("http"), url.getProtocol());
    ASSERT_EQ(String(), url.getAuthority());
    ASSERT_EQ(String(), url.getUserInfo());
    ASSERT_EQ(String(), url.getHost());
    ASSERT_EQ(-1, url.getPort());
    ASSERT_EQ(80, url.getDefaultPort());
    ASSERT_EQ(String(), url.getFile());
    ASSERT_EQ(String(), url.getPath());
    ASSERT_EQ(String(), url.getQuery());
    ASSERT_EQ(String(), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testAtSignInUserInfo)
{
    ASSERT_THROW(URL("http://user@userhost.com:password@host"),
                 MalformedURLException)
        << ("Should have thrown an MalformedURLException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testUserNoPassword)
{
    URL url("http://user@host");
    ASSERT_EQ(String("user@host"), url.getAuthority());
    ASSERT_EQ(String("user"), url.getUserInfo());
    ASSERT_EQ(String("host"), url.getHost());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testUserNoPasswordExplicitPort)
{
    URL url("http://user@host:8080");
    ASSERT_EQ(String("user@host:8080"), url.getAuthority());
    ASSERT_EQ(String("user"), url.getUserInfo());
    ASSERT_EQ(String("host"), url.getHost());
    ASSERT_EQ(8080, url.getPort());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testUserPasswordHostPort)
{
    URL url("http://user:password@host:8080");
    ASSERT_EQ(String("user:password@host:8080"), url.getAuthority());
    ASSERT_EQ(String("user:password"), url.getUserInfo());
    ASSERT_EQ(String("host"), url.getHost());
    ASSERT_EQ(8080, url.getPort());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testUserPasswordEmptyHostPort)
{
    URL url("http://user:password@:8080");
    ASSERT_EQ(String("user:password@:8080"), url.getAuthority());
    ASSERT_EQ(String("user:password"), url.getUserInfo());
    ASSERT_EQ(String(), url.getHost());
    ASSERT_EQ(8080, url.getPort());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testUserPasswordEmptyHostEmptyPort)
{
    URL url("http://user:password@");
    ASSERT_EQ(String("user:password@"), url.getAuthority());
    ASSERT_EQ(String("user:password"), url.getUserInfo());
    ASSERT_EQ(String(), url.getHost());
    ASSERT_EQ(-1, url.getPort());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testPathOnly)
{
    URL url("http://host/path");
    ASSERT_EQ(String("/path"), url.getFile());
    ASSERT_EQ(String("/path"), url.getPath());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testQueryOnly)
{
    URL url("http://host?query");
    ASSERT_EQ(String("?query"), url.getFile());
    ASSERT_EQ(String(), url.getPath());
    ASSERT_EQ(String("query"), url.getQuery());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testFragmentOnly)
{
    URL url("http://host#fragment");
    ASSERT_EQ(String(), url.getFile());
    ASSERT_EQ(String(), url.getPath());
    ASSERT_EQ(String("fragment"), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testAtSignInPath)
{
    URL url("http://host/file@foo");
    ASSERT_EQ(String("/file@foo"), url.getFile());
    ASSERT_EQ(String("/file@foo"), url.getPath());
    ASSERT_EQ(String(), url.getUserInfo());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testColonInPath)
{
    URL url("http://host/file:colon");
    ASSERT_EQ(String("/file:colon"), url.getFile());
    ASSERT_EQ(String("/file:colon"), url.getPath());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testSlashInQuery)
{
    URL url("http://host/file?query/path");
    ASSERT_EQ(String("/file?query/path"), url.getFile());
    ASSERT_EQ(String("/file"), url.getPath());
    ASSERT_EQ(String("query/path"), url.getQuery());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testQuestionMarkInQuery)
{
    URL url("http://host/file?query?another");
    ASSERT_EQ(String("/file?query?another"), url.getFile());
    ASSERT_EQ(String("/file"), url.getPath());
    ASSERT_EQ(String("query?another"), url.getQuery());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testAtSignInQuery)
{
    URL url("http://host/file?query@at");
    ASSERT_EQ(String("/file?query@at"), url.getFile());
    ASSERT_EQ(String("/file"), url.getPath());
    ASSERT_EQ(String("query@at"), url.getQuery());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testColonInQuery)
{
    URL url("http://host/file?query:colon");
    ASSERT_EQ(String("/file?query:colon"), url.getFile());
    ASSERT_EQ(String("/file"), url.getPath());
    ASSERT_EQ(String("query:colon"), url.getQuery());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testQuestionMarkInFragment)
{
    URL url("http://host/file#fragment?query");
    ASSERT_EQ(String("/file"), url.getFile());
    ASSERT_EQ(String("/file"), url.getPath());
    ASSERT_EQ(String(), url.getQuery());
    ASSERT_EQ(String("fragment?query"), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testColonInFragment)
{
    URL url("http://host/file#fragment:80");
    ASSERT_EQ(String("/file"), url.getFile());
    ASSERT_EQ(String("/file"), url.getPath());
    ASSERT_EQ(-1, url.getPort());
    ASSERT_EQ(String("fragment:80"), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testSlashInFragment)
{
    URL url("http://host/file#fragment/path");
    ASSERT_EQ(String("/file"), url.getFile());
    ASSERT_EQ(String("/file"), url.getPath());
    ASSERT_EQ(String("fragment/path"), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testSlashInFragmentCombiningConstructor)
{
    URL url("http", "host", "/file#fragment/path");
    ASSERT_EQ(String("/file"), url.getFile());
    ASSERT_EQ(String("/file"), url.getPath());
    ASSERT_EQ(String("fragment/path"), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testHashInFragment)
{
    URL url("http://host/file#fragment#another");
    ASSERT_EQ(String("/file"), url.getFile());
    ASSERT_EQ(String("/file"), url.getPath());
    ASSERT_EQ(String("fragment#another"), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testEmptyPort)
{
    URL url("http://host:/");
    ASSERT_EQ(-1, url.getPort());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testNonNumericPort)
{
    ASSERT_THROW(URL("http://host:x/"), MalformedURLException)
        << ("Should have thrown an MalformedURLException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testNegativePort)
{
    ASSERT_THROW(URL("http://host:-2/"), MalformedURLException)
        << ("Should have thrown an MalformedURLException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testNegativePortEqualsPlaceholder)
{
    ASSERT_THROW(URL("http://host:-1/"), MalformedURLException)
        << ("Should have thrown an MalformedURLException");
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRelativePathOnQuery)
{
    URL base("http://host/file?query/x");
    URL url(base, "another");
    ASSERT_EQ(std::string("http://host/another"), url.toString());
    ASSERT_EQ(String("/another"), url.getFile());
    ASSERT_EQ(String("/another"), url.getPath());
    ASSERT_EQ(String(), url.getQuery());
    ASSERT_EQ(String(), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRelativeFragmentOnQuery)
{
    URL base("http://host/file?query/x#fragment");
    URL url(base, "#another");
    ASSERT_EQ(std::string("http://host/file?query/x#another"), url.toString());
    ASSERT_EQ(String("/file?query/x"), url.getFile());
    ASSERT_EQ(String("/file"), url.getPath());
    ASSERT_EQ(String("query/x"), url.getQuery());
    ASSERT_EQ(String("another"), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testPathContainsRelativeParts)
{
    URL url("http://host/a/b/../c");
    ASSERT_EQ(std::string("http://host/a/c"),
              url.toString());  // RI doesn't canonicalize
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRelativePathAndFragment)
{
    URL base("http://host/file");
    ASSERT_EQ(std::string("http://host/another#fragment"),
              URL(base, "another#fragment").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRelativeParentDirectory)
{
    URL base("http://host/a/b/c");
    ASSERT_EQ(std::string("http://host/a/d"), URL(base, "../d").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRelativeChildDirectory)
{
    URL base("http://host/a/b/c");
    ASSERT_EQ(std::string("http://host/a/b/d/e"), URL(base, "d/e").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRelativeRootDirectory)
{
    URL base("http://host/a/b/c");
    ASSERT_EQ(std::string("http://host/d"), URL(base, "/d").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRelativeFullUrl)
{
    URL base("http://host/a/b/c");
    ASSERT_EQ(std::string("http://host2/d/e"),
              URL(base, "http://host2/d/e").toString());
    ASSERT_EQ(std::string("https://host2/d/e"),
              URL(base, "https://host2/d/e").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRelativeDifferentScheme)
{
    URL base("http://host/a/b/c");
    ASSERT_EQ(std::string("https://host2/d/e"),
              URL(base, "https://host2/d/e").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRelativeDifferentAuthority)
{
    URL base("http://host/a/b/c");
    ASSERT_EQ(std::string("http://another/d/e"),
              URL(base, "//another/d/e").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRelativeWithScheme)
{
    URL base("http://host/a/b/c");
    ASSERT_EQ(std::string("http://host/a/b/c"), URL(base, "http:").toString());
    ASSERT_EQ(std::string("http://host/"), URL(base, "http:/").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testMalformedUrlsRefusedByFirefoxAndChrome)
{
    URL base("http://host/a/b/c");
    // TODO
    //    ASSERT_EQ(std::string("http://"), URL(base, "http://").toString()); //
    //    fails on RI; path retained ASSERT_EQ(std::string("http://"), URL(base,
    //    "//").toString()); // fails on RI
    ASSERT_EQ(std::string("https:"), URL(base, "https:").toString());
    ASSERT_EQ(std::string("https:/"), URL(base, "https:/").toString());
    //    ASSERT_EQ(std::string("https://"), URL(base, "https://").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRfc1808NormalExamples)
{
    URL base("http://a/b/c/d;p?q");
    ASSERT_EQ(std::string("https:h"), URL(base, "https:h").toString());
    ASSERT_EQ(std::string("http://a/b/c/g"), URL(base, "g").toString());
    ASSERT_EQ(std::string("http://a/b/c/g"), URL(base, "./g").toString());
    ASSERT_EQ(std::string("http://a/b/c/g/"), URL(base, "g/").toString());
    ASSERT_EQ(std::string("http://a/g"), URL(base, "/g").toString());
    ASSERT_EQ(std::string("http://g"), URL(base, "//g").toString());
    ASSERT_EQ(std::string("http://a/b/c/d;p?y"),
              URL(base, "?y").toString());  // RI fails; file lost
    ASSERT_EQ(std::string("http://a/b/c/g?y"), URL(base, "g?y").toString());
    ASSERT_EQ(std::string("http://a/b/c/d;p?q#s"), URL(base, "#s").toString());
    ASSERT_EQ(std::string("http://a/b/c/g#s"), URL(base, "g#s").toString());
    ASSERT_EQ(std::string("http://a/b/c/g?y#s"), URL(base, "g?y#s").toString());
    ASSERT_EQ(std::string("http://a/b/c/;x"), URL(base, ";x").toString());
    ASSERT_EQ(std::string("http://a/b/c/g;x"), URL(base, "g;x").toString());
    ASSERT_EQ(std::string("http://a/b/c/g;x?y#s"),
              URL(base, "g;x?y#s").toString());
    ASSERT_EQ(std::string("http://a/b/c/d;p?q"), URL(base, "").toString());
    ASSERT_EQ(std::string("http://a/b/c/"), URL(base, ".").toString());
    ASSERT_EQ(std::string("http://a/b/c/"), URL(base, "./").toString());
    ASSERT_EQ(std::string("http://a/b/"), URL(base, "..").toString());
    ASSERT_EQ(std::string("http://a/b/"), URL(base, "../").toString());
    ASSERT_EQ(std::string("http://a/b/g"), URL(base, "../g").toString());
    ASSERT_EQ(std::string("http://a/"), URL(base, "../..").toString());
    ASSERT_EQ(std::string("http://a/"), URL(base, "../../").toString());
    ASSERT_EQ(std::string("http://a/g"), URL(base, "../../g").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRfc1808AbnormalExampleTooManyDotDotSequences)
{
    URL base("http://a/b/c/d;p?q");
    ASSERT_EQ(std::string("http://a/g"),
              URL(base, "../../../g").toString());  // RI doesn't normalize
    ASSERT_EQ(std::string("http://a/g"), URL(base, "../../../../g").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRfc1808AbnormalExampleRemoveDotSegments)
{
    URL base("http://a/b/c/d;p?q");
    ASSERT_EQ(std::string("http://a/g"),
              URL(base, "/./g").toString());  // RI doesn't normalize
    ASSERT_EQ(std::string("http://a/g"),
              URL(base, "/../g").toString());  // RI doesn't normalize
    ASSERT_EQ(std::string("http://a/b/c/g."), URL(base, "g.").toString());
    ASSERT_EQ(std::string("http://a/b/c/.g"), URL(base, ".g").toString());
    ASSERT_EQ(std::string("http://a/b/c/g.."), URL(base, "g..").toString());
    ASSERT_EQ(std::string("http://a/b/c/..g"), URL(base, "..g").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRfc1808AbnormalExampleNonsensicalDots)
{
    URL base("http://a/b/c/d;p?q");
    ASSERT_EQ(std::string("http://a/b/g"), URL(base, "./../g").toString());
    ASSERT_EQ(std::string("http://a/b/c/g/"), URL(base, "./g/.").toString());
    ASSERT_EQ(std::string("http://a/b/c/g/h"), URL(base, "g/./h").toString());
    ASSERT_EQ(std::string("http://a/b/c/h"), URL(base, "g/../h").toString());
    ASSERT_EQ(std::string("http://a/b/c/g;x=1/y"),
              URL(base, "g;x=1/./y").toString());
    ASSERT_EQ(std::string("http://a/b/c/y"),
              URL(base, "g;x=1/../y").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRfc1808AbnormalExampleRelativeScheme)
{
    URL base("http://a/b/c/d;p?q");
    // this result is permitted; strict parsers prefer "http:g"
    ASSERT_EQ(std::string("http://a/b/c/g"), URL(base, "http:g").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testRfc1808AbnormalExampleQueryOrFragmentDots)
{
    URL base("http://a/b/c/d;p?q");
    ASSERT_EQ(std::string("http://a/b/c/g?y/./x"),
              URL(base, "g?y/./x").toString());
    ASSERT_EQ(std::string("http://a/b/c/g?y/../x"),
              URL(base, "g?y/../x").toString());
    ASSERT_EQ(std::string("http://a/b/c/g#s/./x"),
              URL(base, "g#s/./x").toString());
    ASSERT_EQ(std::string("http://a/b/c/g#s/../x"),
              URL(base, "g#s/../x").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testSquareBracketsInUserInfo)
{
    URL url("http://user:[::1]@host");
    ASSERT_EQ(String("user:[::1]"), url.getUserInfo());
    ASSERT_EQ(String("host"), url.getHost());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testComposeUrl)
{
    URL url("http", "host", "a");
    ASSERT_EQ(String("http"), url.getProtocol());
    ASSERT_EQ(String("host"), url.getAuthority());
    ASSERT_EQ(String("host"), url.getHost());
    ASSERT_EQ(String("/a"),
              url.getFile());  // RI fails; doesn't insert '/' separator
    ASSERT_EQ(std::string("http://host/a"), url.toString());  // fails on RI
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testComposeUrlWithNullHost)
{
    URL url("http", String(), "a");
    ASSERT_EQ(String("http"), url.getProtocol());
    ASSERT_EQ(String(), url.getAuthority());
    ASSERT_EQ(String(), url.getHost());
    ASSERT_EQ(String("a"), url.getFile());
    ASSERT_EQ(std::string("http:a"), url.toString());  // fails on RI
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testFileUrlExtraLeadingSlashes)
{
    URL url("file:////foo");
    ASSERT_EQ(String(), url.getAuthority());  // RI returns String()
    ASSERT_EQ(String("//foo"), url.getPath());
    //    ASSERT_EQ(std::string("file:////foo"), url.toString());  TODO
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testFileUrlWithAuthority)
{
    URL url("file://x/foo");
    ASSERT_EQ(String("x"), url.getAuthority());
    ASSERT_EQ(String("/foo"), url.getPath());
    ASSERT_EQ(std::string("file://x/foo"), url.toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testEmptyAuthority)
{
    URL url("http:///foo");
    ASSERT_EQ(String(), url.getAuthority());
    ASSERT_EQ(String("/foo"), url.getPath());
    // ASSERT_EQ(std::string("http:///foo"), url.toString()); // RI drops '//'
    // TODO
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testHttpUrlExtraLeadingSlashes)
{
    URL url("http:////foo");
    ASSERT_EQ(String(), url.getAuthority());  // RI returns String()
    ASSERT_EQ(String("//foo"), url.getPath());
    //    ASSERT_EQ(std::string("http:////foo"), url.toString());  TODO
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testFileUrlRelativePath)
{
    URL base("file:a/b/c");
    ASSERT_EQ(std::string("file:a/b/d"), URL(base, "d").toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testFileUrlDottedPath)
{
    URL url("file:../a/b");
    ASSERT_EQ(String("../a/b"), url.getPath());
    ASSERT_EQ(std::string("file:../a/b"), url.toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testParsingDotAsHostname)
{
    URL url("http://./");
    ASSERT_EQ(String("."), url.getAuthority());
    ASSERT_EQ(String("."), url.getHost());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testSquareBracketsWithIPv4)
{
    ASSERT_THROW(URL("http://[192.168.0.1]/"), MalformedURLException)
        << ("Should have thrown an MalformedURLException");

    URL url("http", "[192.168.0.1]", "/");
    ASSERT_EQ(String("[192.168.0.1]"), url.getHost());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testSquareBracketsWithHostname)
{
    ASSERT_THROW(URL("http://[www.android.com]/"), MalformedURLException)
        << ("Should have thrown an MalformedURLException");

    URL url("http", "[www.android.com]", "/");
    ASSERT_EQ(String("[www.android.com]"), url.getHost());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testIPv6WithoutSquareBrackets)
{
    ASSERT_THROW(URL("http://fe80::1234/"), MalformedURLException)
        << ("Should have thrown an MalformedURLException");

    URL url("http", "fe80::1234", "/");
    ASSERT_EQ(String("[fe80::1234]"), url.getHost());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testIpv6WithSquareBrackets)
{
    URL url("http://[::1]:2/");
    ASSERT_EQ(String("[::1]"), url.getHost());
    ASSERT_EQ(2, url.getPort());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testEqualityWithNoPath)
{
    ASSERT_TRUE(!URL("http://android.com").equals(URL("http://android.com/")));
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testUrlDoesNotEncodeParts)
{
    URL url("http", "host", 80, "/doc|search?q=green robots#over 6\"");
    ASSERT_EQ(String("http"), url.getProtocol());
    ASSERT_EQ(String("host:80"), url.getAuthority());
    ASSERT_EQ(String("/doc|search"), url.getPath());
    ASSERT_EQ(String("q=green robots"), url.getQuery());
    ASSERT_EQ(String("over 6\""), url.getRef());
    ASSERT_EQ(std::string("http://host:80/doc|search?q=green robots#over 6\""),
              url.toString());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testSchemeCaseIsCanonicalized)
{
    URL url("HTTP://host/path");
    ASSERT_EQ(String("http"), url.getProtocol());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testEmptyAuthorityWithPath)
{
    URL url("http:///path");
    ASSERT_EQ(String(), url.getAuthority());
    ASSERT_EQ(String("/path"), url.getPath());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testEmptyAuthorityWithQuery)
{
    URL url("http://?query");
    ASSERT_EQ(String(), url.getAuthority());
    ASSERT_EQ(String(), url.getPath());
    ASSERT_EQ(String("query"), url.getQuery());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testEmptyAuthorityWithFragment)
{
    URL url("http://#fragment");
    ASSERT_EQ(String(), url.getAuthority());
    ASSERT_EQ(String(), url.getPath());
    ASSERT_EQ(String("fragment"), url.getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testCombiningConstructorsMakeRelativePathsAbsolute)
{
    ASSERT_EQ(String("/relative"), URL("http", "host", "relative").getPath());
    ASSERT_EQ(String("/relative"),
              URL("http", "host", -1, "relative").getPath());
    ASSERT_EQ(String("/relative"),
              URL("http", "host", -1, "relative", NULL).getPath());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testCombiningConstructorsDoNotMakeEmptyPathsAbsolute)
{
    ASSERT_EQ(String(), URL("http", "host", "").getPath());
    ASSERT_EQ(String(), URL("http", "host", -1, "").getPath());
    ASSERT_EQ(String(), URL("http", "host", -1, "", NULL).getPath());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testPartContainsSpace)
{
    ASSERT_THROW(URL("ht tp://host/"), MalformedURLException)
        << ("Should have thrown an MalformedURLException");

    ASSERT_EQ(String("user name"), URL("http://user name@host/").getUserInfo());
    ASSERT_EQ(String("ho st"), URL("http://ho st/").getHost());

    ASSERT_THROW(URL("http://host:80 80/"), MalformedURLException)
        << ("Should have thrown an MalformedURLException");

    ASSERT_EQ(String("/fi le"), URL("http://host/fi le").getFile());
    ASSERT_EQ(String("que ry"), URL("http://host/file?que ry").getQuery());
    ASSERT_EQ(String("re f"), URL("http://host/file?query#re f").getRef());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(URLTest, testUnderscore)
{
    URL url("http://a_b.c.d.net/");
    ASSERT_EQ(String("a_b.c.d.net"), url.getAuthority());
    // The RFC's don't permit underscores in hostnames, but URL accepts them
    // (unlike URI).
    ASSERT_EQ(String("a_b.c.d.net"), url.getHost());
}
