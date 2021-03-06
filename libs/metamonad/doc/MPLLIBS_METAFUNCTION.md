# MPLLIBS_METAFUNCTION

## Synopsis

```cpp
#define MPLLIBS_METAFUNCTION(name, args) \
  // unspecified
```

## Description

This macro is intended to be used for defining forwarding metafunctions. The
macro call expands to metafunction forwarding using inheritance. `args` is the
list of template arguments, each of them in parentheses. The `body` is expected
to follow the macro call in double parentheses. `args` can not be empty.

The metafunctions defined using this macro support currying.

`body` is expected to use `typename` for all dependent names, even when in the
following situation:

```cpp
MPLLIBS_METAFUNCTION(foo, (A)) ((typename A::type));
```

## Header

```cpp
#include <mpllibs/metamonad/metafunction.hpp>
```

## Expression semantics

Given an `eval_arg` template

```cpp
template <class T>
struct eval_arg : T {};
```

inheriting from its argument, for any `body` angly-bracket expression, `n > 0`
and `arg1` ... `argn` template arguments the following

```cpp
MPLLIBS_METAFUNCTION(name, (arg1)(arg2)...(argn)) ((body));
```

is equivalent to

```cpp
template <class arg1, class arg2, ..., class argn>
struct name___impl : eval_arg<body> {};
```

For any `n >= k`, `t1`, ..., `tk` classes and `v1`, ..., `vn-k` variables

```cpp
name<t1, ..., tk>::type
```

is equivalent to

```cpp
lambda_c<v1, ..., vn-k, name__impl<t1, ..., tk, v1, ..., vn-k>>
```

## Example

```cpp
using namespace boost::mpl;

MPLLIBS_METAFUNCTION(double_value, (N)) ((times<int_<2>, N>));
```

<p class="copyright">
Copyright Abel Sinkovics (abel at elte dot hu) 2011.
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
[http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt)
</p>

[[up]](reference.html)



