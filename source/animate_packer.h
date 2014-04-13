#ifndef ANIMATE_PACKER_H
#define ANIMATE_PACKER_H

#include <vector>
#include <map>
#include <QtGui>
#include <QtXml>
#include "ui_animate_packer.h"

class AnimatePacker: public QMainWindow {
    Q_OBJECT
public:
    AnimatePacker(QWidget *parent = 0);
    ~AnimatePacker();

public:
    static QDomNode findMetadataDict(QDomElement root);
    static std::map<QString, QString> parseDict(QDomNode dictElm);
    static std::vector<int> parseStrToIntArray(QString s);
    static QRect strToRect(QString rectStr);
    static QPoint strToPoint(QString pointStr);
    static QSize strToSize(QString sizeStr);
    static bool strToBool(QString boolStr);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void closeEvent(QCloseEvent *event);
    virtual void timerEvent(QTimerEvent *event);

private slots:
    void about();

    void addPlist(QString path);
    void openPlist();
    void removePlist(QString name);
    void deletePlist();

    void createXml();
    void openXml(QString path);
    void saveXml();
    void loadXml();

    void createAnimation();
    void copyAnimation();
    void deleteAnimation();
    void playAnimation();
    void stopAnimation();
    void zoomIn();
    void zoomOut();
    void changeAnimationAttribute(QTableWidgetItem* item);

    void chooseFrame();
    void resetListViewMode();
    void changeListMode();
    void changeIconMode();

    void moveDownFrame();
    void moveUpFrame();

    void deleteSpriteFrame();

    void openSpritesFramesList(int id);
    void deleteSpritesFramesList(int id);
    void changePreviewSpriteFrame(QListWidgetItem * current,QListWidgetItem * previous);

    void changeBackground();
private:
    void changeSpriteFramesList(int id);
    void changeSpriteFramesList(SpriteFramesList *listWidget);

    static void fillBackground(QWidget *widget, QColor color);

    QString path;

    bool isPlaying;
    int timerId;
    int frameId;
    int zoom;
    std::map<QString, QImage> spriteNameToImageMap;

    SpriteFramesList *nullSpriteFramesList;
    SpriteFramesList *currentSpriteFramesList;
    std::vector<SpriteFramesList*> spriteFramesLists;

    Ui::AnimatePackerClass ui;
};

#endif // ANIMATE_PACKER_H
