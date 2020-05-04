N, A, B = map(int, input().split())
sum_all = 0
for d in range(1, N + 1):
    d_str = str(d)
    sum = 0
    for val in d_str:
        sum += int(val)
    if A <= sum <= B:
        sum_all += d
print(sum_all)
