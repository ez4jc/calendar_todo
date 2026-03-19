#ifndef SYSTEMTRAYMANAGER_H
#define SYSTEMTRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>

class QWidget;

class SystemTrayManager : public QObject {
    Q_OBJECT

public:
    explicit SystemTrayManager(QObject *parent = nullptr);
    ~SystemTrayManager();

    void setMainWindow(QWidget *window);
    void setupTrayIcon();
    bool isTrayIconSupported() const;

signals:
    void showRequested();
    void quitRequested();

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowWindow();
    void onQuit();

private:
    void createContextMenu();

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_contextMenu;
    QWidget *m_mainWindow;
};

#endif // SYSTEMTRAYMANAGER_H
