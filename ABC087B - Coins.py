A = int(input())
B = int(input())
C = int(input())
X = int(input())
cnt = 0
for a in range(A + 1):
    pay_500 = 500 * a
    for b in range(B + 1):
        pay_100 = 100 * b
        for c in range(C + 1):
            pay_50 = 50 * c
            if pay_500 + pay_100 + pay_50 == X:
                cnt += 1
print(cnt)
