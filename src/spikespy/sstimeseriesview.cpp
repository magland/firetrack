#include "sstimeseriesview.h"
#include "sstimeseriesplot.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QDebug>
#include "sslabelsmodel1.h"
#include <math.h>

class SSTimeSeriesViewPrivate {
public:
	SSTimeSeriesView *q;

	SSARRAY *m_data;
	bool m_data_is_owner;
	SSLabelsModel *m_labels;
	SSTimeSeriesPlot *m_plot;

	bool m_clip_mode;

	void advance_to_clip(bool backwards);
};


SSTimeSeriesView::SSTimeSeriesView(QWidget *parent) : SSAbstractView(parent) {
	d=new SSTimeSeriesViewPrivate;
	d->q=this;

	d->m_data=0;
	d->m_labels=0;
	d->m_data_is_owner=false;

	d->m_clip_mode=false;

	d->m_plot=new SSTimeSeriesPlot;
	d->m_plot->setMargins(0,0,5,38); //timeline (30) + event labels (8)
	//d->m_plot->setUnderlayPainter(d->m_underlay_painter);

	QVBoxLayout *layout=new QVBoxLayout;
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->setContentsMargins(0,0,0,0);
	setLayout(layout);
	layout->addWidget(plot());
}

SSTimeSeriesView::~SSTimeSeriesView() {
	if (d->m_data) {
		if (d->m_data_is_owner)
			delete d->m_data;
	}
	delete d;
}

SSTimeSeriesPlot *SSTimeSeriesView::plot()
{
	return d->m_plot;
}

QString SSTimeSeriesView::viewType()
{
	return "SSTimeSeriesView";
}

void SSTimeSeriesView::keyPressEvent(QKeyEvent *evt)
{
	if (evt->modifiers()&Qt::ControlModifier) {
		if (evt->key()==Qt::Key_Left) {
			if (d->m_clip_mode) {
				d->advance_to_clip(true);
				return;
			}
		}
		else if (evt->key()==Qt::Key_Right) {
			if (d->m_clip_mode) {
				d->advance_to_clip(false);
				return;
			}
		}
	}
	SSAbstractView::keyPressEvent(evt);
}

void SSTimeSeriesView::setData(SSARRAY *data,bool is_owner) {

	if (!data->fileHierarchyExists()) {
		qWarning() << "File hierarchy does not exist. Creating.";
		data->createFileHierarchyIfNeeded();
	}

	if ((d->m_data)&&(d->m_data_is_owner)) {
		delete d->m_data; d->m_data=0;
	}

	d->m_data=data;
	d->m_data_is_owner=is_owner;
	setMaxTimepoint(data->size(1)-1);

	d->m_plot->setData(d->m_data);

	this->setCurrentX(0);
}

void SSTimeSeriesView::setLabels(DiskReadMda *TL,bool is_owner)
{
	SSLabelsModel1 *L=new SSLabelsModel1;
	L->setTimepointsLabels(TL,is_owner);
	d->m_labels=L;
	d->m_plot->setLabels(L,true);
}

/*void SSTimeSeriesView::setConnectZeros(bool val)
{
	d->m_plot->setConnectZeros(val);
}*/

void SSTimeSeriesView::setClipMode(bool val)
{
	d->m_clip_mode=val;
}

bool SSTimeSeriesView::clipMode()
{
	return d->m_clip_mode;
}

SSLabelsModel *SSTimeSeriesView::getLabels()
{
	return d->m_labels;
}

float SSTimeSeriesView::currentValue()
{
	if (!d->m_data) return 0;
	int ch0=currentChannel();
	if (ch0>=0) {
		return d->m_data->value(ch0,(int)currentX());
	}
	else return 0;

}

SSARRAY *SSTimeSeriesView::data() {
	return d->m_data;
}


void SSTimeSeriesViewPrivate::advance_to_clip(bool backwards)
{
	if (!m_data) return;
	int N=m_data->size(1);
	//int ch0=0;
	int x=q->currentX();
	if (x<0) x=0;
	int direction=1; if (backwards) direction=-1;
	int x0=x;

	//find the first time skip
	bool done=false;
	while (!done) {
		x0+=direction;
		if (x0<0) done=true;
		if (x0>=N) done=true;
		if (!done) {
			int t1=q->getTimepointForX(x0);
			int t2=q->getTimepointForX(x0+direction);
			if (abs(t1-t2)>1) {
				done=true;
				x0+=direction;
			}
		}
	}

	int x1=x0;
	//find the time skip
	done=false;
	while (!done) {
		x0+=direction;
		if (x0<0) done=true;
		if (x0>=N) done=true;
		if (!done) {
			int t1=q->getTimepointForX(x0);
			int t2=q->getTimepointForX(x0+direction);
			if (abs(t1-t2)>1) {
				done=true;
			}
		}
	}

	q->setCurrentX((x0+x1)/2);
	emit q->requestCenterOnCursor();
}

/*
void SSTimeSeriesViewPrivate::advance_to_clip(bool backwards)
{
	if (!m_data) return;
	int N=m_data->size(1);
	int ch0=0;
	int x=q->currentX();
	if (x<0) x=0;
	int direction=1; if (backwards) direction=-1;
	int x0=x;

	//find the first two zeros in a row (a hack)
	bool done=false;
	while (!done) {
		x0+=direction;
		if (x0<0) done=true;
		if (x0>=N) done=true;
		if (!done) {
			float val1=m_data->value(ch0,x0);
			float val2=m_data->value(ch0,x0+direction);
			if ((!val1)&&(!val2)) done=true;
		}
	}

	//find the first nonzero
	done=false;
	while (!done) {
		x0+=direction;
		if (x0<0) done=true;
		if (x0>=N) done=true;
		if (!done) {
			float val=m_data->value(ch0,x0);
			if (val) done=true;
		}
	}

	int x1=x0;
	//find the next two zeros in a row (a hack)
	done=false;
	while (!done) {
		x0+=direction;
		if (x0<0) done=true;
		if (x0>=N) done=true;
		if (!done) {
			float val1=m_data->value(ch0,x0);
			float val2=m_data->value(ch0,x0+direction);
			if ((!val1)&&(!val2)) done=true;
		}
	}

	q->setCurrentX((x0+x1)/2);
	emit q->requestCenterOnCursor();
}
*/
