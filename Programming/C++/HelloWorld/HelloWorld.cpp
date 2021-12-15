#include <iostream>
#include <vector>
using namespace std;

// Question :
// Class design

    // Parking lot 50 slots
    //     Any vehicle can be parked in any slot
    //         No 2 vehicle can be parked in same slot at a time
    //             For a calender day,
    // the parking fee is 100 rs

    //         entry_to_parking_lot

    //             exit_from_parking_lot -
    //     cal total fare

class Parking{
    public:
    int arr[50];


}

class Vehicle :Parking
{
public:
    int fare;
    string name;
    int vehicle_number;
    date entry_date;
    date exit_date;
    int slot_number;
    Vehicle(string s)
    {
        name = s;
    }

        void entry(date en,int slot)
        {
            entry_date = en; // current date also
            slot_number = get_available_slot_number();
            fare = 100;

            if (slot < 50)
            {
                if (arr[slot] == 0)
                {
                    arr[slot] = 1;
                    cout <<"Vehicle parked in slot: " << slot <<"at date :" <<entry_date <<endl;
                }
                else
                {
                    cout << "Park in another slot, this one is full"
                }
            }
            else{
            cout << "Slot chose is more than 50; choose slot in range"
        }
        }
    void exit(date ex){
        // if (arr[ex]==0){
        //     cout<<"Slot already empty"
        // }
        // available_slot()
        else{
            exit_date = ex;
            arr[slot_number] = 0;
            fare();
            cout << "Vehicle exited";
        }
        
    }
    int fare(){
        if (entry_date==NULL){
            cout<< "Vehicle haven't entered"
            return 0;
        }
        if(exit_date==NULL){
            cout << "Vehicle haven't exited";
            return 0;
        }
        if (exit_date>entry_date){
            int num_of_days = exit_date - entry_date;
            fare += num_of_days * 100;
        }
    }

}

int main()
{
    Vehicle abc = new Vehicle();

    return 0;
}

// Tech Stacks - Kotlin, ruby on rails, PostgreSQL, AWS 