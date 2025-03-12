cmake_minimum_required(VERSION 3.5)

project(qt_akp_reg LANGUAGES CXX VERSION 0.0.1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QMAKE_CXXFLAGS ${QMAKE_CXXFLAGS} -Ofast -march=x86-64 -std=c++11 -minline-all-stringops -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2)

configure_file(app_config.h.in "${qt_akp_reg_SOURCE_DIR}/app_config.h")
configure_file(resources.rc.in "${qt_akp_reg_SOURCE_DIR}/resources.rc")

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt5 COMPONENTS Widgets Gui Core Network LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Gui Core Network LinguistTools REQUIRED)


message("==CMAKE_SOURCE_DIR======>" ${CMAKE_SOURCE_DIR})
message("==CMAKE_BINARY_DIR======>" ${CMAKE_BINARY_DIR})
message("==CMAKE_INSTALL_PREFIX==>" ${CMAKE_INSTALL_PREFIX})
message("==CMAKE_PREFIX_PATH=====>" ${CMAKE_PREFIX_PATH})
message("==QT_QMAKE_EXECUTABLE===>" ${QT_QMAKE_EXECUTABLE})
message("==QT_VERSION_MAJOR======>" ${QT_VERSION_MAJOR})

#find_file("TNG.ico" NAMES .. REQUIRED)

set(SOURCES
    main.cpp
    mainwindow.cpp
    dialog_well_info.cpp
    dialog_setup.cpp
    vak_8_2pc.cpp
    akp_class.cpp
    akp_check_state.cpp
    qt_bigreed.cpp
    qt_vk.cpp
    qt_time_line.cpp
    qt_vak_32_fkd.cpp
    qt_deptcol.cpp
    qt_ml.cpp
    velocity.cpp
    qt_akp_file.cpp
)

set(HEADERS
    mainwindow.h
    dialog_well_info.h
    dialog_setup.h
    app_config.h
    tool_type.h
    vak_8.h
    vak_8_2pc.h
    akp_class.h
    akp_check_state.h
    qt_bigreed.h
    qt_vk.h
    qt_time_line.h
    qt_vak_32_fkd.h
    qt_deptcol.h
    qt_ml.h
    velocity.h
    qt_akp_file.h
)

set(UI mainwindow.ui
       dialog_well_info.ui
       dialog_setup.ui
)

set(RESOURCE_FILES
    app_resources.qrc
    resources.rc)

set(TS_FILES qt_akp_reg_ru_RU.ts)

if(ANDROID)
  add_library(qt_akp_reg SHARED
    ${SOURCES}
    ${HEADERS}
    ${UI}
    ${RESOURCE_FILES}
    ${TS_FILES}
  )
else()
  add_executable(qt_akp_reg
    ${SOURCES}
    ${HEADERS}
    ${UI}
    ${RESOURCE_FILES}
    ${TS_FILES}
  )
endif()

if (WIN32)
    find_library(SOCKET_LIB libwsock32 ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})
    target_link_libraries(qt_akp_reg
        PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
        PRIVATE Qt${QT_VERSION_MAJOR}::Gui
        PRIVATE Qt${QT_VERSION_MAJOR}::Core
        PRIVATE Qt${QT_VERSION_MAJOR}::Network
        ${SOCKET_LIB}
        )
    message("==CMAKE_CXX_COMPILER==========>" ${CMAKE_CXX_COMPILER})
    message("============================================")
#    message("===> "${SOCKET_LIB})
#    message("============================================")
else()
    target_link_libraries(qt_akp_reg 
        PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
        PRIVATE Qt${QT_VERSION_MAJOR}::Gui
        PRIVATE Qt${QT_VERSION_MAJOR}::Core
        PRIVATE Qt${QT_VERSION_MAJOR}::Network
        )
endif()

GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

#set_target_properties(
#    qt_akp_reg
#    PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
#    )

install(TARGETS qt_akp_reg CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
#install(FILES Qt${QT_VERSION_MAJOR}::Widgets DESTINATION ..)
#install(FILES Qt${QT_VERSION_MAJOR}::Gui     DESTINATION ..)
#install(FILES Qt${QT_VERSION_MAJOR}::Core    DESTINATION ..)
#install(FILES Qt${QT_VERSION_MAJOR}::Network DESTINATION ..)

install(FILES ${QT_DLL_PATH_tmp}/Qt${QT_VERSION_MAJOR}Widgetsd.dll CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
install(FILES ${QT_DLL_PATH_tmp}/Qt${QT_VERSION_MAJOR}Guid.dll     CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
install(FILES ${QT_DLL_PATH_tmp}/Qt${QT_VERSION_MAJOR}Cored.dll    CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
install(FILES ${QT_DLL_PATH_tmp}/Qt${QT_VERSION_MAJOR}Testd.dll    CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
install(FILES ${QT_DLL_PATH_tmp}/Qt${QT_VERSION_MAJOR}Networkd.dll CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
#install(FILES ${CMAKE_SOURCE_DIR}/images/TNG.ico                   CONFIGURATIONS Debug RUNTIME DESTINATION ${CMAKE_BINARY_DIR})

install(TARGETS qt_akp_reg CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
install(FILES ${QT_DLL_PATH_tmp}/Qt${QT_VERSION_MAJOR}Widgets.dll  CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
install(FILES ${QT_DLL_PATH_tmp}/Qt${QT_VERSION_MAJOR}Gui.dll      CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
install(FILES ${QT_DLL_PATH_tmp}/Qt${QT_VERSION_MAJOR}Core.dll     CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
install(FILES ${QT_DLL_PATH_tmp}/Qt${QT_VERSION_MAJOR}Test.dll     CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
install(FILES ${QT_DLL_PATH_tmp}/Qt${QT_VERSION_MAJOR}Network.dll  CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_BINARY_DIR})
#install(FILES ${CMAKE_SOURCE_DIR}/images/TNG.ico                   CONFIGURATIONS Release RUNTIME DESTINATION ${CMAKE_BINARY_DIR})

add_custom_command(TARGET qt_akp_reg POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Widgets> $<TARGET_FILE_DIR:qt_akp_reg>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Gui>     $<TARGET_FILE_DIR:qt_akp_reg>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Core>    $<TARGET_FILE_DIR:qt_akp_reg>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Network> $<TARGET_FILE_DIR:qt_akp_reg>
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}images/TNG.ico             $<TARGET_FILE_DIR:qt_akp_reg>
        )
