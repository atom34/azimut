#include "Point.h"

Point::Point() : m_latitude(90), m_longitude(90){}
Point::~Point(){}
//-------------------------------------------------------------------------------------------------
double Point::latitude() const{ return m_latitude; }
double Point::longitude() const{ return m_longitude; }
double Point::azimuth(double lat, double lon) const
{
    //перевод в радианы
    double phi1 = m_latitude*pi/180.0;
    double lam1 = m_longitude*pi/180.0;
    double phi2 = lat*pi/180.0;
    double lam2 = lon*pi/180.0;

    //аргумент азимута
    double arg = cos(phi1)*tan(phi2)/sin(lam2 - lam1) - sin(phi1)/tan(lam2 - lam1);
    //азимут (в исключительных случаях азимут полагается = 90 градусов)
    double az{};
    if(qIsNaN(arg) || qIsInf(arg)) az = pi/2;
    else az = pi/2 - atan(arg);

    if(!qIsNaN(az) && qIsFinite(az)) return az;
    return pi/2;
}
//-------------------------------------------------------------------------------------------------
void Point::setLatitude(double lat){ m_latitude = lat; }
void Point::setLongitude(double lon){ m_longitude = lon; }
//-------------------------------------------------------------------------------------------------
