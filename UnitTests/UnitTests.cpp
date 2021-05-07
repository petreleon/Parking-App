#include "pch.h"
#include "CppUnitTest.h"
#include "../Parking2/time.hpp"
#include "../Parking2/ReadableTime.hpp"
#include "../Parking2/EventCar.hpp"
#include "../Parking2/ParkingPlace.hpp"
#include "../Parking2/Money.hpp"
#include "../Parking2/Identifier.hpp"
#include "../Parking2/EventCar.hpp"
#include "../Parking2/ParkingManager.hpp"
#include <chrono>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(UnitTests)
	{
	public:

		TEST_METHOD(Add_minutes)
		{
			Time time{ 122 }, time2{ 122 + 60 };
			time.addTime<std::chrono::minutes>(1);
			Assert::IsTrue(time == time2);
		}

		TEST_METHOD(Add_hours)
		{
			Time time{ 122 }, time2{ 122 + 60*60 };
			time.addTime<std::chrono::hours>(1);
			Assert::IsTrue(time == time2);
		}
		TEST_METHOD(Time_conversion_1)
		{
			// avoid summer hour - hope
			ReadableTime readableTime({ "03 05 2021 12:00:00" });
			tm TM;
			readableTime.toTm(TM);
			Time time;
			time.from_gmtm(TM);
			tm TM2 = time.to_gmtm();
			ReadableTime readableTime2;
			readableTime2.fromTm(TM2);
			Assert::IsTrue(readableTime == readableTime2);
		}
		TEST_METHOD(Time_conversion_2)
		{
			// avoid summer hour - hope
			ReadableTime readableTime({ "03 05 2021 12:00:00" });
			Time time;
			time.fromReadable(readableTime);
			ReadableTime readableTime2;
			time.toReadable(readableTime2);
			Assert::IsTrue(readableTime == readableTime2);
		}
		TEST_METHOD(Instatiate_event_class)
		{
			EventCar exampleEvent({ {"Car1"},[] {

			} });
			EventCar exampleEvent2({ {"Car1"},[] {

			} });
			Assert::IsTrue(exampleEvent == exampleEvent2);
		}
		TEST_METHOD(parking_place_is_same_only_by_name)
		{
			ParkingPlace place = { "Place1", {"GREEN"}, {12} };
			ParkingPlace place2 = { "Place1", {"GREEN"}, {1123} };
			Assert::IsTrue(place == place2);
		}
		TEST_METHOD(Money_just_two_decimals)
		{
			Money money1 = { 1.234 };
			Money money2 = { 1.2345 };
			money1.approximate();
			money2.approximate();
			Assert::IsTrue(money1 == money2);
		}
		TEST_METHOD(Identifier_object)
		{
			Identifier id1;
			Identifier id2 = id1;
			Identifier id3;
			Assert::IsTrue(id3.v == id1.v + 1);
		}
		TEST_METHOD(EventCar_same_if_different_funcions)
		{
			int x;
			EventCar name{ "123" };
			EventCar name2{ "123" , [&x]() {
				int x1 = 1232;
				x = x1;
			} };
			name2.execution();
			Assert::IsTrue(x == 1232 and name == name2);
		}
		TEST_METHOD(Pricing_table_add_all_day_price)
		{
			Zone zone{ "ZONA" };
			PricingTable pricingTable;

			pricingTable.addPrice(zone, { 20.0001 });
			DayTime dayTime{ { {{1},{0}} } };
			Assert::IsTrue(pricingTable.table[{zone, dayTime}].v == 20);
		}
		TEST_METHOD(Pricing_table_add_selected_in_day_price)
		{
			Zone zone{ "ZONA" };
			PricingTable pricingTable;
			DayTime begin{ { {{2},{0}} } };
			DayTime end{ { {{3},{0}} } };
			pricingTable.addPrice(zone, { 20.0001 });
			pricingTable.addPrice(zone, { 30.0001 }, begin, end);

			DayTime dayTime1{ { {{1},{0}} } };
			DayTime dayTime2{ { {{2},{0}} } };
			DayTime dayTime3{ { {{2},{15}} } };

			Assert::IsTrue(pricingTable.table[{zone, dayTime1}].v == 20 and pricingTable.table[{zone, end}].v == 20 and pricingTable.table[{zone, dayTime2}].v == 30 and pricingTable.table[{zone, dayTime3}].v == 30);
		}
		TEST_METHOD(Parking_One_Car)
		{
			ParkingPlace place = { "Place1", {"PURPLE"}, {12} };
			Zone zone{ "ZONA" };
			ParkingManager parkingManager;
			ReadableTime timeNow({ "03 05 2021 12:00:00" });
			ReadableTime timeStopSimulation({ "03 05 2021 22:00:00" });
			Time stop;
			stop.fromReadable(timeStopSimulation);
			parkingManager.actualTime.fromReadable(timeNow);
			parkingManager.placeZone[place] = zone;
			parkingManager.pricingTable.addPrice(zone, { 20.0001 });
			Car car{ "car1" };
			ReadableTime readableTimeStartParking({ "03 05 2021 13:00:00" });
			ReadableTime readableTimeStopParking({ "03 05 2021 14:15:00" });
			Time timeStartParking;
			timeStartParking.fromReadable(readableTimeStartParking);
			Time timeStopParking;
			timeStopParking.fromReadable(readableTimeStopParking);
			parkingManager.safeParkingCar(place, car, timeStartParking, timeStopParking);
			parkingManager.timeForwardUntil(stop);
			Assert::IsTrue(parkingManager.wallet.v == 25);
		}
		TEST_METHOD(Not_Parking_One_Car_bypass_time_limit)
		{
			ParkingPlace place = { "Place1", {"ORANGE"}, {1} };
			Zone zone{ "ZONA" };
			ParkingManager parkingManager;
			ReadableTime timeNow({ "03 05 2021 12:00:00" });
			ReadableTime timeStopSimulation({ "03 05 2021 22:00:00" });
			Time stop;
			stop.fromReadable(timeStopSimulation);
			parkingManager.actualTime.fromReadable(timeNow);
			parkingManager.placeZone[place] = zone;
			parkingManager.pricingTable.addPrice(zone, { 20.0001 });
			Car car{ "car1" };
			ReadableTime readableTimeStartParking({ "03 05 2021 13:00:00" });
			ReadableTime readableTimeStopParking({ "03 05 2021 14:15:00" });
			Time timeStartParking;
			timeStartParking.fromReadable(readableTimeStartParking);
			Time timeStopParking;
			timeStopParking.fromReadable(readableTimeStopParking);
			parkingManager.safeParkingCar(place, car, timeStartParking, timeStopParking);
			parkingManager.timeForwardUntil(stop);
			Assert::IsTrue(parkingManager.wallet.v == 0);
		}
		TEST_METHOD(Parking_One_Car_with_different_time_prices)
		{
			ParkingPlace place = { "Place1", {"PURPLE"}, {12} };
			Zone zone{ "ZONA" };
			ParkingManager parkingManager;
			ReadableTime timeNow({ "03 05 2021 12:00:00" });
			ReadableTime timeStopSimulation({ "03 05 2021 22:00:00" });
			Time stop;
			stop.fromReadable(timeStopSimulation);
			parkingManager.actualTime.fromReadable(timeNow);
			parkingManager.placeZone[place] = zone;
			parkingManager.pricingTable.addPrice(zone, { 20.0001 });
			parkingManager.pricingTable.addPrice(zone, { 40.0001 }, { { {{14},{0}} } }, { { {{15},{0}} } });
			Car car{ "car1" };
			ReadableTime readableTimeStartParking({ "03 05 2021 13:00:00" });
			ReadableTime readableTimeStopParking({ "03 05 2021 14:15:00" });
			Time timeStartParking;
			timeStartParking.fromReadable(readableTimeStartParking);
			Time timeStopParking;
			timeStopParking.fromReadable(readableTimeStopParking);
			parkingManager.safeParkingCar(place, car, timeStartParking, timeStopParking);
			parkingManager.timeForwardUntil(stop);
			Assert::IsTrue(parkingManager.wallet.v == 30);
		}
		TEST_METHOD(Parking_One_Car_and_message_exists)
		{
			ParkingPlace place = { "Place1", {"PURPLE"}, {12} };
			Zone zone{ "ZONA" };
			ParkingManager parkingManager;
			ReadableTime timeNow({ "03 05 2021 12:00:00" });
			ReadableTime timePauseSimulation({ "03 05 2021 14:05:00" });
			ReadableTime timeStopSimulation({ "03 05 2021 22:00:00" });
			Time pause;
			Time stop;
			pause.fromReadable(timePauseSimulation);
			stop.fromReadable(timeStopSimulation);
			parkingManager.actualTime.fromReadable(timeNow);
			parkingManager.placeZone[place] = zone;
			parkingManager.pricingTable.addPrice(zone, { 20.0001 });
			Car car{ "car1" };
			ReadableTime readableTimeStartParking({ "03 05 2021 13:00:00" });
			ReadableTime readableTimeStopParking({ "03 05 2021 14:15:00" });
			Time timeStartParking;
			timeStartParking.fromReadable(readableTimeStartParking);
			Time timeStopParking;
			timeStopParking.fromReadable(readableTimeStopParking);
			parkingManager.safeParkingCar(place, car, timeStartParking, timeStopParking);
			parkingManager.timeForwardUntil(pause);
			Assert::IsTrue(parkingManager.messageEvent.size() == 1);
		}
		TEST_METHOD(Parking_One_Car_and_reply_yes_to_message)
		{
			ParkingPlace place = { "Place1", {"PURPLE"}, {12} };
			Zone zone{ "ZONA" };
			ParkingManager parkingManager;
			ReadableTime timeNow({ "03 05 2021 12:00:00" });
			ReadableTime timePauseSimulation({ "03 05 2021 14:05:00" });
			ReadableTime timeStopSimulation({ "03 05 2021 22:00:00" });
			Time pause;
			Time stop;
			pause.fromReadable(timePauseSimulation);
			stop.fromReadable(timeStopSimulation);
			parkingManager.actualTime.fromReadable(timeNow);
			parkingManager.placeZone[place] = zone;
			parkingManager.pricingTable.addPrice(zone, { 20.0001 });
			Car car{ "car1" };
			ReadableTime readableTimeStartParking({ "03 05 2021 13:00:00" });
			ReadableTime readableTimeStopParking({ "03 05 2021 14:15:00" });
			Time timeStartParking;
			timeStartParking.fromReadable(readableTimeStartParking);
			Time timeStopParking;
			timeStopParking.fromReadable(readableTimeStopParking);
			parkingManager.safeParkingCar(place, car, timeStartParking, timeStopParking);
			parkingManager.timeForwardUntil(pause);
			long long lastID = ID - 1;
			Identifier id(lastID);
			parkingManager.messageEvent[id].accept();

			parkingManager.timeForwardUntil(stop);
			Assert::IsTrue(parkingManager.wallet.v == 30);
		}
		TEST_METHOD(Parking_One_Car_and_reply_yes_to_message_two_times)
		{
			ParkingPlace place = { "Place1", {"PURPLE"}, {12} };
			Zone zone{ "ZONA" };
			ParkingManager parkingManager;
			ReadableTime timeNow({ "03 05 2021 12:00:00" });
			ReadableTime timePause1Simulation({ "03 05 2021 14:05:00" });
			ReadableTime timePause2Simulation({ "03 05 2021 14:20:00" });
			ReadableTime timeStopSimulation({ "03 05 2021 22:00:00" });
			Time pause1;
			Time pause2;
			Time stop;
			pause1.fromReadable(timePause1Simulation);
			pause2.fromReadable(timePause2Simulation);
			stop.fromReadable(timeStopSimulation);
			parkingManager.actualTime.fromReadable(timeNow);
			parkingManager.placeZone[place] = zone;
			parkingManager.pricingTable.addPrice(zone, { 20.0001 });
			Car car{ "car1" };
			ReadableTime readableTimeStartParking({ "03 05 2021 13:00:00" });
			ReadableTime readableTimeStopParking({ "03 05 2021 14:15:00" });
			Time timeStartParking;
			timeStartParking.fromReadable(readableTimeStartParking);
			Time timeStopParking;
			timeStopParking.fromReadable(readableTimeStopParking);
			parkingManager.safeParkingCar(place, car, timeStartParking, timeStopParking);
			parkingManager.timeForwardUntil(pause1);
			long long lastID = ID - 1;
			Identifier id(lastID);
			parkingManager.messageEvent[id].accept();
			parkingManager.timeForwardUntil(pause2);
			id.v += 1;
			parkingManager.messageEvent[id].accept();
			parkingManager.timeForwardUntil(stop);

			Assert::IsTrue(parkingManager.wallet.v == 35);
		}
	};
}
