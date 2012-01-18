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
#ifndef PROJECTVIEWER_H
#define PROJECTVIEWER_H

#include "compilersettings.h"
#include <QGraphicsView>
#include <QWidget>


#include <QGraphicsSceneDragDropEvent>
#include <linknode.h>

#include <moduleregistry.h>
#include <module.h>
#include <mainwindow.h>
#include <simulation.h>
class ModelNode;
class GroupNode;


class  VIBE_HELPER_DLL_EXPORT ProjectViewer : public QGraphicsScene
{
public:
    int id;
    ProjectViewer(  DM::Group * g, QWidget * parent = 0);
    void addModule(ModelNode *  m);
    void setModelNodes( QVector<ModelNode * > * m) {this->mnodes = m;}
    void setGroupNodes(QVector<GroupNode * > * g) {this->gnodes = g;}
    void setResultViewer(MainWindow * w){this->ResultViewer = w;}
    void setSimulation(DM::Simulation * simulation){this->simulation = simulation;}


protected:
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);

private:
    QVector<ModelNode* > * mnodes;
    QVector<GroupNode* > * gnodes;

    DM::ModuleRegistry * moduleregistry;
    DM::Simulation * simulation;
    MainWindow * ResultViewer;

    DM::Group * group;

};

#endif // PROJECTVIEWER_H
