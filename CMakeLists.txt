project(ProvidenceKOSLookup)
cmake_minimum_required(VERSION 2.6.2)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Qt4 REQUIRED QtCore QtGui QtXml QtScript)
find_package(OpenSSL REQUIRED)

if( NOT WIN32 )
  if(${QT_PHONON_LIBRARY})
    set(PHONON_LIBRARY_ ${QT_PHONON_LIBRARY})
    set(PHONON_INCLUDE_DIR_ ${QT_PHONON_INCLUDE_DIR})
    set(PHONON_FOUND_ TRUE)
  else()
    find_package(Phonon)
    set(PHONON_LIBRARY_ ${PHONON_LIBRARY})
    set(PHONON_INCLUDE_DIR_ ${PHONON_INCLUDE_DIR})
    set(PHONON_FOUND_ TRUE)
  endif()
else()
   include(${QT_USE_FILE})
   set(QT_USE_QTMAIN TRUE)
   set(RCFILE res/app.rc)
   set(WIN_APPRC "res/app.rc")
   set(PHONON_LIBRARY_ ${QT_PHONON_LIBRARY})
   set(PHONON_INCLUDE_DIR_ ${QT_PHONON_INCLUDE_DIR})
   set(PHONON_FOUND_ TRUE)
   set(GUI_TYPE WIN32)
   set(_QT_EXTRA_LIBRARIES ${QT_QTMAIN_LIBRARY})
endif()

if(NOT PHONON_FOUND_)
 message(FATAL_ERROR "Phonon Library not found!")
endif(NOT PHONON_FOUND_)

include_directories(${QT_INCLUDES} ${PHONON_INCLUDE_DIR_} ${CMAKE_CURRENT_BINARY_DIR})

OPTION (BUILD_SHARED_LIBS "Build shared libraries." OFF)
set(CMAKE_AUTOMOC TRUE)
SET(QT_USE_PHONON TRUE)

add_subdirectory(lib)
add_subdirectory(ui)


QT4_ADD_RESOURCES(ProvidenceKOSLookup_RCC res/ressources.qrc)

add_executable(ProvidenceKOSLookup ${GUI_TYPE} ProvidenceKOSLookup.cpp main.cpp ${ProvidenceKOSLookup_RCC} ${WIN_APPRC})
target_link_libraries(ProvidenceKOSLookup pkosMainWindow pkosLogWatcher pkosNetwork pkosEveAPI pkosCvaAPI pkosAudio pkosCacheDB pkosHtDB ${QT_QTCORE_LIBRARY} ${_QT_EXTRA_LIBRARIES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET_TARGET_PROPERTIES(ProvidenceKOSLookup PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
    SET_TARGET_PROPERTIES(ProvidenceKOSLookup PROPERTIES RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
    SET_TARGET_PROPERTIES(ProvidenceKOSLookup PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    SET_TARGET_PROPERTIES(ProvidenceKOSLookup PROPERTIES MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)

SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/ProvidenceKOSLookup")
IF(WIN32)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/ProvidenceKOSLookup.exe")
ENDIF(WIN32)

install(TARGETS ProvidenceKOSLookup RUNTIME DESTINATION bin COMPONENT Runtime)
# WINDOWS INSTALLER STUFF
set(_OPENSSL_ROOT_HINTS
  ${OPENSSL_ROOT_DIR}
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OpenSSL (32-bit)_is1;Inno Setup: App Path]"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OpenSSL (64-bit)_is1;Inno Setup: App Path]"
  ENV OPENSSL_ROOT_DIR
  )
file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _programfiles)
set(_OPENSSL_ROOT_PATHS
  "${_programfiles}/OpenSSL"
  "${_programfiles}/OpenSSL-Win32"
  "${_programfiles}/OpenSSL-Win64"
  "C:/OpenSSL/"
  "C:/OpenSSL-Win32/"
  "C:/OpenSSL-Win64/"
  )

unset(_programfiles)
set(_OPENSSL_ROOT_HINTS_AND_PATHS
  HINTS ${_OPENSSL_ROOT_HINTS}
  PATHS ${_OPENSSL_ROOT_PATHS}
  )

set(CMAKE_FIND_LIBRARY_SUFFIXES .dll)
find_library(OPENSSL_libeay32 
  NAMES libeay32
  ${_OPENSSL_ROOT_HINTS_AND_PATHS}
  NO_DEFAULT_PATH
)

find_library(OPENSSL_ssleay32
  NAMES ssleay32
  ${_OPENSSL_ROOT_HINTS_AND_PATHS}
  NO_DEFAULT_PATH
)

if(WIN32)
install(FILES ${QT_PLUGINS_DIR}/phonon_backend/phonon_ds94.dll 
        DESTINATION ${plugin_dest_dir}/plugins/phonon_backend COMPONENT Runtime)
install(FILES ${QT_PLUGINS_DIR}/imageformats/qjpeg4.dll 
        DESTINATION ${plugin_dest_dir}/plugins/imageformats COMPONENT Runtime)
install(FILES ${QT_PLUGINS_DIR}/sqldrivers/qsqlite4.dll 
        DESTINATION ${plugin_dest_dir}/plugins/sqldrivers COMPONENT Runtime)
install(FILES ${OPENSSL_libeay32} ${OPENSSL_ssleay32} 
        DESTINATION ${plugin_dest_dir} COMPONENT Runtime)

INSTALL(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime)

SET(DIRS ${QT_LIBRARY_DIR} ${QT_BINARY_DIR})
INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime)
set(CPACK_GENERATOR NSIS)

endif()
set(CPACK_PACKAGE_NAME "ProvidenceKOS")
set(CPACK_PACKAGE_VENDOR "lokis-chaos.de")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ProvidenceKOS - a KOS Checker")
set(CPACK_PACKAGE_VERSION "1.5.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "5")
set(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_EXECUTABLES "ProvidenceKOSLookup;Providence KOS Lookup")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ProvidenceKOS")
include(CPack)