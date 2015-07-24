#ifndef DISKWRITEMDA_H
#define DISKWRITEMDA_H

#include <QObject>
#include "diskreadmda.h"
#include "mdaio.h"

class DiskWriteMdaPrivate;

class DiskWriteMda : public QObject
{
public:
	friend class DiskWriteMdaPrivate;
	DiskWriteMda(const QString &path="");
	~DiskWriteMda();

	void setPath(const QString &path);
	void useTemporaryFile();

	void setDataType(int data_type);
	void allocate(int N1,int N2,int N3=1,int N4=1,int N5=1,int N6=1);
	void setValue(float val,int i1,int i2,int i3=0,int i4=0,int i5=0,int i6=0);
	void setValues(float *vals,int i1,int i2,int i3=0,int i4=0,int i5=0,int i6=0);
	DiskReadMda toReadMda();

private:
	DiskWriteMdaPrivate *d;
};

#endif // DISKWRITEMDA_H
