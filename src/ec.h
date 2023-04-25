#ifndef EC_H
#define EC_H

#include <QTimer>
#include <QDBusInterface>

class EC : public QObject
{
    Q_OBJECT
public:
    EC();
    ~EC();

    unsigned short *getFanRPM();
    unsigned short getFanMode();
    unsigned short getCustomFanSpeed();
    unsigned short getChargeMode();
    unsigned short getChargeLimit();
    int setFanMode(unsigned short mode);
    void setCustomFanSpeed(unsigned short speed);
    void setChargeLimit(unsigned short limit);
    void setChargeMode(unsigned short mode);
    void setDBus(QDBusInterface *dbus);

public slots:
    void checkEC();

private:
    bool adjustableFanMode;
    QString hwmonDir;
    //QTimer *timer;
    unsigned short fanRPM[2], customFanSpeed, fanMode, chargeMode, chargeLimit;
    QDBusInterface *dbus;
};

#endif // EC_H
