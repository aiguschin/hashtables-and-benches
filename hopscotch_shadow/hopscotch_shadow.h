#pragma once

#include <cassert>
#include <concepts>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <type_traits>

#include <sparsehash/sparsetable>

using std::cout;
using std::endl;
using std::pair;

using google::sparsetable;

template<typename T>
concept Printable = requires(T val)
{
    { std::cout << val } -> std::same_as<std::ostream&>;
};

/*template<typename T>
concept NotPrintable = !Printable<T>;*/

template<Printable T>
void print(const T& val) {
    cout << val;
}

template<typename T>
void print(const T& val) {
    cout << &val;
}

/*template<NotPrintable T>
std::ostream& operator<<(std::ostream& os, T& val) {
    return (os << &val);
}*/

template<class Key, class Hash = std::hash<Key>>
class HopscotchShadow {
private:
    // table size MUST be 2^n
    sparsetable<Key> vals{}; // (64); // some basic init -- can be any power of 2
    int hop_range = 32;
    int add_range = 128;
    int max_resize_tries = 2; // corresponds both to tries in single resize and to resize calls on add TODO maybe fix?
    Hash hasher{};

public:
    HopscotchShadow(){
        vals = sparsetable<Key>(64);

    };
    HopscotchShadow(int init_hop_range, int init_add_range, int init_max_resize_tries) {
        vals = sparsetable<Key>(64);
        hop_range = init_hop_range;
        add_range = init_add_range;
        max_resize_tries = init_max_resize_tries;
    }

    void set_size(uint32_t size) {
        vals.resize(size);
    }

    uint32_t hash(const Key& key) const {
        return hasher(key) & (vals.size() - 1);
    }

    uint32_t find_elem(const Key& key) const; // returns index of key in vals
    bool contains(const Key& key) const;
    pair<uint32_t, bool> insert(const Key& key); // returns {index of key in vals, true if inserted otherwise false}
    uint32_t erase(const Key& key); // returns 1 if key was deleted, 0 otherwise
    void print() const;
    
    uint32_t get_size() const {
        return vals.num_nonempty();
    }
    uint32_t get_max_size() const {
        return vals.size();
    }
    float load_factor() const {
        return static_cast<float>(vals.num_nonempty()) / static_cast<float>(vals.size());
    }

private:
    void resize();
    pair<uint32_t, bool> tryinsert(const Key& key); // returns {index of key in vals, true if inserted otherwise false}

};

template<class Key, class Hash>
void HopscotchShadow<Key, Hash>::print() const {
    cout << "Table: ";
    for (auto it = vals.nonempty_begin(); it != vals.nonempty_end(); ++it) {
        //print(*it);
        cout << *it;
        cout << " ";
    }
    cout << endl;
    cout << "Elements: " << vals.num_nonempty() << endl;
    cout << "Size: " << vals.size() << endl;
}

template<class Key, class Hash>
void HopscotchShadow<Key, Hash>::resize() {
    uint32_t next_size = vals.size();
    for (int iteration = 0; iteration < max_resize_tries; ++iteration) {
        next_size *= 2;
        HopscotchShadow<Key, Hash> new_table(hop_range, add_range, max_resize_tries);
        new_table.set_size(next_size);

        bool flag = true;
        for (int i = 0; i < static_cast<int>(vals.size()); ++i) { // TODO only check existing elems via nonempty iterator
            if (vals.test(i)) {
                //Key cur_key = vals[i];
                //flag = new_table.tryinsert(cur_key).second;
                flag = new_table.tryinsert(vals[i]).second;
                /*pair<uint32_t, bool> res = new_table.tryinsert(cur_key);
                flag = res.second;*/
                if (!flag) {
                    break;
                }
            }
        }

        if (!flag) {
            // unsuccessful resize -- try again
            continue; 
        }

        std::swap(vals, new_table.vals);
        return;
    }
    throw std::runtime_error("Resize was unsuccessful");
}

template<class Key, class Hash>
uint32_t HopscotchShadow<Key, Hash>::find_elem(const Key& key) const {
    uint32_t bucket_ind = hash(key);
    uint32_t ind_to_check = bucket_ind;
    for (int num_steps = 0; num_steps < add_range; ++num_steps) {
        if (vals.test(ind_to_check)) {
            // key is real
            /*if (hash(vals[ind_to_check]) == bucket_ind) {
                // key is in this bucket
                if (vals[ind_to_check] == key) {
                    // key is found
                    return ind_to_check;
                }
            }*/
            if (vals[ind_to_check] == key) {
                return ind_to_check;
            }
        }

        ++ind_to_check;
        ind_to_check &= (vals.size() - 1);
    }

    return vals.size();
}

template<class Key, class Hash>
bool HopscotchShadow<Key, Hash>::contains(const Key& key) const {
    return (find_elem(key) != vals.size());
}

template<class Key, class Hash>
uint32_t HopscotchShadow<Key, Hash>::erase(const Key& key) {
    uint32_t elem_ind = find_elem(key);
    if (elem_ind == vals.size()) {
        // no key here -- return
        return 0;
    }
    vals.erase(elem_ind);
    return 1;
}

template<class Key, class Hash>
pair<uint32_t, bool> HopscotchShadow<Key, Hash>::tryinsert(const Key& key) {
    // firstly check if contains
    uint32_t position_of_this = find_elem(key);
    if (position_of_this != vals.size()) {
        return {position_of_this, false};
    }
    // if not contains -- insert normally

    // find first empty cell
    uint32_t bucket_ind = hash(key);
    uint32_t ind_to_check = bucket_ind;
    int right_shift;
    for (right_shift = 0; right_shift < add_range; ++right_shift) {
        if (!vals.test(ind_to_check)) {
            // found free cell
            break;
        }

        ++ind_to_check;
        ind_to_check &= (vals.size() - 1);
    }
    
    if (right_shift == add_range) {
        // didn't found free cell -- can't add
        return {vals.size(), false};
    }

    // init cell with garbage
    // something is going wrong with initialization of garbage via empty key?...
    // no! the hopscotch insert is just slow as heck
    /*if constexpr(std::is_integral_v<Key>) {
        vals[ind_to_check] = 1;
    } else {
        vals[ind_to_check] = Key{};
    }*/
    vals[ind_to_check] = Key{};

    // if we have to move elements -- move them
    while (right_shift >= hop_range) {
        bool is_moved = false;
        // check if we can move element into this cell from left to right
        for (int shift_to_move = right_shift - hop_range + 1; shift_to_move < right_shift; ++shift_to_move) {
            uint32_t ind_to_move_from = (bucket_ind + shift_to_move) & (vals.size() - 1);
            uint32_t bucket_to_move_from = hash(vals[ind_to_move_from]);
            // check if ind_to_check is in range of bucket_to_move_from
            if ((ind_to_check >= bucket_to_move_from && ind_to_check - bucket_to_move_from < static_cast<uint32_t>(hop_range))
                ||
                (ind_to_check < bucket_to_move_from && ind_to_check + vals.size() - bucket_to_move_from < static_cast<uint32_t>(hop_range))) 
            {
                uint32_t cur_size = vals.num_nonempty();
                assert(vals.test(ind_to_check));
                assert(vals.test(ind_to_move_from));
                // move filled cell
                Key tmp = vals[ind_to_move_from];
                vals[ind_to_check] = tmp;
                /*if (cur_size + 1 != vals.num_nonempty()) {
                    cout << "Ind to check: " << ind_to_check << endl;
                    cout << "Bucket to move from: " << bucket_to_move_from << endl;
                    cout << "Ind to move from: " << ind_to_move_from << endl;
                    cout << "Test for ind_to_check now: " << vals.test(ind_to_check) << endl;
                    cout << "Test for ind_to_move_from now: " << vals.test(ind_to_move_from) << endl;
                    Key val_on_ind_to_check = vals[ind_to_check];
                    cout << "Elem on ind_to_check: " << val_on_ind_to_check << endl;
                    Key val_on_ind_to_move_from = vals[ind_to_move_from];
                    cout << "Elem on ind_to_move_from: " << val_on_ind_to_move_from << endl;
                    throw std::runtime_error("Key was not inserted");
                }*/
                //vals.erase(ind_to_move_from);

                // fill "empty" cell with garbage
                /*if constexpr(std::is_integral_v<Key>) {
                    vals[ind_to_move_from] = 1;
                } else {
                    vals[ind_to_move_from] = Key{};
                }*/
                vals[ind_to_move_from] = Key{};

                ind_to_check = ind_to_move_from;
                right_shift = shift_to_move;
                is_moved = true;
                uint32_t size_now = vals.num_nonempty();
                assert(cur_size == size_now);
                /*if (cur_size != size_now) {
                    cout << "Cur size: " << cur_size << endl;
                    cout << "Size now: " << size_now << endl;
                    throw std::runtime_error("Bad sizes");
                }*/
                break;
            }
        }

        if (!is_moved) {
            // can't move and are out of range
            // erase garbage, return
            vals.erase(ind_to_check);
            return {vals.size(), false};
        }
    }

    // now we are in range
    uint32_t cur_size = vals.num_nonempty();
    vals[ind_to_check] = key;
    uint32_t size_now = vals.num_nonempty();
    assert(cur_size == size_now);
    return {ind_to_check, true};
}

template <class Key, class Hash>
pair<uint32_t, bool> HopscotchShadow<Key, Hash>::insert(const Key& key) {
    pair<uint32_t, bool> res = tryinsert(key);
    if (res.first != vals.size()) {
        // insert was successful or key already existed
        return res;
    }

    // resize while unsuccessful
    int iter_count = 0;
    while (iter_count < max_resize_tries) {
        resize();
        pair<uint32_t, bool> res_now = tryinsert(key);
        if (res_now.second) {
            // we know here is no key
            return res_now;
        }
        ++iter_count;
    }
    // add fails...
    throw std::runtime_error("Couldn't resize table on add");
}
