#ifndef DISKARRAYMODEL_H
#define DISKARRAYMODEL_H

#include <QObject>
#include "mda.h"

class DiskArrayModelPrivate;
class DiskArrayModel : public QObject
{
	Q_OBJECT
public:
	friend class DiskArrayModelPrivate;
	explicit DiskArrayModel(QObject *parent = 0);
	~DiskArrayModel();
	void setPath(QString path);
	void setFromMda(const Mda &X);
	QString path();
	bool fileHierarchyExists();
	void createFileHierarchyIfNeeded();
	Mda loadData(int scale,int t1,int t2);
	float value(int ch,int t);
	int size(int dim);
private:
	DiskArrayModelPrivate *d;

signals:

public slots:
};

#endif // DISKARRAYMODEL_H
