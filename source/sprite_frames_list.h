/*
 * sprite_frames_list.h
 *
 *  Created on: 2012-2-12
 *      Author: goldlion
 */

#ifndef SPRITE_FRAMES_LIST_H_
#define SPRITE_FRAMES_LIST_H_

#include <QtGui>

class SpriteFramesList: public QListWidget {
Q_OBJECT
public:
	SpriteFramesList(QWidget *parent = 0);
	virtual ~SpriteFramesList();

public slots:
	bool swapItem(int row1,int row2);
	void deleteItem();

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);
};

#endif /* SPRITE_FRAMES_LIST_H_ */
