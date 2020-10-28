//  Copyright (c) 2001-2011 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/karma_char.hpp>
#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_directive.hpp>
#include <boost/spirit/include/karma_operator.hpp>
#include <boost/spirit/include/karma_nonterminal.hpp>
#include <boost/spirit/include/karma_string.hpp>

#include "test.hpp"

using namespace spirit_test;

///////////////////////////////////////////////////////////////////////////////
int
main()
{
    using namespace boost::spirit;
    using namespace boost::spirit::ascii;

    {
        BOOST_TEST(test("a b ", delimit[char_('a') << 'b']));
        BOOST_TEST(test("a*b*", delimit('*')[char_('a') << 'b']));

        BOOST_TEST(test("ab c d", 
            char_('a') << delimit[char_('b') << 'c'] << 'd'));
        BOOST_TEST(test("ab*c*d", 
            char_('a') << delimit('*')[char_('b') << 'c'] << 'd'));

        BOOST_TEST(test_delimited("a b ", delimit[char_('a') << 'b'], char_(' ')));
        BOOST_TEST(test_delimited("a*b*", delimit('*')[char_('a') << 'b'], char_(' ')));

        BOOST_TEST(test_delimited("a b c d ", 
            char_('a') << delimit[char_('b') << 'c'] << 'd', char_(' ')));
        BOOST_TEST(test_delimited("a b*c*d ", 
            char_('a') << delimit('*')[char_('b') << 'c'] << 'd', char_(' ')));
    }

    {
        BOOST_TEST(test("ab", verbatim[char_('a') << 'b']));
        BOOST_TEST(test("abcd", 
            char_('a') << verbatim[char_('b') << 'c'] << 'd'));

        BOOST_TEST(test_delimited("ab ", 
            verbatim[char_('a') << 'b'], char_(' ')));
        BOOST_TEST(test_delimited("a bc d ", 
            char_('a') << verbatim[char_('b') << 'c'] << 'd', char_(' ')));
    }

    {
        BOOST_TEST(test("ab", no_delimit[char_('a') << 'b']));
        BOOST_TEST(test("abcd", 
            char_('a') << no_delimit[char_('b') << 'c'] << 'd'));

        BOOST_TEST(test_delimited("ab", 
            no_delimit[char_('a') << 'b'], char_(' ')));
        BOOST_TEST(test_delimited("a bcd ", 
            char_('a') << no_delimit[char_('b') << 'c'] << 'd', char_(' ')));
    }

    {
        // The doubled delimiters at the end are generated by the 'b' generator 
        // and the verbatim[] directive. Currently, there is no easy way to 
        // avoid this.
        BOOST_TEST(test("a b  ", delimit[verbatim[delimit[char_('a') << 'b']]]));
        BOOST_TEST(test_delimited("a*b**", 
            verbatim[delimit[char_('a') << 'b']], char_('*')));
    }

    {
        karma::rule<output_iterator<char>::type, BOOST_TYPEOF(", ")> r = "abc";
        BOOST_TEST(test("abc, ", delimit(", ")[r]));
    }

    return boost::report_errors();
}

