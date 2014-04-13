/*
 * sprites_list.cpp
 *
 *  Created on: 2012-2-12
 *      Author: goldlion
 */

#include "sprites_list.h"

SpritesList::SpritesList(QWidget *parent ):QListWidget(parent) {

}

SpritesList::~SpritesList() {
}

void SpritesList::resizeEvent(QResizeEvent *event){
    reset();
}

void SpritesList::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        startPos = event->pos();
    QListWidget::mousePressEvent(event);
}

void SpritesList::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            performDrag();
    }
    QListWidget::mouseMoveEvent(event);
}

void SpritesList::performDrag() {
    QListWidgetItem *item = currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->text());
        mimeData->setImageData(item->icon());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        if (drag->exec(Qt::MoveAction) == Qt::MoveAction){

        }
    }
}
