#include "firetrackwidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QListWidget>
#include "ftelectrodearraywidget.h"
#include "sstimeseriesplot.h"

class FireTrackWidgetPrivate {
public:
	FireTrackWidget *q;

	FTElectrodeArrayWidget *m_widget;
	QListWidget *m_waveform_list;
	SSTimeSeriesPlot *m_plot;

	Mda m_waveforms;
	Mda m_locations;
	int m_current_waveform_index;

	void update_waveform_list();
	void set_current_waveform_index(int ind);
};


FireTrackWidget::FireTrackWidget(QWidget *parent) : QMainWindow(parent)
{
	d=new FireTrackWidgetPrivate;
	d->q=this;

	d->m_current_waveform_index=-1;

	d->m_widget=new FTElectrodeArrayWidget;
	d->m_waveform_list=new QListWidget; d->m_waveform_list->setFixedWidth(120);
	connect(d->m_waveform_list,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(slot_current_waveform_changed()));

	d->m_plot=new SSTimeSeriesPlot;
	DiskArrayModel *X=new DiskArrayModel;
	Mda XX; XX.allocate(5,80*6);
	for (int n=0; n<XX.N2(); n++) {
		for (int m=0; m<XX.N1(); m++) {
			XX.setValue(qrand()%100,m,n);
		}
	}
	X->setFromMda(XX);
	d->m_plot->setData(X);
	d->m_plot->initialize();
	d->m_plot->setFixedWidth(250);

	QWidget *CW=new QWidget;
	QHBoxLayout *CL=new QHBoxLayout;
	CW->setLayout(CL);
	CL->addWidget(d->m_waveform_list);
	CL->addWidget(d->m_widget);
	CL->addWidget(d->m_plot);
	this->setCentralWidget(CW);
	qDebug() << "Testing";
}

void FireTrackWidget::setElectrodeLocations(const Mda &L)
{
	d->m_locations=L;
	d->m_widget->setElectrodeLocations(L);
}

void FireTrackWidget::setWaveforms(const Mda &X)
{
	d->m_waveforms=X;
	d->update_waveform_list();
	d->set_current_waveform_index(0);
}

void FireTrackWidget::setWaveforms(DiskReadMda *X)
{
	qDebug() << "setWaveforms" << X->N1() << X->N2() << X->N3();
	Mda Y; Y.allocate(X->N1(),X->N2(),X->N3());
	for (int z=0; z<X->N3(); z++)
		for (int y=0; y<X->N2(); y++)
			for (int x=0; x<X->N1(); x++) {
				Y.setValue(X->value(x,y,z),x,y,z);
			}
	setWaveforms(Y);
}

void FireTrackWidget::setElectrodeLocations(DiskReadMda *X)
{
	qDebug() << "setElectrodeLocations" << X->N1() << X->N2();
	Mda Y; Y.allocate(X->N1(),X->N2());
	for (int y=0; y<X->N2(); y++)
		for (int x=0; x<X->N1(); x++) {
			Y.setValue(X->value(x,y),x,y);
		}
	setElectrodeLocations(Y);
}

void FireTrackWidget::slot_current_waveform_changed()
{
	QListWidgetItem *it=d->m_waveform_list->currentItem();
	if (!it) return;
	d->set_current_waveform_index(it->data(Qt::UserRole).toInt());
}

FireTrackWidget::~FireTrackWidget()
{
	delete d;
}



void FireTrackWidgetPrivate::update_waveform_list()
{
	m_waveform_list->clear();
	int N=m_waveforms.N3();
	for (int i=0; i<N; i++) {
		QListWidgetItem *it=new QListWidgetItem();
		it->setText(QString("WF %1").arg(i+1));
		it->setData(Qt::UserRole,i);
		m_waveform_list->addItem(it);
	}
}

void FireTrackWidgetPrivate::set_current_waveform_index(int ind)
{
	m_current_waveform_index=ind;
	Mda Y; Y.allocate(m_waveforms.N1(),m_waveforms.N2());
	for (int y=0; y<m_waveforms.N2(); y++)
		for (int x=0; x<m_waveforms.N1(); x++) {
			Y.setValue(m_waveforms.value(x,y,m_current_waveform_index),x,y);
		}
	m_widget->setWaveform(Y);
}
