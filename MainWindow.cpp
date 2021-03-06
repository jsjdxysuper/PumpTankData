#include <QtNetwork>
#include <QXmlStreamReader>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QtXml/QDomNodeList>
#include <QDebug>
#include "MainWindow.h"
using namespace std;


QString ALLMODULETEXTPATTERN = "<div class=\"b-wiki-panel\"><div class=\"b-modules\"><div class=\"b-modules_header\"><img src=\"http://wikicdn\\.wargaming\\.net/images/c/ca/Module_wot_turret\\.png\" alt=\"turret\" class=\"b-modules_img\"><img src=\"http://wikicdn\\.wargaming\\.net/images/8/8f/Module_wot_gun\\.png\" alt=\"img\" class=\"b-modules_img b-modules_img__small\">[^\r\n]+";
QString SPLITEACHMODULEPATTERN = "<table class=\"t-modules\">.*</table>";

QString TOPHPPATTERN = "\\d+(?=</span><span style=\"color:gray\"> HP</span> </span><span class=\"t-performance_left\"> Hit Points)";
QString STOCKHPPATTERN="\\d+(?=</span><span class=\"top\">\\d+</span><span style=\"color:gray\"> HP</span> </span><span class=\"t-performance_left\"> Hit Points)";
QString UPANGLEPATTERN = "\\d*(,\\d{3})*(?=<span style=\"color:gray\">°</span></span> </span><span class=\"t-performance_left\"> Elevation Arc)";
QString DOWNANGLEPATTERN ="\\d*(,\\d{3})*(?=<span style=\"color:gray\">°</span>/\\+\\d+<span style=\"color:gray\">°</span></span> </span><span class=\"t-performance_left\"> Elevation Arc)";
QString LEFTRIGHTANGLEPATTERN = "\\d*(?=<span style=\"color:gray\">°</span> </span><span class=\"t-performance_left\"> Gun Arc)";
QString HULLARMORPATTERN = "\\d{1,3}/\\d{1,3}/\\d{1,3}(?=<span style=\"color:gray\"> mm</span> </span><span class=\"t-performance_left\"> Hull Armor)";
QString TOPTURRETARMORPATTERN = "\\d{1,3}/\\d{1,3}/\\d{1,3}(?=</span><span style=\"color:gray\"> mm</span> </span><span class=\"t-performance_left\"> Turret Armor)";
QString STOCKTURRETARMORPATTERN = "\\d{1,3}/\\d{1,3}/\\d{1,3}(?=</span><span class=\"top\">\\d{1,3}/\\d{1,3}/\\d{1,3}</span><span style=\"color:gray\"> mm</span> </span><span class=\"t-performance_left\"> Turret Armor)";
QString GOLDCOSTPATTERN = "(?<=<p><span class=\"b-description-img_price b-description-img_price&#95;_gold\">)\\d{1,7}";
QString SILVERCOSTPATTERN = "(?<=<p><span class=\"b-description-img_price\">)\\d{1,7}";
QString STATIONCAMOUFLAGEPATTERN = "(?<=<ul class=\"b-standart-list\"><li>Stationary: )\\d{0,2}.*\\d{0,2}(?=%)";
QString MOVINGCAMOUFLAGEPATTERN = "(?<=<li>When Moving: )\\d{0,2}.*\\d{0,2}(?=%)";
QString FIRINGCAMOUFLAGEPATTERN = "(?<=<li>When Firing: )\\d{0,2}.*\\d{0,2}(?=%)";
QString THRUSTWEIGHTRATIOPATTERN = "(?<=<span class=\"top\">)\\d{1,4}.*\\d{0,4}(?=</span><span style=\"color:gray\"> hp/t</span> </span><span class=\"t-performance_left\"> Power/Wt Ratio </span></td>)";
QString SPEEDPATTERN = "\\d{1,4}/\\d{0,4}(?=<span style=\"color:gray\"> km/h</span> </span><span class=\"t-performance_left\"> Speed Limit)";
//总的函数，负责从网上获得文本
//
void MainWindow::getDataFromWeb()
{

    setWebText();
    setAllModulesData();
    setEveryModuleData();

    qDebug()<<QString("炮塔");
    qDebug()<<turrets;
    qDebug()<<QString("火炮");
    qDebug()<<guns;
    qDebug()<<QString("发动机");
    qDebug()<<engines;
    qDebug()<<QString("无线电");
    qDebug()<<radios;
    qDebug()<<QString("履带");
    qDebug()<<suspensions;

    setTankPara();
}

void MainWindow::setTankPara()
{
    //获取顶级生命值
    qDebug()<<TOPHP;
    QRegExp rx(TOPHPPATTERN);
    int pos = webText.indexOf(rx);
    //qDebug()<<rx.capturedTexts();
    if(pos>=0)
    {
        QStringList temp = rx.capturedTexts();
        QString topHP = temp.takeFirst();
        tankParameter[TOPHP] = topHP;
        //qDebug()<<allModulesText;
        printSomething(tankParameter[TOPHP]);
    }
    //获取坦克初始生命值
    rx.setPattern(STOCKHPPATTERN);
    pos = webText.indexOf(rx);
    qDebug()<<STOCKHPPATTERN;
    if(pos>=0)
    {
        QStringList temp = rx.capturedTexts();
        QString stockHP = temp.takeFirst();
        tankParameter[STOCKHP]=stockHP;
        printSomething(tankParameter[STOCKHP]);

    }
    //获取坦克仰角
    rx.setPattern(UPANGLEPATTERN);
    pos = webText.indexOf(rx);
    qDebug()<<UPANGLEPATTERN;
    QString up_angle;
    if(pos>=0)
    {
        QStringList temp = rx.capturedTexts();
        up_angle = temp.takeFirst();
    }
    //获取坦克俯角
    rx.setPattern(DOWNANGLEPATTERN);
    pos = webText.indexOf(rx);
    qDebug()<<DOWNANGLEPATTERN;
    QString down_angle;
    if(pos>=0)
    {
        QStringList temp = rx.capturedTexts();
        down_angle = temp.takeFirst();
    }
    tankParameter[UP_DOWN_ANGLE]=up_angle+"\\"+down_angle;
    printSomething(tankParameter[UP_DOWN_ANGLE]);
}
void MainWindow::writeIntoFile()
{
    QString outputFile;
    outputFile = ui->lineEditOutFile->text();
    QFile file(outputFile);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    //out<<outputText<<endl;
}
//获得某种坦克的整个网页的源代码，以返回值的形式返回
void MainWindow::setWebText()
{
    QString webStr;
    webStr = ui->lineEditWebsite->text();
    QUrl url(webStr);
    QNetworkAccessManager manager;
    QEventLoop loop;
    qDebug() << "Reading code form " << webStr;
    //发出请求
    QNetworkReply *reply = manager.get(QNetworkRequest(url));
    //请求结束并下载完成后，退出子事件循环
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    //开启子事件循环
    loop.exec();

    webText = reply->readAll();
}
//获得含有坦克所有配件参数文本块，存入allModulesText类属性里面
void MainWindow::setAllModulesData()
{
    QString pattern=ALLMODULETEXTPATTERN;
    //qDebug()<<pattern;
    QRegExp rx(pattern);
    int pos = webText.indexOf(rx);
    //qDebug()<<rx.capturedTexts();
    if(pos>=0)
    {
        QStringList temp = rx.capturedTexts();
        allModulesText = temp.takeFirst();
        printSomething(allModulesText);
        //qDebug()<<allModulesText;
    }
}
//用table分割坦克不同模块
//调用getAllMatchResults函数，把坦克的不同模块的文本放入QSet<QString>结构中
//调用setModuleData设置坦克不同模块的类参数
void MainWindow::setEveryModuleData()
{

    QRegExp rx(SPLITEACHMODULEPATTERN);
    rx.setMinimal(true);
    int pos = allModulesText.indexOf(rx);

    QSet<QString> allResults = getAllMatchResults(allModulesText,SPLITEACHMODULEPATTERN);
    QSet<QString>::iterator iter = allResults.begin();
    while(iter!= allResults.end())
    {
        qDebug()<<*iter;
        setModuleData(*iter);
        iter++;
    }
    int i;
}

//把所给不同模块的参数填入相应的链表
bool MainWindow::setModuleData(QString moduleText)
{
    QDomDocument doc;
    doc.setContent(moduleText);
    QDomElement root = doc.documentElement();

    if(root.tagName() != "table"||root.text()=="")
    {
        qDebug()<<"error:it is a null element or not a table Element"<<endl;
        return false;
    }
    QDomNodeList childs = root.childNodes();
    QDomNodeList gradeChilds = childs.item(0).childNodes();
    QString moduleName = gradeChilds.item(1).toElement().text();

    for(int i=1;i<childs.length();i++)
    {
        QDomNodeList eachGradeChilds = childs.item(i).childNodes();
        QStringList tempModule;
        for(int j=0;j<eachGradeChilds.length();j++)
        {
            QString para = eachGradeChilds.item(j).toElement().text();
            tempModule.append(para);

        }
        qDebug()<<tempModule;
        if(moduleName.compare("turret",Qt::CaseInsensitive)==0)
        {
            this->turrets.insert(tempModule.takeAt(1),tempModule);
        }
        else if(moduleName.compare("gun",Qt::CaseInsensitive)==0)
        {
            this->guns.insert(tempModule.takeAt(1),tempModule);
        }
        else if(moduleName.compare("engine",Qt::CaseInsensitive)==0)
        {
            this->engines.insert(tempModule.takeAt(1),tempModule);
        }
        else if(moduleName.compare("suspension",Qt::CaseInsensitive)==0)
        {
            this->suspensions.insert(tempModule.takeAt(1),tempModule);
        }
        else if(moduleName.compare("radio",Qt::CaseInsensitive)==0)
        {
            this->radios.insert(tempModule.takeAt(1),tempModule);
        }
        else{
            return false;
        }
    }
    return true;
}
//把坦克的每个模块的文本放入QSet<QString>中
QSet<QString> MainWindow::getAllMatchResults(const QString text, const QString regexp)
{
    QSet<QString> resultSet;
    QRegExp rx(regexp);
    rx.setMinimal(true);
    int pos = 0;
    while ((pos = rx.indexIn(text, pos)) != -1)
    {
        pos += rx.matchedLength();
        QString result = rx.cap(0);
        resultSet << result;
    }
    return resultSet;
}


void MainWindow::printSomething(QString str)
{
    ui->textEditProcess->append(str);
}
