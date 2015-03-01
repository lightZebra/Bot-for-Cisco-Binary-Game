#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Windows.h"
#include <QVector>
#include <QtGui>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_firstBinButton_clicked();
    void on_topLeftDecLineButton_clicked();
    void on_bottomRightDecLineButton_clicked();
    void on_pointIntoDecLineButton_clicked();
    void on_winButton_clicked();

private:
    QVector<QVector<QVector<int> > > Mask;
    //QTableWidget *tableWidget;
    QVector<int> Number;
    QImage desktopImage;

    QPair<int, int> kilMistake(int xx, int yy, int dx, int dy);
    void getNumber(int xx, int yy);
    void closeEvent(QCloseEvent *);
    void clickMouse(int x, int y);
    void clickBin(QString s);
    void clickDec(int t);
    void pause(int t);
    void makeScreen();
    void getBinStan();
    void cutImage();    
    void step();
    void cutToNumber();

    int isWhite(QColor pixel);

    int baseFinishX;
    int baseFinishY;
    int baseStartX;
    int baseStartY;
    int numberPosX;
    int numberPosY;
    int finishX;
    int finishY;
    int startX;
    int startY;

    bool loop;

    int binStan[8];
    int X[8];
    int Y[8];
    int n;
    int k;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
