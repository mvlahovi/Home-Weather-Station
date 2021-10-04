
// include the library code:
#include <LiquidCrystal.h>
#include <avr/pgmspace.h> 
#include <SPI.h> 
#include "nRF24L01.h"
#include "RF24.h"
#include <Wire.h> //needed for I2C
#include <SimpleTimer.h> 
#include <SHT1x.h>
#include <OneWire.h>	// za ds18d20
#include <DallasTemperature.h>  

#define BR_ADCPRES_KONV 5
#define PadalinePin 6

#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

/*
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
Sensor_status_type Error;
*/

#define SHT  _BV(0)
#define DS18D20sol_sensor  _BV(1)
#define MPX_sensor  _BV(2)
#define Wind_smjer_sensor  _BV(3)
#define Wind_speed_sensor  _BV(4)
#define Rain_sensor  _BV(5)
#define Uv_sensor  _BV(6)

struct Meteo_record_type
{
	int Temperatura;		//temperatura x10
	unsigned int Tlak;		//tlak x10
	unsigned char Vlaga;
	int Solar_temp;			//temperatura x10
	unsigned char Max_brz;	//not used
	unsigned char Pros_brz;	//wind speed in two seconds
	unsigned int Padaline;	//dnevne padaline
	unsigned char Smjer;	//average direction in degrees/2
	unsigned char Rain_rate;	//l per hour  *10
	unsigned char Uv;					// *10
};
Meteo_record_type Meteo_record;
Meteo_record_type *p_struct;
float temp_c;
float humidity;
unsigned char got_answer[2];
bool prolaz;

char *Smjer_char[]={" E ","SEE","SSE"," S ","SSW","SWW"," W ","NWW","NNW"," N ","NNE","NEE"};



uint8_t bufferout[sizeof(Meteo_record)];
uint8_t bufferin[sizeof(Meteo_record)];
unsigned int LowPass[10];
char _str[10];	//za pretvorbe

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(19, 3, 8, 9, 10, 11);
static FILE lcdout = {0} ;
static int lcd_putchar(char ch, FILE* stream)
{
    lcd.write(ch) ;
    return (0) ;
}
  RF24 radio(18,12); 
  // Radio pipe addresses for the 2 nodes to communicate.
  const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL }; 
  // the timer object
  SimpleTimer timer;
  
  SHT1x sht1x(22,23);
  
//***********************DS18D20*********************
#define ONE_WIRE_BUS 7
#define TEMPERATURE_PRECISION 12
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
int numberOfDevices; // Number of temperature devices found
DeviceAddress insideThermometer; // We'll use this variable to store a found device address 
unsigned long lastTempRequest = 0;
int  delayInMillis = 0;
float temperatureout = 0.0;
//*****************************************************
unsigned long lastcountbrojac,delta_t,delta_t_half;	//for tip bucket
unsigned char StateFlag,lasttipstatus;
#define oneHour 3600000
//***************************************************** 
volatile unsigned char count;	// for wind speed
unsigned char countertemp,rain_rate_counter;

void setup() 
{
  
  pinMode(PadalinePin, INPUT);
  // set up the LCD's number of columns and rows: 
  lcd.begin(20, 4);
  fdev_setup_stream (&lcdout, lcd_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &lcdout;
    
  p_struct=&Meteo_record;
  
    //********************InitDS18D20*******************************
  // locate devices on the bus
  lcd.setCursor(0,0);	
  printf_P(PSTR("Locating devices..."));
  sensors.begin();
  printf_P(PSTR("Found "));
  printf("%02u",sensors.getDeviceCount());
  printf_P(PSTR(" devices.")); 
  if (sensors.getAddress(insideThermometer, 0))
  {
	sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
	sensors.setWaitForConversion(false);
	sensors.requestTemperatures();
	delayInMillis = 750 / (1 << (12 - TEMPERATURE_PRECISION)); 
	lastTempRequest = millis(); 
  }
  else
  {
	printf_P(PSTR("Unable to find address for Device 0"));
	Meteo_record.Max_brz|=DS18D20sol_sensor;	//senzor error
  }
  radio.begin();
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setDataRate(RF24_250KBPS);
  radio.setPayloadSize(sizeof(Meteo_record));
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]); 
  radio.startListening();
  //radio.printDetails(); 
  
  attachInterrupt(0, Uvecaj_counter, FALLING);
 
  lcd.setCursor(0,3);	
  temp_c = sht1x.readTemperatureC();
  if((temp_c< -30)||(temp_c>70)) {printf_P(PSTR("Unable to find SHT"));Meteo_record.Max_brz|=SHT;	}//greska senzora
  else printf_P(PSTR("Found SHT"));
  delay(2000);	 
  
  timer.setInterval(2000, Read_sensor);
  lcd.clear();
  countertemp=4;
  lastcountbrojac=millis()-oneHour;
  //analogReference(DEFAULT);
  lasttipstatus=digitalRead(PadalinePin);
}

void loop() 
{
  
  timer.run();
   
 // First, stop listening so we can talk.
    radio.stopListening();
//setup buffer
 uint8_t *p_char_struct;
 p_char_struct= (uint8_t*)p_struct; 
 for(unsigned char i=0; i<sizeof(Meteo_record);i++) bufferout[i] = *p_char_struct++; 
 
 bool ok = radio.write( bufferout, sizeof(bufferout) );
  //lcd.setCursor(0, 3); 
  //if (ok) printf("ok...");
  //else printf("failed");

    // Now, continue listening
    radio.startListening(); 
 
   // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      got_answer[0] = 99; 
      got_answer[1] = 99;
    }
    else radio.read( &got_answer, 2); // Grab the response
	
	Update_lcd();
	Tip_update();
	Meteo_record.Smjer=Smjer_update();
	if(got_answer[0]==1 && prolaz) prolaz=FALSE;//one time for day
	if(got_answer[0]==0 && !prolaz) //one time for day
	{
		Meteo_record.Padaline=0;
		lcd.clear();
		prolaz = TRUE;
	}
	
   //delay(1000); 
}


void Update_lcd(void)
{
	lcd.setCursor(15,0);
	printf("%02u:%02u",(int)got_answer[0],(int)got_answer[1]);
	
	lcd.setCursor(0,0);		//temperatura zraka
	printf("Temp=");
        if(Meteo_record.Temperatura<-300 || Meteo_record.Temperatura>700) Meteo_record.Temperatura=999;
	dtostrf((float)Meteo_record.Temperatura/10,5,1, _str);
	printf(_str);
	lcd.print((char)223);printf("C");
	
	lcd.setCursor(0, 1); 
	printf("Tlak=");
	dtostrf((float)Meteo_record.Tlak/10,6,1, _str);
	printf(_str);
	//printf(" hPa");
	
	lcd.setCursor(0,3);		//humidity
	printf("Vl=");
	dtostrf(humidity,4,1, _str);
	printf(_str);
	printf(" ");lcd.print((char)37);
	
	lcd.setCursor(0,2);		//solar temp
	printf("Sol=");
	dtostrf((float)Meteo_record.Solar_temp/10,5,1, _str);
	printf(_str);
	lcd.print((char)223);printf("C");

    lcd.setCursor(12,3);		//padaline
	printf("Pad=");
	dtostrf((float)Meteo_record.Padaline,3,0, _str);
	printf(_str);
	
	lcd.setCursor(13,1);		//padaline
	printf("Smj=");
	//dtostrf((float)Meteo_record.Smjer,3,0, _str);
	//printf(_str);
	if(Meteo_record.Smjer==200) printf("---");
	else
	printf(Smjer_char[Meteo_record.Smjer/15]);
	/*
	lcd.setCursor(13,2);		//padaline
	printf("Brz=");
	dtostrf((float)Meteo_record.Pros_brz,3,0, _str);
	printf(_str);
	*/
	lcd.setCursor(13,2);		//padaline
	printf("Uv=");
	dtostrf((float)Meteo_record.Uv/10,4,1, _str);
	printf(_str);
	
	
}

	
	


void Read_sensor(void)	//svako 2 sec
{
	/***********UV********************/
	ADMUX = (_BV(REFS0)|_BV(MUX2)|_BV(MUX0));			//ref. voltage VCC,ADC5
	delay(2);							//must be delay
  
 float uvIndex;
 int uvIndexValue [12] = { 50, 227, 318, 408, 503, 606, 696, 795, 881, 976, 1079, 1170};

  unsigned int uv = GetADCConversionAVG(3) * 4.875;// Get UV value
  if (uv>1170)
    uv=1170;
    
  unsigned char i;
  for (i = 0; i < 12; i++)
  {
    if (uv <= uvIndexValue[i]) 
    {
      uvIndex = i;
      break;
    }
  }
  
  //calculate 1 decimal if possible
  if (i>0) {
    float vRange=uvIndexValue[i]-uvIndexValue[i-1];
    float vCalc=uv-uvIndexValue[i-1];
    uvIndex+=(1.0/vRange)*vCalc-1.0;
  }
	Meteo_record.Uv = (unsigned char)(uvIndex*10.0);
	
	/**********VJETAR***********/
	Meteo_record.Pros_brz=count;
	count=0;
	
	/*********PRESSURE**********/
	unsigned long SUM_ADCPRESSURE;
	unsigned int ADCPRESSURE;
        //unsigned char i;
	
	ADMUX = (_BV(REFS0)|_BV(MUX3)|_BV(MUX0));			//ref. voltage VCC,ADC0 i ADC1 10X
	delay(2);							//must be delay obavezno
	SUM_ADCPRESSURE=0;
	for (i=0;i<BR_ADCPRES_KONV;i++)
	{							
		ADCPRESSURE=GetADCConversionAVG(1);
		if((ADCPRESSURE&0x0200)==0x0200)		//if negativ
		{
			ADCPRESSURE=(~ADCPRESSURE)&0x01ff;
			ADCPRESSURE=(1000*(5416+(unsigned long)ADCPRESSURE))/542;
		}
		else
		{
		ADCPRESSURE=(1000*(5416-(unsigned long)ADCPRESSURE))/542;
		}
		SUM_ADCPRESSURE=SUM_ADCPRESSURE+ADCPRESSURE;
	}

	ADMUX = (_BV(REFS0)|_BV(MUX2));  //ref. voltage VCC,ADC4 for wind dir

	for (i=0;i<9;i++)
	{
		LowPass[i]=LowPass[i+1];
	}
	LowPass[9]=(unsigned int)(SUM_ADCPRESSURE/BR_ADCPRES_KONV);

	unsigned long SumLowPass=0;
	for(i=0;i<10;i++)
	{
		SumLowPass=(unsigned long)(LowPass[i])+SumLowPass;
	}
	Meteo_record.Tlak=(unsigned int)((SumLowPass/10)+40);	//40 calibration error
	
	/*****************Temperatura i vlaga******************************/
	countertemp++;
	if(countertemp>4)
	{
		temp_c = sht1x.readTemperatureC();
		humidity = sht1x.readHumidity();
		Meteo_record.Temperatura = round(temp_c*10);
		Meteo_record.Vlaga = (unsigned char)(humidity+0.5);
		/*****************Solar*******************************************/
		if (millis() - lastTempRequest >= delayInMillis) Read_DS18D20();// waited long enough?? 
		Meteo_record.Solar_temp = round(temperatureout*10);
		countertemp=0;
	}
	
}

//////////////////////////////////////////////////////////////////////////////
unsigned int GetADCConversionAVG (unsigned char AVG_Number)
{
   unsigned char ADCSR_data,ADCH_data,ADCL_data,i;    // ADC Register Variables
   unsigned int ADC_16;   // Variable that holds 16 bit ADC Value
   ADC_16 = 0;                              // Initialise this variable to zero.
   for(i=0;i<AVG_Number;i++)                // Were taking the average AVG_Number samples.
   {
	 // MCUCR = (_BV(SE)|_BV(SM0)|inp(MCUCR),MCUCR);	  //enable sleep mode

      ADCSRA = (_BV(ADEN)|_BV(ADSC)|0x07); 	  // Start a conversion

      do ADCSR_data = ADCSRA; while( (_BV(ADIF) & ADCSR_data) != 0x10);    // Get the ADC Value
      ADCSRA = (_BV(ADIF)|ADCSR_data,ADCSRA);                                        // Clear the ADC conv interupt bit
      ADCL_data= ADCL; 			                                          // Read the Low Byte
      ADCH_data= ADCH;	  									// Read the High Byte
	  //outp((0<<SE)|(0<<SM0)|inp(MCUCR),MCUCR);					// Isključi sleep mode
      ADC_16 = ((ADCH_data * 0x100) + ADCL_data) + ADC_16;                    // Convert the two 8-bit bytes into a 16 bit word

   }
   ADC_16 = ADC_16 / AVG_Number;             // We are taking the average of AVG_Number samples.
   return(ADC_16); 			                  // Return the 16-Bit Word
}



//////////////////////////////////////////////////////////////////////////////

void Read_DS18D20(void)
{
    temperatureout = sensors.getTempCByIndex(0);
    // immediately after fetching the temperature we request a new sample 
	// in the async modus
    sensors.requestTemperatures(); 
    delayInMillis = 750 / (1 << (12 - TEMPERATURE_PRECISION));
    lastTempRequest = millis(); 
} 

/***************RUTINE ZA OCITAVANJE STATUSA TIP BUCKETA************/

void Tip_update(void )
{
	unsigned char T;
	unsigned long now_time;

	T=Tip();
	now_time=millis();
	if((T==1)&&(now_time!=lastcountbrojac))
	{
		Meteo_record.Padaline++;
		delta_t=delta_t + now_time - lastcountbrojac;
		
		if(rain_rate_counter)
		{
			Meteo_record.Rain_rate=(unsigned char)(((double) 18000/(double)delta_t)*(double)77);
			delta_t_half=delta_t;
			delta_t=0;
			rain_rate_counter=0;
		}
		else rain_rate_counter++;
		/*
		if(delta_t_half>0) 
			{
				delta_t = delta_t_half + now_time - lastcountbrojac;
				delta_t_half=0;
				Meteo_record.Rain_rate=(unsigned char)(((double) 18000/(double)delta_t)*(double)77);
			}
			else delta_t_half = now_time - lastcountbrojac;
		//if (delta_t_half>delta_t)Meteo_record.Rain_rate/=2;
		*/
		StateFlag=0;
		lastcountbrojac=now_time;
	}
	if(Meteo_record.Rain_rate )
	{
		if((now_time - lastcountbrojac)>(2*delta_t_half)){Meteo_record.Rain_rate/=2;delta_t_half*=2;}
	
	}
	else {delta_t_half=0;}
}

//////////////////////////////////////////////////////////////////////////////
unsigned char Tip(void)
{
	unsigned char tipstatus;

	tipstatus=digitalRead(PadalinePin);	//unijeti port i pin
	if (tipstatus!=lasttipstatus)
	{
		StateFlag=1;
		lasttipstatus=tipstatus;
	}
	else StateFlag=0;

    return(StateFlag);
}
/***************************INTERAPT RUTINA ZA BRZINU VJETRA*************/
void Uvecaj_counter(void)	//counter for wind speed
{
	count++;
}

unsigned char Smjer_update(void)	// u main petlji
{
	unsigned int adcsmjer;
        unsigned char smjer;
	adcsmjer=GetADCConversionAVG (1);
	//adcsmjer=analogRead(4);
	if(adcsmjer>705)smjer=0;
		else
		{
			if(adcsmjer>684)smjer=15;
			else
			{
				if(adcsmjer>660)smjer=30;
				else
				{
					if(adcsmjer>634)smjer=45;
					else
					{
						if(adcsmjer>603)smjer=60;
						else
						{
							if(adcsmjer>566)smjer=75;
							else
							{
								if(adcsmjer>521)smjer=90;
								else
								{
									if(adcsmjer>467)smjer=105;
									else
									{
										if(adcsmjer>401)smjer=120;
										else
										{
											if(adcsmjer>323)smjer=135;
											else
											{
												if(adcsmjer>208)smjer=150;
												else
												{
													if(adcsmjer>70)smjer=165;
													else smjer=0;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

	if((adcsmjer<1022)&&(adcsmjer>760)) {smjer=200;Meteo_record.Max_brz|=Wind_smjer_sensor;} //greska senzora		//greska
	return smjer;
}

