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

private:

    QString getWebText();
    void setAllModulesData(QString theWebText);
    QSet<QString>  getAllMatchResults(const QString text, const QString regexp);
    void setEveryModuleData();
    bool setModuleData(QString moduleText);

    void setTankPara();
    void writeIntoFile();

private slots:
    void getDataFromWeb();

};
#endif // FRAMEPUMPTANKDATA_H
