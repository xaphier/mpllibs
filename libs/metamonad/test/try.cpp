// Copyright Abel Sinkovics (abel@sinkovics.hu) 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <mpllibs/metamonad/try_.hpp>
#include <mpllibs/metamonad/exception.hpp>
#include <mpllibs/metamonad/tmp_tag.hpp>
#include <mpllibs/metamonad/tmp_value.hpp>
#include <mpllibs/metamonad/syntax.hpp>
#include <mpllibs/metamonad/is_tag.hpp>
#include <mpllibs/metamonad/let.hpp>
#include <mpllibs/metamonad/make_syntax.hpp>

#include <boost/test/unit_test.hpp>

#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/always.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/assert.hpp>

#include <boost/type_traits/is_same.hpp>

#include "common.hpp"

using mpllibs::metamonad::tmp_tag;
using mpllibs::metamonad::tmp_value;

namespace
{
  struct tag1 : tmp_tag<tag1> {};
  struct e1 : tmp_value<e1, tag1> {};
  
  struct tag2 : tmp_tag<tag2> {};
  struct e2 : tmp_value<e2, tag2> {};
}

BOOST_AUTO_TEST_CASE(test_try_)
{
  using boost::mpl::equal_to;
  using boost::mpl::tag;
  using boost::mpl::plus;
  using boost::mpl::true_;
  
  using mpllibs::metamonad::exception;
  using mpllibs::metamonad::catch_;
  using mpllibs::metamonad::catch_c;
  using mpllibs::metamonad::is_tag;
  using mpllibs::metamonad::try_;
  using mpllibs::metamonad::syntax;
  using mpllibs::metamonad::let;
  using mpllibs::metamonad::make_syntax;

  using boost::is_same;

  // test_no_exception_and_no_catch
  BOOST_MPL_ASSERT((equal_to<int13, try_<syntax<int13> >::type>));

  // test_exception_and_no_catch
  BOOST_MPL_ASSERT((
    equal_to<exception<int13>, try_<syntax<exception<int13> > >::type>
  ));

  // test_no_exception
  BOOST_MPL_ASSERT((
    equal_to<
      int13,
      try_<
        syntax<int13>,
        catch_<e, syntax<is_tag<tag1, e> >, syntax<int11> >
      >::type
    >
  ));

  // test_no_exception_c
  BOOST_MPL_ASSERT((
    equal_to<
      int13,
      try_<syntax<int13>, catch_c<e, is_tag<tag1, e>, int11> >::type
    >
  ));

  // test_catch
  BOOST_MPL_ASSERT((
    equal_to<
      int11,
      try_<
        syntax<exception<e1> >,
        catch_<e, syntax<is_tag<tag1, e> >, syntax<int11> >
      >::type
    >
  ));

  // test_catch_c
  BOOST_MPL_ASSERT((
    equal_to<
      int11,
      try_<syntax<exception<e1> >, catch_c<e, is_tag<tag1, e>, int11> >::type
    >
  ));

  // test_exception_value_in_catch
  BOOST_MPL_ASSERT((
    equal_to<
      int13,
      try_<
        syntax<exception<int13> >,
        catch_<e, syntax<is_tag<tag<int13>::type, e> >, syntax<e> >
      >::type
    >
  ));

  // test_exception_value_in_catch_c
  BOOST_MPL_ASSERT((
    equal_to<
      int13,
      try_<
        syntax<exception<int13> >,
        catch_c<e, is_tag<tag<int13>::type, e>, e>
      >::type
    >
  ));

  // test_not_catching
  BOOST_MPL_ASSERT((
    equal_to<
      exception<int13>,
      try_<
        syntax<exception<int13> >,
        catch_<e, syntax<is_tag<tag2, e> >, syntax<int11> >
      >::type
    >
  ));

  // test_not_catching_c
  BOOST_MPL_ASSERT((
    equal_to<
      exception<int13>,
      try_<syntax<exception<int13> >, catch_c<e, is_tag<tag2, e>, int11> >::type
    >
  ));

  // test_second_catch
  BOOST_MPL_ASSERT((
    equal_to<
      int13,
      try_<
        syntax<exception<e2> >,
        catch_<e, syntax<is_tag<tag1, e> >, syntax<int11> >,
        catch_<e, syntax<is_tag<tag2, e> >, syntax<int13> >
      >::type
    >
  ));

  // test_second_catch_c
  BOOST_MPL_ASSERT((
    equal_to<
      int13,
      try_<
        syntax<exception<e2> >,
        catch_c<e, is_tag<tag1, e>, int11>,
        catch_c<e, is_tag<tag2, e>, int13>
      >::type
    >
  ));

  // test_exception_propagation
  BOOST_MPL_ASSERT((
    equal_to<
      exception<int2>,
      try_<
        syntax<plus<int1, exception<int2> > >,
        catch_<e, syntax<is_tag<tag1, e> >, syntax<int11> >
     > ::type
    >
  ));

  // test_exception_propagation_c
  BOOST_MPL_ASSERT((
    equal_to<
      exception<int2>,
      try_<
        syntax<plus<int1, exception<int2> > >,
        catch_c<e, is_tag<tag1, e>, int11>
     > ::type
    >
  ));

  // test_rethrowing
  BOOST_MPL_ASSERT((
    equal_to<
      exception<int13>,
      try_<
        syntax<exception<e1> >,
        catch_<e, syntax<true_>, syntax<exception<int13> > >
      >::type
    >
  ));

  // test_rethrowing_c
  BOOST_MPL_ASSERT((
    equal_to<
      exception<int13>,
      try_<
        syntax<exception<e1> >,
        catch_c<e, true_, exception<int13> >
      >::type
    >
  ));

  // test_rethrowing_not_caught_by_next_catch
  BOOST_MPL_ASSERT((
    equal_to<
      exception<int13>,
      try_<
        syntax<exception<e1> >,
        catch_<e, syntax<true_>, syntax<exception<int13> > >,
        catch_<e, syntax<true_>, syntax<exception<int13> > >
      >::type
    >
  ));

  // test_rethrowing_not_caught_by_next_catch_c
  BOOST_MPL_ASSERT((
    equal_to<
      exception<int13>,
      try_<
        syntax<exception<e1> >,
        catch_c<e, true_, exception<int13> >,
        catch_c<e, true_, exception<int13> >
      >::type
    >
  ));

  // test_catch_and_let
  BOOST_MPL_ASSERT((
    is_same<
      syntax<catch_<e, make_syntax<e>, make_syntax<e> > >,
      let<
        e, syntax<int11>,
        syntax<catch_<e, make_syntax<e>, make_syntax<e> > >
      >::type
    >
  ));

  // test_catch_c_and_let
  BOOST_MPL_ASSERT((
    is_same<
      syntax<catch_c<e, make_syntax<e>, make_syntax<e> > >,
      let<
        e, syntax<int11>,
        syntax<catch_c<e, make_syntax<e>, make_syntax<e> > >
      >::type
    >
  ));
}


