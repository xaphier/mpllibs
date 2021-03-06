# mpllibs::metaparse user manual

## Table of contents

## What is a parser

A parser is a template metafunction that takes the following arguments:

* a suffix of the input text, a [`string`](string.html)
* a structure describing at which position of the entire input text the suffix
  begins at

The function parses a prefix of the input string.
When the parsing is successful, it returns a structure with the tag
[`accept_tag`](accept_tag.html). The following metafunctions can be used to
query this result:

* [`get_result`](get_result.html)
* [`get_remaining`](get_remaining.html)
* [`get_position`](get_position.html)

When there is a parsing error, the parser returns a structure with the tag
[`fail_tag`](fail_tag.html).

### The input of the parsers

Parsers take a [`string`](string.html) as input, which represents a string
for template metaprograms. For example the string `"Hello World!"` can be
defined the following way:

```cpp
string<'H','e','l','l','o',' ','W','o','r','l','d','!'>
```

This syntax makes the input of the parsers difficult to read. Metaparse works
with compilers using C++98, but the input of the parsers has to be defined the
way it is described above.

Based on `constexpr`, a feature provided by C++11, Metaparse provides a macro,
[`MPLLIBS_STRING`](MPLLIBS_STRING.html) for defining strings:

```cpp
MPLLIBS_STRING("Hello World!")
```

This defines a [`string`](string.html) as well, however, it is easier to
read. The maximum length of the string that can be defined this way is limited,
however, this limit is configurable. It is specified by the 
`MPLLIBS_LIMIT_STRING_SIZE` macro.

### Source positions

A source position is described using a compile-time data structure. The
following functions can be used to query it:

* [`get_col`](get_col.html)
* [`get_line`](get_line.html)

The beginning of the input is [`start`](start.html) which requires
`<mpllibs/metaparse/source_position.hpp>` to be included.

### Error handling

An error is described using a compile-time data structure. It contains
information about the source position where the error was detected and some
description about the error. The description is a class with a
`public: static std::string get_value()` method, which returns the error
message. [`debug_parsing_error`](debug_parsing_error.html) can be used to
display the error message. Metaparse provides the
[`MPLLIBS_DEFINE_ERROR`](MPLLIBS_DEFINE_ERROR.html) macro for defining simple
error messages.

### Some examples of simple parsers

* A parser that parses nothing and always succeeds is
  [`return_`](return_.html).
* A parser that always fails is [`fail`](fail.html).
* A parser that parses one character and returns the parsed character as the
  result is [`one_char`](one_char.html).

## Combining parsers

Complex parsers can be built by combining simple parsers. The parser library
contains a number of parser combinators that build new parsers from already
existing ones.

For example
[`accept_when`](accept_when.html)`<Parser, Predicate, RejectErrorMsg>` is a
parser. It uses `Parser` to parse the input. When `Parser` rejects the input,
the combinator returns the error `Parser` failed with. When `Parser` is
successful, the combinator validates the result using `Predicate`. If the
predicate returns true, the combinator accepts the input, otherwise it generates
an error with the message `RejectErrorMsg`.

Having `accept_when`, `one_char` can be used to build parsers that accept
only digit characters, only whitespaces, etc. For example `digit` accepts only
digit characters:

```cpp
typedef
  mpllibs::metaparse::accept_when<
    mpllibs::metaparse::one_char,
    mpllibs::metaparse::util::is_digit,
    mpllibs::metaparse::errors::digit_expected
  >
  digit;
```

### Sequence

The result of a successful parsing is some value and the remaining string that
was not parsed. The remaining string can be processed by another parser. The
parser library provides a parser combinator, [`sequence`](sequence.html),
that takes a number of parsers as arguments and builds a new parser from them
that:

* Parses the input using the first parser
* If parsing succeeds, it parses the remaining string with the second parser
* It continues applying the parsers in order as long as they succeed
* If all of them succeed, it returns the list of results
* If any of the parsers fails, the combinator fails as well and returns the
  error the first failing parser returned with

### Repetition

We have parsers for letters, we should be able to build parsers for words that
are sequences of letters. Words have no fixed length, the end of the word is the
first character that is not a letter, thus the first character where the letter
parser fails. Since this is a common pattern, the parser library contains a
parser combinator for this, [`any`](any.html)`<Parser>`. It uses `Parser`
repeatedly as long as `Parser` succeeds. The result of parsing is a sequence,
the results we got by repeatedly applying `Parser`.

### Grammars

Metaparse provides a way to define grammars in a syntax that resembles EBNF. The
[`grammar`](grammar.html) template can be used to define a grammar. It can be
used the following way:

```cpp
grammar<MPLLIBS_STRING("plus_exp")>
  ::import<MPLLIBS_STRING("int_token"), token<int_>>::type

  ::rule<MPLLIBS_STRING("ws ::= (' ' | '\n' | '\r' | '\t')*")>::type
  ::rule<MPLLIBS_STRING("plus_token ::= '+' ws"), front<_1>>::type
  ::rule<MPLLIBS_STRING("plus_exp ::= int_token (plus_token int_token)*"), plus_action>::type
```

The code above defines a parser from a grammar definition. The start symbol of
the grammar is `plus_exp`. The lines beginning with `::rule` define rules.
Rules optionally have a semantic action, which is a metafunction class that
transforms the result of parsing after the rule has been applied.
Existing parsers can be bound to names and be used in the rules by importing
them. Lines beginning with `::import` bind existing parsers to names.

The result of a grammar definition is a parser which can be given to other
parser combinators or be used directly. Given that grammars can import existing
parsers and build new ones, they are parser combinators as well.

## Parsing based on `constexpr`

Metaparse is based on template metaprogramming, however, C++11 provides
`constexpr`, which can be used for parsing at compile-time as well. While
implementing parsers based on `constexpr` is easier for a C++ developer, since
its syntax resembles the regular syntax of the language, the result of parsing
has to be a `constexpr` value. Parsers based on template metaprogramming can
build types as the result of parsing. These types may be boxed `constexpr`
values but can be metafunction classes, classes with static functions which can
be called at runtime, etc.

When a parser built with Metaparse needs a sub-parser for processing a part of
the input text and generating a `constexpr` value as the result of parsing, one
can implement the sub-parser based on `constexpr` functions. Metaparse
can be integrated with them and lift their results into C++ template
metaprogramming. An example demonstrating this feature can be found among the
examples (`constexpr_parser`). This capability makes it possible to integrate
Metaparse with parsing libraries based on `constexpr`.

## Monadic parsing

`metaparse` provides a parsing monad implementation based on Metamonad's monadic
framework. The overloads of `bind` and `return_` can be loaded by including
`mpllibs/metaparse/parser_monad.hpp`.

Monadic values are parsers. The monadic `return_` operation constructs a
`return_` parser. The arguments of the monadic `bind` are a parser `P` and a
function `F`, taking a value and buliding a parser. `bind` constructs the
following parser:

* It parses the input using `P`.
* When `P` fails, the error is propagated.
* When `P` succeeds, the result is passed to `F` and the remaining input is
  parsed using the parser returned by `F`.

<p class="copyright">
Copyright Abel Sinkovics (abel at elte dot hu) 2011.
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
[http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt)
</p>

[[up]](index.html)


