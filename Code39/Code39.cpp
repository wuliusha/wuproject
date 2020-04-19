#include <QLabel>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QBitmap>
#include <QPainter>
#include <QDateTime>
#include "Code39.h"

Code39 *Code39::instance = nullptr;
QMutex Code39::mutex;
Code39::Code39()
    :QWidget(nullptr),bmp(QBitmap())
{
    this->setWindowFlags((windowFlags() & ~Qt::WindowMinMaxButtonsHint ) | Qt::WindowStaysOnTopHint | Qt::Tool);
    this->setWindowModality(Qt::ApplicationModal);
    this->resize(600,200);
}

Code39::~Code39()
{

}

void Code39::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(20,20,this->width()-40,this->height()-30,bmp);
    painter.end();
}

Code39 *Code39::getInstance()
{
    if(instance == nullptr)
    {
        mutex.lock();
        instance = new Code39;
        mutex.unlock();
    }
    return instance;
}

bool Code39::getCode39(QString barcode)
{
    //校验字符是否合法
    if(barcode.isEmpty())
        return false;
    if(!testCode39(barcode))
        return false;
    draw(barcode);
    this->repaint();
    this->setWindowTitle(barcode);
    return true;
}

bool Code39::testCode39(QString barcode)
{
    if(map.isEmpty())
        initMap();
    //校验字符是否合法
    if(barcode.isEmpty())
        return false;
    QString com="0123456789ABCDEFGHIJKLMNOPQISTUVWXWZ+-*/%$. ";//合法字符模版
    for(int i=0;i<barcode.size();i++)
    {
        if(!com.contains(barcode.at(i)))
        {
            QMessageBox msg;
            msg.setText(QStringLiteral("条形码字符不合法！\n合法字符:A~Z 0~9 +-/%$[空格]"));
            msg.exec();
            return false;
        }
    }
    return true;
}
void Code39::initMap()
{
    map.insert("A","110101001011");
    map.insert("B","101101001011");
    map.insert("C","110110100101");
    map.insert("D","101011001011");
    map.insert("E","110101100101");
    map.insert("F","101101100101");
    map.insert("G","101010011011");
    map.insert("H","110101001101");
    map.insert("I","101101001101");
    map.insert("J","101011001101");
    map.insert("K","110101010011");
    map.insert("L","101101010011");
    map.insert("M","110110101001");
    map.insert("N","101011010011");
    map.insert("O","110101101001");
    map.insert("P","101101101001");
    map.insert("Q","101010110011");
    map.insert("R","110101011001");
    map.insert("S","101101011001");
    map.insert("T","101011011001");
    map.insert("U","110010101011");
    map.insert("V","100110101011");
    map.insert("W","110011010101");
    map.insert("X","100101101011");
    map.insert("Y","110010110101");
    map.insert("Z","100110110101");
    map.insert("0","101001101101");
    map.insert("1","110100101011");
    map.insert("2","101100101011");
    map.insert("3","110110010101");
    map.insert("4","101001101011");
    map.insert("5","110100110101");
    map.insert("6","101100110101");
    map.insert("7","101001011011");
    map.insert("8","110100101101");
    map.insert("9","101100101101");
    map.insert("+","100101001001");
    map.insert("-","100101011011");
    map.insert("*","100101101101");
    map.insert("/","100100101001");
    map.insert("%","101001001001");
    map.insert("$","100100100101");
    map.insert(".","110010101101");
    map.insert(" ","100110101101");
}

QString Code39::CodeBinary(QString &barcode)
{
    barcode="*"+barcode+"*";
    QString str="";
    for(int i=0;i<barcode.size();i++)
    {
        str+=map[barcode.at(i)]+"0";//每个字符的二进制逻辑码之间，用“0”隔开（加一条白线）
    }
    return str;
}

void Code39::draw(QString &barcode)
{
    QString BinaryNum = CodeBinary(barcode);

    qreal MAXHEIGHT=140;
    qreal FONTHEIGHT=40;

    //设定条码线条宽度px
    qreal LineWidth=3;
    //预留10px的左右空白；
    qreal MAXWIDTH=LineWidth*BinaryNum.size();
    //计算条线宽度时，精度丢失造成条码右边空白，进行右移居中
    qreal move=(MAXWIDTH-LineWidth*BinaryNum.size())/2;
    QBitmap bmp(MAXWIDTH,MAXHEIGHT+FONTHEIGHT);
    QPainter painter(&bmp);
    QPen white,black;
    //    painter.setRenderHint(QPainter::Antialiasing,true);//弧线圆润
    white.setColor(QColor(255,255,255));
    black.setColor(QColor(0,0,0));
    white.setWidth(LineWidth);
    black.setWidth(LineWidth);

    //背景充填白色
    painter.setPen(white);
    painter.drawRect(0,0,MAXWIDTH,MAXHEIGHT+FONTHEIGHT);
    //画线
    for(int i=0;i<BinaryNum.size();i++)
    {
        if(BinaryNum.at(i)=="1")
            painter.setPen(black);
        else
            painter.setPen(white);
        painter.drawLine(i*LineWidth+LineWidth/2+move,0,i*LineWidth+LineWidth/2+move,MAXHEIGHT);
//        painter.drawLine(QPointF(i*LineWidth+LineWidth/2+move,0),QPointF(i*LineWidth+LineWidth/2+move,MAXHEIGHT));
    }

    //添加底部条形码字符
    painter.setPen(black);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setRenderHint(QPainter::TextAntialiasing,true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing,true);
    QFont font;
    font.setFamily("Impact");
    font.setPixelSize(24);
    font.setWeight(0);
    font.setBold(false);
    painter.setFont(font);
    painter.drawText(MAXWIDTH/3,140,MAXWIDTH,FONTHEIGHT,Qt::AlignBottom,barcode.mid(1,barcode.length()-2));
    this->bmp = bmp;
//    bmp.save(path+QDateTime::currentDateTime().toString("MM-dd-hh-mm-ss-zz")+".bmp");
}
