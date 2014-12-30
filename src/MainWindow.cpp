#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QCloseEvent>
#include <QProgressBar>
#include <QVBoxLayout>
#include "LSystem.h"
#include "LSystemPainterWidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow),
    m_iterationThread(), m_iterationTimer(), m_iterating(false)
{
    ui->setupUi(this);

    // set up the L-System
    RulesDict rules;
    rules['F'] = "F[+F]F[-F][F]";
    m_lsystem = LSystemPtr(new LSystem("F", rules));
    m_lsystem->moveToThread(&m_iterationThread);

    connect(this, &MainWindow::start_iteration,
            &*m_lsystem, &LSystem::iterate);
    connect(&*m_lsystem, &LSystem::iteration_finished,
            this, &MainWindow::iteration_finished);
    connect(&*m_lsystem, &LSystem::iteration_progressed,
            this, &MainWindow::update_progress);
    m_iterationThread.start();

    // set up the renderer
    QVBoxLayout *centralLayout = new QVBoxLayout();
    m_rendererWidget = new LSystemPainterWidget(m_lsystem, this);
    centralLayout->addWidget(m_rendererWidget);
    ui->centralWidget->setLayout(centralLayout);

    connect(m_rendererWidget, &LSystemRendererWidgetBase::status_changed,
            this, &MainWindow::renderer_status_update);
    connect(m_rendererWidget, &LSystemRendererWidgetBase::progress_changed,
            this, &MainWindow::update_progress);

    // create the progress bar in the status bar
    m_progressBar = new QProgressBar(ui->statusBar);
    m_progressBar->setMaximumSize(180, 20);
    m_progressBar->setAlignment(Qt::AlignRight);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    ui->statusBar->addPermanentWidget(m_progressBar);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_rendererWidget;

    m_iterationThread.quit();
    m_iterationThread.wait();

    m_lsystem.clear();
}

void MainWindow::update_progress(uint progress)
{
    m_progressBar->setValue(progress);
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::renderer_status_update(const QString &status)
{
    ui->statusBar->showMessage(status, 4000);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // in case we are currently iterating
    //!! @todo Immediatly stop the iteration work
    if (m_iterating)
    {
        qDebug() << "Iterating, cannot close now...";
        connect(&*m_lsystem, &LSystem::iteration_finished,
                this, &MainWindow::close);
        event->ignore();
    }
    // otherwise, we can close the window
    qDebug() << "Closing MainWindow.";
    event->accept();
}

void MainWindow::iteration_finished()
{
    // Wrap up the iteration job
    qDebug() << "generation = " << m_lsystem->generation()
             << ", state_len = " << m_lsystem->state().length();
    QString status = tr("Iterated in %1 ms").arg(m_iterationTimer.elapsed());
    ui->statusBar->showMessage(status, 3000);
    ui->action_nextIteration->setEnabled(true);
    ui->action_render_LSystem->setEnabled(true);
    m_iterating = false;
}


void MainWindow::on_action_nextIteration_triggered()
{
    ui->statusBar->showMessage("Iterating...");
    ui->action_nextIteration->setEnabled(false);
    ui->action_render_LSystem->setEnabled(false);
    m_iterationTimer.start();
    m_iterating = true;
    emit start_iteration();
}

void MainWindow::on_action_render_LSystem_triggered()
{
    m_rendererWidget->render_lSystem();
}
