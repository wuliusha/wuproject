#ifndef CODE39_H
#define CODE39_H

#include <QBitmap>
#include <QWidget>
#include <QMutex>
#include <QLineEdit>
#include <QPushButton>

class Code39 : public QWidget
{
    Q_OBJECT

public:
    QMap<QString,QString> map;

    static Code39 *getInstance();
    bool getCode39(QString barcode);
    bool testCode39(QString barcode);
    void initMap();
    QString CodeBinary(QString &barcode);

private:
    Code39();
    Code39(const Code39&);
    Code39 &operator=(const Code39&);
    ~Code39();
private:
    void paintEvent(QPaintEvent *);
    void draw(QString &barcode);
public:
    QBitmap bmp;
    static Code39 *instance;
    static QMutex mutex;

};

#endif // CODE39_H
