#include <QStateMachine>
#include <QMessageBox>
#include <QSettings>
#include <QxtLogger>
#include "elements.h"
#include "pipeline.h"
#include "pluginmanager.h"
#include "deviceproxy.h"
#include "stripchart.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"



/*!
 * Construct a MainWindow as a child of the given \a parent widget.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pluginManager(new PluginManager(this)),
    pipeline(new Pipeline(this)),
    device(new DeviceProxy(this))
{
    ui->setupUi(this);

    ui->buttonConnect->setDefaultAction(ui->actionConnect);

    connect(ui->spacingSlider, SIGNAL(valueChanged(int)),
            ui->eegPlot, SLOT(setSpacing(int)));

    ui->eegPlot->setNStrips(8);
    ui->eegPlot->setNSamples(1000);
    ui->spacingSlider->setValue(6e3);

    connect(pipeline, SIGNAL(error(QString)), SLOT(showErrorMessage(QString)));
    connect(device, SIGNAL(error(QString)), SLOT(showErrorMessage(QString)));

    connect(pluginManager, SIGNAL(pluginsLoaded(ElementSet*)),
            SLOT(setupPipeline(ElementSet*)));

    buildStateMachine();

    if (QSettings().value("auto-connect", true).toBool())
        QMetaObject::invokeMethod(ui->actionConnect, "trigger", Qt::QueuedConnection);
}

/*!
 * Destroy this MainWindow.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onEegSample(const Sample &sample)
{
    auto eeg = static_cast<const EegSample&>(sample);
    ui->eegPlot->appendData(eeg.values);
}

void MainWindow::on_actionPlugins_triggered()
{
    pluginManager->loadPlugins();
}

void MainWindow::showErrorMessage(QString message)
{
    QMessageBox::warning(this, "Error", message);
}

/*!
 * \page mainwindow-fsm MainWindow State Machine
 *
 * The behaviour of the MainWindow is driven by the following hierarchical
 * state machine.
 *
 * @startuml{mainwindow-fsm.png}
 *
 * [*] -> Disconnected
 * Disconnected : enter / ui.showPage(0)
 * Disconnected -> Connecting : actionConnect
 *
 * Connecting : enter / device.connect()
 * Connecting : enter / spinner.run()
 * Connecting --> Disconnected : device.error
 * Connecting --> Connected : connected
 *
 * Connected : enter / ui.showPage(1)
 * Connected --> Disconnected : actionConnect
 * Connected --> Disconnected : device.error
 *
 * state Connected {
 *      [*] -> Idle
 *      Idle --> Active : buttonCapture
 *      Active --> Idle : buttonCapture
 *      Active --> Idle : pipeline.error
 *      Active : enter / device.startStreaming()
 *      Active : enter / pipeline.start()
 *      Active : exit / device.stopStreaming()
 *      Active : exit / pipeline.stop()
 *      state Active {
 *          [*] -> Starting
 *          Starting : enter / spinner.run()
 *          Starting -> Running : pipeline.started
 *      }
 * }
 *
 * @enduml
 */
void MainWindow::buildStateMachine()
{
    machine = new QStateMachine(this);
    machine->setGlobalRestorePolicy(QState::RestoreProperties);

    auto disconnected = new QState(machine);
    auto connecting = new QState(machine);
    auto connected = new QState(machine);
    auto idle = new QState(connected);
    auto active = new QState(connected);
    auto starting = new QState(active);
    auto running = new QState(active);

    machine->setInitialState(disconnected);
    disconnected->addTransition(ui->actionConnect, SIGNAL(triggered()), connecting);

    connecting->assignProperty(ui->spinnerConnecting, "running", true);
    connecting->assignProperty(ui->actionConnect, "enabled", false);
    connecting->assignProperty(this, "cursor", QCursor(Qt::WaitCursor));
    connect(connecting, SIGNAL(entered()), device, SLOT(connect()));
    connecting->addTransition(device, SIGNAL(error(QString)), disconnected);
    connecting->addTransition(device, SIGNAL(connected()), connected);

    connected->setInitialState(idle);
    connected->assignProperty(ui->stackedWidget, "currentIndex", 1);
    connected->assignProperty(ui->actionConnect, "text", "&Disconnect");
    connected->addTransition(ui->actionConnect, SIGNAL(triggered()), disconnected);
    connected->addTransition(device, SIGNAL(error(QString)), disconnected);
    connect(connected, SIGNAL(exited()), device, SLOT(disconnect()));

    idle->addTransition(ui->buttonCapture, SIGNAL(clicked()), active);

    active->setInitialState(starting);
    connect(active, SIGNAL(entered()), pipeline, SLOT(start()));
    connect(active, SIGNAL(exited()), pipeline, SLOT(stop()));
    // We need to delay evaluation of device->device() by wrapping it in a
    // closure since it is not valid until the device is connected.
    connect(active, &QState::entered, [=](){ device->device()->startStreaming(); });
    connect(active, &QState::exited, [=](){ device->device()->stopStreaming(); });
    active->addTransition(ui->buttonCapture, SIGNAL(clicked()), idle);
    active->addTransition(pipeline, SIGNAL(error(QString)), idle);

    starting->assignProperty(ui->spinnerStarting, "running", true);
    starting->assignProperty(ui->buttonCapture, "enabled", false);
    connecting->assignProperty(this, "cursor", QCursor(Qt::WaitCursor));
    starting->addTransition(pipeline, SIGNAL(started()), running);

    running->assignProperty(ui->buttonCapture, "text", "Stop");

    machine->start();
}

void MainWindow::setupPipeline(ElementSet *elements)
{
    Q_ASSERT(elements);
    pipeline->setElements(elements);

    elements->dataSource->setProperty("host", device->host());
    elements->sampleDecoders[EEG]->setProperty("gain", 1);
    elements->sampleDecoders[EEG]->setProperty("vref", 4.5e6);

    connect(elements->sampleDecoders[EEG],
            SIGNAL(newSample(Sample)), SLOT(onEegSample(Sample)));
}
