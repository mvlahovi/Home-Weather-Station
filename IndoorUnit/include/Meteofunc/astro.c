
const uint8_t daysInMonth [] PROGMEM= { 31,28,31,30,31,30,31,31,30,31,30 };

unsigned char isDst(unsigned char day, unsigned char month, unsigned char dow)
    {
        if (month < 3 || month > 10)  return false; 
        if (month > 3 && month < 10)  return true; 

        int previousSunday = (int)day - (int)dow;

        if (month == 3)  return previousSunday >= 25;
        if (month == 10)  return previousSunday < 25;

        return false; // this line never gonna happend
    }

uint32_t dateTimeToLong(uint16_t y, uint8_t m, uint8_t d ,uint8_t h, uint8_t min, uint8_t s) {
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)
        ++days;
    return (((days + 365 * y + (y + 3) / 4 - 1) * 24L + h) * 60 + min) * 60 + s;
}


void dateTimeLong2Str (uint32_t t, char* dstr) 
{
    uint8_t mm, ss, hh, d, m, yOff;
    uint16_t y;

    //t -= SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970
    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0; ; ++yOff) {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m) {
        uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
    y = 2000 + yOff;
    sprintf(dstr,"%02u/%02u/%04u %02u:%02u:%02u",(uint)d, (uint)m, y, (uint)hh, (uint)mm,(uint)ss);
}

char * Deblank(char *str)
{
	char *out = str, *put = str;

	for(; *str != '\0'; ++str)
	{
		if(*str == '.') *str=',';
		if(*str != ' ')
			*put++ = *str;
	}
	*put = '\0';

	return out;
}

float calc_dewpoint(float h,float t)
//--------------------------------------------------------------------
// calculates dew point
// input:   humidity [%RH], temperature [?C]
// output:  dew point [?C]
{ 
	float logEx,dew_point;
	logEx=0.66077+7.5*t/(237.3+t)+(log10(h)-2);
	dew_point = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);
	return dew_point;
}

float Celcius2Farenhait(float celcius)
{
	return celcius*9.0/5.0+32.0;
}
float Farenhait2Celcius(float farenhait)
{
	return (farenhait-32.0)*5.0/9.0;
}
//--------------------------------------------------------------------
float calc_heatindex( float celcius, float hum)
//--------------------------------------------------------------------
// input:   humidity [%RH], temperature [?C]
{
	float c1= -42.379;
	float c2= 2.04901523;
	float c3= 10.14333127;
	float c4= -0.22475541;
	float c5= -6.83783E-3;
	float c6= -5.481717E-2;
	float c7= 1.22874E-3;
	float c8= 8.5282E-4;
	float c9= -1.99E-6;
	float far,hifar;

	far=Celcius2Farenhait(celcius);
	if((far<80.0)||(hum<40.0)) return (celcius);//temp>26,6
	hifar=c1+c2*far+c3*hum+c4*far*hum+c5*far*far+c6*hum*hum+c7*far*far*hum+c8*far*hum*hum+c9*far*far*hum*hum;
	return(Farenhait2Celcius(hifar));
}
//--------------------------------------------------------------------
float calc_chillindex( float celcius, float wind)
//--------------------------------------------------------------------
// input:   wind [kmh], temperature [?C]
{
	float c1= 13.12;
	float c2= 0.6215;
	float c3= -11.37;
	float c4= 0.3965;
	float expo;

	if((celcius>10.0)||(wind<4.8)) return (celcius);
	expo=pow(wind,0.16);
	return(c1+c2*celcius+c3*expo+c4*celcius*expo);
}
//--------------------------------------------------------------------
float calc_vapour_press( float celcius,float humidity)
//--------------------------------------------------------------------
// input:   wind [kmh], temperature [?C], humidity [%RH]
{
	return(humidity/100*6.105*exp(17.27*celcius/(237.7+celcius)));
}
//--------------------------------------------------------------------
float calc_apparent_temp( float celcius,float humidity, float wind, float radiation)
//--------------------------------------------------------------------
// input:   wind [kmh], temperature [?C], humidity [%RH], radiation [w/m2]
{
	return(celcius+0.348*calc_vapour_press(celcius,humidity)-0.194*wind+(0.07*radiation-10)/(wind*0.278+10.0)-4.25);
}
//--------------------------------------------------------------------
int calc_radiation( float celciusamb,float celciussun)
//--------------------------------------------------------------------
// input:    temperature [?C]
{
	float Tsun,Tamb;
	int Esun;


	Tsun=celciussun + 273;
	Tamb=celciusamb+273;
	if (Tamb>=Tsun) Esun=0;
	else
	{
		Esun=(int)((2268.0*(Tsun*Tsun*Tsun*Tsun - Tamb*Tamb*Tamb*Tamb))/10000000000.0);
	}
	return(Esun);
}
//--------------------------------------------------------------------
unsigned int calc_cloud_base( float celciusamb,float dewpoint)
//--------------------------------------------------------------------
// input:    temperature [?C]
{
	return ((unsigned int)(125*(celciusamb-dewpoint)));
}