#include "ftelectrodearrayview.h"
#include "math.h"

#include <qpainter.h>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>

class FTElectrodeArrayViewPrivate {
public:
	FTElectrodeArrayView *q;
	Mda m_waveform;
	Mda m_waveform_absmax_vals;
	Mda m_waveform_absmax_inds;
	float m_waveform_absmax;
	Mda m_electrode_locations;
	float m_min_electrode_spacing;
	float m_electrode_minx,m_electrode_maxx;
	float m_electrode_miny,m_electrode_maxy;
	float m_pixel_spacing_x,m_pixel_spacing_y;
	int m_timepoint;
	int m_animate_timepoint;
	bool m_animation_paused;
	QList<QPointF> m_electrode_pixel_locations;
	int m_hovered_index;
	QSet<int> m_selected_indices;

	QPointF ind2pix(int i);
	QColor fire_color_map(float pct);
	int find_electrode_index_at(QPointF pt);
	float get_electrode_pixel_radius();
};


FTElectrodeArrayView::FTElectrodeArrayView(QWidget *parent) : QWidget(parent)
{
	d=new FTElectrodeArrayViewPrivate;
	d->q=this;

	d->m_waveform_absmax=1;

	d->m_min_electrode_spacing=1;
	d->m_electrode_minx=0; d->m_electrode_maxx=1;
	d->m_electrode_miny=0; d->m_electrode_maxy=1;

	d->m_pixel_spacing_x=10;
	d->m_pixel_spacing_y=10;

	d->m_hovered_index=-1;

	QPalette pal=this->palette();
	int tmp=(int)(255*0.8);
	pal.setColor(QPalette::Background, QColor(tmp,tmp,tmp));
	this->setPalette(pal);

	d->m_timepoint=-1;
	d->m_animate_timepoint=-1;
	d->m_animation_paused=false;

	this->setMouseTracking(true);

	QTimer::singleShot(100,this,SLOT(slot_timer()));
}

void FTElectrodeArrayView::setElectrodeLocations(const Mda &L)
{
	d->m_electrode_locations=L;
	d->m_min_electrode_spacing=99999;
	d->m_electrode_minx=99999;
	d->m_electrode_maxx=-99999;
	d->m_electrode_miny=99999;
	d->m_electrode_maxy=-99999;
	int M=d->m_electrode_locations.N1();
	for (int i=0; i<M; i++) {
		for (int j=0; j<M; j++) {
			float x1=d->m_electrode_locations.value(i,0);
			float y1=d->m_electrode_locations.value(i,1);
			float x2=d->m_electrode_locations.value(j,0);
			float y2=d->m_electrode_locations.value(j,1);
			float val=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
			if (val!=0) {
				if (val<d->m_min_electrode_spacing) {
					d->m_min_electrode_spacing=val;
				}
			}
			if (x1<d->m_electrode_minx) d->m_electrode_minx=x1;
			if (x1>d->m_electrode_maxx) d->m_electrode_maxx=x1;
			if (y1<d->m_electrode_miny) d->m_electrode_miny=y1;
			if (y1>d->m_electrode_maxy) d->m_electrode_maxy=y1;
		}
	}
	this->update();
}

void FTElectrodeArrayView::setWaveform(const Mda &X)
{
	d->m_waveform=X;
	int M=d->m_waveform.N1();
	int T=d->m_waveform.N2();
	d->m_waveform_absmax_vals.allocate(M,1);
	d->m_waveform_absmax_inds.allocate(M,1);
	d->m_waveform_absmax=0;
	for (int m=0; m<M; m++) {
		float maxval=0;
		int maxind=0;
		for (int t=0; t<T; t++) {
			float val=qAbs(d->m_waveform.value(m,t));
			if (val>maxval) {
				maxval=val;
				maxind=t;
			}
		}
		d->m_waveform_absmax_vals.setValue(maxval,m,0);
		d->m_waveform_absmax_inds.setValue(maxind,m,0);
		if (maxval>d->m_waveform_absmax) d->m_waveform_absmax=maxval;
	}
	this->update();
}

int FTElectrodeArrayView::timepoint()
{
	return d->m_timepoint;
}

void FTElectrodeArrayView::setTimepoint(int val)
{
	if (d->m_timepoint!=val) {
		d->m_animate_timepoint=val;
		d->m_timepoint=val;
		update(); repaint();
		emit signalTimepointChanged();
	}
}

void FTElectrodeArrayView::animate()
{
	d->m_animation_paused=false;
	if (d->m_animate_timepoint<0) d->m_animate_timepoint=0;
}

void FTElectrodeArrayView::pauseAnimation()
{
	d->m_animation_paused=true;
}

void FTElectrodeArrayView::rewindAnimation()
{
	if (d->m_animate_timepoint>=0) {
		d->m_animate_timepoint=0;
		d->m_timepoint=d->m_animate_timepoint;
		update(); repaint();
		emit signalTimepointChanged();
	}
}

void FTElectrodeArrayView::stopAnimation()
{
	d->m_animate_timepoint=-1;
}

bool FTElectrodeArrayView::isAnimating()
{
	return ((d->m_animate_timepoint>=0)&&(!d->m_animation_paused));
}

void FTElectrodeArrayView::paintEvent(QPaintEvent *evt)
{
	Q_UNUSED(evt);
	QPainter painter(this);

	d->ind2pix(0); //to set d->m_pixel_spacing_*
	float spacing=d->get_electrode_pixel_radius();
	float absmax=d->m_waveform_absmax; if (absmax==0) absmax=1;
	int M=d->m_electrode_locations.N1();
	d->m_electrode_pixel_locations.clear();
	for (int i=0; i<M; i++) {
		float val=0;
		if (d->m_timepoint<0)
			val=(d->m_waveform_absmax_vals.value(i,0)/absmax);
		else
			val=(d->m_waveform.value(i,d->m_timepoint))/absmax;

		QColor col=d->fire_color_map(1-(1-val)*(1-val)*(1-val));
		col=QColor(255-col.red(),255-col.green(),255-col.blue());
		QPointF pt=d->ind2pix(i);
		d->m_electrode_pixel_locations << pt;
		painter.setBrush(QBrush(col));
		int tmp=(int)(255*0.7);
		QColor pen_color=QColor(tmp,tmp,tmp);
		int pen_width=1;
		if (i==d->m_hovered_index) {
			pen_color=Qt::yellow;
			pen_width=3;
		}
		if (d->m_selected_indices.contains(i)) {
			pen_color=Qt::red;
			pen_width=qMax(pen_width,2);
		}
		painter.setPen(QPen(QBrush(pen_color),pen_width));
		painter.drawEllipse(pt,spacing,spacing);
	}
}

void FTElectrodeArrayView::mouseMoveEvent(QMouseEvent *evt)
{
	int ind=d->find_electrode_index_at(evt->posF());
	if (ind!=d->m_hovered_index) {
		d->m_hovered_index=ind;
		update();
	}
}

void FTElectrodeArrayView::mousePressEvent(QMouseEvent *evt)
{
	int ind=d->find_electrode_index_at(evt->posF());
	if (ind>=0) {
		if (d->m_selected_indices.contains(ind)) {
			d->m_selected_indices.remove(ind);
		}
		else {
			d->m_selected_indices.insert(ind);
		}
		update();
	}
}

void FTElectrodeArrayView::slot_timer()
{
	int T=d->m_waveform.N2();
	int msec=400;
	if ((d->m_animate_timepoint>=0)&&(!d->m_animation_paused)) {
		if (d->m_timepoint!=d->m_animate_timepoint) {
			d->m_timepoint=d->m_animate_timepoint;
			emit signalTimepointChanged();
		}
		d->m_animate_timepoint++;
		if (d->m_animate_timepoint>=T) {
			d->m_animate_timepoint=-1;
			d->m_timepoint=-1;
			emit signalTimepointChanged();
		}
		this->update(); this->repaint();
		msec=20;
	}
	QTimer::singleShot(msec,this,SLOT(slot_timer()));
}

FTElectrodeArrayView::~FTElectrodeArrayView()
{
	delete d;
}



QPointF FTElectrodeArrayViewPrivate::ind2pix(int i)
{
	float x0=m_electrode_locations.value(i,0);
	float y0=m_electrode_locations.value(i,1);

	float minx=m_electrode_minx-m_min_electrode_spacing; float maxx=m_electrode_maxx+m_min_electrode_spacing;
	float miny=m_electrode_miny-m_min_electrode_spacing; float maxy=m_electrode_maxy+m_min_electrode_spacing;
	float spacing=m_min_electrode_spacing;

	float width0=maxx-minx+2*spacing;
	float height0=maxy-miny+2*spacing;

	float factorx=q->width()/width0;
	float factory=q->height()/height0;
	float factor0=qMin(factorx,factory);

	float offsetx=(q->width()-width0*factor0)/2;
	float offsety=(q->height()-height0*factor0)/2;

	float x1=offsetx+(spacing+x0-minx)*factor0;
	float y1=offsety+(spacing+y0-miny)*factor0;

	m_pixel_spacing_x=m_min_electrode_spacing*factor0;
	m_pixel_spacing_y=m_min_electrode_spacing*factor0;

	return QPointF(x1,y1);
}

QColor FTElectrodeArrayViewPrivate::fire_color_map(float pct)
{
	if (pct>1) pct=1;
	if (pct<0) pct=0;
	float r=0,g=0,b=0;

	if (pct<0.5) r=pct/0.5*255;
	else r=255;

	if (pct<0.4) g=0;
	else if (pct<0.9) g=(pct-0.4)/0.5*255;
	else g=255;

	if (pct<0.5) b=220-qAbs(pct-0.25)/0.25*220;
	else if (pct<0.8) b=0;
	else b=255-(1-pct)/0.2*255;

	return QColor((int)r,(int)g,(int)b);
}

int FTElectrodeArrayViewPrivate::find_electrode_index_at(QPointF pt)
{
	int ret=-1;
	float spacing=get_electrode_pixel_radius();
	for (int i=0; i<m_electrode_pixel_locations.count(); i++) {
		QPointF diff=pt-m_electrode_pixel_locations[i];
		float dist=sqrt(diff.x()*diff.x()+diff.y()*diff.y());
		if (dist<=spacing+1) ret=i;
	}
	return ret;
}

float FTElectrodeArrayViewPrivate::get_electrode_pixel_radius()
{
	return qMax(3.0,(qMin(m_pixel_spacing_x,m_pixel_spacing_y)-3.0)/2);
}

