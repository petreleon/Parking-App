// Parking App.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <chrono>
#include <thread>
#include <deque> 
#include <memory>
#include <regex> //commands
#include <set>
#include <ctime>
#include <iomanip>
#include <map>
#include <functional>
#include <fstream>
#include <cstdio>
#include <tuple>

template <class type> time_t addTime(const time_t& time, const int& value) {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(time) + type(value));
}

template <class type> struct Model {
    type v;
};

template <class type> bool operator< (const Model<type>& model1, const Model<type>& model2) {
    return model1.v < model2.v;
}

struct Car: Model<std::string> {
    
};

struct ParkingPlace : Model<std::string> {

};

struct ParkingType : Model<std::string> {
    std::time_t timeMax(const std::time_t& timeBegin) {
        std::time_t timeMax = 0;
        if (this->v == "ORANGE") {
            timeMax = addTime<std::chrono::hours>(timeBegin, 1);
        }

        if (this->v == "PURPLE") {
            timeMax = addTime<std::chrono::hours>(timeBegin, 3);
        }

        if (this->v == "BLUE") {
            timeMax = addTime<std::chrono::hours>(timeBegin, 24);
        }

        return timeMax;
    }
}; 

struct ParkingZone : Model<std::string> {

};

struct PlaceMax : Model<unsigned> {
    bool possibleToIncrease(const unsigned& counting) {
        return counting < this->v;
    }
};


struct Hour : Model<unsigned> {

};

struct Minute : Model<unsigned> {

};

struct ZoneHourMinute : Model<std::tuple<ParkingZone, Hour, Minute>> {

}; 

struct ZoneTypeMax : Model<std::tuple<ParkingZone, ParkingType, PlaceMax>> {
    
};

//ZoneHourMinute zoneExample1({ {Zone({"22"}), Hour({23}), Minute({23})} });
//ZoneHourMinute zoneExample2({ {Zone({"w22"}), Hour({23}), Minute({23})} });

struct Money : Model<long double> {
    Money operator + (const Money& money) {
        return { this->v + money.v };
    }

    Money operator += (const Money& money) {
        (*this) = (*this) + money;
        return (*this);
    }
};


typedef std::map<time_t, std::vector<std::function< void() >>> EventsMap;
typedef std::map<ParkingPlace, ZoneTypeMax> ParkingPlaceDetails;
typedef std::map < ZoneHourMinute, Money> ParkingZonePrices;
typedef std::map<std::tuple<ParkingPlace, time_t>, std::set<Car>> CarsInParking;

void carCommandParsing(ParkingPlaceDetails& parkingPlaces,const std::string& command, ZoneTypeMax& zoneTypeMax, std::time_t& timeBegin, std::time_t& duration, Car& car, ParkingPlace& place) {
    std::string ignore, date, clock;
    std::stringstream ssComm(command);
    ssComm >> ignore >> car.v >> ignore >> place.v >> date >> clock >> duration;

    std::cout << car.v << " " << place.v << " " << date << " " << clock << std::endl;
    std::stringstream ssTime(date + " " + clock + ":00");
    std::tm timeBeginParking;
    ssTime >> std::get_time(&timeBeginParking, "%d_%m_%Y %H:%M:%S");
    timeBegin = std::mktime(&timeBeginParking);
    zoneTypeMax = parkingPlaces[ParkingPlace({ place })];
}

bool verifyMaxCapacity(std::time_t& timeBegin, std::time_t& timeStop, PlaceMax& maxCapacityOfLocation, ParkingPlace& place, CarsInParking &carsInParking) {
    for (std::time_t timeIterator = timeBegin; timeIterator < timeStop; timeIterator = addTime<std::chrono::minutes>(timeIterator, 15)) {
        if (carsInParking.count({ place, timeIterator })) {
            if (!maxCapacityOfLocation.possibleToIncrease(carsInParking[{ place, timeIterator }].size())) {
                std::cout << "Sorry, all parking reserved." << std::endl;
                return true;
            }
        }
    }
    return false;
}

void addCarForFollowingVerificationsAndPriceComptuting(std::time_t &timeBegin, std::time_t& timeStop, CarsInParking& carsInParking, ParkingPlace& place, EventsMap& events, Car& car, ParkingZone& parkingZone, Money& wallet, ParkingZonePrices& parkingZonePrices) {
    std::tm timeIteratorStruct_;
    for (std::time_t timeIterator = timeBegin; timeIterator < timeStop; timeIterator = addTime<std::chrono::minutes>(timeIterator, 15)) {
        if (!carsInParking.count({ place, timeIterator })) {
            carsInParking[{ place, timeIterator }] = std::set<Car>();
            std::time_t whenToDelete = timeIterator = addTime<std::chrono::minutes>(timeIterator, 15);
            if (!events.count(whenToDelete)) {
                events[whenToDelete] = std::vector<std::function <void()>>();
            }
            events[whenToDelete].push_back([&, place, timeIterator]()
                {
                    carsInParking.erase({ place, timeIterator });
                }
            );
        }
        carsInParking[{ place, timeIterator }].insert(car);

        localtime_s(&timeIteratorStruct_, &timeIterator);
        ZoneHourMinute zoneHourMinute({ {ParkingZone({ parkingZone }), Hour({ (unsigned)timeIteratorStruct_.tm_hour }), Minute({ (unsigned)timeIteratorStruct_.tm_min })} });
        wallet += parkingZonePrices[zoneHourMinute];
    }
}

void programACar(ParkingPlaceDetails& parkingPlaces, std::string& command, std::tm& timeNow, CarsInParking& carsInParking, EventsMap& events, Money& wallet, ParkingZonePrices& parkingZonePrices) {
    std::time_t duration;
    std::time_t timeBegin;
    ParkingPlace place;
    ZoneTypeMax zoneTypeMax;
    Car car;
    carCommandParsing(parkingPlaces, command, zoneTypeMax, timeBegin, duration, car, place);
    PlaceMax maxCapacityOfLocation = std::get<PlaceMax>(zoneTypeMax.v);
    ParkingType parkingType = std::get<ParkingType>(zoneTypeMax.v);
    ParkingZone parkingZone = std::get<ParkingZone>(zoneTypeMax.v);

    if (timeBegin < mktime(&timeNow)) {
        std::cout << "you can't reserve the past." << std::endl;
        return;
    }

    std::time_t timeMax = parkingType.timeMax(timeBegin);

    std::time_t timeStop = addTime<std::chrono::minutes>(timeBegin, duration);
    std::time_t timeMessage = addTime<std::chrono::minutes>(timeStop, -10);

    if (timeStop > timeMax) {
        std::cout << "Time too long" << std::endl;
        return;
    }

    if (verifyMaxCapacity(timeBegin, timeStop, maxCapacityOfLocation, place, carsInParking)) {
        return;
    }

    addCarForFollowingVerificationsAndPriceComptuting(timeBegin, timeStop, carsInParking, place, events, car, parkingZone, wallet, parkingZonePrices);

    if (!events.count(timeBegin)) {
        events[timeBegin] = std::vector<std::function <void()>>();
    }
    events[timeBegin].push_back([car, place]()
        {
            std::cout << car.v << " parking at " << place.v << std::endl;
        }
    );

    if (!events.count(timeMessage)) {
        events[timeMessage] = std::vector<std::function <void()>>();
    }
    events[timeMessage].push_back([car, place]()
        {
            std::cout << "Attention! Your parking for " << car.v << " at " << place.v << " is about to expire" << std::endl;
        }
    );

    if (!events.count(timeStop)) {
        events[timeStop] = std::vector<std::function <void()>>();
    }
    events[timeStop].push_back([car, place]()
        {
            std::cout << car.v << " left " << place.v << std::endl;
        }
    );
}

void event_loop(bool& toClose, bool& pause, std::tm& timeNow, EventsMap& events) {
    for (;!toClose;) {
        if (!pause) {
            time_t actualTime = mktime(&timeNow);
            if (events.count(actualTime)) {
                for (auto& event : events[actualTime]) {
                    event();
                }
                events.erase(actualTime);
            }
            std::cout << "continue" << std::endl;
            std::cout << std::put_time(&timeNow, "%d %m %Y %H:%M:%S") << std::endl;
            actualTime = addTime<std::chrono::minutes>(mktime(&timeNow), 5);
            localtime_s(&timeNow, &actualTime);
        } 
        
        std::chrono::seconds dura(5);
        std::this_thread::sleep_for(dura);
    }
}

void commands(bool& toClose, bool& pause, std::tm& timeNow, EventsMap& events, ParkingZonePrices& parkingZonePrices, ParkingPlaceDetails &parkingPlaces) {
    std::string command;
    ZoneTypeMax zoneTypeMax;
    CarsInParking carsInParking;
    std::tm timeIteratorStruct_;
    Money wallet({ 0 });
    while (!toClose) {
        std::getline(std::cin, command);
        if (command == "exit") {
            toClose = true;
            break;
        }

        if (command == "pause") {
            pause = true;
        }

        if (command == "start") {
            pause = false;
        }

        if (command == "money") {
            std::cout << "Income is:" << wallet.v << std::endl;
        }

        if (std::regex_match(command, std::regex("(Program)(.*)"))) {
            programACar(parkingPlaces, command, timeNow, carsInParking, events, wallet, parkingZonePrices);
        }
    }
}

int main()
{
    ParkingZonePrices parkingZonePrices;

    std::ifstream parkingZones("parking zones.txt");

    std::ifstream parkinglocations("parking locations.txt");
    std::ifstream pricingModifier("price modifiers.txt");
    std::string  idZone, zonePrice;
    while (parkingZones >> idZone >> zonePrice) {
        std::cout << idZone << " " << zonePrice << std::endl;
        Money zonePricePerQuarter = { std::stof(zonePrice) / 4 };
        for (unsigned hour = 0; hour < 24; ++hour) {
            for (unsigned minute = 0; minute < 60; minute += 15) {
                parkingZonePrices[{ { { ParkingZone{ idZone }, Hour{ hour }, Minute{ minute }} } }] = zonePricePerQuarter;
            }
        }
    }
    
    std::string locationID, zoneType, parkingType, maxParking;
    ParkingPlaceDetails parkingPlaces;
    while (parkinglocations >> locationID >> zoneType >> parkingType >> maxParking) {
        parkingPlaces[ParkingPlace({ locationID })] = { { { ParkingZone{zoneType}, ParkingType{parkingType}, PlaceMax{(unsigned)std::stoi(maxParking)} } } };
        std::cout << locationID << " " << zoneType << " " << parkingType << " " << maxParking << std::endl;
    }

    std::string zoneType_, startTime, stopTime, ratio;
    unsigned hourStart, minuteStart, hourStop, minuteStop;
    while (pricingModifier >> zoneType_ >> startTime >> stopTime >> ratio) {
        float ratioMultiplier = std::stof(ratio);
        std::cout << zoneType_ << " " << startTime << " " << stopTime << " " << ratio << " " << std::endl;
        sscanf_s(startTime.c_str(), "%d:%d", &hourStart, &minuteStart);
        sscanf_s(stopTime.c_str(), "%d:%d", &hourStop, &minuteStop);
        for (unsigned hour = hourStart, minute = minuteStart; hour != hourStop or minute != minuteStop; minute += 15, hour += minute / 60, minute %= 60, hour %= 24) {
            parkingZonePrices[{ { { ParkingZone{ idZone }, Hour{ hour }, Minute{ minute }} } }].v *= ratioMultiplier;
        }
    }
    std::tm actualTimeStruct = {};
    std::stringstream ss("9 12 2014 00:00:00");
    ss >> std::get_time(&actualTimeStruct, "%d %m %Y %H:%M:%S");
    time_t actualTime = mktime(&actualTimeStruct);
    std::cout << std::put_time(&actualTimeStruct, "%d %m %Y %H:%M:%S") << std::endl;
    localtime_s(&actualTimeStruct, &actualTime);
    std::cout << std::put_time(&actualTimeStruct, "%d %m %Y %H:%M:%S") << std::endl;
    bool toClose = false;
    bool pause = false;
    EventsMap events;
    std::thread event_thread(event_loop, std::ref(toClose), std::ref(pause), std::ref(actualTimeStruct), std::ref(events));
    std::thread command_thread(commands, std::ref(toClose), std::ref(pause), std::ref(actualTimeStruct), std::ref(events), std::ref(parkingZonePrices), std::ref(parkingPlaces));
    event_thread.join();
    command_thread.join();

    return EXIT_SUCCESS;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
