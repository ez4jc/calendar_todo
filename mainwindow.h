#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QSlider>
#include "calendarview.h"
#include "systemtraymanager.h"

class QLabel;
class QPushButton;
class QHBoxLayout;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadSettings();
    void saveSettings();

protected:
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onPrevMonth();
    void onNextMonth();
    void onDateDoubleClicked(const QDate &date);
    void onOpacityChanged(int value);
    void onShowSettings();
    void onTrayShowRequested();
    void onTrayQuitRequested();

private:
    void setupUI();
    void updateMonthTitle();
    void updateOpacityLabel();

    CalendarView *m_calendarView;
    QLabel *m_monthTitle;
    QPushButton *m_prevButton;
    QPushButton *m_nextButton;
    QPushButton *m_settingsButton;
    QPushButton *m_closeButton;
    QSlider *m_opacitySlider;
    QLabel *m_opacityLabel;

    SystemTrayManager *m_trayManager;

    bool m_isDragging;
    QPoint m_dragPosition;

    qreal m_currentOpacity;
    static const qreal DEFAULT_OPACITY;
};

#endif // MAINWINDOW_H
