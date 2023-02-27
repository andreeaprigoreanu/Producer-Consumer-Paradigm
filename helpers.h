//
// Created by Andreea on 19/11/2022.
//

#ifndef APD_HELPERS_H
#define APD_HELPERS_H

#include <iostream>
#include <cmath>
#include <string>
#include <sys/stat.h>

// function that returns file size
long get_file_size(std::string filename);
// functions that check if multiplication overflow occurs
bool check_multiplication_overflow(int a, int b);
// function that computes x^n
int get_power(int x, int n);
// function that checks if x = y^n, y integer
bool is_perfect_power(int x, int n);

#endif //APD_HELPERS_H
