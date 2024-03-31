
# BFS pseudocode

G = {  'A' : ['B', 'D', 'F'],
       'B' : ['A', 'D'],
       'C' : ['D'],
       'D' : ['A', 'B', 'C', 'F', 'G'],
       'E' : ['G'],
       'F' : ['A', 'D'],
       'G' : ['D', 'E'] } 

def BFS(G, root, target): # Pythonish pseudocode
   S.add(root)
   T.add(root, None)      # value, parent
   Q = [root]
   while Q:
      cur = Q.pop(0)
      if cur is target:
         return cur parent list
      for v in G[cur]:
         if v not in S:
            S.add(v)
            T.add(v, cur)
            Q += [v]

BFS(G, 'Me', 'Ed')
