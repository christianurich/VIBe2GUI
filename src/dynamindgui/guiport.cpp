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
#include "guiport.h"
#include <modelnode.h>
#include <port.h>
#include "ColorPalette.h"
#include <guilink.h>
#include <QGraphicsSceneMouseEvent>
#include <simulation.h>
#include <modulelink.h>

GUIPort::~GUIPort () {
    foreach(GUILink *l, this->links) {
        delete l;
        l = 0;
    }
    this->links.clear();
}
void GUIPort::removeLink(GUILink * l) {
    int index = this->links.indexOf(l);
    if (index > -1) {
        this->links.remove(index);
        DM::Logger(DM::Debug) << "Remove GUILink from" << this->getPortName() << this->links.size();
    }
}
void GUIPort::updatePort(DM::Port * p) {
    this->p = p;

}
GUIPort::GUIPort(ModelNode *modelNode, DM::Port *p) : QGraphicsItem(modelNode)
{
    this->setParentItem(modelNode);
    this->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    this->setAcceptHoverEvents(true);
    this->setAcceptsHoverEvents(true);
    this->PortName = QString::fromStdString(p->getLinkedDataName());
    tmp_link = 0;
    //this->hoverElement = 0;
    this->p = p;
    this->x1 = 0;
    this->isHover = false;
    this->LinkMode = false;
    this->modelNode = modelNode;
    this->PortType = p->getPortType();
    this->simpleTextItem = new QGraphicsSimpleTextItem (QString::fromStdString(p->getLinkedDataName()));

    if (p->getPortType() == DM::VIBe2::INSYSTEM || p->getPortType() == DM::VIBe2::OUTSYSTEM)
        color = COLOR_VECTORPORT;
    if (p->getPortType() == DM::VIBe2::INDOUBLEDATA || p->getPortType() == DM::VIBe2::OUTDOUBLEDATA)
        color = COLOR_DOUBLEPORT
                hoverElement = 0;


}
bool GUIPort::isLinked() {
    if (this->p->getLinks().size() > 0)
        return true;
    return false;
}

void GUIPort::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (this->p->isFullyLinked())
        color = Qt::green;
    if (!this->p->isFullyLinked())
        color = Qt::red;
    painter->setBrush(color);

    if(isHover){

        l = this->simpleTextItem->boundingRect().width()+4;
        h = this->simpleTextItem->boundingRect().height()+4;
        x1 = 0;
        if (this->p->getPortType() > DM::VIBe2::OUTPORTS)
            x1 = -l+14;
        QPainterPath path;
        QPen pen(Qt::black, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        path.addRoundRect(x1, 0,l,h, 10);
        painter->fillPath(path, color);
        painter->strokePath(path, pen);
        painter->setPen(pen);
        painter->drawText(x1+2,15, this->simpleTextItem->text());


    } else {
        QPainterPath path;
        QPen pen(Qt::black, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        path.addRoundRect(0, 0,14,14, 10);
        painter->fillPath(path, color);
        painter->strokePath(path, pen);


        //painter->drawRect(0, 0,  14, 14);
    }

    painter->setBrush(Qt::NoBrush);
}

QRectF GUIPort::boundingRect() const {
    if(isHover){
        QRect r (x1, 0,
                 l, h);
        return r;
    } else {
        QRect r (0, 0,
                 14, 25);
        return r;
    }
}

void GUIPort::hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) {
    this->isHover = true;

    if (this->p->getPortType() == DM::VIBe2::INSYSTEM|| this->p->getPortType() == DM::VIBe2::OUTSYSTEM)
        color = COLOR_VECTORPORT;
    if (p->getPortType() == DM::VIBe2::INDOUBLEDATA || p->getPortType() == DM::VIBe2::OUTDOUBLEDATA)
        color = COLOR_DOUBLEPORT
                prepareGeometryChange ();
    l = this->simpleTextItem->boundingRect().width()+4;
    h = this->simpleTextItem->boundingRect().height()+4;
    x1 = 0;
    if (p->getPortType()  > DM::VIBe2::OUTPORTS  )
        x1 = -l+14;
    this->update(this->boundingRect());
}

void GUIPort::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) {
    this->isHover = false;
    if (!LinkMode) {

        if (p->getPortType()  == DM::VIBe2::INSYSTEM || p->getPortType()  == DM::VIBe2::OUTSYSTEM)
            color = COLOR_VECTORPORT;
        if (p->getPortType() == DM::VIBe2::INDOUBLEDATA || p->getPortType() == DM::VIBe2::OUTDOUBLEDATA)
            color = COLOR_DOUBLEPORT
    }
    prepareGeometryChange ();
    this->update(this->boundingRect());
}

QPointF GUIPort::getConnectionNode() {

    return  QPointF(this->scenePos());
}

int GUIPort::getPortType() {
    return this->PortType;
}

void GUIPort::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )  {
    this->scene()->sendEvent(0, event);
    if (LinkMode) {
        this->tmp_link->setInPort(event->scenePos());
    }
    QList<QGraphicsItem  *> items = this->scene()->items(event->scenePos());
    //Check Hover Event
    bool setHover = false;
    foreach (QGraphicsItem  * item, items) {
        if ( this->type() == item->type() ) {
            GUIPort * link  = (GUIPort *) item;

            if (getPortType() == DM::VIBe2::OUTSYSTEM &&  link->getPortType() == DM::VIBe2::INSYSTEM ) {
                link->setHover(true);
                link->prepareGeometryChange();
                link->update();
                this->hoverElement = link;
                setHover = true;
            }
            if (getPortType() == DM::VIBe2::OUTDOUBLEDATA &&  link->getPortType() == DM::VIBe2::INDOUBLEDATA ) {
                link->setHover(true);
                link->prepareGeometryChange();
                link->update();
                this->hoverElement = link;
                setHover = true;
            }
        }
    }
    if(!setHover) {
        if(this->hoverElement != 0) {
            this->hoverElement->setHover(false);
        }
        this->hoverElement = 0;
    }

}


void GUIPort::mousePressEvent ( QGraphicsSceneMouseEvent * event )  {

    if (getPortType() == DM::VIBe2::INSYSTEM || getPortType() == DM::VIBe2::OUTSYSTEM )
        color = COLOR_VECTORPORT;
    if (p->getPortType() == DM::VIBe2::INDOUBLEDATA || p->getPortType() == DM::VIBe2::OUTDOUBLEDATA)
        color = COLOR_DOUBLEPORT


                if (getPortType() < DM::VIBe2::OUTPORTS) {
            LinkMode = true;
            this->tmp_link = new GUILink();
            this->tmp_link->setOutPort(this);
            this->scene()->addItem(this->tmp_link);
        }

}
DM::Port * GUIPort::getVIBePort() {
    return this->p;
}

void GUIPort::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) {
    std::cout << "mouseReleaseEvent " << std::endl;
    if (getPortType() < DM::VIBe2::OUTPORTS) {
        LinkMode = false;

        if (getPortType()  == DM::VIBe2::INSYSTEM)
            color = COLOR_VECTORPORT;

        this->update(this->boundingRect());
        QList<QGraphicsItem  *> items = this->scene()->items(event->scenePos());
        bool newLink = false;
        foreach (QGraphicsItem  * item, items) {
            if ( this->type() == item->type() ) {
                GUIPort * endLink  = (GUIPort *) item;

                if (getPortType() == DM::VIBe2::OUTSYSTEM &&  endLink->getPortType() == DM::VIBe2::INSYSTEM ) {
                    this->tmp_link->setInPort(endLink);
                    //this->links.append(tmp_link);
                    //Create Link
                    tmp_link->setVIBeLink(this->modelNode->getSimulation()->addLink(tmp_link->getOutPort()->getVIBePort(), tmp_link->getInPort()->getVIBePort()));
                    tmp_link->setSimulation(this->modelNode->getSimulation());
                    newLink = true;
                    tmp_link = 0;

                    //Run Simulation

                    this->modelNode->getSimulation()->run(true, false);



                }
                if (getPortType() == DM::VIBe2::OUTDOUBLEDATA &&  endLink->getPortType() == DM::VIBe2::INDOUBLEDATA ) {
                    this->tmp_link->setInPort(endLink);
                    //this->links.append(tmp_link);
                    //Create Link
                    tmp_link->setVIBeLink(this->modelNode->getSimulation()->addLink(tmp_link->getOutPort()->getVIBePort(), tmp_link->getInPort()->getVIBePort()));
                    tmp_link->setSimulation(this->modelNode->getSimulation());
                    newLink = true;
                    tmp_link = 0;
                }
            }

        }
        if (!newLink ) {
            if (this->tmp_link != 0) {
                delete this->tmp_link;
                this->tmp_link = 0;
            }
        }
    }


}
void GUIPort::refreshLinks() {
    foreach(GUILink * l, this->links) {
        if ( l != 0) {
            l->refresh();
        }
    }
}
void GUIPort::setLink(GUILink * l) {

    int index = this->links.indexOf(l);
    if (index == -1) {
        this->links.append(l);
        DM::Logger(DM::Debug) << "Set Link" << this->getPortName();
    }

}
QVariant GUIPort::itemChange(GraphicsItemChange change, const QVariant &value) {
    if(change == QGraphicsItem::ItemScenePositionHasChanged) {
        this->refreshLinks();
    }
    if (change == QGraphicsItem::ItemVisibleHasChanged) {
        foreach(GUILink * l, this->links)
            l->setVisible(this->isVisible());
    }
    return QGraphicsItem::itemChange(change, value);
}




