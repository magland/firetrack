#include "diskreadmda.h"

#include <QFile>
#include <QFileInfo>
#include <QVector>
#include "mdaio.h"
#include "usagetracking.h"
#include <math.h>
#include <QDebug>

#define CHUNKSIZE 10000

struct DataChunk {
	float *data;
};

class DiskReadMdaPrivate {
public:
	DiskReadMda *q;
	QVector<DataChunk> m_chunks;
	FILE *m_file;
	int m_header_size;
	int m_num_bytes_per_entry;
	int m_total_size;
	QString m_path;
	int m_data_type;
	int m_size[MDAIO_MAX_DIMS];

	int get_index(int i1,int i2,int i3=0,int i4=0,int i5=0,int i6=0);
	float *load_chunk(int i);
	void clear_chunks();
	void load_header();
	void initialize_contructor();
};

DiskReadMda::DiskReadMda(const QString &path) : QObject()
{
	d=new DiskReadMdaPrivate;
	d->q=this;

	d->initialize_contructor();

	if (!path.isEmpty()) setPath(path);
}

DiskReadMda::DiskReadMda(const DiskReadMda &other) : QObject()
{
	d=new DiskReadMdaPrivate;
	d->q=this;

	d->initialize_contructor();

	setPath(other.d->m_path);
}

void DiskReadMda::operator=(const DiskReadMda &other)
{
	setPath(other.d->m_path);
}

DiskReadMda::~DiskReadMda()
{
	d->clear_chunks();
	if (d->m_file)
		jfclose(d->m_file);
	delete d;
}

void DiskReadMda::setPath(const QString &path)
{
	d->clear_chunks();
	if (d->m_file) jfclose(d->m_file); d->m_file=0;
	d->m_path=path;
	d->load_header();
	d->m_chunks.resize(ceil(d->m_total_size*1.0/CHUNKSIZE));
	for (int i=0; i<d->m_chunks.count(); i++) {
		d->m_chunks[i].data=0;
	}
}

int DiskReadMda::N1() const {return size(0);}
int DiskReadMda::N2() const {return size(1);}
int DiskReadMda::N3() const {return size(2);}
int DiskReadMda::N4() const {return size(3);}
int DiskReadMda::N5() const {return size(4);}
int DiskReadMda::N6() const {return size(5);}

int DiskReadMda::totalSize() const
{
	return d->m_total_size;
}

int DiskReadMda::size(int dim) const {
	if (dim>=MDAIO_MAX_DIMS) return 1;

	return d->m_size[dim];
}

float DiskReadMda::value(int i1, int i2, int i3, int i4, int i5, int i6)
{
	int ind=d->get_index(i1,i2,i3,i4,i5,i6);
	float *X=d->load_chunk(ind/CHUNKSIZE);
	if (!X) {
		qWarning() << "chunk not loaded:" << ind << ind/CHUNKSIZE;
		return 0;
	}
	return X[ind%CHUNKSIZE];
}

float DiskReadMda::value1(int ind)
{
	float *X=d->load_chunk(ind/CHUNKSIZE);
	if (!X) return 0;
	return X[ind%CHUNKSIZE];
}

void DiskReadMda::reshape(int N1, int N2, int N3, int N4, int N5, int N6)
{
	int tot=N1*N2*N3*N4*N5*N6;
	if (tot!=d->m_total_size) {
		printf("Warning: unable to reshape %d <> %d: %s\n",tot,d->m_total_size,d->m_path.toLatin1().data());
		return;
	}
	for (int i=0; i<MDAIO_MAX_DIMS; i++) d->m_size[i]=1;
	d->m_size[0]=N1;
	d->m_size[1]=N2;
	d->m_size[2]=N3;
	d->m_size[3]=N4;
	d->m_size[4]=N5;
	d->m_size[5]=N6;
}

void DiskReadMda::write(const QString &path)
{
	if (path==d->m_path) return;
	if (path.isEmpty()) return;
	if (QFileInfo(path).exists()) {
		QFile::remove(path);
	}
	QFile::copy(d->m_path,path);
}

int DiskReadMdaPrivate::get_index(int i1, int i2, int i3, int i4, int i5, int i6)
{
	int inds[6]; inds[0]=i1; inds[1]=i2; inds[2]=i3; inds[3]=i4; inds[4]=i5; inds[5]=i6;
	int factor=1;
	int ret=0;
	for (int j=0; j<6; j++) {
		ret+=factor*inds[j];
		factor*=m_size[j];
	}
	return ret;
}

float *DiskReadMdaPrivate::load_chunk(int i)
{
	if (i>=m_chunks.count()) {
		qWarning() << "i>=m_chunks.count()\n";
		return 0;
	}
	if (!m_chunks[i].data) {
		m_chunks[i].data=(float *)jmalloc(sizeof(float)*CHUNKSIZE);
		for (int j=0; j<CHUNKSIZE; j++) m_chunks[i].data[j]=0;
		if (m_file) {
			fseek(m_file,m_header_size+m_num_bytes_per_entry*i*CHUNKSIZE,SEEK_SET);
			size_t num;
			if (i*CHUNKSIZE+CHUNKSIZE<=m_total_size) num=CHUNKSIZE;
			else {
				num=m_total_size-i*CHUNKSIZE;
			}
			void *data=jmalloc(m_num_bytes_per_entry*num);
			size_t num_read=jfread(data,m_num_bytes_per_entry,num,m_file);
			if (num_read==num) {
				if (m_data_type==MDAIO_TYPE_BYTE) {
					unsigned char *tmp=(unsigned char *)data;
					float *tmp2=m_chunks[i].data;
					for (size_t j=0; j<num; j++) tmp2[j]=(float)tmp[j];
				}
				else if (m_data_type==MDAIO_TYPE_FLOAT32) {
					float *tmp=(float *)data;
					float *tmp2=m_chunks[i].data;
					for (size_t j=0; j<num; j++) tmp2[j]=(float)tmp[j];
				}
				else if (m_data_type==MDAIO_TYPE_INT16) {
					int16_t *tmp=(int16_t *)data;
					float *tmp2=m_chunks[i].data;
					for (size_t j=0; j<num; j++) tmp2[j]=(float)tmp[j];
				}
				else if (m_data_type==MDAIO_TYPE_INT32) {
					int32_t *tmp=(int32_t *)data;
					float *tmp2=m_chunks[i].data;
					for (size_t j=0; j<num; j++) tmp2[j]=(float)tmp[j];
				}
				else if (m_data_type==MDAIO_TYPE_UINT16) {
					quint32 *tmp=(quint32 *)data;
					float *tmp2=m_chunks[i].data;
					for (size_t j=0; j<num; j++) tmp2[j]=(float)tmp[j];
				}
				else {
					printf("Warning: unexpected data type: %d\n",m_data_type);
				}
				jfree(data);
			}
			else {
				qWarning() << "Problem reading from mda 212" << num_read << num << m_path;
			}
		}
		else qWarning() << "File is not open!";
	}
	return m_chunks[i].data;
}

void DiskReadMdaPrivate::clear_chunks()
{
	for (int i=0; i<m_chunks.count(); i++) {
		if (m_chunks[i].data) jfree(m_chunks[i].data);
	}
	m_chunks.clear();
}

void DiskReadMdaPrivate::load_header()
{
	if (m_file) {
		printf("Unexpected problem in load_header\n");
		return;
	}
	if (m_path.isEmpty()) {
		return;
	}

	if (m_path.contains("/spikespy-server?")) {

	}

	m_file=jfopen(m_path.toLatin1().data(),"rb");
	if (!m_file) {
		printf("Unable to open file %s\n",m_path.toLatin1().data());
		return;
	}
	MDAIO_HEADER HH;
	mda_read_header(&HH,m_file);
	m_data_type=HH.data_type;
	m_num_bytes_per_entry=HH.num_bytes_per_entry;
	m_total_size=1;
	for (int i=0; i<MDAIO_MAX_DIMS; i++) {
		m_size[i]=HH.dims[i];
		m_total_size*=m_size[i];
	}
	m_header_size=HH.header_size;
}

void DiskReadMdaPrivate::initialize_contructor()
{
	m_file=0;
	m_header_size=0;
	m_num_bytes_per_entry=0;
	m_total_size=0;
	m_data_type=MDAIO_TYPE_FLOAT32;
	for (int i=0; i<MDAIO_MAX_DIMS; i++) m_size[i]=1;
}
