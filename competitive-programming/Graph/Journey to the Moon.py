'''https://www.hackerrank.com/challenges/journey-to-the-moon/problem
The member states of the UN are planning to send  people to the moon. They want them to be from different countries. You will be given a list of pairs of astronaut ID's. Each pair is made of astronauts from the same country. Determine how many pairs of astronauts from different countries they can choose from.
Example
 

There are  astronauts numbered  through . Astronauts grouped by country are  and . There are  pairs to choose from:  and .
Function Description
Complete the journeyToMoon function in the editor below.
journeyToMoon has the following parameter(s):
int n: the number of astronauts
int astronaut[p][2]: each element  is a  element array that represents the ID's of two astronauts from the same country
Returns 
- int: the number of valid pairs
Input Format
The first line contains two integers  and , the number of astronauts and the number of pairs. 
Each of the next  lines contains  space-separated integers denoting astronaut ID's of two who share the same nationality.
Constraints


Sample Input 0
5 3
0 1
2 3
0 4
Sample Output 0
6
Explanation 0
Persons numbered  belong to one country, and those numbered  belong to another. The UN has  ways of choosing a pair:

Sample Input 1
4 1
0 2
Sample Output 1
5
Explanation 1
Persons numbered  belong to the same country, but persons  and  don't share countries with anyone else. The UN has  ways of choosing a pair:'''


class Node:
    def __init__(self, data):
        self.data = data
        self.parent = None
        self.rank = 0
        self.descendents = 1

    def union(self, other):
        r1 = self.find()
        r2 = other.find()
        if r1 == r2:
            return
        if r1.rank < r2.rank:
            r1.parent = r2
            r2.descendents += r1.descendents
        elif r1.rank > r2.rank:
            r2.parent = r1
            r1.descendents += r2.descendents
        else:
            r2.parent = r1
            r1.descendents += r2.descendents
            r1.rank += 1

    def find(self):
        return self if self.parent is None else self.parent.find()


def get_ans(astros, pairs):
    nodes = [Node(astro) for astro in range(astros)]
    for pair in pairs:
        nodes[pair[0]].union(nodes[pair[1]])
    sizes = [root.descendents for root in nodes if root.find() == root]
    def sq(x): return x * x
    return (sq(sum(sizes)) - sum(map(sq, sizes))) / 2


if __name__ == '__main__':
    astros, lines = map(int, raw_input().split())
    pairs = []
    for i in range(lines):
        pair = tuple(map(int, raw_input().split()))
        pairs.append(pair)
    print get_ans(astros, pairs)
