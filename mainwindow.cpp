#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QFrame>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QEvent>
#include <QGuiApplication>
#include <QHideEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QStyle>
#include <QTimer>
#include <QWidget>
#include <QDesktopWidget>
#include <QApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "databasemanager.h"

#ifdef Q_OS_LINUX
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <cstring>
#endif

namespace {
#ifdef Q_OS_LINUX
const char kShowDesktopShortcutId[] = "show-desktop";
const int kShowDesktopShortcutType = 3;
const char kShowDesktopSavedAccelsKey[] = "show_desktop_saved_accels";
const char kShowDesktopManagedKey[] = "show_desktop_shortcut_managed";

QStringList parseShortcutAccels(const QString &shortcutJson) {
    const QJsonDocument document = QJsonDocument::fromJson(shortcutJson.trimmed().toUtf8());
    if (!document.isObject()) {
        return QStringList();
    }

    const QJsonValue accelsValue = document.object().value("Accels");
    if (!accelsValue.isArray()) {
        return QStringList();
    }

    QStringList accels;
    const QJsonArray accelsArray = accelsValue.toArray();
    for (const QJsonValue &value : accelsArray) {
        if (value.isString()) {
            accels.append(value.toString());
        }
    }
    return accels;
}

QStringList currentShowDesktopShortcutAccels() {
    QDBusInterface keybinding(
        "com.deepin.daemon.Keybinding",
        "/com/deepin/daemon/Keybinding",
        "com.deepin.daemon.Keybinding",
        QDBusConnection::sessionBus()
    );
    if (!keybinding.isValid()) {
        return QStringList();
    }

    QDBusReply<QString> reply = keybinding.call("Query", kShowDesktopShortcutId, kShowDesktopShortcutType);
    if (!reply.isValid()) {
        return QStringList();
    }

    return parseShortcutAccels(reply.value());
}

QStringList loadSavedShowDesktopAccels() {
    const QString saved = DatabaseManager::instance().getSetting(kShowDesktopSavedAccelsKey, "");
    if (saved.isEmpty()) {
        return QStringList();
    }

    const QJsonDocument document = QJsonDocument::fromJson(saved.toUtf8());
    if (!document.isArray()) {
        return QStringList();
    }

    QStringList accels;
    const QJsonArray array = document.array();
    for (const QJsonValue &value : array) {
        if (value.isString()) {
            accels.append(value.toString());
        }
    }
    return accels;
}

void saveSavedShowDesktopAccels(const QStringList &accels) {
    QJsonArray array;
    for (const QString &accel : accels) {
        array.append(accel);
    }

    DatabaseManager::instance().setSetting(
        kShowDesktopSavedAccelsKey,
        QString::fromUtf8(QJsonDocument(array).toJson(QJsonDocument::Compact))
    );
}

bool setShowDesktopShortcutEnabled(bool enabled, const QStringList &restoreAccels = QStringList()) {
    QDBusInterface keybinding(
        "com.deepin.daemon.Keybinding",
        "/com/deepin/daemon/Keybinding",
        "com.deepin.daemon.Keybinding",
        QDBusConnection::sessionBus()
    );
    if (!keybinding.isValid()) {
        return false;
    }

    if (!enabled) {
        QDBusReply<void> reply = keybinding.call("Disable", kShowDesktopShortcutId, kShowDesktopShortcutType);
        return reply.isValid();
    }

    const QStringList accels = restoreAccels.isEmpty() ? (QStringList() << "<Super>D") : restoreAccels;
    keybinding.call("ClearShortcutKeystrokes", kShowDesktopShortcutId, kShowDesktopShortcutType);

    bool success = true;
    for (const QString &accel : accels) {
        QDBusReply<bool> reply = keybinding.call(
            "ModifiedAccel",
            kShowDesktopShortcutId,
            kShowDesktopShortcutType,
            accel,
            true
        );
        success = success && reply.isValid() && reply.value();
    }

    return success;
}

bool queryShowDesktopState() {
    QDBusInterface deepinWm(
        "com.deepin.wm",
        "/com/deepin/wm",
        "com.deepin.wm",
        QDBusConnection::sessionBus()
    );
    if (deepinWm.isValid()) {
        QDBusReply<bool> reply = deepinWm.call("GetIsShowDesktop");
        if (reply.isValid()) {
            return reply.value();
        }
    }

    QDBusInterface kwinProperties(
        "org.kde.KWin",
        "/KWin",
        "org.freedesktop.DBus.Properties",
        QDBusConnection::sessionBus()
    );
    if (kwinProperties.isValid()) {
        QDBusReply<QVariant> reply = kwinProperties.call("Get", "org.kde.KWin", "showingDesktop");
        if (reply.isValid()) {
            return reply.value().toBool();
        }
    }

    return false;
}

void sendNetWmState(Display *display, Window window, long action, Atom first, Atom second = None) {
    if (!display || !window || first == None) {
        return;
    }

    XEvent event;
    std::memset(&event, 0, sizeof(event));
    event.xclient.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = XInternAtom(display, "_NET_WM_STATE", False);
    event.xclient.format = 32;
    event.xclient.data.l[0] = action;
    event.xclient.data.l[1] = first;
    event.xclient.data.l[2] = second;
    event.xclient.data.l[3] = 1;
    event.xclient.data.l[4] = 0;

    XSendEvent(display, DefaultRootWindow(display), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &event);
}
#endif
}

const qreal MainWindow::DEFAULT_OPACITY = 0.4;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_calendarView(nullptr)
    , m_rootWidget(nullptr)
    , m_titleBar(nullptr)
    , m_monthTitle(nullptr)
    , m_prevButton(nullptr)
    , m_nextButton(nullptr)
    , m_desktopPersistenceButton(nullptr)
    , m_settingsButton(nullptr)
    , m_closeButton(nullptr)
    , m_opacitySlider(nullptr)
    , m_opacityLabel(nullptr)
    , m_sizeGrip(nullptr)
    , m_desktopEnforcerTimer(nullptr)
    , m_trayManager(nullptr)
    , m_isDragging(false)
    , m_isResizing(false)
    , m_allowProgrammaticHide(false)
    , m_persistOnShowDesktop(true)
    , m_hasSavedGeometry(false)
    , m_resizeRegion(ResizeNone)
    , m_currentOpacity(DEFAULT_OPACITY)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
#ifdef Q_OS_LINUX
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
#endif
    setMouseTracking(true);
    setMinimumSize(640, 420);

    setupUI();

    m_trayManager = new SystemTrayManager(this);
    m_trayManager->setMainWindow(this);
    m_trayManager->setupTrayIcon();

    connect(m_trayManager, &SystemTrayManager::showRequested,
            this, &MainWindow::onTrayShowRequested);
    connect(m_trayManager, &SystemTrayManager::quitRequested,
            this, &MainWindow::onTrayQuitRequested);

    m_desktopEnforcerTimer = new QTimer(this);
    m_desktopEnforcerTimer->setInterval(150);
    connect(m_desktopEnforcerTimer, &QTimer::timeout,
            this, &MainWindow::enforceDesktopVisibility);
    m_desktopEnforcerTimer->start();

    loadSettings();
    applyDefaultGeometry();
}

MainWindow::~MainWindow() {
    restoreManagedShowDesktopShortcut();
}

void MainWindow::setupUI() {
    m_rootWidget = new QWidget(this);
    m_rootWidget->setObjectName("centralWidget");
    m_rootWidget->setMouseTracking(true);
    m_rootWidget->installEventFilter(this);
    setCentralWidget(m_rootWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_rootWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);

    m_titleBar = new QWidget(m_rootWidget);
    m_titleBar->setFixedHeight(40);
    m_titleBar->setCursor(Qt::OpenHandCursor);
    m_titleBar->installEventFilter(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(5, 0, 5, 0);

    m_prevButton = new QPushButton("◀", m_titleBar);
    m_prevButton->setFixedSize(40, 30);
    m_prevButton->setCursor(Qt::PointingHandCursor);
    connect(m_prevButton, &QPushButton::clicked, this, &MainWindow::onPrevMonth);
    titleLayout->addWidget(m_prevButton);

    m_monthTitle = new QLabel(m_titleBar);
    m_monthTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_monthTitle->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    titleLayout->addWidget(m_monthTitle);

    m_nextButton = new QPushButton("▶", m_titleBar);
    m_nextButton->setFixedSize(40, 30);
    m_nextButton->setCursor(Qt::PointingHandCursor);
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::onNextMonth);
    titleLayout->addWidget(m_nextButton);

    titleLayout->addStretch();

    m_opacityLabel = new QLabel("透明度: 40%", m_titleBar);
    m_opacityLabel->setStyleSheet("color: white; font-size: 12px;");
    titleLayout->addWidget(m_opacityLabel);

    m_opacitySlider = new QSlider(Qt::Horizontal, m_titleBar);
    m_opacitySlider->setMinimum(10);
    m_opacitySlider->setMaximum(100);
    m_opacitySlider->setValue(40);
    m_opacitySlider->setFixedWidth(100);
    connect(m_opacitySlider, &QSlider::valueChanged, this, &MainWindow::onOpacityChanged);
    titleLayout->addWidget(m_opacitySlider);

    m_desktopPersistenceButton = new QPushButton(m_titleBar);
    m_desktopPersistenceButton->setCheckable(true);
    m_desktopPersistenceButton->setFixedSize(74, 30);
    m_desktopPersistenceButton->setCursor(Qt::PointingHandCursor);
    connect(m_desktopPersistenceButton, &QPushButton::toggled,
            this, &MainWindow::onDesktopPersistenceToggled);
    titleLayout->addWidget(m_desktopPersistenceButton);

    m_settingsButton = new QPushButton("⚙", m_titleBar);
    m_settingsButton->setFixedSize(30, 30);
    m_settingsButton->setCursor(Qt::PointingHandCursor);
    connect(m_settingsButton, &QPushButton::clicked, this, &MainWindow::onShowSettings);
    titleLayout->addWidget(m_settingsButton);

    m_closeButton = new QPushButton("×", m_titleBar);
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setCursor(Qt::PointingHandCursor);
    connect(m_closeButton, &QPushButton::clicked, this, &QWidget::close);
    titleLayout->addWidget(m_closeButton);

    mainLayout->addWidget(m_titleBar);

    m_calendarView = new CalendarView(m_rootWidget);
    mainLayout->addWidget(m_calendarView);

    QHBoxLayout *footerLayout = new QHBoxLayout();
    footerLayout->setContentsMargins(0, 0, 0, 0);
    footerLayout->addStretch();

    m_sizeGrip = new QSizeGrip(m_rootWidget);
    m_sizeGrip->setToolTip("拖动这里调整窗口大小");
    m_sizeGrip->installEventFilter(this);
    footerLayout->addWidget(m_sizeGrip, 0, Qt::AlignRight | Qt::AlignBottom);
    mainLayout->addLayout(footerLayout);

    connect(m_calendarView, &CalendarView::dateDoubleClicked,
            this, &MainWindow::onDateDoubleClicked);

    applyWindowStyle();
    updateDesktopPersistenceButton();
    updateMonthTitle();
    updateOpacityLabel();
}

void MainWindow::applyDefaultGeometry() {
    QDesktopWidget *desktop = QApplication::desktop();
    QRect available = desktop->availableGeometry(this);
    if (!available.isValid()) {
        available = desktop->screenGeometry(this);
    }

    const int width = qMax(minimumWidth(), available.width() * 3 / 4);
    const int height = qMax(minimumHeight(), available.height());
    const int x = available.right() - width + 1;
    const int y = available.y();
    setGeometry(x, y, width, height);
}

void MainWindow::markGeometryCustomized() {
    DatabaseManager::instance().setSetting("geometry_customized", "1");
}

void MainWindow::applyWindowStyle() {
    const int panelAlpha = qBound(35, static_cast<int>(m_currentOpacity * 220.0), 235);
    const int buttonAlpha = qBound(45, static_cast<int>(m_currentOpacity * 195.0), 225);
    const int buttonHoverAlpha = qBound(55, buttonAlpha + 20, 245);
    const int buttonPressedAlpha = qBound(40, buttonAlpha - 15, 210);
    const int grooveAlpha = qBound(35, panelAlpha - 10, 220);

    setWindowOpacity(1.0);

    m_rootWidget->setStyleSheet(QString(
        "QWidget#centralWidget { "
        "   background-color: rgba(30, 30, 40, %1); "
        "   border-radius: 8px; "
        "   border: 1px solid rgba(100, 100, 100, 90); "
        "}"
        "QPushButton { "
        "   background-color: rgba(60, 60, 70, %2); "
        "   color: white; "
        "   border: none; "
        "   border-radius: 4px; "
        "   font-size: 14px; "
        "}"
        "QPushButton:hover { "
        "   background-color: rgba(80, 80, 90, %3); "
        "}"
        "QPushButton:pressed { "
        "   background-color: rgba(50, 50, 60, %4); "
        "}"
        "QPushButton:checked { "
        "   background-color: rgba(85, 138, 106, 215); "
        "   color: white; "
        "}"
        "QPushButton:checked:hover { "
        "   background-color: rgba(102, 156, 122, 228); "
        "}"
        "QSlider::groove:horizontal { "
        "   border: 1px solid rgba(100, 100, 100, 128); "
        "   height: 4px; "
        "   background: rgba(80, 80, 80, %5); "
        "   border-radius: 2px; "
        "}"
        "QSlider::handle:horizontal { "
        "   background: rgba(100, 150, 200, 230); "
        "   width: 14px; "
        "   margin: -5px 0; "
        "   border-radius: 7px; "
        "}"
    ).arg(panelAlpha)
     .arg(buttonAlpha)
     .arg(buttonHoverAlpha)
     .arg(buttonPressedAlpha)
     .arg(grooveAlpha));
}

void MainWindow::updateMonthTitle() {
    int year = m_calendarView->currentYear();
    int month = m_calendarView->currentMonth();
    m_monthTitle->setText(QString("%1年%2月").arg(year).arg(month));
}

void MainWindow::updateOpacityLabel() {
    int value = static_cast<int>(m_currentOpacity * 100);
    m_opacityLabel->setText(QString("透明度: %1%").arg(value));
}

void MainWindow::updateDesktopPersistenceButton() {
    if (!m_desktopPersistenceButton) {
        return;
    }

    m_desktopPersistenceButton->blockSignals(true);
    m_desktopPersistenceButton->setChecked(m_persistOnShowDesktop);
    m_desktopPersistenceButton->setText(m_persistOnShowDesktop ? "常驻" : "跟随");
    m_desktopPersistenceButton->setToolTip(
        m_persistOnShowDesktop
            ? "开启后会禁用系统 Win+D，避免当前窗口被显示桌面动作隐藏"
            : "关闭后恢复系统 Win+D，窗口会跟随显示桌面一起隐藏");
    m_desktopPersistenceButton->blockSignals(false);
}

void MainWindow::onPrevMonth() {
    int year = m_calendarView->currentYear();
    int month = m_calendarView->currentMonth() - 1;

    if (month < 1) {
        month = 12;
        year -= 1;
    }

    m_calendarView->setCurrentMonth(year, month);
    updateMonthTitle();
}

void MainWindow::onNextMonth() {
    int year = m_calendarView->currentYear();
    int month = m_calendarView->currentMonth() + 1;

    if (month > 12) {
        month = 1;
        year += 1;
    }

    m_calendarView->setCurrentMonth(year, month);
    updateMonthTitle();
}

void MainWindow::onDateDoubleClicked(const QDate &date) {
    m_calendarView->editDate(date);
}

void MainWindow::onDesktopPersistenceToggled(bool checked) {
    m_persistOnShowDesktop = checked;
    syncShowDesktopShortcutState();
    updateDesktopPersistenceButton();
    saveSettings();

    if (m_persistOnShowDesktop) {
        enforceDesktopVisibility();
    }
}

void MainWindow::onOpacityChanged(int value) {
    m_currentOpacity = value / 100.0;
    applyWindowStyle();
    updateOpacityLabel();
    saveSettings();
}

void MainWindow::onShowSettings() {
}

void MainWindow::onTrayShowRequested() {
    if (m_desktopEnforcerTimer) {
        m_desktopEnforcerTimer->start();
    }
    show();
    showNormal();
    enforceDesktopVisibility();
}

void MainWindow::onTrayQuitRequested() {
    if (m_desktopEnforcerTimer) {
        m_desktopEnforcerTimer->stop();
    }
    restoreManagedShowDesktopShortcut();
    saveSettings();
    qApp->quit();
}

void MainWindow::loadSettings() {
    QString opacityStr = DatabaseManager::instance().getSetting("opacity", "0.4");
    bool ok;
    qreal opacity = QString(opacityStr).toDouble(&ok);
    if (ok && opacity >= 0.1 && opacity <= 1.0) {
        m_currentOpacity = opacity;
        m_opacitySlider->setValue(static_cast<int>(opacity * 100));
        applyWindowStyle();
        updateOpacityLabel();
    }

    m_persistOnShowDesktop = DatabaseManager::instance().getSetting("persist_on_show_desktop", "1") != "0";
    syncShowDesktopShortcutState();
    updateDesktopPersistenceButton();
    m_hasSavedGeometry = false;
}

void MainWindow::saveSettings() {
    DatabaseManager::instance().setSetting("opacity", QString::number(m_currentOpacity));
    DatabaseManager::instance().setSetting("persist_on_show_desktop",
                                           m_persistOnShowDesktop ? "1" : "0");

    QRect geo = geometry();
    QString geometryStr = QString("%1,%2,%3,%4")
        .arg(geo.x())
        .arg(geo.y())
        .arg(geo.width())
        .arg(geo.height());
    DatabaseManager::instance().setSetting("geometry", geometryStr);
}

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange && isMinimized() && m_persistOnShowDesktop) {
        setWindowState(windowState() & ~Qt::WindowMinimized);
        showNormal();
        lowerToDesktopLayer();
        event->accept();
        return;
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::syncShowDesktopShortcutState() {
#ifdef Q_OS_LINUX
    const bool managed = DatabaseManager::instance().getSetting(kShowDesktopManagedKey, "0") == "1";

    if (m_persistOnShowDesktop) {
        QStringList savedAccels = loadSavedShowDesktopAccels();
        if (savedAccels.isEmpty()) {
            savedAccels = currentShowDesktopShortcutAccels();
        }
        if (savedAccels.isEmpty()) {
            savedAccels << "<Super>D";
        }

        saveSavedShowDesktopAccels(savedAccels);
        DatabaseManager::instance().setSetting(kShowDesktopManagedKey, "1");
        setShowDesktopShortcutEnabled(false);
        return;
    }

    if (managed) {
        restoreManagedShowDesktopShortcut();
    }
#endif
}

void MainWindow::restoreManagedShowDesktopShortcut() {
#ifdef Q_OS_LINUX
    const bool managed = DatabaseManager::instance().getSetting(kShowDesktopManagedKey, "0") == "1";
    if (!managed) {
        return;
    }

    QStringList savedAccels = loadSavedShowDesktopAccels();
    if (savedAccels.isEmpty()) {
        savedAccels << "<Super>D";
    }

    setShowDesktopShortcutEnabled(true, savedAccels);
    DatabaseManager::instance().setSetting(kShowDesktopManagedKey, "0");
#endif
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveSettings();
    if (m_desktopEnforcerTimer) {
        m_desktopEnforcerTimer->stop();
    }
    m_allowProgrammaticHide = true;
    hide();
    m_allowProgrammaticHide = false;
    event->ignore();
}

void MainWindow::hideEvent(QHideEvent *event) {
    QMainWindow::hideEvent(event);

    if (m_persistOnShowDesktop && !m_allowProgrammaticHide && !qApp->closingDown()) {
        QTimer::singleShot(0, this, [this]() {
            if (!m_persistOnShowDesktop || m_allowProgrammaticHide || qApp->closingDown()) {
                return;
            }

            show();
            showNormal();
            lowerToDesktopLayer();
        });
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_rootWidget) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                const ResizeRegion region = hitTestResizeRegion(mouseEvent->pos());
                if (region != ResizeNone) {
                    m_isResizing = true;
                    m_resizeRegion = region;
                    m_resizeStartGlobalPos = mouseEvent->globalPos();
                    m_resizeStartGeometry = geometry();
                    return true;
                }
            }
        } else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (m_isResizing && (mouseEvent->buttons() & Qt::LeftButton)) {
                resizeFromGlobalPos(mouseEvent->globalPos());
                return true;
            }

            if (!m_isDragging) {
                updateCursorForRegion(hitTestResizeRegion(mouseEvent->pos()));
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton && m_isResizing) {
                m_isResizing = false;
                m_resizeRegion = ResizeNone;
                updateCursorForRegion(hitTestResizeRegion(mouseEvent->pos()));
                saveSettings();
                return true;
            }
        } else if (event->type() == QEvent::Leave) {
            if (!m_isDragging && !m_isResizing) {
                m_rootWidget->unsetCursor();
            }
        }
    } else if (watched == m_sizeGrip) {
        if (event->type() == QEvent::MouseButtonRelease) {
            markGeometryCustomized();
            saveSettings();
        }
    } else if (watched == m_titleBar) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            const QPoint rootPos = m_rootWidget->mapFromGlobal(mouseEvent->globalPos());
            const ResizeRegion region = hitTestResizeRegion(rootPos);
            if (mouseEvent->button() == Qt::LeftButton && region != ResizeNone) {
                m_isResizing = true;
                m_resizeRegion = region;
                m_resizeStartGlobalPos = mouseEvent->globalPos();
                m_resizeStartGeometry = geometry();
                updateCursorForRegion(region);
                return true;
            }

            if (mouseEvent->button() == Qt::LeftButton && !m_isResizing) {
                m_isDragging = true;
                m_dragPosition = mouseEvent->globalPos() - frameGeometry().topLeft();
                m_titleBar->setCursor(Qt::ClosedHandCursor);
                return true;
            }
        } else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            const QPoint rootPos = m_rootWidget->mapFromGlobal(mouseEvent->globalPos());
            if (m_isResizing && (mouseEvent->buttons() & Qt::LeftButton)) {
                resizeFromGlobalPos(mouseEvent->globalPos());
                return true;
            }

            if ((mouseEvent->buttons() & Qt::LeftButton) && m_isDragging) {
                move(mouseEvent->globalPos() - m_dragPosition);
                return true;
            }

            if (!m_isDragging) {
                const ResizeRegion region = hitTestResizeRegion(rootPos);
                updateCursorForRegion(region);
                m_titleBar->setCursor(region == ResizeNone ? Qt::OpenHandCursor : m_rootWidget->cursor());
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton && m_isResizing) {
                m_isResizing = false;
                m_resizeRegion = ResizeNone;
                updateCursorForRegion(hitTestResizeRegion(m_rootWidget->mapFromGlobal(mouseEvent->globalPos())));
                m_titleBar->setCursor(Qt::OpenHandCursor);
                markGeometryCustomized();
                saveSettings();
                return true;
            }

            if (mouseEvent->button() == Qt::LeftButton) {
                m_isDragging = false;
                m_titleBar->setCursor(Qt::OpenHandCursor);
                markGeometryCustomized();
                saveSettings();
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::moveEvent(QMoveEvent *event) {
    QMainWindow::moveEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    if (m_persistOnShowDesktop) {
        lowerToDesktopLayer();
    }
}

void MainWindow::enforceDesktopVisibility() {
    if (m_allowProgrammaticHide || !isWindow()) {
        return;
    }

    const bool showingDesktop = queryShowDesktopState();

    if (m_persistOnShowDesktop && (showingDesktop || !isVisible() || isMinimized())) {
        show();
        showNormal();
    }

    if (isVisible()) {
        lowerToDesktopLayer();
    }
}

MainWindow::ResizeRegion MainWindow::hitTestResizeRegion(const QPoint &pos) const {
    const int margin = 10;
    const bool onLeft = pos.x() <= margin;
    const bool onRight = pos.x() >= m_rootWidget->width() - margin;
    const bool onTop = pos.y() <= margin;
    const bool onBottom = pos.y() >= m_rootWidget->height() - margin;

    if (onLeft && onTop) {
        return ResizeTopLeft;
    }
    if (onRight && onTop) {
        return ResizeTopRight;
    }
    if (onLeft && onBottom) {
        return ResizeBottomLeft;
    }
    if (onRight && onBottom) {
        return ResizeBottomRight;
    }
    if (onLeft) {
        return ResizeLeft;
    }
    if (onRight) {
        return ResizeRight;
    }
    if (onTop) {
        return ResizeTop;
    }
    if (onBottom) {
        return ResizeBottom;
    }
    return ResizeNone;
}

void MainWindow::updateCursorForRegion(ResizeRegion region) {
    QCursor cursor(Qt::ArrowCursor);
    switch (region) {
        case ResizeLeft:
        case ResizeRight:
            cursor = QCursor(Qt::SizeHorCursor);
            break;
        case ResizeTop:
        case ResizeBottom:
            cursor = QCursor(Qt::SizeVerCursor);
            break;
        case ResizeTopLeft:
        case ResizeBottomRight:
            cursor = QCursor(Qt::SizeFDiagCursor);
            break;
        case ResizeTopRight:
        case ResizeBottomLeft:
            cursor = QCursor(Qt::SizeBDiagCursor);
            break;
        case ResizeNone:
            cursor = QCursor(Qt::ArrowCursor);
            break;
    }

    m_rootWidget->setCursor(cursor);
}

void MainWindow::resizeFromGlobalPos(const QPoint &globalPos) {
    QRect newGeometry = m_resizeStartGeometry;
    const QPoint delta = globalPos - m_resizeStartGlobalPos;

    switch (m_resizeRegion) {
        case ResizeLeft:
        case ResizeTopLeft:
        case ResizeBottomLeft:
            newGeometry.setLeft(m_resizeStartGeometry.left() + delta.x());
            break;
        default:
            break;
    }

    switch (m_resizeRegion) {
        case ResizeRight:
        case ResizeTopRight:
        case ResizeBottomRight:
            newGeometry.setRight(m_resizeStartGeometry.right() + delta.x());
            break;
        default:
            break;
    }

    switch (m_resizeRegion) {
        case ResizeTop:
        case ResizeTopLeft:
        case ResizeTopRight:
            newGeometry.setTop(m_resizeStartGeometry.top() + delta.y());
            break;
        default:
            break;
    }

    switch (m_resizeRegion) {
        case ResizeBottom:
        case ResizeBottomLeft:
        case ResizeBottomRight:
            newGeometry.setBottom(m_resizeStartGeometry.bottom() + delta.y());
            break;
        default:
            break;
    }

    if (newGeometry.width() < minimumWidth()) {
        if (m_resizeRegion == ResizeLeft || m_resizeRegion == ResizeTopLeft || m_resizeRegion == ResizeBottomLeft) {
            newGeometry.setLeft(newGeometry.right() - minimumWidth() + 1);
        } else {
            newGeometry.setRight(newGeometry.left() + minimumWidth() - 1);
        }
    }

    if (newGeometry.height() < minimumHeight()) {
        if (m_resizeRegion == ResizeTop || m_resizeRegion == ResizeTopLeft || m_resizeRegion == ResizeTopRight) {
            newGeometry.setTop(newGeometry.bottom() - minimumHeight() + 1);
        } else {
            newGeometry.setBottom(newGeometry.top() + minimumHeight() - 1);
        }
    }

    setGeometry(newGeometry.normalized());
}

void MainWindow::lowerToDesktopLayer() {
    lower();

#ifdef Q_OS_LINUX
    if (!QGuiApplication::platformName().contains("xcb")) {
        return;
    }

    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        return;
    }

    const Window window = static_cast<Window>(winId());
    const Atom belowAtom = XInternAtom(display, "_NET_WM_STATE_BELOW", False);
    const Atom skipTaskbarAtom = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", False);
    const Atom skipPagerAtom = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", False);
    const Atom stickyAtom = XInternAtom(display, "_NET_WM_STATE_STICKY", False);
    const Atom desktopAtom = XInternAtom(display, "_NET_WM_DESKTOP", False);
    const Atom windowTypeAtom = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    const Atom desktopTypeAtom = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
    const unsigned long allDesktops = 0xFFFFFFFF;

    if (windowTypeAtom != None && desktopTypeAtom != None) {
        XChangeProperty(display, window, windowTypeAtom, XA_ATOM, 32, PropModeReplace,
                        reinterpret_cast<const unsigned char*>(&desktopTypeAtom), 1);
    }

    sendNetWmState(display, window, 1, belowAtom, skipTaskbarAtom);
    sendNetWmState(display, window, 1, skipPagerAtom, stickyAtom);
    XChangeProperty(display, window, desktopAtom, XA_CARDINAL, 32, PropModeReplace,
                    reinterpret_cast<const unsigned char*>(&allDesktops), 1);
    XLowerWindow(display, window);
    XFlush(display);
    XCloseDisplay(display);
#endif
}
