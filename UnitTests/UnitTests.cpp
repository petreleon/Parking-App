#include "pch.h"
#include "CppUnitTest.h"
#include "../Parking2/time.hpp"
#include "../Parking2/ReadableTime.hpp"
#include "../Parking2/EventCar.hpp"
#include "../Parking2/ParkingPlace.hpp"
#include "../Parking2/Money.hpp"
#include "../Parking2/Identifier.hpp"
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
	};
}
