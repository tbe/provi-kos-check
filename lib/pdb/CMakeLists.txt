include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(pdbClient client.cpp)
target_link_libraries(pdbClient ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY})