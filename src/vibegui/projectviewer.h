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
#include "moduledescription.h"

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
    ProjectViewer(  QWidget * parent = 0);
    void setModules(QMap<QString, ModuleDescription> * m) {this->modules = m;}
    void addModule(ModelNode *  m);
    void setModelNodes( QVector<ModelNode * > * m) {this->mnodes = m;}
    void setGroupNodes(QVector<GroupNode * > * g) {this->gnodes = g;}
    void setResultViewer(MainWindow * w){this->ResultViewer = w;}
    void setSimulation(vibens::Simulation * simulation){this->simulation = simulation;}


protected:
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);

private:
    QMap<QString, ModuleDescription> * modules;
    QVector<ModelNode* > * mnodes;
    QVector<GroupNode* > * gnodes;
    //Groups * groups;
    vibens::ModuleRegistry * moduleregistry;
    vibens::Simulation * simulation;
    MainWindow * ResultViewer;

};

#endif // PROJECTVIEWER_H
