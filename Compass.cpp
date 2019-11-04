#include "Compass.h"

Compass::Compass(QWidget *pwgt) : QWidget(pwgt)
{   
    m_imageSize = QSize(250, 250);
    setMinimumSize(m_imageSize);

    m_x_circle_pos = m_imageSize.width()*0.10;
    m_y_circle_pos = m_imageSize.height()*0.10;
    m_diameter = m_imageSize.width()*0.8;

    m_x0 = m_x_circle_pos + m_diameter/2;
    m_y0 = m_y_circle_pos + m_diameter/2;
    m_x = m_x_circle_pos + m_diameter/2;
    m_y = m_y_circle_pos;
}
Compass::~Compass(){}
//-------------------------------------------------------------------------------------------------
void Compass::setAzimut(double azimut)
{
    //азимут в радианах
    m_x = m_x0 + (m_diameter/2)*sin(azimut);
    m_y = m_y0 - (m_diameter/2)*cos(azimut);
    update();
}
//-------------------------------------------------------------------------------------------------
void Compass::paintEvent(QPaintEvent*)
{
    m_imageCompas = QImage(m_imageSize, QImage::Format_ARGB32_Premultiplied);
    m_imageCompas.fill(Qt::white);
    QPainter painter(this);
    //---------------------------------------------------------------------------------------------
    painter.begin(&m_imageCompas);
    //непрозрачный режим
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.end();
    //=============================================================================================
    painter.begin(&m_imageCompas);
    //---------------------------------------------------------------------------------------------
    //рисование окружности
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    QRectF rect(m_x_circle_pos, m_y_circle_pos, m_diameter, m_diameter);
    painter.drawEllipse(rect);
    //рисование точки сервера
    painter.setPen(QPen(Qt::blue, 5, Qt::SolidLine));
    double scale_point = 0.02;
    QRectF point(m_x_circle_pos + m_diameter*(1 - scale_point)/2,
                 m_y_circle_pos + m_diameter*(1 - scale_point)/2,
                 m_diameter*scale_point, m_diameter*scale_point);
    painter.drawEllipse(point);
    //---------------------------------------------------------------------------------------------
    //рисование текста
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    QFont font("Times New Roman", 12);
    painter.setFont(font);
    painter.drawText(m_imageSize.width()/2 - m_imageSize.width()*0.02, m_y_circle_pos*0.75, "N");
    painter.drawText(m_imageSize.width()/2 - m_imageSize.width()*0.02,
                     m_y_circle_pos + m_diameter + m_y_circle_pos*0.75, "S");
    painter.drawText(m_x_circle_pos*0.25, m_y_circle_pos + m_diameter/2 - m_imageSize.height()*0.02, "W");
    painter.drawText(m_x_circle_pos + m_diameter + m_imageSize.height()*0.02,
                     m_y_circle_pos + m_diameter/2, "E");
    //---------------------------------------------------------------------------------------------
    //рисование радиуса   
    QLineF radius(m_x0, m_y0, m_x, m_y);
    painter.setPen(QPen(Qt::red, 1, Qt::SolidLine));
    painter.drawLine(radius);
    //---------------------------------------------------------------------------------------------
    painter.end();
    //=============================================================================================
    //отображение компаса на виджете
    painter.begin(this);
    painter.drawImage(0, 0, m_imageCompas.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    painter.end();
}
//-------------------------------------------------------------------------------------------------
