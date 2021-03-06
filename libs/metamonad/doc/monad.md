# monad

## Synopsis

```cpp
template <class MonadTag>
struct monad;
  // Requires:
  //   publicly inherit from monad_defaults<MonadTag>
  //   struct return_ { template <class> struct apply; };
  //   struct bind { template <class, class> struct apply; };

template <class MonadTag>
struct monad_defaults
{
  struct bind_
  {
    template <class A, class B>
    struct apply
    {
      // unspecified
    };
  };
};
```

## Description

This is a typeclass for monads. For a monad a set of template metaprogramming
values has to be (informally) specified. This set of values is called the
_monadic values_. The typeclass requires the following operations to be
implemented:

* `return_` taking some value and returning a monadic value
* `bind` returning a monadic value. It takes the following arguments:
    * a monadic value
    * a nullary metafunction taking some value and returning a monadic value

For any `MonadTag`, the operations are expected to meet the following
requirements:

* left identity:
    `bind<return_<X>, F>` is equivalent to `apply<F, X>`.
* right identity:
    `bind<M, return_>` is equivalent to `M`.
* associativity:
    `bind<M, lambda_c<x, bind<apply<F, x>, G>>>` is equivalent to
    `bind<bind<M, F>, G>`.

The typeclass implements the following operation:

* `bind_` taking two monadic values as arguments and returns a value.

Due to the way Metamonad [handles versioning](versioning.html), the `monad`
template class has to be specialised in the `mpllibs::metamonad::v1` namespace.

## Header

```cpp
#include <mpllibs/metamonad/monad.hpp>
```

## Example

```cpp
using boost::mpl;

struct join_lists
{
  template <class State, class NewList>
  struct apply :
    insert_range<
      State,
      typename end<State>::type,
      NewList
    >
  {};
};

struct list_tag;

template <>
struct monad<list_tag> : monad_defaults<list_tag>
{
  struct return_
  {
    template <class T>
    struct apply : list<T> {};
  };
  
  struct bind
  {
    template <class A, class F>
    struct apply : fold<typename transform<A, F>::type, list<>, join_lists> {};
  };
};
```

<p class="copyright">
Copyright Abel Sinkovics (abel at elte dot hu) 2011.
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
[http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt)
</p>

[[up]](reference.html)



