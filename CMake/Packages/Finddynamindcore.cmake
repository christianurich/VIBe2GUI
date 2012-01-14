find_package(PkgConfig)

FIND_PATH(DYNAMINDCORE_INCLUDE_DIRS module.h  PATH_SUFFIXES dynamindcore) 
FIND_LIBRARY(DYNAMINDCORE_LIBRARIES dynamindcore)  

include(FindPackageHandleStandardArgs)
