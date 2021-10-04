
#include "Evapotranspiration.h"
 
//		#define PI 3.14159
 
 		double Latitude(int stupnjevi, int minute, bool hemisphere)//North=1,South=0
        {
            double latitude;
            latitude = DegreesToRadians(stupnjevi, minute);
            if (!hemisphere) latitude=latitude * (-1);
            return (latitude);
        }
        int DaysInYear(int day, int month,bool leap_year)
        {
            int J;
            J = (int)(275 * month / 9) - 30 + day - 2;
            if (month < 3) J+=2;
            if (leap_year&&(month >2)) J+=1;
            return J;
        }
        double SezoneCorection(int J)
        {
            double sc;
            double b;
            b = (2 * PI * (J - 81)) / 364;
            sc = 0.1645 * sin(2 * b) - 0.1255 * cos(b) - 0.025 * sin(b);
            return sc;
        }
        double DegreesToRadians(int stupnjevi, int minute)
        {
            return (PI / 180.0 * ((double)stupnjevi + (double)minute / 60.0));
        }
        double SolarTimeAngle(double time, double longitute_time_zone, double longitude, int day, int month, bool leap_year)//omega
        {
            double omega;
            int broj_dana;
            broj_dana = DaysInYear(day, month, leap_year);
            omega = PI / 12 * ((time + 0.06667 * (longitute_time_zone - longitude) + SezoneCorection(broj_dana)) - 12);
            return omega;
        }
        double RelativeDistance(int day, int month, bool leap_year)//dr
        {
             int broj_dana;
             broj_dana = DaysInYear(day, month, leap_year);
             return (1+0.033*cos(2*PI/365.0*(double)broj_dana));
        }
        double SolarDeclination(int day, int month, bool leap_year)//delta
        {
             int broj_dana;
             broj_dana = DaysInYear(day, month, leap_year);
             return (0.409*sin(2*PI/365.0*(double)broj_dana-1.39));
        }
        double SunsetHourAngle(double latitude, int day, int month, bool leap_year)//omegas
        {
            return (acos((-1)*tan(latitude) * tan(SolarDeclination(day, month, leap_year))));
        }
        double DayLightHour(int latitude, int day, int month, bool leap_year)//N
        {
            return(24.0/PI*SunsetHourAngle(latitude,day,month,leap_year));
        }
        double ExtraterrestrialRadiationHourly(double latitude, double longitude, double longitute_time_zone,double time, int day, int month, bool leap_year)
        {
            double dr,omega,omegas,omega1,omega2,sinusi,kosinusi,radiation,delta,razlika;

            const double Gsc=0.0820;

            dr=RelativeDistance(day,month,leap_year);//dr
            omega=SolarTimeAngle(time+0.5,longitute_time_zone,longitude,day,month,leap_year);
            omegas=SunsetHourAngle(latitude,day,month,leap_year);
            if (omega < (-omegas) || omega > omegas) return 0.0;
            omega2=omega+PI/24;
            omega1 = omega - PI / 24;
            delta=SolarDeclination(day,month,leap_year);
            sinusi=sin(latitude)*sin(delta);
            kosinusi=cos(latitude)*cos(delta);
            razlika=sin(omega2) - sin(omega1);
            radiation=12*60/PI*Gsc*dr*(sinusi*(omega2-omega1) + kosinusi*razlika);
            return radiation;
        }
        double SnagaRadiacije(double ETRHourly)
        {
            return (ETRHourly / 3600);
        }
        double SolarRadiation(double ETRHourly, int cloud_cover)//Rs  cloud cover u postotcima
        {
            return ((0.25 + 0.5 *(double) (100-cloud_cover) / 100.0) * ETRHourly);
        }
        double ClearSkySolarRadiation(double ETRHourly, int elevation)//Rso
        {
            return ((0.75 + 2E-5 * (double)elevation) * ETRHourly);
        }
        double VapourPress(double celcius)//e0 saturation vapour pressure
        {
            return (0.6108 * exp(17.27 * celcius / (237.3 + celcius)));
        }
        double ActualVapourPress(double celcius, double humidity)//ea
        {
            return (VapourPress(celcius) * humidity / 100);
        }
        double SlopeSaturationVapourPress(double celcius)
        {
            return (4098.0 * (0.6108 * exp(17.27 * celcius / (celcius + 237.3))) / ((celcius + 237.3) * (celcius + 237.3)));
        }
        double RadiationLongwave(double celcius,double humidity,int cloud_cover,int elevation,double latitude, double longitude, double longitute_time_zone,double time, int day, int month, bool leap_year)//Rnl
        {
            const double ro=2.043E-10;
            bool noc = false;
            double Rlw;
            double T4=(celcius+273.16)*(celcius+273.16)*(celcius+273.16)*(celcius+273.16);
            double ea=ActualVapourPress(celcius,humidity);
            double ETRHourly = ExtraterrestrialRadiationHourly(latitude, longitude, longitute_time_zone, time, day, month, leap_year);
            double cssr = ClearSkySolarRadiation(ETRHourly,elevation);
            double cloudenes;
            if(cssr!=0) cloudenes=SolarRadiation(ETRHourly,cloud_cover)/ClearSkySolarRadiation(ETRHourly,elevation);
            else cloudenes = (100-cloud_cover)/100;
            double omega = SolarTimeAngle(time + 0.5, longitute_time_zone, longitude, day, month, leap_year);
            double omegas = SunsetHourAngle(latitude, day, month, leap_year);
            if (omega < (-omegas) || omega > omegas) noc = true;
            if (noc) Rlw = ro * T4 * (0.34 - 0.14 * sqrt(ea)) * (1.35 * ((100-cloud_cover)/100) - 0.35);
            else Rlw= ro * T4 * (0.34 - 0.14 * sqrt(ea)) * (1.35 * cloudenes - 0.35);
            return Rlw;
        }
        double EvapoTranspirationHourly(double celcius, double humidity,double airpressure,double average_wind, int cloud_cover,double time, int elevation, double longitute_time_zone, double longitude, double latitude,int day, int month, bool leap_year)
        {   //u mm/h
            const double albedo=0.23;
            bool noc=false;
            double Ghr;
            double ET0;

            double SSVP=SlopeSaturationVapourPress(celcius);
            double ETRHourly= ExtraterrestrialRadiationHourly(latitude,longitude,longitute_time_zone,time,day,month,leap_year);
            double Rnl=RadiationLongwave(celcius,humidity,cloud_cover,elevation,latitude,longitude,longitute_time_zone,time,day,month,leap_year);
            double Rs=SolarRadiation(ETRHourly,cloud_cover);
            double Rn=(1-albedo)*Rs - Rnl;
            double omega=SolarTimeAngle(time+0.5,longitute_time_zone,longitude,day,month,leap_year);
            double omegas=SunsetHourAngle(latitude,day,month,leap_year);
            if (omega < (-omegas) || omega > omegas) noc=true;
            if(noc) Ghr=0.5*Rn;
            else Ghr=0.1*Rn;
            double gama=0.000664742 *(airpressure/10.0);
            double e0=VapourPress(celcius);
            double ea=ActualVapourPress(celcius,humidity);
						double nazivnik2 = SSVP+gama*(1+0.34*average_wind);
            if(nazivnik2!=0) ET0=(0.408*SSVP*(Rn-Ghr)+gama*37/(celcius+273)*average_wind*(e0-ea))/nazivnik2;
            else ET0 = 0;
            return(ET0);
        }
