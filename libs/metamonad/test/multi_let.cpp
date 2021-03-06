// Copyright Abel Sinkovics (abel@sinkovics.hu) 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <mpllibs/metamonad/multi_let.hpp>
#include <mpllibs/metamonad/eval_case.hpp>
#include <mpllibs/metamonad/name.hpp>
#include <mpllibs/metamonad/syntax.hpp>
#include <mpllibs/metamonad/eval_syntax.hpp>
#include <mpllibs/metamonad/tmp_value.hpp>
#include <mpllibs/metamonad/pair.hpp>

#include <boost/test/unit_test.hpp>

#include <boost/mpl/map.hpp>
#include <boost/mpl/assert.hpp>

#include <boost/type_traits.hpp>

#include "common.hpp"

using mpllibs::metamonad::tmp_value;

namespace
{
  template <class A, class B>
  struct some_template : tmp_value<some_template<A, B> > {};
}

BOOST_AUTO_TEST_CASE(test_multi_let)
{
  using mpllibs::metamonad::multi_let;
  using mpllibs::metamonad::eval_case;
  using mpllibs::metamonad::syntax;
  using mpllibs::metamonad::eval_syntax;
  using mpllibs::metamonad::pair;

  using boost::mpl::map;

  using boost::is_same;

  using namespace mpllibs::metamonad::name;

  // test_setting_value
  BOOST_MPL_ASSERT((
    is_same<
      int13,
      eval_syntax<multi_let<map<pair<x, syntax<int13> > >, syntax<x> > >::type
    >
  ));

  // test_nothing_to_set
  BOOST_MPL_ASSERT((
    is_same<x, eval_syntax<multi_let<map<>, syntax<x> > >::type>
  ));

  // test_multiple_variables
  BOOST_MPL_ASSERT((
    is_same<
      some_template<int11, int13>,
      eval_syntax<
        multi_let<
          map<pair<x, syntax<int11> >, pair<y, syntax<int13> > >,
          syntax<some_template<x,y> >
        >
      >::type
    >
  ));
}


