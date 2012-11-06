#ifndef WATERBOARD_H
#define WATERBOARD_H

#include <QMainWindow>
#include <QDir>
#include <QListWidgetItem>


namespace Ui {
class WaterBoard;
}

class WaterBoard : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit WaterBoard(QWidget *parent = 0);
    ~WaterBoard();
    
private slots:
    void on_pushButtonWatermarkPDF_clicked();

    void on_pushButtonAddPDF_clicked();

    void on_pushButtonAddDir_clicked();

    void on_pushButtonRemovePDF_clicked();

    void on_listOfPDFs_itemClicked(QListWidgetItem *item);

    void on_pushButtonRemoveAll_clicked();

private:
    Ui::WaterBoard *ui;

    void ListFiles(QDir directory, QString fileExtension);
};

#endif // WATERBOARD_H
