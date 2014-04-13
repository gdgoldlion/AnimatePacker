/*
 * sprite_frames_list.cpp
 *
 *  Created on: 2012-2-12
 *      Author: goldlion
 */

#include "sprite_frames_list.h"
#include "sprites_list.h"

SpriteFramesList::SpriteFramesList(QWidget *parent) :
		QListWidget(parent) {
	setAcceptDrops(true);

	//支持拖拽排序
	setDragDropMode(QAbstractItemView::InternalMove);

	//添加快捷键，获得焦点时，点delete删除item
	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	shortcut->setContext(Qt::WidgetWithChildrenShortcut);
	connect(shortcut, SIGNAL(activated()), this, SLOT(deleteItem()));

}

SpriteFramesList::~SpriteFramesList() {
}

void SpriteFramesList::dragEnterEvent(QDragEnterEvent *event) {
	SpritesList *source = qobject_cast<SpritesList *>(event->source());
	if (source) {
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}

	QListWidget::dragEnterEvent(event);
}

void SpriteFramesList::dragMoveEvent(QDragMoveEvent *event) {
	SpritesList *source = qobject_cast<SpritesList *>(event->source());
	if (source) {
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}

	QListWidget::dragMoveEvent(event);
}

void SpriteFramesList::dropEvent(QDropEvent *event) {
	SpritesList *source = qobject_cast<SpritesList *>(event->source());
	if (source) {
                const QMimeData *mimeData=event->mimeData();
                QIcon icon=mimeData->imageData().value<QIcon>();;
                QString spriteName=mimeData->text();
                addItem(new QListWidgetItem(icon,spriteName));

		event->setDropAction(Qt::MoveAction);
		event->accept();
	}

	QListWidget::dropEvent(event);
}

bool SpriteFramesList::swapItem(int row1,int row2){
	if(row1<0||row1>=count()||row2<0||row2>=count())
		return false;

	if(row1==row2)
		return false;

	if(row1>row2){
		QListWidgetItem *item1 = this->takeItem(row1);
		this->insertItem(row2,item1);
	}else{
		QListWidgetItem *item2 = this->takeItem(row2);
		this->insertItem(row1,item2);
	}

	return true;
}

void SpriteFramesList::deleteItem() {
	QListWidgetItem *item = this->takeItem(this->currentRow());
	delete item;
}
