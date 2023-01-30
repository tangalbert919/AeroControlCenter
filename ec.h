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
    QMessageLogger log;
    unsigned short fanRPM[2];
    unsigned short read16(FILE* f, unsigned char offset);
};

#endif // EC_H
