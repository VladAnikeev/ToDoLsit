#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext> // To expose the object to QML
#include <QFileInfo>

#include "todolist.h"
#include "todomodel.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    qmlRegisterType<TodoModel>("Todo", 1, 0, "TodoModel");
    qmlRegisterUncreatableType<TodoList>("Todo", 1, 0, "TodoList", QStringLiteral("TodoList should not be created in QML"));

    TodoList todoList;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty(QStringLiteral("todoList"), &todoList);
    const QUrl url(QStringLiteral("qrc:/QT6ToDoList/Main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl)
                     {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.load(url);

    todoList.load();

    return app.exec();
}
