/*
 * animations_list.h
 *
 *  Created on: 2012-2-12
 *      Author: goldlion
 */

#ifndef ANIMATIONS_LIST_H_
#define ANIMATIONS_LIST_H_

#include <QtGui>

class AnimationsTable: public QTableWidget {
Q_OBJECT
public:
	AnimationsTable(QWidget *parent = 0);
	virtual ~AnimationsTable();

signals:
	void animationSelected(int id);
	void animationDeleted(int id);

public slots:
	void changeItem();
	void deleteItem();
};

#endif /* ANIMATIONS_LIST_H_ */
