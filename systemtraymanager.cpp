#include "systemtraymanager.h"

#include <QAction>
#include <QIcon>
#include <QWidget>

SystemTrayManager::SystemTrayManager(QObject *parent)
    : QObject(parent)
    , m_trayIcon(nullptr)
    , m_contextMenu(nullptr)
    , m_mainWindow(nullptr)
{
}

SystemTrayManager::~SystemTrayManager() {
    if (m_trayIcon) {
        delete m_trayIcon;
    }
    if (m_contextMenu) {
        delete m_contextMenu;
    }
}

void SystemTrayManager::setMainWindow(QWidget *window) {
    m_mainWindow = window;
}

void SystemTrayManager::setupTrayIcon() {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning("System tray is not available on this system");
        return;
    }

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/resources/icons/app.png"));
    m_trayIcon->setToolTip("日历清单");

    createContextMenu();
    m_trayIcon->setContextMenu(m_contextMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &SystemTrayManager::onTrayIconActivated);

    m_trayIcon->show();
}

bool SystemTrayManager::isTrayIconSupported() const {
    return QSystemTrayIcon::isSystemTrayAvailable();
}

void SystemTrayManager::createContextMenu() {
    m_contextMenu = new QMenu();

    QAction *showAction = new QAction("显示主窗口", this);
    connect(showAction, &QAction::triggered, this, &SystemTrayManager::onShowWindow);
    m_contextMenu->addAction(showAction);

    m_contextMenu->addSeparator();

    QAction *quitAction = new QAction("退出", this);
    connect(quitAction, &QAction::triggered, this, &SystemTrayManager::onQuit);
    m_contextMenu->addAction(quitAction);
}

void SystemTrayManager::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::DoubleClick) {
        onShowWindow();
    }
}

void SystemTrayManager::onShowWindow() {
    if (m_mainWindow) {
        m_mainWindow->show();
        m_mainWindow->activateWindow();
        m_mainWindow->raise();
    }
    emit showRequested();
}

void SystemTrayManager::onQuit() {
    emit quitRequested();
}
