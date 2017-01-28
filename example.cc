#include "arrr.hpp"


int main(int argc, char* argv[]) {
    const auto size = 10;
    arrr::arithmetic_array<float> x(size);
    arrr::arithmetic_array<float> y(size);

    x = 23.0f; // set all elements of x to 23
    y = 42.0f; // set all elements of y to 42

    y += 3.14159f*x; // the expression is evaluated element-wise

    return 0;
}
