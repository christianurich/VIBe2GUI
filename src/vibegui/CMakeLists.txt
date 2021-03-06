find_package(OpenGL)


include_directories(${VIBE2CORE_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}../vibegui)
include_directories(.)
set(MOC_FILES  guilogsink.h gui3drasterdata.h mainwindow.h plot.h viewer.h preferences.h guimodelnode.h modelnode.h guilink.h StdRedirector.h application.h modelnodebutton.h simulationmanagment.h groupnode.h guiresultobserver.h guiimageresultview.h guidataobserver.h)
file (GLOB CPP_GUI_FILES *.cpp)
file (GLOB UI_FILES *.ui)
#set (MOC_FILES datamanagment.h)
QT4_WRAP_UI(CPP_GUI_FILES ${UI_FILES})
QT4_WRAP_CPP(CPP_GUI_FILES ${MOC_FILES})
QT4_ADD_RESOURCES(CPP_GUI_FILES icons.qrc splash.qrc)
file(GLOB QWT_CPPS ../3dparty/qwt-5.2.0/src/*.cpp)
file(GLOB QWT_H ../3dparty/qwt-5.2.0/src/*.h)
QT4_WRAP_CPP(QWT_CPPS ${QWT_H})
file(GLOB ALGLIB_CPP ../vibehelpers/alglib/src/*.cpp)

# Find OpenGL
include_directories( ${QGLVIEWER_INCLUDE_DIR})
include_directories( ${QT_QTGUI_INCLUDE_DIR})
include_directories(../3dparty/qwt-5.2.0/src)
include_directories(../3dparty/alglib/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(vibe2gui SHARED ${CPP_GUI_FILES}  ${QWT_CPPS} ${QTGL_CPPS} ${ALGLIB_CPP})
target_link_libraries(vibe2gui ${VIBE2CORE_LIBRARIES} ${VIBE2HELPER_LIBRARIES} ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${QGLVIEWER_LIBRARIES})


INSTALL(TARGETS vibe2gui
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
