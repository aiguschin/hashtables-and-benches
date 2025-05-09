#include "benches.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <sparsehash/dense_hash_set>
#include <sparsehash/sparse_hash_set>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "hopscotch_shadow.h"
#include "hopscotch_bitmaps.h"

using namespace std::chrono_literals;

using std::cout;
using std::endl;
using std::pair;
using std::unordered_set;
using std::vector;

using google::dense_hash_set;
using google::sparse_hash_set;

/*void hash_insert(unordered_set<int>& set, int k) { set.insert(k); }
//...

template<typename T>
double test_insert(const vector<int>& to_insert, int num_tries) {
    auto sset_begin = std::chrono::steady_clock::now();
    for (int i = 0; i < num_tries; ++i) {
        T hbset_to_bench{};
        for (int v : to_insert) {
            hbset_to_bench.add(v);
        }
    }
    auto sset_end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::milli>(sset_end - sset_begin);
}
// cout << test_insert<std::unordered_set<int>>(to_insert, num_tries);
// cout << test_insert<std::unordered_set<int>>(to_insert, num_tries);*/

void bench_single_size_and_op(int size, OpType type, int num_tries) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<std::mt19937::result_type> distrib(0, 1'000'000'000);
    unordered_set<int> elems{};
    while (static_cast<int>(elems.size()) < size) {
        elems.insert(distrib(rng));
    }
    vector<int> to_insert{};
    for (int v : elems) {
        to_insert.push_back(v);
    }
    std::ranges::shuffle(to_insert, rng);

    if (type == OpType::Insert) {
        auto uset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries; ++i) {
            unordered_set<int> uset_to_bench{};
            for (int v : to_insert) {
                uset_to_bench.insert(v);
            }
        }
        auto uset_end = std::chrono::steady_clock::now();

        auto sset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries; ++i) {
            sparse_hash_set<int> sset_to_bench{};
            sset_to_bench.set_deleted_key(-2);
            for (int v : to_insert) {
                sset_to_bench.insert(v);
            }
        }
        auto sset_end = std::chrono::steady_clock::now();

        auto dset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries; ++i) {
            dense_hash_set<int> dset_to_bench{};
            dset_to_bench.set_deleted_key(-2);
            dset_to_bench.set_empty_key(-1);
            for (int v : to_insert) {
                dset_to_bench.insert(v);
            }
        }
        auto dset_end = std::chrono::steady_clock::now();

        auto hset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries; ++i) {
            HopscotchShadow<int> hset_to_bench{};
            for (int v : to_insert) {
                hset_to_bench.insert(v);
            }
        }
        auto hset_end = std::chrono::steady_clock::now();

        auto hbset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries; ++i) {
            HopscotchHashSet<int> hbset_to_bench{};
            for (int v : to_insert) {
                hbset_to_bench.add(v);
            }
        }
        auto hbset_end = std::chrono::steady_clock::now();

        std::chrono::duration<double, std::milli> uset_time =
            uset_end - uset_begin;
        std::chrono::duration<double, std::milli> sset_time =
            sset_end - sset_begin;
        std::chrono::duration<double, std::milli> dset_time =
            dset_end - dset_begin;
        std::chrono::duration<double, std::milli> hset_time =
            hset_end - hset_begin;
        std::chrono::duration<double, std::milli> hbset_time =
            hbset_end - hbset_begin;

        cout << size << " inserts:" << endl;
        cout << "Unordered_set: " << uset_time << endl;
        cout << "Sparse_hash_set: " << sset_time << endl;
        cout << "Dense_hash_set: " << dset_time << endl;
        cout << "Hopscotch shadow: " << hset_time << endl;
        cout << "Hopscotch bitmaps: " << hbset_time << endl;
        cout << "---------------" << endl;
    } else if (type == OpType::Remove) {
        vector<unordered_set<int>> usets(num_tries);
        vector<sparse_hash_set<int>> ssets(num_tries);
        vector<dense_hash_set<int>> dsets(num_tries);
        vector<HopscotchShadow<int>> hsets(num_tries);
        vector<HopscotchHashSet<int>> hbsets(num_tries);
        for (auto& uset_to_bench : usets) {
            for (int v : to_insert) {
                uset_to_bench.insert(v);
            }
        }
        for (auto& sset_to_bench : ssets) {
            sset_to_bench.set_deleted_key(-2);
            for (int v : to_insert) {
                sset_to_bench.insert(v);
            }
        }
        for (auto& dset_to_bench : dsets) {
            dset_to_bench.set_deleted_key(-2);
            dset_to_bench.set_empty_key(-1);
            for (int v : to_insert) {
                dset_to_bench.insert(v);
            }
        }
        for (auto& hset_to_bench : hsets) {
            for (int v : to_insert) {
                hset_to_bench.insert(v);
            }
        }
        for (auto& hbset_to_bench : hbsets) {
            for (int v : to_insert) {
                hbset_to_bench.add(v);
            }
        }
        std::ranges::shuffle(to_insert, rng);

        auto uset_begin = std::chrono::steady_clock::now();
        for (auto& uset_to_bench : usets) {
            for (int v : to_insert) {
                uset_to_bench.erase(v);
            }
        }
        auto uset_end = std::chrono::steady_clock::now();

        auto sset_begin = std::chrono::steady_clock::now();
        for (auto& sset_to_bench : ssets) {
            for (int v : to_insert) {
                sset_to_bench.erase(v);
            }
        }
        auto sset_end = std::chrono::steady_clock::now();

        auto dset_begin = std::chrono::steady_clock::now();
        for (auto& dset_to_bench : dsets) {
            for (int v : to_insert) {
                dset_to_bench.erase(v);
            }
        }
        auto dset_end = std::chrono::steady_clock::now();

        auto hset_begin = std::chrono::steady_clock::now();
        for (auto& hset_to_bench : hsets) {
            for (int v : to_insert) {
                hset_to_bench.erase(v);
            }
        }
        auto hset_end = std::chrono::steady_clock::now();

        auto hbset_begin = std::chrono::steady_clock::now();
        for (auto& hbset_to_bench : hbsets) {
            for (int v : to_insert) {
                hbset_to_bench.remove(v);
            }
        }
        auto hbset_end = std::chrono::steady_clock::now();

        std::chrono::duration<double, std::milli> uset_time =
            uset_end - uset_begin;
        std::chrono::duration<double, std::milli> sset_time =
            sset_end - sset_begin;
        std::chrono::duration<double, std::milli> dset_time =
            dset_end - dset_begin;
        std::chrono::duration<double, std::milli> hset_time =
            hset_end - hset_begin;
        std::chrono::duration<double, std::milli> hbset_time =
            hbset_end - hbset_begin;

        cout << size << " removes:" << endl;
        cout << "Unordered_set: " << uset_time << endl;
        cout << "Sparse_hash_set: " << sset_time << endl;
        cout << "Dense_hash_set: " << dset_time << endl;
        cout << "Hopscotch shadow: " << hset_time << endl;
        cout << "Hopscotch bitmaps: " << hbset_time << endl;
        cout << "---------------" << endl;
    } else if (type == OpType::TrueContains) {
        unordered_set<int> uset_to_bench{};
        for (int v : to_insert) {
            uset_to_bench.insert(v);
        }
        sparse_hash_set<int> sset_to_bench{};
        sset_to_bench.set_deleted_key(-2);
        for (int v : to_insert) {
            sset_to_bench.insert(v);
        }
        dense_hash_set<int> dset_to_bench{};
        dset_to_bench.set_deleted_key(-2);
        dset_to_bench.set_empty_key(-1);
        for (int v : to_insert) {
            dset_to_bench.insert(v);
        }
        HopscotchShadow<int> hset_to_bench{};
        for (int v : to_insert) {
            hset_to_bench.insert(v);
        }
        HopscotchHashSet<int> hbset_to_bench{};
        for (int v : to_insert) {
            hbset_to_bench.add(v);
        }
        std::ranges::shuffle(to_insert, rng);

        int ucounter = 0;
        auto uset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries * size; ++i) {
            ucounter += uset_to_bench.contains(to_insert[i % size]);
        }
        auto uset_end = std::chrono::steady_clock::now();

        int scounter = 0;
        auto sset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries * size; ++i) {
            scounter += (sset_to_bench.find(to_insert[i % num_tries]) != sset_to_bench.end());
        }
        auto sset_end = std::chrono::steady_clock::now();

        int dcounter = 0;
        auto dset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries * size; ++i) {
            dcounter += (dset_to_bench.find(to_insert[i % num_tries]) != dset_to_bench.end());
        }
        auto dset_end = std::chrono::steady_clock::now();

        int hcounter = 0;
        auto hset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries * size; ++i) {
            hcounter += hset_to_bench.contains(to_insert[i % num_tries]);
        }
        auto hset_end = std::chrono::steady_clock::now();

        int hbcounter = 0;
        auto hbset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries * size; ++i) {
            hbcounter += hbset_to_bench.contains(to_insert[i % num_tries]);
        }
        auto hbset_end = std::chrono::steady_clock::now();

        std::chrono::duration<double, std::milli> uset_time =
            uset_end - uset_begin;
        std::chrono::duration<double, std::milli> sset_time =
            sset_end - sset_begin;
        std::chrono::duration<double, std::milli> dset_time =
            dset_end - dset_begin;
        std::chrono::duration<double, std::milli> hset_time =
            hset_end - hset_begin;
        std::chrono::duration<double, std::milli> hbset_time =
            hbset_end - hbset_begin;

        cout << size << " true contains: " << endl;
        cout << "Unordered_set: " << uset_time << " on load_factor " << uset_to_bench.load_factor() << ", counter = " << ucounter << endl;
        cout << "Sparse_hash_set: " << sset_time << " on load_factor " << sset_to_bench.load_factor() << ", counter = " << scounter  << endl;
        cout << "Dense_hash_set: " << dset_time << " on load_factor " << dset_to_bench.load_factor() << ", counter = " << dcounter  << endl;
        cout << "Hopscotch shadow: " << hset_time << " on load_factor " << hset_to_bench.load_factor() << ", counter = " << hcounter  << endl;
        cout << "Hopscotch bitmaps: " << hbset_time << " on load_factor " << hbset_to_bench.load_factor() << ", counter = " << hbcounter  << endl;
        cout << "---------------" << endl;
    } else if (type == OpType::FalseContains) {
        // generate false guesses
        unordered_set<int> false_elems{};
        while (static_cast<int>(false_elems.size()) < size) {
            int guess = distrib(rng);
            if (!elems.contains(guess)) {
                false_elems.insert(guess);
            }
        }
        vector<int> false_guesses{};
        for (int v : false_elems) {
            false_guesses.push_back(v);
        }
        std::ranges::shuffle(false_guesses, rng);

        unordered_set<int> uset_to_bench{};
        for (int v : to_insert) {
            uset_to_bench.insert(v);
        }
        sparse_hash_set<int> sset_to_bench{};
        sset_to_bench.set_deleted_key(-2);
        for (int v : to_insert) {
            sset_to_bench.insert(v);
        }
        dense_hash_set<int> dset_to_bench{};
        dset_to_bench.set_deleted_key(-2);
        dset_to_bench.set_empty_key(-1);
        for (int v : to_insert) {
            dset_to_bench.insert(v);
        }
        HopscotchShadow<int> hset_to_bench{};
        for (int v : to_insert) {
            hset_to_bench.insert(v);
        }
        HopscotchHashSet<int> hbset_to_bench{};
        for (int v : to_insert) {
            hbset_to_bench.add(v);
        }

        int ucounter = 0;
        auto uset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries * size; ++i) {
            ucounter += uset_to_bench.contains(false_guesses[i % size]);
        }
        auto uset_end = std::chrono::steady_clock::now();

        int scounter = 0;
        auto sset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries * size; ++i) {
            scounter += (sset_to_bench.find(false_guesses[i % num_tries]) != sset_to_bench.end());
        }
        auto sset_end = std::chrono::steady_clock::now();

        int dcounter = 0;
        auto dset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries * size; ++i) {
            dcounter += (dset_to_bench.find(false_guesses[i % num_tries]) != dset_to_bench.end());
        }
        auto dset_end = std::chrono::steady_clock::now();

        int hcounter = 0;
        auto hset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries * size; ++i) {
            hcounter += hset_to_bench.contains(false_guesses[i % num_tries]);
        }
        auto hset_end = std::chrono::steady_clock::now();

        int hbcounter = 0;
        auto hbset_begin = std::chrono::steady_clock::now();
        for (int i = 0; i < num_tries * size; ++i) {
            hbcounter += hbset_to_bench.contains(false_guesses[i % num_tries]);
        }
        auto hbset_end = std::chrono::steady_clock::now();

        std::chrono::duration<double, std::milli> uset_time =
            uset_end - uset_begin;
        std::chrono::duration<double, std::milli> sset_time =
            sset_end - sset_begin;
        std::chrono::duration<double, std::milli> dset_time =
            dset_end - dset_begin;
        std::chrono::duration<double, std::milli> hset_time =
            hset_end - hset_begin;
        std::chrono::duration<double, std::milli> hbset_time =
            hbset_end - hbset_begin;

        cout << size << " false contains: " << endl;
        cout << "Unordered_set: " << uset_time << " on load_factor " << uset_to_bench.load_factor() << ", counter = " << ucounter << endl;
        cout << "Sparse_hash_set: " << sset_time << " on load_factor " << sset_to_bench.load_factor() << ", counter = " << scounter  << endl;
        cout << "Dense_hash_set: " << dset_time << " on load_factor " << dset_to_bench.load_factor() << ", counter = " << dcounter  << endl;
        cout << "Hopscotch shadow: " << hset_time << " on load_factor " << hset_to_bench.load_factor() << ", counter = " << hcounter  << endl;
        cout << "Hopscotch bitmaps: " << hbset_time << " on load_factor " << hbset_to_bench.load_factor() << ", counter = " << hbcounter  << endl;
        cout << "---------------" << endl;
    } else {
        throw std::runtime_error("Unsupported OpType");
    }
}

void bench_single_size(int size, int num_tries) {
    vector<OpType> ops_to_test{OpType::Insert, OpType::Remove, OpType::TrueContains, OpType::FalseContains};
    for (int i = 0; i < static_cast<int>(ops_to_test.size()); ++i) {
        bench_single_size_and_op(size, ops_to_test[i], num_tries);
    }
}

void bench_everything() {
    vector<pair<int, int>> benches{{1'000, 1'000}, {10'000, 100}, {100'000, 20}, {1'000'000, 5}, {10'000'000, 1}};
    for (auto size_and_num_tries : benches) {
        int size = size_and_num_tries.first;
        int num_tries = size_and_num_tries.second;
        bench_single_size(size, num_tries);
        cout << "____________________" << endl;
    }
}
