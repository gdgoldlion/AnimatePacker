/*
 * plist_list.cpp
 *
 *  Created on: 2012-3-12
 *      Author: goldlion
 */

#include "plist_list.h"

PlistList::PlistList(QWidget *parent) :
		QListWidget(parent) {
	//添加快捷键，获得焦点时，点delete删除item
	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	shortcut->setContext(Qt::WidgetWithChildrenShortcut);
	connect(shortcut, SIGNAL(activated()), this, SLOT(deleteItem()));
}

PlistList::~PlistList() {
}

void PlistList::deleteItem() {
	if(this->currentItem()){
		emit plistDeleted(this->currentItem()->text());
	}
	QListWidgetItem *item = this->takeItem(this->currentRow());
	delete item;
}
