#ifndef MPLLIBS_METAPARSE_V1_FIRST_OF_HPP
#define MPLLIBS_METAPARSE_V1_FIRST_OF_HPP

// Copyright Abel Sinkovics (abel@sinkovics.hu)  2009 - 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <mpllibs/metaparse/v1/nth_of.hpp>

namespace mpllibs
{
  namespace metaparse
  {
    namespace v1
    {
      template <
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
          MPLLIBS_LIMIT_SEQUENCE_SIZE,
          class P,
          boost::mpl::na
        )
      >
      struct first_of :
        nth_of_c<0, BOOST_PP_ENUM_PARAMS(MPLLIBS_LIMIT_SEQUENCE_SIZE, P)>
      {};
    }
  }
}

#endif

