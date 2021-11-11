/*1603. Design Parking System
Easy
451217Add to ListShare
Design a parking system for a parking lot. The parking lot has three kinds of parking spaces:
 big, medium, and small, with a fixed number of slots for each size.
Implement the ParkingSystem class:
	• ParkingSystem(int big, int medium, int small) Initializes object of the ParkingSystem class. 
    The number of slots for each parking space are given as part of the constructor.
	• bool addCar(int carType) Checks whether there is a parking space of carType 
    for the car that wants to get into the parking lot. 
    carType can be of three kinds: big, medium, or small, 
    which are represented by 1, 2, and 3 respectively. 
    A car can only park in a parking space of its carType. 
    If there is no space available, return false, else park the car in that size space and return true.
 
Example 1:
Input
["ParkingSystem", "addCar", "addCar", "addCar", "addCar"]
[[1, 1, 0], [1], [2], [3], [1]]
Output
[null, true, true, false, false]
Explanation
ParkingSystem parkingSystem = new ParkingSystem(1, 1, 0);
parkingSystem.addCar(1); // return true because there is 1 available slot for a big car
parkingSystem.addCar(2); // return true because there is 1 available slot for a medium car
parkingSystem.addCar(3); // return false because there is no available slot for a small car
parkingSystem.addCar(1); // return false because there is no available slot for a big car. It is already occupied.
 
Constraints:
	• 0 <= big, medium, small <= 1000
	• carType is 1, 2, or 3
	• At most 1000 calls will be made to addCar
*/
#include <iostream>
using namespace std;

class ParkingSystem
{
public:
    int big, medium, small;
    ParkingSystem(int big1, int medium1, int small1)
    {
        big = big1;
        medium = medium1;
        small = small1;
    }

    bool addCar(int carType)
    {
        if (carType == 1)
        {
            if (big == 0)
                return false;
            big--;
        }
        else if (carType == 2)
        {
            if (medium == 0)
                return false;
            medium--;
        }
        else
        {
            if (small == 0)
                return false;
            small--;
        }
        return true;
    }
};

int main()
{
    ParkingSystem obj = ParkingSystem(1, 1, 0);
    obj.addCar(1) ? cout << "true " : cout << "false ";
    obj.addCar(2) ? cout << "true " : cout << "false ";
    obj.addCar(3) ? cout << "true " : cout << "false ";
    obj.addCar(1) ? cout << "true " : cout << "false ";

    return 0;
}

/*
class ParkingSystem:

    def __init__(self, big, medium, small):
        self.slots = {1: big, 2: medium, 3: small}
        

    def addCar(self, carType: int) -> bool:
        if self.slots[carType] == 0:
            return False
        else:
            self.slots[carType] -= 1
            return True
        
# Your ParkingSystem object will be instantiated and called as such:
# obj = ParkingSystem(big, medium, small)
# param_1 = obj.addCar(carType)

*/
import java.util.*;
import java.lang.*;

class ParkingSystem
{
    int big, medium, small;
public
    ParkingSystem(int big, int medium, int small)
    {
        this.big = big;
        this.medium = medium;
        this.small = small;
    }

public
    boolean addCar(int carType)
    {
        if (carType == 1)
        {
            if (big == 0)
                return false;
            big--;
        }
        else if (carType == 2)
        {
            if (medium == 0)
                return false;
            medium--;
        }
        else
        {
            if (small == 0)
                return false;
            small--;
        }
        return true;
    }
}

class Solution
{
public
    static void main(String args[])
    {
        ParkingSystem obj = new ParkingSystem(1, 1, 0);
        System.out.print(obj.addCar(1) + " ");
        System.out.print(obj.addCar(2) + " ");
        System.out.print(obj.addCar(3) + " ");
        System.out.print(obj.addCar(1) + " ");
    }
}
