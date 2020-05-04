n = int(input())
a_i = map(int, input().split())
# 2進数に変換し右側に0が並ぶ最小数を調べる
min_iter = 2 ** 30
for val in a_i:
    a_bin = bin(val)[2:]
    num_iter = len(a_bin) - a_bin.rfind("1") - 1
    if num_iter < min_iter:
        min_iter = num_iter
print(min_iter)