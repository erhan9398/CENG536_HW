#pragma once
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define RANDOM_GENERATOR(max, min) (\
(rand() % (max - min + 1)) + min)

typedef struct rand_gen {
    /* data */
    int rangeMin;
    int rangeMax;
    int sizeMin;
    int sizeMax;
}random_generator;

void generate_random_numbers(random_generator specs, int index);