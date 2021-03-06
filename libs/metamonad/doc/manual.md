# mpllibs::metamonad user manual

## Table of contents

## Introduction

This library is built on top of [Boost.MPL](http://boost.org/libs/mpl), thus
the reader is expected to be familiar with that library and its concepts.
This tutorial introduces the different components of Metamonad using examples.
The tutorial refers to things coming from
[Boost.MPL](http://boost.org/libs/mpl) as `mpl::...` and omits the `boost::`
namespace.

## Template metafunctions

Template metafunctions are one of the basic concepts template metaprograms are
built around. To define one, the developer has to write a class or a template
class, for example:

```cpp
template <class A, class B>
struct add : mpl::plus<A, B> {};
```

The above metafunction adds two values by calling `mpl::plus`. When someone
reads the code, how can he tell, that `add` is a metafunction, not just a simple
template class? The fact that `add` inherits from `mpl::plus` suggests that it
is a template metafunction, but it may not be that obvious. At the end of the
day, `add` is a template class and is represented as such in the program code.
Metamonad provides the [`MPLLIBS_METAFUNCTION`](MPLLIBS_METAFUNCTION.html)
macro that helps readability here:

```cpp
MPLLIBS_METAFUNCTION(add, (A)(B)) ((mpl::plus<A, B>));
```

This macro takes the name of the metafunction as its first argument and the list
of arguments in brackets as the second argument. This argument list is a
sequence data structure coming from
[Boost.Preprocessor](http://boost.org/libs/preprocessor). The macro call has
to be followed by the body of the macro in double parentheses.

The use of this macro makes it explicit, that `add` is a template metafunction,
not just some random template class. 

### Looking at the result of a metafunction

It is useful to be able to look at the result of calling a metafunction. It
helps development and is invaluable during debugging. Tools like
[Metashell](https://github.com/sabel83/metashell) and
[Templight](http://plc.inf.elte.hu/templight/) can help doing this. They are
based on Clang, thus the compiler evaluating the metaprograms is Clang. Other
compilers may evaluate the metaprograms in a different way and produce other
results. In this case these tools are less useful.

Metamonad offers the [`fail_with_type`](fail_with_type.html) template function.
It can be used to check the result of the above `add` function:

```cpp
int main()
{
  fail_with_type< add<mpl::int_<11>, mpl::int_<2>>::type >();
}
```

Compiling this code generates an error message which contains the result of
calling `add<mpl::int_<11>, mpl::int_<2>>`.

### Currying

What happens when someone tries calling a metafunction with less arguments than
what it expects? For example:

```cpp
add<mpl::int_<1>>::type
```

`add` expects two arguments: the two values to add, but the above expression
called it with only one argument. Metafunctions implemented using
[`MPLLIBS_METAFUNCTION`](MPLLIBS_METAFUNCTION.html) support _currying_, which
is a form of partial evaluation. When the number of arguments provided is less
than what the metafunction needs, it returns a metafunction class expecting the
remaining arguments. The body of the metafunction is evaluated only when all
arguments are provided.

Thus the above expression returns a metafunction class expecting one argument
and then it adds `1` to that argument. It can be used to increment all items of
an `mpl::vector` by one:

```cpp
mpl::transform_view<
  mpl::vector_c<int, 10, 12>,
  add<mpl::int_<1>>
>
```

This example passes the partially evaluated `add<mpl::int_<1>>` to
`mpl::transform_view`. When `mpl::transform_view` calls this partially
evaluated metafunction with the elements of the vector, it adds `1` to them.

Providing _no arguments_ to `add` turns it into a metafunction class taking two
arguments, for example:

```cpp
mpl::apply<add<>, mpl::int_<10>, mpl::int_<3>>::type
```

The above example calls the metafunction class `add<>` with two arguments,
`mpl::int_<10>` and `mpl::int_<3>`. The result of this call is `mpl::int_<13>`.

`add<>` can be used to summarise the elements of a vector:

```cpp
mpl::fold<
  mpl::vector_c<int, 1, 1, 2, 3, 6>,
  mpl::int_<0>,
  add<>
>
```

This example folds over the elements of the vector `[1, 1, 2, 3, 6]`. The
initial state is `0` and the forward operation adds the current element to the
state. The result of the above expression is `mpl::int_<13>`.

Currying makes it easy to build template metafunction classes from template
metafunctions in many cases and can reduce the amount syntactic noise this way.

## Angle-bracket expressions

A call to a metafunction, such as `mpl::plus` looks like the following:

```cpp
mpl::plus<mpl::int_<1>, mpl::int_<2>>
```

The difference between this syntax and the syntax of function calls in C++ is
that this uses angle-brackets. The above expression is an instance of a
template class, but for a template metaprogrammer, this is a template
metaprogramming expression. These expressions are called _angle-bracket
expressions_. Angle-bracket expressions play a key role in providing the tools
Metamonad provides, thus it is important to be able to build and evaluate such
expressions. They may not be as simple as the above example, since metafunction
calls can be combined in one expression:

```cpp
mpl::plus<
  mpl::minus<mpl::int_<2>, mpl::int_<1>>,
  mpl::plus<mpl::int_<1>, mpl::int_<1>>
>
```

If `mpl::plus` and `mpl::minus` were normal C++ functions, the above expression
would look like the following:

```cpp
mpl::plus(mpl::minus(2, 1), mpl::plus(1, 1))
```

The evaluation of such expressions happens the following way: `mpl::minus(2, 1)`
and `mpl::plus(1, 1)` are evaluated in some order and their results are passed
to `mpl::plus` as arguments.

The evaluation of an angle-bracket expression in template metaprogramming
happens in a different way. An angle-bracket expression can be evaluated by
accessing its `::type`. For example:

```cpp
mpl::plus<
  mpl::minus<mpl::int_<2>, mpl::int_<1>>,
  mpl::plus<mpl::int_<1>, mpl::int_<1>>
>::type
```

This evaluates the outermost `mpl::plus` and passes the sub-expressions

* `mpl::minus<mpl::int_<2>, mpl::int_<1>>`
* `mpl::plus<mpl::int_<1>, mpl::int_<1>>`

to `mpl::plus` as arguments. While in C++ it is guaranteed, that all of the
arguments of a function are evaluated before the function is called, it is not
true for angle-bracket expressions. The template metaprogrammer can enforce the
evaluation of a sub-expression by accessing its `::type`, otherwise the
sub-expression is passed to the function as it is. To evaluate all the arguments
of `mpl::plus` before it is called the above example needs to be changed:

```cpp
mpl::plus<
  mpl::minus<mpl::int_<2>, mpl::int_<1>>::type,
  mpl::plus<mpl::int_<1>, mpl::int_<1>>::type
>::type
```

In this case `mpl::minus<mpl::int_<2>, mpl::int_<1>>::type` is a `typedef` of
`mpl::int_<1>` and `mpl::plus<mpl::int_<1>, mpl::int_<1>>::type` is a `typedef`
of `mpl::int_<2>`. Thus, the outermost `mpl::plus` is called with `mpl::int_<1>`
and `mpl::int_<2>` as its arguments. But using `::type` inside an angle-bracket
expression changes its meaning. When `::type` is used for a sub-expression, the
sub-expression itself is not part of the angle-bracket expression any more - it
is replaced by the result of the evaluation.

To be able to pass the sub-expressions to `mpl::plus` without accessing their
`::type`, `mpl::plus` has to accept angle-bracket expressions as arguments.

## Nullary metafunctions

When a class has a nested type called `type`, one can look at it as a
metafunction taking no arguments. Such classes are called _nullary
metafunctions_. Template metaprogrammers come across nullary metafunctions more
often than they may expect it. For example angle-bracket expressions built of
metafunction calls are nullary metafunctions, since they have a nested `::type`
type.

## Lazy metafunctions

Let's start with a simple expression:

```cpp
mpl::plus<
  mpl::int_<11>,
  mpl::int_<2>
>::type
```

This doesn' seem to be a difficult one, it returns `mpl::int_<13>`. Now let's
make it a bit more complicated:

```cpp
mpl::plus<
  mpl::int_<11>,
  mpl::if_<
    mpl::true_,
    mpl::int_<2>,
    mpl::int_<3>
  >
>::type
```

The only difference is that instead of using `mpl::int_<2>` as the second
argument of `mpl::plus`, this code uses `mpl::if_` with a constant true value as
the condition to choose from `mpl::int_<2>` and `mpl::int_<3>`. Of course
`mpl::if_` chooses `mpl::int_<3>`.

This seems to be fine, even though it isn't. Evaluating this example breaks the
compilation. `mpl::plus` complains that
`mpl::if_<mpl::true_, mpl::int_<2>, mpl::int_<3>>` is not a number. This might
seem strange, since evaluating it gives `mpl::int_<2>`. The key thing here is
_evaluating it_ gives `mpl::int_<2>` - it is not evaluated yet, it is a nullary
metafunction waiting to be evaluated.

If `mpl::plus` and `mpl::if_` were normal C++ functions and not metafunctions,
the expression would look like this:

```cpp
plus(11, if_(true, 2, 3))
```

Of course, integer and boolean literals are used instead of the wrapped ones.
During the evaluation of such an expression the arguments are evaluated before
they are passed to a function, thus `if_(true, 2, 3)` is evaluated before it is
passed to `mpl::plus`. This makes this thing work. However, in template
metaprogramming it happens in a different way: the arguments passed to a
template metafunction are not evaluated. They are treated as angle-bracket
expressions and passed to metafunctions like `mpl::plus` as they are. It is the
responsibility of `mpl::plus` to evaluate them if needed.

`mpl::plus` does not evaluate its arguments, it assumes, that they are already
evaluated. Template metafunctions evaluating their arguments are called
_lazy template metafunctions_, while metafunctions assuming that their arguments
are already evaluated are called _strict template metafunctions_. When a
metafunction is strict - like `mpl::plus` - it is the caller's responsibility to
evaluate the arguments before passing them to the metafunction. It can be done
by passing the `::type` of the argument instead of the argument itself to the
metafunction:

```cpp
mpl::plus<
  mpl::int_<11>,
  mpl::if_<
    mpl::true_,
    mpl::int_<2>,
    mpl::int_<3>
  >::type
>::type
```

This code evaluates the second argument of `mpl::plus` before the metafunction
call happens by passing `mpl:if_<...>::type` to `mpl::plus`.

### Writing lazy metafunctions

How can one ensure, that the metafunction he writes is lazy? For example when
someone writes the following metafunction:

```cpp
MPLLIBS_METAFUNCTION(add2, (N)) ((add<mpl::int_<2>, N>));
```

Is `add2` lazy? Well, it depends on `add`. Since `add2` passes its argument
further to `add`, it does not have evaluate it to be lazy. If `add` is lazy, it
makes `add2` lazy, but if `add` is strict, `add2` will be strict.

The metafunctions [Boost.MPL](http://boost.org/libs/mpl) provides are strict.
For example, `mpl::plus` expects that its arguments are evaluated. However, one
can easily write a lazy wrapper for it:

```cpp
MPLLIBS_METAFUNCTION(lazy_plus, (A)(B))
((mpl::plus<typename A::type, typename B::type>));
```

This wrapper handles only the case when exactly two arguments are provided, but
it is enough to demonstrate how to build a lazy wrapper for a strict
metafunction. `lazy_plus` is a lazy metafunction, it evaluates its arguments and
passes them to the strict metafunction `mpl::plus` that does the addition.

When writing such a wrapper, one has to evaluate all of the arguments. Metamonad
provides a macro for these cases, it is
[`MPLLIBS_LAZY_METAFUNCTION`](MPLLIBS_LAZY_METAFUNCTION.html). It is similar
to [`MPLLIBS_METAFUNCTION`](MPLLIBS_METAFUNCTION.html), however, it evaluates
its arguments. It can be used to implement the above example:

```cpp
MPLLIBS_LAZY_METAFUNCTION(lazy_plus, (A)(B)) ((mpl::plus<A, B>));
```

`A` and `B` refer to `typename A::type` and `typename B::type` in the body of
the metafunction, thus it makes it easier to write these lazy wrappers.

Using the arithmetic operations of [Boost.MPL](http://boost.org/libs/mpl)
through the lazy wrappers, one can evaluate the complex expression

```cpp
lazy_plus<
  mpl::int_<11>,
  mpl::if_<
    mpl::true_,
    mpl::int_<2>,
    mpl::int_<3>
  >
>
```

This code still passes an angle-bracket expression as argument to `lazy_plus`,
but it evaluates its argument before it does the addition, thus the above code
works.

### Using strict metafunctions as they were lazy

Wrapping all strict metafunctions with lazy wrappers is possible, but it is
still a lot of work and produces a large amount of code that needs to be
maintained in the future. Metamonad provides a tool, using which there is no
need for these lazy wrappers. It is the [`lazy`](lazy.html) template. It
wraps an angle-bracket expression and changes the way arguments are passed to
the metafunctions. It evaluates the arguments of the metafunctions before
calling them. The above example can be implemented using it:

```cpp
lazy<
  mpl::plus<
    mpl::int_<11>,
    mpl::if_<
      mpl::true_,
      mpl::int_<2>,
      mpl::int_<3>
    >
  >
>
```

The original metafunctions coming from [Boost.MPL](http://boost.org/libs/mpl)
can be used as they were lazy metafunctions because [`lazy`](lazy.html) makes
sure that their arguments are evaluated before they are passed to them.
[`lazy`](lazy.html) evaluates the above expression the following way:

```cpp
mpl::plus<
  mpl::int_<11>::type,
  mpl::if_<
    mpl::true_::type,
    mpl::int_<2>::type,
    mpl::int_<3>::type
  >::type
>::type
```

As `mpl::if_<mpl::true_, mpl::int_<2>, mpl::int_<3>>::type` is `mpl::int_<2>`,
this wrapped number is passed to `mpl::plus`.

`mpl::if_` chooses either its second or third argument based on the value of the
condition. Putting an expression that should not be evaluated, such as
`divides<mpl::int_<1>, mpl::int_<0>>` should not be a problem, since this
expression trying to divide `1` with `0` should not be selected by `mpl::if_`:

```cpp
mpl::plus<
  mpl::int_<11>,
  mpl::if_<
    mpl::true_,
    mpl::int_<2>,
    mpl::divides<mpl::int_<1>, mpl::int_<0>>
  >
>
```

However, putting the entire expression into [`lazy`](lazy.html) changes
things. It makes sure that _all_ arguments of a metafunction are evaluated
before the metafunction is called and since `mpl::if_` is a metafunction, it
evaluates all of its arguments as well - which includes _both_ cases, not just
the selected one. Thus, [`lazy`](lazy.html) evaluates the expression trying
to divide `1` by `0` before `mpl::if_` would get the chance to choose the other
one.

To solve these issues, Metamonad provides the
[`already_lazy`](already_lazy.html) template which can be used inside 
[`lazy`](lazy.html) to protect some sub-expressions from the enforced
argument evaluation.

```cpp
lazy<
  mpl::plus<
    mpl::int_<11>,
    mpl::if_<
      mpl::true_,
      mpl::int_<2>,
      already_lazy<mpl::divides<mpl::int_<1>, mpl::int_<0>>>
    >
  >
>::type
```

[`lazy`](lazy.html) leaves things wrapped with `already_lazy` alone. Thus,
the above expression evaluates the following:

```cpp
mpl::plus<
  mpl::int_<11>::type,
  mpl::if_<
    mpl::true_::type,
    mpl::int_<2>::type,
    mpl::divides<mpl::int_<1>, mpl::int_<0>>
  >::type
>::type
```

The arguments that were not wrapped with [`already_lazy`](already_lazy.html)
are evaluated before they are passed to `mpl::if`, however, the expression that
tries to divide by `0` was wrapped with [`already_lazy`](already_lazy.html)
and is passed unchanged to `mpl::if_`.

### Using lazy in the body of a metafunction

Let's create a metafunction from the above example and replace `mpl::int_<11>`
with a metafunction argument:

```cpp
MPLLIBS_METAFUNCTION(add2, (N))
((
  lazy<
    mpl::plus<
      N,
      mpl::if_<
        mpl::true_,
        mpl::int_<2>,
        mpl::int_<3>
      >
    >
  >
));
```

This code adds `mpl::int_<2>` to its argument. To keep it simple `mpl::int_<3>`
is used instead of the expression trying to divide by `0`.

Calling this metafunction with `mpl::int_<11>` gives the expected result,
`add2<mpl::int_<11>>::type` is `mpl::int_<13>`. However calling it with a
complex expression can break the compilation:

```cpp
add2<
  mpl::if_<
    mpl::true_,
    mpl::int_<11>,
    mpl::divides<mpl::int_<1>, mpl::int_<0>>
  >
>::type
```

When a call like this is done, the body of `add2` is evaluated:

```cpp
lazy<
  mpl::plus<
    mpl::if_<
      mpl::true_,
      mpl::int_<11>,
      mpl::divides<mpl::int_<1>, mpl::int_<0>>
    >,
    mpl::if_<
      mpl::true_,
      mpl::int_<2>,
      mpl::int_<3>
    >
  >
>::type
```

Since arguments are not evaluated when they are passed to a metafunction, the
angle-bracket expression `mpl::if_<...>` is used inside the body of `add2`. But
it is used inside [`lazy`](lazy.html) and therefore it is evaluated as it was
originally written inside [`lazy`](lazy.html). This is probably not what the
caller of `add2` expects. In the above example it evaluates the division by zero
for example.

To avoid these bad surprises, when using [`lazy`](lazy.html) in the body of a
metafunction, the arguments have to be protected. Let's protect the argument
with [`already_lazy`](already_lazy.html):

```cpp
MPLLIBS_METAFUNCTION(add2, (N))
((
  lazy<
    mpl::plus<
      already_lazy<N>,
      mpl::if_<
        mpl::true_,
        mpl::int_<2>,
        mpl::int_<3>
      >
    >
  >
));
```

This code protects the argument `N` inside the [`lazy`](lazy.html) block with
[`already_lazy`](already_lazy.html), thus it is left untouched by
[`lazy`](lazy.html). Calling `add2` with the same argument as before
evaluates the body the following way:

```cpp
lazy<
  mpl::plus<
    already_lazy<
      mpl::if_<
        mpl::true_,
        mpl::int_<11>,
        mpl::divides<mpl::int_<1>, mpl::int_<0>>
      >
    >,
    mpl::if_<
      mpl::true_,
      mpl::int_<2>,
      mpl::int_<3>
    >
  >
>::type
```

Because of using [`lazy`](lazy.html), it evaluates the following expression:

```cpp
mpl::plus<
  mpl::if_<
    mpl::true_,
    mpl::int_<11>,
    mpl::divides<mpl::int_<1>, mpl::int_<0>>
  >,
  mpl::if_<
    mpl::true_::type,
    mpl::int_<2>::type,
    mpl::int_<3>::type
  >::type
>::type
```

As the argument value was protected with [`already_lazy`](already_lazy.html),
it is left untouched while the rest of the expression is evaluated at all
levels. The two arguments passed to `mpl::plus` are two angle-bracket
expression, however the first one - which was the argument - is not evaluated
while the second one is evaluated. Not evaluating the argument is what we used
[`already_lazy`](already_lazy.html) for, but in this case this angle-bracket
expression is given to `mpl::plus` which can not deal with it. Using
[`already_lazy`](already_lazy.html) made `add2` a strict metafunction.

To make `add2` lazy, the argument has to be evaluated before it is passed to
`mpl::plus`, but it should be evaluated by getting its `::type` and without
evaluating any sub-expression of it. Using
[`already_lazy`](already_lazy.html) does not evaluate the argument, but not
using it evaluates it by evaluating all the sub-expressions as well. Another
tool is needed here, which is [`lazy_protect_args`](lazy_protect_args.html).
When it is used inside [`lazy`](lazy.html), the expression wrapped by it is
evaluated but without evaluating any of its sub-expressions.

```cpp
MPLLIBS_METAFUNCTION(add2, (N))
((
  lazy<
    mpl::plus<
      lazy_protect_args<N>,
      mpl::if_<
        mpl::true_,
        mpl::int_<2>,
        mpl::int_<3>
      >
    >
  >
));
```

This version of `add2` is lazy, because by using
[`lazy_protect_args`](lazy_protect_args.html) it evaluates the argument `N`
but without changing the way it would be normally evaluated, thus it won't
surprise the caller of `add2`.

The [`lazy`](lazy.html) template makes it possible to use strict
metafunctions in larger expression, however, as the examples demonstrate, it
adds syntactic overhead to the code, thus using (and building) lazy
metafunctions keeps the code readable and makes it possible to build complex
expressions.

## Tags

In [Boost.MPL](http://boost.org/libs/mpl) compile-time values have a member
type called `tag` that is used to determine whether that value is a wrapped
integral, a list, a vector, etc. There may be more than one implementations of a
compile-time data-type, such as wrapped integrals. As long as they provide the
same interface and have the same `tag`, all metafunctions operating on the
wrapped integrals can deal with them. The `tag` is used to identify that they
are wrapped integrals.

This `tag` works like the type information of compile-time data-structures. 
Polymorphic metafunctions in [Boost.MPL](http://boost.org/libs/mpl) get the
`tag` information of the arguments, instantiate a helper metafunction class with
the tags as template parameters and pass the original arguments to that
metafunction class. Different specializations of the metafunction class can be
created for different tags and tag combinations. Each specialization implements
one overload of the metafunction. Since a new specialization can be implemented
without changing existing code, new overloads for newly created classes can be
implemented later.

Metamonad treats tags as first class citizens of template metaprograms, thus
they are expected to be passed around as data in metaprograms. But in that case
tags need to have a tag as well. Metamonad provides the tag
[`tag_tag`](tag_tag.html) for that.

Metamonad provides the [`tmp_tag`](tmp_tag.html) to help defining tags:

```cpp
struct my_tag : tmp_tag<my_tag> {};
```

The above usage makes use of the
[Curiously recurring template pattern](
  http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
) and provides a `::type` referencing `my_tag` and a `::tag` referencing
`tag_tag`. `my_tag::type` is `my_tag` and `my_tag::tag` is `tag_tag`. `::type`
is needed to avoid lazy metafunctions crashing when one passes a tag to them.

## Template metaprogramming values

Let's add two numbers together using `lazy_plus`:

```cpp
lazy_plus<mpl::int_<1>, mpl::int_<1>>
```

Assuming `lazy_plus` is a lazy wrapper around `mpl::plus`, the first thing it
does is evaluating its arguments, thus taking `mpl::int_<1>::type`. But
`mpl::int_<1>` represents a number, it is not a nullary metafunction. In order
to make it work in situations like this one, it has to be a nullary metafunction
evaluating to itself. Such classes are called _template metaprogramming values_.

Metamonad assumes, that every class that is used as a value in template
metaprogramming is a nullary metafunction evaluating to itself. Metamonad also
assumes, that every metafunction returns a metaprogramming value, thus taking
`::type` twice yields the same result as taking it only once. For example
`mpl::plus<mpl::int_<1>, mpl::int_<1>>::type::type` means the same as
`mpl::plus<mpl::int_<1>, mpl::int_<1>>::type`.

Metamonad provides the [`tmp_value`](tmp_value.html) template for supporting
the creation of metaprogramming values. This template is expected to be used the
following way:

```cpp
struct some_class : tmp_value<some_class> {};
```

The above usage makes use of the [Curiously recurring template pattern](
  http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
) and provides a `::type` referencing `some_class`. Now `some_class::type` is
`some_class`.

[`tmp_value`](tmp_value.html) supports specifying the tag of a class as well.
It accepts an optional second argument, which is when provided defines `::tag`
referencing that second argument. For example:

```cpp
struct some_tag : tmp_tag<some_tag> {};

struct some_class  : tmp_value<some_class, some_tag> {};
```

Using the above, `some_class::type` is `some_class`, `some_class::tag` is
`some_tag`.

### Template metaprogramming values and if

One of the basic programming constructs is `if`, which chooses from two cases
based on a condition. A version of such a construct, that fulfils Metamonad's
assumptions about metafunctions is needed. Such an `if` should always return a
template metaprogramming value.

[Boost.MPL](http://boost.org/libs/mpl) provides two `if` constructs:

* `mpl::if_<C, T, F>` returns either `T` or `F` based on the value of `C`. When
  `T` or `F` is a nullary metafunction, the nullary metafunction is returned,
  which may not be a metaprogramming value.
* `mpl::eval_if<C, T, F>` chooses one from `T` and `F` based on the condition,
  evaluates it and returns the result. Even when the arguments are nullary
  metafunctions, `mpl::eval_if` returns their result - which must be a
  metaprogramming value based on Metamonad's assumption.

Even though `mpl::eval_if` fulfils Metamonad's assumptions - as later sections
present it - Metamonad gives a different meaning to the `eval_` prefix than what
[Boost.MPL](http://boost.org/libs/mpl) uses it for. With Metamonad's
terminology, the selection function should be called `if_`.

Metamonad provides the [`if_`](if_.html) metafunction that accepts nullary
metafunctions as the condition and evaluates the selected argument.

## Boxing

The assumption, that everything template metaprograms operate on is a template
metaprogramming value makes building template metaprograms easier, but it also
means that common types, such as `int`, `double`, etc can not be used in
template metaprograms. Wrapping things that are available at compile-time but
can not be used directly in template metaprograms works well for constant
integral values, why wouldn't it work for classes as well? Metamonad provides
a wrapper class, [`box`](box.html), that wraps any type and turns it into a
metaprogramming value. Boxed types can be safely passed around and unboxed
using [`unbox`](unbox.html) at the end.

## Syntaxes

Angle-bracket expressions can be treated as template metaprogramming code
blocks. If they can be passed around in template metaprograms, complex
control structures can be implemented. But angle-bracket expressions are not
template metaprogramming values - when someone tries accessing their `::type`
they get evaluated. They could be boxed as any other type, but the only thing
that can be done with a boxed type is unboxing it, while there are more
operations for angle-bracket expressions.

Metamonad provides [`syntax`](syntax.html), a custom wrapper for
angle-bracket expressions. An angle-bracket expression wrapped with this
template is called a _syntax_. For example the following syntax can be passed
around as a value in template metaprograms:

```cpp
syntax<
  mpl::plus<mpl::int_<9>, mpl::int_<4>>
>
```

When a lazy metafunction evaluates it, it does not evaluate the wrapped
angle-bracket expression:

```cpp
syntax<
  mpl::plus<mpl::int_<9>, mpl::int_<4>>
>::type
```

returns

```cpp
syntax<
  mpl::plus<mpl::int_<9>, mpl::int_<4>>
>
```

Such syntaxes make sense only when the wrapped expression can be evaluated when
the time has come. Metamonad provides the [`eval_syntax`](eval_syntax.html)
metafunction for that.

```cpp
eval_syntax<
  syntax<
    mpl::plus<mpl::int_<9>, mpl::int_<4>>
  >
>::type
```

The above expression evaluates to `mpl::int_<13>`.

## Variables

An expression may contain variables. When it does, expressions like

```cpp
syntax<
  mpl::plus<a, b>
>
```

can be constructed, where `a` and `b` represent the open variables of the
expression. The value of such variables can then be specified later.
Metamonad assumes, that instances of the [`var`](var.html) template are
variables. The [`var`](var.html) template class has a template argument,
which identifies the variable. Any class that has been declared can be used as
an identifier. For example a variable called `state` can be created the
following way:

```cpp
struct state;

var<state>
```

Another option to name variables is to use template metaprogramming strings as
the identifier. The `MPLLIBS_STRING` macro of Metaparse helps creating them.
For example:

```cpp
var<MPLLIBS_STRING("state")>
```

Metamonad provides pre-defined variables that can be used without declaring
anything. These names are in the `mpllibs::metamonad::name` namespace, thus
writing

```cpp
using namespace mpllibs::metamonad::name;
```

brings only these pre-defined variables in. These variables are called
`a`, `b`, ..., `z`. They are easy to use in expressions, for example:

```cpp
using namespace mpllibs::metamonad::name;

syntax<
  mpl::plus<a, b>
>
```

`a`, `b`, ... are `typedefs` of things wrapped with [`var`](var.html), thus
they can be used without having to write [`var`](var.html) in the code.

## Let expressions

A syntax with variables is not something that could be evaluated.

```cpp
syntax<
  mpl::plus<a, b>
>
```

Trying to evaluate the above syntax using [`eval_syntax`](eval_syntax.html)
would lead to a compilation error, since `mpl::plus` would complain, that an
integer can not be added to a variable. To make the above expression evaluable,
the variables have to be replaced with values. Metamonad provides
[`let`](let.html) for this substitution. It takes three arguments:

* the variable to substitute
* the syntax, occurrences of the variable should be replaced with
* the expression using the variable

For example

```cpp
let<
  a, syntax<mpl::int_<9>>,
  syntax<
    mpl::plus<a, b>
  >
>::type
```

Replaces `a` with `mpl::int_<9>`. It returns the following syntax:

```cpp
syntax<
  mpl::plus<mpl::int_<9>, b>
>
```

It is still not something that can be evaluated, `b` needs to be replaced with a
value as well:

```cpp
let<
  b, syntax<mpl::int_<4>>,
  syntax<
    let<
      a, syntax<mpl::int_<9>>,
      syntax<
        mpl::plus<a, b>
      >
    >
  >
>::type
```

The outer [`let`](let.html) replaces `b` with `mpl::int_<4>`. But things
wrapped with [`syntax`](syntax.html) are treated as values, thus when a
syntax contains another thing wrapped with [`syntax`](syntax.html), 
[`let`](let.html) does not change it. Since the expression `mpl::plus<a, b>`
is wrapped with [`syntax`](syntax.html), the outer [`let`](let.html) does
not change it.

Metamonad provides another version of [`let`](let.html),
[`let_c`](let_c.html) to resolve this issue. This is similar to
[`let`](let.html), but instead of taking syntaxes, it takes angle-bracket
expressions as arguments and wraps them with [`syntax`](syntax.html)
internally.

```cpp
let_c<
  b, mpl::int_<4>,
  let_c<
    a, mpl::int_<9>,
    mpl::plus<a, b>
  >
>::type
```

The above code uses [`let_c`](let_c.html) instead of [`let`](let.html).
The outer [`let_c`](let_c.html) does the substitution and wraps its result
with [`syntax`](syntax.html), thus it returns

```cpp
syntax<
  let_c<
    a, mpl::int_<9>,
    mpl::plus<a, mpl::int_<4>>
  >
>
```

To get the fully substituted expression, this syntax needs to be evaluated:

```cpp
eval_syntax<
  syntax<
    let_c<
      a, mpl::int_<9>,
      mpl::plus<a, mpl::int_<4>>
    >
  >
>
```

Evaluating the syntax evaluates the inner [`let_c`](let_c.html) of the
original expression, which substitutes `a` with `mpl::int_<9>` and returns

```cpp
syntax<
  mpl::plus<mpl::int_<9>, mpl::int_<4>>
>
```

Which is the original syntax with both substitutions done. Thus, the working
version of this example is the following:

```cpp
eval_syntax<
  let_c<
    b, mpl::int_<4>,
    let_c<
      a, mpl::int_<9>,
      mpl::plus<a, b>
    >
  >
>::type
```

The outer [`let_c`](let_c.html) substitutes `b` and returns a syntax
containing the inner [`let_c`](let_c.html). This syntax is evaluated by
[`eval_syntax`](eval_syntax.html), which evaluates the inner
[`let_c`](let_c.html) that does the other substitution and gives the fully
substituted version of `syntax<mpl::plus<a, b>>`.

[`let_c`](let_c.html) used inside [`eval_syntax`](eval_syntax.html) is so
common, that Metamonad provides [`eval_let_c`](eval_let_c.html) that does
this:

```cpp
eval_let_c<
  b, mpl::int_<4>,
  let_c<
    a, mpl::int_<9>,
    mpl::plus<a, b>
  >
>::type
```

This example uses [`eval_let_c`](eval_let_c.html) instead of using
[`let_c`](let_c.html) inside [`eval_syntax`](eval_syntax.html) to make the
code more compact. The inner [`let_c`](let_c.html) remains
[`let_c`](let_c.html), thus it returns the fully substituted syntax. If this
needs to be evaluated as well, [`eval_let_c`](eval_let_c.html) can be used
there as well. To see the difference:

* When the inner [`let_c`](let_c.html) remains [`let_c`](let_c.html), the
  result of the above expression is
  `syntax<mpl::plus<mpl::int_<9>, mpl::int_<4>>>`.

* When [`eval_let_c`](eval_let_c.html) is used instead of
  [`let_c`](let_c.html), the result is `mpl::int_<13>`.

The main difference between [`let_c`](let_c.html) and
[`eval_let_c`](eval_let_c.html) is that while one of them transforms a
syntax, the other one transforms a syntax and evaluates the result. In that
sense, [`eval_let_c`](eval_let_c.html) is more like the _let expressions_
other languages provide.

## Multi-let expressions

As presented in the previous chapter, substituting multiple variables inside one
expression may be difficult and one has to be careful to avoid a number of
pitfalls. Metamonad provides a special version of [`let`](let.html) for
substituting multiple variables at a time, this is
[`multi_let`](multi_let.html). It can be used the following way:

```cpp
multi_let<
  mpl::map<
    mpl::pair<b, syntax<mpl::int_<4>>>,
    mpl::pair<a, syntax<mpl::int_<9>>>
  >,
  syntax<mpl::plus<a, b>>
>::type
```

[`multi_let`](multi_let.html) takes an `mpl::map` as its first argument
describing all the variable bindings and the syntax to do the substitution in as
the second argument. It substitutes all variables specified by the `mpl::map` in
the syntax.

## Algebraic data-types

The classic implementation of a list in functional programming languages is that
a list is either an empty list or the combination of a head element and the
remaining list. This can be represented in template metaprogramming as well
using a class and a template class.

The empty list can be represented using a type:

```cpp
struct empty : tmp_value<empty> {};
```

It is a type that is also a template metaprogramming value. A list with a head
element and a remaining list can be represented using a template class:

```cpp
template <class Head, class Tail>
struct cons : tmp_value<con<Head, Tail>> {};
```

`Head` represents the first element of the list and `Tail` is a list - either
`empty` or an instance of `cons`.

Any list can be represented using these two elements. For example the empty
list is `empty`. The `[1, 2, 3, 4]` list is

```cpp
cons<
  mpl::int_<1>,
  cons<
    mpl::int_<2>,
    cons<
      mpl::int_<3>,
      cons<
        mpl::int_<4>,
        empty
      >
    >
  >
>
```

This approach uses `cons` recursively to build longer and longer lists. The
`empty` class stops this recursion.

The `empty` class and the instances of `cons` are template metaprogramming
values, thus `empty` and `cons` are used for constructing values. They are
called constructors. They represent a list together, thus the two constructors
together form a type. These types are called _algebraic data-types_. Such types
have a name and a number of constructors.

Metamonad provides the [`MPLLIBS_DATA`](MPLLIBS_DATA.html) macro for defining
algebraic data-types. The list type can be created the following way:

```cpp
MPLLIBS_DATA(list, 0, ((empty, 0))((cons, 1)));
```

The name of the type is `list`. The second macro argument, `0` specifies that
the type itself has no arguments - more on that later. The third argument of the
macro is a preprocessor sequence of two element tuples. Each tuple defines a
constructor. The first element of the tuple is the name of the constructor, the
second element is the arity.

The tag of the values built using the constructor is `algebraic_data_type_tag`,
thus the polymorphic metafunctions of [Boost.MPL](http://boost.org/libs/mpl)
can be overloaded for algebraic data-types.

The macro defines the class `list_tag`. If the second argument of
[`MPLLIBS_DATA`](MPLLIBS_DATA.html) is greater than `0`, this class becomes a
template class taking classes as template arguments. This `list_tag` class
describes the type of the values. When it is a template class, the type has type
arguments, for example defining `list` the following way

```cpp
MPLLIBS_DATA(list, 1, ((empty, 0))((cons, 1)));
```

defines `list_tag` to be a template class taking one argument. This argument can
be used to describe the type of the list elements.

The values built using the constructors don't carry this type information with
them. Metamonad constructs and metafunctions assume that when it is needed, the
calling code provides this type information as an extra argument to the
metafunctions.

### Algebraic data-types and laziness

Constructors of algebraic data-types serve as metafunctions for constructing
algebraic data-type values. They are lazy metafunctions, for example the
expression

```cpp
cons<
  mpl::plus<mpl::int_<9>, mpl::int_<4>>,
  empty
>::type
```

builds the value

```cpp
cons<
  mpl::int_<13>,
  empty
>
```

This value is then a metaprogramming value, thus

```cpp
cons<
  mpl::int_<13>,
  empty
>::type
```

gives

```cpp
cons<
  mpl::int_<13>,
  empty
>
```

When a constructor of an algebraic data-type is evaluated, it evaluates its
arguments and instantiates itself again with the evaluated arguments. When the
arguments are nullary metafunctions, this serves as a metafunction call. When
the arguments are already metaprogramming values, it turns the constructor into
a metaprogramming value as it instantiates itself again with the same arguments.

### Maybe

The division function provided by [Boost.MPL](http://boost.org/libs/mpl)
breaks the compilation when one tries dividing by zero. One could build a
version that returns some error value in such a situation - but what should be
that error value?

An algebraic data-type can be introduced to represent results that may be
failures. The most simple such type is [`maybe`](maybe.html). It has two
constructors:

* `just<...>` which represents the result of a successful computation

* `nothing` which represents the failure

Using [`maybe`](maybe.html) it is easy to implement the safe division
function:

```cpp
MPLLIBS_LAZY_METAFUNCTION(safe_divides, (A)(B))
((
  mpl::eval_if<
    mpl::equal_to<mpl::int_<0>, B>,
    nothing,
    just<mpl::divides<A, B>>
  >
));
```

This function checks if `B` is null and when it is, it returns `nothing`
indicating that the division is not valid. Otherwise it returns the result of
the division wrapped by `just`.

### Either

[`maybe`](maybe.html) is a good tool for error reporting, however, it can
only tell that there was a failure. It can not provide further details about the
problem which makes debugging much more difficult. Another algebraic data-type,
[`either`](either.html) can be used in such situations where further details
about the problem can be useful. It has two constructors:

* `right<...>` for representing results of a successful computation

* `left<...>` for representing failures

The argument of `left` can provide further details about the problem. For
example, a special class, `division_by_zero` can be used to indicate what
happened:

```cpp
struct division_by_zero : tmp_value<division_by_zero> {};

MPLLIBS_LAZY_METAFUNCTION(safe_divides, (A)(B))
((
  mpl::eval_if<
    mpl::equal_to<mpl::int_<0>, B>,
    left<division_by_zero>,
    right<mpl::divides<A, B>>
  >
));
```

This implementation of `safe_divides` returns an [`either`](either.html)
value. When the division is valid, it returns the result of it wrapped by
`right`, otherwise it returns `division_by_zero` wrapped by `left`.

## Pattern matching

Let's write a metafunction for getting the first element of a non-empty list.
The metafunction will be called `head`. For example when it is called with
`cons<mpl::int_<13>, empty>`, it should return `mpl::int_<13>`. In order to
implement such a function, a tool is needed for getting the first argument of
the `cons` constructor. Using a syntax it is easy to specify which part of the
constructor is needed:

```cpp
syntax<cons<h, _>>
```

The above syntax uses the `h` name to point out the interesting argument of the
constructor and `_` to express that the second argument can be anything, it is
not important.

Metamonad provides the [`match`](match.html) metafunction for doing the
matching:

```cpp
MPLLIBS_METAFUNCTION(head, (L))
((
  eval_syntax<
    mpl::at<
      typename match<syntax<cons<h, _>>, L>::type,
      h
    >
  >
))
```

[`match`](match.html) matches the list `L` against the pattern `cons<h, _>`.
When it matches, it returns an `mpl::map` binding variables to syntaxes. When it
doesn't match, it returns an [`exception`](exception.html), which in this
case breaks the compilation, as `mpl::at` can not deal with it. More on that
later. Let's assume, that it matches. In that case, `mpl::at` gets the syntax
`h` referred to in the pattern and the enclosing
[`eval_syntax`](eval_syntax.html) unpacks and evaluates it.

For example calling `head<cons<mpl::int_<13>, empty>>::type` evaluates the
following expression:

```cpp
eval_syntax<
  mpl::at<
    typename match<syntax<cons<h, _>>, cons<mpl::int_<13>, empty>>::type,
    h
  >
>
```

It evaluates `match<syntax<cons<h, _>>, cons<mpl::int_<13>, empty>>::type`,
which tries matching the value `cons<mpl::int_<13>, empty>` against the pattern
`cons<h, _>`. It matches and binds the value `mpl::int_<13>` to `h`, thus
[`match`](match.html) returns the following `mpl::map`:

```cpp
mpl::map<
  mpl::pair<h, syntax<mpl::int_<13>>>
>
```

This `mpl::map` describes that the value `mpl::int_<13>` is bound to `y`.

[`match`](match.html) has a [`match_c`](match_c.html) pair, which takes
the pattern as an angle-bracket expression instead of a syntax.

### Using a variable twice in a pattern

What happens when a variable appears twice in a pattern? For example

```cpp
mpl::pair<x, x>
```

is a pattern describing that it expects an `mpl::pair`. `x` should be bound to
the first element _and_ `x` should be bound to the second element as well. Only
those `mpl::pair` values match, where the two elements of the `mpl::pair` are
the same. For example:

```cpp
match_c<
  mpl::pair<x, x>,
  mpl::pair<mpl::int_<11>, mpl::int_<13>>
>::type
```

This example returns an [`exception`](exception.html), since the two elements
of `mpl::pair<mpl::int_<11>, mpl::int_<13>>` differ, thus it can not be matched
against the pattern, which expects the same value at both positions. At the same
time

```cpp
match_c<
  mpl::pair<x, x>,
  mpl::pair<mpl::int_<13>, mpl::int_<13>>
>::type
```

works fine, since in this case both elements of the `mpl::pair` are
`mpl::int_<13>`, thus it can be matched against the pattern. It returns the
following `mpl::map`:

```cpp
mpl::map<
  mpl::pair<x, syntax<mpl::int_<13>>>
>
```

This `mpl::map` binds `mpl::int_<13>` to `x`. Even though `x` appeared twice in
the pattern, it is bound only once.

Of course, the special element in the patterns, `_` can be used multiple times
without extra restrictions. Every occurrence of it means _here can be anything_.

```cpp
match_c<
  mpl::pair<_, _>,
  mpl::pair<mpl::int_<11>, mpl::int_<13>>
>::type
```

In the above example `mpl::int_<11>` is matched against the pattern `_` and
later `mpl::int_<13>` is matched against the pattern `_`. Both of them matches
and none of them is in the resulting `mpl::map`, thus, the above expression
returns

```cpp
mpl::map<>
```

It returns an empty `mpl::map`, since there were no variables in the pattern.

## Match-let expressions

An earlier example used [`multi_let`](multi_let.html) to substitute multiple
variables at the same time in one expression:

```cpp
multi_let<
  mpl::map<
    mpl::pair<b, syntax<mpl::int_<4>>>,
    mpl::pair<a, syntax<mpl::int_<9>>>
  >,
  syntax<mpl::plus<a, b>>>
>::type
```

[`multi_let`](multi_let.html) takes an `mpl::map` describing the variable
bindings and the expression to do the substitution in. The `mpl::map` describing
the bindings has the same format as the output of [`match_c`](match_c.html),
thus this map can be built using pattern matching as well:

```cpp
multi_let<
  match_c<mpl::pair<a, b>, mpl::pair<mpl::int_<9>, mpl::int_<4>>>,
  syntax<mpl::plus<a, b>>
>::type
```

This example builds a pair from the values `mpl::int_<9>` and `mpl::int_<4>` and
uses pattern matching to bind `a` to `mpl::int_<9>` and `b` to `mpl::int_<4>`.
[`match_c`](match_c.html) returns

```cpp
mpl::map<
  mpl::pair<b, syntax<mpl::int_<4>>>,
  mpl::pair<a, syntax<mpl::int_<9>>>
>
```

which is the map describing the variable bindings for
[`multi_let`](multi_let.html). Metamonad provides
[`match_let`](match_let.html), which combines [`match`](match.html) and
[`multi_let`](multi_let.html) this way and lets the developer write the above
example in a more compact way:

```cpp
match_let<
  syntax<mpl::pair<a, b>>, mpl::pair<mpl::int_<9>, mpl::int_<4>>,
  syntax<mpl::plus<a, b>>
>::type
```

It takes the pattern as the first argument, the expression to match against the
pattern as the second one and the expression to do the binding in as the third
one. It does the matching and does all the variable replacements.

The common extensions, such as [`match_let_c`](match_let_c.html),
[`eval_match_let`](eval_match_let.html) or
[`eval_match_let_c`](eval_match_let_c.html) are also available.

## Case expressions

Assume, that a `safe_divides` function is available for doing an integral
division. When the second argument is zero, it returns `nothing`, otherwise it
returns the result wrapped by `just`. Using this function, let's write another
one which takes two numbers and tries dividing them. When this is possible, it
returns the result of the division, otherwise it returns `1`. Since this
function can not fail, there is no point in wrapping the result by `just` or
something similar.

The problem here is that the result of `safe_divides` needs to be checked if it
is `nothing` or `just`, and when it is `just`, the value needs to be unwrapped.
Metamonad provides [`case_`](case_.html) to support such situations. It can
be used the following way:

```cpp
MPLLIBS_METAFUNCTION(f, (A)(B))
((
  eval_syntax<
    case_< safe_divides<A, B>,
      matches<syntax<just<x>>, syntax<x>>,
      matches<syntax<nothing>, syntax<mpl::int_<1>>>
    >
  >
));
```

The above code defines the function described earlier. It takes two arguments,
`A` and `B` and tries dividing them with `safe_divides`. The result of it is
given to [`case_`](case_.html) which has two cases, each of which is
described by a `matches` block. A `matches` block takes a pattern and a syntax.
When the value returned by the original expression matches the pattern of a
`matches` block, that block is selected and the body of it, which is the second
template argument of `matches` is returned. The open variables of the pattern
can be used in the corresponding body. The patterns are tried in order and the
first one that matches is selected. When none of them matches,
[`case_`](case_.html) returns an [`exception`](exception.html).

As [`case_`](case_.html) returns a syntax, it needs to be evaluated, that is
why the whole expression is wrapped by [`eval_syntax`](eval_syntax.html). To
reduce the syntactic noise of codes using [`case_`](case_.html), Metamonad
provides [`eval_case`](eval_case.html) which is the combination of
[`eval_syntax`](eval_syntax.html) and [`case_`](case_.html).

Metamonad provides `matches_c` as well, which is a version of `matches` that
takes the pattern and the body as angle-bracket expressions and wraps them with
[`syntax`](syntax.html) internally.

Using all the above, the `f` example can be simplified a bit:

```cpp
MPLLIBS_METAFUNCTION(f, (A)(B))
((
  eval_case< safe_divides<A, B>,
    matches_c<just<x>, x>,
    matches_c<nothing, mpl::int_<1>>
  >
));
```

This code uses `matches_c` to avoid writing [`syntax`](syntax.html) several
times and [`eval_case`](eval_case.html) immediately evaluate the body of the
`matches_c` that was selected.

## Lambda expressions

Metafunction classes are callable objects in template metaprogramming. They are
values, thus they can be passed around in template metaprograms, but they are
functions at the same time, thus they can be called. Metamonad provides tools to
construct such objects.

A metafunction class is a function and as every other function, it has function
arguments and a function body. Metamonad provides [`lambda`](lambda.html) to
construct a metafunction class:

```cpp
typedef lambda<a, b, syntax<mpl::plus<a, b>>> add;
```

The above example builds a metafunction class for adding two values. The
arguments of the function are represented by variables, the body is represented
by a syntax.

The tools provided by [Boost.MPL](http://boost.org/libs/mpl) can be used to
call this metafunction class. For example:

```cpp
mpl::apply_wrap2<add, mpl::int_<11>, mpl::int_<2>>::type
```

The above expression calls the metafunction class `add` with two arguments:
`mpl::int_<11>` and `mpl::int_<2>`. `add` evaluates
`mpl::plus<mpl::int_<11>, mpl::int_<2>>` and returns its result,
`mpl::int_<13>`.

[`lambda`](lambda.html) takes its body as a syntax, but Metamonad provides
[`lambda_c`](lambda_c.html) as well, which takes its body as an angle-bracket
expression.

What makes metafunction classes useful is that they can be passed around as data
in metaprograms. They can be arguments of or be returned by a metafunction. Such
metafunctions that operate on functions are called _higher order metafunctions_.
For example:

```cpp
MPLLIBS_METAFUNCTION(addn, (N)) ((lambda_c<x, mpl::plus<N, x>>));
```

This example defines a metafunction, `addn` taking a number, `N` as argument and
building a metafunction class taking a number and increasing it with `N`. It can
be used as a complicated way of adding two values:

```cpp
mpl::apply_wrap1<
  addn<mpl::int_<11>>::type,
  mpl::int_<2>
>::type
```

This example uses `addn` to add the value `mpl::int_<2>` to `mpl::int_<11>`. The
expression `addn<mpl::int_<11>>` builds a metafunction class expecting one
argument and adding it to `mpl::int_<11>`. `mpl::apply_wrap1` is used to call
this metafunction class with `mpl::int_<2>`.

The above example demonstrated how functions can return metafunction classes.
Let's build another example showing how a function can take a metafunction class
as argument. The classic example for such functions is `twice`, a function
taking a metafunction class and an argument, calling the metafunction class with
the argument and calling it again with the result coming from the first call.
For example `twice<addn<mpl::int_<1>>, mpl::int_<11>>` would evaluate the
following:

```cpp
mpl::apply_wrap1<
  addn<mpl::int_<1>>::type,
  mpl::apply_wrap1<
    addn<mpl::int_<1>>::type,
    mpl::int_<11>
  >::type
>::type
```

This calls `addn<mpl::int_<1>>` with `mpl::int_<11>` first and then with
`apply_wrap1<addn<mpl::int_<1>>::type, mpl::int_<11>>` again. `twice` can be
implemented the following way:

```cpp
MPLLIBS_METAFUNCTION(twice, (F)(A))
((
  mpl::apply_wrap1<
    typename F::type,
    typename mpl::apply_wrap1<
      typename F::type,
      A
    >::type
  >
));
```

It is lazy for its `F` argument (and leaves laziness for `A` on `F`). It calls
`F` with `A` and calls `F` with the result of this. Thus, for example
`twice<addn<mpl::int_<1>>, mpl::int_<11>>::type` returns `mpl::int_<13>`, since
it adds `1` to `11` twice.

### Currying

As template metafunctions built with
[`MPLLIBS_METAFUNCTION`](MPLLIBS_METAFUNCTION.html), template metafunction
classes built with [`lambda`](lambda.html) support currying. For example:

```cpp
mpl::apply_wrap2<add, mpl::int_<1>>::type
```

`add` expects two arguments: the two values to add, but the above expression
called it with only one argument. It returns another metafunction class
expecting the remaining arguments. The body of the [`lambda`](lambda.html) is
evaluated only when all arguments are provided.

Thus the above expression returns another metafunction class expecting one
argument and then it adds `1` to that argument. Thus, by using currying, there
is no need for the `addn` metafunction. For example:

```cpp
twice<mpl::apply_wrap1<add, mpl::int_<1>>, mpl::int_<11>>::type
```

This example calls `twice` with a metafunction class adding `1` to its argument,
but this time this metafunction class was not constructed using `addn`, but by
using currying by calling `add` with only one argument.

Currying makes it possible to think of `add` as the metafunction class version
of `addn`: 

* `addn` is a _metafunction_ taking one argument and returning a metafunction
  class taking another argument and adding the two arguments together.

* `add` is a _metafunction class_ taking one argument and returning a
  metafunction class taking another argument and adding the two arguments
  together.

This commonality makes the development of complex metafunction classes easier.

## Recursive let bindings

[`lambda`](lambda.html) can be used to construct functions inside an
expression and [`let`](let.html) can be used to replace all occurrences of a
variable with some expression. Let's look at how the two could be used together
to define a function for a sub-expression using [`let`](let.html) and
[`lambda`](lambda.html):

```cpp
let_c<
  f, lambda_c<a, b, mpl::plus<a, b>>,
  mpl::apply_wrap2<f, mpl::int_<11>, mpl::int_<2>>
>::type
```

The above expression binds the metafunction class
`lambda_c<a, b, mpl::plus<a, b>>` to the variable `f` in the expression
`mpl::apply_wrap<f, mpl::int_<11>, mpl::int_<2>>`, thus it uses
[`let`](let.html) to define an addition function for that expression.

So far, so good, let's do something more complicated. What about defining a
recursive function using [`let`](let.html) and [`lambda`](lambda.html)?
Let's write a factorial this way:

```cpp
let_c<
  f,
  lambda_c<n,
    if_<
      mpl::equal_to<mpl::int_<0>, n>,
      mpl::int_<1>,
      lazy_times<n, mpl::apply_wrap1<f, lazy_minus<n, mpl::int_<1>>>>
    >
  >,
  mpl::apply_wrap1<f, mpl::int_<3>>
>::type
```

`lazy_minus` and `lazy_times` are lazy wrappers around the arithmetic functions
coming from [Boost.MPL](http://boost.org/libs/mpl). The above
[`let_c`](let_c.html) expression binds a metafunction class to the `f`
variable in an expression calling this metafunction class with the argument
`mpl::int_<3>`. This may seem to be fine for the first time, but there is a
problem here. The [`let_c`](let_c.html) expression binds the metafunction
class to the `f` variable in the expression `mpl::apply_wrap1<f, mpl::int_<3>>`
only, while `f` is used inside [`lambda_c`](lambda_c.html) as well, since the
factorial function is defined as a recursive function.

Since [`let_c`](let_c.html) does not bind `f` inside
[`lambda_c`](lambda_c.html), `f` is a
[free variable](http://en.wikipedia.org/wiki/Lambda_calculus#Free_variables)
of the [`lambda_c`](lambda_c.html) expression. When the above expression is
evaluated and the evaluation reaches the first recursive call,
`mpl::apply_wrap1` complains, that `f` is not a function, but a variable. This
complain may be a bit difficult to read, since it is coming from the compiler
not knowing that it is a template metaprogramming error.

Metamonad provides the [`letrec`](letrec.html) construct for recursive
[`let`](let.html) bindings. The difference between [`let`](let.html) and
[`letrec`](letrec.html) is that [`letrec`](letrec.html) does the binding
in the expression bound to the variable itself as well. Using
[`letrec_c`](letrec_c.html) in the above example binds the metafunction class
to `f` inside the metafunction class as well:

```cpp
letrec_c<
  f,
  lambda_c<n,
    if_<
      mpl::equal_to<mpl::int_<0>, n>,
      mpl::int_<1>,
      times<n, mpl::apply_wrap1<f, minus<n, mpl::int_<1>>>>
    >
  >,
  mpl::apply_wrap1<f, mpl::int_<3>>
>::type
```

This example works, since due to the recursive binding provided by
[`letrec_c`](letrec_c.html) the metafunction class can call itself
recursively to calculate the factorial value.

## Haskell typeclasses in template metaprograms

[Boost.MPL](http://boost.org/libs/mpl) supports polymorphic metafunctions.
For example the addition operation makes sense for a number of different types.
Integers, longs, etc can be added. An `add` metafunction the way
[Boost.MPL](http://boost.org/libs/mpl) supports it can be implemented the
following way:

```cpp
template <class TagA, class TagB>
struct add_impl;

MPLLIBS_LAZY_METAFUNCTION(add, (A)(B))
((mpl::apply_wrap2<add_impl<typename A::tag, typename B::tag>, A, B>));
```

It assumes, that each metaprogramming value has a tag associated with it and
declares a template class, `add_impl` taking two arguments: the tags of the
values to be added. The instances of this template are assumed to be
metafunction classes taking two arguments: the values to be added. A new
overload can be added by specialising `add_impl` for some tag. For example to
specialise it for integers:

```cpp
typedef mpl::int_<0>::tag int_tag;

template <>
struct add_impl<int_tag, int_tag> : tmp_value<add_impl<int_tag, int_tag>>
{
  MPLLIBS_METAFUNCTION(apply, (A)(B))
  ((mpl::int_<A::type::value + B::type::value>));
};
```

This code specialises `add_impl` for the case when both arguments are
`mpl::int_` values and does the addition by unwrapping the arguments, evaluating
the addition and wrapping the arguments again.

To prepare `add` for adding `mpl::long_` values, `add_impl` needs to be
specialised again:

```cpp
typedef mpl::long_<0>::tag long_tag;

template <>
struct add_impl<long_tag, long_tag> : tmp_value<add_impl<long_tag, long_tag>>
{
  MPLLIBS_METAFUNCTION(apply, (A)(B))
  ((mpl::long_<A::type::value + B::type::value>));
};
```

This specialisation works the same way as the previous one, but it wraps the
results as `mpl::long_` values.

How can a subtraction operation be implemented? By writing another overloadable
metafunction for it:

```cpp
template <class TagA, class TagB>
struct sub_impl;

MPLLIBS_LAZY_METAFUNCTION(sub, (A)(B))
((mpl::apply_wrap2<sub_impl<typename A::tag, typename B::tag>, A, B>));
```

This code declares the `sub_impl` template class, that can be specialised later
and it defines the `sub` metafunction instantiating and calling `sub_impl` with
the tags of the arguments. Multiplication and division can be implemented in a
similar way.

Let's introduce the concept of a _number_ and assume, that numbers can be added,
subtracted, multiplied and divided. Thus, when something is a number, then all
of these metafunctions are overloaded to handle it. How is the connection
between these metafunctions implemented? It lives in the documentation,
describing what _being a number_ means. What happens when someone introduces a
new type, for example `float_<...>` and forgets to overload `add` for `float_`?
When someone uses `float_` values assuming that they are numbers and tries
calling `add` for `float_` values, he gets an error message saying that `add` is
not overloaded for `float_` values (he actually gets an error message
complaining about the missing definition of `add_impl<float_tag, float_tag>`,
but it is easy to learn what it means). What is missing there, is: _the code
assumed that_
`float_`
_is a number, but it isn't_.

There is a way of adding this information to the error message by using
_traits_. The operations of the _number_ concept can be collected into a trait:

```cpp
template <class Tag>
struct number;
```

This template class, when instantiated with a tag defines _all_ operations
required by the number concept. Declaring this template means: there is a
concept called `number`. Unfortunately the way it can specify the list of
required operations is listing them in the documentation or a comment:

```cpp
template <class Tag>
struct number;
```

The above code defines the `number` concept and lists the required operations.
To say that `mpl::int_` _values are numbers_, this template class needs to be
specialised:

```cpp
template <>
struct number<int_tag> : tmp_value<number<int_tag>>
{
  struct add : tmp_value<add>
  {
    MPLLIBS_LAZY_METAFUNCTION(apply, (A)(B)) ((int_<A::value + B::value>));
  };

  struct sub : tmp_value<sub>
  {
    MPLLIBS_LAZY_METAFUNCTION(apply, (A)(B)) ((int_<A::value + B::value>));
  };

  struct mul : tmp_value<mul>
  {
    MPLLIBS_LAZY_METAFUNCTION(apply, (A)(B)) ((int_<A::value * B::value>));
  };

  struct div : tmp_value<div>
  {
    MPLLIBS_LAZY_METAFUNCTION(apply, (A)(B)) ((int_<A::value / B::value>));
  };
};
```

This code implements the four operations for `mpl::int_` values. To add
`mpl::int_<2>` to `mpl::int_<11>` using this solution, one has to write

```cpp
mpl::apply_wrap2<number<int_tag>::add, int_<11>, int_<2>>::type
```

This code calls the `add` operation of the `number` concept for the two values
to add. As this approach is inspired by typeclasses in Haskell, concepts like
`number` are called _typeclasses_. Following the Haskell terminology, when the
`number` template class is specialised for `mpl::int_` values, `mpl::int_`
values are _instances_ of the `number` typeclass. To avoid having to use
`mpl::apply_wrap` all the time, these operations can be wrapped by helper
metafunctions:

```cpp
MPLLIBS_LAZY_METAFUNCTION(add, (Tag)(A)(B))
((mpl::apply_wrap2<typename number<Tag>::add, A, B>));
```

This `add` function makes calling the `add` operation of the `number` typeclass
easier, since it can be used the following way:

```cpp
add<int_tag, int_<11>, int_<2>>::type
```

This solution passes the tag to use and the arguments of the `add` operation to
the `add` wrapper metafunction. Instead of expecting it as an extra argument,
the `add` wrapper metafunction could use the `::tag` of one of its arguments as
well:

```cpp
MPLLIBS_LAZY_METAFUNCTION(add_, (A)(B))
((mpl::apply_wrap2<typename number<typename A::tag>::add, A, B>));
```

This version of the `add` wrapper metafunction takes only two arguments: the
values to add and uses the `::tag` of the first argument to choose the right
instance of the `number` typeclass.

While not having to pass the tag as an extra argument to `add_` is more
convenient, the ability to be able to explicitly specify it adds extra
flexibility to this solution which typeclasses provided by Metamonad
([`monad`](monad.html), [`monoid`](monoid.html)) can take advantage of.

### Default implementation of typeclass operations

Let's implement an `ord` typeclass for values that can be compared:

```cpp
template <class Tag>
struct ord;
```

This typeclass expects one operation - more can be added later. Let's make
`mpl::int_` values an instance of this typeclass:

```cpp
template <>
struct ord<int_tag>
{
  struct less : tmp_value<less>
  {
    MPLLIBS_LAZY_METAFUNCTION(apply, (A)(B))
    ((mpl::bool_<(A::value < B::value)>));
  };
};
```

The above code specialises `ord` for `int_tag` and implements the comparison
operation by unwrapping the values, comparing them and wrapping the result
again. Since the result of a comparison is a logical value, the result is
wrapped with `mpl::bool_`.

Now let's make `mpl::long_` values an instance of `ord` as well:

```cpp
template <>
struct ord<long_tag>
{
  struct less : tmp_value<less>
  {
    MPLLIBS_LAZY_METAFUNCTION(apply, (A)(B))
    ((mpl::bool_<(A::value < B::value)>));
  };
};
```

The implementation of the `less` operation for `mpl::long_` values is the same
as it was for `mpl::int_` values. And for a number of other types it is the
same as well. Not for all of them, for example for `mpl::string` values it
would be different. But the above implementation would be a reasonable default
implementation, that could be overridden for instances that can not use it.

The default implementation can be provided by another template class,
`ord_defaults`:

```cpp
template <class Tag>
struct ord_defaults
{
  struct less : tmp_value<less>
  {
    MPLLIBS_LAZY_METAFUNCTION(apply, (A)(B))
    ((mpl::bool_<(A::value < B::value)>));
  };
};
```

This template class defines the default `less` operation. It takes the tag as a
template argument in case the default implementation of an operation would rely
on another operation provided by the `ord` typeclass - this is not the case in
this example.

Implementations of `ord` can make use of the defaults:

```cpp
template <>
struct ord<int_tag> : ord_defaults<int_tag> {};

template <>
struct ord<long_tag> : ord_defaults<long_tag> {};
```

Both instances of `ord` inherit form `ord_defaults` and inherit the default
implementation of the operations. The above code may suggest that having the
following implementation may be a good idea:

```cpp
template <class Tag>
struct ord : ord_defaults<Tag> {};
```

This code inherits from `ord_defaults` for all `Tag` and the specialisations for
`int_tag` and `long_tag` are not needed, since they are already handled by this
code. What happens, when someone tries comparing two `mpl::vector_c` values
using `ord`?

```cpp
mpl::apply_wrap2<
  ord<vector_tag>::less,
  vector_c<int, 1, 2>,
  vector_c<int, 3>
>::type
```

As `mpl::vector_c` values are not instances of the `ord` typeclass, they can not
be compared, thus it emits an error message. Will the error message say:
_vectors can not be compared_? No, it won't. It will complain about that an
`mpl::vector_c` has no `::value`. However, by defining `ord` the original way
makes the error message better:

```cpp
template <class Tag>
struct ord;

template <>
struct ord<int_tag> : ord_defaults<int_tag> {};

template <>
struct ord<long_tag> : ord_defaults<long_tag> {};
```

Trying to compare vectors with `ord` would generate an error message complaining
about that `ord<vector_tag>` has no definition, which means that vector values
are not instances of `ord`, thus they can not be compared. As the
metaprogramming libraries have no control over the error messages the compiler
displays, the connection between the error message and its meaning needs to be
learned, but fortunately it always means the same.

To keep the code maintainable typeclasses are always expected to have a
`_defaults` implementation even when it is empty and specialisations of the
trait of the typeclass are always expected to inherit from it.

### Writing generic code using typeclasses

A benefit of using typeclasses is that generic code can be written using them.
For example, let's define a typeclass for values that can be added (no further
arithmetic operations are required this time):

```cpp
template <class Tag>
struct addable;

template <class Tag>
struct addable_defaults {};
```

It expects one operation, `add` which implements adding two values. It has no
default implementation, thus the class `addable_defaults` is empty. A helper
metafunction for calling `add` is useful:

```cpp
MPLLIBS_LAZY_METAFUNCTION(add, (Tag)(A)(B))
((mpl::apply_wrap2<typename addable<Tag>::add, A, B>))
```

This is the convenience wrapper for calling the `add` operation. The `mpl::int_`
values can be made instances of this typeclass:

```cpp
template <>
struct addable<int_tag> : addable_defaults<int_tag>
{
  struct add : tmp_value<add>
  {
    MPLLIBS_LAZY_METAFUNCTION(apply, (A)(B)) ((mpl::int_<A::value + B::value>));
  };
};
```

This is similar to the `add` operation of the earlier `number` typeclass. Since
only addition is required, lists can be made an instance of this typeclass in a
reasonable way:

```cpp
typedef mpl::list<>::tag list_tag;

template <>
struct addable<list_tag> : addable_defaults<list_tag>
{
  typedef
    lambda_c<a, b,
      lazy<
        mpl::insert_range<
          lazy_protect_args<a>,
          mpl::end<lazy_protect_args<a>>,
          lazy_protect_args<b>
        >
      >
    >
    add;
};
```

This implementation appends the two lists together using `mpl::insert_range`.
Thus, evaluating:

```cpp
add<list_tag, mpl::list_c<int, 1, 2>, mpl::list_c<int, 3, 4>>::type
```

gives `mpl::list_c<int, 1, 2, 3, 4>` as its result.

When the values of a type can be added, they can be multiplied by non-negative
integer values as well. For any `x` value, `3 * x` can be implemented as
`x ` x ` x`. A metafunction can be written that accepts any `addable` value and
a non-negative multiplier and does the multiplication:

```cpp
MPLLIBS_LAZY_METAFUNCTION(mult, (T)(N))
((
  if_<
    mpl::equal_to<mpl::int_<1>, N>,
    T,
    add<typename T::tag, mult<T, mpl::minus<N, int_<1>>>, T>
  >
));
```

This metafunction takes `T`, the value to be multiplied and `N`, the multiplier.
It uses a recursive implementation to call `add` `N` times. And as it uses the
`addable` typeclass, it works for `mpl::int_` and `mpl::list_c` values:

* `mult<mpl::int_<3>, mpl::int_<4>>::type` evaluates to `mpl::int_<12>`
* `mult<mpl::vector_c<int, 1>, mpl::int_<4>>::type` evaluates to
  `mpl::vector_c<int, 1, 1, 1, 1>`.

What happens when someone tries multiplying `mpl::void_` values, that can not be
added? When `mult` tries calling the `add` operation of `addable`, the compiler
emits an error about that `mpl::void_` values are not addable.

## Monoids

[Monoids](http://en.wikipedia.org/wiki/Monoid) come from abstract algebra,
they are a combination of a set of values and a binary operation on them. To be
a monoid, the binary operation has to be _associative_ and there has to be an
_identity value_.

For example the `mpl::int_<...>` values and the `mpl::plus<..., ...>` operation
form a monoid. Addition - implemented by `mpl::plus` - is associative and the
identity value is `mpl::int_<0>`, since `mpl::plus<x, mpl::int_<0>>` and
`mpl::plus<mpl::int_<0>, x>` both return `x` for any `x` number.

Another example is `mpl::int_<...>` values and the `mpl::times<..., ...>`
operation, they also form a monoid. Multiplication - implemented by `mpl::times`
- is associative and the identity value is `mpl::int_<1>`.

The above examples show that the same set of values with different operations
can form different monoids - the identity value depends on the operation.

Not only numbers can form monoids. Lists together with list concatenation form
a monoid as well. The identity value in this case is the empty list.

Metamonad provides the [`monoid`](monoid.html) typeclass for representing
monoids. Unfortunately, the set of values can only be specified informally by
the documentation. However, this makes the definition of these sets more
flexible than it is in Haskell, since it is not limited by the type system. The
binary operation of the monoid is an operation expected by the typeclass.  It is
called `mappend`. The typeclass also expects a nullary metafunction, `mempty`
returning the identity value.

In Haskell a type can be an instance of a typeclass only once, but as the
examples at the beginning of this chapter show the same set of values can be
part of multiple typeclasses when they are combined with different operations.
Since the typeclass implementation of Metamonad is based on tags instead of
types and this is not deduced from the values but provided as a template
argument when using the operations of the typeclass, in Metamonad a type may be
part of different monoids depending on which tags are used.

## Monads

Monads play a central role in the Haskell language. Based on the commonalities
of Haskell and template metaprogramming, Metamonad brings monads into template
metaprogramming. A monad is a structure for abstracting computations. It
consists of the following:

* A `tag`, identifying the monad itself
* A set of metaprogramming values
* Two basic operations:
    * `return_<...>` taking some value and returning an element of the set
    * `bind<..., ...>` taking an element of the set and a function, taking some
      value and returning an element of the set. `bind` returns an element of
      the set.
* Two further operations:
    * `bind_` taking two monadic values and returning a new one. It has a
      default implementation that calls `bind` with the first value and a
      metafunction class that always returns the second argument of `bind_`.
    * `fail` taking a value and returning a monadic value that breaks the
      execution of the monad. It has a default implementation that throws a
      compile-time exception. More on them later.

In Haskell the set of values is identified by the type system, in template
metaprogramming it is identified by an informal description in the
documentation.

Similarly to Haskell, Metamonad provides the [`monad`](monad.html) typeclass
for implementing monads. It requires `return_` and `bind` to be implemented as
metafunction classes.

Metamonad uses `tags` to identify monads themselves, but not to identify the set
of values. `tags` are used as a replacement of Haskell's value constructors and
pattern matching. For example Haskell's `Maybe` is implemented the following
way to support metafunction overloading:

```cpp
struct nothing_tag;
struct just_tag;

struct nothing { typedef nothing_tag tag; };

template <class T>
struct just { typedef just_tag tag; };
```

The `Maybe` monad consists of the `nothing` and `just` values, which have
different `tags`. Because of this, `tags` cannot be used to identify the set of
values belonging to a monad.

Metamonad provides metafunctions for the operations implemented by the
typeclass to hide the typeclass itself behind monads. These functions take the
`tag` of the monad as their first argument and call the same operation of the
typeclass:

```cpp
template <class MonadTag, class SomeValue>
struct return_;

template <
  class MonadTag,
  class SomeValue,
  class FunctionReturningElementOfSet
>
struct bind;
```

Haskell has semantic expectations for monads that are documented but can not be
verified by the compiler. The C++ template metaprogramming equivalent of these
expectations are the following.

* left identity:
    `monad<Tag>::bind<monad<Tag>::return_<X>, F>` is equivalent to
    `mpl::apply<F, X>`.
* right identity:
    `monad<Tag>::bind<M, monad<Tag>::return_>` is equivalent to `M`.
* associativity:
    `monad<Tag>::bind<M, lambda<x, monad<Tag>::bind<mpl::apply<F, x>, G>>>`
    is equivalent to `monad<Tag>::bind<monad<Tag>::bind<M, F>, G>`.
    [`lambda`](lambda.html) is Metamonad's lambda expression implementation.

Similarly to Haskell, Metamonad cannot verify these expectations. It is the
responsibility of the monad's author to satisfy these expectations.

### do notation

Haskell provides syntactic sugar for monads, called _do notation_. In Haskell,
a _do block_ is associated with a monad and contains a number of monadic
function calls and value bindings. Here is an example do block:

```cpp
do
  r <- may_fail1 13
  may_fail2 r
```

This evaluates `may_fail1 13`, binds `r` to its result and evaluates
`may_fail2 r`. Metamonad implements a similar notation for writing monadic
template metaprograms. A do block using Metamonad looks like the following:

```cpp
do_<monad_tag,
  step1,
  step2,

  // ...

  stepn
>
```

`monad_tag` is the `tag` identifying the monad. This has to be passed to the
`return_` and `bind` functions. `do_<monad_tag>` is a metafunction class, taking
the steps of the do block as arguments. The steps have to be syntaxes and of
course [`do_c`](do_c.html) is also provided. A step is either a nullary
metafunction returning a monadic value or a binding of an expression to a name.
A binding is expressed by the following structure:

```cpp
set<name, step>
```

where `set` is a template class. `step` is a nullary metafunction returning a
monadic value. `name` is the name of a class, the binding binds the result of
`step` to this name. The bound name can be used in the steps of the do block
following the binding.

Using `return_` in do blocks directly is not convenient since the `tag` of the
monad has to be passed to the `return_` metafunction explicitly, thus every time
`return_` is used, the `tag` of the monad needs to be repeated. For example:

```cpp
do_c<monad_tag,
  step1,
  step2,
  set<some_name, return_<monad_tag, some_value>>,

  // ...

  stepn
>
```

To make using `return_` in do blocks simpler, Metamonad provides a template
class, `do_return`, that can be used in do blocks:

```cpp
do_c<monad_tag,
  step1,
  step2,
  set<some_name, do_return<some_value>>,

  // ...

  stepn
>
```

A `do_return<X>` instance of this template class gets automatically translated
to `return_<Tag, X>` inside a do block with `Tag` as tag;

## Error handling in template metaprograms

A number of tools have been presented for error handling in template
metaprograms. The easiest way is to emit a compilation error and break the
compilation process. When this happens, no wrapper code prepared for handling
error situations can recover from this.

One can use [`maybe`](maybe.html) to be able to report that there was a
failure. In this case returning `nothing` indicates that there was an error and
returning the result wrapped by `just` indicates success. The problem with this
approach is that no further details can be provided - the only information the
caller gets is that something went wrong and it is his task to guess what.

To overcome the limitation of [`maybe`](maybe.html),
[`either`](either.html) can report error details as well. A function may
return the result of the computation wrapped by `right` when things went fine
and some template metaprogramming value wrapped by `left` when something went
wrong. The caller can interpret the value wrapped by `left` to get further
information about the problem. This works, but it still changes the interface of
the function. Something as simple as `safe_divides<mpl::int_<26>, mpl::int_<2>>`
returns `right<mpl::int_<13>>` and the caller has to unwrap it later.

Metamonad provides another approach for error reporting. When the computation is
successful, the result can be returned as it is without further wrappers like
`just` or `right`. However, when something goes wrong, the function can return
some metaprogramming value wrapped by [`exception`](exception.html). The
value itself can provide further details about the problem that happened.

Using [`exception`](exception.html), the `safe_divides` function can be
implemented the following way:

```cpp
struct division_by_zero : tmp_value<division_by_zero> {};

MPLLIBS_LAZY_METAFUNCTION(safe_divides, (A)(B))
((
  if_<
    mpl::equal_to<mpl::int_<0>, B>,
    exception<division_by_zero>,
    mpl::divides<A, B>
  >
));
```

This code defines the template metaprogramming value `division_by_zero` to
describe the problem of dividing by zero. `safe_divides` checks if `B` is zero.
When it is, it returns `division_by_zero` wrapped by
[`exception`](exception.html), otherwise it returns the result of the
division.

A function taking two numbers, dividing them when possible and returning `1`
when they can not be divided can be implemented the following way:

```cpp
MPLLIBS_METAFUNCTION(f, (A)(B))
((
  eval_case< safe_divides<A, B>,
    matches_c<exception<_>, mpl::int_<1>>,
    matches_c<r, r>
  >
));
```

This code calls `safe_divides` and does pattern matching on its result. When it
matches the pattern `exception<_>`, it returns `mpl::int_<1>`, otherwise it
returns the result of `safe_divides`, since everything matches the pattern `r`.

This can be done, because template metaprogramming is loosely typed and no
type-checker complains about the type of the return value of `safe_divides`.

### Catching exceptions

[`case_`](case_.html) and [`eval_case`](eval_case.html) can be used to
detect and handle exceptions, however, they are generic tools. Metamonad
provides further tools for catching exceptions. The above example can be
implemented using [`catch_all`](catch_all.html) as well:

```cpp
MPLLIBS_METAFUNCTION(f, (A)(B))
((catch_all<safe_divides<A, B>, lambda_c<_, mpl::int_<1>>>));
```

This code evaluates `safe_divides` and if it returned an
[`exception`](exception.html), the lambda expression is called and the value
wrapped by [`exception`](exception.html) is passed to it as argument.
Otherwise the result is returned. This can be used to catch all exceptions that
may be returned by an expression.

When only a subset of the exceptions should be caught,
[`catch_just`](catch_just.html) can be used. It takes a predicate that
decides if the exception should be handled or it should be returned unchanged.

```cpp
MPLLIBS_METAFUNCTION(f, (A)(B))
((
  catch_just<
    safe_divides<A, B>,
    lambda_c<e, boost::is_same<division_by_zero, e>>,
    lambda_c<_, mpl::int_<1>>
  >
));
```

This code catches only `division_by_zero` exceptions. For the rest of the
exceptions the predicate, which is the second argument of
[`catch_just`](catch_just.html), returns false and in those cases the
handler, the third argument of [`catch_just`](catch_just.html) is not called.

### Exception propagation

Let's assume, that two metafunctions, `may_throw1` and `may_throw2` are given.
Both of them takes a number as argument and either returns a number or an
exception. Let's try to evaluate the following expression:

```cpp
mpl::plus<
  may_throw1<mpl::int_<11>>::type,
  may_throw2<mpl::int_<13>>::type
>
```

The above expression adds the result of `may_throw1` and `may_throw2` together
and works fine as long as they don't return an exception. But when any of them
returns an exception, `mpl::plus` breaks the compilation, since exceptions can
not be added. Ideally, `mpl::plus` should forward the exception to its caller by
returning it. However, `mpl::plus` among a large number of metafunctions is not
prepared for forwarding exceptions. Thus, the user of `mpl::plus` has to make
sure, that exceptions coming from sub-expressions are handled properly. The
above example can be rewritten to prepare for every possible exception:

```cpp
eval_case< may_throw1<mpl::int_<11>>,
  matches_c<exception<e>, exception<e>>,
  matches_c<a,
    eval_case< may_throw2<mpl::int_<13>>,
      matches_c<exception<e>, exception<e>>,
      matches_c<b, mpl::plus<a, b>>
    >
  >
>
```

It evaluates `may_throw1<mpl::int_<11>>` first, checks if it has returned an
exception. If so, the exception is propagated, otherwise
`may_throw3<mpl::int_<13>>` is evaluated and if it has returned an error, it is
propagated. When none of them has returned an error, `mpl::plus` is called.

To propagate exceptions properly, every function call that may return an
exception has to be individually verified - and when one looks at the expression
that is prepared for exception propagation, it takes time to figure out what the
code is really doing, exception propagation adds too much syntactic noise.

Metamonad defines the [exception monad](exception_monad.html) for handling
this situation. The monadic values are errors wrapped by
[`exception`](exception.html) and values not wrapped by
[`exception`](exception.html), thus, basically every template metaprogramming
value is a monadic value. The `bind` operation checks if its first argument is
an exception. If yes, it returns that exception otherwise it evaluates the
action, the second argument of `bind`. `return` is the identity function.

Using the exception monad and the the do notation, the above expression can be
simplified:

```cpp
do_c<exception_tag,
  set<a, may_throw1<mpl::int_<11>>>,
  set<b, may_throw2<mpl::int_<13>>>,
  mpl::plus<a, b>
>
```

It makes use of the do notation to simplify error propagation. The arguments of
`mpl::plus` are stored in temporary variables, `a` and `b` and `mpl::plus` is
called at the last step only. Due to the way do blocks are evaluated, when any
of the arguments evaluates to an [`exception`](exception.html), the execution
of the do block gets stopped and the exception is returned.

This way of writing such expressions is easier and has less syntactic noise,
however, it is still not as readable as the original form. Metamonad provides
another tool, [`make_monadic`](make_monadic.html) for transforming the
original expression into this form.

```cpp
make_monadic<
  exception_tag,
  mpl::plus<
    may_throw1<mpl::int_<11>>::type,
    may_throw2<mpl::int_<13>>::type
  >
>
```

The above code automatically transforms the original expression, which is the
second argument of [`make_monadic`](make_monadic.html) into the form using
the do block, thus it adds exception propagation to it. Since the monad to use
is an argument of this function, it is not limited to the exception monad only.

### Try blocks

Let's add exception handling to the above example. When evaluating the
expression returns an exception, some special value should be returned - based
on the type of the error. It can be done using the tools presented so far:

```cpp
catch_all<
  catch_just<
    make_monadic<
      exception_tag,
      mpl::plus<
        may_throw1<mpl::int_<11>>::type,
        may_throw2<mpl::int_<13>>::type
      >
    >,
    lambda_c<e, boost::is_same<e, division_by_zero>>,
    lambda_c<_, mpl::int_<1>>
  >,
  lambda_c<_, mpl::int_<0>>
>
```

The above expression may look overly complicated for the first time, even though
it does simple things only. It turns the original expression into a monadic one
by using [`make_monadic`](make_monadic.html) and checks if it has returned a
`division_by_zero` [`exception`](exception.html) by using
[`catch_just`](catch_just.html). In that case, it returns `mpl::int_<1>`,
otherwise it checks if the expression has returned any other exception by using
[`catch_all`](catch_all.html). If there was another exception, it returns
`mpl::int_<0>`, otherwise it returns the result of the original expression.

To reduce the amount of syntactic noise and make the above code more compact,
Metamonad provides [`try_`](try_.html) blocks:

```cpp
try_<
  syntax<
    mpl::plus<
      may_throw1<mpl::int_<11>>::type,
      may_throw2<mpl::int_<13>>::type
    >
  >,
  catch_<e, syntax<boost::is_same<e, division_by_zero>>, syntax<mpl::int_<1>>>,
  catch_<e, syntax<mpl::true_>, syntax<mpl::int_<0>>>
>
```

[`try_`](try_.html) takes a syntax as its first argument and adds exception
propagation to it using [`make_monadic`](make_monadic.html). When no further
arguments are provided, [`try_`](try_.html) returns the result of evaluating
this, thus it can be used as a short form of `make_monadic<exception_tag, ...>`.
However, `catch_` arguments can be provided, that can handle any exception
returned by the expression. The first argument of such a `catch_` is a variable,
which will reference the exception in the rest of the `catch_` block. The second
argument is a syntax that should evaluate to a boolean value. It is used to
decide if this `catch_` block should handle the exception. When it evaluates to
true, the third argument of `catch_`, which is also a syntax is evaluated and
its result is returned by [`try_`](try_.html).

The `catch_` blocks are checked in order and the first one that handles the
exception is used - the remaining blocks are not checked. Even, when the handler
returns an exception, it is not handled by any of the remaining `catch_` blocks.

The `_c` versions of [`try_`](try_.html) and `catch_` are also provided and
using them helps making the code more compact:

```cpp
try_c<
  mpl::plus<
    may_throw1<mpl::int_<11>>::type,
    may_throw2<mpl::int_<13>>::type
  >,
  catch_c<e, boost::is_same<e, division_by_zero>, mpl::int_<1>>,
  catch_c<e, mpl::true_, mpl::int_<0>>
>
```

This code does the same as the previous one, however by using
[`try_c`](try_c.html) and `catch_c` the [`syntax`](syntax.html) templates
could be removed.

<p class="copyright">
Copyright Abel Sinkovics (abel at elte dot hu) 2011.
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
[http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt)
</p>

[[up]](index.html)


