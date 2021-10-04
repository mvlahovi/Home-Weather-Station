
#include "Arduino.h"

double Latitude(int stupnjevi, int minute, bool hemisphere);
int DaysInYear(int day, int month,bool leap_year);
double SezoneCorection(int J);
double DegreesToRadians(int stupnjevi, int minute);
double SolarTimeAngle(double time, double longitute_time_zone, double longitude, int day, int month, bool leap_year);
double RelativeDistance(int day, int month, bool leap_year);//dr
double SolarDeclination(int day, int month, bool leap_year);//delta
double SunsetHourAngle(double latitude, int day, int month, bool leap_year);//omegas
double DayLightHour(int latitude, int day, int month, bool leap_year);//N
double ExtraterrestrialRadiationHourly(double latitude, double longitude, double longitute_time_zone,double time, int day, int month, bool leap_year);
double SnagaRadiacije(double ETRHourly);
double SolarRadiation(double ETRHourly, int cloud_cover);//Rs  cloud cover u postotcima
double ClearSkySolarRadiation(double ETRHourly, int elevation);//Rso
double VapourPress(double celcius);//e0 saturation vapour pressure
double ActualVapourPress(double celcius, double humidity);//ea
double SlopeSaturationVapourPress(double celcius);
double RadiationLongwave(double celcius,double humidity,int cloud_cover,int elevation,double latitude, double longitude, double longitute_time_zone,double time, int day, int month, bool leap_year);//Rnl
double EvapoTranspirationHourly(double celcius, double humidity,double airpressure,double average_wind, int cloud_cover,double time, int elevation, double longitute_time_zone, double longitude, double latitude,int day, int month, bool leap_year);
