#ifndef POINT_H
#define POINT_H

#include <QFloat16>
#include <cmath>
using namespace std;
const double pi = 3.14159;

class Point
{
public:
    Point();
    ~Point();

    double latitude() const;
    double longitude() const;
    double azimuth(double lat, double lon) const;

    void setLatitude(double lat);
    void setLongitude(double lon);
private:
    double m_latitude;
    double m_longitude;
};

#endif // POINT_H
