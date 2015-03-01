#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    //tableWidget = new QTableWidget;

    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();

    ui->bottomRightDecLineButton->hide();
    ui->pointIntoDecLineButton->hide();
    ui->topLeftDecLineButton->hide();
    ui->winButton->hide();

    MainWindow::adjustSize();
    QDesktopServices::openUrl(QUrl("http://forums.cisco.com/CertCom/game/binary_game_page.htm"));
    loop = false;

    QFile fileIn("maskFile.txt");
    if (!fileIn.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&fileIn);

    while (!in.atEnd()) {
        int t;
        QString s;
        QVector<QVector<int> > v;
        for (int i = 0; i < 8; i++) {
            QVector<int> part;
            for (int j = 0; j < 6; j++) {
                in >> t;
                part.push_back(t);
            }
            v.push_back(part);
        }
        Mask.push_back(v);
        in >> t;
        Number.push_back(t);
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    loop = false;
    MainWindow::close();
}

void MainWindow::on_winButton_clicked()
{
    if (!loop) ui->winButton->setText("Press this to stop");
        else   ui->winButton->setText("Press this to win");

    loop = !loop;
    if (loop) step();
}

void MainWindow::on_firstBinButton_clicked()
{
    ui->label_2->show();
    ui->topLeftDecLineButton->show();
    ui->topLeftDecLineButton->setFocus();

    X[0] = QCursor::pos().x();
    Y[0] = QCursor::pos().y();

    for (int i = 1; i < 8; i++) {
        X[i] = X[i-1] + 5 + 41;
        Y[i] = Y[i-1];
    }
}

void MainWindow::on_topLeftDecLineButton_clicked()
{
    ui->label_3->show();
    ui->bottomRightDecLineButton->show();
    ui->bottomRightDecLineButton->setFocus();
    baseStartX = QCursor::pos().x();
    baseStartY = QCursor::pos().y();
}

void MainWindow::on_bottomRightDecLineButton_clicked()
{
    ui->label_4->show();
    ui->pointIntoDecLineButton->show();
    ui->pointIntoDecLineButton->setFocus();
    baseFinishX = QCursor::pos().x();
    baseFinishY = QCursor::pos().y();
}

void MainWindow::on_pointIntoDecLineButton_clicked()
{
    ui->winButton->show();
    ui->winButton->setFocus();
    numberPosX = QCursor::pos().x();
    numberPosY = QCursor::pos().y();
}


void MainWindow::getBinStan()
{
    for (int i = 0; i < 8; i++) {
        SetCursorPos(X[i], Y[i]);
        if (desktopImage.pixel(X[i], Y[i]) / 1000000 <= 4291) {
            binStan[i] = 0;
        } else {
            binStan[i] = 1;
        }
        pause(20);
    }
}

QPair<int, int> MainWindow::kilMistake(int xx, int yy, int dx, int dy)
{
    int B[8][6];
    int nom = -1;
    int min = 100000000;

    int A[8][6];
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 6; i++) {
            A[j][i] = isWhite(desktopImage.pixel(xx+i,yy+j));
        }
//        qDebug() << A[j][0] << A[j][1] << A[j][2]
//                 << A[j][3] << A[j][4] << A[j][5];
    }
//    qDebug();

    for (int k = 0; k < Number.size(); k++) {
        int kil = 0;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 6; j++) {
                if (i+dx > 7 || i+dx < 0 || j+dy > 5 || j+dy < 0) {
                    B[i][j] = isWhite(desktopImage.pixel(xx+j+dx,yy+i+dy));
                } else {
                    B[i][j] = A[i+dx][j+dy];
                }
                if (B[i][j] != Mask.at(k)[i][j]) kil++;
            }
        }
        if (kil < min) {
            min = kil;
            nom = Number.at(k);
        }
    }
    QPair<int, int> best;
    best.first = min;
    best.second = nom;

    return best;
}

void MainWindow::getNumber(int xx, int yy)
{
    QPair<int, int> t;
    QPair<int, int> best = kilMistake(xx,yy,0,0);

    t = kilMistake(xx,yy,1,0);
    if (t.first < best.first) best = t;
    t = kilMistake(xx,yy,-1,0);
    if (t.first < best.first) best = t;
    t = kilMistake(xx,yy,0,1);
    if (t.first < best.first) best = t;
    t = kilMistake(xx,yy,0,-1);
    if (t.first < best.first) best = t;
    t = kilMistake(xx,yy,1,-1);
    if (t.first < best.first) best = t;
    t = kilMistake(xx,yy,-1,-1);
    if (t.first < best.first) best = t;

//    qDebug() << best.second;

    n = n * 10 + best.second;
}

void MainWindow::cutToNumber()
{
    /*
    image
                 x first point
        --------->
       |
       |
       |
       |
       |
     y \/
      second point
      */
    n = 0;
    startX = baseStartX;
    startY = baseStartY;
    finishX = baseFinishX;
    finishY = baseFinishY;

    cutImage();
    getBinStan();

    if ((finishX-startX+1) * (finishY - startY + 1) == 1) {
        n = -1;
        return;
    }

    //tableWidget->setColumnCount(finishX - startX + 1);
    //tableWidget->setRowCount(finishY - startY + 1);

    /*for (int i = startX; i <= finishX; i++) {
        for (int j = startY; j <= finishY; j++) {
            QColor pixel = desktopImage.pixel(i,j);
            QTableWidgetItem *widget = new QTableWidgetItem();
            widget->setBackgroundColor(pixel);
            tableWidget->setItem(j-startY,i-startX, widget);
        }
    }*/

    bool startNumber = false;
    for (int i = startX; i <= finishX; i++) {
        bool fl = false;
        for (int j = startY; j <= finishY; j++) {
            QColor pixel = desktopImage.pixel(i,j);
            if (isWhite(pixel)) {
                if (!startNumber) {
                    getNumber(i, startY);
                    startNumber = true;
                }
                fl = true;
            }
        }
        if (!fl) startNumber = false;
    }
}

void MainWindow::step()
{
    int retX = QCursor::pos().x();
    int retY = QCursor::pos().y();
    makeScreen();
    cutToNumber();

    if (n == -1) {
        int t = 0;
        for (int i = 7, d = 1; i >= 0; i--) {
            t += d * binStan[i];
            d *= 2;
        }
        clickDec(t);
    } else {
        int t = n;
        QString s = "";
        for (int i = 7, d = 128; i >= 0; i--) {
            if (t >= d) {
                s += "1";
                t -= d;
            } else {
                s += "0";
            }
            d /= 2;
        }
        clickBin(s);
    }

    SetCursorPos(retX, retY);
    pause(350);

    if (loop) step();
}

void MainWindow::cutImage()
{
    while (startY < finishY) {
        bool fl = false;
        for (int i = startX; i <= finishX; i++) {
            QColor pixel = desktopImage.pixel(i,startY);
            if (isWhite(pixel)) fl = true;
        }
        if (fl) break;
        startY++;
    }
    while (finishY > startY) {
        bool fl = false;
        for (int i = startX; i <= finishX; i++) {
            QColor pixel = desktopImage.pixel(i,finishY);
            if (isWhite(pixel)) fl = true;
        }
        if (fl) break;
        finishY--;
    }
    while (startX < finishX) {
        bool fl = false;
        for (int j = startY; j <= finishY; j++) {
            QColor pixel = desktopImage.pixel(startX, j);
            if (isWhite(pixel)) fl = true;
        }
        if (fl) break;
            startX++;
    }
    while (finishX > startX) {
        bool fl = false;
        for (int j = startY; j <= finishY; j++) {
            QColor pixel = desktopImage.pixel(finishX, j);
            if (isWhite(pixel)) fl = true;
        }
        if (fl) break;
        finishX--;
    }
}
void MainWindow::clickBin(QString s) {
    for (int i = 0; i < s.length(); i++) {
        if ((int)s[i].toAscii()-48 != binStan[i]) {
            clickMouse(X[i], Y[i]);
        }
    }
}

void MainWindow::clickDec(int t)
{
    clickMouse(numberPosX, numberPosY);
    pause(50);
    QString s = QString::number(t);
    for (int i = 0; i < s.length(); i++) {
        keybd_event((int)s[i].toAscii(),0,0,0);
        pause(50);
    }
    keybd_event(VK_RETURN,0,0,0);
    pause(50);
}

void MainWindow::pause(int t)
{
    QTime time;
    time.start();
    for(;time.elapsed() < t;) QApplication::processEvents();
}

void MainWindow::makeScreen()
{
    QPixmap desktopPixMap = QPixmap::grabWindow(QApplication::desktop()->winId());
    desktopImage = desktopPixMap.toImage();
}

void MainWindow::clickMouse(int x, int y)
{
    SetCursorPos(x, y);
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
}

int MainWindow::isWhite(QColor pixel) {
    return (pixel.red() == 255 && pixel.green() == 255 && pixel.blue() == 255);
}
