
# File: fib_memo.py

# Fibonacci with memoization

M = {}  # dict of already computed values
def F(n):         # n is int, >= 0
    if n in M:
        return M[n]
    if n == 0 or n == 1:
        ret = n
    else:
        ret = F(n - 1) + F(n - 2)
    M[n] = ret
    return ret

# test code:

if __name__ == '__main__':
    print('F(0):', F(0))
    print('F(1):', F(1))
    print('F(2):', F(2))
    print('F(5):', F(5))
    print('F(10):', F(10))
    print('F(15):', F(15))
    print('F(20):', F(20))
    print('F(25):', F(25))
    print('F(30):', F(30))
    print('F(35):', F(35))
    print('F(40):', F(40))
    print('F(60):', F(60))
    print('F(80):', F(80))
    print('F(100):', F(100))
    print('F(200):', F(200))
    
