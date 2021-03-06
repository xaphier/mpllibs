#ifndef MPLLIBS_METAPARSE_V1_RANGE_HPP
#define MPLLIBS_METAPARSE_V1_RANGE_HPP

// Copyright Abel Sinkovics (abel@sinkovics.hu)  2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <mpllibs/metaparse/v1/accept_when.hpp>
#include <mpllibs/metaparse/v1/one_char.hpp>
#include <mpllibs/metaparse/v1/util/in_range.hpp>
#include <mpllibs/metaparse/v1/error/unexpected_character.hpp>

namespace mpllibs
{
  namespace metaparse
  {
    namespace v1
    {
      template <class From, class To>
      struct range :
        accept_when<
          one_char,
          util::in_range<From, To>,
          error::unexpected_character
        >
      {};
    }
  }
}

#endif

