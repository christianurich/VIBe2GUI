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
#include "viewer.h"
#include "boost/foreach.hpp"
#include "stdio.h"
//#include "triangulate.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <database.h>
#include <datamanagement.h>
#include <QGLShaderProgram>
#include <vibe_logger.h>
#include <spline1d.h>
#include "QCoreApplication"
#include <boost/numeric/ublas/matrix.hpp>
#include <QThread>
#include "glextensions.h"
#include "vectordatahelper.h"
#include "vectordata.h"
#include "griddatahelper.h"
#include "griddata.h"
#include "parceldata.h"

//#ifdef _WIN32
//#include <wglext.h>
//PFNGLACTIVETEXTUREPROC glActiveTexture;    //Declare your function pointer in a .cpp file
//#endif
using namespace boost::numeric::ublas;
using namespace std;

void  tcbBegin (GLenum prim)
{
    glBegin (prim);
}

void tcbVertex (void *data)
{
    glVertex3dv ((GLdouble *)data);
}

void tcbEnd ()
{
    glEnd ();
}

void tcbCombine (GLdouble c[3], void *d[4], GLfloat w[4], void **out)
{
    GLdouble *nv = (GLdouble *) malloc(sizeof(GLdouble)*3);

    nv[0] = c[0];
    nv[1] = c[1];
    nv[2] = c[2];
    *out = nv;
}

void errorCallback(GLenum errorCode)
{
    const GLubyte *estring;

    estring = gluErrorString(errorCode);
    fprintf (stderr, "Tessellation Error: %s\n", estring);
    exit (0);
}
Viewer::~Viewer() {

}
void Viewer::setArrangment(int arr) {
    this->arrangement = arr;
}

void Viewer::redrawRasterData() {
    int rows = this->scene.rData.getHeight();
    int columns =  this->scene.rData.getWidth();
    float size = this->scene.rData.getCellSize();
    double NoValue = this->scene.rData.getNoValue();
    double offsetmin = 0;
    qglviewer::Vec minvec;
    minvec.x = -10;
    minvec.y = -10;
    minvec.z = 0;

    qglviewer::Vec maxvec;
    maxvec.x = this->scene.rData.getWidth()* this->scene.rData.getCellSize();
    maxvec.y = this->scene.rData.getHeight()* this->scene.rData.getCellSize();
    maxvec.z = 1000;
    this->setSceneBoundingBox(minvec, maxvec);
    double minValue =  NoValue;//= this->scene.rData.getMinValue();
    double maxValue = NoValue;//= this->scene.rData.getMaxValue();
    QImage map(columns, rows, QImage::Format_ARGB32);
    QImage heighmap(columns, rows, QImage::Format_ARGB32);
    //Interpolation of RasterData
    matrix<float> vals_tmp (columns+1, rows+1);
    matrix<Colors> RGBcolers_tmp(columns+1, rows+1);

    for ( long y = 0; y < vals_tmp.size2();  ++y) {
        for ( long x = 0; x < vals_tmp.size1();  ++x) {
            double sum = 0;
            double counter = 0;
            for ( long j = y-1; j <= y; j++ ) {
                for ( long i = x-1; i <= x; i++ ) {
                    double val = this->scene.rData.getValue(i,j);
                    if (val != NoValue) {
                        if (minValue == NoValue || val < minValue ) {
                            minValue = val;
                        }
                        if (maxValue == NoValue || val > maxValue ) {
                            maxValue = val;
                        }
                    }

                    if (NoValue != val ) {
                        sum += val;
                        counter++;

                    }
                }
            }
            vals_tmp(x,y) = (sum/counter);
        }
    }
    if (minValue < 0) {
        offsetmin = minValue*-1;
    }
    minValue = 0;
    maxValue = maxValue+offsetmin;
    //Create Color Interploations
    int n = 3;
    ap::real_1d_array R_x;
    ap::real_1d_array R_y;
    ap::real_1d_array G_x;
    ap::real_1d_array G_y;
    ap::real_1d_array B_x;
    ap::real_1d_array B_y;

    spline1dinterpolant R;
    spline1dinterpolant G;
    spline1dinterpolant B;


    R_x.setlength(n);
    R_y.setlength(n);
    G_x.setlength(n);
    G_y.setlength(n);
    B_x.setlength(n);
    B_y.setlength(n);
    //min
    R_x(0) = minValue;
    R_y(0) = 1.0;
    G_x(0) = minValue;
    G_y(0) = 0.0;
    B_x(0) = minValue;
    B_y(0) = 0.0;

    //middle
    R_x(1) = (maxValue + minValue)/2.;
    R_y(1) = 0.0;
    G_x(1) = (maxValue + minValue)/2.;
    G_y(1) = 1.0;
    B_x(1) = (maxValue + minValue)/2.;
    B_y(1) = 0.0;

    //max
    R_x(2) = maxValue;
    R_y(2) = 0.0;
    G_x(2) = maxValue;
    G_y(2) = 0.0;
    B_x(2) = maxValue;
    B_y(2) = 1.0;

    spline1dbuildlinear(R_x,R_y,n, R);
    spline1dbuildlinear(G_x,G_y,n, G);
    spline1dbuildlinear(B_x,B_y,n, B);

    for (int i=0; i<rows; i++) {
        for (int j=0; j<columns; j++) {
            Colors co;
            co.R = (float)spline1dcalc(R, this->scene.rData.getValue(j,i)+offsetmin);
            co.G = (float)spline1dcalc(G,this->scene.rData.getValue(j,i)+offsetmin);
            co.B = (float)spline1dcalc(B,this->scene.rData.getValue(j,i)+offsetmin);
            RGBcolers_tmp(j,i) = co;
            QColor c(co.R*255, co.G*255, co.B*255);
            //QColor c(255, 0, 0);
            map.setPixel(j,i,c.rgba());
            double colorread = (this->scene.rData.getValue(j,i)+offsetmin)*255/(maxValue-minValue);

            int cr = colorread/255;
            int yr = colorread - 255*cr;
            QColor c1(cr, yr, 0);
            heighmap.setPixel(j,i,c1.rgba());
        }
    }
    map.save("image.bmp");
    heighmap.save("heighmap.bmp");

    //vals_org = vals;
    QVector<QImage> v;
    v= maps[QString::fromStdString(this->scene.rData.getUUID()+"%"+(this->scene.rData.getName()))];
    v.append(map);
    maps[QString::fromStdString(this->scene.rData.getUUID()+"%"+(this->scene.rData.getName()))] = v;
    v= highmaps[QString::fromStdString(this->scene.rData.getUUID()+"%"+(this->scene.rData.getName()))];
    v.append(heighmap);
    highmaps[QString::fromStdString(this->scene.rData.getUUID()+"%"+(this->scene.rData.getName()))] = v;
    /** v= maps[QString::number(counterGlob)];
    v.append(map);
    maps[QString::number(counterGlob)] = v;
    v= highmaps[QString::number(counterGlob)];
    v.append(heighmap);
    highmaps[QString::number(counterGlob)] = v;**/
    //maps.append(map);
    //highmaps.append(heighmap);

    //this->list_vals_org.push_back(vals_tmp);
    //this->list_RGBcolers.push_back(RGBcolers_tmp);
    std::cout << rows << "\t" << columns << "\t" << size << "\t" << std::endl;
    if (this->FirstGeneration == true) {
        for (int i=0; i<rows; i++) {
            for (int j=0; j<columns+1; j++) {
                this->vertices.append(QVector3D(j*size,(i+1)*size, 0));
                this->vertices.append(QVector3D(j*size, i*size,0));
                this->texCoords.append(QVector2D(j*size/(columns*size),(i+1)*size/(rows*size)));
                this->texCoords.append(QVector2D(j*size/(columns*size), i*size/(rows*size)));
            }
        }

        std::cout << "SetData" << std::endl;

        this->FirstGeneration  = false;
    }
    animate();
    dataavalible = true;


}

void Viewer::animate() {
    if (wait ==1) {
        if (dataavalible) {

            sumscale = sumscale+scale;
            if (sumscale > 1) {
                sumscale = 0;
            }
            if (sumscale == 0) {
                sumscale = 0;
                if (this->current < this->totalData-2) {
                    std::cout << "Current " << this->current << std::endl;
                    this->current++;
                } else {
                    this->current = 0;
                }
            }




            drawable = true;


        }
        wait = 0;
    }
    wait++;
}
void  Viewer::drawWithNames() {

    //if (drawVectorData) {
    drawGridDataWithName();
    /*  glPushMatrix();

        glPushName(1);

        glCallList(currList);


        glPopName();

        glPopMatrix();*/

    return;
    //}


    // const float nbSteps = 200.0;
    //glPushMatrix();
    //lPushName(0);
    // drawGridData();
    /*  glBegin(GL_QUAD_STRIP);

    for (int i=0; i<nbSteps; ++i)
    {
        const float ratio = i/nbSteps;
        const float angle = 21.0*ratio;
        const float c = cos(angle);
        const float s = sin(angle);
        const float r1 = 1.0 - 0.8f*ratio;
        const float r2 = 0.8f - 0.8f*ratio;
        const float alt = ratio - 0.5f;
        const float nor = 0.5f;
        const float up = sqrt(1.0-nor*nor);
        glColor3f(1.0-ratio, 0.2f , ratio);
        glNormal3f(nor*c, up, nor*s);
        glVertex3f(r1*c, alt, r1*s);
        glVertex3f(r2*c, alt+0.05f, r2*s);
    }
    glEnd();*/

    //glPopName();
    //glPopMatrix();
}

void Viewer::draw()

{

    if (this->currentYear < 0 )
        return;
    /* const float nbSteps = 200.0;
    glPushMatrix();

     glBegin(GL_QUAD_STRIP);

    for (int i=0; i<nbSteps; ++i)
    {
        const float ratio = i/nbSteps;
        const float angle = 21.0*ratio;
        const float c = cos(angle);
        const float s = sin(angle);
        const float r1 = 1.0 - 0.8f*ratio;
        const float r2 = 0.8f - 0.8f*ratio;
        const float alt = ratio - 0.5f;
        const float nor = 0.5f;
        const float up = sqrt(1.0-nor*nor);
        glColor3f(1.0-ratio, 0.2f , ratio);
        glNormal3f(nor*c, up, nor*s);
        glVertex3f(r1*c, alt, r1*s);
        glVertex3f(r2*c, alt+0.05f, r2*s);
    }
    glEnd();

    glPopMatrix();

    // Draw the intersection line
    glBegin(GL_LINES);
    glVertex3fv(orig);
    glVertex3fv(orig + 100.0*dir);
    glEnd();

    // Draw (approximated) intersection point on selected object
    if (selectedName() >= 0)
    {
        glColor3f(0.9f, 0.2f, 0.1f);
        glBegin(GL_POINTS);
        glVertex3fv(selectedPoint);
        glEnd();
    }

    return;*/
    //Draw Text
    glColor3f(1.0f, 1.0f , 1.0f);
    if (drawVectorData) {



        glDisable(GL_LIGHTING);

        drawText(this->width()- 100,this->height()- 20, QString::number(currentYear));
        //drawText(20, 20, "Sum Population "+ QString::number(this->scenedata_grid[this->currentYear]->Sum));
        glEnable(GL_LIGHTING);
        glPushMatrix();
        //glCallList(currList);
        drawGridData();
        drawNetworkData();
        glPopMatrix();

        /*    // Draw the intersection line
        glBegin(GL_LINES);
        glVertex3fv(orig);
        glVertex3fv(orig + 100.0*dir);
        glEnd();

        // Draw (approximated) intersection point on selected object
        if (selectedName() >= 0)
        {
            glColor3f(0.9f, 0.2f, 0.1f);
            glBegin(GL_POINTS);
            glVertex3fv(selectedPoint);
            glEnd();
        }*/


        return;
    }
    if (glActiveTexture == 0) {

        return;
    }

    if (this->drawable == true) {
        if (this->arrangement == VERTICAL) {
            offsetx = 0;
            offsety = 0;
            offsetz = 2000;
        }
        if (this->arrangement == LINEX) {
            offsetx = this->scene.rData.getWidth()*this->scene.rData.getCellSize()+100;
            offsety = 0;
            offsetz = 0;
        }
        if (this->arrangement == LINEY) {
            offsetx = 0;
            offsety = this->scene.rData.getHeight()*this->scene.rData.getCellSize()+100;
            offsetz = 0;
        }
        int counter = 0;
        int counter2 = 0;
        foreach(QString s, maps.keys()) {
            counter++;
            if ((maps.keys().size()-counter) < 10) {
                //if (texturesSet == true)
                //glDeleteTextures(4, textures);

                program->bind();
                program->enableAttributeArray(vertexLocation);
                program->enableAttributeArray(texVertLocation);
                program->setAttributeArray(vertexLocation, vertices.constData());
                program->setAttributeArray(texVertLocation, texCoords.constData());

                textures[0] = bindTexture (maps[s][current], GL_TEXTURE_2D);
                textures[2] = bindTexture (highmaps[s][current], GL_TEXTURE_2D);
                if (maps[s].size() == 1) {
                    textures[1] = bindTexture (maps[s][current], GL_TEXTURE_2D);
                    textures[3] = bindTexture (highmaps[s][current], GL_TEXTURE_2D);
                } else {
                    textures[1] = bindTexture (maps[s][current+1], GL_TEXTURE_2D);
                    textures[3] = bindTexture (highmaps[s][current+1], GL_TEXTURE_2D);
                }
                texturesSet = true;
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textures[0]);
                program->setUniformValue(textureLocation, 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, textures[1]);
                program->setUniformValue(textureLocation1, 1);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, textures[2]);
                program->setUniformValue(textureLocationHeighMap, 2);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, textures[3]);
                program->setUniformValue(textureLocationHeighMap1, 3);
                program->setUniformValue("multiplier", multiplier);
                program->setUniformValue("scalevert", sumscale);
                program->setUniformValue("scale", sumscale);
                program->setUniformValue("offsetx", offsetx*counter2);
                program->setUniformValue("offsety", offsety*counter2);
                program->setUniformValue("offsetz", offsetz*counter2);
                for (int i=0; i<Height; i++) {
                    glDrawArrays(GL_TRIANGLE_STRIP, (Width+1)*i*2, (Width+1)*2);
                }
                program->disableAttributeArray(vertexLocation);
                program->disableAttributeArray(texVertLocation);
                counter2++;
                program->release();
            }
        }

    }
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_LIGHTING);
    drawText(20,20,"Time " + QString::number(current));
    glEnable(GL_LIGHTING);


}


void Viewer::init()
{


    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_CULL_FACE);
    getGLExtensionFunctions().resolve(this->context());

    //this->createShader();

    startAnimation();


    if (glActiveTexture == 0) {

        vibens::Logger(vibens::Error) << "glActiveTexture";
        return;
    }

    this->setBackgroundColor(Qt::black);

}

void Viewer::setVectorData(VectorData vec) {
    qglviewer::Vec minvec;
    minvec.x = -10000;
    minvec.y = -10000;
    minvec.z = -1;

    qglviewer::Vec maxvec;
    maxvec.x = 10000;
    maxvec.y = 10000;
    maxvec.z = 10000;
    this->setSceneBoundingBox(minvec, maxvec);
    vibens::Logger(vibens::Debug) << "SetVectorData";
    Attribute attr;
    attr = vec.getAttributes("Globals");
    int year = attr.getAttribute("Year");
    vibens::Logger(vibens::Debug) << "SetVectorData ";
    vibens::Logger(vibens::Debug) << year;
    this->currentYear = year;
    std::cout << "DataType" << attr.getStringAttribute("Datatype") << std::endl;
    if (attr.getStringAttribute("Datatype").compare("GridData") == 0) {
        std::cout << "GridData" << std::endl;
        GridData * grid = new GridData();
        grid->setYear(year);
        grid->setCurrentAttribute("Population");
        grid->setVectorData(vec);
        grid->Sum = GriddataHelper::sum(&vec,"GRID_", "Population");
        this->scenedata_grid[year] = grid;
        grid->createDataForViewer();

        vibens::Logger(vibens::Debug) << "Population ";
        vibens::Logger(vibens::Debug) <<  grid->Sum;
    }
    if (attr.getStringAttribute("Datatype").compare("ParcelBased") == 0) {
        std::cout << "Parcelbased" << std::endl;
        ParcelData * grid = new ParcelData();
        grid->setYear(year);
        grid->setCurrentAttribute("Population");
        grid->setVectorData(vec);
        this->scenedata_grid[year] = grid;
        grid->createDataForViewer();

        vibens::Logger(vibens::Debug) << "Population ";
        vibens::Logger(vibens::Debug) <<  grid->Sum;
    }

    /*  if (attr.getStringAttribute("DataType").compare("NetworkData")) {
        NetworkData * net = new NetworkData();
        net->Year = year;
        net->CurrenAttribute = "Diameter";
        net->vec = vec;
        this->scenedata_network[year] = net;
        this->createNetworkData(year);
    }*/


}
void Viewer::setRasterData(RasterData rData) {
    this->scene.rData = rData;
}
void  Viewer::setRasterData(QString UUID,QString name) {
    sceneChanged = true;
    std::cout << "SetRasterData" << std::endl;
    RasterData *r =& vibens::DataManagement::getInstance().getDataBase()->getRasterData(UUID.toStdString(), name.toStdString());

    this->scene.rData = RasterData();
    this->scene.rData.setSize(r->getWidth(), r->getHeight(), r->getCellSize());
    std::cout << "SetRasterData " << r->getValue(0,0) << std::endl;
    for (int i = 0; i < r->getWidth();  i++) {
        for (int j = 0; j < r->getHeight(); j++) {
            this->scene.rData.setValue(i,j,r->getValue(i,j));
        }
    }
    this->redrawRasterData();
    this->Height = this->scene.rData.getHeight();
    this->Width = this->scene.rData.getWidth();
    this->cellSize = this->scene.rData.getCellSize();
    this->totalData++;
    std::cout << "Delta " << totalData<< std::endl;
}
void  Viewer::addRasterData(QVector<RasterData> rv) {

    QObject * obj = QObject::sender();
    sceneChanged = true;
    //if (strangeError == 0) {
    std::cout << "SetRasterData" << std::endl;
    foreach (RasterData r, rv) {
        this->scene.rData = r;
        this->Height = this->scene.rData.getHeight();
        this->Width = this->scene.rData.getWidth();
        this->cellSize = this->scene.rData.getCellSize();
        this->redrawRasterData();
    }
    counterGlob++;
    this->totalData++;

}
void  Viewer::addVectorData(QVector<VectorData> rv) {

    QObject * obj = QObject::sender();
    sceneChanged = true;
    //if (strangeError == 0) {
    std::cout << "SetVectorData" << std::endl;
    foreach (VectorData r, rv) {
        /*this->scene.rData = r;
        this->Height = this->scene.rData.getHeight();
        this->Width = this->scene.rData.getWidth();
        this->cellSize = this->scene.rData.getCellSize();
        this->redrawRasterData();*/
        GriddataHelper::createColorTable(&r, "GRID_", "Population");
        this->setVectorData(r);
    }
    //counterGlob++;
    //this->totalData++;



}
Viewer::Viewer(QWidget* parent, bool antialiasing)
    : QGLViewer(parent)
{

    this->currentYear = -1;
    drawVectorData = true;
    //#ifdef _WIN32
    //glActiveTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress("glActiveTexture");
    if (glActiveTexture == 0) {
        vibens::Logger( vibens::Error) << "glActiveTexture" ;
    }
    //#endif
    this->setFPSIsDisplayed(false);
    //this->setStereoDisplay(true);

    this->drawable = false;
    this->FirstGeneration = true;
    dataavalible = false;
    // Restore previous viewer state.
    this->currList = -1;
    this->sumscale = 0;
    this->current = 0;
    texturesSet = false;
    wait = 0;
    totalData = 0;
    this->multiplier = 1;
    scale = 0.1;
    this->arrangement = 1;
    this->counterGlob = 0;


    init();




}
void Viewer::setMultiplier(int val) {
    this->multiplier = val;
}
void Viewer::setSpeed(double val) {
    this->scale = val;
}
void Viewer::createShader() {
    std::cout << "Shader Start" << std::endl;
    program = new QGLShaderProgram(this);
    QGLShader *vs = new QGLShader(QGLShader::Vertex);
    if (!vs->compileSourceCode(   "attribute highp vec4 vertex;\n"
                                  "uniform sampler2D heigh, heigh1;\n"
                                  "attribute mediump vec4 texCoord;\n"
                                  "uniform float scalevert;\n"
                                  "uniform float multiplier;\n"
                                  "uniform float offsetx;\n"
                                  "uniform float offsety;\n"
                                  "uniform float offsetz;\n"
                                  "varying mediump vec4 texc;\n"
                                  "void main(void)\n"
                                  "{\n"
                                  "   vec4 newVertexPos, dv;\n"
                                  "   float df;\n"
                                  "   texc = texCoord;\n"
                                  "vec4 col1 = texture2D(heigh,texc.st)*multiplier;"
                                  "vec4 col2 = texture2D(heigh1,texc.st)*multiplier;"
                                  "vec4 coldelta = col1-col2;"
                                  "vec4 c = col1-coldelta*scalevert;"
                                  "   newVertexPos = vertex;\n"
                                  "   newVertexPos.z = (c.r*255+c.g)\n;"
                                  "   newVertexPos.x = newVertexPos.x+offsetx\n;"
                                  "   newVertexPos.y = newVertexPos.y+offsety\n;"
                                  "   newVertexPos.z = newVertexPos.z+offsetz\n;"
                                  "   gl_Position = gl_ModelViewProjectionMatrix*newVertexPos;\n"
                                  "}")) {
        std::cout << "ERRORR" << std::endl;
        std::cout <<vs->log().toStdString() << std::endl;
    }
    QGLShader *fs = new QGLShader(QGLShader::Fragment);
    if (!fs->compileSourceCode("uniform sampler2D texture, texture1;\n"
                               "uniform float scale;\n"
                               "varying mediump vec4 texc;\n"
                               "void main(void)\n"
                               "{\n"
                               "vec4 col1 = texture2D(texture,texc.st);"
                               "vec4 col2 = texture2D(texture1,texc.st);"
                               "vec4 coldelta = col1-col2;"
                               "vec4 c = col1-coldelta*scale;"
                               "    gl_FragColor = c;\n"
                               "}\n"))
    {
        std::cout << "ERRORR1" << std::endl;
        std::cout <<vs->log().toStdString() << std::endl;
    }
    program->addShader(vs);
    program->addShader(fs);

    program->link();
    program->bind();

    vertexLocation = program->attributeLocation("vertex");
    texVertLocation = program->attributeLocation("texCoord");
    textureLocation = program->uniformLocation("texture");
    textureLocation1 = program->uniformLocation("texture1");
    textureLocationHeighMap = program->uniformLocation("heigh");
    textureLocationHeighMap1 = program->uniformLocation("heigh1");

}
void Viewer::postSelection(const QPoint& point) {
    // Compute orig and dir, used to draw a representation of the intersecting line
    camera()->convertClickToLine(point, orig, dir);

    // Find the selectedPoint coordinates, using camera()->pointUnderPixel().
    bool found;
    selectedPoint = camera()->pointUnderPixel(point, found);
    selectedPoint -= 0.01f*dir; // Small offset to make point clearly visible.
    // Note that "found" is different from (selectedObjectId()>=0) because of the size of the select region.

    if (selectedName() == -1)
        QMessageBox::information(this, "No selection",
                                 "No object selected under pixel " + QString::number(point.x()) + "," + QString::number(point.y()));
    else {
        stringstream ss;
        ss << "GRID_" << selectedName();
      /*  GridData * grid = this->scenedata_grid[this->currentYear];
        Attribute attr = grid->vec.getAttributes(ss.str());

        std::vector<std::string> names = grid->getAttributesInGridCell(selectedName());

        foreach(std::string name, names)
            std::cout << name << std::endl;


        QMessageBox::information(this, "Selection",
                                 "Grid number " + QString::number(selectedName()) + " Population: " + QString::number(attr.getAttribute("Population")));*/



    }

    //vibens::Logger(vibens::Standard) << "Somthing Selected";
}
void Viewer::createNetworkData(int Year) {
    NetworkData * net = this->scenedata_network[Year];
    foreach (std::string name, VectorDataHelper::findElementsWithIdentifier("Conduit", net->vec.getEdgeNames()) ) {
        std::vector<Edge> edges =  scene.vec.getEdges(name);
        std::vector<Point> points =  scene.vec.getPoints(name);
        foreach( Edge e, edges ) {
            net->VectorCoordinates.push_back((float)points[e.id1].x);
            net->VectorCoordinates.push_back((float)points[e.id1].y);
            net->VectorCoordinates.push_back((float)points[e.id1].z);
            net->VectorCoordinates.push_back((float)points[e.id2].x);
            net->VectorCoordinates.push_back((float)points[e.id2].y);
            net->VectorCoordinates.push_back((float)points[e.id2].z);
        }
    }

}

void Viewer::createGridData(int Year) {

    /* this->tess= gluNewTess();
#ifdef _WIN32
    gluTessCallback (tess, GLU_TESS_BEGIN, (void (__stdcall*)()) glBegin);
    gluTessCallback (tess, GLU_TESS_VERTEX, (void (__stdcall*)()) glVertex3dv);
    gluTessCallback (tess, GLU_TESS_COMBINE, (void (__stdcall*)()) tcbCombine);
    gluTessCallback (tess, GLU_TESS_END, (void (__stdcall*)()) glEnd);
    gluTessCallback(tess, GLU_TESS_ERROR,  (void (__stdcall*)()) errorCallback);
#else
    gluTessCallback (tess, GLU_TESS_BEGIN, (void(*)()) glBegin);
    gluTessCallback (tess, GLU_TESS_VERTEX,  (void(*)()) glVertex3dv);
    gluTessCallback (tess, GLU_TESS_COMBINE, (void(*)())  tcbCombine);
    gluTessCallback (tess, GLU_TESS_END, (void(*)()) glEnd);
    gluTessCallback(tess, GLU_TESS_ERROR,   (void(*)())  errorCallback);
#endif
*/
    //GridData * grid = this->scenedata_grid[Year];
    qglviewer::Vec minvec;
    minvec.x = -10000;
    minvec.y = -10000;
    minvec.z = -1;

    qglviewer::Vec maxvec;
    maxvec.x = 10000;
    maxvec.y = 10000;
    maxvec.z = 10000;
    this->setSceneBoundingBox(minvec, maxvec);




}
void Viewer::drawGridDataWithName() {
    if (this->scenedata_grid.keys().indexOf(this->currentYear) == -1)
        return;
    ILinkedVectorData * grid= this->scenedata_grid[this->currentYear];
    int length =  grid->getVectorCoordinates().size() / 12;

    int k = 0;
    if (grid->getVectorCoordinates().size()  == 0)
        return;



    for (int i = 0; i < length; i++) {
        glPushMatrix();
        glPushName(i);
        glBegin(GL_QUADS);
        for (int j = 0; j < 4; j++) {
            glColor3f(grid->getVectorColor()[k],grid->getVectorColor()[k+1], grid->getVectorColor()[k+2]);
            glVertex3f(grid->getVectorCoordinates()[k], grid->getVectorCoordinates()[k+1], grid->getVectorCoordinates()[k+2]);
            k +=3;
        }

        glEnd();
        glPopName();
        glPopMatrix();


    }



}


void Viewer::drawGridData() {

    if (this->scenedata_grid.keys().indexOf(this->currentYear) == -1)
        return;

    ILinkedVectorData * grid= this->scenedata_grid[this->currentYear];

    int length = grid->getVectorCoordinates().size() / 12;

    int k = 0;
    if (grid->getVectorCoordinates().size()  == 0)
        return;
    for (int i = 0; i < length; i++) {
        glBegin(GL_QUADS);
        for (int j = 0; j < 4; j++) {
            glColor3f(grid->getVectorColor()[k],grid->getVectorColor()[k+1], grid->getVectorColor()[k+2]);
            glVertex3f(grid->getVectorCoordinates()[k], grid->getVectorCoordinates()[k+1], grid->getVectorCoordinates()[k+2]);
            k +=3;
        }
        glEnd();
    }
}

void Viewer::drawNetworkData() {

    if (this->scenedata_network.keys().indexOf(this->currentYear) == -1)
        return;

    NetworkData * grid= this->scenedata_network[this->currentYear];

    int length = grid->VectorCoordinates.size() / 6;
    /*   glBegin(GL_LINES);
        {

            foreach (std::string name, scene.vec.getEdgeNames() ) {
                std::vector<Edge> edges =  scene.vec.getEdges(name);
                std::vector<Point> points =  scene.vec.getPoints(name);
                foreach( Edge e, edges ) {
                    glVertex3f(float(points[e.id1].x), float(points[e.id1].y), float(points[e.id1].z));
                    glVertex3f(float(points[e.id2].x), float(points[e.id2].y), float(points[e.id2].z));
                }
            }
        }
        glEnd();
        */
    int k = 0;
    if (grid->VectorCoordinates.size()  == 0)
        return;

    for (int i = 0; i < length; i++) {
        glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f , 1.0f);
        glPointSize(1.0f);
        for (int j = 0; j < 2; j++) {
            //glColor3f(grid->VectorColor[k],grid->VectorColor[k+1], grid->VectorColor[k+2]);
            glVertex3f(grid->VectorCoordinates[k], grid->VectorCoordinates[k+1], grid->VectorCoordinates[k+2]);
            k +=3;
        }
        glEnd();
    }
}

/*  foreach (std::string name, scene.vec.getFaceNames() ) {
        if (QString::fromStdString(name).contains("COLOR") == false ) {
            bool existsColor = false;
            std::stringstream s;
            s << "COLOR_" << name;
            foreach(std::string n, scene.vec.getPointsNames()) {
                if (n.compare(s.str()) == 0) {
                    existsColor = true;
                    break;
                }
            }

            std::vector<Face> faces =  scene.vec.getFaces(name);
            std::vector<Point> points =  scene.vec.getPoints(name);
            std::vector<Point> colors;
            if (existsColor == true) {
                colors = scene.vec.getPoints(s.str());
            }
            foreach( Face f, faces ) {

                glBegin(GL_QUADS);
                foreach(int id, f.ids) {
                    if (existsColor == true) {
                        glColor3f( colors[id].x, colors[id].y, colors[id].z);
                    } else {
                        glColor3f( 0.0, 1.0, 0.0);
                    }
                    glVertex3f((float)points[id].x, (float)points[id].y, (float)points[id].z);

                }
                glEnd();
            }
        }
    }*/

/*   glBegin(GL_LINES);
    {
        glColor3f(1.0f, 1.0f , 1.0f);
        glPointSize(1.0f);
        foreach (std::string name, scene.vec.getEdgeNames() ) {
            std::vector<Edge> edges =  scene.vec.getEdges(name);
            std::vector<Point> points =  scene.vec.getPoints(name);
            foreach( Edge e, edges ) {
                glVertex3f(float(points[e.id1].x), float(points[e.id1].y), float(points[e.id1].z));
                glVertex3f(float(points[e.id2].x), float(points[e.id2].y), float(points[e.id2].z));
            }
        }
    }
    glEnd();
    glBegin(GL_LINES);
    {
        glColor3f(1.0f, 1.0f , 1.0f);
        glPointSize(1.0f);
        foreach (std::string name, scene.vec.getEdgeNames() ) {
            std::vector<Edge> edges =  scene.vec.getEdges(name);
            std::vector<Point> points =  scene.vec.getPoints(name);
            foreach( Edge e, edges ) {
                glVertex3f(float(points[e.id1].x), float(points[e.id1].y), float(points[e.id1].z));
                glVertex3f(float(points[e.id2].x), float(points[e.id2].y), float(points[e.id2].z));
            }
        }
    }
    glEnd();
    glBegin(GL_POINTS);
    {
        glPointSize(4.0f);
        glColor3f(1.0f, 0.0f , 0.0f);

        foreach (std::string name, scene.vec.getPointsNames()) {
            std::vector<Point> points =  scene.vec.getPoints(name);
            foreach( Point p, points ) {
                glVertex3f(float(p.x), float(p.y), float(p.z));
            }
        }
    }
    glEnd();*/
//glEndList();


