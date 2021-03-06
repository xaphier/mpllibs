# maybe monoid

This is the C++ template metaprogramming equivalent of Haskell's Maybe monoid.

## General information

* header: `<mpllibs/metamonad/maybe.hpp>`
* tag of monad: `maybe_tag<T>` where `T` is the tag of the value wrapped by
  `just`.
* `mempty`: `nothing`
* `mappend`:
    * When both arguments are `nothing`, the result is `nothing`
    * When one argument is `nothing`, the result is the other argument
    * When none of the arguments is `nothing`, the two values wrapped by `just`
      get merged by using `mappend` on the monoid formed by `T` values. The
      result of this gets wrapped by `just`.

<p class="copyright">
Copyright Abel Sinkovics (abel at elte dot hu) 2011.
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
[http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt)
</p>

[[up]](reference.html)


