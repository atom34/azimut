#include "TcpClient.h"

TcpClient::TcpClient(const QString &host, int port, QWidget *pwgt)
    : QWidget(pwgt), m_nProgressBarMax(1000), m_nNextBlockSize(0)
{
    setWindowTitle("Клиент");
    setMaximumSize(250, 600);
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost(host, port);
    //---------------------------------------------------------------------------------------------
    m_pProgressBar = new QProgressBar;
    m_pProgressBar->setMinimum(0);
    m_pProgressBar->setMaximum(m_nProgressBarMax);
    m_pProgressBar->setValue(0);
    m_pProgressBar->setTextVisible(false);
    //---------------------------------------------------------------------------------------------
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slotError(QAbstractSocket::SocketError)));
    connect(m_pTcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(slotDisplayConnectionState(QAbstractSocket::SocketState)));
    //---------------------------------------------------------------------------------------------
    QVBoxLayout *pvbxLayout = new QVBoxLayout;
    QGridLayout *pgrdLayout = new QGridLayout;
    //---------------------------------------------------------------------------------------------
    pgrdLayout->addWidget(new QLabel(""), 0, 0);
    pgrdLayout->addWidget(new QLabel("Градусов"), 0, 1);
    pgrdLayout->addWidget(new QLabel("Минут"), 0, 2);
    pgrdLayout->addWidget(new QLabel("Секунд"), 0, 3);
    //---------------------------------------------------------------------------------------------
    pgrdLayout->addWidget(new QLabel("Широта"), 1, 0);

    m_ptxtLatitudeDegrees = new QLineEdit;
    m_ptxtLatitudeDegrees->setObjectName("Latitude");
    m_ptxtLatitudeDegrees->setValidator(new QIntValidator(-90,90));
    connect(m_ptxtLatitudeDegrees, SIGNAL(textChanged(QString)), SLOT(slotSendToServer(QString)));
    pgrdLayout->addWidget(m_ptxtLatitudeDegrees, 1, 1);

    m_ptxtLatitudeMinutes = new QLineEdit;
    m_ptxtLatitudeMinutes->setObjectName("Latitude");
    m_ptxtLatitudeMinutes->setValidator(new QIntValidator(0,59));
    connect(m_ptxtLatitudeMinutes, SIGNAL(textChanged(QString)), SLOT(slotSendToServer(QString)));
    pgrdLayout->addWidget(m_ptxtLatitudeMinutes, 1, 2);

    m_ptxtLatitudeSeconds = new QLineEdit;
    m_ptxtLatitudeSeconds->setObjectName("Latitude");
    m_ptxtLatitudeSeconds->setValidator(new QIntValidator(0,59));
    connect(m_ptxtLatitudeSeconds, SIGNAL(textChanged(QString)), SLOT(slotSendToServer(QString)));
    pgrdLayout->addWidget(m_ptxtLatitudeSeconds, 1, 3);
    //---------------------------------------------------------------------------------------------
    pgrdLayout->addWidget(new QLabel("Долгота"), 2, 0);

    m_ptxtLongitudeDegrees = new QLineEdit;
    m_ptxtLongitudeDegrees->setObjectName("Longitude");
    m_ptxtLongitudeDegrees->setValidator(new QIntValidator(-180,180));
    connect(m_ptxtLongitudeDegrees, SIGNAL(textChanged(QString)), SLOT(slotSendToServer(QString)));
    pgrdLayout->addWidget(m_ptxtLongitudeDegrees, 2, 1);

    m_ptxtLongitudeMinutes = new QLineEdit;
    m_ptxtLongitudeMinutes->setObjectName("Longitude");
    m_ptxtLongitudeMinutes->setValidator(new QIntValidator(0,59));
    connect(m_ptxtLongitudeMinutes, SIGNAL(textChanged(QString)), SLOT(slotSendToServer(QString)));
    pgrdLayout->addWidget(m_ptxtLongitudeMinutes, 2, 2);

    m_ptxtLongitudeSeconds = new QLineEdit;
    m_ptxtLongitudeSeconds->setObjectName("Longitude");
    m_ptxtLongitudeSeconds->setValidator(new QIntValidator(0,59));
    connect(m_ptxtLongitudeSeconds, SIGNAL(textChanged(QString)), SLOT(slotSendToServer(QString)));
    pgrdLayout->addWidget(m_ptxtLongitudeSeconds, 2, 3);
    //---------------------------------------------------------------------------------------------
    pgrdLayout->addWidget(new QLabel("Порт сервера"), 3, 0);
    m_ptxtServerPort = new QLineEdit;
    m_ptxtServerPort->setValidator(new QIntValidator);
    connect(m_ptxtServerPort, SIGNAL(returnPressed()), SLOT(slotConnectToNewPort()));
    pgrdLayout->addWidget(m_ptxtServerPort, 3, 1);
    //---------------------------------------------------------------------------------------------
    pvbxLayout->addLayout(pgrdLayout);
    //---------------------------------------------------------------------------------------------
    m_ptxtInfo = new QTextEdit;
    m_ptxtInfo->setReadOnly(true);
    //---------------------------------------------------------------------------------------------
    pvbxLayout->addWidget(m_pProgressBar);
    pvbxLayout->addWidget(m_ptxtInfo);
    setLayout(pvbxLayout);
    //---------------------------------------------------------------------------------------------
    readSettings();
}
TcpClient::~TcpClient(){}
void TcpClient::closeEvent(QCloseEvent*)
{
    writeSettings();
    m_pTcpSocket->disconnectFromHost();
}
void TcpClient::writeSettings()
{
    QFile file("client_coordinates.txt");
    QTextStream out(&file);

    if(file.open(QFile::WriteOnly | QFile::Truncate))
    {
        out << m_ptxtLatitudeDegrees->text() << '\t'
            << m_ptxtLatitudeMinutes->text() << '\t'
            << m_ptxtLatitudeSeconds->text() << "\r\n"
            << m_ptxtLongitudeDegrees->text() << '\t'
            << m_ptxtLongitudeMinutes->text() << '\t'
            << m_ptxtLongitudeSeconds->text() << "\r\n";
        file.close();
    }
}
void TcpClient::readSettings()
{
    QFile file("client_coordinates.txt");
    QTextStream in(&file);
    QString line;
    QStringList list;

    if(file.open(QFile::ReadOnly))
    {
        line = in.readLine();
        list = line.split('\t');
        m_ptxtLatitudeDegrees->setText(list[0]);
        m_ptxtLatitudeMinutes->setText(list[1]);
        m_ptxtLatitudeSeconds->setText(list[2]);

        line = in.readLine();
        list = line.split('\t');
        m_ptxtLongitudeDegrees->setText(list[0]);
        m_ptxtLongitudeMinutes->setText(list[1]);
        m_ptxtLongitudeSeconds->setText(list[2]);
        file.close();
    }

    double latitude = calcLatitude();
    double longitude = calcLongitude();

    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_5_9);
    out << quint16(2*sizeof(double)) << latitude << longitude;

    m_pTcpSocket->waitForConnected(5000);
    if(m_pTcpSocket->state() == QAbstractSocket::ConnectedState)
        m_pTcpSocket->write(arrBlock);
}
//-------------------------------------------------------------------------------------------------
double TcpClient::convertToFloatingPointValue(int degs, int mins, int secs) const
{
    double seconds = (double)(secs/60.0);
    double minutes = (double)(mins/60.0) + seconds;
    double degrees = (double)degs + minutes;
    return degrees;
}
double TcpClient::calcLatitude() const
{
    int degs = (m_ptxtLatitudeDegrees->text()).toInt();
    int mins = (m_ptxtLatitudeMinutes->text()).toInt();
    int secs = (m_ptxtLatitudeSeconds->text()).toInt();
    return convertToFloatingPointValue(degs, mins, secs);
}
double TcpClient::calcLongitude() const
{
    int degs = (m_ptxtLongitudeDegrees->text()).toInt();
    int mins = (m_ptxtLongitudeMinutes->text()).toInt();
    int secs = (m_ptxtLongitudeSeconds->text()).toInt();
    return convertToFloatingPointValue(degs, mins, secs);
}
QPair<double, double> TcpClient::calcPoint(const QString& lineEditName) const
{
    double latitude;
    double longitude;

    if(lineEditName == "Latitude")
    {
        int degs = (m_ptxtLatitudeDegrees->text()).toInt();
        int mins = (m_ptxtLatitudeMinutes->text()).toInt();
        int secs = (m_ptxtLatitudeSeconds->text()).toInt();
        latitude = convertToFloatingPointValue(degs, mins, secs);
        longitude = calcLongitude();
    }
    else if(lineEditName == "Longitude")
    {
        int degs = (m_ptxtLongitudeDegrees->text()).toInt();
        int mins = (m_ptxtLongitudeMinutes->text()).toInt();
        int secs = (m_ptxtLongitudeSeconds->text()).toInt();
        latitude = calcLatitude();
        longitude = convertToFloatingPointValue(degs, mins, secs);
    }

    return QPair<double,double>(latitude, longitude);
}
//-------------------------------------------------------------------------------------------------
void TcpClient::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_9);
    for(;;)
    {
        if(!m_nNextBlockSize){
            if(m_pTcpSocket->bytesAvailable() < sizeof(quint16)) break;
            in >> m_nNextBlockSize;
        }
        if(m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) break;

        QTime time;
        QString str;
        in >> time >> str;

        m_ptxtInfo->append(time.toString() + " " + str);
        m_nNextBlockSize = 0;
    }
}
void TcpClient::slotError(QAbstractSocket::SocketError err)
{
    QString Error =
            "Ошибка: " + (err == QAbstractSocket::HostNotFoundError ?
                    "Хост не найден. " :
                             err == QAbstractSocket::RemoteHostClosedError ?
                                 "Удаленный хост закрыт. " :
                                 err == QAbstractSocket::ConnectionRefusedError ?
                                     "Соединение не установлено. ":
                                     QString(m_pTcpSocket->errorString()));
    m_ptxtInfo->append(Error);
}
void TcpClient::slotSendToServer(const QString& str)
{
    Q_UNUSED(str);

    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);

    QLineEdit *ptxt = qobject_cast<QLineEdit*>(sender());
    QString lineEditName = ptxt->objectName();

    QPair<double,double> p = calcPoint(lineEditName);
    double latitude = p.first;
    double longitude = p.second;

    out << quint16(2*sizeof(double)) << latitude << longitude;
    m_pTcpSocket->write(arrBlock);
}
void TcpClient::slotConnected()
{
    m_ptxtInfo->append("Получен сигнал от сервера. ");
}
//-------------------------------------------------------------------------------------------------
void TcpClient::slotDisplayConnectionState(QAbstractSocket::SocketState state)
{
    switch (state) {
    case QAbstractSocket::UnconnectedState: { m_pProgressBar->setValue(0); break; }
    case QAbstractSocket::HostLookupState: { m_pProgressBar->setValue(m_nProgressBarMax/4); break; }
    case QAbstractSocket::ConnectingState: { m_pProgressBar->setValue(m_nProgressBarMax/2); break; }
    case QAbstractSocket::ConnectedState:
    {
        for(int i = 0; i < m_nProgressBarMax; ++i)
        {
            m_pProgressBar->setValue(i);
            qApp->processEvents();
        }
        break;
    }
    case QAbstractSocket::BoundState: { m_pProgressBar->setValue(m_nProgressBarMax/3); break; }
    case QAbstractSocket::ClosingState: { m_pProgressBar->setValue(0); break; }
    case QAbstractSocket::ListeningState: { m_pProgressBar->setValue(m_nProgressBarMax/5); break; }
    default: break;
    }
}
void TcpClient::slotConnectToNewPort()
{
    QLineEdit *pServerPort = qobject_cast<QLineEdit*>(sender());
    quint16 port{};
    if(pServerPort) port = (pServerPort->text()).toUShort();

    if(port)
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->connectToHost("localhost", port);
    }
}
//-------------------------------------------------------------------------------------------------
