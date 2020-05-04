import sys

N, Y = map(int, input().split())
flg_truth = False
for s_1 in range(N + 1):
    for s_2 in range(s_1, N + 1):
        n_10000 = s_1
        n_5000 = s_2 - s_1
        n_1000 = N - s_2
        money = 10000 * n_10000 + 5000 * n_5000 + 1000 * n_1000
        if money == Y:
            print("{} {} {}".format(n_10000, n_5000, n_1000))
            sys.exit()
if not flg_truth:
    print("-1 -1 -1")