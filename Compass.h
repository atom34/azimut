#ifndef COMPASS_H
#define COMPASS_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <cmath>

class Compass : public QWidget
{
     Q_OBJECT
public:
    Compass(QWidget* pwgt = nullptr);
    ~Compass();
    void setAzimut(double azimut);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    //изображение компаса
    QImage m_imageCompas;
    //размер изображения
    QSize m_imageSize;
    //x-координата левого угла окружности
    double m_x_circle_pos;
    //y-координата левого угла окружности
    double m_y_circle_pos;
    //диаметр окружности
    double m_diameter;
    //координаты центра окружности
    double m_x0, m_y0;
    //координаты точки клиента
    double m_x, m_y;
};

#endif // COMPASS_H
