#ifndef WATERBOARD_H
#define WATERBOARD_H

#include <QMainWindow>

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

    void on_pushButtonOpenPDF_clicked();

private:
    Ui::WaterBoard *ui;
    QString inputfile;
};

#endif // WATERBOARD_H
