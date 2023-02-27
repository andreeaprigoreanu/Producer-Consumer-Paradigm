#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cmath>
#include <string>
#include <queue>
#include <map>
#include <set>

using namespace std;

long get_file_size(string filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

bool check_multiplication_overflow(int a, int b) {
    int res = a * b;
    if (a != 0 && res / a != b) {
        return 1;
    }
    return 0;
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
            high = guess - 1;
        } else {
            if (power_value < x) {
                low = guess + 1;
            } else {
                return true;
            }
        }
    }

    return false;
}

int main(int argc,char* argv[])
{
//    int x, n;
//
//    cin >> x >> n;
//
//    cout << is_perfect_power(x, n) << endl;

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

    /*
    while (!files.empty()) {
        cout << files.top().first << " " << files.top().second << endl;
        files.pop();
    }
    */
    vector<pthread_t> threads(num_mappers);
    std::vector<std::map<int, std::set<int>>> results;
    // create mappers and reducers threads
    while (!files.empty()) {
        std::string filename;
        // declare map for partial lists
        map<int, set<int>> partial_lists;

        std::pair<long, std::string> new_file = files.top();
        filename = new_file.second;
        files.pop();

        cout << "Am luat " + filename << endl;

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
            for (int exp = 2; exp <= num_reducers + 1; exp++) {
                if (is_perfect_power(x, exp)) {
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

        results.push_back(partial_lists);

    }

    for (auto partial_lists : results) {
        for (auto entry : partial_lists) {
            cout << entry.first << " : ";
            for (auto x : entry.second) {
                cout << x << " ";
            }
            cout << endl;
        }
    }

    return 0;
}
