#include "find.h"
#include "ui_find.h"
#include <QTextEdit>

Find::Find(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Find)
{
    ui->setupUi(this);
}

Find::~Find()
{
    delete ui;
}

void Find::on_FindNextButton_clicked()
{
    QTextDocument::FindFlags findFlags = QTextDocument::FindFlags();
    if(ui->radioButton->isChecked()) findFlags.setFlag(QTextDocument::FindBackward);
    if(ui->checkBox->isChecked()) findFlags.setFlag(QTextDocument::FindCaseSensitively);
    emit findText(ui->lineEdit->text(), findFlags);
}

void Find::on_lineEdit_textChanged(const QString&)
{
    ui->FindNextButton->setEnabled(
                !ui->lineEdit->text().isEmpty());
}
