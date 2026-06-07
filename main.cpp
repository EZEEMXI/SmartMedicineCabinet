#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "appstate.h"
#include "src/theme.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("智能药柜");
    app.setOrganizationName("SmartCabinet");

    // 创建全局状态单例
    AppState appState;
    appState.init();

    Theme theme;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("App", &appState);
    engine.rootContext()->setContextProperty("Theme", &theme);

    const QUrl url("qrc:/main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    engine.load(url);
    return app.exec();
}
