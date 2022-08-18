/*
706. Design HashMap
Easy
1959213Add to ListShare
Design a HashMap without using any built-in hash table libraries.
Implement the MyHashMap class:
	• MyHashMap() initializes the object with an empty map.
	• void put(int key, int value) inserts a (key, value) pair into the HashMap. If the key already exists in the map, update the corresponding value.
	• int get(int key) returns the value to which the specified key is mapped, or -1 if this map contains no mapping for the key.
	• void remove(key) removes the key and its corresponding value if the map contains the mapping for the key.
 
Example 1:
Input
["MyHashMap", "put", "put", "get", "get", "put", "get", "remove", "get"]
[[], [1, 1], [2, 2], [1], [3], [2, 1], [2], [2], [2]]
Output
[null, null, null, 1, -1, null, 1, null, -1]
Explanation
MyHashMap myHashMap = new MyHashMap();
myHashMap.put(1, 1); // The map is now [[1,1]]
myHashMap.put(2, 2); // The map is now [[1,1], [2,2]]
myHashMap.get(1);    // return 1, The map is now [[1,1], [2,2]]
myHashMap.get(3);    // return -1 (i.e., not found), The map is now [[1,1], [2,2]]
myHashMap.put(2, 1); // The map is now [[1,1], [2,1]] (i.e., update the existing value)
myHashMap.get(2);    // return 1, The map is now [[1,1], [2,1]]
myHashMap.remove(2); // remove the mapping for 2, The map is now [[1,1]]
myHashMap.get(2);    // return -1 (i.e., not found), The map is now [[1,1]]
 
Constraints:
	• 0 <= key, value <= 106
	• At most 104 calls will be made to put, get, and remove.
*/
class MyHashMap
{
public:
    vector<int> keys;
    vector<int> values;

    /** Initialize your data structure here. */
    MyHashMap()
    {
    }

    /** value will always be non-negative. */
    void put(int key, int value)
    {
        vector<int>::iterator it = find(keys.begin(), keys.end(), key);
        if (it != keys.end())
        {
            values[(it - keys.begin())] = value;
        }
        else
        {
            keys.push_back(key);
            values.push_back(value);
        }
    }

    /** Returns the value to which the specified key is mapped, or -1 if this map contains no mapping for the key */
    int get(int key)
    {
        vector<int>::iterator it = find(keys.begin(), keys.end(), key);
        if (it == keys.end())
        {
            return -1;
        }
        else
        {
            return values[it - keys.begin()];
        }
    }

    /** Removes the mapping of the specified value key if this map contains a mapping for the key */
    void remove(int key)
    {
        vector<int>::iterator it = find(keys.begin(), keys.end(), key);
        if (it != keys.end())
        {
            keys.erase(it);
            values.erase(values.begin() + (it - keys.begin()));
        }
    }
};

/**
 * Your MyHashMap object will be instantiated and called as such:
 * MyHashMap* obj = new MyHashMap();
 * obj->put(key,value);
 * int param_2 = obj->get(key);
 * obj->remove(key);



Cache (types)
Crickinfo
Vending machine
Instagram
Lift Elevator design


Last year questions

Tapir Rain water
0,1 matrix 
00 shortest math
Tree traversal
How far Node is ..how many nodes
Rotten tomatoes

Geeksforgeeks -> Goldman Sachs -> Interview Experience

Mutex Semaphores
Multi threading 

Companies -> Goldman Sachs, Morgan Stanley, Tower, One direct, Capital one, Jp Morgan, Blackrock (Associate (SDE II))


 */
