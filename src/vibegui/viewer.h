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

#ifndef VIEWER_H
#define VIEWER_H

#include "compilersettings.h"

#include <QGLViewer/qglviewer.h>
#include <QGLShaderProgram>
#include "vector"
#include "rasterdata.h"
#include "vectordata.h"
#include <GL/gl.h>
//#include "GL/glu.h"
#include <list>
#include "ILinkedVectorData.h"

//#ifdef _WIN32
//#include <glext.h>
//extern PFNGLACTIVETEXTUREPROC glActiveTexture;  //Put this in a .h so that you can include the header in all your other .cpp
//#endif


class QWidget;
struct VIBE_HELPER_DLL_EXPORT Scene {
    VectorData vec;
    RasterData rData;
};
enum  ARRANMGMENTS {
    VERTICAL,
    LINEX,
    LINEY
};


struct NetworkData {
    QString CurrenAttribute;
    int Year;
    VectorData vec;
    std::vector<float> VectorCoordinates;
    std::vector<float> VectorColor;
};

class VIBE_HELPER_DLL_EXPORT Viewer : public QGLViewer {

    Q_OBJECT
    struct Colors{
        float R;
        float G;
        float B;
    };
protected:
    virtual void draw();
    virtual void drawWithNames();
    void drawGridData();
    void  drawNetworkData();
    void drawGridDataWithName();
    virtual void init();
    virtual void animate();
    float sumscale;
    Scene scene;
    GLuint currList;
    bool sceneChanged;
    virtual void postSelection(const QPoint& point);
    QMap<QString, QVector<QImage> > maps;
    QMap<QString, QVector<QImage> > highmaps;
    int totalData;
    int wait;

    QVector<QVector3D> vertices;
    QVector<QVector2D> texCoords;
    int numberOfLists;
    int current;
    int Height;
    int Width;
    int strangeError;
    double cellSize;


    int vertexLocation;
    int texVertLocation;
    int textureLocation;
    int textureLocation1;
    int textureLocationHeighMap;
    int textureLocationHeighMap1;
    float multiplier;
    double scale;
    void createShader();
    int arrangement;

    float offsetx;
    float offsety;
    float offsetz;
    int counterGlob;

    QString text;

    int currentYear;
public:

    Viewer(QWidget * parent, bool antialiasing = false);
    void createGridData(int Year);
    void createNetworkData(int Year);


    void redrawRasterData();
    void setVectorData(VectorData vec);
    void setRasterData(RasterData rData);

    bool drawVectorData;
    bool dataavalible;
    bool drawable;
    bool texturesSet;


signals:
    void update();

private:
    QMap<int, ILinkedVectorData * > scenedata_grid;
    QMap<int, NetworkData * > scenedata_network;
    QGLShaderProgram *program;
    GLuint textures[4];
    bool FirstGeneration;
    ~Viewer();
    qglviewer::Vec orig, dir, selectedPoint;
    GLUtesselator* tess;

    std::vector<float> * VectorCoordinates;
    std::vector<float> * VectorColor;



public slots:
    void setCurrentYear(int Year) {this->currentYear = Year;}
    void setRasterData(QString,QString);
    void addRasterData(QVector<RasterData>);
    void addVectorData(QVector<VectorData>);
    void setMultiplier(int);
    void setSpeed(double);
    void setArrangment(int);
}; // end class Viewer



#endif // VIEWER_H
