#ifndef EC_H
#define EC_H

#include <QTimer>

class EC : public QObject
{
    Q_OBJECT
public:
    EC();
    ~EC();
    QTimer *timer;

    unsigned short *getFanRPM();

public slots:
    void checkEC();

private:
    QString hwmonDir;
    unsigned short fanRPM[2];

    void setFanMode(unsigned int mode);
};

#endif // EC_H
