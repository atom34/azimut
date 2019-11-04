#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QTime>
#include <QProgressBar>
#include <QApplication>
#include <QHostAddress>
#include <QIntValidator>
#include <QFile>

class TcpClient : public QWidget
{
    Q_OBJECT
public:
    TcpClient(const QString& host, int port, QWidget *pwgt = nullptr);
    ~TcpClient();
protected:
    void closeEvent(QCloseEvent*) override;
    void writeSettings();
    void readSettings();
private:
    double convertToFloatingPointValue(int degs, int mins, int secs) const;
    double calcLatitude() const;
    double calcLongitude() const;
    QPair<double, double> calcPoint(const QString& lineEditName) const;
private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer(const QString& str);
    void slotConnected();
    void slotDisplayConnectionState(QAbstractSocket::SocketState);
    void slotConnectToNewPort();
private:
    QTcpSocket *m_pTcpSocket;

    QLineEdit *m_ptxtLatitudeDegrees;
    QLineEdit *m_ptxtLatitudeMinutes;
    QLineEdit *m_ptxtLatitudeSeconds;

    QLineEdit *m_ptxtLongitudeDegrees;
    QLineEdit *m_ptxtLongitudeMinutes;
    QLineEdit *m_ptxtLongitudeSeconds;

    QLineEdit *m_ptxtServerPort;
    QProgressBar *m_pProgressBar;
    int m_nProgressBarMax;

    QTextEdit *m_ptxtInfo;
    quint16 m_nNextBlockSize;
};

#endif // TCPCLIENT_H
