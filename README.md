ARRR
====

A vectorizing expression template array library.

The library currently consists of a single class `arithmetic_array`
in the namespace `arrr`. It should be usable with any fundamental
types but double and float get optimized to sse2 or avx code if
available (use -msse2, -mavx or -march=native on your favorite compiler).
ARRR uses C++11 features and has been tested on gcc 4.8.1, clang 3.3 and
icc 14.

Basic usage:
```c++
// arithmetic arrays are dynamically sized (runtime) but are
// not resizeable once created. They provide most of the typical
// standard container interface (operator[], begin, end, size, data...)
arrr::arithmetic_array<float> x(size);
arrr::arithmetic_array<float> y(size);

x = 23.0f; // set all elements of x to 23
y = 42.0f; // set all elements of y to 42

y += 3.14159f*x; // the expression is evaluted elementwise

// arithmetic arrays can also be statically sized with the second
// template argument (0 means dynamic and is the default value)
arrr::arithmetic_array<float, size> z;

```

ARRR is mostly a shorter and nicer reimplementation of a library called
SALT that was a proof of concept of the employed loop unrolling
technique and is described here: http://arxiv.org/abs/1109.1264
