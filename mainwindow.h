#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QPrinter>
#include<QFontMetricsF>
#include<QRectF>
#include<QPrintPreviewWidget>
#include<QTextEdit>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPainter>
#include<QPrinterInfo>
#include<QPixmap>
#include "Code39/Code39.h"
#include "threadserver.h"
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
    void on_Print_Button_clicked();
    void on_PreviewButton_clicked();

    void printPreviewSlot(QPrinter *printerPixmap);

private:
    Ui::MainWindow *ui;
    threadServer *threadServerI;
    QThread *threadServerItem;

    QList<QPushButton *> btns;
};

#endif // MAINWINDOW_H
