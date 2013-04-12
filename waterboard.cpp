#include "waterboard.h"
#include "ui_waterboard.h"
#include <QTextDocument>
#include <QPrinter>
#include <QProcess>
#include <QPainter>
#include <QFileDialog>
#include <QAbstractTextDocumentLayout>
#include <QMessageBox>
#include <unistd.h>

WaterBoard::WaterBoard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WaterBoard)
{
    ui->setupUi(this);
    ui->pushButtonWatermarkPDF->setDisabled(true);
    ui->pushButtonRemovePDF->setDisabled(true);
}

WaterBoard::~WaterBoard()
{
    delete ui;
}

void WaterBoard::on_pushButtonWatermarkPDF_clicked()
{
    // 1. create a watermark to stamp on the PDF:
    QString watermarkfile = "temp_watermark.pdf";

    QTextDocument doc;
    //doc.setDefaultStyleSheet("watermark.css"); helpt blijkbaar niet om te printen :-(
    QString watermarktext = "<center><font color=\"gray\">";
    watermarktext.append(tr("Uitsluitend te gebruiken door "));
    watermarktext.append(ui->lineEditStudentName->text());
    watermarktext.append(" (");
    watermarktext.append(ui->lineEditDate->text());
    watermarktext.append(") ");
    watermarktext.append(ui->lineEditSchool->text());
    watermarktext.append("</font></center>");
    doc.setHtml(watermarktext);

    QPrinter printer;
    // <vvim> very stupid, but to get the vertical text, I first make it a landscape page, than use PDFtk to rotate it, than watermark
    printer.setOrientation(QPrinter::Portrait);

    // <vvim> to avoid the printing of page numbers (very annoying if you are making a template for a watermark), QPrinter needs a proper paperSize, see http://stackoverflow.com/questions/9430133/page-number-in-qtextdocument-for-envelopes
    QSizeF paperSize;
    paperSize.setWidth(printer.width());
    paperSize.setHeight(printer.height());
    printer.setPageMargins(0.0, 0.0, 0.0, 0.0, QPrinter::Point);
    doc.setPageSize(paperSize);

    printer.setOutputFileName(watermarkfile);
    printer.setOutputFormat(QPrinter::PdfFormat);
    doc.print(&printer);
    printer.newPage();

    // 2. rotate the watermark so that the stamp will come on the left-handside of the PDF
    QString watermarkfile_rotated = "temp_watermark_rotated.pdf";

    //QMessageBox msgbox;
    //msgbox.setText(QDir::currentPath());
    //msgbox.exec();

    QProcess *proc = new QProcess();
    QString program = "pdftk";

    QStringList arguments_rotate;
    arguments_rotate << watermarkfile << "cat" << "1S" << "output" << watermarkfile_rotated;

    proc->execute(program, arguments_rotate);

    // 3. for each pdf in the ListOfPDFs: stamp the watermark onto it
    while(ui->listOfPDFs->count()>0)
    {
        QListWidgetItem *temporary_item = ui->listOfPDFs->takeItem(0);  // takeItem REMOVES and RETURNS, so no extra DELETE needed: http://www.riverbankcomputing.co.uk/static/Docs/PyQt4/html/qlistwidget.html#takeItem
        QString inputfile = temporary_item->text();
        QString outputfile = inputfile; outputfile.append("-watermark.pdf");

        //pdftk %1 background c:\\temp\\file.pdf output output.pdf
        QStringList arguments;
        arguments << inputfile << "stamp" << watermarkfile_rotated << "output" << outputfile;

        proc->execute(program, arguments);

        if(ui->checkBoxOverwriteExistingFile->isChecked())
        {
            // 1. check if there indeed was a 'outputfile' created
            if(QFile::exists(outputfile))
            {
                // 2. checkbox "Overwrite" was checked AND the outputfile has really been created
                //      => remove the inputfile and rename the outpufile to the inputfile.
                QFile::remove(inputfile);
                // overwrite the original file
                QFile::rename(outputfile,inputfile);
            }
        }
    }

    QMessageBox msgbox;
    msgbox.setText(tr("PDFs zijn gewatermerkt."));
    msgbox.exec();

    ui->pushButtonWatermarkPDF->setDisabled(true);
}

void WaterBoard::on_pushButtonAddPDF_clicked()
{
    // open Dialog Box
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open PDF om te watermerken"), "./", tr("PDF bestanden (*.pdf);;Alle bestanden (*.* *)"));

    // change label if PDF file was chosen
    if(fileName != NULL)
    {
        QListWidgetItem *newpdftoadd = new QListWidgetItem(fileName);
        ui->listOfPDFs->addItem(newpdftoadd);
        // now we can also enable the functionality of the addPageRangeButton and the startCuttingProcessButton (without a PDF file, they are useless)
        ui->pushButtonWatermarkPDF->setEnabled(true);

        ui->statusBar->showMessage(tr("Nieuw PDF-bestand toegevoegd"));
    }
/*
    DO NOT put an "else". Imagine the scenario that the user has selected a PDF file to cut, than accidentally presses "Open File" again and presses "CANCEL" to correct his error.

    else
    {
        ui->PDFFileNameLabel->setText(tr("...geen PDF gekozen"));
    }
*/
}

void WaterBoard::ListFiles(QDir directory, QString fileExtension)
{
    // code based on 'momesana': http://www.qtcentre.org/threads/9854-Searching-files-from-directories-amp-its-subdirectories
    QDir dir(directory);
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QFileInfo finfo, list)
    {
        if (finfo.suffix() == fileExtension)
        {
            QListWidgetItem *newpdftoadd = new QListWidgetItem(finfo.absoluteFilePath());
            ui->listOfPDFs->addItem(newpdftoadd);
            // now we can also enable the functionality of the addPageRangeButton and the startCuttingProcessButton (without a PDF file, they are useless)
            ui->pushButtonWatermarkPDF->setEnabled(true);
        }
        if (finfo.isDir())
        {
            ListFiles(QDir(finfo.absoluteFilePath()), fileExtension);
        }
    }
}

void WaterBoard::on_pushButtonAddDir_clicked()
{
    // open Dialog Box
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open folder met PDFs om te watermerken"), "./");

    if(dirName != NULL)
    {
        QDir dir(dirName);
        ListFiles(dir, "pdf");
        ui->statusBar->showMessage(tr("PDF-bestanden uit folder en subfolders toegevoegd"));
    }
}

void WaterBoard::on_pushButtonRemovePDF_clicked()
{
    qDeleteAll(ui->listOfPDFs->selectedItems()); // see http://lists.trolltech.com/qt-interest/2007-09/thread00253-0.html
    ui->listOfPDFs->clearSelection(); // don't show any selection (normally the selection shifts, I want it gone
    ui->pushButtonRemovePDF->setDisabled(true);
    ui->statusBar->showMessage(tr("PDF verwijderd."));
}

void WaterBoard::on_listOfPDFs_itemClicked(QListWidgetItem *item)
{
    ui->pushButtonRemovePDF->setEnabled(true);
}

void WaterBoard::on_pushButtonRemoveAll_clicked()
{
    QMessageBox msgBox;

    msgBox.setText(tr("Verwijder alle PDFs."));
    msgBox.setInformativeText(tr("Ben je zeker dat je de hele lijst wilt wissen?"));
    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::YesAll);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();

    if (ret == QMessageBox::YesAll)
    {
        ui->listOfPDFs->clear();
        ui->pushButtonRemovePDF->setDisabled(true);
        ui->statusBar->showMessage(tr("Alle PDFs verwijderd."));
    }
}
