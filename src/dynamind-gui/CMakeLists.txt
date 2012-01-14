include_directories(${VIBE2CORE_INCLUDE_DIRS})
include_directories(../vibegui/)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../vibegui/)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(muparser/include ../3dparty/qwt-5.2.0/src)
file(GLOB VIBE2 *.cpp)
QT4_ADD_RESOURCES(VIBE2 icons.qrc)

add_executable(vibe2-gui ${VIBE2} )
target_link_libraries(vibe2-gui vibe2gui ${VIBE2CORE_LIBRARIES} ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${QGLVIEWER_LIBRARIES})

INSTALL(TARGETS vibe2-gui
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)


