#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <queue>

#include "map_reduce.h"
#include "helpers.h"

using namespace std;

int main(int argc,char* argv[])
{
    int num_mappers = stoi(string(argv[1]));
    int num_reducers = stoi(string(argv[2]));
    string main_filename(argv[3]);

    // open input file
    ifstream fin(main_filename);
    if (!fin.is_open()) {
        cerr << "Couldn't open file" << endl;
        exit(1);
    }

    // declare max heap to store files according to their size
    priority_queue<pair<long, string>> files;

    // read from file
    int num_files;
    fin >> num_files;
    string file_name;
    for (int i = 0; i < num_files; i++) {
        fin >> file_name;
        long file_size = get_file_size(file_name);

        if (file_size == -1) {
            cerr << "Couldn't get file size" << endl;
            exit(1);
        }
        files.push(make_pair(file_size, file_name));
    }
    // close file
    fin.close();

    vector<pthread_t> threads(num_mappers + num_reducers);
    // initialise arguments for threads
    // std::vector<std::map<int, std::set<int>>> results;
    pthread_mutex_t mutex1, mutex2;
    pthread_mutex_init(&mutex1, nullptr);
    pthread_mutex_init(&mutex2, nullptr);
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, nullptr, num_mappers + num_reducers);
    auto *common = new common_args(&barrier);
    // initialize mappers arguments
    mapper_args mapper_args(num_reducers, files, &mutex1, &mutex2, common);
    // initialise reducers arguments
    vector<reducer_args> reducer_arg;
    for (int i = 0; i < num_reducers; i++) {
        reducer_args arg(i + 2, common);
        reducer_arg.push_back(arg);
    }

    // create mappers and reducers threads
    for (int i = 0; i < num_mappers + num_reducers; i++) {
        if (i < num_mappers) {
            pthread_create(&threads[i], nullptr, mapper_function, &mapper_args);
        } else {
            pthread_create(&threads[i], nullptr, reducer_function, &reducer_arg[i - num_mappers]);
        }
    }

    // join all threads
    for (int i = 0; i < num_mappers + num_reducers; i++) {
        pthread_join(threads[i], nullptr);
    }

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_barrier_destroy(&barrier);

    return 0;
}
