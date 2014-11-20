#include <QtNetwork>
#include <QXmlStreamReader>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QtXml/QDomNodeList>
#include <QDebug>
#include "MainWindow.h"
using namespace std;


QString patterModulesData = "<div class=\"b-wiki-panel\"><div class=\"b-modules\"><div class=\"b-modules_header\"><img src=\"http://wikicdn\\.wargaming\\.net/images/c/ca/Module_wot_turret\\.png\" alt=\"turret\" class=\"b-modules_img\"><img src=\"http://wikicdn\\.wargaming\\.net/images/8/8f/Module_wot_gun\\.png\" alt=\"img\" class=\"b-modules_img b-modules_img__small\">[^\r\n]+";
QString splitEachModuleData = "<table class=\"t-modules\">.*</table>";
//总的函数，负责从网上获得文本
//
void MainWindow::getDataFromWeb()
{

    QString webText = getWebText();
    setAllModulesData(webText);
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

}

void MainWindow::setTankPara()
{}
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
QString MainWindow::getWebText()
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
    QString outputText = reply->readAll();

    return outputText;
}
//获得含有坦克所有配件参数文本块，存入allModulesText类属性里面
void MainWindow::setAllModulesData(QString theWebText)
{
    QString pattern=patterModulesData;
    //qDebug()<<pattern;
    QRegExp rx(pattern);
    int pos = theWebText.indexOf(rx);
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

    QRegExp rx(splitEachModuleData);
    rx.setMinimal(true);
    int pos = allModulesText.indexOf(rx);

    QSet<QString> allResults = getAllMatchResults(allModulesText,splitEachModuleData);
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
