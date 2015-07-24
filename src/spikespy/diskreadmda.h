#ifndef DISKREADMDA_H
#define DISKREADMDA_H

#include "diskreadmda.h"
#include <QString>
#include <QObject>

class DiskReadMdaPrivate;

class DiskReadMda : public QObject
{
	Q_OBJECT
public:
	friend class DiskReadMdaPrivate;
	explicit DiskReadMda(const QString &path="");
	DiskReadMda(const DiskReadMda &other);
	void operator=(const DiskReadMda &other);
	~DiskReadMda();

	void setPath(const QString &path);

	int N1() const;
	int N2() const;
	int N3() const;
	int N4() const;
	int N5() const;
	int N6() const;
	int totalSize() const;
	int size(int dim) const;
	float value(int i1,int i2,int i3=0,int i4=0,int i5=0,int i6=0);
	float value1(int ii);
	void reshape(int N1,int N2,int N3=1,int N4=1,int N5=1,int N6=1);
	void write(const QString &path);

private:
	DiskReadMdaPrivate *d;
};

#endif // DISKREADMDA_H
