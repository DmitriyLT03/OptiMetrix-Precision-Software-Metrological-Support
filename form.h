#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <device_error.h>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
private slots:
    void on_button_hide_clicked();
    void on_button_save_clicked();
    void on_button_open_clicked();

private:
    Ui::Form *ui;
};

#endif // FORM_H
