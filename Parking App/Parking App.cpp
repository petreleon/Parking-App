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



void event_loop(bool& toClose, bool& pause, std::tm& timeNow, std::map<time_t, std::vector<std::function< void() >>>& events) {
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
            actualTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(mktime(&timeNow)) + std::chrono::minutes(5));
            localtime_s(&timeNow,&actualTime);
        }
        
        std::chrono::seconds dura(5);
        std::this_thread::sleep_for(dura);
    }
}

void commands(bool& toClose, bool& pause, std::tm& timeNow, std::map<time_t, std::vector<std::function< void() >>>& events, std::map < std::tuple <std::string, unsigned, unsigned>, float>  &parkingZonePrices, std::map<std::string, std::tuple < std::string, std::string, unsigned> > &parkingPlaces) {
    std::string command;
    std::map<std::tuple<std::string, time_t>, std::set<std::string>> carsInParking;
    std::tm timeIteratorStruct_;
    long double wallet = 0;
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
            std::cout << "Income is:" << wallet << std::endl;
        }

        if (std::regex_match(command, std::regex("(Program)(.*)"))) {
            int duration;
            bool possible = true;
            std::string car, ignore, date, clock, place;
            std::stringstream ssComm(command);
            ssComm >> ignore >> car >> ignore >> place >> date >> clock >> duration;

            std::cout << car << " " << place << " " << date << " " << clock << std::endl;
            std::stringstream ssTime(date + " " + clock+":00");
            std::tm timeBeginParking;
            ssTime >> std::get_time(&timeBeginParking, "%d_%m_%Y %H:%M:%S");
            std::time_t timeBegin = std::mktime(&timeBeginParking);
            unsigned maxCapacityOfLocation = std::get<2>(parkingPlaces[place]);
            std::string parkingType = std::get<1>(parkingPlaces[place]);
            std::string parkingZone = std::get<0>(parkingPlaces[place]);
            
            if (timeBegin < mktime(&timeNow)) {
                std::cout << "you can't reserve the past." << std::endl;
                continue;
            }

            std::time_t timeMax = 0;

            if (parkingType == "ORANGE") {
                timeMax = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(timeBegin) + std::chrono::minutes(60));
            }

            if (parkingType == "PURPLE") {
                timeMax = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(timeBegin) + std::chrono::hours(3));
            }

            if (parkingType == "BLUE") {
                timeMax = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(timeBegin) + std::chrono::hours(24));
            }

            std::time_t timeStop = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(timeBegin) + std::chrono::minutes(duration));
            std::time_t timeMessage = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(timeStop) - std::chrono::minutes(10));

            if (timeStop > timeMax) {
                std::cout << "Time too long" << std::endl;
                continue;
            }

            for (std::time_t timeIterator = timeBegin; timeIterator < timeStop; timeIterator = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(timeIterator) + std::chrono::minutes(15))) {
                if (carsInParking.count({ place, timeIterator })) {
                    if (carsInParking[{ place, timeIterator }].size() == maxCapacityOfLocation) {
                        std::cout << "Sorry, all parking reserved." << std::endl;
                        possible = false;
                        break;
                    }
                }
            }

            if (possible == false) {
                continue;
            }

            for (std::time_t timeIterator = timeBegin; timeIterator < timeStop; timeIterator = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(timeIterator) + std::chrono::minutes(15))) {
                if (!carsInParking.count({ place, timeIterator })) {
                    carsInParking[{ place, timeIterator }] = std::set<std::string>();
                    std::time_t whenToDelete = timeIterator = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(timeIterator) + std::chrono::minutes(15));
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
                wallet += parkingZonePrices[{ parkingZone, timeIteratorStruct_.tm_hour, timeIteratorStruct_.tm_min}];
            }

            if (!events.count(timeBegin)) {
                events[timeBegin] = std::vector<std::function <void()>>();
            }
            events[timeBegin].push_back([car, place]()
                {
                    std::cout << car << " parking at " << place << std::endl;
                }
            );

            if (!events.count(timeMessage)) {
                events[timeMessage] = std::vector<std::function <void()>>();
            }
            events[timeMessage].push_back([car, place]()
                {
                    std::cout << "Attention! Your parking for " << car << " at " << place << " is about to expire" << std::endl;
                }
            );

            if (!events.count(timeStop)) {
                events[timeStop] = std::vector<std::function <void()>>();
            }
            events[timeStop].push_back([car, place]()
                {
                    std::cout << car << " left " << place << std::endl;
                }
            );
        }
    }
}

int main()
{
    std::map < std::tuple <std::string,unsigned, unsigned>, float>  parkingZonePrices;

    std::ifstream parkingZones("parking zones.txt");

    std::ifstream parkinglocations("parking locations.txt");
    std::ifstream pricingModifier("price modifiers.txt");
    std::string  idZone, zonePrice;
    while (parkingZones >> idZone >> zonePrice) {
        std::cout << idZone << " " << zonePrice << std::endl;
        float zonePricePerQuarter = std::stof(zonePrice) / 4;
        for (unsigned hour = 0; hour < 24; ++hour) {
            for (unsigned minute = 0; minute < 60; minute += 15) {
                parkingZonePrices[{idZone, hour, minute}] = zonePricePerQuarter;
            }
        }
    }
    
    std::string locationID, zoneType, parkingType, maxParking;
    std::map<std::string, std::tuple < std::string, std::string, unsigned> > parkingPlaces;
    while (parkinglocations >> locationID >> zoneType >> parkingType >> maxParking) {
        parkingPlaces[locationID] = { zoneType, parkingType, std::stoi(maxParking) };
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
            parkingZonePrices[{idZone, hour, minute}] *= ratioMultiplier;
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
    std::map<time_t, std::vector<std::function< void() >>> events;
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
