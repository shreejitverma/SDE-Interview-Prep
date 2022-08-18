#include "vehicle_factory.hpp"
#include <iostream>
using namespace std;

int main(){
    string vehicleType;
    cin >> vehicleType;
    Vehicle *vehicle = VehicleFactory::getVehicle(vehicleType);
    if(vehicle != NULL){
        vehicle->createVehicle();
    }
    return 0;
}