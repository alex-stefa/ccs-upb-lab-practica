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

#ifndef VZMAINWND_H
#define VZMAINWND_H

#include <QtCore>
#include <QtGui>

#include "vzgraphicsobj.h"
#include "vzmatrix.h"
#include "vzimage.h"


namespace Ui
{
    class VzMainWnd;
}

class VzMainWnd : public QMainWindow
{
    Q_OBJECT

public:
    VzMainWnd(QWidget *parent = 0);
    ~VzMainWnd();

private slots:
    void open();
    void voronoize();
    void triangulate();
    void about();

private:
    void loadImage(const QString& filename);

private:
    Ui::VzMainWnd *ui;

    QScrollArea imgScroll;
    QLabel imgLabel;
    VzImage image;
};

#endif // VZMAINWND_H
