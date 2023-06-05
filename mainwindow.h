#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractButton>
#include <QMainWindow>
#include <device_controller.h>
#include <QTimer>
#include <QHeaderView>
#include <QStringLiteral>
#include <QAbstractItemModel>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QAction>
#include <QMouseEvent>
#include <form.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif

private slots:
    void update_position();
    void on_button_connect_clicked();
    void on_button_disconnect_clicked();
    void on_run_relative_displacement_clicked();
    void on_run_target_point_clicked();
    void on_button_right_pressed();
    void on_button_left_pressed();
    void on_go_home_clicked();
    void on_button_stop_clicked();
    void on_button_expert_mode_clicked();
    void on_button_apply_clicked();
    void on_button_cancel_clicked();


private:
    int i = 0;
    Ui::MainWindow *ui;
    Form *f;
    device_controller c;
    void createActions();
    void createMenus();

    QMenu *deviceMenu;
    QAction *connectDevice;
    QAction *disconnectDevice;

    QMenu *modeMenu;
    QAction *expertMenu;
    QTimer *timer;
};
#endif // MAINWINDOW_H
