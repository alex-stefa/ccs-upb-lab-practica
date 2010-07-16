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

#include <QtCore>
#include <QtGui>

#include "vzimage.h"





static QPoint GetCenter(const VzGraphicsObj* obj);


QVector<QPoint> VzImage::objFill = VzImage::GetFillVector();




VzImage::VzImage()
    : map(0, 0)
    , bkgColor(qRgb(255, 255, 255))
{
}


VzImage::VzImage(const QImage& image)
    : image(image)
    , map(image.size())
    , bkgColor(qRgb(255, 255, 255))
{
    fillObjects();
}


VzImage::~VzImage()
{
    clearObjects();
}


QSize VzImage::size() const
{
    return image.size();
}


void VzImage::loadImage(const QImage& newImage)
{
    clearObjects();
    image = newImage;
    map = VzMatrix(image.size());
    fillObjects();
    //qDebug("Numar obiecte: %d\n", objs.size());
}


QImage VzImage::getImage() const
{
    return image;
}


QVector<QLineF> VzImage::voronoize()
{
    fillObjects();

    QVector<QPoint> sample;

    foreach (VzGraphicsObj* obj, objs) {
        int index = 0;
        foreach (QPoint pnt, *obj) {            
            if (index == 0) {
                sample.append(pnt);
            }
            index = (index + 1) % 10;
        }
    }
    int count = sample.size();

    float *x = new float[count];
    float *y = new float[count];

    int index = 0;
    foreach (QPoint pnt, sample) {
        x[index] = pnt.x();
        y[index] = pnt.y();
        index++;
    }

    VoronoiDiagramGenerator vdg;
    QVector<QLineF> lines;
    float x1, y1, x2, y2, xs1, ys1, xs2, ys2;
    vdg.generateVoronoi(x, y, (long)count, 0, image.size().width(), 0, image.size().height(), 2);
    
    delete []x;
    delete []y;

    vdg.resetIterator();
    
    while(vdg.getNext(x1, y1, x2, y2, xs1, ys1, xs2, ys2)) {
        if (map.at((int)xs1, (int)ys1) != map.at((int)xs2, (int)ys2))
            lines.append(QLineF(x1, y1, x2, y2));
    }

    return lines;
}


QVector<QLineF> VzImage::triangulate()
{
     if (objs.size() == 0)
        fillObjects();

    VertexSet centers;
    TriangleSet triangles;
    EdgeSet edges;
    QVector<QLineF> rEdges;

    foreach(VzGraphicsObj* obj, objs) {
        centers.insert(GetCenter(obj));
    }

    Delaunay d;
    d.Triangulate(centers, triangles);
    d.TrianglesToEdges(triangles, edges);

    for (EdgeSet::const_iterator eIt = edges.begin(); eIt != edges.end(); eIt++)
        rEdges.append(QLineF(eIt->v1->GetPoint(), eIt->v2->GetPoint()));

    return rEdges;
}



void VzImage::clearObjects()
{
    foreach (VzGraphicsObj* obj, objs)
        delete obj;
    objs.clear();
}


void VzImage::fillObjects()
{
    if (objs.size() == 0) {
        for (int i = 0; i < image.size().width(); i++) {
            for (int j = 0; j < image.size().height(); j++) {
                if (map.at(i, j) == 0) {
                    QPoint point(i, j);
                    if (image.pixel(point) != bkgColor)
                        fillObjectAt(point);
                }
            }
        }
    }
}


void VzImage::fillObjectAt(const QPoint& point)
{
    Q_ASSERT(image.size() == map.size());

    VzGraphicsObj *obj = new VzGraphicsObj;
    QQueue<QPoint> qpoints;

    qpoints.push_back(point);
    map.at(point) = objs.size() + 1;

    int nghCount = 0;
    while (!qpoints.empty()) {
        QPoint crt = qpoints.first();
        qpoints.pop_front();
        nghCount = 0;

        foreach (QPoint p, objFill) {
            QPoint filled(crt.x() + p.x(), crt.y() + p.y());
            if (image.valid(filled) && image.pixel(filled) != bkgColor &&
                map.at(filled) == 0) {
                qpoints.append(filled);
                map.at(filled) = objs.size() + 1;
                nghCount++;
            }
        }

        if (nghCount < 8) //only contour pixels are added
            obj->add(crt);
    }
    objs.append(obj);
}



QVector<QPoint> VzImage::GetFillVector()
{
    QVector<QPoint> vFill;
    vFill.push_back(QPoint(-1, 0));
    vFill.push_back(QPoint(-1, 1));
    vFill.push_back(QPoint(0, 1));
    vFill.push_back(QPoint(1, 1));
    vFill.push_back(QPoint(1, 0));
    vFill.push_back(QPoint(1, -1));
    vFill.push_back(QPoint(0, -1));
    vFill.push_back(QPoint(-1, -1));
    return vFill;
}


QPoint GetCenter(const VzGraphicsObj* obj)
{
    Q_ASSERT(obj != 0);
    VzGraphicsObj::const_iterator objIt = obj->begin();
    double xCenter = objIt->x(), yCenter = objIt->y();
    int objnum = 1;

    for (; objIt != obj->end(); objIt++) {
        xCenter = (objnum * xCenter + objIt->x()) / (objnum + 1);
        yCenter = (objnum * yCenter + objIt->y()) / (objnum + 1);
        objnum++;
    }

    return QPoint((int)xCenter, (int)yCenter);
}
