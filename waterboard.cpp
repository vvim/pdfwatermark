#include "waterboard.h"
#include "ui_waterboard.h"
#include <QTextDocument>
#include <QPrinter>
#include <QProcess>
#include <QPainter>
#include <QFileDialog>
#include <QAbstractTextDocumentLayout>
#include <QMessageBox>


WaterBoard::WaterBoard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WaterBoard)
{
    ui->setupUi(this);
    ui->pushButtonWatermarkPDF->setDisabled(true);
    ui->lineEditDate->setDisabled(true);
    ui->lineEditStudentName->setDisabled(true);
    ui->lineEditSchool->setDisabled(true);
    inputfile = "";
}

WaterBoard::~WaterBoard()
{
    delete ui;
}

void WaterBoard::on_pushButtonWatermarkPDF_clicked()
{
    QString watermarkfile = "temp_watermark.pdf";
    QString watermarkfile_rotated = "temp_watermark_rotated.pdf";
    QString outputfile = inputfile; outputfile.append("-watermark.pdf");

    QTextDocument doc;
    //doc.setDefaultStyleSheet("watermark.css"); helpt blijkbaar niet om te printen :-(
    QString watermarktext = "<center><font color=\"gray\">";
    watermarktext.append(tr("Uitsluitend te gebruiken door "));
    watermarktext.append(ui->lineEditStudentName->text());
    watermarktext.append(" (");
    watermarktext.append(ui->lineEditDate->text());
    watermarktext.append(") * ");
    watermarktext.append(ui->lineEditSchool->text());
    watermarktext.append("</font></center>");
    doc.setHtml(watermarktext);


    QPrinter printer;
    // <vvim> very stupid, but to get the vertical text, I first make it a landscape page, than use PDFtk to rotate it, than watermark
    printer.setOrientation(QPrinter::Landscape);

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

    QProcess *proc = new QProcess();
    QString program = "pdftk";

    //pdftk %1 background c:\\temp\\file.pdf output output.pdf
    QStringList arguments;
    arguments << inputfile << "background" << watermarkfile_rotated << "output" << outputfile;

    QStringList arguments_rotate;
    arguments_rotate << watermarkfile << "cat" << "1W" << "output" << watermarkfile_rotated;

    proc->execute(program, arguments_rotate);
    proc->execute(program, arguments);

    QMessageBox msgbox;
    msgbox.setText(tr("PDF is nu gewatermerkt en is opgeslagen als ").append(outputfile.toStdString().c_str()));
    msgbox.exec();
}


void WaterBoard::on_pushButtonOpenPDF_clicked()
{
    // open Dialog Box
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open PDF om te watermerken"), "./", tr("PDF bestanden (*.pdf);;Alle bestanden (*.* *)"));

    // change label if PDF file was chosen
    if(fileName != NULL)
    {
        ui->PDFFileNameLabel->setText(fileName);
        // now we can also enable the functionality of the addPageRangeButton and the startCuttingProcessButton (without a PDF file, they are useless)
        ui->pushButtonWatermarkPDF->setEnabled(true);
        ui->lineEditDate->setEnabled(true);
        ui->lineEditStudentName->setEnabled(true);
        ui->lineEditSchool->setEnabled(true);

        ui->statusBar->showMessage(tr("Nieuw PDF-bestand geopend"));
        inputfile = fileName;
    }
/*
    DO NOT put an "else". Imagine the scenario that the user has selected a PDF file to cut, than accidentally presses "Open File" again and presses "CANCEL" to correct his error.

    else
    {
        ui->PDFFileNameLabel->setText(tr("...geen PDF gekozen"));
    }
*/
}
