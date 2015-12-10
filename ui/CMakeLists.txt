include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(pkosListEntryCompact koslistentrycompact.cpp)
target_link_libraries(pkosListEntryCompact ${QT_QTGUI_LIBRARY})

add_library(pkosListSeperator koslistseperator.cpp)
target_link_libraries(pkosListSeperator ${QT_QTGUI_LIBRARY})

add_library(pkosAboutBox aboutbox.cpp)
target_link_libraries(pkosAboutBox ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})

add_library(pkosConfigDialog configdialog.cpp)
target_link_libraries(pkosConfigDialog pkosAudio ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${PHONON_LIBRARY_})

add_library(pkosMainWindow mainwindow.cpp)
target_link_libraries(pkosMainWindow pkosListEntryCompact pkosListSeperator pkosAboutBox pkosConfigDialog ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})