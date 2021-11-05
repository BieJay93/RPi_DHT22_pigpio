/*
#	-------------------------------------------------------
#	RASPBERRY PI DHT22 SIMPLE USAGE EXAMPLE
#	
#	Simple example code for reading DHT22 sensor data 
#
#	build with:
#		g++ -Wall simpleUsage.cpp -o example -lpigpio
#	-------------------------------------------------------
*/


#include <iostream>
#include <unistd.h>
#include <pigpio.h>

#include "dht22.cpp"

int main()
{
	// initialize the pigpio library
	gpioInitialise();

	int dhtpin = 4;

	// there are two methods of using this lib
	// 1: use the get-functions:

	std::cout << "Humidity: " << DHT22_getHum(dhtpin) << "%" << std::endl;
	std::cout << "Temperature: " << DHT22_getTemp(dhtpin) << "°C" << std::endl;;

	// 2: use the variables directly :
	// (faster beacause it reads the sensor data only once)

	DHT22_read(dhtpin);
	std::cout << "Humidity: " << DHT22_humid << "%" << std::endl;
	std::cout << "Temperature: " << DHT22_tempC << "°C" << std::endl;;

	//close pigpio
    gpioTerminate();

    return 0;
}