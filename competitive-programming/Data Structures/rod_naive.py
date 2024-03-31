
# File: rod_naive.py

# naive rod cutting

def price(i):
    if i >= 1 and i <= 4:
        return [0, 1, 5, 8, 9][i]
    return 0

def Rev(N):
    mxrev = 0
    if N <= 1:
        mxrev = price(N)
    else:
        for k in range(1,N+1):  # [1..N]
            mxrev = max(mxrev,price(k)+Rev(N-k))
    return mxrev


# test code:

if __name__ == '__main__':
    print('Rev(10):', Rev(10))
    print('Rev(15):', Rev(15))
    print('Rev(20):', Rev(20))
    print('Rev(25):', Rev(25))
    print('Rev(30):', Rev(30))
    
