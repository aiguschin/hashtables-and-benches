#include <iostream>

#include "benches.h"
#include "hopscotch_shadow.h"

using std::cout;
using std::endl;

int main() {
    //bench_single_size_and_op(1000000, OpType::Insert);
    //bench_single_size_and_op(1000000, OpType::Remove);
    //bench_single_size_and_op(1000000, OpType::TrueContains);
    //bench_single_size_and_op(1000000, OpType::TrueContains);
    //bench_single_size(1000000);
    bench_everything();
    /*print<int>(1);
    cout << endl;*/
    /*print<vector<int>>({1});
    cout << endl;*/
    /*HopscotchShadow<int, std::hash<int>> table{};
    table.print();*/
}