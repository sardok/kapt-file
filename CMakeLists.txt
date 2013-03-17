cmake_minimum_required(VERSION 2.8)

#Update cmake module path
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

# Project Name
PROJECT(kapt-file)

# The version numbers
SET (cin_VERSION_MAJOR 0)
SET (cin_VERSION_MINOR 1)

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(AptPkg REQUIRED)

# Set source files
FILE (GLOB SOURCES "src/*.cc")

# Set header files
FILE (GLOB HEADERS "src/*.h")

# Set Moc headers
SET (MOC_HEADERS src/http_resource_medium.h;src/resource_manager.h;src/main_window.h)
# Pass moc headers to moc stage
QT4_WRAP_CPP(HEADERS_MOC ${MOC_HEADERS})

# Set Ui files
SET (UI_FILES_RAW src/mainwindow.ui)
# Processed ui file to uic
QT4_WRAP_UI(UI_FILES_PRCS ${UI_FILES_RAW})

# Enabled network module
SET (QT_USE_QTNETWORK TRUE)

INCLUDE ( ${QT_USE_FILE} )
ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES(
  ${APTPKG_INCLUDEDIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

SET (CMAKE_CXX_FLAGS "-std=c++11 -Wall -g ${CMAKE_CXX_FLAGS}")


# Add the executable and libraries to be linked against.
ADD_EXECUTABLE(kapt-file
  ${SOURCES}
  ${HEADERS_MOC}
  ${UI_FILES_PRCS})

TARGET_LINK_LIBRARIES (kapt-file ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES (kapt-file ${APTPKG_LIBRARY_RELEASE})