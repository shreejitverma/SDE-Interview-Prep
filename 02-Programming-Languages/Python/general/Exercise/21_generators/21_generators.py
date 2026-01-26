# Author: Shreejit Verma
 # GitHub: https://github.com/shreejitverma

def next_square():
    i = 1
    while True:
        yield i * i
        i += 1


for n in next_square():
    if n > 25:
        break
    print(n)
