cmake_minimum_required(VERSION 3.26)
project(CPP_Project4_0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.14.2/5.14.2/mingw73_64")

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(CPP_Project4_0 main.cpp
        Class_ATM/ATM.cpp
        Class_ATM/ATM.h
        Class_Card/Card.cpp
        Class_Card/Card.h
        Class_QT/one_class/one_class.cpp
        Class_QT/one_class/one_class.hpp
        Class_QT/one_class/one_class.ui
        Class_QT/two_class/two_class.cpp
        Class_QT/two_class/two_class.hpp
        Class_QT/two_class/two_class.ui
        Class_QT/one_class/picture.qrc
        Class_QT/two_class/picture.qrc
        Class_QT/three_class/picture.qrc
        Class_QT/three_class/three_class.cpp
        Class_QT/three_class/three_class.hpp
        Class_QT/three_class/three_class.ui
        Class_Dos/DOS.cpp
        Class_Dos/DOS.h
        Class_QT/enquiry_class/enquiry.cpp
        Class_QT/enquiry_class/enquiry.h
        Class_QT/enquiry_class/enquiry.ui
        Class_QT/change_class/change.cpp
        Class_QT/change_class/change.h
        Class_QT/change_class/change.ui
        Class_QT/logan_class/logan.cpp
        Class_QT/logan_class/logan.h
        Class_QT/logan_class/logan.ui
        Class_QT/menu_class/widget.cpp
        Class_QT/menu_class/widget.h
        Class_QT/menu_class/widget.ui
        Class_QT/recharge_class/recharge.cpp
        Class_QT/recharge_class/recharge.h
        Class_QT/recharge_class/recharge.ui
        Class_QT/withdrawal_class/withdrawal.cpp
        Class_QT/withdrawal_class/withdrawal.h
        Class_QT/withdrawal_class/withdrawal.ui
        Class_QT/warning_class/warning.cpp
        Class_QT/warning_class/warning.h
        Class_QT/warning_class/warning.ui
        Class_QT/registration_class/registration.cpp
        Class_QT/registration_class/registration.h
        Class_QT/registration_class/registration.ui
        Class_QT/csv/csv.cpp
        Class_QT/csv/csv.h
        Class_QT/csv/csv.ui
)
target_link_libraries(CPP_Project4_0
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
