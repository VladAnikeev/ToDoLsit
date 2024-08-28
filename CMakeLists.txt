cmake_minimum_required(VERSION 3.20)

project(QT6ToDoList VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

# Укажите версию C++
set(CMAKE_CXX_STANDARD 17)

# Укажите директорию, в которую будут собираться бинарные файлы
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

qt_standard_project_setup()

qt_add_executable(appQT6ToDoList
    main.cpp
    todolist.cpp
    todomodel.cpp
    todolist.h
    todomodel.h
)

qt_add_qml_module(appQT6ToDoList
    URI QT6ToDoList
    VERSION 1.0
    QML_FILES
        Main.qml
        TodoList.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appQT6ToDoList PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appQT6ToDoList
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appQT6ToDoList
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appQT6ToDoList
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
