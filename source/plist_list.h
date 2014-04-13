/*
 * plist_list.h
 *
 *  Created on: 2012-3-12
 *      Author: goldlion
 */

#ifndef PLIST_LIST_H_
#define PLIST_LIST_H_

#include <QtGui>

class PlistList: public QListWidget {
Q_OBJECT
public:
	PlistList(QWidget *parent = 0);
	virtual ~PlistList();

signals:
	void plistDeleted(QString name);

public slots:
	void deleteItem();
};

#endif /* PLIST_LIST_H_ */
