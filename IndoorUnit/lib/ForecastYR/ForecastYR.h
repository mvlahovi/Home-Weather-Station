#include "Arduino.h"
#include "ParserLib.h"
#include "T6963.h"

#include "New_Weather\Clear_sky.h"
#include "New_Weather\Fair.h"
#include "New_Weather\Partly_cloudy.h"
#include "New_Weather\Cloudy.h"		
#include "New_Weather\Light_rain_showers.h"			
#include "New_Weather\Rain_showers.h"		
#include "New_Weather\Heavy_rain_showers.h"		
#include "New_Weather\Light_rain_showers_and_thunder.h"		
#include "New_Weather\Rain_showers_and_thunder.h"	
#include "New_Weather\Heavy_rain_showers_and_thunder.h"		
#include "New_Weather\Light_sleet_showers.h"		
#include "New_Weather\Sleet_showers.h"		
#include "New_Weather\Heavy_sleet_showers.h"		
#include "New_Weather\Light_sleet_showers_and_thunder.h"	
#include "New_Weather\Sleet_showers_and_thunder.h"
#include "New_Weather\Heavy_sleet_showers_and_thunder.h"
#include "New_Weather\Light_snow_showers.h"		
#include "New_Weather\Snow_showers.h"	
#include "New_Weather\Heavy_snow_showers.h"	
#include "New_Weather\Light_snow_showers_and_thunder.h"	
#include "New_Weather\Snow_showers_and_thunder.h"	
#include "New_Weather\Heavy_snow_showers_and_thunder.h"		
#include "New_Weather\Light_rain.h"		
#include "New_Weather\Rain.h"		
#include "New_Weather\Heavy_rain.h"		
#include "New_Weather\Light_rain_and_thunder.h"	
#include "New_Weather\Rain_and_thunder.h"		
#include "New_Weather\Heavy_rain_and_thunder.h"		
#include "New_Weather\Light_sleet.h"		
#include "New_Weather\Sleet.h"		
#include "New_Weather\Heavy_sleet.h"		
#include "New_Weather\Light_sleet_and_thunder.h"	
#include "New_Weather\Sleet_and_thunder.h"	
#include "New_Weather\Heavy_sleet_and_thunder.h"		
#include "New_Weather\Light_snow.h"		
#include "New_Weather\Snow.h"		
#include "New_Weather\Heavy_snow.h"		
#include "New_Weather\Light_snow_and_thunder.h"		
#include "New_Weather\Snow_and_thunder.h"	
#include "New_Weather\Heavy_snow_and_thunder.h"	
#include "New_Weather\Fog.h"
#include "New_Weather\Default.h"	
#include "New_Weather\Heavy_rain_showers_and_thunder_n.h"
#include "New_Weather\Heavy_rain_showers_n.h"
#include "New_Weather\Heavy_sleet_showers_and_thunder_n.h"
#include "New_Weather\Heavy_sleet_showers_n.h"
#include "New_Weather\Heavy_snow_showers_and_thunder_n.h"
#include "New_Weather\Heavy_snow_showers_n.h"
#include "New_Weather\Light_rain_showers_and_thunder_n.h"
#include "New_Weather\Light_rain_showers_n.h"
#include "New_Weather\Light_sleet_showers_and_thunder_n.h"
#include "New_Weather\Light_sleet_showers_n.h"
#include "New_Weather\Light_snow_showers_and_thunder_n.h"
#include "New_Weather\Light_snow_showers_n.h"
#include "New_Weather\Partly_cloudy_n.h"
#include "New_Weather\Rain_showers_and_thunder_n.h"
#include "New_Weather\Rain_showers_n.h"
#include "New_Weather\Sleet_showers_and_thunder_n.h"
#include "New_Weather\Sleet_showers_n.h"
#include "New_Weather\Snow_showers_and_thunder_n.h"
#include "New_Weather\Snow_showers_n.h"
#include "New_Weather\Fair_n.h"
#include "New_Weather\Clear_sky_n.h"




#define RL_MAX_CHARS 200    
#define BROJ_PROGNOZA 6
#define BROJ_UCITANIH_PROGNOZA 14
#define RAZDOBLJE 12

//#define DEBAGIRANJE

extern T6963 LCD;
extern int cIndex;
extern char line[RL_MAX_CHARS];
extern int line_number;
extern int korak; 
extern byte prognoza[BROJ_PROGNOZA][2];	
extern bool gotova_prognoza;

extern Parser parser;

	typedef struct {
			uint16_t godina;
			uint8_t mjesec;
			uint8_t datum;
			uint8_t sat;
			uint8_t min;
			uint8_t symbol;
			float preci;
			float windd;
			float winds;
			float temp;
			float press;
		} prognoza_st;

	
extern prognoza_st *p_prognoza;
extern prognoza_st *p_prognoza_konacna;

enum weather_symbols
{
	Clear_sky,
	Fair,
	Partly_cloudy,
	Cloudy,			
	Light_rain_showers,			
	Rain_showers,			
	Heavy_rain_showers,			
	Light_rain_showers_and_thunder,			
	Rain_showers_and_thunder,		
	Heavy_rain_showers_and_thunder,			
	Light_sleet_showers,			
	Sleet_showers,			
	Heavy_sleet_showers,			
	Light_sleet_showers_and_thunder,		
	Sleet_showers_and_thunder,
	Heavy_sleet_showers_and_thunder,
	Light_snow_showers,			
	Snow_showers,		
	Heavy_snow_showers,		
	Light_snow_showers_and_thunder,		
	Snow_showers_and_thunder,		
	Heavy_snow_showers_and_thunder,			
	Light_rain,			
	Rain,			
	Heavy_rain,			
	Light_rain_and_thunder,		
	Rain_and_thunder,			
	Heavy_rain_and_thunder,			
	Light_sleet,			
	Sleet,			
	Heavy_sleet,			
	Light_sleet_and_thunder,		
	Sleet_and_thunder,		
	Heavy_sleet_and_thunder,			
	Light_snow,			
	Snow,			
	Heavy_snow,			
	Light_snow_and_thunder,			
	Snow_and_thunder,		
	Heavy_snow_and_thunder,		
	Fog,
	Default,
	Heavy_rain_showers_and_thunder_n,
	Heavy_rain_showers_n,
	Heavy_sleet_showers_and_thunder_n,
	Heavy_sleet_showers_n,
	Heavy_snow_showers_and_thunder_n,
	Heavy_snow_showers_n,
	Light_rain_showers_and_thunder_n,
	Light_rain_showers_n,
	Light_sleet_showers_and_thunder_n,
	Light_sleet_showers_n,
	Light_snow_showers_and_thunder_n,
	Light_snow_showers_n,
	Partly_cloudy_n,
	Rain_showers_and_thunder_n,
	Rain_showers_n,
	Sleet_showers_and_thunder_n,
	Sleet_showers_n,
	Snow_showers_and_thunder_n,
	Snow_showers_n,
	Fair_n,
	Clear_sky_n
};
const char* const weather_symbols_tbl[] PROGMEM = {
	Clear_skybmp,
	Fairbmp,
	Partly_cloudybmp,
	Cloudybmp,			
	Light_rain_showersbmp,			
	Rain_showersbmp,			
	Heavy_rain_showersbmp,			
	Light_rain_showers_and_thunderbmp,			
	Rain_showers_and_thunderbmp,		
	Heavy_rain_showers_and_thunderbmp,			
	Light_sleet_showersbmp,			
	Sleet_showersbmp,			
	Heavy_sleet_showersbmp,			
	Light_sleet_showers_and_thunderbmp,		
	Sleet_showers_and_thunderbmp,
	Heavy_sleet_showers_and_thunderbmp,
	Light_snow_showersbmp,			
	Snow_showersbmp,		
	Heavy_snow_showersbmp,		
	Light_snow_showers_and_thunderbmp,		
	Snow_showers_and_thunderbmp,		
	Heavy_snow_showers_and_thunderbmp,			
	Light_rainbmp,			
	Rainbmp,			
	Heavy_rainbmp,			
	Light_rain_and_thunderbmp,		
	Rain_and_thunderbmp,			
	Heavy_rain_and_thunderbmp,			
	Light_sleetbmp,			
	Sleetbmp,			
	Heavy_sleetbmp,			
	Light_sleet_and_thunderbmp,		
	Sleet_and_thunderbmp,		
	Heavy_sleet_and_thunderbmp,			
	Light_snowbmp,			
	Snowbmp,			
	Heavy_snowbmp,			
	Light_snow_and_thunderbmp,			
	Snow_and_thunderbmp,		
	Heavy_snow_and_thunderbmp,		
	Fogbmp,
	Defaultbmp,
	Heavy_rain_showers_and_thunder_nbmp,
	Heavy_rain_showers_nbmp,
	Heavy_sleet_showers_and_thunder_nbmp,
	Heavy_sleet_showers_nbmp,
	Heavy_snow_showers_and_thunder_nbmp,
	Heavy_snow_showers_nbmp,
	Light_rain_showers_and_thunder_nbmp,
	Light_rain_showers_nbmp,
	Light_sleet_showers_and_thunder_nbmp,
	Light_sleet_showers_nbmp,
	Light_snow_showers_and_thunder_nbmp,
	Light_snow_showers_nbmp,
	Partly_cloudy_nbmp,
	Rain_showers_and_thunder_nbmp,
	Rain_showers_nbmp,
	Sleet_showers_and_thunder_nbmp,
	Sleet_showers_nbmp,
	Snow_showers_and_thunder_nbmp,
	Snow_showers_nbmp,
	Fair_nbmp,
	Clear_sky_nbmp
};

	void init_forecast(char * line, int sizeof_line);//razdoblje u satima
	void timef();
    void symbolf();
    void precif();
    void winddf();
    void windsf();
    void tempf();
    void presf();
    void spremanje(int prolaz, int pomakni);
    void obrada_prognoze();
    int parse_char(char c);
	void copy_struct(int i);
	void draw_weather_symbol (int x, int y, weather_symbols ws);
	void draw_forecast (int x, int y, int weather, int sat);


