#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "device_error.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(480, 700));
    this->createActions();
    this->createMenus();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update_position);
    timer->setInterval(17);
    timer->start();
    update_position();

    connect(ui->on_go_home, &QPushButton::clicked, this, &MainWindow::on_go_home_clicked);
    connect(ui->on_run_relative_displacement, &QPushButton::clicked, this, &MainWindow::on_run_relative_displacement_clicked);
    connect(ui->on_run_target_point, &QPushButton::clicked, this, &MainWindow::on_run_target_point_clicked);
    connect(ui->on_button_right, &QPushButton::pressed, this, &MainWindow::on_button_right_pressed);
    connect(ui->on_button_left, &QPushButton::pressed, this, &MainWindow::on_button_left_pressed);
    connect(ui->on_button_stop, &QPushButton::clicked, this, &MainWindow::on_button_stop_clicked);
    connect(ui->on_button_right, &QPushButton::released, this, &MainWindow::on_button_stop_clicked);
    connect(ui->on_button_left, &QPushButton::released, this, &MainWindow::on_button_stop_clicked);
    connect(ui->applyButton, &QPushButton::clicked, this, &MainWindow::on_button_apply_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::on_button_cancel_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(connectDevice);
    menu.addAction(disconnectDevice);
    menu.addAction(expertMenu);
    menu.exec(event->globalPos());
}

void MainWindow::createMenus()
{
    deviceMenu = menuBar()->addMenu(tr("&Устройство"));
    deviceMenu->addAction(connectDevice);
    deviceMenu->addAction(disconnectDevice);

    modeMenu = menuBar()->addMenu(tr("&Режим"));
    modeMenu->addAction(expertMenu);
}
void MainWindow::createActions()
{
    connectDevice = new QAction(tr("Подключить устройство"), this);
    connect(connectDevice, &QAction::triggered, this, &MainWindow::on_button_connect_clicked);

    disconnectDevice = new QAction(tr("Отключить устройство"), this);
    connect(disconnectDevice, &QAction::triggered, this, &MainWindow::on_button_disconnect_clicked);

    expertMenu = new QAction(tr("Режим эксперта"), this);
    connect(expertMenu, &QAction::triggered, this, &MainWindow::on_button_expert_mode_clicked);
}


void MainWindow::on_button_connect_clicked()
{
    try {
        c.connect_controller();
    }
    catch (device_error& e) {
        QMessageBox::critical(this, "Ошибка", e.get_error_c_string());
    }
}

void MainWindow::on_button_disconnect_clicked()
{
    try {
        c.disconnect_controller();
    }
    catch (device_error& e) {
        QMessageBox::critical(this, "Ошибка", e.get_error_c_string());
    }
}

void MainWindow::update_position()
{
    if (c.get_connection_status() == true) {
        QString pos = QString(c.get_position().c_str());
        ui->label_2->setText(pos);
    } else {
        ui->label_2->setText("Устройство подключено");
    }
}


void MainWindow::on_go_home_clicked()
{
    try {
        c.home();
    }
    catch (device_error& e) {
        QMessageBox::critical(this, "Ошибка", e.get_error_c_string());
    }
}

void MainWindow::on_button_stop_clicked()
{
    try {
        c.stop();
    }
    catch (device_error &e) {
        QMessageBox::critical(this, "Ошибка", e.get_error_c_string());
    }
}

void MainWindow::on_button_expert_mode_clicked()
{
    this->f = new Form(this);
    this->f->show();
}

void MainWindow::on_button_apply_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                        "Открыть таблицу корректировок",
                                                        ".",
                                                        "Binary files (*.bin)");
    try {
        this->c.read_file(filename.toStdString().c_str());
    } catch (device_error &e) {
        QMessageBox::critical(this, "Ошибка", e.get_error_c_string());
    }
}

void MainWindow::on_button_cancel_clicked()
{
    c.table_applied = false;
    free(c.table);
}


void MainWindow::on_button_left_pressed()
{
    try {
        c.move_left_msec(17);
    }
    catch (device_error& e) {
        QMessageBox::critical(this, "Ошибка", e.get_error_c_string());
    }
}


void MainWindow::on_button_right_pressed()
{
    try {
        c.move_right_msec(17);
    }
    catch (device_error& e) {
        QMessageBox::critical(this, "Ошибка", e.get_error_c_string());
    }
}

void MainWindow::on_run_relative_displacement_clicked()
{
    try {
        float  moving;
        moving = ui->lineEdit->text().toFloat();
        c.move(moving);
    }
    catch (device_error& e) {
        QMessageBox::critical(this, "Ошибка", e.get_error_c_string());
    }
}

void MainWindow::on_run_target_point_clicked()
{
    try {
        float  move_to;
        move_to = ui->lineEdit_2->text().toFloat();
        c.move_to(move_to);
    }
    catch (device_error& e) {
        QMessageBox::critical(this, "Ошибка", e.get_error_c_string());
    }
}
