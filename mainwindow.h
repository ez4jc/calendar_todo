#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QSlider>
#include <QSizeGrip>
#include "calendarview.h"
#include "systemtraymanager.h"

class QLabel;
class QEvent;
class QHideEvent;
class QMoveEvent;
class QPushButton;
class QResizeEvent;
class QShowEvent;
class QTimer;
class QHBoxLayout;
class QWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    enum ResizeRegion {
        ResizeNone,
        ResizeLeft,
        ResizeRight,
        ResizeTop,
        ResizeBottom,
        ResizeTopLeft,
        ResizeTopRight,
        ResizeBottomLeft,
        ResizeBottomRight
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadSettings();
    void saveSettings();

protected:
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onPrevMonth();
    void onNextMonth();
    void onDateDoubleClicked(const QDate &date);
    void onOpacityChanged(int value);
    void onShowSettings();
    void onTrayShowRequested();
    void onTrayQuitRequested();

private:
    void applyDefaultGeometry();
    void applyWindowStyle();
    ResizeRegion hitTestResizeRegion(const QPoint &pos) const;
    void updateCursorForRegion(ResizeRegion region);
    void resizeFromGlobalPos(const QPoint &globalPos);
    void lowerToDesktopLayer();
    void setupUI();
    void updateMonthTitle();
    void updateOpacityLabel();

    CalendarView *m_calendarView;
    QWidget *m_rootWidget;
    QWidget *m_titleBar;
    QLabel *m_monthTitle;
    QPushButton *m_prevButton;
    QPushButton *m_nextButton;
    QPushButton *m_settingsButton;
    QPushButton *m_closeButton;
    QSlider *m_opacitySlider;
    QLabel *m_opacityLabel;
    QSizeGrip *m_sizeGrip;
    QTimer *m_desktopEnforcerTimer;

    SystemTrayManager *m_trayManager;

    bool m_isDragging;
    bool m_isResizing;
    bool m_allowProgrammaticHide;
    bool m_hasSavedGeometry;
    QPoint m_dragPosition;
    QPoint m_resizeStartGlobalPos;
    QRect m_resizeStartGeometry;
    ResizeRegion m_resizeRegion;

    qreal m_currentOpacity;
    static const qreal DEFAULT_OPACITY;
};

#endif // MAINWINDOW_H
