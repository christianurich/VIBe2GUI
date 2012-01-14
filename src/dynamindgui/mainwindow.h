/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * DAnCE4Water (Dynamic Adaptation for eNabling City Evolution for Water) is a
 * strategic planning tool ti identify the transtion from an from a conventional
 * urban water system to an adaptable and sustainable system at a city scale
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
 * @section DESCRIPTION
 *
 * The class contains the main window for the VIBe2 Framework
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "compilersettings.h"
#include "moduleregistry.h"
#include <QMainWindow>
#include "preferences.h"
#include "plot.h"
#include <ui_mainwindow.h>

#include <QMap>
#include <QVector>

#include <moduledescription.h>
#include <guiresultobserver.h>
class QTreeWidgetItem;

class ProjectViewer;
class ModelNode;
class SimulationManagment;
class DataManagment;
class GroupNode;
class GuiLogSink;
class DMDatabase;

namespace DM {

    class Simulation;
}



class VIBE_HELPER_DLL_EXPORT MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    ProjectViewer * getProjectViewer() {return this->scene;}
    QTreeWidgetItem * getRootItemModelTree(){return this->rootItemModelTree;}
    GuiLogSink *log_updater;
private:
    DM::Simulation * simulation;
    DMDatabase * database;
    DataManagment * data;
    SimulationManagment * simmanagment;

    void createModuleListView();
    QMap<QString, ModuleDescription> modules;
    QVector<ModelNode * > * mnodes;
    QVector<GroupNode * > * gnodes;
    ProjectViewer * scene;
    QString currentDocument;
    int counter;
    bool running;
    QTreeWidgetItem * rootItemModelTree;
    DM::ModuleRegistry registry;
    void writeGUIInformation(QString FileName);
    QwtPlotMarker * points;


public slots:
    void runSimulation();
    void sceneChanged();
    void preferences();
    void saveSimulation();
    void loadSimulation(int id=0);
    void clearSimulation();
    void setRunning();
    void saveAsSimulation();
    void importSimulation(QString fileName = "", QPointF = QPointF(0,0));
    void editGroup();
    void registerResultWindow(GUIResultObserver *);
    void registerPlotWindow(GUIResultObserver *, double, double);
    void updateRasterData(QString,  QString);
    void updateResultImage(QString);
    void updatePlotData(double d);
    void SimulationFinished();
    void startEditor();
    void ReloadSimulation();


private slots:
    void on_actionZoomReset_activated();
    void on_actionZoomOut_activated();
    void on_actionZoomIn_activated();
signals:
    void updateSplashMessage(QString);

};

#endif // MAINWINDOW_H
