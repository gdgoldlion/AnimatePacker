/*
 * sprites_list.h
 *
 *  Created on: 2012-2-12
 *      Author: goldlion
 */

#ifndef SPRITES_LIST_H_
#define SPRITES_LIST_H_

#include <QtGui>

class SpritesList: public QListWidget {
Q_OBJECT
public:
	SpritesList(QWidget *parent = 0);
	virtual ~SpritesList();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
        void resizeEvent(QResizeEvent *event);
private:
	void performDrag();
	QPoint startPos;
};

#endif /* SPRITES_LIST_H_ */
