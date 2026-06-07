cmake_minimum_required(VERSION 3.16)
project(SmartMedicineCabinet VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ── Qt6 模块 ──────────────────────────────────
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Quick
    Qml
    Sql
    Network
    Widgets        # 某些平台需要
)

qt_add_executable(SmartMedicineCabinet
    main.cpp
    src/mqttclient.h       src/mqttclient.cpp
    src/databasemanager.h  src/databasemanager.cpp
    src/appstate.h         src/appstate.cpp
    src/theme.h            src/theme.cpp
    resources.qrc
    # ── All QML files explicitly listed for qmlcachegen type registration ──
    main.qml
    qml/pages/DashboardPage.qml
    qml/pages/LoginPage.qml
    qml/pages/HomePage.qml
    qml/pages/DrugShelfPage.qml
    qml/pages/DrugDetailPage.qml
    qml/pages/CartPage.qml
    qml/pages/OrderPage.qml
    qml/pages/ProfilePage.qml
    qml/pages/PersonalInfoPage.qml
    qml/pages/HealthMonitorPage.qml
    qml/pages/HealthNewsPage.qml
    qml/pages/NewsDetailPage.qml
    qml/components/DrugCard.qml
    qml/components/CartItem.qml
    qml/components/OrderItem.qml
)

target_link_libraries(SmartMedicineCabinet PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
    Qt6::Sql
    Qt6::Network
)

# ── QML 导入路径 ──────────────────────────────
target_include_directories(SmartMedicineCabinet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ── Android 部署 ──────────────────────────────
if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android" CACHE INTERNAL "")

    # ── OpenSSL for TLS (bundle prebuilt .so from KDAB) ──
    include(FetchContent)
    FetchContent_Declare(
        android_openssl
        DOWNLOAD_EXTRACT_TIMESTAMP true
        URL https://gitee.com/mirrors_KDAB/android_openssl/repository/archive/master.zip
    )
    FetchContent_MakeAvailable(android_openssl)
    include(${android_openssl_SOURCE_DIR}/android_openssl.cmake)
    add_android_openssl_libraries(SmartMedicineCabinet)

    set_target_properties(SmartMedicineCabinet PROPERTIES
        QT_ANDROID_APPLICATION_ARGUMENTS ""
    )
endif()

# ── 安装规则 ──────────────────────────────────
install(TARGETS SmartMedicineCabinet
    BUNDLE  DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
