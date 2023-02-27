#include "helpers.h"

long get_file_size(std::string filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

bool check_multiplication_overflow(int a, int b) {
    int res = a * b;
    if (a != 0 && res / a != b) {
        return true;
    }
    return false;
}

// function to compute x^n
int get_power(int x, int n)
{
    int res = 1;

    // Check till the number becomes zero
    while (n > 0) {
        if (check_multiplication_overflow(res, x)) {
            return 0;
        }

        // if n is odd, multiply x with result
        if (n % 2 == 1) {
            res = res * x;
        }

        n = n / 2;

        // Change x to x^2
        x = x * x;
    }
    return res;
}

// function that checks if x = y^n, y integer
bool is_perfect_power(int x, int n) {
    if (x == 0) {
        return false;
    }
    if (x == 1) {
        return true;
    }

    int low = 2, high = sqrt(x), guess, power_value;

    while (low <= high) {
        guess = low + (high - low) / 2;
        power_value = get_power(guess, n);

        // in case of overflow, decrease high
        if (power_value == 0 || power_value > x) {
            // search in the lower half
            high = guess - 1;
        } else {
            if (power_value < x) {
                // search in the upper half
                low = guess + 1;
            } else {
                return true;
            }
        }
    }

    return false;
}
