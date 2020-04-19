#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_25->setText(QDateTime::currentDateTime().toString("dd hh:mm::ss"));
    if(Code39::getInstance()->getCode39("55555555"))
    {
        ui->label_9->setPixmap(Code39::getInstance()->bmp);
        ui->label_26->setPixmap(Code39::getInstance()->bmp);
        qDebug()<<"bmp=: "<<Code39::getInstance()->bmp;
    }
    threadServerI=new threadServer;
    threadServerItem=new QThread();
    threadServerI->moveToThread(threadServerItem);
    threadServerItem->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_Print_Button_clicked()
{
//    QPrinterInfo获取所有打印机QPrinterInfo::availablePrinterNames();
//    QPrinter选择打印机setPrinterName(printer);
    QPrinterInfo PrinterInfo;
    QString  PrinterName = PrinterInfo.defaultPrinterName(); // 默认打印机名字
    QStringList PrinterNameList = PrinterInfo.availablePrinterNames(); // 各个打印机名字
    QPrinter::DuplexMode mode = PrinterInfo.defaultDuplexMode();// 返回此打印机的默认双面打印模式。
    qDebug()<<"PrinterName:="<<PrinterName;

    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName(PrinterName);//指定打印机名

    QFont font;
    font.setPointSize(16);   //设置字体大小

    //自定义纸张大小
    printer.setPageSize(QPrinter::Custom);
    printer.setPaperSize(QSizeF(ui->widget->width()/3,ui->widget->height()*0.3),
                         QPrinter::Point);

    printer.setOrientation(QPrinter::Portrait);
    //获取界面的图片
    QPixmap pixmap = QPixmap::grabWidget(ui->widget, ui->widget->rect());
    QPainter painterPixmap(ui->widget);
    //painterPixmap.setViewport(5,-10,500,300);
    painterPixmap.begin(&printer);

    painterPixmap.setFont(font);

    QRect rect = painterPixmap.viewport();
    int x = rect.width() / pixmap.width();
    int y = rect.height() / pixmap.height();
    painterPixmap.scale(1,1);//设置缩放比例
    painterPixmap.drawPixmap(5, -30, pixmap);//调整打印位置坐标
    painterPixmap.end();

    qDebug()<<x<<y<<ui->widget->rect();
  //  ui->stackedWidget->render(&printer);
}

void MainWindow::on_PreviewButton_clicked()
{
    QPrinter printer(QPrinter::HighResolution);
    //自定义纸张大小
    printer.setPageSize(QPrinter::Custom);
    printer.setPaperSize(QSizeF(ui->widget->width()/3+30,ui->widget->height()*0.3+30),
                         QPrinter::Point);
    QPrintPreviewDialog preview(&printer, this);
    preview.setMinimumSize(600,300);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreviewSlot(QPrinter*)));
    preview.exec ();

}

void MainWindow::printPreviewSlot(QPrinter *printerPixmap)
{
    printerPixmap->setOrientation(QPrinter::Portrait);
    //获取界面的图片
    QPixmap pixmap = QPixmap::grabWidget(ui->widget, ui->widget->rect());
    QPainter painterPixmap(this);
    //painterPixmap.setViewport(5,-30,500,300);
    painterPixmap.begin(printerPixmap);
    QRect rect = painterPixmap.viewport();
    int x = rect.width() / pixmap.width();
    int y = rect.height() / pixmap.height();
    qDebug()<<x<<y<<ui->widget->rect();
    painterPixmap.scale(1, 1);
    painterPixmap.drawPixmap(20, 20, pixmap);
    painterPixmap.end();

}
