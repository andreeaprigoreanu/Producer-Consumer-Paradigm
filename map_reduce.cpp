#include "map_reduce.h"
#include "helpers.h"

using namespace std;

void *mapper_function(void *arg) {
  	auto *args = (mapper_args *)arg;
    std::string filename;
    // declare map for partial lists
    map<int, set<int>> partial_lists;

    while (true) {
        // use mutex to ensure that only one thread extracts a file
        // at a given moment
        pthread_mutex_lock(args->mutex1);
        // if there are still unprocessed files, current thread will extract
        // a file from the files heap
        if (!args->files.empty()) {
            std::pair<long, std::string> new_file = args->files.top();
            filename = new_file.second;
            args->files.pop();
        } else {
            pthread_mutex_unlock(args->mutex1);
            break;
        }
        pthread_mutex_unlock(args->mutex1);

        // open file
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Couldn't open file" << endl;
            exit(1);
        }

        // read numbers
        int n;
        file >> n;
        int x;
        for (int i = 0; i < n; i++) {
            file >> x;
            for (int exp = 2; exp <= args->num_reducers + 1; exp++) {
                // check if number x is perfect power of exp
                if (is_perfect_power(x, exp)) {
                    // add number to partial list of exponent exp
                    if (partial_lists.count(exp)) {
                        partial_lists[exp].insert(x);
                    } else {
                        set<int> list;
                        list.insert(x);
                        partial_lists[exp] = list;
                    }
                }
            }
        }

        file.close();
    }

    // add file to partial results vector
    pthread_mutex_lock(args->mutex2);
    args->common->results.push_back(partial_lists);
    pthread_mutex_unlock(args->mutex2);

    // use barrier to ensure that mappers finish processing all files
    // before reducers start
    pthread_barrier_wait( args->common->barrier);

  	pthread_exit(nullptr);
}

void *reducer_function(void *arg) {
    auto *args = (reducer_args *)arg;

    // use barrier to ensure that mappers finish processing all files
    // before reducers start
    pthread_barrier_wait( args->common->barrier);

    int exp = args->exp;
    // set to save perfect powers of exp without duplicates
    set<int> perfect_powers;

    for (auto & result :  args->common->results) {
        // add perfect powers found by a mapper
        perfect_powers.insert(result[exp].begin(), result[exp].end());
    }

    // write result to output file
    int num_powers = perfect_powers.size();
    string out_filename = "out" + to_string(exp) + ".txt";
    ofstream fout(out_filename);
    if (!fout.is_open()) {
        cerr << "Couldn't open file" << endl;
        exit(1);
    }
    fout << num_powers;
    fout.close();

    pthread_exit(nullptr);
}
