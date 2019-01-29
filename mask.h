#ifndef MASK_H
#define MASK_H

#include <QVector>
#include <QDebug>

enum class MaskType
{
    VerticalMack,
    HorizontalMask
};

class Mask
{
public:
    Mask(int w, int h, MaskType mt);
    QVector<QVector<int> > mask() const;
    int width() const;
    int height() const;

    void setMask(const QVector<QVector<int> > &mask);
    void setWidth(int width);
    void setHeight(int height);

    int maskType() const;
    void setMaskType(MaskType mt);

private:
    int _width;
    int _height;
    MaskType _maskType;
    QVector< QVector<int> > _mask;
    //
};

#endif // MASK_H
