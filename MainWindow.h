#ifndef FRAMEPUMPTANKDATA_H
#define FRAMEPUMPTANKDATA_H
#include "ui_formpumptankdata.h"
#include <QMainWindow>

class MainWindow:public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent=0)
    {
        ui = new Ui::FormPumpTankData;
        ui->setupUi(this);
        connect(ui->buttonWebsitePump,SIGNAL(clicked()),this,SLOT(getDataFromWeb()));
    }

    ~MainWindow()
    {
        delete ui;
    }
    void printSomething(QString str);
private:
    Ui::FormPumpTankData *ui;
    QString allModulesText;
    QStringList tankParameter;


    QMap<QString,QStringList> turrets;
    QMap<QString,QStringList> guns;
    QMap<QString,QStringList> suspensions;
    QMap<QString,QStringList> engines;
    QMap<QString,QStringList> radios;

public:

    //获得某种坦克的整个网页的源代码，以返回值的形式返回
    QString getWebText();
    //获得含有坦克所有配件参数文本块，存入allModulesText类属性里面
    void setAllModulesData(QString theWebText);

    //用table分割坦克不同模块
    //调用getAllMatchResults函数，把坦克的不同模块的文本放入QSet<QString>结构中
    //调用setModuleData设置坦克不同模块的类参数
    void setEveryModuleData();

private:
    void setTankPara();
    void writeIntoFile();
    //把坦克的每个模块的文本放入QSet<QString>中
    QSet<QString>  getAllMatchResults(const QString text, const QString regexp);
    //把所给不同模块的参数填入相应的链表
    bool setModuleData(QString moduleText);
private slots:
    void getDataFromWeb();

};
#endif // FRAMEPUMPTANKDATA_H
