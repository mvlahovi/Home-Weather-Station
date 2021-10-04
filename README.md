

# Home Weather Station

Home Weather Station  is a do-it-yourself weather station. The goal of the project is to monitor, store and visualize data, and send it to your server as well as  Thingspeak. The second goal was to be as cheap as possible. Over the past ten years it has evolved from a small project to this one.

![Home Weather Station](https://github.com/mvlahovi/Home-Weather-Station/blob/main/Resources/IndoorUnit2.jpg)

## Weather data

* wind speed (m/s)
* wind direction (angle)
* wind gust (m/s)
* wind gust direction (angle)
* cumulative wind run (m/day) 
* rain (mm)
* rain rate (mm/h)
* outdoor temperature (Cº)
* absolute atmospheric pressure (Pascal)
* relative outdoor humidity (%)
* indoor temperature (Cº)
* relative indoor humidity (%)
* solar radiation (W/m2)
* UV radiation
* Evapotranspiration (mm/h)

![Home Weather Station](https://github.com/mvlahovi/Home-Weather-Station/blob/main/Resources/Sensors2.jpg)

## Brief description of the work

After simple processing of sensor data, they are continuously sent trough the nRF24L01 wireless module 
and directional antennas, towards the indoor unit.

![Home Weather Station](https://github.com/mvlahovi/Home-Weather-Station/blob/main/Resources/Slide2.JPG)

The indoor unit receives data via the antenna and the nRF24L01 wireless module and refreshes the display .
Every 15 minutes the data collected in that 15 minute period are processed and saved. 
Data processing involves identifying and removing sporadic outliers.
Basic data is entered in the EEPROM for statistics and graphs and everything is written to the SD card 
in the form of raw data and a monthlylog file identical to the monthly file in Cumulus software. 
Also part of this data sends to the ThingSpeak channel.

![Home Weather Station](https://github.com/mvlahovi/Home-Weather-Station/blob/main/Resources/Slide1.JPG)

As the file on the SD card is compatible with the Cumulus monthly file, all data can be imported 
in Cumulus software itself which is free.

All files can be loaded on a PC via a UDP connection by sending the appropriate commands:
'list' - displays files and folders on the sd card
'read /(corresponding path)' - send the appropriate file over UDP
'send' - send real time data compatible with EasyWeatherData file.

There are also some functions that take place every hour, once a day and once a month.
The weather forecast is updated every 3 hours by calling from www.yr.no as an .xml file 
(no api key required) with corresponding location data.
The file is parsed and the necessary data is extracted and displayed graphically.

The exact time is checked once a day from the NTP service and corrected if necessary.
The firmware can be loaded via wireless and ArduinoOTA.

Software on a PC written in python tkinter, loads data from the indoor unit via udp connection,
processes all data, removes outliers, interpolates missing data and saves it in MySQL database.
The software has a graphical interface with graphical display of data.

## Motivation and goals
Before the current approach I have programed and tested other version like: AVRgcc before arduino, arduino mega32, mega2560 integrated with sd/microsd modules, etc etc etc and finally the current approach turned out to be the most effective for my goals.
In the end, I got everything working properly. Here you can see the weather station in action.

### Affordable 
I tried to make the price as low as possible but without loss of accuracy.

### Simple and repeatable to build
In this version I used a graphic display with 5V power supply and with 15 control lines which I would not recommend to anyone. So I don’t give a schematic diagram.
The display works properly, but with a complicated interface.

## Power loss 
Electricity goes away many times, specially during storms. Outdoor unit that is powered by a 30W solar cell and a 15 Ah battery which is enough for a few days. The indoor unit is powered by a computer 480W uninterruptible power supply UPS which is enough for a few hours.
In future versions will be incorporate battery voltage measurement and low voltage warning.

## Software stability
Arduino software is really robust, and in several years of work there were only 2-3 cases of program freezing.
Many pieces of code could be rewritten and greatly improved because the part was written at the beginning of my programming. (I am not a programmer by profession).

## Data visualization and diagnosis
Data visualization and diagnosis is built-in, but many improvements could be made.
In future version better LCD with touch screen or E-ink dispay will be incorporate.

## Data log and storage 
As mentioned before, part of data is stored in Eeprom, but all data is stored on sd card.
First as raw data and then as a record in monthly log file compatible with the Cumulus format.

## Libraries used

 ### Indoor unit
 (Arduino library)

 * T6963.h 	-> functions  for display
 * Arial__8.h	-> font size 8 
 * Arial__6.h	-> font size 6  
 * Arial__14.h	-> font size 14  

 * FS.h 		 -> for SD card
 * SD.h  		 -> for SD card
 * SPI.h 		 -> for SPI communication

 * RTClib.h 	 -> for real time clock
 * Adafruit_BME280.h 	 -> for BME280
 * ForecastYR.h 	 -> for a three-day weather forecast from www.yr.no
 * nRF24L01.h 	 -> for nRF24L01
 * RF24.h 	 -> for nRF24L01

 * WiFi.h	 -> for WiFi
 * WiFiUdp.h 	 -> for WiFi komunikaciju s PC
 * ArduinoOTA.h 	 -> for upload firmware
 * HTTPClient.h 	-> HTTPClient
 * Meteofunc/astro.c 	-> various functions
 * Meteofunc/commands.c 	-> various communication functions
 * Adafruit_EEPROM_I2C.h 	 -> for I2C
 * Evapotranspiration.h 	 -> for evapotranspiration calculation
 * ThingSpeak.h 		 -> for ThingSpeak 

==================================================================
### Outdoor unit
 (AVRgcc library)

 * LiquidCrystal.h	//for character display
 * SPI.h 
 * nRF24L01.h
 * RF24.h
 * Wire.h 		//needed for I2C
 * SimpleTimer.h 
 * SHT1x.h		//for SHT75
 * OneWire.h		// for ds18d20
 * DallasTemperature.h  	// for ds18d20

![Home Weather Station](https://github.com/mvlahovi/Home-Weather-Station/blob/main/Resources/OutdoorUnit2.jpg)

# Python software

Arduino module connects to an python software by UDP connection. 

![Home Weather Station](https://github.com/mvlahovi/Home-Weather-Station/blob/main/Resources/MeteoGUI2.jpg)



# Schematics diagram

One of the problems was that you cannot use certain GPIO when using the WiFi functionality of the ESP32. 
Refer to RandomNerdTutorials for a lot of info on connecting peripherals to the ESP32. 


# Author
* **Marinko Vlahovic** <mailto:mlahovi1@gmail.com>

# Notes
The project is still valid and in use. Several people around the world have implemented it successfuly and I thank those who had send me some greetings and photos about their implementation.
