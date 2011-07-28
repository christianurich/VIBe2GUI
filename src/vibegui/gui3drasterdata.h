/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
 *
 * Copyright (C) 2011  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef GUI3DRASTERDATA_H
#define GUI3DRASTERDATA_H

#include <QWidget>
#include <mainwindow.h>
#include <rasterdata.h>

namespace Ui {
class GUI3DRasterData;
}

class GUI3DRasterData : public QWidget
{
    Q_OBJECT

public:
    explicit GUI3DRasterData(QWidget *parent = 0);
    ~GUI3DRasterData();


private:
    Ui::GUI3DRasterData *ui;
    QString UUID;
    QString DataName;



public slots:
    void addRasterData(QString, QString);
    void addRasterData(QVector<RasterData>);
    void addVectorData(QVector<VectorData>);

signals:
    void newRasterData(QString, QString);
    void newRasterData(QVector<RasterData>);
    void newVectorData(QVector<VectorData>);


};

#endif // GUI3DRASTERDATA_H
