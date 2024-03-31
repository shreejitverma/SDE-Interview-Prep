
# File BT_app.py
# Author(s):

import BinaryTree as bt

bt1 = bt.BinaryTree()     # an empty BinaryTree
bt1.insert(12)
bt1.insert(7)
bt1.insert(13)
bt1.insert(-4)

bt2 = bt.BinaryTree()     # an empty tree
print('bt2:', bt2)        # bt2:

print('bt1:', bt1)        # bt1: -4 7 12 13

print('bt1.sum():', bt1.sum())   # bt1.sum(): 28

