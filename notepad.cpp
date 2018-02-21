#include "notepad.h"
#include "ui_notepad.h"
#include "find.h"

Notepad::Notepad(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Notepad)
{
    finder = new Find(this);
    finder->setModal(false);
    finder->setWindowFlags(Qt::Dialog
                         | Qt::MSWindowsFixedSizeDialogHint
                         | Qt::WindowTitleHint
                         | Qt::WindowCloseButtonHint
                         | Qt::CustomizeWindowHint);
    QObject::connect(finder, SIGNAL(findText(QString,QTextDocument::FindFlags)), this, SLOT(FindNext_triggered(QString,QTextDocument::FindFlags)));

    replacer = new FindAndReplace(this);
    replacer->setModal(false);
    replacer->setWindowFlags(Qt::Dialog
                           | Qt::MSWindowsFixedSizeDialogHint
                           | Qt::WindowTitleHint
                           | Qt::WindowCloseButtonHint
                           | Qt::CustomizeWindowHint);
    QObject::connect(replacer, SIGNAL(findText(QString,QTextDocument::FindFlags)), this, SLOT(FindNext_triggered(QString,QTextDocument::FindFlags)));
    QObject::connect(replacer, SIGNAL(replaceText(QString)), this, SLOT(Replace_triggered(QString)));
    QObject::connect(replacer, SIGNAL(replaceAll(QString,QString,QTextDocument::FindFlags)), this, SLOT(ReplaceAll_triggered(QString,QString,QTextDocument::FindFlags)));

    ui->setupUi(this);

    this->setCentralWidget(ui->textEdit);
    QPalette p = ui->textEdit->palette();
    p.setColor(QPalette::Inactive, QPalette::HighlightedText, p.color(QPalette::Active, QPalette::HighlightedText));
    p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Active, QPalette::Highlight));
    ui->textEdit->setPalette(p);
}

Notepad::~Notepad()
{
    delete ui;
}

void Notepad::on_actionNew_triggered()
{
    currentFile.clear();
    ui->textEdit->setText(QString());
    setWindowTitle("Untitled - Notepad");
}

void Notepad::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        currentFile.clear();
        return;
    }
    currentFile = fileName;
    setWindowTitle(fileName + " - Notepad");
    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->setText(text);
    file.close();
}

void Notepad::on_actionSave_triggered()
{
    QString fileName;
    if (currentFile.isEmpty())
    {
        fileName = QFileDialog::getSaveFileName(this, "Save");
        currentFile = fileName;
        setWindowTitle(fileName + " - Notepad");
    }
    else
    {
        fileName = currentFile;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.close();
}

void Notepad::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as");
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    currentFile = fileName;
    setWindowTitle(fileName + " - Notepad");
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.close();
}


void Notepad::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void Notepad::on_actionPrint_triggered()
{
#if QT_CONFIG(printer)
    QPrinter printDev;
#if QT_CONFIG(printdialog)
    QPrintDialog dialog(&printDev, this);
    if (dialog.exec() == QDialog::Rejected)
    {
        QMessageBox::warning(this, "Warning", "Cannot be used on non-native printers");
        return;
    }
#endif // QT_CONFIG(printdialog)
    ui->textEdit->print(&printDev);
#endif // QT_CONFIG(printer)
}

void Notepad::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}

void Notepad::on_actionCut_triggered()
{
    ui->textEdit->cut();
}

void Notepad::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}

void Notepad::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}

void Notepad::on_actionDelete_triggered()
{
    ui->textEdit->textCursor().removeSelectedText();
}

void Notepad::on_textEdit_selectionChanged()
{
    bool hasSelection = ui->textEdit->textCursor().hasSelection();
    ui->actionCut->setEnabled(hasSelection);
    ui->actionCopy->setEnabled(hasSelection);
    ui->actionDelete->setEnabled(hasSelection);
}

void Notepad::on_actionAdd_Time_Date_triggered()
{
    QLocale loc;
    ui->textEdit->textCursor().insertText(
                QTime::currentTime().toString(loc.timeFormat(QLocale::ShortFormat)) + " " +
                QDate::currentDate().toString(loc.dateFormat(QLocale::ShortFormat)));
}

void Notepad::on_actionFind_triggered()
{
    finder->show();
}

void Notepad::on_actionFind_next_triggered()
{
    if (findText.isEmpty())
        finder->show();
    else
        ui->textEdit->find(findText, findFlags);
}

void Notepad::on_actionReplace_triggered()
{
     replacer->show();
}

void Notepad::on_actionSelect_All_triggered()
{
    ui->textEdit->selectAll();
}

void Notepad::FindNext_triggered(QString what, QTextDocument::FindFlags flags)
{
    findText = what;
    findFlags = flags;
    ui->textEdit->find(findText, findFlags);
}

void Notepad::Replace_triggered(QString with)
{
    if (ui->textEdit->textCursor().hasSelection())
        ui->textEdit->textCursor().insertText(with);
}

void Notepad::ReplaceAll_triggered(QString what, QString with, QTextDocument::FindFlags flags)
{
    findText = what;
    findFlags = flags;
    ui->textEdit->textCursor().beginEditBlock();
    while (ui->textEdit->find(findText, findFlags))
    {
        if (ui->textEdit->textCursor().hasSelection())
            ui->textEdit->textCursor().insertText(with);
    }
    ui->textEdit->textCursor().endEditBlock();
}
