#include <Arduino.h>
#include <stdio.h>
#include "T6963.h"
#include "Arial__8.h"
#include "Arial__6.h"
#include "Arial__14.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "RTClib.h"
#include "Adafruit_BME280.h"
#include "ForecastYR.h"
#include "nRF24L01.h"
#include "RF24.h"

#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include "Meteofunc/astro.c"
#include "Meteofunc/commands.c"
#include "Adafruit_EEPROM_I2C.h"
#include "Evapotranspiration.h"
#include "ThingSpeak.h"

////////////////declaration func/////////////////
void osnovnaPozadina(void);
void Add_average_smjer(void);
unsigned char Count_average_smjer(int xr, int yr);
void Write_to_data_month_file(void);
void WriteThingspeakChannel(void);

#define RECORD_TIME 15
#define TRESHOLD_SOLAR	70	//za total sun hour
float tip_bucket_faktor = 0.077;	// 1 tip = 0,077 l	 13 tips = 1 l/m2

struct Sensor_status_type
{
	unsigned char SHT;
	unsigned char DS18D20sol_sensor;
	unsigned char MPX_sensor;
	unsigned char Wind_smjer_sensor;
	unsigned char Wind_speed_sensor;
	unsigned char Rain_sensor;
	unsigned char Uv_sensor;
};

struct Error_status_type
{
	unsigned char Display;
	unsigned char NoSDcard;
	unsigned char InitSDcard;
	unsigned char OpenFileSDcard;
	unsigned char CreateFileSDcard;
	unsigned char WriteSDcard;
	unsigned char RTC;
	unsigned char RF24;
	unsigned char DS18D20in;
	bool Base;	// if last record not saved this is true
	struct	Sensor_status_type Sensor;
};

enum velicina {
	eVrijeme,
	eTemperatura,
	eTlak,
	eVlaga,
	eSolar_temp,
	eMax_brz,
	ePros_brz,
	ePadaline,	//  reset na 100
	eSmjer
};


struct Error_status_type Error;	//error structure

const char* ssid = "**********";	//insert your data
const char* password = "********";
IPAddress local_IP(192, 168, ***, ***);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

////////////////////////TCP Com////////////////////////////////
unsigned int localUdpComPort = 4210;  // local port to listen on
char incomingPacket[30];  // buffer for incoming packets
const uint ServerPort = 23;
WiFiServer Server(ServerPort);
WiFiClient RemoteClient; 
/**********STRINGS TO USE****************************/
const char prog_days[] PROGMEM = "NePoUtSrCePeSu";
const char  prog_text_smjer[] PROGMEM = " N NNEEEN E EESSSE S SSWWWS W WWNNNW";
char text_smjer[] = "txt";  // This is a holder for text
char _str[20];	//for coversions
char comma[] = ";";
char LF[] = "\n";
char CR[] = "\r";
uint64_t cardSize;

T6963 LCD(240,128,6,32);// 240x128 Pixel and 6x8 Font and 32k
/***********CG RAM caracter*********/
const unsigned char char_delta[]={0x00,0x00,0x04,0x0A,0x11,0x1F,0x00,0x00};
const unsigned char char_stupanj[]={0x00,0x04,0x0A,0x04,0x00,0x00,0x00,0x00};
const unsigned char char_in[]={0x00,0x00,0x10,0x00,0x17,0x15,0x15,0x00};
const unsigned char char_hPa1[]={0x00,0x00,0x11,0x11,0x1D,0x15,0x15,0x00};
const unsigned char char_hPa2[]={0x00,0x00,0x30,0x17,0x35,0x07,0x05,0x00};
const unsigned char char_antena[]={0x15,0x0E,0x04,0x04,0x04,0x04,0x04,0x00};
const unsigned char char_C[]={0x18,0x24,0x20,0x20,0x20,0x24,0x18,0x00};
const unsigned char char_06h[]={0x00,0x04,0x0C,0x1C,0x1F,0x0E,0x04,0x00};
const unsigned char char_24h[]={0x00,0x04,0x0E,0x1F,0x1F,0x0E,0x04,0x00};
const unsigned char char_12h[]={0x00,0x04,0x0C,0x1C,0x1C,0x0C,0x04,0x00};
const unsigned char char_gore[]={0x00,0x04,0x0E,0x15,0x04,0x04,0x04,0x00};
const unsigned char char_goredesno[]={0x00,0x0F,0x03,0x05,0x09,0x10,0x00,0x00};
const unsigned char char_desno[]={0x00,0x04,0x02,0x1F,0x02,0x04,0x00,0x00};
const unsigned char char_doledesno[]={0x00,0x10,0x09,0x05,0x03,0x0F,0x00,0x00};
const unsigned char char_dole[]={0x00,0x04,0x04,0x04,0x15,0x0E,0x04,0x00};
#define gore 10
#define goredesno 11
#define desno 12
#define doledesno 13
#define dole 14
/********************Small font*************************/
prog_char Small_font[] PROGMEM = {	
	0x01,0x10,0x00,0x00,	//  .
	0x02,0x18,0x06,0x00,	//  /
	0x03,0x0C,0x12,0x0C,	//  0
	0x02,0x04,0x1E,0x00,	//	1
	0x02,0x1A,0x16,0x00,	//  2
	0x03,0x11,0x15,0x0A,	//  3
	0x02,0x0E,0x18,0x00,	//  4
	0x02,0x16,0x1A,0x00,	//  5
	0x03,0x0E,0x15,0x18,	//	6
	0x02,0x1A,0x06,0x00,	//  7
	0x03,0x0A,0x15,0x0A,	//  8
	0x03,0x02,0x15,0x0E,	//  9
	0x01,0x14,0x00,0x00 	//  :
};
//////////////////////////////////////////////////////
const char prog_months[] PROGMEM = "sijvljozutrasvilipsrpkolrujlisstupro";

#define RECORD_TIME 15
#define VEL_EEPROM_RECORDA	18
#define EEPROM_ADDR 0x50  // the default address i2c memory
#define MAX_EEPROM_RECORD (24*(60/RECORD_TIME)-1)	//24 hour * 60 min/record_time
#define EEP_BASE_TABLE		((MAX_EEPROM_RECORD+1)*VEL_EEPROM_RECORDA+10)//1738
#define EEP_ADDR_tek_record	4000
#define EEP_ADDR_tip_bucket_factor	4002	// u mililitrima
#define EEP_ADDR_lcd_brightness	4004		//u postotcima
#define EEP_ADDR_lcd_bl_time	4005		//u sekundama
#define EEP_ADDR_day_no_rain	4006		//u danima
#define EEP_ADDR_yesterday_rainfall	4007		//u tipovima
#define EEP_ADDR_monthly_rainfall	4009		//u tipovima
#define EEP_ADDR_yearly_rainfall	4011		//u tipovima
#define EEP_ADDR_last_rainfall	4013		//u minutama
#define EEP_ADDR_total_sun	4015				// 1h = 100
#define EEP_ADDR_rain_tbl_ptr		4017
#define EEP_ADDR_last_forecast_long	4019	// time to sec
#define EEP_ADDR_uiPadalineLast 4021	//padaline on last record


#define EEP_ADDR_last_rain_base	4050		// start rain table in l/m2 X 10
#define EEP_ADDR_last_rain_end	4108		// last data for 30 days
/********************Weather data********************/
float fCurrTemperature;
float fCurrPressure;
float fCurrSolar;
float fCurrHumidity;
float fCurrWindSpeed;
float fCurrRainfall;	// od 00h u l/h
float fCurrWindBearing;
float fCurrRainfallRate;
float fCurrUV;
float fCurrInsideTemperature;
float fCurrInsideHumidity;
float fInsidePressure;
float frain24h;	// in last 24h u l/h
uint8_t ucRecordWindGust;
uint8_t ucRecordSmjer;	// :2
uint16_t uiRecordWindAverage;	//x10
float fEvth_global;	// u mm/h
uint32_t ulTotalWindCounter;
uint32_t ulCounter2secProlaz;
uint32_t ulWindCounter;

uint8_t ucRecordRainRainrate;
uint16_t uiPadalineLast;
uint16_t uiRecordPadaline;
uint16_t MeteoRecordPadalineLOC;
uint8_t ucNoRain;	// u days
int16_t rain_tbl_ptr;
uint32_t ulLastForecastTime;
uint16_t uiMonthlyRain;

/********** Wind Direction Storage *************/
const uint8_t Smjer_tocke[] = {

	226,	26,	233,	30,	233,	22,	//E
	224,	33,	228,	39,	232,	32,	//120
	220,	37,	219,	45,	226,	41,	//150
	213,	39,	209,	46,	217,	46,	//S
	207,	37,	200,	41,	207,	45,	//210
	202,	33,	194,	32,	198,	39,	//240
	200,	26,	193,	22,	193,	30,	//W
	202,	20,	198,	13,	194,	20,	//300
	207,	15,	207,	7,	200,	11,	//330
	213,	13,	217,	6,	209,	6,	//N
	220,	15,	226,	11,	219,	7,	//30
	224,	20,	232,	20,	228,	13	//60
};
uint8_t ucLastSmjer=30;
uint8_t ucLastMinSmjer;
int16_t xrez,yrez;
/*******************************ForecastYR***************************/
char line[RL_MAX_CHARS];
int sizeof_line = RL_MAX_CHARS;
int cIndex;
Parser parser(line,sizeof_line);
prognoza_st prognoza_structura;
prognoza_st prognoza_konacna_structura;
prognoza_st *p_prognoza;
prognoza_st *p_prognoza_konacna;
int line_number;
int korak,progn; 
byte prognoza[BROJ_PROGNOZA][2];	
bool gotova_prognoza = false;
/**********************************************************/

Adafruit_EEPROM_I2C i2ceeprom;
Adafruit_BME280 bme; // use I2C interface

struct Meteo_record_type
{
	int Temperatura;		//temperatura x10
	int Tlak;		//tlak x10	pressure integer
	unsigned char Vlaga;
	int Solar_temp;			//temperature x10
	unsigned char Max_brz;	//ne koristi se u transferu
	unsigned char Pros_brz; //brzina u prosle  2 sekunde 
	unsigned int Padaline;	//dnevne padaline
	unsigned char Smjer;	//prosjecan smjer u stupnjevi/2
	unsigned char Rain_rate;	//l na sat   
	unsigned char Uv;
};
Meteo_record_type Meteo_record;
Meteo_record_type Meteo_record_last;
Meteo_record_type *p_struct;
uint16_t tek_record;
/*************RF24************************/
 RF24 radio(0,2);
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 
	0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
//uint8_t bufferout[sizeof(Meteo_record)]; //polja za prenos podataka rf24
char bufferin[14];  //veličina strukture ESP32 krivo izračuna
/******************************************/

RTC_DS1307 rtc;
DateTime now;
long timesec;		//for timers
long last2SecTime;
long lmidnightsec = 0;	// vrijeme zadnjeg ponoćnog recorda
bool timestart,uspjesnoOcitano;
uint8_t today;
uint8_t lastDay;
uint8_t lastMonth;
uint8_t lastMinute;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 0;	//znot used for now
byte menu_active = true;
byte menu = 0;
byte dayEvent = false;
byte monthEvent = false;
/*******************************************************************/
int prolaz_t = 0; //test
int prolaz_s = 0;
int prolaz_v = 0;
/******************Thingspeak***************************************/
unsigned long myChannelNumber = *********;	//insert your data
const char * myWriteAPIKey = "***************";
char longTime[24];//longTime = "2017-01-12 13:22:54";
// Field number macros
#define FIELDTEMP 1
#define FIELDHUM 2
#define FIELDPRESS 3
#define FIELDRAIN 4
#define FIELDWINDSP 5
#define FIELDWINDDIR 6
#define FIELDRAD 7
#define FIELDUV 8
/*******************************************************************/
int readPrognoza(fs::FS &fs, const char * path)
{
	cIndex = 0;
	line_number = 0;
	korak = 0;
   int i;
	 File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return -1;
    }
	p_prognoza = ( prognoza_st*) malloc(BROJ_UCITANIH_PROGNOZA * sizeof( prognoza_st));
	p_prognoza_konacna = ( prognoza_st*) malloc(BROJ_PROGNOZA  * sizeof( prognoza_st));
	Serial.println("File loading");	
   while (file.available() && gotova_prognoza == false) 
      {
         parse_char( file.read());
		}

	for ( i=0; i<6;i++)
	{
		prognoza[i][0]=( p_prognoza_konacna + i)->symbol;
		prognoza[i][1]=( p_prognoza_konacna + i)->sat;
		Serial.print(prognoza[i][0]);Serial.print(" ");Serial.print(prognoza[i][1]);
	}
   file.close();
	Serial.println("Forecast ready");
 	free( p_prognoza );
	free(p_prognoza_konacna);
	line_number = 0;
	return i;
}


int read_from_YR(fs::FS &fs, const char * path)
{
	File myFile;
	int status = 0;
	
	if (fs.exists(path))
	{
    Serial.println("Removing path");
    fs.remove(path);
	}
 	
	myFile = fs.open(path, FILE_WRITE);
	if(!myFile)
	{
		Serial.println("Failed to open file for writing");
		status = -2;
		return status;
	}
	if (WiFi.status() == WL_CONNECTED)
	{
      HTTPClient http;

      Serial.print("[HTTP] begin...\n");
        // configure server and url
		http.begin("https://www.yr.no/place/Croatia/Dubrovnik-Neretva/Dubrovnik/forecast.xml"); //Specify your URL
      Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
      int httpCode = http.GET();
		status = -1;
      if(httpCode == HTTP_CODE_OK) 
		{
          // get lenght of document (is -1 when Server sends no Content-Length header)
         int len = http.getSize();
          // create buffer for read
         uint8_t buff[128] = { 0 };
          // get tcp stream
         WiFiClient * stream = http.getStreamPtr();
          // read all data from server
         while(http.connected() && (len > 0 || len == -1)) 
			{
           // get available data size
            size_t size = stream->available();

            if(size) 
				{
               // read up to 128 byte
               int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                // write it to SD
               myFile.write(buff, c);
					//Serial.write(buff, c);
               if(len > 0)   len -= c;
            }
                    delay(1);
         }
         Serial.println();
         Serial.print("[HTTP] connection closed or file end.\n");
			status = 1;
      } 
		else 
		{
         Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
			status = -1;
      }
		http.end();
	}
	myFile.close();	
	return status;
}	

unsigned char glcd_print_small(unsigned char x,unsigned char y, char *in, prog_char * defFont)
{
	prog_char * defChar;
	char map,ch;
	unsigned char index,mask,i,j,with;
	const char with_space=2;
	
	while((map = *in++)) 
	{
		if (map == 32) 
		{
			for(i = 0; i < with_space ; i++)
			{
				for(j=0 ; j<6  ; j++)
				{   
					LCD.writePixel(  x+i , y+j, 0 );
				}//End i
			}// End j
			x+=with_space+1;	//space
		}
		else
		{
			if((map<46)||(map>58)) continue;
			else
				{
					if (map > 45) index=(map-46)*4;
					defChar = defFont + index;
					with = pgm_read_byte(defChar++);
					for( i = 0; i < with ; i++)
					{
						ch = pgm_read_byte(defChar++);

						mask = 0x01;
						for(j=0 ; j<6  ; j++)
						{   
							LCD.writePixel(  x+i , y+j, ch & mask );
							mask <<= 1;
						}//End i
					}// End j
					x+=with+1;
				}
		}
	}// End while
	return x;
}
///////////////////////////////////////////////////////////////////////////////
void updateSmjer(uint8_t prolaz)
{
	uint8_t pokaz;
	int x0,y0,x1,y1,x2,y2;
		for (uint8_t i = 0; i < prolaz; i++)
		{
			if(i) pokaz = ((Meteo_record.Smjer/15)*6);
			else pokaz = ((ucLastSmjer/15)*6);
			x0 = (int)Smjer_tocke[pokaz];
			y0 = (int)Smjer_tocke[pokaz+1];
			x1 = (int)Smjer_tocke[pokaz+2];
			y1 = (int)Smjer_tocke[pokaz+3];
			x2 = (int)Smjer_tocke[pokaz+4];
			y2 = (int)Smjer_tocke[pokaz+5];
			LCD.createLine( x0, y0, x1, y1, 2);		// 2 znaci XOR draw
			LCD.createLine( x0, y0, x2, y2, 2);
		}
	ucLastSmjer = Meteo_record.Smjer;
} 
///////////////////////////////////////////////////////////////////////////////////
void update_10min_Smjer(uint8_t min_smjer,uint8_t prolaz)
{
	unsigned char pokaz;
	int x0,y0,x1,y1,x2,y2,x,y;

	for (uint8_t i = 0; i<prolaz;i++)
	{
		if(i)pokaz = ((min_smjer/15)*6);
		else pokaz = ((ucLastMinSmjer/15)*6);
		x0 = (int)Smjer_tocke[pokaz];
		y0 = (int)Smjer_tocke[pokaz+1];
		x1 = (int)Smjer_tocke[pokaz+2];
		y1 = (int)Smjer_tocke[pokaz+3];
		x2 = (int)Smjer_tocke[pokaz+4];
		y2 = (int)Smjer_tocke[pokaz+5];
		x = round((x0+x1+x2)/3); 
		y = round((y0+y1+y2)/3);

		LCD.writePixel(x, y,2);
		LCD.writePixel(x-1, y,2);
		LCD.writePixel(x+1, y,2);
		LCD.writePixel(x, y-1,2);
		LCD.writePixel(x, y+1,2);
		LCD.writePixel(x-1, y-1,2);
		LCD.writePixel(x+1, y+1,2);
		LCD.writePixel(x-1, y+1,2);
		LCD.writePixel(x+1, y-1,2);
	}
	ucLastMinSmjer = min_smjer;
} 
///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
char Error_constrain(int value,const int min, const int max)
{
	int old_value;

	old_value=value;
	value = constrain(value, min, max);
	if (old_value==value) return 0;
	else
		return 1;
}
//////////////////////////////////////////////////////////////////////////////////
int16_t ReadEeprom(const enum velicina vel, uint16_t poz)
{
	return( i2ceeprom.read16(VEL_EEPROM_RECORDA * poz + vel*2));
}
///////////////////////////////////////////////////////////////////////
int lastdata(const enum velicina vel)
{
	unsigned int ptr;

	if (tek_record==0) ptr=MAX_EEPROM_RECORD;
	else ptr=tek_record-1;
	return(ReadEeprom(vel,ptr));
}
////////////////////////////////////////////////////////////////////////////////////
void  Test_data(void)
{
	/*if(abs(Meteo_record_last.Temperatura-Meteo_record.Temperatura)>15 && !!timestart) {
		Meteo_record.Temperatura=Meteo_record_last.Temperatura;
		Error.Sensor.SHT = 5;
	}*/ //ubacio u loop
	/*if(abs(Meteo_record_last.Vlaga-Meteo_record.Vlaga)>30 && !timestart) {
		Meteo_record.Vlaga=Meteo_record_last.Vlaga;
		Error.Sensor.SHT = 5;
	}*/
	/*if(abs(Meteo_record_last.Solar_temp-Meteo_record.Solar_temp)>50 && !timestart) {
		Meteo_record.Solar_temp=Meteo_record_last.Solar_temp;
		Error.Sensor.DS18D20sol_sensor = 5;
	}*/
	if(abs(Meteo_record_last.Tlak-Meteo_record.Tlak)>20 && !timestart) {
		Meteo_record.Tlak=Meteo_record_last.Tlak;
		Error.Sensor.MPX_sensor = 5;
	}
	else Meteo_record_last.Tlak = Meteo_record.Tlak;

	if((Meteo_record.Pros_brz-Meteo_record_last.Pros_brz)>40 && !timestart) {
		Meteo_record.Pros_brz=Meteo_record_last.Pros_brz;
		Error.Sensor.Wind_speed_sensor = 5;
	}
	else Meteo_record_last.Pros_brz = Meteo_record.Pros_brz;
	
	if((Meteo_record.Rain_rate-Meteo_record_last.Rain_rate)>50 && !timestart) {
		Meteo_record.Rain_rate=Meteo_record_last.Rain_rate;
		Error.Sensor.Rain_sensor = 5;
	}
	else Meteo_record_last.Rain_rate = Meteo_record.Rain_rate;

	if(abs(Meteo_record_last.Uv-Meteo_record.Uv)>20 && !timestart) {
		Meteo_record.Uv=Meteo_record_last.Uv;
		Error.Sensor.Uv_sensor = 5;
	}
	else Meteo_record_last.Uv = Meteo_record.Uv;
	timestart = false;
}

////////////////////////////////////////////////////////////////////////////////////
void Test_jump_data(void)
{
	bool time;
	if(abs(lastdata(eVrijeme)-((int)now.hour()*100 + (int)now.minute()/RECORD_TIME))< 100) time=true;
	else time = false;

	if(abs(lastdata(eTemperatura)-Meteo_record.Temperatura)>100 && time) {
		Meteo_record.Temperatura=lastdata(eTemperatura);
		Error.Sensor.SHT = 3;
	}
	if(abs(lastdata(eTlak)-(int)Meteo_record.Tlak)>50 && time) {
		Meteo_record.Tlak=lastdata(eTlak);
		Error.Sensor.MPX_sensor = 3;
	}
	if(abs(lastdata(eVlaga)-(int)Meteo_record.Vlaga)>40 && time) {
		Meteo_record.Vlaga=(unsigned char)lastdata(eVlaga);
		Error.Sensor.SHT = 3;
	}
	if(abs(lastdata(eSolar_temp)-Meteo_record.Solar_temp)>200 && time) {
		Meteo_record.Solar_temp=lastdata(eSolar_temp);
		Error.Sensor.DS18D20sol_sensor = 3;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void WriteEepromRecord(void)
{
	uint16_t address;
	address =tek_record*VEL_EEPROM_RECORDA;
	
	i2ceeprom.write16( address ,(int16_t)now.hour()*100 + (int16_t)now.minute()/RECORD_TIME);	//kodirano vrijeme
	address = address+2;
	i2ceeprom.write16( address ,(int16_t)Meteo_record.Temperatura);
	address = address+2;
	i2ceeprom.write16( address ,(int16_t)Meteo_record.Tlak);
	address = address+2;
	i2ceeprom.write16( address ,(int16_t)Meteo_record.Vlaga);
	address = address+2;
	i2ceeprom.write16( address ,(int16_t)Meteo_record.Solar_temp);
	address = address+2;
	i2ceeprom.write16( address ,(int16_t)ucRecordWindGust);	
	address = address+2;
	i2ceeprom.write16( address ,uiRecordWindAverage); 
	address = address+2;
	i2ceeprom.write16( address ,uiRecordPadaline);
	address = address+2;
	i2ceeprom.write16( address ,(int16_t)ucRecordSmjer);
	tek_record++;
	if(tek_record > MAX_EEPROM_RECORD) tek_record=0;
	i2ceeprom.write16(EEP_ADDR_tek_record, tek_record);
}

////////////////////////////////////////////////////////////////////////
int difhour (const enum velicina vel,unsigned char hour)
{
	int podatak;
	unsigned int delta ;
	unsigned int ptr_end;
	unsigned int ptr_start;

	delta = (60/RECORD_TIME)*(unsigned int)hour;
	if (tek_record==0) ptr_start=MAX_EEPROM_RECORD;
	else ptr_start=tek_record-1;
	if (ptr_start < delta) ptr_end = MAX_EEPROM_RECORD - (delta-ptr_start-2);
	else ptr_end = ptr_start - (delta-1);
	podatak=ReadEeprom(vel,ptr_start)-ReadEeprom(vel,ptr_end);
	return (podatak);
}
////////////////////////////////////////////////////////////////////////
int16_t sum24hour (const enum velicina vel)
{
	unsigned int ptr;
	int16_t data_int =0;

	ptr = tek_record;

	for(int in=0;in<96;in++)
	{		
		data_int= data_int + ReadEeprom(vel,ptr);
		ptr++;
		if(ptr > MAX_EEPROM_RECORD) ptr=0;
	}
	return(data_int);
}
////////////////////////////////////////////////////////////////////////
int16_t sumlasthour (const enum velicina vel)
{
	unsigned int ptr;
	int16_t data_int =0;

	if (tek_record==0) ptr=MAX_EEPROM_RECORD;
	else ptr=tek_record-1;

	for(int in=0;in<4;in++)
	{		
		data_int= data_int + ReadEeprom(vel,ptr);
		if(ptr == 0) ptr=MAX_EEPROM_RECORD;
		else ptr--;
	}
	return(data_int);
}
////////////////////////////////////////////////////////////////////////
void maxhour (const enum velicina vel,unsigned char hour, int *p_max, int *p_vrijeme)
{
	int podatak;
	unsigned int delta ;
	unsigned int ptr_end;
	unsigned int ptr_start;

	*p_max=-1000;

	delta = (60/RECORD_TIME)*(unsigned int)hour;
	if (tek_record==0) ptr_start=MAX_EEPROM_RECORD;
	else ptr_start=tek_record-1;
	if (ptr_start<delta) ptr_end=MAX_EEPROM_RECORD-(delta-ptr_start-2);
	else ptr_end=ptr_start-(delta-1);

	for (int index = 0; index < delta; index++)
	{
		podatak=ReadEeprom(vel,ptr_end);
			if(podatak>*p_max) {
				*p_max=podatak; 
				*p_vrijeme=ReadEeprom(eVrijeme,ptr_end);
			}
			ptr_end++;
		if (ptr_end > MAX_EEPROM_RECORD) ptr_end = 0;
	}
}
////////////////////////////////////////////////////////////////////////
void minhour (const enum velicina vel,unsigned char hour, int *p_min, int *p_vrijeme)
{
	int podatak;
	unsigned int delta ;
	unsigned int ptr_end;
	unsigned int ptr_start;

	*p_min=15000;

	delta = (60/RECORD_TIME)*(unsigned int)hour;
	if (tek_record==0) ptr_start=MAX_EEPROM_RECORD;
	else ptr_start=tek_record-1;
	if (ptr_start<delta) ptr_end=MAX_EEPROM_RECORD-(delta-ptr_start-2);
	else ptr_end=ptr_start-(delta-1);

	for (int index = 0; index < delta; index++)
	{
		podatak=ReadEeprom(vel,ptr_end);
			if(podatak < *p_min) {
				*p_min=podatak; 
				*p_vrijeme=ReadEeprom(eVrijeme,ptr_end);
			}
			ptr_end++;
		if (ptr_end > MAX_EEPROM_RECORD) ptr_end = 0;
	}
}
////////////////////////////////////////////////////////////////////////
void averagehour (const enum velicina vel,unsigned char hour, int *p_ave)
{
	int podatak;
	unsigned int delta ;
	unsigned int ptr_end;
	unsigned int ptr_start;
	unsigned int counter;

	*p_ave=0;
	podatak = 0;
	counter = 0;

	delta = (60/RECORD_TIME)*(unsigned int)hour;
	if (tek_record==0) ptr_start=MAX_EEPROM_RECORD;
	else ptr_start=tek_record-1;
	if (ptr_start<delta) ptr_end=MAX_EEPROM_RECORD-(delta-ptr_start-2);
	else ptr_end=ptr_start-(delta-1);

	for (int index = 0; index < delta; index++)
	{
		podatak = podatak + ReadEeprom(vel,ptr_end);
		ptr_end++;
		if (ptr_end > MAX_EEPROM_RECORD) ptr_end = 0;
		counter++;
	}
	if (counter) *p_ave = podatak / counter;
}

////////////////////////////////////////////////////////////////////////////////////////
void Draw_dnevni_graph(const enum velicina vel,unsigned char x0,unsigned char y0,unsigned char max_long,unsigned char max_high )
{//Draw_dnevni_graph(eTemperatura,3,120,100,36);
	float data, scale_y,data_max, data_min, data_ave;
	unsigned int ptr;
	int data_max_int, data_int, data_min_int, data_ave_int;
	int timel;
   int time_min = 0;
	int time_max = 0;
	unsigned int t;
	unsigned char in, x, y;
	unsigned char  razlika = 0;
	unsigned char  y_last = 0;
	unsigned char  ys = 0;
	unsigned char x_pomak = 1;
	const unsigned char y_offset_dole = 2;
	const unsigned char y_offset_gore = 4;
	const unsigned char x_offset_lijevo=2;
	bool dot=false;

	unsigned char duljina=0;
	unsigned char duljinalast=0;
	unsigned char krajX=0;
	const unsigned char TEXT_HIGH=8;

	ptr = tek_record;
	LCD.drawrectbyte(x0,y0-max_high,max_high,max_long/6 + 2, 0x00);//LCD.fillRect(x0, y0-max_high, x0+max_long, y0,0);
	
	data_int = ReadEeprom(vel,ptr);
	data_max_int=data_int; data_min_int=data_int;
	data_ave_int = 0;
	for(in=0;in<96;in++)
	{		
		data_int=ReadEeprom(vel,ptr);
		if(data_int > data_max_int) {data_max_int=data_int; time_max=ptr;}
		if(data_int < data_min_int) {data_min_int=data_int; time_min=ptr;}
		data_ave_int = data_ave_int + data_int;
		ptr++;
		if(ptr > MAX_EEPROM_RECORD) ptr=0;
	}
	data_ave_int = data_ave_int/96;
	if(vel==eTemperatura || vel==eTlak || vel==eSolar_temp || vel==ePros_brz) 
		{
			data_max=((float)data_max_int)/10.0f;
			data_min=((float)data_min_int)/10.0f;
			data_ave=((float)data_ave_int)/10.0f;
		}
		else
		{
			data_max=(float)data_max_int;
			data_min=(float)data_min_int;
			data_ave=(float)data_ave_int;
		}
	
	if (max_long > 191) x_pomak = 2;
	
	timel=(int) now.hour() * 60 + (((int) now.minute()/15)* 15)+15;	//pocetno vrijeme
	if(timel >= 1440) timel-=1440;
		
	t = ReadEeprom(eVrijeme,time_max);
	time_max = ((t/100)*60)+(t%10)*15-15;
		
	t = ReadEeprom(eVrijeme,time_min);
	time_min = ((t/100)*60)+(t%10)*15-15;

	for(in=1;in<4;in++)
	{
		timel +=360; 
		if(timel >= 1440) timel-=1440;
		sprintf(_str,"%02u:%02u", timel/60, timel%60);
		glcd_print_small( x0 + (x_pomak*24*in)-4, y0+1,_str, Small_font);	//2., 3. ,4.
	}
	timel +=360; 
	if(timel >= 1440) timel-=1440;
	
	// ispis podataka na y os
	dtostrf(data_min, 4, 1, _str);
	glcd_print_small( x0 + max_long + 1, y0-5,  _str, Small_font);
	
	dtostrf(data_max, 4, 1, _str);
	glcd_print_small( x0 + max_long + 1, y0-max_high,  _str, Small_font);

	dtostrf((data_min + data_max)/2.0, 4, 1, _str);
	glcd_print_small( x0 + max_long + 1, ((2*y0)-(max_high+5))/2,  _str, Small_font);
	
	//iscrtavanje grafa
	scale_y = ((float)(data_max_int-data_min_int))/((float)(max_high - y_offset_gore));
	if(vel==eTemperatura || vel==eTlak || vel==eSolar_temp || vel==ePros_brz) scale_y = scale_y/10.0f;

	//duljina = 0;	duljinalast=0;		
	ptr = tek_record;
	for(in=0;in<96;in++)
	{
		data_int=ReadEeprom(vel,ptr);
		if(vel==eTemperatura || vel==eTlak || vel==eSolar_temp || vel==ePros_brz) data = ((float)data_int)/10.0f;
		else data = (float)data_int;		
		if(scale_y!=0) razlika = (unsigned char)(((data - data_min) / scale_y)+0.5f);
		x = x0 + x_offset_lijevo+in*x_pomak;
		y = y0 - razlika  - y_offset_dole;
		
		if (y > (y0-max_high+y_offset_gore+TEXT_HIGH)) 	duljina++;

		if ((duljina > 0) && ((y <= (y0-max_high+y_offset_gore+TEXT_HIGH)) || (in==95)))
		{
			if (duljina > duljinalast)
			{
				krajX = x-1;
				duljinalast = duljina;
			}
			duljina=0;
		}

		LCD.writePixel(x, y,1);
		if(dot)
		{
			ys = y_last;
			if (y > y_last)
			{
				ys++;
				while(ys < y) { LCD.writePixel(x, ys, 1); ys++; }
			}
			else
			{
				ys--;
				while(ys > y) { LCD.writePixel(x, ys, 1); ys--; } 
			}
		}
		y_last = y;
		dot=true;
		ptr++;
		if(ptr > MAX_EEPROM_RECORD) ptr=0;
	}
	if(duljinalast > 24)
	{
		unsigned char xText,yText,dText;
		dText = 24;		
		xText = krajX - dText - ((duljinalast-dText) / 2);
		yText = y0-max_high;
		if (vel == eTemperatura) LCD.glcd_print2_P( xText, yText,"Temp", &Arial__6, 1);
		if (vel == ePros_brz) LCD.glcd_print2_P( xText, yText,"Wind", &Arial__6, 1);
		if (vel ==  eMax_brz) LCD.glcd_print2_P( xText, yText,"Gust", &Arial__6, 1);

		if (vel == eMax_brz) dtostrf(data_ave,4,0, _str);
		else dtostrf(data_ave,5,1, _str);
		
		LCD.glcd_print2_P(xText, yText+8 ,&_str[0],&Arial__6,1);
		
	}

	// za debug
/*	if((vel==eTemperatura)||(vel==ePros_brz))LCD.TextGoTo(4,11);
	else LCD.TextGoTo(24,11);
	LCD.writeStringPgm(F("X= "));
	dtostrf( (float)krajX,3,0, _str);
	LCD.writeString(_str);
	LCD.writeStringPgm(F("d= "));
	dtostrf((float)duljinalast,3,0, _str);
	LCD.writeString(_str);
*/
}
////////////////////////////////////////////////////////////////////////////////////////
void Draw_rain_graph(unsigned char x0,unsigned char y0,unsigned char sirina_stupca,unsigned char max_high )
{	//Draw_rain_graph( 133, 82, 1, 40)
	float data, scale_y,data_max;
	int16_t ptr;
	int data_max_int, data_int;
	unsigned char max_long = sirina_stupca * 30;
	unsigned char in, x, y;
	unsigned char  razlika = 0;
	const unsigned char y_offset_dole = 0;
	const unsigned char y_offset_gore = 2;
	const unsigned char x_offset_lijevo=2;
	
	ptr = rain_tbl_ptr;
	LCD.drawrectbyte(x0,y0-max_high,max_high,max_long/6 + 2, 0x00);//LCD.fillRect(x0, y0-max_high, x0+max_long, y0,0);
	
	data_max_int=0;
	uiMonthlyRain = 0;
	for(in=0;in<30;in++)
	{		
		data_int=i2ceeprom.read16 (ptr);
		if(data_int > data_max_int) data_max_int = data_int;
		ptr = ptr + 2;
		if(ptr > EEP_ADDR_last_rain_end) ptr = EEP_ADDR_last_rain_base;
		uiMonthlyRain = uiMonthlyRain + data_int;
	}
		
	data_max=((float)data_max_int)/10.0;

	dtostrf(data_max, 4, 1, _str);
	glcd_print_small( x0 + max_long + 3, y0-max_high + 7,  _str, Small_font);

	//iscrtavanje grafa
	scale_y = data_max / ((float)(max_high - y_offset_gore));
				
	ptr = rain_tbl_ptr;
	for(in=0;in<30;in++)
	{
		data_int=i2ceeprom.read16 (ptr);
		data = ((float)data_int)/10.0f;
		if(scale_y!=0) razlika = (unsigned char)((data / scale_y)+0.5f);
		x = x0 + x_offset_lijevo + in *sirina_stupca;
		y = y0 - razlika  - y_offset_dole;
		if ((razlika == 0) && (data_int > 7)) y++; 	//probati da se istakne mala padalina
		for (uint8_t inin = 0; inin < sirina_stupca; inin++)
		{
			unsigned char yt = y0;
			do {LCD.writePixel(x+inin, yt, 1); yt--;} while(yt > y);
			//LCD.createLine(x+inin, y0, x+inin, y, 1);
		}
		ptr = ptr + 2;
		if (ptr > EEP_ADDR_last_rain_end) ptr = EEP_ADDR_last_rain_base;
	
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
void Add_average_smjer(void)
{
	switch (Meteo_record.Smjer)
	{
	case 0:		
		yrez+=(int)Meteo_record.Pros_brz;
		break;
	case 15:		
		xrez+=(int)((double)Meteo_record.Pros_brz * 0.5);
		yrez+=(int)((double)Meteo_record.Pros_brz * 0.87);
		break;
	case 30:		
		xrez+=(int)((double)Meteo_record.Pros_brz * 0.87);
		yrez+=(int)((double)Meteo_record.Pros_brz * 0.5);
		break;
	case 45:		
		xrez+=(int)Meteo_record.Pros_brz;
		break;
	case 60:		
		xrez+=(int)((double)Meteo_record.Pros_brz * 0.87);
		yrez+=(int)((double)Meteo_record.Pros_brz * -0.5);
		break;
	case 75:		
		xrez+=(int)((double)Meteo_record.Pros_brz * 0.5);
		yrez+=(int)((double)Meteo_record.Pros_brz * -0.87);
		break;		
	case 90:		
		yrez+=(int)Meteo_record.Pros_brz * -1;
		break;
	case 105:		
		xrez+=(int)((double)Meteo_record.Pros_brz * -0.5);
		yrez+=(int)((double)Meteo_record.Pros_brz * -0.87);
		break;
	case 120:		
		xrez+=(int)((double)Meteo_record.Pros_brz * -0.87);
		yrez+=(int)((double)Meteo_record.Pros_brz * -0.5);
		break;
	case 135:		
		xrez+=(int)Meteo_record.Pros_brz * -1;
		break;
	case 150:		
		xrez+=(int)((double)Meteo_record.Pros_brz * -0.87);
		yrez+=(int)((double)Meteo_record.Pros_brz * 0.5);
		break;
	case 165:		
		xrez+=(int)((double)Meteo_record.Pros_brz * -0.5);
		yrez+=(int)((double)Meteo_record.Pros_brz * 0.87);
		break;
	default:
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////
unsigned char Count_average_smjer(int xr, int yr)
{
	unsigned char smjerh;
	smjerh=0;

	int k;

	if((yr==0)&&(xr==0)){
		return 0xff;
	}	//ti?ina
	if(yr==0)
	{
		if(xr<0) smjerh=135;
		else smjerh=45;
		xr=0;
		return smjerh;
	}
	if(xr==0)
	{
		if(yr<0) smjerh=90;
		else smjerh=0;
		yr=0;
		return smjerh;
	}
	k=(int)(abs((double)(xr)/(double)(yr))*100);
	if((yr>0)&&(xr>0))
	{
		if(k>373) smjerh=45;
		else
		{
			if(k>100) smjerh=30;
			else
			{
				if(k>27) smjerh=15;
				else
					smjerh=0;
			}
		}
	}
	if((xr>0)&&(yr<0))
	{
		if(k>373) smjerh=45;
		else
		{
			if(k>100) smjerh=60;
			else
			{
				if(k>27) smjerh=75;
				else
					smjerh=90;
			}
		}
	}
	if((yr<0)&&(xr<0))
	{
		if(k>373) smjerh=135;
		else
		{
			if(k>100) smjerh=120;
			else
			{
				if(k>27) smjerh=105;
				else
					smjerh=90;
			}
		}
	}
	if((xr<0)&&(yr>0))
	{
		if(k>373) smjerh=135;
		else
		{
			if(k>100) smjerh=150;
			else
			{
				if(k>27) smjerh=165;
				else
					smjerh=0;
			}
		}
	}
	xr=0;
	yr=0;
	return smjerh;
}

//////////////////////////////////////////////////////////////////
void checkRSSI()
{
	int bars;
	long RSSI;

	RSSI = WiFi.RSSI();
	if (RSSI > -55) { 
    bars = 5;
  } else if (RSSI < -55 && RSSI > -65) {
    bars = 4;
  } else if (RSSI < -65 && RSSI > -70) {
    bars = 3;
  } else if (RSSI < -70 && RSSI > -78) {
    bars = 2;
  } else if (RSSI < -78 && RSSI > -82) {
    bars = 1;
  } else {
    bars = 0;
  }
	int boja, ytt;
	for (int b=0; b < 6; b++) 
	{
		ytt = b + 1;
		if ( b < bars+1) boja = 1;
		else boja = 0;
		do { LCD.writePixel(b, 6-ytt, boja); ytt--;} while(ytt > 0);
   }
}
/////////////////////////////////////////////////////////////////////////////////
void ResetErrors(void)
{
	Error.NoSDcard = 0;
	Error.InitSDcard = 0;
	Error.OpenFileSDcard = 0;
	Error.CreateFileSDcard = 0;
	Error.WriteSDcard = 0;
	Error.RTC = 0;
	Error.RF24 = 0;
	Error.Sensor.SHT = 0;
	Error.Sensor.DS18D20sol_sensor = 0;
	Error.Sensor.MPX_sensor = 0;
	Error.Sensor.Wind_smjer_sensor = 0;
	Error.Sensor.Wind_speed_sensor = 0;
	Error.Sensor.Rain_sensor = 0;
	Error.Sensor.Uv_sensor = 0;
}
/////////////////////////////////////////////////////////////////////////////////
void twoSecEvents(void)
{	
	fCurrInsideTemperature = bme.readTemperature();
	fCurrInsideHumidity = bme.readHumidity();
	fInsidePressure = bme.readPressure() / 100.0F + 1.1;	//+1.1 because altitude
	
	ulWindCounter = ulWindCounter + (uint32_t)Meteo_record.Pros_brz;
	ulCounter2secProlaz++;
	Add_average_smjer();
	checkRSSI();	
}
//////////////////////////////////////////////////////////////////////////////////
void dayEvents()
{
	
	if (ucNoRain == 255) ucNoRain = 0;
	
	if( fCurrRainfall <= 1.0 ) ucNoRain++;
	i2ceeprom.write8 (EEP_ADDR_day_no_rain,ucNoRain);
	i2ceeprom.write16 (rain_tbl_ptr,(uint16_t)(fCurrRainfall*10) );
	rain_tbl_ptr = rain_tbl_ptr + 2; 
	if (rain_tbl_ptr > EEP_ADDR_last_rain_end) rain_tbl_ptr = EEP_ADDR_last_rain_base;
	i2ceeprom.write16 (EEP_ADDR_rain_tbl_ptr,rain_tbl_ptr);
	i2ceeprom.write16 (EEP_ADDR_yearly_rainfall,i2ceeprom.read16(EEP_ADDR_yearly_rainfall) + MeteoRecordPadalineLOC);
	i2ceeprom.write16 (EEP_ADDR_monthly_rainfall,i2ceeprom.read16(EEP_ADDR_monthly_rainfall) + MeteoRecordPadalineLOC);
	i2ceeprom.write16(EEP_ADDR_total_sun,0);

	lmidnightsec = now.secondstime();
}
////////////////////////////////////////////////////////////////////////////////////
void monthEvents()
{
   i2ceeprom.write16 (EEP_ADDR_monthly_rainfall,0);
	if (now.month()==1) i2ceeprom.write16(EEP_ADDR_yearly_rainfall,0);  //New Year
}
//////////////////////////////////////////////////////////////////////////////////////
void recordEvents(void)
{
	Test_jump_data();
	//Draw_wind_tendency();

	ucRecordSmjer=Count_average_smjer(xrez,yrez);
	if (ucRecordSmjer == 0xff) ucRecordSmjer = Meteo_record.Smjer;	// 0xff zna?i ti?ina
	xrez = yrez = 0;
	if(ulCounter2secProlaz != 0) uiRecordWindAverage = (unsigned int)((((float)ulWindCounter)*10.0+5.0) / (float)ulCounter2secProlaz);
		
	uiRecordPadaline = MeteoRecordPadalineLOC - uiPadalineLast;

	if ((uiRecordPadaline > 0) && (fCurrRainfall > 1.0))	i2ceeprom.write16(EEP_ADDR_last_rainfall,0);
		else i2ceeprom.write16 (EEP_ADDR_last_rainfall, i2ceeprom.read16(EEP_ADDR_last_rainfall) + 1);
	
	if( fCurrRainfall>1.0 ) ucNoRain=0;
	frain24h = (float)(sum24hour(ePadaline)) * tip_bucket_faktor;	//ovo je resetiralo procesor

	ulTotalWindCounter = ulTotalWindCounter + ulWindCounter;

	if((Meteo_record.Solar_temp-Meteo_record.Temperatura)>TRESHOLD_SOLAR)	//total_sun_hour *100 tj, 1h=100
		i2ceeprom.write16(EEP_ADDR_total_sun,i2ceeprom.read16(EEP_ADDR_total_sun)+(60/RECORD_TIME));

	WriteEepromRecord(); 
	Write_to_data_month_file();
	WriteThingspeakChannel();
	ResetErrors();
	//delay(5000);
	if (dayEvent == true) {dayEvents(); dayEvent = false;}
	if (monthEvent == true) {monthEvents(); monthEvent = false;}

	//if (Meteo_record.Padaline < uiPadalineLast) uiPadalineLast = 0;// prelaz na novi dan
	uiPadalineLast = Meteo_record.Padaline;
	i2ceeprom.write16 (EEP_ADDR_uiPadalineLast, uiPadalineLast);
	MeteoRecordPadalineLOC = Meteo_record.Padaline;

	ulWindCounter = 0;
	ucRecordWindGust = 0;
	ucRecordRainRainrate = 0;
	ulCounter2secProlaz = 0;

	osnovnaPozadina();
	Meteo_record.Smjer = ucLastSmjer = 30; updateSmjer(1);	//for animation
	update_10min_Smjer(ucLastMinSmjer,1);	//for animation

	if (ucLastMinSmjer != ucRecordSmjer) update_10min_Smjer(ucRecordSmjer,2);

	int Pros1H; int *pPros1H = &Pros1H;
	averagehour (ePros_brz, 1, pPros1H);
	LCD.TextGoTo(34,2);
	//LCD.writeString("   ");
	LCD.TextGoTo(33,2);
	dtostrf((double)Pros1H / 10.0, 4, 1, _str); 
	LCD.writeString(_str);
	LCD.TextGoTo(34,3);
	LCD.writeString("kmh");
 
	if (((lmidnightsec + 420) < timesec) && (lmidnightsec != 0))	//update time from NTP
	{
		configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);	// 7min after midnight
		struct tm timeinfo;
		if(getLocalTime(&timeinfo))
		{	
			rtc.adjust(DateTime(timeinfo.tm_year +1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
			lmidnightsec = 0;
		}
	}

	Draw_dnevni_graph(eTemperatura,3,120,100,36);
	Draw_dnevni_graph(eMax_brz,125,120,100,36);	//Draw_dnevni_graph(ePros_brz,125,120,100,36);
	LCD.createLine(120,80,120,127,1);	//v
	Draw_rain_graph( 133, 52, 1, 24);

	uint8_t tsthr = now.hour(); 
	if ((((tsthr + 2) % 3)==0) && (now.minute()==0))	//every 3 hour in 1,4,7,10,13,16,19,22 only once
	{
		gotova_prognoza = false;
		if (WiFi.status() != WL_CONNECTED)
		{
			WiFi.disconnect();
			LCD.TextGoTo(13,7);
	    	LCD.writeStringPgm(F("Conn to WiFi"));
			WiFi.mode(WIFI_STA);
			WiFi.begin(ssid, password);
			byte wifipassconn = 0;
   		 while (WiFi.status() != WL_CONNECTED && wifipassconn < 7) {
       		delay(500);
				LCD.writeString(".");
				wifipassconn++;
   		 }
			if (WiFi.status() != WL_CONNECTED) LCD.writeStringPgm(F("Not Conn"));
			else LCD.writeStringPgm(F("Connected"));
		}
		if (read_from_YR(SD, "/YR.XML") == 1)
		{
			progn = readPrognoza(SD, "/YR.XML");
			ulLastForecastTime =  dateTimeToLong(now.year(), now.month(), now.day(), now.hour(), now.minute(), 0);
			i2ceeprom.write32( EEP_ADDR_last_forecast_long, ulLastForecastTime);
		}
	}
	progn = 1;	//for draw again

	// za debug
	//i2ceeprom.write8(2000, Meteo_record.Vlaga);
	//LCD.TextGoTo(22,13);
	//LCD.writeStringPgm(F("U Rec Eventu = "));
	//dtostrf( (float)now.minute(),4,0, _str);
	//LCD.writeString(_str);
	//dtostrf((float)ReadEeprom(eTemperatura, tek_record-1)/10.0,6,1, _str);
	//LCD.writeString(_str);

	//LCD.TextGoTo(22,14);
	//LCD.writeStringPgm(F("EE Hum = "));
	//dtostrf( (float)i2ceeprom.read8(2000),4,0, _str);
	//LCD.writeString(_str);
	//LCD.writeStringPgm(F("tec_rec "));
	//dtostrf( (float)i2ceeprom.read16(4000),4,0, _str);
	//LCD.writeString(_str);
}

/////////////////////////////////////////////////////////////////////////////////////
void Write_to_data_month_file(void)
{ 
	char filenm[] = "/XXXYYlog.txt";  // This is a holder for the full file name
	char str[12];
	char _str[10];
	File datafl;
	float data;
	char Year_conv;

	memcpy_P(filenm+1,prog_months+3*(now.month()-1),3);
	Year_conv = (char)(now.year() % 100);
	filenm[4] = '0' + (Year_conv / 10 );
	filenm[5] = '0' + (Year_conv % 10);


	if (! SD.exists(filenm))
	{
			// only open a new file if it doesn't exist
		datafl = SD.open(filenm, FILE_WRITE);    // Open the correct file
		if (datafl)  datafl.close();
	}
	
	datafl = SD.open(filenm, FILE_APPEND);    // Open the correct file 
	if (datafl)
	{// if the file is available, write to it:
		str[0] = '0' + ((char)now.day() / 10);
		str[1] = '0' + ((char)now.day() % 10);
		str[2] = '.';
		str[3] = '0' + ((char)now.month() / 10);
		str[4] = '0' + ((char)now.month() % 10);
		str[5] = '.';
		str[6] = '0' + (Year_conv / 10 );
		str[7] = '0' + (Year_conv % 10);
		str[8] = '\0';
		strcat(str, comma);
		datafl.print(str);
		str[0] = '0' + ((char)now.hour() / 10);
		str[1] = '0' + ((char)now.hour() % 10);
		str[2] = ':';
		str[3] = '0' + ((char)now.minute() / 10);
		str[4] = '0' + ((char)now.minute() % 10);
		str[5] = '\0';
		strcat(str, comma);
		datafl.print(str);	//1

		*str='\0';
		dtostrf(fCurrTemperature,5,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//2

		*str='\0';
		dtostrf(fCurrHumidity,5,0, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//3

		data = calc_dewpoint(fCurrHumidity, fCurrTemperature);
		*str='\0';
		dtostrf(data,5,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//4

		*str='\0';
		dtostrf((float)uiRecordWindAverage/10.0,5,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//5

		*str='\0';
		dtostrf((float)ucRecordWindGust,5,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//6

		*str='\0';
		dtostrf((float)ucRecordSmjer*2,5,0, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//7

		*str='\0';
		dtostrf((float)ucRecordRainRainrate,5,0, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//8

		*str='\0';
		dtostrf(fCurrRainfall,5,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//9

		*str='\0';
		dtostrf(fCurrPressure,6,2, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//10

		*str='\0';
		dtostrf((float)(i2ceeprom.read16(EEP_ADDR_yearly_rainfall) + MeteoRecordPadalineLOC)*tip_bucket_faktor,5,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);//ukupno padaline u godinu dana	//11

		*str='\0';
		dtostrf(fCurrInsideTemperature,5,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//12

		*str='\0';
		dtostrf(fCurrInsideHumidity,3,0, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);//unutarnja vlaga	//13

		*str='\0';
		dtostrf((float)ucRecordWindGust,5,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);//isto kao gust	//14
		
		*str='\0';
		data = calc_chillindex(fCurrTemperature,(float)uiRecordWindAverage/10.0);
		dtostrf(data,5,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//15

		*str='\0';
		data = calc_heatindex(fCurrTemperature, fCurrHumidity);
		dtostrf(data,5,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//16

		*str='\0';
		dtostrf(fCurrUV,4,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//17

		*str='\0';
		data = calc_radiation(fCurrTemperature,fCurrSolar);
		dtostrf(data,5,0, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);	//18

		double etrh = ExtraterrestrialRadiationHourly(0.7451, 0.315, 0.2618,(float) now.hour(), (int) now.day(),(int) now.month(), false);
		//cloud cover ???
		double cssr = ClearSkySolarRadiation(etrh, 30);
		double rso = SnagaRadiacije(cssr)*1000000.0;//Rso
		int cloud_cover;
		bool noc;
		if (etrh==0) noc=true;
		else noc=false;
		double pros_vj_ms;
		pros_vj_ms=(float)uiRecordWindAverage/36.6;
		if((rso > 0)&&(data < rso)&&(noc==false)) cloud_cover = constrain((int)((1.0-(data/rso))*100.0),0,100);
		else cloud_cover=0;	
		if (noc==true) 
		{
			if((Meteo_record.Solar_temp+5)<Meteo_record.Temperatura)cloud_cover=10;
			else 
			{
				if (pros_vj_ms>3) cloud_cover=10;
				else cloud_cover=50;
			}
		}

		data = EvapoTranspirationHourly(fCurrTemperature,fCurrHumidity,fCurrPressure,pros_vj_ms,cloud_cover,(double)now.hour(), 30,0.2618, 0.315, 0.7451,(int)now.day(),(int)now.month(), false);
		fEvth_global = data;	//ako treba globalna varijabla
		*str='\0';
		dtostrf(data,4,2, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);//evapotrenspiration

		*str='\0';
		dtostrf(0.00,5,2, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);//annual avapotranspiration

		*str='\0';
		data = calc_apparent_temp(fCurrTemperature,fCurrHumidity,(float)uiRecordWindAverage/10.0,calc_radiation(fCurrTemperature,fCurrSolar));
		dtostrf(data,5,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);

		data = SnagaRadiacije(ClearSkySolarRadiation(etrh, 30))*1000000.0;//Rso
		*str='\0';
		dtostrf(data,5,0, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);

		*str='\0';
		dtostrf((float)i2ceeprom.read16(EEP_ADDR_total_sun)/16,5,1,_str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);

		*str='\0';
		dtostrf((float)ucRecordSmjer*2,3,0, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);

		*str='\0';
		dtostrf(0.0,3,1, _str);
		strcat(str, Deblank(_str));
		strcat(str, comma);
		datafl.print(str);//RG-11

		*str='\0';
		dtostrf((float)sumlasthour(ePadaline) * tip_bucket_faktor,5,1, _str);
		strcat(str, Deblank(_str));
		datafl.println(str);
		datafl.close();	//padaline u zadnjem satu
	}
}

//////////////////////////////////////////////////////////////////////////////
void osnovnaPozadina(void) //load basic background
{ 
	LCD.clearText();
	LCD.clearGraphic(); 
	//Load_bmp_file(0,0, "logdata/meteo.bmp");
   LCD.createLine(77,0,77,80,1);	//v
	LCD.createLine(104,54,104,80,1);	//v
	LCD.createLine(120,80,120,127,1);	//v
	LCD.createLine(131,7,131,80,1);	//v
	LCD.createLine(158,54,158,80,1);	//v
	LCD.createLine(184,0,184,80,1);	//v
   LCD.createLine(212,54,212,80,1);	//v

	 LCD.createLine(77,7,184,7,1);	//h
    LCD.createLine(77,54,239,54,1);	//h
    LCD.createLine(0,80,239,80,1);	//h
//////////
	//ru?a vjetrova
	const unsigned char OFFSETX=63, OFFSETY=4;
	LCD.createCircle(150+OFFSETX, 30-OFFSETY, 22, 1);
	LCD.createCircle(150+OFFSETX, 30-OFFSETY, 23, 1);
	LCD.glcd_print2_P(151+OFFSETX,5-OFFSETY,"N",&Arial__6,1); 
	LCD.glcd_print2_P(151+OFFSETX,49-OFFSETY,"S",&Arial__6,1);
	LCD.glcd_print2_P(128+OFFSETX,26-OFFSETY,"W",&Arial__6,1); 
	LCD.glcd_print2_P(171+OFFSETX,26-OFFSETY,"E",&Arial__6,1);  
	//boja_smjer=0;		//za neispis prvog smjera vjetra
	//boja_minsmjer=0;


	
	LCD.glcd_print2_P(0,22,"out",&Arial__6,1);
	LCD.TextGoTo(11,2);
	LCD.writeExtChar(1); 
	LCD.writeExtChar(6);//LCD.glcd_print2_P(60,22,"C",&Arial__6,1);
	LCD.glcd_print2_P(0,38,"RH",&Arial__6,1);
	LCD.glcd_print2_P(66,33,"%",&Arial__8,1);   LCD.createLine(77,0,77,80,1);
	LCD.glcd_print2_P(0,54,"sol",&Arial__6,1);
	LCD.glcd_print2_P(0,69,"p",&Arial__6,1);
	LCD.TextGoTo(11,6);
	LCD.writeExtChar(1); 
	LCD.writeExtChar(6);	
	LCD.TextGoTo(11,9);
	LCD.writeExtChar(3);
	LCD.writeExtChar(4);	//print hPa2//

	LCD.TextGoTo(3,0); LCD.writeStringPgm(F("Outdoor"));
	LCD.TextGoTo(14,1); LCD.writeStringPgm(F("Indoor"));
	//LCD.TextGoTo(24,1); LCD.writeStringPgm(F("Rain"));
	LCD.TextGoTo(1,0);
	LCD.writeExtChar(5);	//print antena za wifi
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void displayUpdate(void)
{
	unsigned char dst;
	dst = isDst(now.day(),now.month(), now.dayOfTheWeek());
	
	if(menu==0)
	{
		if (!menu_active)
			{
				LCD.clearGraphic();
				osnovnaPozadina();
				//Draw_small_graph((velicina)graph,graph_hour);
				//Small_graph_label();
				//update_10min_Smjer((unsigned int)record_smjer*2);
				//Draw_tendency();
			}
		
		
		LCD.TextGoTo(13,0);
		sprintf(_str,"%02u/%02u/%02u %02u:%02u:%02u",(uint)now.day(), (uint)now.month(),(uint)now.year()%100, (uint)now.hour() + dst, (uint)now.minute(),(uint)now.second());
   	    LCD.writeString(_str);

		LCD.TextGoTo(31,0);
		char dayname[]="  ";		//printf("%1u",(int) now.dayOfWeek);
		memcpy_P(dayname,prog_days+2*(now.dayOfTheWeek()),2);
		sprintf(_str,"%2s",dayname);
		LCD.writeString(_str);

		LCD.TextGoTo(13,2);
		LCD.writeStringPgm(F("T "));
		dtostrf( fCurrInsideTemperature,4,1, _str);
		LCD.writeString(_str);
		LCD.writeExtChar(1); 
		LCD.writeExtChar(6);

		LCD.TextGoTo(13,3);
	    LCD.writeStringPgm(F("H "));
		dtostrf(fCurrInsideHumidity,4,1, _str);
		LCD.writeString(_str);
	    LCD.writeStringPgm(F("%"));

		LCD.TextGoTo(13,4);
	    LCD.writeStringPgm(F("UV "));
		dtostrf(fCurrUV,4,1, _str);
		LCD.writeString(_str);

		LCD.TextGoTo(13,5);
	    LCD.writeStringPgm(F("EVTH"));
		dtostrf(fEvth_global,4,2, _str);
		LCD.writeString(_str);
		//LCD.writeStringPgm(F("mm"));

		//Dodao za debug
		//LCD.TextGoTo(13,2);
		//LCD.writeStringPgm(F("MrP "));
		//dtostrf ( Meteo_record.Padaline, 4, 0, _str); LCD.writeString(_str);	
		//LCD.TextGoTo(13,5);
		//LCD.writeStringPgm(F("LOC "));
		//dtostrf ( MeteoRecordPadalineLOC, 4, 0, _str); LCD.writeString(_str);	

		//LCD.TextGoTo(22,3);
		//LCD.writeStringPgm(F("       "));

		if ((frain24h < 1.0) && (MeteoRecordPadalineLOC == 0)) 
		{
			LCD.TextGoTo(22,1); LCD.writeStringPgm(F(" No Rain"));
			LCD.TextGoTo(22,2);
			uint16_t minNoRain = i2ceeprom.read16(EEP_ADDR_last_rainfall);
			float dy = (float)minNoRain / 96.0; // 1440 min/ RECORDTIME = 96 tj.24*4
			dtostrf(dy, 4, 1, _str); LCD.writeString(_str);
			//dtostrf((float)ucNoRain,3,0, _str); LCD.writeString(_str);
			LCD.writeStringPgm(F(" d"));
			LCD.TextGoTo(22,3);
			LCD.writeStringPgm(F("30d "));
			dtostrf ((float)uiMonthlyRain / 10.0, 4, 1, _str); LCD.writeString(_str);

		}
		else
		{
			LCD.TextGoTo(22,1); LCD.writeStringPgm(F("  Rain  "));
			LCD.TextGoTo(22,2);
			LCD.writeStringPgm(F("24h "));
			dtostrf ( frain24h + (float)(MeteoRecordPadalineLOC - uiPadalineLast)*tip_bucket_faktor, 4, 1, _str); LCD.writeString(_str);		
			LCD.TextGoTo(22,3);
			LCD.writeStringPgm(F("15m "));
			dtostrf ((float)(lastdata(ePadaline))*tip_bucket_faktor, 4, 1, _str); LCD.writeString(_str);
		}
/*		
LCD.TextGoTo(22,2);
LCD.writeStringPgm(F("TPtr "));
dtostrf((float)rain_tbl_ptr,4,0, _str);
LCD.writeString(_str);
LCD.TextGoTo(22,1);
LCD.writeStringPgm(F("Eptr "));
dtostrf((float)(i2ceeprom.read16 (rain_tbl_ptr)),4,1, _str);
LCD.writeString(_str);
LCD.TextGoTo(22,3);
LCD.writeStringPgm(F("E+2 "));
dtostrf((float)(i2ceeprom.read16 (rain_tbl_ptr+2)),4,1, _str);
LCD.writeString(_str);
*/	
		if(Error.Sensor.SHT) LCD.glcd_print2_P(18,15," ---- ",&Arial__14,1);
		else
		{
			dtostrf(fCurrTemperature,5,1, _str);
			if( strchr( _str, '0'+1 )) { 
				_str[5]=' '; 
				_str[6]='\0'; 
				LCD.glcd_print2_P(18,15,&_str[0],&Arial__14,1);
			}
			else LCD.glcd_print2_P(18,15,&_str[0],&Arial__14,1);
		}
		if(Error.Sensor.MPX_sensor) LCD.glcd_print2_P(18,15," ---- ",&Arial__14,1);
		else
		{
			dtostrf(fInsidePressure,6,1, _str);
			//dtostrf(bme.readPressure() / 100.0F,6,1, _str);
			if( strchr( _str, '0'+1 )) { 
				_str[6]=' '; 
				_str[7]='\0'; 
				LCD.glcd_print2_P(11,63,&_str[0],&Arial__14,1);
			}
			else LCD.glcd_print2_P(11,63,&_str[0],&Arial__14,1);
		}
		if(Error.Sensor.DS18D20sol_sensor) LCD.glcd_print2_P(18,47," ---- ",&Arial__14,1);
		else
		{
			dtostrf(fCurrSolar,5,1, _str);
			if( strchr( _str, '0'+1 )) { 
				_str[5]=' '; 
				_str[6]='\0'; 
				LCD.glcd_print2_P(18,47,&_str[0],&Arial__14,1);
			}
			else LCD.glcd_print2_P(18,47,&_str[0],&Arial__14,1);
		}
		if(Error.Sensor.SHT) LCD.glcd_print2_P(18,15," ---- ",&Arial__14,1);
		else
		{
			dtostrf(fCurrHumidity,3,0, _str);
			if( strchr( _str, '0'+1 )) { 
				_str[3]=' '; 
				_str[4]='\0'; 
				LCD.glcd_print2_P(22,31,&_str[0],&Arial__14,1);
			}
			else  LCD.glcd_print2_P(22,31,&_str[0],&Arial__14,1);	
		}

		if(Meteo_record.Smjer != ucLastSmjer) updateSmjer(2);	
		
		if (progn)
		{
			draw_forecast (80, 57, prognoza[0][0],prognoza[0][1]);
			draw_forecast (108, 57, prognoza[1][0],prognoza[1][1]);
			draw_forecast (136, 57, prognoza[2][0],prognoza[2][1]);
			draw_forecast (164, 57, prognoza[3][0],prognoza[3][1]);
			draw_forecast (191, 57, prognoza[4][0],prognoza[4][1]);
			draw_forecast (218, 57, prognoza[5][0],prognoza[5][1]);
		
			char dstr[] = {'0','0',':','0','0','\0'};
			dateTimeLong2Str (ulLastForecastTime,_str);
			for(byte i = 0; i < 5; i++) {dstr[i] = _str[11 + i];}
			dstr[5] = '\0';
			glcd_print_small( 220, 48, dstr, Small_font);
		
			LCD.createLine(104,54,104,80,1);	//v
			LCD.createLine(131,54,131,80,1);	//v
			LCD.createLine(158,54,158,80,1);	//v
			LCD.createLine(184,54,184,80,1);	//v
			LCD.createLine(212,54,212,80,1);	//v
			progn = 0;
		}
	}
}

void CheckForConnections()
{
  if (Server.hasClient())
  {
    // If we are already connected to another computer, 
    // then reject the new connection. Otherwise accept
    // the connection. 
    if (RemoteClient.connected())
    {
      Serial.println("Connection rejected");
      Server.available().stop();
    }
    else
    {
      Serial.println("Connection accepted");
      RemoteClient = Server.available();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
	delay(200);
	/////////////////LCD///////////////////////////
	LCD.Initialize();
	Serial.begin(115200);
	LCD.TextGoTo(0,0);
   //Serial.println("LCD initialized");
	LCD.writeStringPgm(F("LCD initialized\n\r"));
  /************************************************************/
	LCD.DefineCharacter(0, char_delta);   /*(unsigned char*)*/
	LCD.DefineCharacter(1, char_stupanj);   /*(unsigned char*)*/
	LCD.DefineCharacter(2, char_in);   /*(unsigned char*)*/
	LCD.DefineCharacter(3, char_hPa1);   /*(unsigned char*)*/
	LCD.DefineCharacter(4, char_hPa2);   /*(unsigned char*)*/
	LCD.DefineCharacter(5, char_antena);   /*(unsigned char*)*/
	LCD.DefineCharacter(6, char_C);   /*(unsigned char*)*/
	LCD.DefineCharacter(7, char_06h);   /*(unsigned char*)*/
	LCD.DefineCharacter(8, char_24h);   /*(unsigned char*)*/
	LCD.DefineCharacter(9, char_12h);   /*(unsigned char*)*/
	LCD.DefineCharacter(gore, char_gore);   /*(unsigned char*)10*/
	LCD.DefineCharacter(goredesno, char_goredesno);   /*(unsigned char*)11*/
	LCD.DefineCharacter(desno, char_desno);   /*(unsigned char*)12*/
	LCD.DefineCharacter(doledesno, char_doledesno);   /*(unsigned char*)13*/
	LCD.DefineCharacter(dole, char_dole);   /*(unsigned char*)14*/
/*********************BME280*******************************/
	unsigned status;
    // default settings
    status = bme.begin();  
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    if (!status) 
	{
        LCD.writeStringPgm(F("Could not find a valid BME280 sensor"));
        LCD.writeStringPgm(F("SensorID was: 0x"));
		byte ID_sens = bme.sensorID(); Serial.println(ID_sens,16);
		if ( ID_sens == 255 ) LCD.writeStringPgm(F(" Bad address, a BMP 180 or BMP 085\n\r"));
    }
	 else LCD.writeStringPgm(F("BME280 initialized\n\r"));
/////////////////////SD CARD///////////////////////////////
  	if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }
    LCD.writeStringPgm(F("SD Card Type: "));
    if(cardType == CARD_MMC){
        LCD.writeStringPgm(F("MMC"));
    } else if(cardType == CARD_SD){
        LCD.writeStringPgm(F("SDSC"));
    } else if(cardType == CARD_SDHC){
        LCD.writeStringPgm(F("SDHC"));
    } else {
        LCD.writeStringPgm(F("UNKNOWN"));
    }
	LCD.writeStringPgm(F("\n\r"));
    cardSize = SD.cardSize() / (1024 * 1024);
    LCD.writeStringPgm(F("SD Card Size: "));
	dtostrf((double)cardSize,9,0, _str); 
	LCD.writeString(_str);
	LCD.writeStringPgm(F(" MB\n\r"));
    //appendFile(SD, "/test.txt", "World!\n");
////////////////////RTC//////////////////////////////
    if (! rtc.begin()) {
   	LCD.writeStringPgm(F("Couldn't find RTC"));
  	}

  if (! rtc.isrunning()) {
    LCD.writeStringPgm(F("RTC is NOT running!"));
    rtc.adjust(DateTime(2020, 4, 17, 3, 0, 0));
  	}
	else LCD.writeStringPgm(F("RTC running\n\r"));
   //rtc.adjust(DateTime(2020, 5, 23, 10, 3, 0));

	//////////////////////////WiFi//////////////////////////////
	if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
   		LCD.writeStringPgm(F("STA Failed to configure"));
  	}
	WiFi.mode(WIFI_STA);
	LCD.writeString("\n\r");
	LCD.writeStringPgm(F("Connecting to Marinko_WiFi\n\r"));
	WiFi.begin(ssid, password);
	byte wifipassconn = 0;
    while (WiFi.status() != WL_CONNECTED && wifipassconn < 15) {
        delay(500);
		LCD.writeString(".");
		wifipassconn++;
    }
	 if (WiFi.status() != WL_CONNECTED)
	 {
		LCD.writeString("\n\r");
		LCD.writeStringPgm(F("WiFi not connected!!"));
 	 }
	  else
	  {
		LCD.writeString("\n\r");
		LCD.writeStringPgm(F("IP address: "));
		sprintf(_str,  WiFi.localIP().toString().c_str());
		LCD.writeString(_str);LCD.writeString("\n\r");
	  }
	  	delay(1000);
////////////OTA setup///////////////////
	ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
//////////////OTA end
//******************InitRF24******************************
radio.begin();
// optionally, increase the delay between retries & # of retries
radio.setRetries(15,15);
radio.setDataRate(RF24_250KBPS);
radio.setPayloadSize(14);

radio.openWritingPipe(pipes[1]);
radio.openReadingPipe(1,pipes[0]); 
Serial.print("Connected RF24 ");LCD.writeStringPgm(F("Connected RF24\n\r"));
Serial.print(radio.isChipConnected());
radio.startListening();
	delay(1000);
//radio.printDetails(); 
//*************************I2Ceeprom***************************** 
 if (i2ceeprom.begin(0x50)) {
	 Serial.println("Found I2C EEPROM");LCD.writeStringPgm(F("Found I2C EEPROM\n\r"));}
	 else Serial.println("I2C EEPROM not identified ...\r\n");
		delay(1000);

////////////////////////////////NTP time//////////////////////////////
configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
struct tm timeinfo;
if(getLocalTime(&timeinfo))
{
	LCD.writeStringPgm(F("Obtain time from NTP\n\r"));
	rtc.adjust(DateTime(timeinfo.tm_year +1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
}
else LCD.writeStringPgm(F("Failed to obtain NTP\n\r"));
//////////////////////////////////////////////////////////////////////

p_prognoza = & prognoza_structura;
p_prognoza_konacna = & prognoza_konacna_structura;	//inicijalizacija pointera

//if (read_from_YR(SD, "/YR.XML") == 1)
gotova_prognoza = false;
progn = readPrognoza(SD, "/YR.XML");

	delay(3000);
	osnovnaPozadina();

	p_struct = &Meteo_record;
	// init for timers
	now = rtc.now();
	last2SecTime = now.secondstime();
	timestart = true;
   today = now.day();
	lastDay = now.dayOfTheWeek();
   lastMonth = now.month();
	lastMinute = now.minute();
	
	 tek_record = i2ceeprom.read16(EEP_ADDR_tek_record);
	 ucNoRain = i2ceeprom.read8 (EEP_ADDR_day_no_rain);
	 uiPadalineLast = i2ceeprom.read16 (EEP_ADDR_uiPadalineLast);

	 rain_tbl_ptr = i2ceeprom.read16 (EEP_ADDR_rain_tbl_ptr); //rain_tbl_ptr = EEP_ADDR_last_rain_base; //samo za pocetak
	 rain_tbl_ptr = constrain(rain_tbl_ptr, EEP_ADDR_last_rain_base, EEP_ADDR_last_rain_end);

	 ulLastForecastTime = i2ceeprom.read32(EEP_ADDR_last_forecast_long);
	Serial.printf("LasteeForCst=%u%%\r", ulLastForecastTime);
	 if ((ulLastForecastTime + 22000) < (dateTimeToLong(now.year(), now.month(), now.day(), now.hour(), now.minute(), 0)))
	 {
			if (read_from_YR(SD, "/YR.XML") == 1)
			{
				gotova_prognoza = false;
				progn = readPrognoza(SD, "/YR.XML");
				ulLastForecastTime = dateTimeToLong(now.year(), now.month(), now.day(), now.hour(), now.minute(), 0);
				Serial.printf("LastForCst=%u%%\r", ulLastForecastTime);
				i2ceeprom.write32( EEP_ADDR_last_forecast_long, ulLastForecastTime);
				Serial.printf("LasteeForCst= %u%%\r", i2ceeprom.read32(EEP_ADDR_last_forecast_long));
			}
	 }
	LCD.drawrectbyte(0,81,46,40,0x00);	//LCD.fillRect(0, 81, 119, 127,0);
	Draw_dnevni_graph(ePros_brz,3,120,100,36); //Draw_dnevni_graph(eTemperatura,3,120,100,36);
	Draw_dnevni_graph(eMax_brz,125,120,100,36);	//Draw_dnevni_graph(ePadaline,125,120,100,36);
	LCD.createLine(120,80,120,127,1);	//v
	Draw_rain_graph( 133, 52, 1, 24);

	 Meteo_record.Smjer = ucLastSmjer = 30; updateSmjer(1);	//zbog animacije
	 ucLastMinSmjer = ucRecordSmjer;  update_10min_Smjer(ucRecordSmjer,1);	//zbog animacije
	 frain24h = (float)sum24hour(ePadaline) * tip_bucket_faktor;	//ovo je resetiralo procesor
/*
	 uint16_t rain_test[] = {0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,36,20,0,0,0,76,10,5,0, 0,0,0,0};
	 uint16_t tptr = EEP_ADDR_last_rain_base;
	 for(uint8_t in=0;in<30;in++)
	{	
		i2ceeprom.write16 (tptr, rain_test[in]);
		tptr = tptr + 2;
	}
*/

Server.begin();
ThingSpeak.begin(RemoteClient);  // Initialize ThingSpeak
}	//end setup

////////////////////////////////////////////////////////////////////////
char* Text_smjer_vjetra(unsigned char smjer)
{
		if(!Error_constrain((int) smjer,0,165)) memcpy_P(text_smjer,prog_text_smjer+3*(smjer/15),3);				
		return text_smjer;
}
////////////////////////////////////////////////////////////////////////
void sendEasyWeatherData()
{	
	char str[15];
	char _str[10];
	unsigned char Year_conv = now.year() % 100;
	str[0] = '2'; 
	str[1] = '0';
	str[2] = '0' + (Year_conv / 10 );
	str[3] = '0' + (Year_conv % 10);
	str[4] = '-';
	str[5] = '0' + ((char)now.month() / 10);
	str[6] = '0' + ((char)now.month() % 10);
	str[7] = '-';
	str[8] = '0' + ((char)now.day() / 10);
	str[9] = '0' + ((char)now.day() % 10);
	str[10] = ' ';
	str[11] = '\0';
	RemoteClient.print(str);
	
	str[0] = '0' + ((char)now.hour() / 10);
	str[1] = '0' + ((char)now.hour() % 10);
	str[2] = ':';
	str[3] = '0' + ((char)now.minute() / 10);
	str[4] = '0' + ((char)now.minute() % 10);
	str[5] = ':';
	str[6] = '0' + ((char)now.second() / 10);
	str[7] = '0' + ((char)now.second() % 10);
	str[8] = '\0';
	strcat(str, comma);
	RemoteClient.print(str);
	
	*str='\0';
	dtostrf(fCurrInsideHumidity,3,0, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.print(str);//unutarnja vlaga
	
	*str='\0';
	dtostrf(fCurrInsideTemperature,5,1, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.print(str);
	
	*str='\0';
	dtostrf(fCurrHumidity,5,0, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.print(str);

	*str='\0';
	dtostrf(fCurrTemperature,5,1, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.print(str);

	*str='\0';
	dtostrf(calc_dewpoint(fCurrHumidity, fCurrTemperature),5,1, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.print(str);
	
	*str='\0';
	dtostrf(calc_chillindex(fCurrTemperature,(float)uiRecordWindAverage/10.0),5,1, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.print(str);
	
	*str='\0';
	dtostrf(fCurrPressure,6,1, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.print(str);
	
	*str='\0';
	dtostrf(fCurrWindSpeed,5,1, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.print(str);
	
	*str='\0';
	dtostrf((float)lastdata(eMax_brz),5,1, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.print(str);
	
	*str='\0';
	strcat(str, Text_smjer_vjetra(Meteo_record.Smjer));
	strcat(str, comma);
	RemoteClient.print(str);
	
	*str='\0';
	dtostrf((float)sumlasthour(ePadaline) * tip_bucket_faktor,5,0, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.print(str);
	
	*str='\0';
	dtostrf((float)(i2ceeprom.read16(EEP_ADDR_yearly_rainfall) + MeteoRecordPadalineLOC)*tip_bucket_faktor,5,1, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.print(str);//ukupno padaline u godinu dana
	
	*str='\0';
	dtostrf(fCurrUV,4,1, _str);
	strcat(str, Deblank(_str));
	strcat(str, comma);
	RemoteClient.println(str);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void WriteThingspeakChannel(void)
{

	char myStatus[150];
	*myStatus='\0';
	char tmpstr[4]; tmpstr[1] = '\0';
	// set the fields with the values
  	ThingSpeak.setField(FIELDTEMP, fCurrTemperature);
  	ThingSpeak.setField(FIELDHUM, fCurrHumidity);
  	ThingSpeak.setField(FIELDPRESS, fCurrPressure);
  	ThingSpeak.setField(FIELDRAIN, fCurrRainfall);
	ThingSpeak.setField(FIELDWINDSP, lastdata(eMax_brz));
  	ThingSpeak.setField(FIELDWINDDIR, Meteo_record.Smjer*2);
  	ThingSpeak.setField(FIELDRAD, calc_radiation(fCurrTemperature,fCurrSolar));
  	ThingSpeak.setField(FIELDUV, fCurrUV);

	unsigned char Year_conv = now.year() % 100;
	longTime[0] = '2';
	longTime[1] = '0';
	longTime[2] = '0' + (Year_conv / 10 );
	longTime[3] = '0' + (Year_conv % 10);
	longTime[4] = '-';
	longTime[5] = '0' + ((char)now.month() / 10);
	longTime[6] = '0' + ((char)now.month() % 10);
	longTime[7] = '-';
	longTime[8] = '0' + ((char)now.day() / 10);
	longTime[9] = '0' + ((char)now.day() % 10);
	longTime[10] = ' ';
	longTime[11] = '0' + ((char)now.hour() / 10);
	longTime[12] = '0' + ((char)now.hour() % 10);
	longTime[13] = ':';
	longTime[14] = '0' + ((char)now.minute() / 10);
	longTime[15] = '0' + ((char)now.minute() % 10);
	longTime[16] = ':';
	longTime[17] = '0' + ((char)now.second() / 10);
	longTime[18] = '0' + ((char)now.second() % 10);
	longTime[19] = '\0';
	//Serial.println(longTime);
	//LCD.TextGoTo(1,1); LCD.writeString(longTime); LCD.writeString("\n\r");
	ThingSpeak.setCreatedAt(longTime);

	//set Status
	unsigned char ErrorSDcard = 0;
	if (Error.NoSDcard) ErrorSDcard &= 1;
	if (Error.InitSDcard) ErrorSDcard &= 2;
	if (Error.OpenFileSDcard) ErrorSDcard &= 4;
	if (Error.CreateFileSDcard) ErrorSDcard &= 8;
	if (Error.WriteSDcard) ErrorSDcard &= 16;
	strcat (myStatus,"Disp   ");
	tmpstr[0] = '0' + Error.Display; strcat (myStatus, tmpstr);strcat(myStatus,"   ");
	strcat (myStatus,"SDcard ");
	tmpstr[0] = '0' + ErrorSDcard/10;
	tmpstr[1] = '0' + ErrorSDcard%10;
	tmpstr[2] = '\0'; strcat (myStatus, tmpstr);strcat(myStatus,"   ");
	tmpstr[1] = '\0';
	strcat (myStatus,"RTC    ");
	tmpstr[0] = '0' + Error.RTC; strcat (myStatus, tmpstr);strcat(myStatus,"   ");
	strcat (myStatus,"RF24   ");
	tmpstr[0] = '0' + Error.RF24; strcat (myStatus, tmpstr);strcat(myStatus,"   ");
	strcat (myStatus,"SHT    ");
	tmpstr[0] = '0' + Error.Sensor.SHT; strcat (myStatus, tmpstr);strcat(myStatus,"   ");
	strcat (myStatus,"DS18D2 ");
	tmpstr[0] = '0' + Error.Sensor.DS18D20sol_sensor; strcat (myStatus, tmpstr);strcat(myStatus,"   ");
	strcat (myStatus,"MPX    ");
	tmpstr[0] = '0' + Error.Sensor.MPX_sensor; strcat (myStatus, tmpstr);strcat(myStatus,"   ");
	strcat (myStatus,"WindDi ");
	tmpstr[0] = '0' + Error.Sensor.Wind_smjer_sensor; strcat (myStatus, tmpstr);strcat(myStatus,"   ");
	strcat (myStatus,"WindSp ");
	tmpstr[0] = '0' + Error.Sensor.Wind_speed_sensor; strcat (myStatus, tmpstr);strcat(myStatus,"   ");
	strcat (myStatus,"Rain   ");
	tmpstr[0] = '0' + Error.Sensor.Rain_sensor; strcat (myStatus, tmpstr); strcat(myStatus,"   ");
	strcat (myStatus,"UV     ");
	tmpstr[0] = '0' + Error.Sensor.Uv_sensor; strcat (myStatus, tmpstr);
	//LCD.writeString(myStatus); LCD.writeString("\n\r");
	//Serial.println(myStatus);
	ThingSpeak.setStatus(myStatus);

	// write to the ThingSpeak channel
	int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
	/*if(x == 200){
		Serial.println("Channel update successful.");
		LCD.writeString("Channel update successful."); LCD.writeString("\n\r");
	}
	else{
		Serial.print("Problem updating channel. HTTP error code ");
		Serial.println(String(x));
		dtostrf( x,4,0, _str);
		LCD.writeString("Problem. HTTP error:"); LCD.writeString(_str); LCD.writeString("\n\r");
	}*/
}	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{   
	ArduinoOTA.handle();	//OTA handler
 	now = rtc.now();
	timesec = now.secondstime();
	displayUpdate();

// if there is data ready
if ( radio.available() )
	{
			// Fetch the payload, and see if this was the last one.
			radio.read( bufferin, 14 );
			LCD.TextGoTo(39,0);
			LCD.writeExtChar(5);	//print antena
			
			//setup buffer
			Meteo_record.Temperatura = ((((int16_t)bufferin[1] & 0x00ff) << 8) | (int16_t)bufferin[0]);		//temperatura x10
			Meteo_record.Tlak = ((((uint16_t)bufferin[3] & 0x00ff) << 8) | (uint16_t)bufferin[2]);		//tlak x10	tlak stavljen kao integer
			Meteo_record.Vlaga = bufferin[4];
			Meteo_record.Solar_temp = ((((int16_t)bufferin[6] & 0x00ff) << 8) | (int16_t)bufferin[5]);		//temperatura x10
			Meteo_record.Max_brz = bufferin[7];	//ne koristi se u transferu
			Meteo_record.Pros_brz = bufferin[8]; //brzina u prosle  2 sekunde 
			Meteo_record.Padaline = ((((uint16_t)bufferin[10] & 0x00ff) << 8) | (uint16_t)bufferin[9]);	//dnevne padaline
			Meteo_record.Smjer = bufferin[11];	//prosjecan smjer u stupnjevi/2
			Meteo_record.Rain_rate = bufferin[12];	//l na sat   
			Meteo_record.Uv = bufferin[13];

		// First, stop listening so we can talk
			radio.stopListening();
		// Send the final one back.
			unsigned char answer[]={ now.hour(),now.minute()};
			radio.write( answer, sizeof(answer) );
		// printf("Sent response.\n\r");
		// Now, resume listening so we catch the next packets.
		radio.startListening();

		//////////////// test temperaturnih podataka na jump i vrijednost
		if( Error_constrain(Meteo_record.Temperatura,-200,500)) //temperatura x10
		{
			Error.Sensor.SHT = 2;
			Meteo_record.Temperatura=Meteo_record_last.Temperatura;
		}		
		else
		{
			if(abs(Meteo_record_last.Temperatura-Meteo_record.Temperatura)>15 && !timestart && prolaz_t<10)
			{
				Meteo_record.Temperatura=Meteo_record_last.Temperatura;
				Error.Sensor.SHT = 5;
				prolaz_t++;
			}
			else 
			{
				Meteo_record_last.Temperatura = Meteo_record.Temperatura;
				Error.Sensor.SHT = 0;
				prolaz_t = 0;
			}
		}
		fCurrTemperature = (float)Meteo_record.Temperatura/10.0;

		if( Error_constrain(Meteo_record.Tlak,9500,11000))Error.Sensor.MPX_sensor = 2;		//tlak x10
			else Error.Sensor.MPX_sensor = 0;
		fCurrPressure = (float)Meteo_record.Tlak/10.0;
		
		////////////////test podataka vlage na jump i vrijednost
		if( Error_constrain(Meteo_record.Vlaga,0,100))
		{
			Error.Sensor.SHT= 2;
			Meteo_record.Vlaga=Meteo_record_last.Vlaga;
		}		
		else
		{
			if(abs(Meteo_record_last.Vlaga-Meteo_record.Vlaga)>30 && !timestart && prolaz_v<10)
			{
				Meteo_record.Vlaga=Meteo_record_last.Vlaga;
				Error.Sensor.SHT = 5;
				prolaz_v++;
			}
			else 
			{
				Meteo_record_last.Vlaga = Meteo_record.Vlaga;
				Error.Sensor.SHT = 0;
				prolaz_v = 0;
			}
		}
		fCurrHumidity = (float) Meteo_record.Vlaga;

		///////////////// test podataka solarne temperature na jump i vrijednost
		if( Error_constrain(Meteo_record.Solar_temp,-200,700))	//temperatura x10
		{
			Error.Sensor.DS18D20sol_sensor = 2;
			Meteo_record.Solar_temp=Meteo_record_last.Solar_temp;
		}			
		else 
		{
			
			if(((abs(Meteo_record_last.Solar_temp-Meteo_record.Solar_temp))>50) && !timestart && (prolaz_s<10)) 
			{
				Meteo_record.Solar_temp=Meteo_record_last.Solar_temp;
				Error.Sensor.DS18D20sol_sensor = 5;
				prolaz_s++;
			}
			else 
			{
				Meteo_record_last.Solar_temp = Meteo_record.Solar_temp;
				Error.Sensor.DS18D20sol_sensor = 0;
				prolaz_s = 0;
			}
		}
		fCurrSolar = (float)Meteo_record.Solar_temp/10.0;

			//if( Error_constrain(Meteo_record.Max_brz,0,255))Error.Sensor.;	//ne koristi se u transferu
		if( Error_constrain(Meteo_record.Pros_brz,0,255))Error.Sensor.Wind_speed_sensor= 2; //brzina u prosle  2 sekunde 
		fCurrWindSpeed = (float)Meteo_record.Pros_brz;

		if( Error_constrain(Meteo_record.Padaline, 0,65500)) Error.Sensor.Rain_sensor= 2;	//dnevne padaline
		if (Meteo_record.Padaline >= uiPadalineLast)
		{
			MeteoRecordPadalineLOC = Meteo_record.Padaline;
			fCurrRainfall = (float)Meteo_record.Padaline*tip_bucket_faktor;// nije prelaz na novi dan
		}
		if (timestart)
		{
			uiPadalineLast = Meteo_record.Padaline;
			MeteoRecordPadalineLOC = Meteo_record.Padaline;
		}
		if( Error_constrain(Meteo_record.Smjer,-1,166))Error.Sensor.Wind_smjer_sensor= 2;	//prosjecan smjer u stupnjevi/2
			else Error.Sensor.Wind_smjer_sensor= 0;
		Meteo_record.Smjer=constrain(Meteo_record.Smjer,0,165);
		fCurrWindBearing = (float)Meteo_record.Smjer*2;

		Meteo_record.Rain_rate = constrain(Meteo_record.Rain_rate,0,255);
		if( Error_constrain(Meteo_record.Rain_rate,0,255))Error.Sensor.Rain_sensor=2;	//l na sat  *10
		fCurrRainfallRate = (float) Meteo_record.Rain_rate;

		if( Error_constrain(Meteo_record.Uv,0,130))Error.Sensor.Uv_sensor= 2;
			else  Error.Sensor.Uv_sensor= 0; 	
		Meteo_record.Uv=(unsigned char)(((unsigned int)Meteo_record.Uv * 14)/10);	//korekcija mjerenja za k=1,4
		fCurrUV = (float) Meteo_record.Uv/10;
		Test_data();
		if (Meteo_record.Pros_brz > ucRecordWindGust) ucRecordWindGust = Meteo_record.Pros_brz;
		if (Meteo_record.Rain_rate > ucRecordRainRainrate) ucRecordRainRainrate = Meteo_record.Rain_rate;
		uspjesnoOcitano = true;
	}
	else
	{
		LCD.TextGoTo(39,0);
		LCD.writeStringPgm(F(" "));	//brisi antena
	} 
			
	if ((last2SecTime + 2) < timesec) 
	{
   	last2SecTime = timesec;
   	if (uspjesnoOcitano) twoSecEvents();
		uspjesnoOcitano = false;
	}
	if (now.month() != lastMonth)
   {
   	lastMonth = now.month();
   	monthEvent=true;
   }
	if ((now.dayOfTheWeek()==1) && (now.dayOfTheWeek() != lastDay))
	{
		lastDay = now.dayOfTheWeek();
		//weekEvent=true;
	}
	if (now.day() != today)
   {
   	today = now.day();
   	dayEvent=true;
   }
	if(((now.minute() % RECORD_TIME) == 0) && (now.minute() != lastMinute)) //every after record time
	{
		lastMinute = now.minute();
		recordEvents();
	}

	CheckForConnections();
	while (RemoteClient.connected() && RemoteClient.available())
   {
		const char list[] = "list";
		const char read[] = "read";
		const char send[] = "send";
		// receive incoming UDP packets
		int len = RemoteClient.read((uint8_t*)incomingPacket, 30);
		if (len > 0) incomingPacket[len] = '\0';
		
		if (compare(incomingPacket, list, 4))
		{
			listDir(SD,"/", 0);
		}
		else
		{
			if (compare(incomingPacket, read, 4))
			{
				char pathfile[14];
				memcpy(pathfile, incomingPacket+5, 13);
				pathfile[13]='\0';
				readFile(SD, pathfile);
			}
			else
			{
				if (compare(incomingPacket, send, 4))
				{
					sendEasyWeatherData();
				}
				else
				RemoteClient.printf("Unown command %s\n", incomingPacket);
			}
		}
  }

}
