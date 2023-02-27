#ifndef TEMA1_MAP_REDUCE
#define TEMA1_MAP_REDUCE

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <queue>
#include <map>
#include <set>
#include <pthread.h>

// struct to save arguments used by both mappers and reducers
struct common_args {
    pthread_barrier_t *barrier;
    std::vector<std::map<int, std::set<int>>> results;

    common_args(pthread_barrier_t *barrier) {
        this->barrier = barrier;
    }
};

// struct to save arguments used mappers
struct mapper_args {
    int num_reducers;
    std::priority_queue<std::pair<long, std::string>> files;
    pthread_mutex_t *mutex1;
    pthread_mutex_t *mutex2;
    common_args *common;

    mapper_args(int num_reducers,
                std::priority_queue<std::pair<long, std::string>> &files,
                pthread_mutex_t *mutex1,
                pthread_mutex_t *mutex2,
                common_args *common) {
        this->num_reducers = num_reducers;
        this->files = files;
        this->mutex1 = mutex1;
        this->mutex2 = mutex2;
        this->common = common;
    }
};

// struct to save arguments used by reducers
struct reducer_args {
    int exp;
    common_args *common;

    reducer_args(int exp, common_args *common) {
        this->exp = exp;
        this->common = common;
    }
};

// function for mappers
void *mapper_function(void *arg);
// function for reducers
void *reducer_function(void *arg);

#endif // TEMA1_MAP_REDUCE