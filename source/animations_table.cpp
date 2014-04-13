/*
 * animations_list.cpp
 *
 *  Created on: 2012-2-12
 *      Author: goldlion
 */

#include "animations_table.h"

AnimationsTable::AnimationsTable(QWidget *parent) :
		QTableWidget(parent) {
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(changeItem()));

	//均分列宽
	horizontalHeader()->setResizeMode(QHeaderView::Stretch);

	//添加快捷键，获得焦点时，点delete删除item
	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
	shortcut->setContext(Qt::WidgetWithChildrenShortcut);
	connect(shortcut, SIGNAL(activated()), this, SLOT(deleteItem()));

    setSelectionBehavior ( QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    setSelectionMode ( QAbstractItemView::SingleSelection); //设置选择模式，选择单行
}

AnimationsTable::~AnimationsTable() {
}

void AnimationsTable::changeItem() {
	emit animationSelected(this->currentRow());
}

void AnimationsTable::deleteItem() {
	emit animationDeleted(this->currentRow());
	this->removeRow(this->currentRow());
}
