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
#include <modelnode.h>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSimpleTextItem>
#include <linknode.h>
#include <guilink.h>
#include <guimodelnode.h>
#include <QMenu>

#include "projectviewer.h"
#include <QGraphicsDropShadowEffect>
#include <QPen>
#include <QLinearGradient>
#include "ColorPalette.h"
#include "modelnodebutton.h"

#include <module.h>
#include <guiport.h>
#include <boost/foreach.hpp>
#include <simulation.h>
#include <QApplication>
#include <QInputDialog>
#include <groupnode.h>

using namespace boost;
std::string ModelNode::getParameterAsString(std::string name) {

    std::ostringstream val;
    int id = this->VIBeModule->getParameterList()[name];
    if (id == vibens::VIBe2::DOUBLE || id == vibens::VIBe2::LONG || id == vibens::VIBe2::STRING) {
        if (id == vibens::VIBe2::DOUBLE)
            val << this->VIBeModule->getParameter<double>(name);
        if (id == vibens::VIBe2::LONG)
            val << this->VIBeModule->getParameter<long>(name);
        if (id == vibens::VIBe2::STRING)
            val << this->VIBeModule->getParameter<std::string>(name);
        return val.str();
    }
}

void ModelNode::updatePorts () {

    //Add Ports
    //If Port exists is checked by addPort
    BOOST_FOREACH (vibens::Port * p, this->VIBeModule->getInPorts()){
        this->addPort(p);
    }
    BOOST_FOREACH (vibens::Port * p, this->VIBeModule->getOutPorts()) {
        this->addPort(p);
    }

    for (int i = this->ports.size()-1; i > -1; i--) {
          GUIPort * gp = this->ports[i];
          if (gp->getVIBePort()->isPortTuple())
              continue;
          if (gp->getPortType()  > vibens::VIBe2::OUTPORTS ) {
              bool  portExists = false;

              BOOST_FOREACH (vibens::Port * p, this->VIBeModule->getInPorts()){
                  std::string portname1 = p->getLinkedDataName();
                  std::string portname2 = gp->getPortName().toStdString();
                  if (portname1.compare(portname2) == 0) {
                      portExists = true;
                  }
              }
              if (!portExists) {
                  ExistingInPorts.removeAt(ExistingInPorts.indexOf(gp->getPortName()));
                  this->ports.remove(i);
                  delete gp;
              }
          }
          if (gp->getPortType()  < vibens::VIBe2::OUTPORTS ) {
              bool  portExists = false;

              BOOST_FOREACH (vibens::Port * p, this->VIBeModule->getOutPorts()){
                  if (p->getLinkedDataName().compare(gp->getPortName().toStdString()) == 0) {
                      portExists = true;
                  }
              }
              if (!portExists) {
                  ExistingOutPorts.removeAt(ExistingOutPorts.indexOf(gp->getPortName()));
                  this->ports.remove(i);
                  delete gp;
              }
          }
      }



}
void ModelNode::resetModel() {
    this->VIBeModule = this->simulation->resetModule(this->VIBeModule->getUuid());
    foreach(GUIPort * p, this->ports) {
        vibens::Port * po = 0;
        if ((this->VIBeModule->getInPort( p->getPortName().toStdString()) == 0)) {
            po = this->VIBeModule->getOutPort( p->getPortName().toStdString());
        } else {
            po = this->VIBeModule->getInPort( p->getPortName().toStdString());
        }
        p->updatePort( po );
    }

}

void ModelNode::addPort(vibens::Port * p) {
    foreach (QString pname, ExistingInPorts) {
        if (pname.compare(QString::fromStdString(p->getLinkedDataName())) == 0) {
            return;
        }
    }

    ExistingInPorts << QString::fromStdString(p->getLinkedDataName());
    GUIPort * gui_p = new  GUIPort(this, p);
    ports.append(gui_p);
    if  (p->getPortType() < vibens::VIBe2::OUTPORTS) {
        gui_p->setPos(this->boundingRect().width()-gui_p->boundingRect().width(),gui_p->boundingRect().height()*this->outputCounter++);
    }else {
        gui_p->setPos(0,gui_p->boundingRect().height()*this->inputCounter++);
    }
}


/*void ModelNode::removePort(int Type, QString s) {
    int counter = 0;
    int itemID;

    this->update();
}*/


//ModelNode

ModelNode::ModelNode( vibens::Module * VIBeModule, vibens::Simulation * simulation, QVector<ModelNode * > * modelnodes, MainWindow * widget)
{
    this->ResultWidget = widget;
    this->guiPortObserver.setModelNode(this);
    this->guiResultObserver.setResultWidget(this->ResultWidget);
    VIBeModule->addResultObserver(& this->guiResultObserver);
    this->minimized = false;
    this->visible = true;
    this->setParentItem(0);
    this->outputCounter = 0;
    this->inputCounter = 0;
    this->parentGroup = 0;
    this->VIBeModule = VIBeModule;
    this->id = VIBeModule->getID();
    this->simulation = simulation;
    this->nodes = modelnodes;
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    this->simpleTextItem = new QGraphicsSimpleTextItem ("Module:" + QString::fromStdString(VIBeModule->getName()));
    double w = this->simpleTextItem->boundingRect().width()+40;
    w = w < 140 ? 140 : w;
    l = w+4;
    h =  this->simpleTextItem->boundingRect().height()+65;
    VIBeModule->addPortObserver( & this->guiPortObserver);
    this->updatePorts();

    Color = COLOR_MODULE;
}

ModelNode::ModelNode(QGraphicsItem * parent, QGraphicsScene * scene) :QGraphicsItem(parent, scene) {

}


void ModelNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(this->isSelected() == true) {
        Color = COLOR_MODULESELECTED;

    } else {
        Color = COLOR_MODULE;
    }

    if(this->visible){
        QPen pen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);


        QLinearGradient linearGrad(QPointF(0, h), QPointF(0, 0));
        QColor c1 = Color;
        QColor c2 = Color;
        linearGrad.setColorAt(0, c1);
        linearGrad.setColorAt(.4, c2);
        QBrush brush(linearGrad);

        painter->setBrush(Qt::white);
        painter->setPen(pen);
        QPainterPath path;
        path.addRoundRect(0, 0,l,h, 10);
        painter->fillPath(path, brush);
        painter->strokePath(path, pen);

        painter->drawText(QPoint(22,15), "Name:" + QString::fromStdString(this->VIBeModule->getName()));
        painter->drawText(QPoint(22,35), "Module:" + QString::fromStdString(this->VIBeModule->getClassName()));
        if (this->parentGroup)
            painter->drawText(QPoint(22,55), "Group: "+ QString::fromStdString(this->parentGroup->getVIBeModel()->getName()));


    }
}


QRectF ModelNode::boundingRect() const {
    QRect r (0, 0,
             l, h);
    return r;

}

QVariant ModelNode::itemChange(GraphicsItemChange change, const QVariant &value) {
    return QGraphicsItem::itemChange(change, value);
}

ModelNode::~ModelNode() {
    int index = this->nodes->indexOf(this);
    this->nodes->remove(index);
    if (this->VIBeModule != 0)
        this->simulation->removeModule(this->VIBeModule->getUuid());
    this->VIBeModule = 0;

    if (this->parentGroup != 0) {
        this->parentGroup->removeModelNode(this);
    }
    foreach (GUIPort * p, ports) {
        delete p;
        p = 0;
    }
    ports.clear();
    delete this->simpleTextItem;

}



void ModelNode::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )  {

    if (this->parentGroup != 0) {
        this->parentGroup->recalculateLandH();
        this->parentGroup->update();
    }

    QGraphicsItem::mouseMoveEvent(event);



}

void ModelNode::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event ) {
    if(this->visible){
        if (this->VIBeModule->createInputDialog() == false )
        {
            QWidget * gui  = new GUIModelNode(this->getVIBeModel(), this);
            gui->show();
        }
    }
}
void ModelNode::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    /*if(this->minimized){
        QGraphicsItem::mousePressEvent(event );
    }*/
}


GUIPort * ModelNode::getGUIPort(vibens::Port * p) {

    foreach (GUIPort * gui_p, this->ports){
        if (gui_p->getVIBePort() == p)
            return gui_p;
    }
    std::cout << "NO PORT FOUND" << std::endl;

    return 0;
}

void ModelNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu menu;
    QAction  * a_edit = menu.addAction("edit");
    QAction * a_rename = menu.addAction("rename");
    QAction  * a_delete = menu.addAction("delete");
    QMenu * GroupMenu =     menu.addMenu("Groups");

    GroupMenu->setTitle("Group");
    QVector<QAction *> actions;
    std::vector<vibens::Group*> gs = this->simulation->getGroups();
    BOOST_FOREACH (vibens::Group * g, gs) {
        if (this->VIBeModule->getUuid().compare(g->getUuid())) {
            QAction *a = GroupMenu->addAction(QString::fromStdString(g->getName()));
            a->setObjectName(QString::fromStdString(g->getUuid()));
            actions.push_back(a);
        }

    }
    if (this->VIBeModule->getGroup()->getUuid().compare(this->simulation->getRootGroup()->getUuid()) != 0) {
        QAction *a = GroupMenu->addAction("none");
        a->setObjectName(QString::fromStdString(this->simulation->getRootGroup()->getUuid()));
        actions.push_back(a);
    }

    foreach (QAction *a,  actions) {
        connect( a, SIGNAL( activated() ), this, SLOT( addGroup() ), Qt::DirectConnection );
    }

    connect( a_delete, SIGNAL( activated() ), this, SLOT( deleteModelNode() ), Qt::DirectConnection );
    connect( a_edit, SIGNAL( activated() ), this, SLOT( editModelNode() ), Qt::DirectConnection );
    connect( a_rename, SIGNAL(activated() ), this, SLOT( renameModelNode() ), Qt::DirectConnection);

    menu.exec(event->screenPos());

}
void ModelNode::editModelNode() {
    GUIModelNode * gui  = new GUIModelNode(this->getVIBeModel() ,this);
    gui->show();
}

void ModelNode::renameModelNode() {
    QString text =QInputDialog::getText(0, "Name", tr("User name:"), QLineEdit::Normal);
    if (!text.isEmpty())
        this->getVIBeModel()->setName(text.toStdString());
}

void ModelNode::deleteModelNode() {

    delete this;
}
void ModelNode::removeGroup() {
    QString name = QObject::sender()->objectName();
    this->update(this->boundingRect());
}

void ModelNode::setMinimized(bool b) {
    if (!this->isGroup() ){
        this->minimized = b;

        bool visible = true;
        if  (b) {
            visible = false;

        }
        this->setVisible(visible);
    }
}


void ModelNode::addGroup() {
    //Find GroupNode
    QString name = QObject::sender()->objectName();
    GroupNode * gn;
    if (name.compare(QString::fromStdString(this->simulation->getRootGroup()->getUuid())) == 0) {
        this->VIBeModule->setGroup((vibens::Group * ) this->simulation->getRootGroup());

        return;
    }
    foreach (ModelNode * m, *(this->nodes)) {
        if (m->getVIBeModel()->isGroup()) {
            if (name.compare(QString::fromStdString(m->getVIBeModel()->getUuid())) == 0) {
                gn = (GroupNode * ) m;
            }
        }
    }

    this->parentGroup = gn;
    gn->addModelNode(this);
    this->parentGroup->recalculateLandH();
    this->parentGroup->setGroupZValue();
    this->parentGroup->update();
}
