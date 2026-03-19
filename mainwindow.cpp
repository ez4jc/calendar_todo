#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QFrame>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>
#include "tododialog.h"
#include "databasemanager.h"

const qreal MainWindow::DEFAULT_OPACITY = 0.4;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_calendarView(nullptr)
    , m_monthTitle(nullptr)
    , m_prevButton(nullptr)
    , m_nextButton(nullptr)
    , m_settingsButton(nullptr)
    , m_closeButton(nullptr)
    , m_opacitySlider(nullptr)
    , m_opacityLabel(nullptr)
    , m_trayManager(nullptr)
    , m_isDragging(false)
    , m_currentOpacity(DEFAULT_OPACITY)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setupUI();

    m_trayManager = new SystemTrayManager(this);
    m_trayManager->setMainWindow(this);
    m_trayManager->setupTrayIcon();

    connect(m_trayManager, &SystemTrayManager::showRequested,
            this, &MainWindow::onTrayShowRequested);
    connect(m_trayManager, &SystemTrayManager::quitRequested,
            this, &MainWindow::onTrayQuitRequested);

    setWindowOpacity(m_currentOpacity);

    QDesktopWidget *desktop = QApplication::desktop();
    if (desktop->geometry().width() > 0 && desktop->geometry().height() > 0) {
        setGeometry(desktop->geometry());
    }

    loadSettings();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);

    QWidget *titleBar = new QWidget(centralWidget);
    titleBar->setFixedHeight(40);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(5, 0, 5, 0);

    m_prevButton = new QPushButton("◀", titleBar);
    m_prevButton->setFixedSize(40, 30);
    m_prevButton->setCursor(Qt::PointingHandCursor);
    connect(m_prevButton, &QPushButton::clicked, this, &MainWindow::onPrevMonth);
    titleLayout->addWidget(m_prevButton);

    m_monthTitle = new QLabel(titleBar);
    m_monthTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_monthTitle->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    titleLayout->addWidget(m_monthTitle);

    m_nextButton = new QPushButton("▶", titleBar);
    m_nextButton->setFixedSize(40, 30);
    m_nextButton->setCursor(Qt::PointingHandCursor);
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::onNextMonth);
    titleLayout->addWidget(m_nextButton);

    titleLayout->addStretch();

    m_opacityLabel = new QLabel("透明度: 40%", titleBar);
    m_opacityLabel->setStyleSheet("color: white; font-size: 12px;");
    titleLayout->addWidget(m_opacityLabel);

    m_opacitySlider = new QSlider(Qt::Horizontal, titleBar);
    m_opacitySlider->setMinimum(10);
    m_opacitySlider->setMaximum(100);
    m_opacitySlider->setValue(40);
    m_opacitySlider->setFixedWidth(100);
    connect(m_opacitySlider, &QSlider::valueChanged, this, &MainWindow::onOpacityChanged);
    titleLayout->addWidget(m_opacitySlider);

    m_settingsButton = new QPushButton("⚙", titleBar);
    m_settingsButton->setFixedSize(30, 30);
    m_settingsButton->setCursor(Qt::PointingHandCursor);
    connect(m_settingsButton, &QPushButton::clicked, this, &MainWindow::onShowSettings);
    titleLayout->addWidget(m_settingsButton);

    m_closeButton = new QPushButton("×", titleBar);
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setCursor(Qt::PointingHandCursor);
    connect(m_closeButton, &QPushButton::clicked, this, &QWidget::close);
    titleLayout->addWidget(m_closeButton);

    mainLayout->addWidget(titleBar);

    m_calendarView = new CalendarView(centralWidget);
    mainLayout->addWidget(m_calendarView);

    connect(m_calendarView, &CalendarView::dateDoubleClicked,
            this, &MainWindow::onDateDoubleClicked);

    centralWidget->setStyleSheet(
        "QWidget#centralWidget { "
        "   background-color: rgba(30, 30, 40, 0.85); "
        "   border-radius: 8px; "
        "   border: 1px solid rgba(100, 100, 100, 0.3); "
        "}"
        "QPushButton { "
        "   background-color: rgba(60, 60, 70, 0.8); "
        "   color: white; "
        "   border: none; "
        "   border-radius: 4px; "
        "   font-size: 14px; "
        "}"
        "QPushButton:hover { "
        "   background-color: rgba(80, 80, 90, 0.9); "
        "}"
        "QPushButton:pressed { "
        "   background-color: rgba(50, 50, 60, 0.9); "
        "}"
        "QSlider::groove:horizontal { "
        "   border: 1px solid rgba(100, 100, 100, 0.5); "
        "   height: 4px; "
        "   background: rgba(80, 80, 80, 0.8); "
        "   border-radius: 2px; "
        "}"
        "QSlider::handle:horizontal { "
        "   background: rgba(100, 150, 200, 0.9); "
        "   width: 14px; "
        "   margin: -5px 0; "
        "   border-radius: 7px; "
        "}"
    );

    updateMonthTitle();
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
    TodoDialog dialog(date, this);
    dialog.setModal(true);
    dialog.exec();

    m_calendarView->refreshCells();
}

void MainWindow::onOpacityChanged(int value) {
    m_currentOpacity = value / 100.0;
    setWindowOpacity(m_currentOpacity);
    updateOpacityLabel();
    saveSettings();
}

void MainWindow::onShowSettings() {
}

void MainWindow::onTrayShowRequested() {
}

void MainWindow::onTrayQuitRequested() {
    saveSettings();
    qApp->quit();
}

void MainWindow::loadSettings() {
    QString opacityStr = DatabaseManager::instance().getSetting("opacity", "0.4");
    bool ok;
    qreal opacity = QString(opacityStr).toDouble(&ok);
    if (ok && opacity >= 0.1 && opacity <= 1.0) {
        m_currentOpacity = opacity;
        setWindowOpacity(m_currentOpacity);
        m_opacitySlider->setValue(static_cast<int>(opacity * 100));
        updateOpacityLabel();
    }

    QString geometryStr = DatabaseManager::instance().getSetting("geometry", "");
    if (!geometryStr.isEmpty()) {
        QStringList parts = geometryStr.split(",");
        if (parts.size() == 4) {
            int x = parts[0].toInt();
            int y = parts[1].toInt();
            int w = parts[2].toInt();
            int h = parts[3].toInt();
            if (w > 0 && h > 0) {
                setGeometry(x, y, w, h);
            }
        }
    }
}

void MainWindow::saveSettings() {
    DatabaseManager::instance().setSetting("opacity", QString::number(m_currentOpacity));

    QRect geo = geometry();
    QString geometryStr = QString("%1,%2,%3,%4")
        .arg(geo.x())
        .arg(geo.y())
        .arg(geo.width())
        .arg(geo.height());
    DatabaseManager::instance().setSetting("geometry", geometryStr);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    hide();
    event->ignore();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton && m_isDragging) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
}
