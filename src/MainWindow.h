#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include "LSystemRendererWidgetBase.h"

namespace Ui {
class MainWindow;
}
class QProgressBar;
class QCloseEvent;

/**
 * @brief The main window of the application.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    /**
     * @brief Allows MainWindow to visually account for a progress in a work
     * (e.g. iteration).
     * @param progress Progress in percent.
     */
    void update_progress(uint progress);

private slots:
    // UI slots
    void on_actionQuit_triggered();
    void on_action_nextIteration_triggered();

    // L-System and rendering slots
    void renderer_status_update(const QString &status);
    void iteration_finished(); //!< Fired when L-System finished iterating

    void on_action_render_LSystem_triggered();

signals:
    void start_iteration();

private:
    void closeEvent(QCloseEvent *event);

    Ui::MainWindow *ui;
    QProgressBar *m_progressBar;
    LSystemRendererWidgetBase *m_rendererWidget;

    LSystemPtr m_lsystem;
    QThread m_iterationThread;
    QElapsedTimer m_iterationTimer;
    bool m_iterating;
};

#endif /* MAINWINDOW_H */
