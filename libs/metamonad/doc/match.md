# match

## Synopsis

```cpp
template <class Pattern, class Expression>
struct match
{
  // unspecified
};
```

## Description

Metafunction implementing pattern matching. It matches the angly-bracket
expression `Expression` against the pattern `Pattern`, which has to be a syntax.

Every class matches itself. The following two cases are exceptions:

* `Pattern` may contain instances of the `template <class Name> struct var;`
  template. These instances refer to open variables of the pattern. Any class
  matches them. When multiple occurrences of the same variable would match
  different classes, the expression doesn't match the pattern.

* `Pattern` may contain the `_` class, which refer to an anonymous open
  variable. Any class matches it.

When the expression matches the pattern, `match` returns a `boost::mpl::map`,
where they keys are the open variables of the pattern and the values are the
classes that were matched against them. The values of the map are syntaxes.

When the expression doesn't match, `match` returns a
[`bad_match`](bad_match.html) [`exception`](exception.html).

## Header

```cpp
#include <mpllibs/metamonad/match.hpp>
```

## Expression semantics

For any `x` and `y` classes, `t1` and `t2` templates taking two classes as
arguments the following are equivalent:

```cpp
using namespace boost::mpl;

match<syntax<int>, int>::type
map<>

match<syntax<_>, int>::type
map<>

match<syntax<int>, double>::type
exception<...>

match<syntax<var<x>>, int>::type
map<pair<var<x>, syntax<int>>>

match<syntax<t1<var<x>, var<y>>>, t1<int, double>>::type
map<pair<var<x>, int>, pair<var<y>, syntax<double>>>

match<syntax<t1<var<x>, var<y>>>, t2<int, double>>::type
exception<...>

match<syntax<t1<var<x>, var<x>>>, t1<int, int>>::type
map<pair<var<x>, syntax<int>>>

match<syntax<t1<var<x>, var<x>>>, t1<int, double>>::type
exception<...>

match<syntax<var<x>>, t1<int, double>>::type
map<pair<var<x#, t1<int, double>>>
```

## Example

```cpp
using namespace mpllibs::metamonad::name;

template <class A>
struct maybe_something :
  boost::mpl::if_<
    typename boost::is_same<A, int>::type,
    just<double>,
    nothing
  >
{};

typedef
  boost::mpl::at<
    match<syntax<just<var<x>>>, maybe_something<int>::type>::type,
    var<x>
  >::type
  this_is_double;
```

<p class="copyright">
Copyright Abel Sinkovics (abel at elte dot hu) 2011.
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
[http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt)
</p>

[[up]](reference.html)



