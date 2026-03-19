#include <QApplication>
#include <QDebug>
#include <QDir>

#include "mainwindow.h"
#include "databasemanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("CalendarTodo");
    app.setApplicationVersion("1.0.0");
    app.setQuitOnLastWindowClosed(false);

    if (!DatabaseManager::instance().initDatabase()) {
        qCritical("Failed to initialize database!");
        return 1;
    }

    MainWindow window;
    window.show();

    return app.exec();
}
