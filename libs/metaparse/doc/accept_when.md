# accept_when

## Synopsis

```cpp
template <class P, class Pred, class Msg>
struct accept_when
{
  template <class S, class Pos>
  struct apply
  {
    // unspecified
  };
};
```

## Description

Parser combinator taking a parser, a predicate and an error message as
arguments. It builds a parser that accepts the input if and only if the original
parsers accepts it and the predicate is `true` for the result of the
original parser. When the original parser, `P` fails, the error returned by `P`
is forwarded. When the predicate returns `false` on the result of the original
parser, an error with `Msg` as the error message is returned.

## Header

```cpp
#include <mpllibs/metaparse/accept_when.hpp>
```

## Expression semantics

For any `p` parser, `pred` predicate, `msg` error message, `s` compile-time
string and `pos` source position

```cpp
boost::mpl::apply<accept_when<p, pred, msg>, s, pos>
```

is equivalent to

```cpp
boost::mpl::apply<p, s, pos>
```

when `boost::mpl::apply<p, s, pos>` doesn't return an error and
`boost::mpl::apply<pred, mpllibs::metaparse::get_result<boost::mpl::apply<p, s, pos>>>::type`
is `true`. It is an error with `msg` as the error message otherwise.

## Example

```cpp
typedef
  accept_when<
    mpllibs::metaparse::one_char,
    mpllibs::metaparse::util::is_digit,
    mpllibs::metaparse::errors::digit_expected
  >
  accept_digit;
```

<p class="copyright">
Copyright Abel Sinkovics (abel at elte dot hu) 2011.
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
[http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt)
</p>

[[up]](reference.html)


