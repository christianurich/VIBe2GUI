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
#ifndef DATAMANAGMENT_H
#define DATAMANAGMENT_H

#include "compilersettings.h"
#include "dataobserver.h"
#include "QTreeWidgetItem"
#include "viewer.h"
#include <boost/unordered_map.hpp>
#include <string>
#include <plot.h>
//#include <QMutex>



#define RASTERDATA 2
#define VECTORDATA 1

using namespace boost;
namespace vibens {
class Module;
}

class  VIBE_HELPER_DLL_EXPORT DataManagment : public vibens::DataObserver
{
private:
    QTreeWidgetItem * rootVectorItem;
    QTreeWidgetItem * rootRasterItem;
    Viewer * viewer;
    Plot * plot;
    unordered_map <std::string, VectorData> vec_data;
    unordered_map <std::string, RasterData> r_data;
    std::string currentScene;
    void deleteTreeWidgetItem(QTreeWidgetItem* item);
    //QMutex mutex;

    double x ;


public:
    DataManagment(QTreeWidgetItem * rootItem, Viewer * viewer, Plot * plot);

    void observeNewRasterData(vibens::Module * module, std::string name);
    void observeNewVectorData(vibens::Module * module, std::string name);

    void observerDeletedRasterData(vibens::Module  * module);
    void observerDeletedVectorData(vibens::Module   * module);

    void setCurrentScene(std::string scene, int ID);

    ~DataManagment();


};

#endif // DATAMANAGMENT_H
