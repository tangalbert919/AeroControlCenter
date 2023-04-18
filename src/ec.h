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
    int setFanMode(unsigned short mode);
    void setCustomFanSpeed(unsigned short speed);
    void setDBus(QDBusInterface *dbus);

public slots:
    void checkEC();

private:
    bool adjustableFanMode;
    QString hwmonDir;
    //QTimer *timer;
    unsigned short fanRPM[2], customFanSpeed, fanMode;
    QDBusInterface *dbus;
};

#endif // EC_H
