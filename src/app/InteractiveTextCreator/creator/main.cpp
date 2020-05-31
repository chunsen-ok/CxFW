#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDir>
#include <QDebug>

#include <itcore/localstorage.h>

void onAppQuit() {
    LocalStorage::drop();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QObject::connect(&app,&QGuiApplication::aboutToQuit,onAppQuit);

    LocalStorage::self().loadData("SELECT * FROM story;");

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
