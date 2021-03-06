// Copyright Abel Sinkovics (abel@sinkovics.hu) 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <mpllibs/metaparse/first_of.hpp>
#include <mpllibs/metaparse/is_error.hpp>
#include <mpllibs/metaparse/start.hpp>
#include <mpllibs/metaparse/get_result.hpp>

#include "common.hpp"

#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/apply_wrap.hpp>
#include <boost/mpl/assert.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_first_of)
{
  using mpllibs::metaparse::get_result;
  using mpllibs::metaparse::first_of;
  using mpllibs::metaparse::start;
  using mpllibs::metaparse::is_error;
  
  using boost::mpl::equal_to;
  using boost::mpl::apply_wrap2;

  // test_one_char
  BOOST_MPL_ASSERT((
    equal_to<
      get_result<apply_wrap2<first_of<lit_h>, str_hello, start> >::type,
      char_h
    >
  ));

  // test_two_chars
  BOOST_MPL_ASSERT((
    equal_to<
      get_result<apply_wrap2<first_of<lit_h, lit_e>, str_hello, start> >::type,
      char_h
    >
  ));

  // test_first_fails
  BOOST_MPL_ASSERT((
    is_error<apply_wrap2<first_of<lit_x, lit_e>, str_hello, start> >
  ));

  // test_second_fails
  BOOST_MPL_ASSERT((
    is_error<apply_wrap2<first_of<lit_h, lit_x>, str_hello, start> >
  ));

  // test_empty_input
  BOOST_MPL_ASSERT((is_error<apply_wrap2<first_of<lit_h,lit_e>, str_,start> >));

  // test_three_chars
  BOOST_MPL_ASSERT((
    equal_to<
      get_result<
        apply_wrap2<first_of<lit_h, lit_e, lit_l>, str_hello, start>
      >::type,
      char_h
    >
  ));
}

