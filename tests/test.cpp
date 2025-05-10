#include <random>
#include <string>
#include <vector>

#include "hopscotch_bitmaps.h"
#include "hopscotch_shadow.h"

using std::vector;

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Simple insert") {
    HopscotchShadow<int> table{};
    table.set_deleted_key(-1);
    REQUIRE(table.get_size() == 0);
    for (int i = 0; i < 10; ++i) {
        REQUIRE((table.insert(i)).second);
        REQUIRE(static_cast<int>(table.get_size()) == i + 1);
    }
}

TEST_CASE("Insert copy") {
    HopscotchShadow<int> table{};
    table.set_deleted_key(-1);
    pair<uint32_t, bool> res = table.insert(0);
    REQUIRE(res.first == 0);
    REQUIRE(res.second);
    for (int i = 0; i < 1'000; ++i) {
        REQUIRE_FALSE((table.insert(0)).second);
    }
    REQUIRE(table.get_size() == 1);
    REQUIRE(table.get_max_size() == 64);
}

TEST_CASE("Erase") {
    HopscotchShadow<int> table{};
    table.set_deleted_key(-1);
    REQUIRE(table.erase(100500) == 0);
    for (int i = 0; i < 10; ++i) {
        table.insert(i);
    }
    for (int i = 0; i < 10; ++i) {
        REQUIRE(table.erase(i) == 1);
        REQUIRE(table.erase(i) == 0);
        REQUIRE(table.erase(i + 10) == 0);
    }
}

TEST_CASE("Contains") {
    HopscotchShadow<int> table{};
    table.set_deleted_key(-1);
    for (int i = 0; i < 10; ++i) {
        table.insert(i);
    }
    for (int i = 0; i < 10; ++i) {
        REQUIRE(table.contains(i));
        REQUIRE_FALSE(table.contains(i + 10));
        table.erase(i);
        REQUIRE_FALSE(table.contains(i));
    }
}

TEST_CASE("Types") {
    HopscotchShadow<double> double_table{};
    double_table.set_deleted_key(-0.5);
    double_table.insert(0.5);
    REQUIRE(double_table.contains(0.5));
    REQUIRE(double_table.erase(0.5) == 1);
    REQUIRE_FALSE(double_table.contains(0.5));

    HopscotchShadow<std::string> string_table{};
    string_table.set_deleted_key("DELETED");
    REQUIRE((string_table.insert("Apple")).second);
    string_table.insert("Banana");
    string_table.insert("Peach");
    REQUIRE(string_table.contains("Banana"));
    REQUIRE_FALSE(string_table.contains("Burger"));
    REQUIRE(string_table.erase("Banana") == 1);
    REQUIRE_FALSE(string_table.contains("Banana"));
    REQUIRE(string_table.erase("Banana") == 0);

    HopscotchShadow<std::string> another_string_table{};
    another_string_table.set_deleted_key("DELETED");
    for (int i = 0; i < 1000; ++i) {
        REQUIRE(static_cast<int>(another_string_table.get_size()) == i);
        another_string_table.insert(std::to_string(i));
    }
    REQUIRE(another_string_table.get_size() == 1000);
    REQUIRE(another_string_table.contains("228"));
    REQUIRE_FALSE(another_string_table.contains("-1"));
}

TEST_CASE("Custom Hashes") {
    // TODO implement
}

TEST_CASE("Big Shadow") {
    auto rd = std::random_device{};
    auto rng = std::default_random_engine{rd()};
    vector<int> to_insert{};
    for (int i = 0; i < 100'000; ++i) {
        to_insert.push_back(i);
    }
    std::ranges::shuffle(to_insert, rng);
    HopscotchShadow<int> table{};
    table.set_deleted_key(-1);
    for (int v : to_insert) {
        table.insert(v);
    }
    REQUIRE(table.get_size() == 100'000);
    std::ranges::shuffle(to_insert, rng);
    for (int i = 0; i < 100'000; i += 10) {
        table.erase(to_insert[i]);
    }
    REQUIRE(table.get_size() == 90'000);
    for (int i = 0; i < 100'000; ++i) {
        if (i % 10 == 0) {
            REQUIRE_FALSE(table.contains(to_insert[i]));
        } else {
            REQUIRE(table.contains(to_insert[i]));
        }
    }
}

TEST_CASE("Big Random Shadow Only True") {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<std::mt19937::result_type> distrib(
        0, 1'000'000'000);
    unordered_set<int> elems{};
    while (static_cast<int>(elems.size()) < 100'000) {
        elems.insert(distrib(rng));
    }
    vector<int> to_insert{};
    for (int v : elems) {
        to_insert.push_back(v);
    }
    std::ranges::shuffle(to_insert, rng);

    HopscotchShadow<int> table{};
    table.set_deleted_key(-2);
    for (int v : to_insert) {
        REQUIRE(table.insert(v).second);
    }
    REQUIRE(table.get_size() == 100'000);
    std::ranges::shuffle(to_insert, rng);
    /*for (int i = 0; i < 100'000; i += 10) {
        table.erase(to_insert[i]);
    }
    REQUIRE(table.get_size() == 90'000);
    for (int i = 0; i < 100'000; ++i) {
        if (i % 10 == 0) {
            REQUIRE_FALSE(table.contains(to_insert[i]));
        } else {
            REQUIRE(table.contains(to_insert[i]));
        }
    }*/

    // test:: each element appears exactly once
    unordered_set<int> multi_elems{};
    for (auto it = table.vals.nonempty_begin(); it != table.vals.nonempty_end();
         ++it) {
        int this_key = *it;
        REQUIRE(elems.contains(this_key));
        elems.erase(this_key);
        /*REQUIRE_FALSE(multi_elems.contains(this_key));
        multi_elems.insert(this_key);*/
    }
    REQUIRE(elems.empty());
    for (int i = 0; i < 10 * 100'000; ++i) {
        //cout << i << " " << to_insert[i % 100'000] << endl;
        REQUIRE(table.contains(to_insert[i % 100'000]));
    }
}

TEST_CASE("Big Bitmaps") {
    auto rd = std::random_device{};
    auto rng = std::default_random_engine{rd()};
    vector<int> to_insert{};
    for (int i = 0; i < 100'000; ++i) {
        to_insert.push_back(i);
    }
    std::ranges::shuffle(to_insert, rng);
    HopscotchHashSet<int> table{};
    for (int v : to_insert) {
        table.add(v);
    }
    REQUIRE(table.get_num_elements() == 100'000);
    std::ranges::shuffle(to_insert, rng);
    for (int i = 0; i < 100'000; i += 10) {
        table.remove(to_insert[i]);
    }
    REQUIRE(table.get_num_elements() == 90'000);
    for (int i = 0; i < 100'000; ++i) {
        if (i % 10 == 0) {
            REQUIRE_FALSE(table.contains(to_insert[i]));
        } else {
            REQUIRE(table.contains(to_insert[i]));
        }
    }
}

/*TEST_CASE("builtin_ffs") {
    cout << __builtin_ffs(12) << endl;
    cout << __builtin_ffs(0) << endl;
    cout << __builtin_ffs(1) << endl;
    cout << minbit(12) << endl;
    cout << minbit(0) << endl;
    cout << minbit(1) << endl;
    cout << minbit(0xffffffff) << endl;
    cout << __builtin_ffs(0xffffffff) << endl;
}*/
