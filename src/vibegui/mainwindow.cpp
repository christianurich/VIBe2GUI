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
 * The class contains the main windows for the DAnCE4Water GUI
 */

#include "mainwindow.h"
#include <boost/python.hpp>

#include "QDir"
#include "QThread"
#include "QTreeWidgetItem"
#include "QFileDialog"
#include "QSettings"
#include "modulereader.h"

#include "simulationio.h"
#include "guilink.h"
#include "StdRedirector.h"
#include "cstdio"
#include <QTreeWidget>
#include "simulationmanagment.h"
//#include "pythonenv.h"
#include <projectviewer.h>
#include <modelnode.h>
#include "datamanagment.h"
#include <module.h>
#include <boost/foreach.hpp>
#include <simulation.h>
#include <dmdatabase.h>
#include <datamanagement.h>
#include <groupnode.h>
#include <QWidget>
#include <QThreadPool>
#include <guiimageresultview.h>
#include <vibe_log.h>
#include <guilogsink.h>
#include <guidataobserver.h>
#include <plot.h>
#include <pythonenv.h>
using namespace boost;
void outcallback( const char* ptr, std::streamsize count, void* pTextBox )
{
    (void) count;
    QPlainTextEdit * p = static_cast< QPlainTextEdit* >( pTextBox );

    if (*(ptr) > 37 ) {
        int c = *(ptr);
        QString s = QString::fromAscii(ptr);
        p->appendPlainText( ptr );
    }
}
void MainWindow::updateResultImage(QString filename) {
    std::cout << "From Mainwindow" << filename.toStdString() << std::endl;
}
void MainWindow::registerResultWindow(GUIResultObserver * ress) {
    GUIImageResultView * w = new GUIImageResultView();

    this->tabWidget_2->addTab(w, QString::fromStdString(this->simulation->getModuleWithUUID(ress->getUUID())->getName()));
    QString f = QString::fromStdString(ress->getfirstImage());
    w->updateImage(f);

    //Update Windows
    connect(ress, SIGNAL(updateImage(QString)), w, SLOT(updateImage(QString)), Qt::BlockingQueuedConnection);
}
void MainWindow::updatePlotData(double d) {
    std::cout << "Update Plot Data" << std::endl;
}

void MainWindow::registerPlotWindow(GUIResultObserver * ress, double x, double y) {
    /* GUIImageResultView * w = new GUIImageResultView();


    QString f = QString::fromStdString(ress->getfirstImage());
    w->updateImage(f);

    //Update Windows*/
    Plot * p = new Plot();
    // QDockWidget *dw = new QDockWidget(this);
    //dw->setAllowedAreas(Qt::BottomDockWidgetArea);

    //dw->setWidget(p);
    //this->tabWidget->addTab(dw, QString::fromStdString(this->simulation->getModuleWithUUID(ress->getUUID())->getName()));
    //this->addDockWidget(Qt::BottomDockWidgetArea, dw);
    // dw->show();

    this->tabWidget->addTab(p,QString::fromStdString(this->simulation->getModuleWithUUID(ress->getUUID())->getName()) );

    vibens::Logger(vibens::Debug) << "Register Plot";
    connect(ress, SIGNAL(newDoubleDataForPlot(double, double)), p, SLOT(appendData(double, double)), Qt::BlockingQueuedConnection);
    p->appendData( x,y);
}



void MainWindow::ReloadSimulation() {
    this->simulation->reloadModules();
    this->
    createModuleListView();
}

void MainWindow::startEditor() {
    //vibens::PythonEnv::getInstance()->startEditra();
}

MainWindow::MainWindow(QWidget * parent)
{
    log_updater = new GuiLogSink();
    //#ifdef _DEBUG
    vibens::Log::init(log_updater,vibens::Debug);
    //#else
    //    vibens::Log::init(log_updater,vibens::Standard);
    //#endif
    running =  false;
    setupUi(this);
    vibens::DataManagement::init();
    this->database = new  DMDatabase();
    vibens::DataManagement::getInstance().registerDataBase(this->database);
    vibens::PythonEnv *env = vibens::PythonEnv::getInstance();
    env->addPythonPath("/home/christian/programms/VIBe2Core/build/debug");

    //this->graphicsView->setViewport(new QGLWidget());

    this->simulation = new vibens::Simulation();
    this->simulation->registerDataBase(this->database);

    this->scene = new ProjectViewer();
    this->scene->setSimulation(this->simulation);
    this->simulation->registerNativeModules("/home/c8451045/Documents/VIBe2Core/build/debug/libdmtestmodule.so");
    this->simulation->registerPythonModules("/home/c8451045/Documents/VIBe2Core/scripts");
    this->scene->setResultViewer(this);

    this->mnodes = new QVector<ModelNode*>();
    this->gnodes = new QVector<GroupNode*>();
    //this->groups = Groups(this);
    log_widget->connect(log_updater, SIGNAL(newLogLine(QString)), SLOT(appendPlainText(QString)), Qt::QueuedConnection);
    connect( actionRun, SIGNAL( activated() ), this, SLOT( runSimulation() ), Qt::DirectConnection );
    connect( actionPreferences, SIGNAL ( activated() ), this, SLOT(preferences() ), Qt::DirectConnection );
    //connect(buildsim, SIGNAL(clicked()), this , SLOT(buildSimulation()), Qt::DirectConnection);
    connect(runSim, SIGNAL(clicked()), this , SLOT(runSimulation()), Qt::DirectConnection);
    connect(actionSave, SIGNAL(activated()), this , SLOT(saveSimulation()), Qt::DirectConnection);
    connect(actionSaveAs, SIGNAL(activated()), this , SLOT(saveAsSimulation()), Qt::DirectConnection);
    connect(actionOpen, SIGNAL(activated()), this , SLOT(loadSimulation()), Qt::DirectConnection);
    connect(actionNew, SIGNAL(activated()), this , SLOT(clearSimulation()), Qt::DirectConnection);
    connect(actionImport, SIGNAL(activated()), this , SLOT(importSimulation()), Qt::DirectConnection);
    connect(actionEditor, SIGNAL(activated()), this , SLOT(startEditor()), Qt::DirectConnection);
    connect(actionReload_Modules, SIGNAL(activated()), this , SLOT(ReloadSimulation()), Qt::DirectConnection);
    connect(edit_groups, SIGNAL(clicked()), this , SLOT(editGroup()), Qt::DirectConnection);

    //runSim->setDisabled(true);
    //treeWidget->setColumnCount(1);
    currentDocument = "";



    QSettings settings("IUT", "VIBe2");
    //settings.beginGroup("Preferences");
    if(settings.value("pythonModules").toString().isEmpty()) {
        counter++;
        this->preferences();
    }
    this->simmanagment = new SimulationManagment();

    connect(this->simmanagment, SIGNAL(valueChanged(int)), this->progressBar, SLOT(setValue(int)));
    //connect(this->simmanagment, SIGNAL(valueChanged(int)), this, SLOT(test(int)));
    this->progressBar->setRange(0,1);
    this->progressBar->setValue(0);

    createModuleListView();

    this->rootItemModelTree = new QTreeWidgetItem();
    this->rootItemModelTree->setText(0, "Groups");
    this->rootItemModelTree->setText(1, "");
    this->rootItemModelTree->setExpanded(true);
    this->modelTree->setColumnCount(2);
    this->modelTree->addTopLevelItem(this->rootItemModelTree);
    this->modelTree->headerItem()->setText(0, "Module");
    this->modelTree->headerItem()->setText(1, "");
    this->scene->setModules(& this->modules);
    this->scene->setModelNodes(this->mnodes);
    this->scene->setGroupNodes(this->gnodes);
    //this->scene->setModuleRegistry(& this->registry);
    this->graphicsView->setScene(scene);
    this->graphicsView->setAcceptDrops(true);

    // this->points = new QwtPlotMarker();
    // this->points->attach(this->PlotWindow);

    //QThreadPool::setMaxThreadCount(1);

    //StdRedirector<> *  myRedirector = new StdRedirector<>( std::cout, outcallback, this->plainTextEdit );




    //Init ModuleRegistry


    // create3DViewer();



}

void MainWindow::createModuleListView() {
    this->treeWidget->clear();
    std::list<std::string> mlist = (this->simulation->getModuleRegistry()->getRegisteredModules());
    std::map<std::string, std::vector<std::string> > mMap (this->simulation->getModuleRegistry()->getModuleMap());
    this->treeWidget->setColumnCount(1);
    for (std::map<std::string, std::vector<std::string> >::iterator it = mMap.begin(); it != mMap.end(); ++it) {
        QTreeWidgetItem * items = new QTreeWidgetItem(this->treeWidget);
        std::string name = it->first;
        items->setText(0, QString::fromStdString(name));
        std::vector<std::string> names = it->second;
        std::sort(names.begin(), names.end());
        BOOST_FOREACH(std::string name, names) {
            QTreeWidgetItem * item;
            item = new QTreeWidgetItem(items);
            item->setText(0,QString::fromStdString(name));
            item->setText(1,"Module");
        }
    }



    //Add VIBe Modules
    QStringList filters;
    filters << "*.vib";
   QSettings settings("IUT", "VIBe2");
    QStringList moduleshome = settings.value("VIBeModules",QStringList()).toString().replace("\\","/").split(",");
    for (int index = 0; index < moduleshome.size(); index++) {
        QDir d = QDir(moduleshome[index]);
        QStringList list = d.entryList(filters);
        QString module_name = d.absolutePath().split("/").last();
        mMap.clear();
        foreach(QString name, list) {
            std::vector<std::string> vec = mMap[module_name.toStdString()];
            vec.push_back(name.toStdString());
            mMap[module_name.toStdString()]  = vec;
        }

        for (std::map<std::string, std::vector<std::string> >::iterator it = mMap.begin(); it != mMap.end(); ++it) {
            QTreeWidgetItem * items = new QTreeWidgetItem(this->treeWidget);
            std::string name = it->first;
            items->setText(0, QString::fromStdString(name));
            std::vector<std::string> names = it->second;
            std::sort(names.begin(), names.end());

            BOOST_FOREACH(std::string name, names) {
                QTreeWidgetItem * item;
                item = new QTreeWidgetItem(items);
                item->setText(0,QString::fromStdString(name));
                item->setText(1,"VIBe");
                stringstream filename;
                filename << moduleshome[0].toStdString() << "/" << name;
                item->setText(2,QString::fromStdString(filename.str()));
            }
        }
    }


}
void MainWindow::editGroup() {
    std::cout << "Edit Group" << std::endl;
    QTreeWidgetItem * item = this->modelTree->currentItem () ;
    if (item == 0) {
        return;
    }


}
void MainWindow::updateRasterData(QString UUID,  QString Name) {
    //this->widget->setRasterData(UUID, Name);

}

void MainWindow::runSimulation() {
    this->database->resetDataBase();
    for (int  i = 0; i < this->mnodes->size(); i++) {
        ModelNode * m = this->mnodes->at(i);
        m->resetModel();
    }
    connect(simulation, SIGNAL(finished()), this , SLOT(SimulationFinished()), Qt::DirectConnection);
    simulation->run();

    return;

}
void MainWindow::SimulationFinished() {
    std::cout << "Finished1111" << std::endl;
}

void MainWindow::preferences() {
    Preferences *p =  new Preferences(this);
    p->exec();
}

void MainWindow::setRunning() {
    this->running = false;
}

void MainWindow::sceneChanged() {
    std::cout << "Scene Changed" << std::endl;

}
void MainWindow::saveAsSimulation() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save VIBe File"), "", tr("VIBe Files (*.vib)"));
    if (!fileName.isEmpty()) {
        if (!fileName.contains(".vib"))
            fileName+=".vib";
        this->simulation->writeSimulation(fileName.toStdString());
        this->writeGUIInformation(fileName);
        this->currentDocument = fileName;
    }

}
void MainWindow::saveSimulation() {
    if (!this->currentDocument.isEmpty()) {
        this->simulation->writeSimulation(this->currentDocument.toStdString());
        this->writeGUIInformation(currentDocument);
    } else {
        this->saveAsSimulation();
    }

}
void MainWindow::writeGUIInformation(QString FileName) {

    //Find upper left corner;
    float minx;
    float miny;
    for (int i = 0; i < this->mnodes->size(); i++) {
        ModelNode * m = this->mnodes->at(i);
        if (i == 0)        {
            minx = m->pos().x();
            miny = m->pos().y();
            continue;
        }
        if (minx > m->pos().x()) {
            minx =  m->pos().x();
        }
        if (miny > m->pos().y()) {
            miny =  m->pos().y();
        }
    }

    std::cout << "Save File" << std::endl;

    QFile file(FileName);
    file.open(QIODevice::Append);
    QTextStream out(&file);
    out << "<VIBe2GUI>" << "\n";
    out << "\t"<<"<GUI_Nodes>" << "\n";

    BOOST_FOREACH(ModelNode * m, *(this->mnodes)) {
        out  << "\t" << "\t"<<"<GUI_Node>" << "\n";
        out << "\t" << "\t"<< "\t" << "<GUI_UUID value=\""
            << QString::fromStdString(m->getVIBeModel()->getUuid()) << "\"/>" << "\n";

        out << "\t" << "\t"<< "\t" << "<GUI_PosX value=\""
            << m->scenePos().x() - minx << "\"/>" << "\n";



        out << "\t" << "\t"<< "\t" << "<GUI_PosY value=\""
            << m->scenePos().y() - miny << "\"/>" << "\n";

        out << "\t" << "\t"<< "\t" << "<GUI_Minimized value=\""
            << m->isMinimized() << "\"/>" << "\n";

        out  << "\t" << "\t"<<"</GUI_Node>" << "\n";

    }
    out << "\t"<<"</GUI_Nodes>" << "\n";

    out << "</VIBe2GUI>" << "\n";
    out << "</VIBe2>"<< "\n";

    file.close();
}

void MainWindow::clearSimulation() {
    while (this->gnodes->size() > 0) {
        delete this->gnodes->at(0);
        this->gnodes->remove(0);
    }
    while (this->mnodes->size() > 0)
        delete this->mnodes->at(0);
    this->currentDocument = "";

    this->database->resetDataBase();


}
void MainWindow::importSimulation(QString fileName, QPointF offset) {
    if (fileName.compare("") == 0)
        fileName = QFileDialog::getOpenFileName(this,
                                                tr("Open VIBe File"), "", tr("VIBe Files (*.vib)"));

    if (!fileName.isEmpty()){


        std::map<std::string, std::string> UUID_Translation = this->simulation->loadSimulation(fileName.toStdString());
        SimulationIO simio;
        simio.loadSimluation(fileName, this->simulation, UUID_Translation, this->mnodes);

        std::vector<ModelNode*> new_modules;

        for (std::map<std::string, std::string>::iterator it = UUID_Translation.begin(); it != UUID_Translation.end(); ++it ) {
            std::string uuid = it->second;
            vibens::Module * m = this->simulation->getModuleWithUUID(uuid);

            foreach(ModelNode * mn, *(mnodes)) {
                if (mn->getVIBeModel() == m ) {
                    new_modules.push_back(mn);
                    break;
                }
            }

        }

        vibens::Logger(vibens::Debug) << offset.x();
        for (int i = 0; i < new_modules.size(); i++) {

            ModelNode * module = new_modules.at(i);
            module->setResultWidget(this);


            std::string GroupUUID = module->getVIBeModel()->getGroup()->getUuid();
            this->scene->addItem(module);
            foreach (ModelNode * m, new_modules) {
                if (m->getVIBeModel()->isGroup()) {
                    if (m->getVIBeModel()->getUuid().compare(GroupUUID) == 0) {
                        GroupNode * gn = (GroupNode * ) m;
                        gn->addModelNode(module);
                        module->setParentGroup(gn);
                    }
                }

            }
            module->setPos(module->pos()+offset);
            vibens::Logger(vibens::Debug) << module->pos().x();
            module->updatePorts();


        }


        BOOST_FOREACH(vibens::ModuleLink * l,this->simulation->getLinks()) {
            //Check if Linke is allready places

            std::cout << "link" << std::endl;
            GUILink * gui_link  = new GUILink();
            ModelNode * outmodule = 0;
            ModelNode * inmodule = 0;

            foreach(ModelNode * mn, new_modules) {
                if (mn->getVIBeModel() == l->getOutPort()->getModule()) {
                    outmodule = mn;
                    break;
                }
            }
            foreach(ModelNode * mn, new_modules) {
                if (mn->getVIBeModel() == l->getInPort()->getModule()) {
                    inmodule = mn;
                    break;
                }
            }
            if (outmodule != 0 && inmodule != 0) {
                gui_link->setOutPort(outmodule->getGUIPort(l->getOutPort()));
                gui_link->setInPort(inmodule->getGUIPort(l->getInPort()));
                gui_link->setVIBeLink(l);
                gui_link->setSimulation(this->simulation);
                this->scene->addItem(gui_link);
            }



        }
    }
    for (int i = 0; i < this->mnodes->size(); i++) {
        ModelNode * m = this->mnodes->at(i);
        if (m->isMinimized())
            m->setMinimized(m->isMinimized());
    }
    for (int i = 0; i < this->gnodes->size(); i++) {
        GroupNode * m = this->gnodes->at(i);
        if (m->isMinimized())
            m->setMinimized(m->isMinimized());
    }


    this->scene->update();


}

void MainWindow::loadSimulation(int id) {

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open VIBe File"), "", tr("VIBe Files (*.vib)"));

    if (!fileName.isEmpty()){
        this->clearSimulation();
        this->currentDocument = fileName;
        std::map<std::string, std::string> UUID_Translation = this->simulation->loadSimulation(fileName.toStdString());
        SimulationIO simio;
        simio.loadSimluation(fileName, this->simulation, UUID_Translation, this->mnodes);
        for (int i = 0; i < this->mnodes->size(); i++) {

            ModelNode * module = this->mnodes->at(i);
            module->setResultWidget(this);

            std::string GroupUUID = module->getVIBeModel()->getGroup()->getUuid();
            this->scene->addItem(module);
            foreach (ModelNode * m, *(this->mnodes)) {
                if (m->getVIBeModel()->isGroup()) {
                    if (m->getVIBeModel()->getUuid().compare(GroupUUID) == 0) {
                        GroupNode * gn = (GroupNode * ) m;
                        gn->addModelNode(module);
                        module->setParentGroup(gn);

                    }
                }
            }
            module->updatePorts();


        }


        BOOST_FOREACH(vibens::ModuleLink * l,this->simulation->getLinks()) {
            std::cout << "link" << std::endl;
            GUILink * gui_link  = new GUILink();
            ModelNode * outmodule;
            ModelNode * inmodule;
            foreach(ModelNode * mn, *(mnodes)) {
                if (mn->getVIBeModel() == l->getOutPort()->getModule()) {
                    outmodule = mn;
                    break;
                }
            }
            foreach(ModelNode * mn, *(mnodes)) {
                if (mn->getVIBeModel() == l->getInPort()->getModule()) {
                    inmodule = mn;
                    break;
                }
            }
            gui_link->setOutPort(outmodule->getGUIPort(l->getOutPort()));
            gui_link->setInPort(inmodule->getGUIPort(l->getInPort()));
            gui_link->setVIBeLink(l);
            gui_link->setSimulation(this->simulation);
            this->scene->addItem(gui_link);

        }
    }
    for (int i = 0; i < this->mnodes->size(); i++) {
        ModelNode * m = this->mnodes->at(i);
        if (m->isMinimized())
            m->setMinimized(m->isMinimized());
    }
    for (int i = 0; i < this->gnodes->size(); i++) {
        GroupNode * m = this->gnodes->at(i);
        if (m->isMinimized())
            m->setMinimized(m->isMinimized());
    }


    this->scene->update();

}
MainWindow::~MainWindow() {
    for ( int i = 0; i < mnodes->size(); i++ ) {
        ModelNode * m = mnodes->at(i);
        delete m;
    }
    delete this->scene;
    delete this->mnodes;
    delete this->simulation;
}

void MainWindow::on_actionZoomIn_activated()
{
    QWidget * view = (QWidget *)   this->tabWidget_2->currentWidget();
    if (view->objectName().compare("Model_View") == 0){
        graphicsView->scale(1.2, 1.2);
        return;
    }
    if (view->objectName().compare("Result_View") == 0){
        return;
    }
    if (view->objectName().compare("Result_View") != 0){
        GUIImageResultView * v = ( GUIImageResultView *) view;
        v->getView()->scale(1.2, 1.2);
        return;
    }
}

void MainWindow::on_actionZoomOut_activated()
{
    QWidget * view = (QWidget *)   this->tabWidget_2->currentWidget();
    if (view->objectName().compare("Model_View") == 0){
        graphicsView->scale(0.8, 0.8);
        return;
    }
    if (view->objectName().compare("Result_View") == 0){
        //graphicsView->scale(1.2, 1.2);
        return;
    }
    if (view->objectName().compare("Result_View") != 0){
        GUIImageResultView * v = ( GUIImageResultView *) view;
        v->getView()->scale(0.8, 0.8);
        return;
    }
}

void MainWindow::on_actionZoomReset_activated()
{
    QWidget * view = (QWidget *)   this->tabWidget_2->currentWidget();
    if (view->objectName().compare("Model_View") == 0){
        graphicsView->fitInView(graphicsView->sceneRect(), Qt::KeepAspectRatio);
        return;
    }
    if (view->objectName().compare("Result_View") == 0){
        return;
    }
    if (view->objectName().compare("Result_View") != 0){
        GUIImageResultView * v = ( GUIImageResultView *) view;
        v->getView()->fitInView(graphicsView->sceneRect(), Qt::KeepAspectRatio);
        return;
    }

}
