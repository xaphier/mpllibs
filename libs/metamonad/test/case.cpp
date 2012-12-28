// Copyright Abel Sinkovics (abel@sinkovics.hu) 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK

#include <mpllibs/metamonad/case.hpp>
#include <mpllibs/metamonad/returns.hpp>

#include <mpllibs/metatest/boost_test.hpp>
#include <boost/test/unit_test.hpp>

#include <boost/type_traits.hpp>

namespace
{
  struct x;
  struct y;

  template <class A, class B>
  struct some_template;

  template <class A, class B>
  struct some_other_template;
}

BOOST_AUTO_TEST_CASE(test_case)
{
  using mpllibs::metatest::meta_require;

  using mpllibs::metamonad::case_;
  using mpllibs::metamonad::matches;
  using mpllibs::metamonad::exception;
  using mpllibs::metamonad::no_case_matched;
  using mpllibs::metamonad::var;
  using mpllibs::metamonad::returns;

  using boost::is_same;

  meta_require<
    is_same<
      exception<no_case_matched<int> >,
      case_<
        int,
        matches<double, float>
      >::type
    >
  >(MPLLIBS_HERE, "test_no_matches");

  meta_require<
    is_same<
      float,
      case_<
        int,
        matches<int, returns<float> >
      >::type
    >
  >(MPLLIBS_HERE, "test_match");

  meta_require<
    is_same<
      float,
      case_<
        int,
        matches<int, returns<float> >,
        matches<double, char>
      >::type
    >
  >(MPLLIBS_HERE, "test_first_matches");

  meta_require<
    is_same<
      float,
      case_<
        int,
        matches<double, char>,
        matches<int, returns<float> >
      >::type
    >
  >(MPLLIBS_HERE, "test_second_matches");

  meta_require<
    is_same<
      some_other_template<int, double>,
      case_<
        some_template<int, double>,
        matches<
          some_template<var<x>, var<y> >,
          returns<some_other_template<x, y> >
        >
      >::type
    >
  >(MPLLIBS_HERE, "test_vars_in_pattern");
}

