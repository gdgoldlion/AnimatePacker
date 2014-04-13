#include "animate_packer.h"
#include <algorithm>
#include <QtDebug>
#include <QtGui>
#include "algorithm"
#include "sprite_frames_list.h"

using namespace std;

AnimatePacker::AnimatePacker(QWidget *parent) :	QMainWindow(parent) {
    ui.setupUi(this);

    QApplication::setWindowIcon(QIcon(":/image/icon.png"));
    setWindowIcon(QIcon(":/image/icon.png"));

    setAcceptDrops(true);

    nullSpriteFramesList = ui.spriteFramesList;
    currentSpriteFramesList = nullSpriteFramesList;
    isPlaying = false;
    zoom = 1;

    fillBackground(ui.label, QColor(199, 237, 204));
    fillBackground(ui.label_2, QColor(199, 237, 204));
    fillBackground(ui.label_3, QColor(199, 237, 204));
    fillBackground(ui.label_4, QColor(199, 237, 204));
    fillBackground(ui.label_5, QColor(199, 237, 204));

    connect(ui.plistList, SIGNAL(plistDeleted(QString)), this,SLOT(removePlist(QString)));
    connect(ui.animationTable, SIGNAL(animationSelected(int)), this,SLOT(openSpritesFramesList(int)));
    connect(ui.animationTable, SIGNAL(animationDeleted(int)), this,	SLOT(deleteSpritesFramesList(int)));
    connect(ui.animationTable,SIGNAL(itemChanged ( QTableWidgetItem *  )),this,SLOT(  changeAnimationAttribute(QTableWidgetItem* )));
    connect(ui.spritesList,SIGNAL(currentItemChanged (QListWidgetItem* ,QListWidgetItem*)),this,SLOT(changePreviewSpriteFrame(QListWidgetItem * , QListWidgetItem * )));

    connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(createXml()));
    connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(saveXml()));
    connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(loadXml()));
    connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui.actionPlay, SIGNAL(triggered()), this, SLOT(playAnimation()));
    connect(ui.actionStop, SIGNAL(triggered()), this, SLOT(stopAnimation()));
    connect(ui.actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(ui.actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(ui.actionChoose, SIGNAL(triggered()), this, SLOT(chooseFrame()));
    connect(ui.actionDeletePlist, SIGNAL(triggered()), this,SLOT(deletePlist()));
    connect(ui.actionDeleteAnimation, SIGNAL(triggered()), this,SLOT(deleteAnimation()));
    connect(ui.actionDeleteSpriteFrame, SIGNAL(triggered()), this,SLOT(deleteSpriteFrame()));
    connect(ui.actionMoveDown, SIGNAL(triggered()), this,SLOT(moveDownFrame()));
    connect(ui.actionMoveUp, SIGNAL(triggered()), this, SLOT(moveUpFrame()));
    connect(ui.actionAnimation, SIGNAL(triggered()), this,SLOT(createAnimation()));
    connect(ui.actionOpenPlist,SIGNAL(triggered()),this,SLOT(openPlist()));
    connect(ui.actionPalette,SIGNAL(triggered()),this,SLOT(changeBackground()));
    connect(ui.actionCopy,SIGNAL(triggered()),this,SLOT(copyAnimation()));
    connect(ui.actionIconMode,SIGNAL(triggered()),this,SLOT(changeIconMode()));
    connect(ui.actionListMode,SIGNAL(triggered()),this,SLOT(changeListMode()));
}

AnimatePacker::~AnimatePacker() {

}

#define _____CodeBlockAbout_Drop_OpenFile

void AnimatePacker::dragEnterEvent(QDragEnterEvent *event) {
    if (!event->mimeData()->hasUrls())
        return;

    QFileInfo fileInfo(event->mimeData()->urls().at(0).toString());
    QString suffix=fileInfo.suffix();

    if (suffix == "plist") {
        event->acceptProposedAction();
    } else if (suffix == "xml") {
        event->acceptProposedAction();
    }
}

void AnimatePacker::dropEvent(QDropEvent *event) {
    if (!event->mimeData()->hasUrls())
        return;

    QUrl url=event->mimeData()->urls().at(0);
    QString path=url.toLocalFile();
    QFileInfo fileInfo(url.toString());
    QString suffix=fileInfo.suffix();

    if (suffix== "plist") { //添加plist
        ui.plistList->addItem(fileInfo.fileName());

        addPlist(path);

    } else if (suffix == "xml") { //打开新的xml
        createXml(); //清除

        openXml(path);
    }
}

#define _____CodeBlockAbout_Slot

void AnimatePacker::createXml() {
    ui.plistList->clear();
    ui.animationTable->clearContents(); //删除内容
    ui.animationTable->setRowCount(0); //重置行数
    ui.spriteFramesList->clear();
    ui.spritesList->clear();
    ui.imageLabel->setPixmap(QPixmap());
    path.clear();

    spriteNameToImageMap.clear();

    changeSpriteFramesList(nullSpriteFramesList);
}

void AnimatePacker::loadXml() {
    QString path = QFileDialog::getOpenFileName(this, tr("Choose to file"), ".",
                                                tr("Xml Files(*.xml)"));

    if (path.isEmpty()) {
        return;
    }

    createXml(); //清除

    openXml(path);
}

void AnimatePacker::openXml(QString path) {
    this->path = path;

    QFile file(path);
    QFileInfo fileInfo(path);
    QString dir = fileInfo.absolutePath(); //获取xml所在路径

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(
                    this,
                    QObject::tr("Read failure"),
                    QObject::tr("Cannot read file \n%1\n%2").arg(file.fileName()).arg(
                        file.errorString()));
        return; //文件无法打开
    }

    QDomDocument doc;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn)) {
        QMessageBox::warning(
                    this,
                    QObject::tr("Read failure"),
                    QObject::tr("Syntax error in file \n%1\n%2 row %3 column").arg(errorLine).arg(
                        errorColumn));
        return; //加载文件出错
    }

    QDomElement root = doc.documentElement(); //根为root元素

    //plist
    QDomNodeList plistNodeList = root.elementsByTagName("plist");
    for (unsigned int i = 0; i < plistNodeList.length(); i++) {
        QDomElement plistElement = plistNodeList.at(i).toElement();

        ui.plistList->addItem(plistElement.text());

        //读取plist，添加sprite
        QString plistPath = dir + QDir::separator() + plistElement.text();
        addPlist(plistPath);
    }

    //animation
    QDomNodeList animationsNodeList = root.elementsByTagName("animation");
    ui.animationTable->setRowCount(animationsNodeList.length());
    for (unsigned int i = 0; i < animationsNodeList.length(); i++) {
        QDomElement animationElement = animationsNodeList.at(i).toElement();

        QTableWidgetItem *nameItem = new QTableWidgetItem(
                    animationElement.firstChildElement("name").text());
        ui.animationTable->setItem(i, 0, nameItem);

        QTableWidgetItem *delayItem = new QTableWidgetItem(
                    animationElement.firstChildElement("delay").text());
        ui.animationTable->setItem(i, 1, delayItem);

        QTableWidgetItem *flipXItem= new QTableWidgetItem;
        flipXItem->setFlags( flipXItem->flags() &~Qt::ItemIsEditable  );//不可编辑
        bool flipX=animationElement.firstChildElement("flipX").text()=="true"?true:false;
        flipXItem->setCheckState(flipX?Qt::Checked:Qt::Unchecked);//加入单选
        ui.animationTable->setItem (i, 2, flipXItem);

        QTableWidgetItem *flipYItem= new QTableWidgetItem;
        flipYItem->setFlags( flipYItem->flags() &~Qt::ItemIsEditable  );//不可编辑
        bool flipY=animationElement.firstChildElement("flipY").text()=="true"?true:false;
        flipYItem->setCheckState(flipY?Qt::Checked:Qt::Unchecked);//加入单选
        ui.animationTable->setItem (i, 3, flipYItem);

        QDomNodeList spriteFrameNodeList = animationElement.elementsByTagName(
                    "spriteFrame");
        SpriteFramesList *spriteFrameList = new SpriteFramesList(this);
        spriteFramesLists.push_back(spriteFrameList);
        for (unsigned int j = 0; j < spriteFrameNodeList.length(); j++) {
            QDomElement spriteFrameElement = spriteFrameNodeList.at(j).toElement();

            QString spriteName=spriteFrameElement.text();
            QImage image=spriteNameToImageMap[spriteName] ;
            spriteFrameList->addItem(new QListWidgetItem(QIcon(QPixmap::fromImage(image)), spriteName));
        }
    }

    file.close();
}

void AnimatePacker::saveXml() {
    if (path.isNull()) {
        QString tempPath = QFileDialog::getSaveFileName(this,
                                                        tr("Choose to preserve location"), ".", tr("Xml Files(*.xml)"));

        if (tempPath.isEmpty()) {
            return;
        }

        path = tempPath;
    }

    QFile file(path);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(
                    this,
                    QObject::tr("Write failure"),
                    QObject::tr("Cannot write file \n%1\n%2").arg(file.fileName()).arg(
                        file.errorString()));
        return;
    }

    QDomDocument doc;

    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction("xml",
                                                  "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction); //写入文件头

    QDomElement root = doc.createElement("root");
    doc.appendChild(root);

    //plist
    QDomElement plistsElement = doc.createElement("plists");
    root.appendChild(plistsElement);
    for (int i = 0; i < ui.plistList->count(); i++) {
        QDomElement plistElement = doc.createElement("plist");
        plistsElement.appendChild(plistElement);

        //使用文本节点
        QDomText plistText = doc.createTextNode(ui.plistList->item(i)->text());
        plistElement.appendChild(plistText);
    }

    //animation and spriteFrames
    QDomElement animationsElement = doc.createElement("animations");
    root.appendChild(animationsElement);
    for (int i = 0; i < ui.animationTable->rowCount(); i++) {
        QDomElement animationElement = doc.createElement("animation");
        animationsElement.appendChild(animationElement);

        QDomElement nameElement = doc.createElement("name");
        animationElement.appendChild(nameElement);
        //使用文本节点
        QDomText nameText = doc.createTextNode(ui.animationTable->item(i, 0)->text());
        nameElement.appendChild(nameText);

        QDomElement delayElement = doc.createElement("delay");
        animationElement.appendChild(delayElement);
        //使用文本节点
        QDomText delayText = doc.createTextNode(ui.animationTable->item(i, 1)->text());
        delayElement.appendChild(delayText);

        QDomElement flipXElement = doc.createElement("flipX");
        animationElement.appendChild(flipXElement);
        //使用文本节点
        QString flipXStr=ui.animationTable->item(i, 2)->checkState()==Qt::Checked?"true":"false";
        QDomText flipXText = doc.createTextNode(flipXStr);
        flipXElement.appendChild(flipXText);

        QDomElement flipYElement = doc.createElement("flipY");
        animationElement.appendChild(flipYElement);
        //使用文本节点
        QString flipYStr=ui.animationTable->item(i, 3)->checkState()==Qt::Checked?"true":"false";
        QDomText flipYText = doc.createTextNode(flipYStr);
        flipYElement.appendChild(flipYText);

        QListWidget *spriteFramesList = spriteFramesLists[i];
        for (int j = 0; j < spriteFramesList->count(); j++) {
            QDomElement spriteFrameElement = doc.createElement("spriteFrame");
            animationElement.appendChild(spriteFrameElement);

            //使用文本节点
            QDomText spriteFrameText = doc.createTextNode(spriteFramesList->item(j)->text());
            spriteFrameElement.appendChild(spriteFrameText);
        }
    }

    QTextStream out(&file);
    doc.save(out, 4); //输出文件，4表示tab是4个空格
    file.close();
}

void AnimatePacker::openPlist(){
    QString path = QFileDialog::getOpenFileName(this, tr("Choose to file"), ".",
                                                tr("Plist Files(*.plist)"));

    if (path.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(path);
    ui.plistList->addItem(fileInfo.fileName());

    addPlist(path);
}

void AnimatePacker::addPlist(QString path) {
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(
                    this,
                    QObject::tr("Read failure"),
                    QObject::tr("Cannot read file \n%1\n%2").arg(file.fileName()).arg(
                        file.errorString()));
        return; //文件无法打开
    }

    QDomDocument doc;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn)) {
        QMessageBox::warning(
                    this,
                    QObject::tr("Read failure"),
                    QObject::tr("Syntax error in file \n%1\n%2 row %3 column").arg(errorLine).arg(
                        errorColumn));
        return; //加载文件出错
    }

    QDomElement root = doc.documentElement();
    QDomNodeList dictNodeList = root.elementsByTagName("dict");

    //判断HD模式
    bool isHD = !(path.indexOf("-hd", 0)==-1);
    int mulHD = isHD?2:1;

    //读metadata////
    QDomNode metaDataNode=findMetadataDict(root);
    map<QString,QString> metaDataDict=parseDict(metaDataNode);

    //读fromat
    int format=metaDataDict["format"].toInt();

    //加载图片
    QString imageFileName;
    switch(format)
    {
    case 0:
    case 1:
    case 2:
        imageFileName=metaDataDict["realTextureFileName"];
        break;
    case 3:
        QDomNode nameNode=metaDataNode.toElement().elementsByTagName("dict").at(0);
        map<QString,QString> nameDict=parseDict(nameNode);
        imageFileName=nameDict["textureFileName"]+nameDict["textureFileExtension"];
        break;
    }
    QFileInfo fileInfo(path);
    QString imagePath = fileInfo.absolutePath() + QDir::separator() + imageFileName;
    QImage source(imagePath);

    //读frames////
    QDomNode framesNode=dictNodeList.at(1);
    QDomNodeList framesNodeList=framesNode.childNodes();

    for(int i=0;i<framesNodeList.length();i+=2)
    {
        //读frame名字
        QString frameName=framesNodeList.at(i).toElement().text();

        //读frame图片
        map<QString,QString> frameDict=parseDict(framesNodeList.at(i+1));

        qreal x,y,w,h;
        qreal sx,sy,sw,sh;
        bool isRotated;

        switch(format)
        {
        case 0:
            w=frameDict["originalWidth"].toInt();
            h=frameDict["originalHeight"].toInt();
            sx=frameDict["x"].toInt();
            sy=frameDict["y"].toInt();
            sw=frameDict["width"].toInt();
            sh=frameDict["height"].toInt();
            x=w/2-sw/2+frameDict["offsetX"].toInt();
            y=h/2-sh/2+frameDict["offsetY"].toInt();
            isRotated=false;
            break;
        case 1:
        {
            QRect frame = strToRect(frameDict["frame"]);
            QPoint offset = strToPoint(frameDict["offset"]);
            QSize sourceSize = strToSize(frameDict["sourceSize"]);

            sx=frame.x();
            sy=frame.y();
            sw=frame.width();
            sh=frame.height();
            w=sourceSize.width();
            h=sourceSize.height();
            x=w/2-sw/2+offset.x();
            y=h/2-sh/2+offset.y();
            isRotated=false;
            break;
        }
        case 2:
        {
            QRect frame = strToRect(frameDict["frame"]);
            QRect sourceColorRect = strToRect(frameDict["sourceColorRect"]);
            QSize sourceSize = strToSize(frameDict["sourceSize"]);

            w=sourceSize.width();
            h=sourceSize.height();
            x=sourceColorRect.x();
            y=sourceColorRect.y();
            sx=frame.x();
            sy=frame.y();
            sw=frame.width();
            sh=frame.height();
            isRotated=strToBool(frameDict["rotated"]);
            if(isRotated)
            {
                swap(sw,sh);
            }
            break;
        }
        case 3:
        {//TODO
            QRect textureRect = strToRect(frameDict["textureRect"]);
            QRect spriteColorRect = strToRect(frameDict["spriteColorRect"]);
            QSize spriteSourceSize = strToSize(frameDict["spriteSourceSize"]);

            w=spriteSourceSize.width();
            h=spriteSourceSize.height();
            x=spriteColorRect.x();
            y=spriteColorRect.y();
            sx=textureRect.x();
            sy=textureRect.y();
            sw=textureRect.width();
            sh=textureRect.height();
            isRotated=strToBool(frameDict["textureRotated"]);
            break;
        }
        }

        QImage image(w,h, QImage::Format_ARGB32);
//        image.fill(Qt::red);
        QPainter painter(&image);

        painter.translate(x,y);

        if(isRotated)
        {
            painter.translate(sh/2,sw/2);//将原点移动到图片中心
            painter.rotate(-90); //顺时针旋转90度
            painter.translate(-sw/2,-sh/2);//使原点复原
        }

        painter.drawImage(0,0,source, sx,sy,sw,sh);

        spriteNameToImageMap[frameName] = image; //覆盖式插入

        ui.spritesList->addItem(new QListWidgetItem(QIcon(QPixmap::fromImage(image)), frameName));
    }

    file.close();
}

void AnimatePacker::removePlist(QString name) {
    //因为各种原因，我并没有把Plist对应那些Sprite在程序内存中保存，所以移除Plist时，会把该Plist打开，并遍历Sprite删除

    QFileInfo fileInfo(this->path);
    QString path = fileInfo.absolutePath() + QDir::separator() + name; //获取xml所在路径
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(
                    this,
                    QObject::tr("Read failure"),
                    QObject::tr("Cannot read file \n%1\n%2").arg(file.fileName()).arg(
                        file.errorString()));

        return;
    }

    QDomDocument doc;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn)) {
        QMessageBox::warning(
                    this,
                    QObject::tr("Read failure"),
                    QObject::tr("Syntax error in file \n%1\n%2 row %3 column").arg(errorLine).arg(
                        errorColumn));

        return;
    }

    QDomElement root = doc.documentElement();

    QDomNodeList dictNodeList = root.elementsByTagName("dict");
    QDomNodeList dictNodeList2 =
            dictNodeList.at(0).toElement().elementsByTagName("dict");
    QDomNodeList keyNodeList =
            dictNodeList2.at(0).toElement().elementsByTagName("key");

    //连带清除
    for (unsigned int i = 0; i < keyNodeList.length(); i += 6) {
        QString key = keyNodeList.at(i).toElement().text();

        QList<QListWidgetItem*> spriteItemList = ui.spritesList->findItems(key,Qt::MatchExactly); //精确查找匹配
        QListWidgetItem *spriteItem = spriteItemList.at(0);
        ui.spritesList->removeItemWidget(spriteItem);
        delete spriteItem;

        for (unsigned int j = 0; j < spriteFramesLists.size(); j++) {
            QListWidget *spriteFramesList = spriteFramesLists[j];
            QList<QListWidgetItem*> spriteFrameItemList =
                    spriteFramesList->findItems(key, Qt::MatchExactly); //精确查找匹配

            if (!spriteFrameItemList.isEmpty()) {
                QListWidgetItem *spriteFrameItem = spriteFrameItemList.at(0);

                spriteFramesList->removeItemWidget(spriteFrameItem);
                delete spriteFrameItem;
            }
        }
    }

    file.close();
}

void AnimatePacker::deletePlist() {
    ui.plistList->deleteItem();
}

void AnimatePacker::moveDownFrame() {
    if (currentSpriteFramesList == nullSpriteFramesList)
        return;

    int row = currentSpriteFramesList->currentRow();

    if (currentSpriteFramesList->swapItem(row, row + 1))
        currentSpriteFramesList->setCurrentRow(row + 1);
}

void AnimatePacker::moveUpFrame() {
    if (currentSpriteFramesList == nullSpriteFramesList)
        return;

    int row = currentSpriteFramesList->currentRow();

    if (currentSpriteFramesList->swapItem(row, row - 1))
        currentSpriteFramesList->setCurrentRow(row - 1);
}

void AnimatePacker::chooseFrame() {
    if (!ui.spritesList->count())
        return;

    if(ui.spritesList->currentRow()==-1)
        return;

    QString itemName = ui.spritesList->currentItem()->text();

    if (itemName.isEmpty())
        return;

    if (currentSpriteFramesList == nullSpriteFramesList)
        return;

    currentSpriteFramesList->addItem(itemName);
}

void AnimatePacker::resetListViewMode(){
    ui.spritesList->reset();
}

void AnimatePacker::changeListMode(){
    ui.spritesList->setViewMode(QListView::ListMode);
}

void AnimatePacker::changeIconMode(){
    ui.spritesList->setViewMode(QListView::IconMode);
}

void AnimatePacker::deleteSpriteFrame() {
    currentSpriteFramesList->deleteItem();
}

#define _____CodeBlockAbout_Aniamtion

void AnimatePacker::createAnimation() {
    int currentRow = ui.animationTable->rowCount();
    ui.animationTable->setRowCount(currentRow + 1);
    spriteFramesLists.push_back(new SpriteFramesList(this));

    ui.animationTable->selectRow(currentRow);
    //填入缺省值
    ui.animationTable->setItem (currentRow, 0, new QTableWidgetItem(QString("untitle")+QString::number(currentRow)));
    ui.animationTable->setItem (currentRow, 1, new QTableWidgetItem("0.3"));
    QTableWidgetItem *flipXItem= new QTableWidgetItem;
    flipXItem->setFlags( flipXItem->flags() &~Qt::ItemIsEditable  );//不可编辑
    flipXItem->setCheckState(Qt::Unchecked);//加入单选
    ui.animationTable->setItem (currentRow, 2, flipXItem);
    QTableWidgetItem *flipYItem= new QTableWidgetItem;
    flipYItem->setFlags( flipXItem->flags() &~Qt::ItemIsEditable  );//不可编辑
    flipYItem->setCheckState(Qt::Unchecked);//加入单选
    ui.animationTable->setItem (currentRow, 3, flipYItem);

    //	changeSpriteFramesList(currentRow);
}

void AnimatePacker::changeAnimationAttribute(QTableWidgetItem* item){
    changePreviewSpriteFrame(currentSpriteFramesList->currentItem(),0);
}

void AnimatePacker::copyAnimation(){
    int rowCount=ui.animationTable->rowCount();

    if(rowCount==0)
        return;

    if(ui.animationTable->currentRow()==-1)
        return;

    ui.animationTable->setRowCount(rowCount + 1);

    int currentRow=ui.animationTable->currentRow();
    SpriteFramesList *sourceSpriteFrameList=spriteFramesLists[currentRow];

    ui.animationTable->setItem (rowCount, 0, new QTableWidgetItem(QString("untitle")+QString::number(rowCount)));
    QString delay=ui.animationTable->item(currentRow,1)->text();
    ui.animationTable->setItem (rowCount, 1, new QTableWidgetItem(delay));

    QTableWidgetItem *flipXItem= new QTableWidgetItem;
    flipXItem->setFlags( flipXItem->flags() &~Qt::ItemIsEditable  );//不可编辑
    Qt::CheckState flipXCheckState=ui.animationTable->item(currentRow,2)->checkState();
    flipXItem->setCheckState(flipXCheckState);//加入单选
    ui.animationTable->setItem (rowCount, 2, flipXItem);

    QTableWidgetItem *flipYItem= new QTableWidgetItem;
    flipYItem->setFlags( flipXItem->flags() &~Qt::ItemIsEditable  );//不可编辑
    Qt::CheckState flipYCheckState=ui.animationTable->item(currentRow,3)->checkState();
    flipYItem->setCheckState(flipYCheckState);//加入单选
    ui.animationTable->setItem (rowCount, 3, flipYItem);

    SpriteFramesList *destSpriteFramesList=new SpriteFramesList(this);
    spriteFramesLists.push_back(destSpriteFramesList);

    for(int i=0;i<sourceSpriteFrameList->count();i++){
        QListWidgetItem *listWidgetItem=sourceSpriteFrameList->item(i);
        destSpriteFramesList->addItem(new QListWidgetItem(listWidgetItem->icon(), listWidgetItem->text()));
    }

    ui.animationTable->selectRow(rowCount);
}

void AnimatePacker::deleteAnimation() {
    ui.animationTable->deleteItem();
}

void AnimatePacker::playAnimation() {
    int spriteFrameCount = currentSpriteFramesList->count();

    if (!isPlaying && spriteFrameCount) {
        isPlaying = true;
        QTableWidgetItem *delayItem = ui.animationTable->item(
                    ui.animationTable->currentRow(), 1);
        int interval = delayItem->text().toDouble() * 1000;
        timerId = startTimer(interval);
    }
}

void AnimatePacker::stopAnimation() {
    if (isPlaying) {
        isPlaying = false;
        killTimer(timerId);
    }
}

void AnimatePacker::timerEvent(QTimerEvent *event) {
    int row = currentSpriteFramesList->currentRow();
    int count = currentSpriteFramesList->count();

    if (++row >= count)
        row = 0;

    currentSpriteFramesList->setCurrentRow(row);
}

void AnimatePacker::zoomIn() {
    zoom++;

    changePreviewSpriteFrame(currentSpriteFramesList->currentItem(), 0);
}

void AnimatePacker::zoomOut() {
    if (--zoom < 1)
        zoom = 1;

    changePreviewSpriteFrame(currentSpriteFramesList->currentItem(), 0);
}

void AnimatePacker::openSpritesFramesList(int id) {
    changeSpriteFramesList(id);
}

void AnimatePacker::deleteSpritesFramesList(int id) {
    vector<SpriteFramesList*>::iterator spriteFramesItr = std::find(
                spriteFramesLists.begin(), spriteFramesLists.end(),
                spriteFramesLists[id]);

    if (spriteFramesItr != spriteFramesLists.end()) {
        changeSpriteFramesList(nullSpriteFramesList);

        QListWidget *spriteFramesList = *spriteFramesItr;
        spriteFramesLists.erase(spriteFramesItr);
        delete spriteFramesList;
    }
}

void AnimatePacker::changePreviewSpriteFrame(QListWidgetItem * current,
                                             QListWidgetItem * previous) {
    if (current) {
        QImage image = spriteNameToImageMap[current->text()];
        QPixmap pixmap=QPixmap::fromImage(image);

        //缩放处理
        pixmap=pixmap.scaled(image.size() * zoom, Qt::IgnoreAspectRatio, Qt::FastTransformation);

        //通过判断QListWidgetItem的上层QListWidget来确认是否需要调用翻转功能
        if(current->listWidget()!=ui.spritesList){
            //镜像处理
            int currentRow=ui.animationTable->currentRow();
            qreal flipX=ui.animationTable->item(currentRow,2)->checkState()==Qt::Checked?-1:1;
            qreal flipY=ui.animationTable->item(currentRow,3)->checkState()==Qt::Checked?-1:1;
            QTransform matrix;
            matrix.scale(flipX,flipY);
            pixmap=pixmap.transformed(matrix, Qt::FastTransformation);
        }

        ui.imageLabel->setPixmap(pixmap);
    } else {
        ui.imageLabel->clear();
    }
}

void AnimatePacker::changeSpriteFramesList(int id) {
    ui.verticalLayout_3->removeWidget(currentSpriteFramesList);
    currentSpriteFramesList->setHidden(true);
    disconnect(
                currentSpriteFramesList,
                SIGNAL( currentItemChanged (QListWidgetItem* , QListWidgetItem*)),
                this,
                SLOT(changePreviewSpriteFrame(QListWidgetItem * , QListWidgetItem * )));

    if(id==-1){//防止野指针报错
        currentSpriteFramesList = nullSpriteFramesList;
    }else{
        currentSpriteFramesList = spriteFramesLists[id];
    }
    currentSpriteFramesList->setHidden(false);
    ui.verticalLayout_3->addWidget(currentSpriteFramesList);
    connect(
                currentSpriteFramesList,
                SIGNAL( currentItemChanged (QListWidgetItem* , QListWidgetItem*)),
                this,
                SLOT(changePreviewSpriteFrame(QListWidgetItem * , QListWidgetItem * )));

    if (currentSpriteFramesList->count()) {
        currentSpriteFramesList->setCurrentRow(-1);//取消选择,使下一行能个发出信号,但是可能会触发错误
        currentSpriteFramesList->setCurrentRow(0);

        if (isPlaying) {
            stopAnimation();
            playAnimation();
        }
    }else{
        changePreviewSpriteFrame(0,0);
    }
}

void AnimatePacker::changeSpriteFramesList(SpriteFramesList *listWidget) {
    ui.verticalLayout_3->removeWidget(currentSpriteFramesList);
    currentSpriteFramesList->setHidden(true);

    currentSpriteFramesList = listWidget;
    currentSpriteFramesList->setHidden(false);
    ui.verticalLayout_3->addWidget(currentSpriteFramesList);
}

void AnimatePacker::changeBackground(){
    QColor color = QColorDialog::getColor(Qt::white, this);

    if(!color.isValid())
        return;

    fillBackground(ui.imageLabel,color);
}

#define _____CodeBlockAbout_Other

void AnimatePacker::closeEvent(QCloseEvent *event) {
    if (true/*okToContinue()*/) {
        event->accept(); //事件接受
    } else {
        event->ignore(); //事件取消
    }

}

void AnimatePacker::about() {
    QMessageBox::about(this, tr("About"),
                   #if defined(Q_OS_WIN32)
                       tr("<h2>AnimatePacker for Win32 v2.04 Build1</h2>"
                      #elif defined(Q_OS_MAC)
                       tr("<h2>AnimatePacker for Mac v2.04 Build1</h2>"
                      #endif
                          "<p>author:goldlion"
                          "<p>email:gdgoldlion@gmail.com"
                          "<p>qq:233424570"));
}

QDomNode AnimatePacker::findMetadataDict(QDomElement root)
{
    QDomNodeList nodeList=root.firstChild().childNodes();

    for(int i=0;i<nodeList.length();i++)
    {
        if(nodeList.at(i).toElement().text()=="metadata")
        {
            return nodeList.at(i+1);
        }
    }
}

std::map<QString, QString> AnimatePacker::parseDict(QDomNode dictElm)
{
    QDomNodeList children=dictElm.childNodes();

    map<QString, QString> keyToValueMap;

    for(int i=0;i<children.length();i+=2)
    {
        QDomElement keyElm=children.at(i).toElement();
        QDomElement valueElm=children.at(i+1).toElement();

        keyToValueMap[keyElm.text()]=
                valueElm.text().isEmpty()?valueElm.nodeName():valueElm.text();

        qDebug()<<keyElm.text()<<","<<(valueElm.text().isEmpty()?valueElm.nodeName():valueElm.text())<<endl;
    }

    return keyToValueMap;
}

std::vector<int> AnimatePacker::parseStrToIntArray(QString s)
{
    std::vector<int> numbers;
    QString temp;
    for(int i=0;i<s.length();i++)
    {
        QChar c=s.at(i);
        if(c.isNumber()||c=='-')
        {
            temp.append(c);
        }
        else
        {
            if(!temp.isEmpty())
            {
                int ii=temp.toInt();
                numbers.push_back(temp.toInt());
            }
            temp.clear();
        }
    }

    return numbers;
}

QRect AnimatePacker::strToRect(QString rectStr) {
    std::vector<int> numbers=parseStrToIntArray(rectStr);

    return QRect(numbers[0], numbers[1], numbers[2], numbers[3]);
}

QPoint AnimatePacker::strToPoint(QString pointStr){
    std::vector<int> numbers=parseStrToIntArray(pointStr);

    return QPoint(numbers[0],numbers[1]);
}

QSize AnimatePacker::strToSize(QString sizeStr){
    std::vector<int> numbers=parseStrToIntArray(sizeStr);

    return QSize(numbers[0],numbers[1]);
}

bool AnimatePacker::strToBool(QString boolStr)
{
    return boolStr=="true";
}

void AnimatePacker::fillBackground(QWidget *widget, QColor color) {
    widget->setAutoFillBackground(true);

    QPalette palette;
    palette.setColor(QPalette::Background, color);
    widget->setPalette(palette);
}
