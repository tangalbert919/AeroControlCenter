#ifndef EC_H
#define EC_H

#include <QTimer>

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

public slots:
    void checkEC();

private:
    bool adjustableFanMode;
    QString hwmonDir;
    //QTimer *timer;
    unsigned short fanRPM[2], customFanSpeed, fanMode;
};

#endif // EC_H
