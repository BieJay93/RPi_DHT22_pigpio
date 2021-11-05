/*
#	-------------------------------------------------------
#	RASPBERRY PI DHT22 THREADED EXAMPLE
#	
#	Example code for reading DHT22 sensor data from a subthread
#
#	build with:
#		g++ -Wall threadedUsage.cpp -o example -lpigpio - lpthread
#	-------------------------------------------------------
*/


#include <iostream>
#include <thread>
#include <unistd.h>
#include <pigpio.h>

#include "dht22.cpp"

uint8_t running = 1;


void* threadDHT(void* pin)
{
	// read the sensor data every 5 seconds
	while(running)
	{
		DHT22_read(*static_cast<int*>(pin));
		std::cout << "Humidity: " << DHT22_humid << "%" << std::endl;
		std::cout << "Temperature: " << DHT22_tempC << "°C" << std::endl;;
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	std::cout << "Thread ended" << std::endl;
}



int main()
{

	// initialize the pigpio library
	gpioInitialise();

	int dhtpin = 4;

	// create a new thread and start the sensor reading in background
	pthread_t* thread1 = gpioStartThread(threadDHT, &dhtpin);

	// run until user quits
	std::string cinStr;
	while (true)
	{
		std::getline(std::cin, cinStr);
		if(cinStr == "q")
		{
			running = 0;
			break;
		}
	}

	// stop the subthread
	gpioStopThread(thread1);

	//close pigpio
    gpioTerminate();

    return 0;
}