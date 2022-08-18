#include "vehicle_factory.hpp"

Vehicle* VehicleFactory::getVehicle(string vehicleType){
    Vehicle *vehicle;
    if(vehicleType == "car"){
        vehicle = new Car();
    }
    else if(vehicleType == "bike"){
        vehicle = new Bike();
    }
    else{
        cout << "Invalid vehicle type" << endl;
        return NULL;
    }
    return vehicle;
}