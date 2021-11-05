#define DHT22_DEBUG         0		// 1 = display debug messages
#define DHT22_MAXRETRIES    5		// in the rare case, that a reading fails

uint8_t 	DHT22_Lock = 0;

int 		DHT22_data[5];
uint8_t 	DHT22_bits = 0;

uint8_t 	DHT22_readDone = 0;
uint32_t 	DHT22_lastUp = 0;

float DHT22_tempC, DHT22_humid;


void DHT22_cb(int pin, int lvl, uint32_t tick)
{
	// just to be sure that no timeout occurred
	if(lvl < 2) 
	{
		// first edge has to be a falling edge (lvl = 1)
		// if not, we missed it, so skip one step
		if(DHT22_bits == 0 && lvl == 1)
		{
			DHT22_bits++;
		}

		// ignore the first four edges
		// and calculate all bits on falling edge
		else if(DHT22_bits > 4 && DHT22_bits < 85) 
		{
			//rising edge, start time measurement
			if(lvl == 1)
			{
				DHT22_lastUp = tick;
			}

			if(lvl == 0)
			{
				DHT22_data[ (DHT22_bits-5)/16 ] <<= 1;
			
				// calculate the passed time since the last edge up
				// greater than 70us means bit is 1 (68us here for tolerance)
				if( (tick - DHT22_lastUp) >= 68)
				{
					DHT22_data[ (DHT22_bits-5)/16 ] |= 1;
				}
			}

		}
		
		else if(DHT22_bits >= 85)
		{
			DHT22_readDone = 1;
		}
		DHT22_bits++;
	}

}

int DHT22_cycle(int pin)
{
	// only read one sensor per time
	while(DHT22_Lock == 1)
	{
		usleep(100000);
	}
	DHT22_Lock = 1;

	DHT22_data[0] = DHT22_data[1] = DHT22_data[2] = DHT22_data[3] = DHT22_data[4] = 0;
	DHT22_bits = 0;
	DHT22_readDone = 0;

	gpioSetMode(pin, PI_OUTPUT);
	gpioWrite(pin,1);
	usleep(5000);

	gpioWrite(pin,0);
	usleep(5000);

	gpioSetMode(pin,PI_INPUT);
	gpioSetPullUpDown(pin, PI_PUD_UP);

	gpioSetAlertFunc(pin, DHT22_cb);

	
	uint8_t timeout = 0;

	while(DHT22_readDone != 1 && timeout < 100)
	{
		usleep(1000);
		timeout++;
	}
	
	if(DHT22_readDone != 1)
	{
		if(DHT22_DEBUG == 1)
		{
			std::cout << "DHT22: ERROR at Cycle " << (int)DHT22_bits << std::endl;
		}
		
		gpioSetAlertFunc(pin, NULL);
		DHT22_Lock = 0;
		return 0;
	}
	else if(DHT22_readDone == 1)
	{
		// check checksum
		if( (DHT22_data[4] != ((DHT22_data[0] + DHT22_data[1] + DHT22_data[2] + DHT22_data[3]) & 0xFF)))
		{
			if(DHT22_DEBUG == 1)
			{
				std::cout << "DHT22: ERROR checksum missmatch" << std::endl;
				std::cout << "DHT22: DATA " << std::to_string(DHT22_data[0]) << " " << std::to_string(DHT22_data[1]) << " " << std::to_string(DHT22_data[2]) << " " << std::to_string(DHT22_data[3]) << " " << std::to_string(DHT22_data[4]) << " " << std::endl;
			}
			gpioSetAlertFunc(pin, NULL);
			DHT22_Lock = 0;
			return 0;
		}
	}
	
	gpioSetAlertFunc(pin, NULL);
	DHT22_Lock = 0;
	return 1;
}

void DHT22_read(int pin)
{
	for(int i = 0; i < DHT22_MAXRETRIES; i++)
	{
		if(DHT22_cycle(pin) == 1)
		{
			break;
		}
		usleep(500000);
	}

	// decode and output the sensor data
	if(DHT22_data[2] >= 128)
	{
		DHT22_tempC = (((float)DHT22_data[2] - 128) * 256 + (float)DHT22_data[3]) / -10;
	}
	else 
	{
		DHT22_tempC = ((float)DHT22_data[2] * 256 + (float)DHT22_data[3]) / 10;
	}
	
	DHT22_humid = ((float)DHT22_data[0] * 256 + (float)DHT22_data[1]) / 10;

	if(DHT22_DEBUG == 1)
	{
		std::cout << DHT22_humid << "% Luftfeuchtigkeit" << std::endl;
		std::cout << DHT22_tempC << "°C" << std::endl;
	}
}

float DHT22_getTemp(int pin)
{
	DHT22_read(pin);
	return DHT22_tempC;
}
float DHT22_getHum(int pin)
{
	DHT22_read(pin);
	return DHT22_humid;
}