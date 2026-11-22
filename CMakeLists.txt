
cmake_minimum_required(VERSION 3.21)

project(SafeRoute VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(SafeRoute
    main.cpp

    bfs.h
    bfs.cpp

    custom_queue.h
    custom_queue.cpp

    mainwindow.h
    mainwindow.cpp

    mapwidget.h
    mapwidget.cpp

    graph.h
    graph.cpp

    dijkstra.h
    dijkstra.cpp

    min_heap.h
    min_heap.cpp

    max_heap.h
    max_heap.cpp
)

target_link_libraries(SafeRoute PRIVATE Qt6::Widgets)
