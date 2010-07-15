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

#include "vzmatrix.h"
#include "vzdelaunay.h"
#include "vzdiagram.h"

#include "vzmainwnd.h"
#include "ui_vzmainwnd.h"

#include <iostream>
#include <fstream>



VzMainWnd::VzMainWnd(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::VzMainWnd)
{
    ui->setupUi(this);

    imgLabel.setBackgroundRole(QPalette::Dark);
    imgLabel.setAlignment(Qt::AlignLeft | Qt::AlignTop);

    imgScroll.setBackgroundRole(QPalette::Dark);
    imgScroll.setWidget(&imgLabel);

    setCentralWidget(&imgScroll);

    connect(ui->openAct,    SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->quitAct,    SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->vrnzAct,    SIGNAL(triggered()), this, SLOT(voronoize()));
    connect(ui->trngAct,    SIGNAL(triggered()), this, SLOT(triangulate()));
    connect(ui->aboutAct,   SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}


VzMainWnd::~VzMainWnd()
{
    delete ui;
}


void VzMainWnd::open()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
    if (!filename.isEmpty()) {
        loadImage(filename);
    }
}


void VzMainWnd::voronoize()
{
    QImage crtImage = image.getImage();
    if (crtImage.isNull())
        return;
    QPixmap pixmap = QPixmap::fromImage(crtImage);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::red, 1));
    //QTime timer;
    //timer.start();
    QVector<QLineF> lines = image.voronoize();
    //qDebug("Voronoize: time elapsed: %d ms", timer.elapsed());
    foreach (QLineF line, lines) {
        painter.drawLine(line);
    }
    painter.end();
    imgLabel.setPixmap(pixmap);
}


void VzMainWnd::triangulate()
{
    QImage crtImage = image.getImage();
    if (crtImage.isNull())
        return;
    QPixmap pixmap = QPixmap::fromImage(crtImage);
    QPainter painter(&pixmap);
    /*
    painter.setPen(QPen(Qt::red, 1));
    foreach(VzTriangle tri, triangles) {
        QPointF center = tri.GetCenter();
        qreal radius = tri.GetRadius();
        painter.drawEllipse(center, radius, radius);
    }
    */
    //QTime timer;
    //timer.start();
    QVector<QLineF> edges = image.triangulate();
    //qDebug("Triangulate: time elapsed: %d ms", timer.elapsed());

    painter.setPen(QPen(Qt::blue, 1));
    foreach (QLineF edge, edges) {
        painter.drawLine(edge);
    }
    painter.end();
    imgLabel.setPixmap(pixmap);
}



void VzMainWnd::about()
{
    QMessageBox::about(this, tr("About Voronoize"),
                       tr("<h2>Voronoize 1.0</h2>"
                          "<p>Authors: Dragos Tarcatu, Liviu Frateanu"
                          "<p>Voronoize is a small computational geometry application for construction of "
                          "Area Voronoi Diagrams and Delaunay triangulations based on raster images."
                          "<p>Voronoize was written in C++ using Nokia Qt v.4.6.3"));
}


void VzMainWnd::loadImage(const QString& filename)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QImage newImage;
    bool loaded = newImage.load(filename);
    newImage = newImage.convertToFormat(QImage::Format_Mono);
    QApplication::restoreOverrideCursor();

    if (loaded) {
        image.loadImage(newImage);
        imgLabel.setPixmap(QPixmap::fromImage(newImage));
        imgLabel.resize(newImage.size());
    }
    else {
        QMessageBox::warning(this, tr("Voronoize"),
                             tr("Error while loading image."));
    }
}
