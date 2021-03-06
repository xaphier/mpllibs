# catch_just

## Synopsis

```cpp
template <class E, class Pred, class Handler>
struct catch_just
{
  // unspecified
};
```

## Description

Evaluates the angle-bracket expression `E`. When it evaluates to an
[`exception`](exception.html), the value wrapped by `exception` is passed to
the predicate `Pred`. If it returns true, the value is passed to the
metafunction class `Handler` and its result is returned by `catch_all`. When
there is no exception or the predicate returns false, `catch_just` returns the
result of `E`.

## Header

```cpp
#include <mpllibs/metamonad/catch_just.hpp>
```

## Expression semantics

For any `t` angle-bracket expression not evaluating to an instance of
`exception`, `p` predicate, `u` angle-bracket expression evaluating to
`exception<e>` for some `e` type, `pt` predicate returning true for `e`, `pf`
predicate returning false for `e` and `f` metafunction class taking one argument
the following are equivalent:

```cpp
catch_all<syntax<t>, p, f>::type
t::type
```

```cpp
catch_all<syntax<u>, pt, f>::type
f::type::apply<e>::type
```

```cpp
catch_all<syntax<u>, pf, f>::type
u::type
```

## Example

```cpp
using boost::mpl::int_;
using namespace mpllibs::metamonad::name;

catch_just<
  syntax<may_fail2<may_fail1<int_<13>>>>,
  is_tag<list_tag>,
  lambda_c<e, int_<11>>
>::type
```

<p class="copyright">
Copyright Abel Sinkovics (abel at elte dot hu) 2011.
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
[http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt)
</p>

[[up]](reference.html)



