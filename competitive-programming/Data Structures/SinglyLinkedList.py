
# File:   SinglyLinkedList.py
# Author: John K. Ostlund

class SinglyLinkedList:
   class _SLLNode:
      def __init__(self, value,
                         next_node = None):
         self._value = value
         self._next = next_node   # next is keyword

   def __init__(self):
      self._first = None

   def __str__(self):
      if self._first == None:
         return ''
      else:
         ret = str(self._first._value)
         _next = self._first._next
         while _next != None:
            ret += ' ' + str(_next._value)
            _next = _next._next
         return ret

   def insert(self, value):
      self._first = \
             SinglyLinkedList._SLLNode(value,
                                   self._first)

   def append(self, value):
      new_node = SinglyLinkedList._SLLNode(
                                   value, None)
      if self._first == None:
         self._first = new_node
      else:
         _next = self._first
         while _next._next != None:
            _next = _next._next
         _next._next = new_node


