#include "firetrackwidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QListWidget>
#include "ftelectrodearraywidget.h"
#include "sstimeseriesview.h"
#include "ftplotoptions.h"

class FireTrackWidgetPrivate {
public:
	FireTrackWidget *q;

	FTElectrodeArrayWidget *m_widget;
	QListWidget *m_waveform_list;
	SSTimeSeriesView *m_plot;
	FTPlotOptions *m_plot_options;

	Mda m_waveforms;
	Mda m_locations;
	int m_current_waveform_index;

	int m_left_clicked_current_index;
	QSet<int> m_left_clicked_current_index_viewed_waveform_indices;

	void update_waveform_list();
	void set_current_waveform_index(int ind);
};


FireTrackWidget::FireTrackWidget(QWidget *parent) : QMainWindow(parent)
{
	d=new FireTrackWidgetPrivate;
	d->q=this;

	d->m_current_waveform_index=-1;

	d->m_left_clicked_current_index=-1;

	d->m_widget=new FTElectrodeArrayWidget;
	connect(d->m_widget,SIGNAL(signalSelectedElectrodesChanged()),this,SLOT(slot_selected_electrodes_changed()));
	connect(d->m_widget,SIGNAL(signalTimepointChanged()),this,SLOT(slot_timepoint_changed()));
	connect(d->m_widget,SIGNAL(signalElectrodeLeftClicked(int)),this,SLOT(slot_electrode_left_clicked(int)));


	d->m_waveform_list=new QListWidget; d->m_waveform_list->setFixedWidth(120);
	connect(d->m_waveform_list,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(slot_current_waveform_changed()));

	d->m_plot=new SSTimeSeriesView;
	d->m_plot->setVerticalZoomFactor(1);
	DiskArrayModel *X=new DiskArrayModel;
	Mda XX; XX.allocate(5,80*6);
	for (int n=0; n<XX.N2(); n++) {
		for (int m=0; m<XX.N1(); m++) {
			XX.setValue(qrand()%100,m,n);
		}
	}
	X->setFromMda(XX);
	int tmp_timepoint=d->m_plot->currentTimepoint();
	d->m_plot->setData(X);
	d->m_plot->initialize();
	d->m_plot->setFixedWidth(250);
	d->m_plot->setCurrentTimepoint(tmp_timepoint);
	connect(d->m_plot,SIGNAL(currentXChanged()),this,SLOT(slot_plot_timepoint_changed()));

	d->m_plot_options=new FTPlotOptions;
	connect(d->m_plot_options,SIGNAL(signalOptionsChanged()),this,SLOT(slot_plot_options_changed()));
	connect(d->m_plot_options,SIGNAL(signalVerticalScaling(float)),this,SLOT(slot_plot_vertical_scaling(float)));
    d->m_widget->setAnimationSpeed(d->m_plot_options->animationSpeed());

	QHBoxLayout *HL=new QHBoxLayout;
	HL->addWidget(d->m_waveform_list);
	HL->addWidget(d->m_widget);
	HL->addWidget(d->m_plot);

	QWidget *bottom_widget=new QWidget;
	bottom_widget->setFixedHeight(150);
	QHBoxLayout *L1=new QHBoxLayout;
	L1->addWidget(d->m_widget->optionsWidget());
	L1->addWidget(d->m_plot_options);
	L1->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding));
	bottom_widget->setLayout(L1);

	QWidget *CW=new QWidget;
	QVBoxLayout *CL=new QVBoxLayout;
	CL->addLayout(HL);
	CL->addWidget(bottom_widget);
	CW->setLayout(CL);
	this->setCentralWidget(CW);


    d->m_widget->animate();
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

	//set a bunch of selected electrodes, which will then be reset to those with maximal peaks
	QList<int> dummy;
	int num=X.N1();
	if (num>20) num=10;
    if (X.N3()<=1) num=0; //because in this case we are probably viewing the raw data
	for (int i=0; i<num; i++) {
		if (i<X.N1()) {
			dummy << i;
		}
	}
	d->m_widget->setSelectedElectrodeIndices(dummy);

	float absmax=0;
	for (int n=0; n<X.N3(); n++)
		for (int t=0; t<X.N2(); t++)
			for (int m=0; m<X.N1(); m++) {
				float val=qAbs(X.value(m,t,n));
				if (val>absmax) absmax=val;
			}
	d->m_widget->setGlobalAbsMax(absmax);

	d->set_current_waveform_index(0);
}

void FireTrackWidget::setWaveforms(DiskReadMda *X)
{
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
	Mda Y; Y.allocate(X->N1(),X->N2());
	for (int y=0; y<X->N2(); y++)
		for (int x=0; x<X->N1(); x++) {
			Y.setValue(X->value(x,y),x,y);
		}
	setElectrodeLocations(Y);
}

void FireTrackWidget::resizeEvent(QResizeEvent *evt)
{
	Q_UNUSED(evt);
	{
		float W0=this->width()*0.15;
		W0=qMax(W0,80.0F);
		W0=qMin(W0,300.0F);
		d->m_waveform_list->setFixedWidth(W0);
	}
	{
		float W0=this->width()*0.2;
		W0=qMax(W0,150.0F);
		W0=qMin(W0,350.0F);
		d->m_plot->setFixedWidth(W0);
	}
}

void FireTrackWidget::slot_current_waveform_changed()
{
	QListWidgetItem *it=d->m_waveform_list->currentItem();
	if (!it) return;
	d->set_current_waveform_index(it->data(Qt::UserRole).toInt());
}

void FireTrackWidget::slot_selected_electrodes_changed()
{
	QList<int> inds=d->m_widget->selectedElectrodeIndices();
	int M=qMax(1,inds.count());
	int T=d->m_waveforms.N2();
	int N=1;
	Mda X; X.allocate(M,T,N);
	QStringList channel_labels;
	for (int m=0; m<inds.count(); m++) {
		int ii=inds[m];
		for (int t=0; t<T; t++) {
			X.setValue(d->m_waveforms.value(ii,t,d->m_current_waveform_index),m,t);
		}
		channel_labels << QString("%1").arg(inds[m]+1);
	}
	DiskArrayModel *XX=new DiskArrayModel; XX->setFromMda(X);
	if (d->m_plot->data()) {
		delete d->m_plot->data();
	}
	int tmp_timepoint=d->m_plot->currentTimepoint();
	d->m_plot->setChannelLabels(channel_labels);
	d->m_plot->setData(XX);
	d->m_plot->initialize();
	d->m_plot->setCurrentTimepoint(tmp_timepoint);
}

void FireTrackWidget::slot_timepoint_changed()
{
	d->m_plot->setCurrentTimepoint(d->m_widget->timepoint());
}

void FireTrackWidget::slot_plot_timepoint_changed()
{
	d->m_widget->setTimepoint(d->m_plot->currentTimepoint());
}

void FireTrackWidget::slot_electrode_left_clicked(int ind)
{

	if (ind!=d->m_left_clicked_current_index) {
		d->m_left_clicked_current_index_viewed_waveform_indices.clear();
	}

	float best_val=0;
	float best_val_ind=0;
	for (int i=0; i<d->m_waveforms.N3(); i++) {
		if (!d->m_left_clicked_current_index_viewed_waveform_indices.contains(i)) {
			float max_t_val=0;
			for (int t=0; t<d->m_waveforms.N2(); t++) {
				float tmp=qAbs(d->m_waveforms.value(ind,t,i));
				if (tmp>max_t_val) {
					max_t_val=tmp;
				}
			}
			if (max_t_val>best_val) {
				best_val=max_t_val;
				best_val_ind=i;
			}
		}
	}
	d->m_left_clicked_current_index_viewed_waveform_indices.insert(best_val_ind);
	d->set_current_waveform_index(best_val_ind);
	for (int i=0; i<d->m_waveform_list->count(); i++) {
		QListWidgetItem *it=d->m_waveform_list->item(i);
		if (it->data(Qt::UserRole).toInt()==best_val_ind) {
			d->m_waveform_list->setCurrentItem(it);
		}
	}

	d->m_left_clicked_current_index=ind;
}

void FireTrackWidget::slot_plot_options_changed()
{
	d->m_plot->setUniformVerticalChannelSpacing(d->m_plot_options->uniformVerticalChannelSpacing());
    d->m_widget->setAnimationSpeed(d->m_plot_options->animationSpeed());
    d->m_widget->setLoopAnimation(d->m_plot_options->loopAnimation());
}

void FireTrackWidget::slot_plot_vertical_scaling(float val)
{
	d->m_plot->setVerticalZoomFactor(d->m_plot->verticalZoomFactor()*val);
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
		it->setText(QString("Neuron %1").arg(i+1));
		it->setData(Qt::UserRole,i);
		m_waveform_list->addItem(it);
	}
}

void FireTrackWidgetPrivate::set_current_waveform_index(int ind)
{
	if (m_current_waveform_index==ind) return;
	m_current_waveform_index=ind;
	Mda Y; Y.allocate(m_waveforms.N1(),m_waveforms.N2());
	for (int y=0; y<m_waveforms.N2(); y++)
		for (int x=0; x<m_waveforms.N1(); x++) {
			Y.setValue(m_waveforms.value(x,y,m_current_waveform_index),x,y);
		}
	m_widget->setWaveform(Y);
	q->slot_selected_electrodes_changed();
}
