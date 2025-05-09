#pragma once

enum class OpType { Insert, Remove, TrueContains, FalseContains };

void bench_single_size_and_op(int size, OpType type, int num_tries);

void bench_single_size(int size, int num_tries);

void bench_everything();
