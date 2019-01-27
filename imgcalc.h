#ifndef IMGCALC_H
#define IMGCALC_H

#include <QObject>
#include <QThread>

class ImgCalc : public QThread
{
    Q_OBJECT
public:
    ImgCalc();
};

#endif // IMGCALC_H
