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
#include "gui3drasterdata.h"
#include "ui_gui3drasterdata.h"

GUI3DRasterData::GUI3DRasterData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GUI3DRasterData)
{
    std::cout << "Create 3D Viewer" << std::endl;
    ui->setupUi(this);
    connect(this, SIGNAL(newRasterData(QString, QString)), this->ui->viewer3D, SLOT(setRasterData(QString,QString)));
    connect(this, SIGNAL(newRasterData(QVector<RasterData>)), this->ui->viewer3D, SLOT(addRasterData(QVector<RasterData>)));
    connect(this, SIGNAL(newVectorData(QVector<VectorData>)), this->ui->viewer3D, SLOT(addVectorData(QVector<VectorData>)));
    connect(ui->multiplier, SIGNAL(valueChanged(int)), this->ui->viewer3D, SLOT(setMultiplier(int)));
    connect(ui->doubleSpinBox_speed, SIGNAL(valueChanged(double)),  this->ui->viewer3D, SLOT(setSpeed(double)));
    connect(ui->comboBox_Arrangment, SIGNAL(currentIndexChanged(int)),this->ui->viewer3D, SLOT(setArrangment(int)));
}

GUI3DRasterData::~GUI3DRasterData()
{
    delete ui;
}

void GUI3DRasterData::addRasterData(QString UUID, QString Name) {
    std::cout << "Add RasterData" << std::endl;
    emit newRasterData(UUID, Name);
}

void GUI3DRasterData::addRasterData(QVector<RasterData> r) {
    std::cout << "Greatings from Update RasterData" << std::endl;
    emit newRasterData(r);
    //std::cout << r.getValue(0,0) << std::endl;
}

void GUI3DRasterData::addVectorData(QVector<VectorData> r) {
    std::cout << "Greatings from Update Vectordata" << std::endl;
    emit newVectorData(r);
    //std::cout << r.getValue(0,0) << std::endl;
}
