// Copyright Abel Sinkovics (abel@sinkovics.hu) 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <mpllibs/metaparse/spaces.h>
#include <mpllibs/metaparse/util/is_nothing.h>

#include "common.h"

#include <mpllibs/metatest/test.h>
#include <mpllibs/metatest/TestSuite.h>

#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/equal.hpp>

namespace
{
  const mpllibs::metatest::TestSuite suite("spaces");
  
  typedef boost::mpl::list_c<char, 'e', 'l', 'l', 'o'> str_ello;
  typedef
    boost::mpl::list_c<char, ' ', '\t', '\n', '\r', 'e', 'l', 'l', 'o'>
    str_____ello;

  typedef
    mpllibs::metaparse::util::is_nothing<
      boost::mpl::apply<mpllibs::metaparse::spaces, str_hello>
    >
    TestRejectNoSpace;

  typedef
    boost::mpl::not_<
      mpllibs::metaparse::util::is_nothing<
        boost::mpl::apply<mpllibs::metaparse::spaces, str__ello>
      >
    >
    TestAcceptOneSpace;

  typedef
    boost::mpl::equal<
      boost::mpl::apply<mpllibs::metaparse::spaces, str__ello>::type::second,
      str_ello
    >
    TestAcceptOnlySpace;

  typedef
    boost::mpl::not_<
      mpllibs::metaparse::util::is_nothing<
        boost::mpl::apply<mpllibs::metaparse::spaces, str_____ello>
      >
    >
    TestAcceptAllSpaces;

  typedef
    boost::mpl::equal<
      boost::mpl::apply<mpllibs::metaparse::spaces, str_____ello>::type::second,
      str_ello
    >
    TestConsumeAllSpaces;
}

MPLLIBS_ADD_TEST(suite, TestRejectNoSpace)
MPLLIBS_ADD_TEST(suite, TestAcceptOneSpace)
MPLLIBS_ADD_TEST(suite, TestAcceptOnlySpace)
MPLLIBS_ADD_TEST(suite, TestAcceptAllSpaces)
MPLLIBS_ADD_TEST(suite, TestConsumeAllSpaces)


