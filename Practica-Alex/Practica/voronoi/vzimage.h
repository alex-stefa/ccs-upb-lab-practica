/* Copyright (C) 1996, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.
   This file is part of Voronoize.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.

   Dragos Tarcatu <dragos.tarcatu@cti.pub.ro>
   Liviu Frateanu <liviu.frateanu@cti.pub.ro>
*/


#ifndef VZIMAGE_H
#define VZIMAGE_H

#include <QImage>
#include <QVector>
#include <QPoint>
#include <QLineF>

#include "vzmatrix.h"
#include "vzgraphicsobj.h"
#include "vzdiagram.h"
#include "vzdelaunay.h"


class VzImage
{
public:
    VzImage();
    VzImage(const QImage& image);
    ~VzImage();

    QSize size() const;

    void loadImage(const QImage& image);
    QImage getImage() const;

    QVector<QLineF> voronoize();
    QVector<QLineF> triangulate();

private:
    QImage image;
    VzMatrix map;
    QRgb bkgColor;
    QVector<VzGraphicsObj*> objs;

    static QVector<QPoint> objFill;

private:
    void clearObjects();
    void fillObjects();
    void fillObjectAt(const QPoint& point);

    static QVector<QPoint> GetFillVector();
};


#endif //VZIMAGE_H
