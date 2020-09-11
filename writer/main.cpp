#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

#include "listmodel.h"

static void registerTypes();

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    registerTypes();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

static void registerTypes() {
    qmlRegisterType<ListModel>("App.Type",1,0,"ListModel");
    // qmlRegisterType<ListElement>("App.Type",1,0, "ListElement");
}
