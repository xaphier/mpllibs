// Copyright Abel Sinkovics (abel@sinkovics.hu) 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <mpllibs/metaparse/entire_input.hpp>
#include <mpllibs/metaparse/is_error.hpp>
#include <mpllibs/metaparse/start.hpp>
#include <mpllibs/metaparse/get_result.hpp>
#include <mpllibs/metaparse/one_char.hpp>
#include <mpllibs/metaparse/get_message.hpp>

#include "common.hpp"

#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/apply_wrap.hpp>
#include <boost/mpl/assert.hpp>

#include <boost/type_traits/is_same.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_entire_input)
{
  using mpllibs::metaparse::get_result;
  using mpllibs::metaparse::entire_input;
  using mpllibs::metaparse::start;
  using mpllibs::metaparse::is_error;
  using mpllibs::metaparse::one_char;
  using mpllibs::metaparse::get_message;
  
  using boost::mpl::equal_to;
  using boost::mpl::apply_wrap2;

  using boost::is_same;

  typedef entire_input<one_char> ei;
 
  // test_accept_entire_input
  BOOST_MPL_ASSERT((
    equal_to<get_result<apply_wrap2<ei, str_h, start> >::type, char_h>
  ));

  // test_reject_non_entire_input
  BOOST_MPL_ASSERT((is_error<apply_wrap2<ei, str_hello, start> >));

  // test_predefined_error_message
  BOOST_MPL_ASSERT((
    is_same<
      test_failure,
      get_message<
        apply_wrap2<entire_input<one_char, test_failure>, str_hello, start>
      >::type
    >
  ));
}

