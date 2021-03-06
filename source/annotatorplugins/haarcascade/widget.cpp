#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QFile>
#include "haarcascade.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new HaarCascade::Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setHaarcascade(Annotator::Plugins::Haarcascade *haarcascade)
{
    this->haarcascade = haarcascade;
}

void Widget::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select Haarcascade XML"),".",tr("XML Files (*.xml)"));
    if(QFile::exists(fileName))
    {
        this->ui->xmlFileLineEdit->setText(fileName);
        haarcascade->loadCascade(fileName.toStdString());
    }
}

void Widget::on_objectNameLineEdit_editingFinished()
{
    haarcascade->setObjectName(ui->objectNameLineEdit->text().toStdString());
}

void Widget::on_newObjectsCheckBox_toggled(bool checked)
{
    haarcascade->setNewObjects(checked);
    ui->objectNameLineEdit->setEnabled(checked);
}
