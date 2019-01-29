#include "mask.h"

Mask::Mask(int h, int w, MaskType mt) :
    _height(h), _width(w), _maskType(mt)
{

}

QVector<QVector<int> > Mask::mask() const
{
    return _mask;
}

void Mask::setMask(const QVector<QVector<int> > &mask)
{
    _mask = mask;
}

int Mask::width() const
{
    return _width;
}

void Mask::setWidth(int width)
{
    _width = width;
}

int Mask::height() const
{
    return _height;
}

void Mask::setHeight(int height)
{
    _height = height;
}

int Mask::maskType() const
{
    return _maskType;
}

void Mask::setMaskType(MaskType mt)
{
    _maskType = mt;
}
