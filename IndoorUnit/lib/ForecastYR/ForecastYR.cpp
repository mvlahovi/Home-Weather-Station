#include "ForecastYR.h"	
	
	void timef()
      {
        parser.JumpAfter('='); parser.Skip(1);
        (p_prognoza + korak)->godina = parser.Read_Uint16();parser.Skip(1);
        (p_prognoza + korak)->mjesec = parser.Read_Uint8();parser.Skip(1);
        (p_prognoza + korak)->datum = parser.Read_Uint8();parser.Skip(1);
        (p_prognoza + korak)->sat = parser.Read_Uint8();parser.Skip(1);
        (p_prognoza + korak)->min = parser.Read_Uint8();
	#ifdef DEBAGIRANJE	
          Serial.println("vrijeme od ");
          Serial.print((p_prognoza + korak)->godina); Serial.print('-');
          Serial.print((p_prognoza + korak)->mjesec); Serial.print('-');
          Serial.print((p_prognoza + korak)->datum); Serial.print(' ');
          Serial.print((p_prognoza + korak)->sat); Serial.print(':');
          Serial.print((p_prognoza + korak)->min);
          Serial.print("\n\r");
	#endif
          }
    void symbolf()
    {
        parser.JumpAfter('='); parser.Skip(1);
        (p_prognoza + korak)->symbol = parser.Read_Uint16();
	#ifdef DEBAGIRANJE
        Serial.print("symbol "); Serial.print((p_prognoza + korak)->symbol); Serial.print("\n");
	#endif        
    }
    void precif()
    {
        parser.JumpAfter('='); parser.Skip(1);
        (p_prognoza + korak)->preci = parser.Read_UnsignedFloat();
	#ifdef DEBAGIRANJE
        Serial.print("preci "); Serial.print((p_prognoza + korak)->preci); Serial.print("\n");
	#endif
    }
    void winddf()
    {
        parser.JumpAfter('='); parser.Skip(1);
        (p_prognoza + korak)->windd = parser.Read_UnsignedFloat();
	#ifdef DEBAGIRANJE
        Serial.print("wind dir  "); Serial.print((p_prognoza + korak)->windd, 1); Serial.print("\n");
	#endif
    }
    void windsf()
    {
        parser.JumpAfter('='); parser.Skip(1);
        (p_prognoza + korak)->winds = parser.Read_UnsignedFloat();
	#ifdef DEBAGIRANJE
        Serial.print("wind speed(ms) "); Serial.print((p_prognoza + korak)->winds,1); Serial.print("\n");
	#endif
    }
    void tempf()
    {
        parser.JumpAfter('='); parser.Skip(2);
        parser.JumpAfter('='); parser.Skip(1);
        (p_prognoza + korak)->temp = parser.Read_UnsignedFloat();
	#ifdef DEBAGIRANJE
        Serial.print("temp "); Serial.print((p_prognoza + korak)->temp,1); Serial.print("\n");
	#endif
    }
    void presf()
    {
        parser.JumpAfter('='); parser.Skip(2);
        parser.JumpAfter('='); parser.Skip(1);
        (p_prognoza + korak)->press = parser.Read_UnsignedFloat();
	 #ifdef DEBAGIRANJE
        Serial.print("pressure "); Serial.print((double)((p_prognoza + korak)->press), 1); Serial.print("\n");
        Serial.print("\n");
	#endif
		  korak++;
    }
    void spremanje(int prolaz, int pomakni)
    {
        int k = prolaz;
		int n = (prolaz * 2) + pomakni;
      (p_prognoza_konacna+k)->godina = (p_prognoza+n)->godina;
		(p_prognoza_konacna+k)->mjesec = (p_prognoza+n)->mjesec;
		(p_prognoza_konacna+k)->datum = (p_prognoza+n)->datum;
		(p_prognoza_konacna+k)->sat = (p_prognoza+n)->sat;
		(p_prognoza_konacna+k)->min = (p_prognoza+n)->min;
		(p_prognoza_konacna+k)->symbol = (p_prognoza+n+1)->symbol;
		(p_prognoza_konacna+k)->preci = (p_prognoza+n)->preci + (p_prognoza+n+1)->preci;
		(p_prognoza_konacna+k)->winds = max((p_prognoza+n)->winds, (p_prognoza+n+1)->winds);
		(p_prognoza_konacna+k)->windd = max((p_prognoza+n)->symbol, (p_prognoza+n+1)->symbol);
		if ( (p_prognoza + n)->sat == 6 ) (p_prognoza_konacna+k)->temp = max((p_prognoza+n)->temp, (p_prognoza+n+1)->temp);
		else  (p_prognoza_konacna+k)->temp = min((p_prognoza+n)->temp, (p_prognoza+n+1)->temp);
    }
	
	void copy_struct(int i, int p)
	{
		(p_prognoza_konacna+i)->godina = (p_prognoza+i+p)->godina;
		(p_prognoza_konacna+i)->mjesec = (p_prognoza+i+p)->mjesec;
		(p_prognoza_konacna+i)->datum = (p_prognoza+i+p)->datum;
		(p_prognoza_konacna+i)->sat = (p_prognoza+i+p)->sat;
		(p_prognoza_konacna+i)->min = (p_prognoza+i+p)->min;
		(p_prognoza_konacna+i)->symbol = (p_prognoza+i+p)->symbol;
		(p_prognoza_konacna+i)->preci = (p_prognoza+i+p)->preci;
		(p_prognoza_konacna+i)->winds = (p_prognoza+i+p)->winds;
		(p_prognoza_konacna+i)->windd = (p_prognoza+i+p)->symbol;
		(p_prognoza_konacna+i)->temp = (p_prognoza+i+p)->temp;
	}	
	
    void obrada_prognoze()
    {

			if (RAZDOBLJE == 12)
			{
				
				if ((p_prognoza+1)->sat == 0 ||  (p_prognoza+1)->sat == 12 ) 
				{
					//copystr1;usporedi 2 i 3, 4 i 5, 6 i 7,....
					copy_struct(0, 1);
					for (int br_pr = 1; br_pr < BROJ_PROGNOZA; br_pr++)
					{
						spremanje ( br_pr, 0);
					}
				}
				else
				{
					for (int br_pr = 0; br_pr < BROJ_PROGNOZA; br_pr++)
					{
						spremanje ( br_pr, 1);
					}
					//usporedi 1 i 2, 3 i 4,.....
				}
			}
			else
			{
				for (int br_pr = 0; br_pr < BROJ_PROGNOZA; br_pr++)
				{
					copy_struct ( br_pr, 0 );
				#ifdef DEBAGIRANJE
					Serial.print(br_pr); Serial.print(". prognoza konacna"); Serial.print((p_prognoza_konacna + br_pr)->symbol); Serial.print("\n");
				#endif
				}
			}
	}

	int parse_char(char c)
	{
		if ( cIndex > RL_MAX_CHARS ) return -1;
//Serial.print(c);
		if (c != '\n' )
		{
			line[cIndex] = c;
			cIndex++;
		}
		else
		{	
			parser.Init((char*)line, cIndex+1);
			//Serial.print(" Index=");Serial.print(cIndex);
//for (int i=0; i < cIndex+1; i++){	Serial.print(line[i]);}
			parser.JumpTo('<');
			parser.Search((char*)"<time ",6,timef);
			parser.Search((char*)"<symb",5,symbolf);
			parser.Search((char*)"<prec",5,precif);
			parser.Search((char*)"<windD",6,winddf);
			parser.Search((char*)"<windS",6,windsf);
			parser.Search((char*)"<temp",5,tempf);
			parser.Search((char*)"<pres",5,presf);
			parser.Reset();
			if (korak >= BROJ_UCITANIH_PROGNOZA && gotova_prognoza == false) 
			{
				obrada_prognoze();
				gotova_prognoza = true;
			}
			line_number++;
			cIndex = 0;
		}
		return(line_number); 
	}

void draw_weather_symbol (int x, int y, weather_symbols ws)
{
	LCD.drawPic(x, y, (char*)pgm_read_ptr(&(weather_symbols_tbl[ws])), 22, 4);
}

void draw_forecast (int x, int y, int weather, int sat)
{	 
	bool night;
	if ((sat > 5) && (sat < 17)) night = false;
	else night = true;

	switch ( weather )
	{
		case 1: if (night) draw_weather_symbol (x, y, Clear_sky_n);
					else draw_weather_symbol (x, y, Clear_sky);
					 break;
		case 2: if (night) draw_weather_symbol (x, y, Fair_n);
					else draw_weather_symbol (x, y, Fair);
					 break;
		case 3: if (night) draw_weather_symbol (x, y, Partly_cloudy_n);
					else draw_weather_symbol (x, y, Partly_cloudy);
					 break;
		case 4: draw_weather_symbol (x, y, Cloudy);
					 break;
		case 40: if (night) draw_weather_symbol (x, y, Light_rain_showers_n);
					else draw_weather_symbol (x, y, Light_rain_showers);
					 break;
		case 5: if (night) draw_weather_symbol (x, y, Rain_showers_n);
					else draw_weather_symbol (x, y, Rain_showers);
					 break;
		case 41: if (night) draw_weather_symbol (x, y, Heavy_rain_showers_n);
					else draw_weather_symbol (x, y, Heavy_rain_showers);
					 break;
		case 24: if (night) draw_weather_symbol (x, y, Light_rain_showers_and_thunder_n);
					else draw_weather_symbol (x, y, Light_rain_showers_and_thunder);
					 break;
		case 6: if (night) draw_weather_symbol (x, y, Rain_showers_and_thunder_n);
					else draw_weather_symbol (x, y, Rain_showers_and_thunder);
					 break;
		case 25: if (night) draw_weather_symbol (x, y, Heavy_rain_showers_and_thunder_n);
					else draw_weather_symbol (x, y, Heavy_rain_showers_and_thunder);
					 break;
		case 42: if (night) draw_weather_symbol (x, y, Light_sleet_showers_n);
					else draw_weather_symbol (x, y, Light_sleet_showers);
					 break;
		case 7: if (night) draw_weather_symbol (x, y, Sleet_showers_n);
					else draw_weather_symbol (x, y, Sleet_showers);
					 break;
		case 43: if (night) draw_weather_symbol (x, y, Heavy_sleet_showers_n);
					else draw_weather_symbol (x, y, Heavy_sleet_showers);
					 break;
		case 26: if (night) draw_weather_symbol (x, y, Light_sleet_showers_and_thunder_n);
					else draw_weather_symbol (x, y, Light_sleet_showers_and_thunder);
					 break;
		case 20: if (night) draw_weather_symbol (x, y, Sleet_showers_and_thunder_n);
					else draw_weather_symbol (x, y, Sleet_showers_and_thunder);
					 break;
		case 27: if (night) draw_weather_symbol (x, y, Heavy_sleet_showers_and_thunder_n);
					else draw_weather_symbol (x, y, Heavy_sleet_showers_and_thunder);
					 break;
		case 44: if (night) draw_weather_symbol (x, y, Light_snow_showers_n);
					else draw_weather_symbol (x, y, Light_snow_showers);
					 break;
		case 8: if (night) draw_weather_symbol (x, y, Snow_showers_n);
					else draw_weather_symbol (x, y, Snow_showers);
					 break;
		case 45: if (night) draw_weather_symbol (x, y, Heavy_snow_showers_n);
					else draw_weather_symbol (x, y, Heavy_snow_showers);
					 break;
		case 28: if (night) draw_weather_symbol (x, y, Light_snow_showers_and_thunder_n);
					else draw_weather_symbol (x, y, Light_snow_showers_and_thunder);
					 break;
		case 21: if (night) draw_weather_symbol (x, y, Snow_showers_and_thunder_n);
					else draw_weather_symbol (x, y, Snow_showers_and_thunder);
					 break;
		case 29: if (night) draw_weather_symbol (x, y, Heavy_snow_showers_and_thunder_n);
					else draw_weather_symbol (x, y, Heavy_snow_showers_and_thunder);
					 break;
		case 46: draw_weather_symbol (x, y, Light_rain); break;
		case 9: draw_weather_symbol (x, y, Rain); break;
		case 10: draw_weather_symbol (x, y, Heavy_rain); break;
		case 30: draw_weather_symbol (x, y, Light_rain_and_thunder); break;
		case 22: draw_weather_symbol (x, y, Rain_and_thunder); break;
		case 11: draw_weather_symbol (x, y, Heavy_rain_and_thunder); break;
		case 47: draw_weather_symbol (x, y, Light_sleet); break;
		case 12: draw_weather_symbol (x, y, Sleet); break;
		case 48: draw_weather_symbol (x, y, Heavy_sleet); break;
		case 31: draw_weather_symbol (x, y, Light_sleet_and_thunder); break;
		case 23: draw_weather_symbol (x, y, Sleet_and_thunder); break;
		case 32: draw_weather_symbol (x, y, Heavy_sleet_and_thunder); break;
		case 49: draw_weather_symbol (x, y, Light_snow); break;
		case 13: draw_weather_symbol (x, y, Snow); break;
		case 50: draw_weather_symbol (x, y, Heavy_snow); break;
		case 33: draw_weather_symbol (x, y, Light_snow_and_thunder); break;
		case 14: draw_weather_symbol (x, y, Snow_and_thunder); break;
		case 34: draw_weather_symbol (x, y, Heavy_snow_and_thunder); break;
		case 15: draw_weather_symbol (x, y, Fog); break;
		default: draw_weather_symbol (x, y, Default);
	};
}