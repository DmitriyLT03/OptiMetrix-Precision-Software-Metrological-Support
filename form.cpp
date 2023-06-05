#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    connect(ui->hideForm, &QPushButton::clicked, this, &Form::on_button_hide_clicked);
    ui->tableWidget->setColumnCount(1);
    ui->tableWidget->setRowCount(540);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Корректировка");

    for (int i = 0; i < 540; i++) {
        QTableWidgetItem *item = ui->tableWidget->item(i, 0);
        if (!item) {
            item = new QTableWidgetItem();
            ui->tableWidget->setItem(i, 0, item);
        }
        item->setText("0.0");
    }

    connect(ui->saveButton, &QPushButton::clicked, this, &Form::on_button_save_clicked);
    connect(ui->openButton, &QPushButton::clicked, this, &Form::on_button_open_clicked);

}
Form::~Form()
{
    delete ui;
}

void Form::on_button_hide_clicked()
{
    this->hide();
}

void Form::on_button_save_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Открыть таблицу корректировок",
                                                    ".",
                                                    "Binary files (*.bin)");
    if (filename.isEmpty()) {
        return;
    }
    try {
        FILE *f = fopen(filename.toStdString().c_str(), "wb");
        if (!f) {
            fclose(f);
            throw device_error("Не получилось открыть файл с таблицей корректировок для записи");
        }
        for (int i = 0; i < 540; i++) {
            bool ok;
            float dx = ui->tableWidget->item(i, 0)->text().toFloat(&ok);
            if (!ok) {
                fclose(f);
                throw device_error("Не получилось счиатть число из графического интерфейса");
            }
            fwrite(&dx, sizeof(float), 1, f);
        }
        fclose(f);
    } catch (device_error &e) {
        QMessageBox::critical(this, "Ошибка", e.get_error_c_string());
    }


}

void Form::on_button_open_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Открыть таблицу корректировок",
                                                    ".",
                                                    "Binary files (*.bin)");
    if (filename.isEmpty()) {
        return;
    }

    try {
        FILE *f = fopen(filename.toStdString().c_str(), "rb");
        if (!f) {
            throw device_error("Не получилось открыть файл с таблицей корректировок для чтения");
        }
        for (int i = 0; i < 540; i++) {
            float value;
            fread(&value, sizeof(float), 1, f);
            QTableWidgetItem *item = ui->tableWidget->item(i, 0);
            item->setText(QString::number(value));
        }
        fclose(f);
    } catch (device_error &e) {
        QMessageBox::critical(this, "Ошибка", e.get_error_c_string());
    }

}
