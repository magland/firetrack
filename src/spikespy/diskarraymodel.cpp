#include "diskarraymodel.h"
#include <stdio.h>
#include <QDebug>
#include <QMap>
#include <QFileInfo>
#include <QByteArray>
#include "sscommon.h"
#include <QDir>
#include <QProgressDialog>
#include <QApplication>
#include <QTime>
#include <QDateTime>
#include "mdaio.h"

class DiskArrayModelPrivate {
public:
	DiskArrayModel *q;
	QString m_path;
	int m_scale_factor;
	int m_chunk_size;
	int m_num_channels;
	int m_num_timepoints;
	int m_data_type;
	QMap<QString,Mda *> m_loaded_chunks;
	Mda m_mda;
	bool m_set_from_mda;
	void read_header();
	Mda *load_chunk(int scale,int chunk_ind);
	Mda *load_chunk_from_file(QString path,int scale,int chunk_ind);
	QVector<float> load_data_from_mda(QString path,int n,int i1,int i2,int i3);
	QString get_code(int scale,int chunk_size,int chunk_ind);
	QString get_multiscale_file_name(int scale);
	bool file_exists(QString path);
};

DiskArrayModel::DiskArrayModel(QObject *parent) : QObject(parent)
{
	d=new DiskArrayModelPrivate;
	d->q=this;

	d->m_set_from_mda=false;
	d->m_scale_factor=4;
	d->m_chunk_size=1;
	d->m_num_channels=1;
	d->m_num_timepoints=0;
	d->m_data_type=MDA_TYPE_REAL;
}

DiskArrayModel::~DiskArrayModel()
{
	foreach (Mda *X,d->m_loaded_chunks) {
		delete X;
	}
	delete d;
}

void DiskArrayModel::setPath(QString path) {
	d->m_path=path;
	d->read_header();

	d->m_chunk_size=qMax(10000/d->m_num_channels,1000);
}

void DiskArrayModel::setFromMda(const Mda &X)
{
	d->m_mda=X;
	d->m_set_from_mda=true;
	d->m_num_channels=X.N1();
	d->m_num_timepoints=X.N2();
	d->m_data_type=MDA_TYPE_REAL;
}

QString DiskArrayModel::path()
{
	return d->m_path;
}

Mda DiskArrayModel::loadData(int scale,int t1,int t2) {
	Mda X; X.setDataType(d->m_data_type);
	int d3=2;
	X.allocate(d->m_num_channels,t2-t1+1,d3);

	if (d->m_set_from_mda) {
		if (scale==1) {
			for (int tt=t1; tt<=t2; tt++) {
				for (int mm=0; mm<d->m_num_channels; mm++) {
					X.setValue(d->m_mda.value(mm,tt),mm,tt-t1,0);
					X.setValue(d->m_mda.value(mm,tt),mm,tt-t1,1);
				}
			}
		}
		else {
			for (int tt=t1; tt<=t2; tt++) {
				for (int mm=0; mm<d->m_num_channels; mm++) {
					float minval=d->m_mda.value(mm,tt*scale);
					float maxval=d->m_mda.value(mm,tt*scale);
					for (int dt=0; dt<scale; dt++) {
						float tmpval=d->m_mda.value(mm,tt*scale+dt);
						if (tmpval<minval) minval=tmpval;
						if (tmpval>maxval) maxval=tmpval;
					}
					X.setValue(d->m_mda.value(mm,tt),mm,tt-t1,minval);
					X.setValue(d->m_mda.value(mm,tt),mm,tt-t1,maxval);
				}
			}
		}
		return X;
	}

	int chunk_ind=t1/d->m_chunk_size;
	for (int i=chunk_ind; i*d->m_chunk_size<=t2; i++) {
		int tA1,tB1;
		int tA2,tB2;
		if (i>chunk_ind) {
			tA1=0; //where it's coming from
		}
		else {
			tA1=t1-i*d->m_chunk_size; //where it's coming from
		}

		if ((i+1)*d->m_chunk_size<=t2) {
			tB1=d->m_chunk_size-1; //where it's coming from
		}
		else {
			tB1=t2-i*d->m_chunk_size; //where it's coming from
		}
		tA2=tA1+i*d->m_chunk_size-t1; //where it's going to
		tB2=tB1+i*d->m_chunk_size-t1; //where it's going to
		Mda *C=d->load_chunk(scale,i);
		if (!C) return X;
		for (int dd=0; dd<d3; dd++) {
			for (int tt=tA2; tt<=tB2; tt++) {
				for (int ch=0; ch<d->m_num_channels; ch++) {
					X.setValue(C->value(ch,tt-tA2+tA1,dd),ch,tt,dd);
				}
			}
		}
	}

	return X;

}
float DiskArrayModel::value(int ch,int t) {
	if (d->m_set_from_mda) {
		return d->m_mda.value(ch,t);
	}
	Mda tmp=loadData(1,t,t);
	return tmp.value(ch,0);
}

bool DiskArrayModel::fileHierarchyExists() {
	if (d->m_set_from_mda) {
		return true;
	}
	int scale0=1;
	while (d->m_num_timepoints/(scale0*MULTISCALE_FACTOR)>1) scale0*=MULTISCALE_FACTOR;
	QString path0=d->get_multiscale_file_name(scale0);
	return (d->file_exists(path0));
}

bool do_minmax_downsample(QString path1,QString path2,int factor,QProgressDialog *dlg,bool first) {
	FILE *inf=jfopen(path1.toLatin1().data(),"rb");
	if (!inf) {
		qWarning() << "Problem reading data from mda for do_minmax_downsample: " << path1 << factor;
		return false;
	}
	FILE *outf=jfopen(path2.toLatin1().data(),"wb");
	if (!outf) {
		qWarning() << "Problem writing data to mda for do_minmax_downsample: " << path2 << factor;
		jfclose(inf);
		return false;
	}


	MDAIO_HEADER HH;
	mda_read_header(&HH,inf);
	int dim1=HH.dims[0];
	int dim2=HH.dims[1];
	int dim3=HH.dims[2];
	if (first) {
		dim2=dim2*dim3; dim3=1; //I know, it's a hack
	}
	int header_size=HH.header_size;
	int data_type=HH.data_type;

	qint32 dim1b=dim1;
	qint32 dim2b=dim2/factor; //if (dim2b*factor<dim2) dim2b++;
	int remainder=dim2-dim2b*factor;
	qint32 dim3b=2;
	qint32 num_dimsb=3;

	HH.dims[0]=dim1b;
	HH.dims[1]=dim2b;
	HH.dims[2]=dim3b;
	HH.num_dims=num_dimsb;
	mda_write_header(&HH,outf);

	QTime timer; timer.start();

	if (data_type==MDA_TYPE_BYTE) {
		unsigned char *buf=(unsigned char *)jmalloc(sizeof(unsigned char)*factor*dim1);
		for (int kk=0; kk<2; kk++) { //determines min or max
			if (dim3==1) {
				fseek(inf,header_size,SEEK_SET); //need to go back to first part in this case
			}
			for (long i=0; i<dim2b; i++) {
				if ((i%100==0)&&(timer.elapsed()>=500)) {
					timer.start();
					if (dlg->wasCanceled()) {
						QFile::remove(path2);
						return false;
					}
					dlg->setValue((i*50)/dim2b+50*kk);
					qApp->processEvents();
				}
				int numread=mda_read_byte(buf,&HH,factor*dim1,inf);
				//int numread=fread(buf,sizeof(unsigned char),factor*dim1,inf);
				unsigned char vals[dim1]; for (int a=0; a<dim1; a++) vals[a]=0;
				if (numread<factor*dim1) {
					qWarning() << "Warning, unexpected problem in do_minmax_downsample" << kk << i << numread << factor << dim1;
				}
				for (int ch=0; ch<dim1; ch++) {
					vals[ch]=buf[0*dim1+ch];
					for (int j=0; j<numread/dim1; j++) {
						if (kk==0) vals[ch]=qMin(vals[ch],buf[j*dim1+ch]);
						else if (kk==1) vals[ch]=qMax(vals[ch],buf[j*dim1+ch]);
					}
				}
				mda_write_byte(vals,&HH,dim1,outf);
				//fwrite(vals,sizeof(unsigned char),dim1,outf);
			}
			if (remainder>0) {
				int numread=mda_read_byte(buf,&HH,remainder*dim1,inf);
				//int b0=fread(buf,sizeof(unsigned char),remainder*dim1,inf);
				Q_UNUSED(numread);
			}
		}
		jfree(buf);
	}
	else if (data_type==MDA_TYPE_INT32) {
		qint32 *buf=(qint32 *)jmalloc(sizeof(qint32)*factor*dim1);
		for (int kk=0; kk<2; kk++) { //determines min or max
			if (dim3==1) {
				fseek(inf,header_size,SEEK_SET); //need to go back to first part in this case
			}
			for (long i=0; i<dim2b; i++) {
				if ((i%100==0)&&(timer.elapsed()>=500)) {
					timer.start();
					if (dlg->wasCanceled()) {
						QFile::remove(path2);
						return false;
					}
					dlg->setValue((i*50)/dim2b+50*kk);
					qApp->processEvents();
				}
				int numread=mda_read_int32(buf,&HH,factor*dim1,inf);
				//int numread=fread(buf,sizeof(qint32),factor*dim1,inf);
				qint32 vals[dim1]; for (int a=0; a<dim1; a++) vals[a]=0;
				if (numread<factor*dim1) {
					qWarning() << "Warning, unexpected problem in do_minmax_downsample" << kk << i << numread << factor << dim1;
				}
				for (int ch=0; ch<dim1; ch++) {
					vals[ch]=buf[0*dim1+ch];
					for (int j=0; j<numread/dim1; j++) {
						if (kk==0) vals[ch]=qMin(vals[ch],buf[j*dim1+ch]);
						else if (kk==1) vals[ch]=qMax(vals[ch],buf[j*dim1+ch]);
					}
				}
				mda_write_int32(vals,&HH,dim1,outf);
				//fwrite(vals,sizeof(qint32),dim1,outf);
			}
			if (remainder>0) {
				int numread=mda_read_int32(buf,&HH,remainder*dim1,inf);
				//int b0=fread(buf,sizeof(qint32),remainder*dim1,inf);
				Q_UNUSED(numread)
			}
		}
		jfree(buf);
	}
	else if (data_type==MDA_TYPE_REAL) {
		float *buf=(float *)jmalloc(sizeof(float)*factor*dim1);
		for (int kk=0; kk<2; kk++) { //determines min or max
			if (dim3==1) {
				fseek(inf,header_size,SEEK_SET); //need to go back to first part in this case
			}
			for (long i=0; i<dim2b; i++) {
				if ((i%100==0)&&(timer.elapsed()>=500)) {
					timer.start();
					if (dlg->wasCanceled()) {
						QFile::remove(path2);
						return false;
					}
					dlg->setValue((i*50)/dim2b+50*kk);
					qApp->processEvents();
				}
				int numread=mda_read_float32(buf,&HH,factor*dim1,inf);
				//int numread=fread(buf,sizeof(float),factor*dim1,inf);
				float vals[dim1]; for (int a=0; a<dim1; a++) vals[a]=0;
				if (numread<factor*dim1) {
					qWarning() << "Warning, unexpected problem in do_minmax_downsample" << kk << i << numread << factor << dim1;
				}
				for (int ch=0; ch<dim1; ch++) {
					vals[ch]=buf[0*dim1+ch];
					for (int j=0; j<numread/dim1; j++) {
						if (kk==0) vals[ch]=qMin(vals[ch],buf[j*dim1+ch]);
						else if (kk==1) vals[ch]=qMax(vals[ch],buf[j*dim1+ch]);
					}
				}
				mda_write_float32(vals,&HH,dim1,outf);
				//fwrite(vals,sizeof(float),dim1,outf);
			}
			if (remainder>0) {
				int numread=mda_read_float32(buf,&HH,remainder*dim1,inf);
				//int b0=fread(buf,sizeof(float),remainder*dim1,inf);
				Q_UNUSED(numread)
			}
		}
		jfree(buf);
	}
	else if (data_type==MDA_TYPE_SHORT) {
		qint16 *buf=(qint16 *)jmalloc(sizeof(qint16)*factor*dim1);
		for (int kk=0; kk<2; kk++) { //determines min or max
			if (dim3==1) {
				fseek(inf,header_size,SEEK_SET); //need to go back to first part in this case
			}
			for (long i=0; i<dim2b; i++) {
				if ((i%100==0)&&(timer.elapsed()>=500)) {
					timer.start();
					if (dlg->wasCanceled()) {
						QFile::remove(path2);
						return false;
					}
					dlg->setValue((i*50)/dim2b+50*kk);
					qApp->processEvents();
				}
				int numread=mda_read_int16(buf,&HH,factor*dim1,inf);
				//int numread=fread(buf,sizeof(qint16),factor*dim1,inf);
				qint16 vals[dim1]; for (int a=0; a<dim1; a++) vals[a]=0;
				if (numread<factor*dim1) {
					qWarning() << "Warning, unexpected problem in do_minmax_downsample" << kk << i << numread << factor << dim1;
				}
				for (int ch=0; ch<dim1; ch++) {
					vals[ch]=buf[0*dim1+ch];
					for (int j=0; j<numread/dim1; j++) {
						if (kk==0) vals[ch]=qMin(vals[ch],buf[j*dim1+ch]);
						else if (kk==1) vals[ch]=qMax(vals[ch],buf[j*dim1+ch]);
					}
				}
				mda_write_int16(vals,&HH,dim1,outf);
				//fwrite(vals,sizeof(qint16),dim1,outf);
			}
			if (remainder>0) {
				int numread=mda_read_int16(buf,&HH,remainder*dim1,inf);
				//int b0=fread(buf,sizeof(qint16),remainder*dim1,inf);
				Q_UNUSED(numread)
			}
		}
		jfree(buf);
	}
	else if (data_type==MDA_TYPE_UINT16) {
		quint16 *buf=(quint16 *)jmalloc(sizeof(quint16)*factor*dim1);
		for (int kk=0; kk<2; kk++) { //determines min or max
			if (dim3==1) {
				fseek(inf,header_size,SEEK_SET); //need to go back to first part in this case
			}
			for (long i=0; i<dim2b; i++) {
				if ((i%100==0)&&(timer.elapsed()>=500)) {
					timer.start();
					if (dlg->wasCanceled()) {
						QFile::remove(path2);
						return false;
					}
					dlg->setValue((i*50)/dim2b+50*kk);
					qApp->processEvents();
				}
				int numread=mda_read_uint16(buf,&HH,factor*dim1,inf);
				//int numread=fread(buf,sizeof(quint16),factor*dim1,inf);
				quint16 vals[dim1]; for (int a=0; a<dim1; a++) vals[a]=0;
				if (numread<factor*dim1) {
					qWarning() << "Warning, unexpected problem in do_minmax_downsample" << kk << i << numread << factor << dim1;
				}
				for (int ch=0; ch<dim1; ch++) {
					vals[ch]=buf[0*dim1+ch];
					for (int j=0; j<numread/dim1; j++) {
						if (kk==0) vals[ch]=qMin(vals[ch],buf[j*dim1+ch]);
						else if (kk==1) vals[ch]=qMax(vals[ch],buf[j*dim1+ch]);
					}
				}
				mda_write_uint16(vals,&HH,dim1,outf);
				//fwrite(vals,sizeof(quint16),dim1,outf);
			}
			if (remainder>0) {
				int numread=mda_read_uint16(buf,&HH,remainder*dim1,inf);
				//int b0=fread(buf,sizeof(quint16),remainder*dim1,inf);
				Q_UNUSED(numread)
			}
		}
		jfree(buf);
	}
	else {
		qWarning() << "Unsupported data type in do_minmax_downsample" << data_type;
	}

	jfclose(outf);
	jfclose(inf);

	return true;
}

void DiskArrayModel::createFileHierarchyIfNeeded() {
	if (d->m_set_from_mda) return;
	if (fileHierarchyExists()) return;

	QProgressDialog dlg("Creating file hierarchy","Cancel",0,100);
	dlg.show();
	qApp->processEvents();

	QString last_path=d->m_path;
	int scale0=MULTISCALE_FACTOR;
	while (d->m_num_timepoints/scale0>1) {
		dlg.setLabelText(QString("Creating file hierarchy %1").arg(d->m_num_timepoints/scale0));
		qApp->processEvents();
		QString new_path=d->get_multiscale_file_name(scale0);
		if (!QDir(QFileInfo(new_path).path()).exists()) {
			//the following line is crazy!!
			QDir(QFileInfo(QFileInfo(new_path).path()).path()).mkdir(QFileInfo(QFileInfo(new_path).path()).fileName());
		}
		if (!do_minmax_downsample(last_path,new_path,MULTISCALE_FACTOR,&dlg,(scale0==MULTISCALE_FACTOR))) {
			exit(-1);
			return;
		}
		last_path=new_path;
		scale0*=MULTISCALE_FACTOR;
	}

	// This was the method that involves loading the whole thing into memory

	/*
	Mda X; X.read(d->m_path.toLatin1().data());
	int i3=0;
	int scale0=MULTISCALE_FACTOR;
	while (d->m_num_timepoints/scale0>1) {
		int N2=X.size(1)/MULTISCALE_FACTOR; if (N2*MULTISCALE_FACTOR<X.size(1)) N2++;
		Mda X2; X2.setDataType(d->m_data_type);
		X2.allocate(d->m_num_channels,N2,2);
		for (int tt=0; tt<N2; tt++) {
			for (int ch=0; ch<d->m_num_channels; ch++) {
				float minval=X.value(ch,tt*MULTISCALE_FACTOR,0);
				float maxval=X.value(ch,tt*MULTISCALE_FACTOR,i3);
				for (int dt=0; dt<MULTISCALE_FACTOR; dt++) {
					minval=qMin(minval,X.value(ch,tt*MULTISCALE_FACTOR+dt,0));
					maxval=qMax(maxval,X.value(ch,tt*MULTISCALE_FACTOR+dt,i3));
				}
				X2.setValue(minval,ch,tt,0);
				X2.setValue(maxval,ch,tt,1);
			}
		}
		QString path0=d->get_multiscale_file_name(scale0);
		X2.write(path0.toLatin1().data());
		i3=1;
		scale0*=MULTISCALE_FACTOR;
		X=X2;
	}
	*/
}

void DiskArrayModelPrivate::read_header() {
	FILE *inf=jfopen(m_path.toLatin1().data(),"rb");
	if (!inf) {
		qWarning() << "Problem opening file: " << m_path;
		return;
	}


	MDAIO_HEADER HH;
	mda_read_header(&HH,inf);
	int dim1=HH.dims[0];
	int dim2=HH.dims[1];
	int dim3=HH.dims[2];
	int data_type=HH.data_type;

	jfclose(inf);

	m_data_type=data_type;
	m_num_channels=dim1;
	m_num_timepoints=dim2*dim3;
}
QString DiskArrayModelPrivate::get_code(int scale,int chunk_size,int chunk_ind) {
	return QString("%1-%2-%3").arg(scale).arg(chunk_size).arg(chunk_ind);
}

Mda *DiskArrayModelPrivate::load_chunk(int scale,int chunk_ind) {
	QString code=get_code(scale,m_chunk_size,chunk_ind);
	if (m_loaded_chunks.contains(code)) {
		return m_loaded_chunks[code];
	}
	QString path=get_multiscale_file_name(scale);
	Mda *X=load_chunk_from_file(path,scale,chunk_ind);
	if (X) {
		//qDebug()  << "################################  Loaded chunk"  << scale << chunk_ind << X->totalSize()*4*1.0/1000000;
		m_loaded_chunks[code]=X;
	}
	return X;

}

Mda *DiskArrayModelPrivate::load_chunk_from_file(QString path,int scale,int chunk_ind) {
	if (scale==1) {
		QVector<float> d0=load_data_from_mda(path,m_num_channels*m_chunk_size,0,chunk_ind*m_chunk_size,0);
		if (d0.isEmpty()) return 0;
		Mda *ret=new Mda();
		ret->setDataType(m_data_type);
		ret->allocate(m_num_channels,m_chunk_size,2);
		int ct=0;
		for (int tt=0; tt<m_chunk_size; tt++) {
			for (int ch=0; ch<m_num_channels; ch++) {
				float val=0;
				if (ct<d0.count()) val=d0[ct];
				ret->setValue(val,ch,tt,0);
				ret->setValue(val,ch,tt,1);
				ct++;
			}
		}
		return ret;
	}
	else {
		QVector<float> d0=load_data_from_mda(path,m_num_channels*m_chunk_size,0,chunk_ind*m_chunk_size,0);
		QVector<float> d1=load_data_from_mda(path,m_num_channels*m_chunk_size,0,chunk_ind*m_chunk_size,1);

		if (d0.isEmpty()) return 0;
		Mda *ret=new Mda();
		ret->setDataType(m_data_type);
		ret->allocate(m_num_channels,m_chunk_size,2);
		int ct=0;
		for (int tt=0; tt<m_chunk_size; tt++) {
			for (int ch=0; ch<m_num_channels; ch++) {
				float val0=0,val1=0;
				if (ct<d0.count()) val0=d0[ct];
				if (ct<d1.count()) val1=d1[ct];
				ret->setValue(val0,ch,tt,0);
				ret->setValue(val1,ch,tt,1);

				ct++;
			}
		}

		return ret;
	}

}

QVector<float> DiskArrayModelPrivate::load_data_from_mda(QString path,int n,int i1,int i2,int i3) {
	QVector<float> ret;
	FILE *inf=jfopen(path.toLatin1().data(),"rb");
	if (!inf) {
		qWarning() << "Problem reading data from mda: " << path << i1 << i2 << i3 << n;
		return ret;
	}

	MDAIO_HEADER HH;
	mda_read_header(&HH,inf);
	int dim1=HH.dims[0];
	int dim2=HH.dims[1];
	int header_size=HH.header_size;
	int data_type=HH.data_type;
	int num_bytes=HH.num_bytes_per_entry;

	int i0=i1+i2*dim1+i3*dim1*dim2;
	fseek(inf,header_size+num_bytes*i0,SEEK_SET);
	ret.resize(n);
	if (data_type==MDA_TYPE_BYTE) {
		unsigned char *X=(unsigned char *)jmalloc(sizeof(unsigned char)*n);
		int b0=mda_read_byte(X,&HH,n,inf);
		//int b0=fread(X,sizeof(unsigned char),n,inf);
		Q_UNUSED(b0)
		for (int i=0; i<n; i++) {
			ret[i]=(float)X[i];
		}
		jfree(X);
	}
	else if (data_type==MDA_TYPE_INT32) {
		qint32 *X=(qint32 *)jmalloc(sizeof(qint32)*n);
		int b0=mda_read_int32(X,&HH,n,inf);
		//int b0=fread(X,sizeof(qint32),n,inf);
		Q_UNUSED(b0)
		for (int i=0; i<n; i++) {
			ret[i]=(float)X[i];
		}
		jfree(X);
	}
	else if (data_type==MDA_TYPE_REAL) {
		float *X=(float *)jmalloc(sizeof(float)*n);
		int bytes_read=mda_read_float32(X,&HH,n,inf);
		//int bytes_read=fread(X,sizeof(float),n,inf);
		Q_UNUSED(bytes_read);
		for (int i=0; i<n; i++) {
			ret[i]=(float)X[i];
		}
		jfree(X);
	}
	else if (data_type==MDA_TYPE_SHORT) {
		short *X=(short *)jmalloc(sizeof(short)*n);
		int bytes_read=mda_read_int16(X,&HH,n,inf);
		//int bytes_read=fread(X,sizeof(short),n,inf);
		Q_UNUSED(bytes_read)
		for (int i=0; i<n; i++) {
			ret[i]=(float)X[i];
		}
		jfree(X);
	}
	else if (data_type==MDA_TYPE_UINT16) {
		quint16 *X=(quint16 *)jmalloc(sizeof(quint16)*n);
		int bytes_read=mda_read_uint16(X,&HH,n,inf);
		//int bytes_read=fread(X,sizeof(quint16),n,inf);
		Q_UNUSED(bytes_read);
		for (int i=0; i<n; i++) {
			ret[i]=(float)X[i];
		}
		jfree(X);
	}
	else {
		qWarning() << "Unsupported data type: " << data_type;
		ret.clear();
	}

	jfclose(inf);
	return ret;
}
int DiskArrayModel::size(int dim) {
	if (dim==0) return d->m_num_channels;
	else if (dim==1) return d->m_num_timepoints;
	else return 1;
}

QString DiskArrayModelPrivate::get_multiscale_file_name(int scale) {
	QString str=QString(".%1").arg(scale);
	QDateTime time=QFileInfo(m_path).lastModified();
	QString timestamp=time.toString("yyyy-mm-dd-hh-mm-ss");
	QString subdir="spikespy."+QFileInfo(m_path).baseName()+"."+timestamp+"/";
	if (scale==1) {
		str="";
		subdir="";
	}
	return QFileInfo(m_path).path()+"/"+subdir+QFileInfo(m_path).baseName()+str+".mda";
}
bool DiskArrayModelPrivate::file_exists(QString path) {
	return QFileInfo(path).exists();
}
