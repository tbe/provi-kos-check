include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(pdb)

add_library(pkosNetwork network.cpp)
target_link_libraries(pkosNetwork ${QT_QTNETWORK_LIBRARY})

add_library(pkosEveAPI eveapixmlhandler.cpp eveapi.cpp)
target_link_libraries(pkosEveAPI pkosNetwork ${QT_QTXML_LIBRARY})

add_library(pkosCvaAPI cvakosapi.cpp)
target_link_libraries(pkosCvaAPI pkosNetwork ${QT_QTSCRIPT_LIBRARY})

add_library(pkosLogWatcher logwatcher.cpp)
target_link_libraries(pkosLogWatcher ${QT_QTCORE_LIBRARY})

add_library(pkosAudio audio.cpp)
target_link_libraries(pkosAudio ${QT_QTCORE_LIBRARY} ${PHONON_LIBRARY_})

add_library(pkosCacheDB cachedb.cpp)
target_link_libraries(pkosCacheDB ${QT_QTCORE_LIBRARY} ${QT_QTSQL_LIBRARY})

add_library(pkosHtDB hotdropperdb.cpp)
target_link_libraries(pkosHtDB ${QT_QTCORE_LIBRARY} ${QT_QTSQL_LIBRARY} pdbClient)
