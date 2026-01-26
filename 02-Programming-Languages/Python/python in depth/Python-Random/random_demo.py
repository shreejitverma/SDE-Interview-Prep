# Author: Shreejit Verma
 # GitHub: https://github.com/shreejitverma

import random

deck = list(range(1, 53))

hand = random.sample(deck, k=5)
print(hand)
