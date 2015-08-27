#include "sstimeseriesplot.h"
#include <QPainter>
#include <QList>
#include "plotarea.h"
#include <QDebug>
#include <QTime>
#include <QMap>
#include "sscommon.h"
#define MIN_NUM_PIXELS 4000

class SSTimeSeriesPlotPrivate {
public:
	SSTimeSeriesPlot *q;
	SSARRAY *m_data;
	SSLabelsModel1 *m_labels;
	bool m_labels_is_owner;
	int m_max_timepoint;
	int m_num_channels;

	QList<double> m_minvals;
	QList<double> m_maxvals;
	PlotArea m_plot_area;
	QList<double> m_yrange_minvals,m_yrange_maxvals;
	QList<double> m_plot_offsets;
	QList<double> m_plot_minvals;
	QList<double> m_plot_maxvals;
	QList<double> m_plot_y1;
	QList<double> m_plot_y2;
	QList<QColor> m_channel_colors;
	QStringList m_channel_labels;
	QList<QColor> m_label_colors;
	QPixmap m_image;
	bool m_image_needs_update;
	//QMap<int,Mda *> m_multiscale_min_arrays;
	//QMap<int,Mda *> m_multiscale_max_arrays;
	int m_margins[4];
	bool m_uniform_vertical_channel_spacing;
  
	void set_data2();
	QColor get_channel_color(int num);
	void setup_plot_area();
};

SSTimeSeriesPlot::SSTimeSeriesPlot(QWidget *parent) : SSAbstractPlot(parent) {
	d=new SSTimeSeriesPlotPrivate();
	d->q=this;

	d->m_image_needs_update=false;

	d->m_data=0;
	d->m_labels=0;
	d->m_labels_is_owner=0;

	d->m_max_timepoint=0;
	d->m_num_channels=1;

	d->m_uniform_vertical_channel_spacing=true;

	QList<QString> color_strings;

	color_strings << "darkblue" << "darkgreen" << "darkred" << "darkcyan" << "darkmagenta" << "darkorange" << "black";
	for (int i=0; i<color_strings.size(); i++) {
		d->m_channel_colors << QColor(color_strings[i]);
	}

	color_strings.clear();
	color_strings << "#F7977A" << "#FDC68A"
				<< "#C4DF9B" << "#82CA9D"
				<< "#6ECFF6" << "#8493CA"
				<< "#A187BE" << "#F49AC2"
				<< "#F9AD81" << "#FFF79A"
				<< "#A2D39C" << "#7BCDC8"
				<< "#7EA7D8" << "#8882BE"
				<< "#BC8DBF" << "#F6989D";
	for (int i=0; i<color_strings.size(); i++) {
		d->m_label_colors << QColor(color_strings[i]);
	}

	d->m_plot_area.setMarkerColors(d->m_label_colors);
	QList<QString> mlabels;
	for (int i=0; i<5000; i++) mlabels << QString("%1").arg(i);
	d->m_plot_area.setMarkerLabels(mlabels);

	d->m_margins[0]=d->m_margins[1]=d->m_margins[2]=d->m_margins[3]=0;

	connect(this,SIGNAL(replotNeeded()),this,SLOT(slot_replot_needed()));
}

SSTimeSeriesPlot::~SSTimeSeriesPlot() {
	/*
	foreach (Mda *X,d->m_multiscale_min_arrays) {
		delete X;
	}
	foreach (Mda *X,d->m_multiscale_max_arrays) {
		delete X;
	}*/
	if (d->m_labels) {
		if (d->m_labels_is_owner)
			delete d->m_labels;
	}
	delete d;
}

void SSTimeSeriesPlot::updateSize()
{

	d->m_plot_area.setPosition(d->m_margins[0],d->m_margins[2]);
	d->m_plot_area.setSize(width()-d->m_margins[0]-d->m_margins[1], height()-d->m_margins[1]-d->m_margins[3]);

}

void SSTimeSeriesPlot::paintPlot(QPaintEvent *event) {

	Q_UNUSED(event);

	QPainter painter(this);

	updateSize();

	if ((width()!=d->m_image.width())||(height()!=d->m_image.height())) {
		d->m_image_needs_update=true;
	}

	if (d->m_image_needs_update) {
		d->m_image=QPixmap(width(),height());
		d->m_image.fill(QColor(0,0,0,0));
		QPainter painter2(&d->m_image);
		d->m_plot_area.refresh(&painter2);
		d->m_image_needs_update=false;
	}

	painter.drawPixmap(0,0,d->m_image);
}

void SSTimeSeriesPlot::setData(SSARRAY *data) {

	if (!data) {
		qWarning() << "Unexpected problem in SSTimeSeriesPlot::setData. data is null.";
		return;
	}
	d->m_data=data;
	d->m_max_timepoint=d->m_data->size(1)-1;
	d->m_num_channels=d->m_data->size(0);

	d->set_data2();

	d->m_image_needs_update=true;
	update();

}

void SSTimeSeriesPlot::setLabels(SSLabelsModel1 *L,bool is_owner) {

	d->m_labels=L;
	d->m_labels_is_owner=is_owner;
	d->m_image_needs_update=true;
	update();

}

void SSTimeSeriesPlot::initialize()
{
	d->setup_plot_area();
}

DiskArrayModel *SSTimeSeriesPlot::data()
{
	return d->m_data;
}

void SSTimeSeriesPlot::setChannelLabels(const QStringList &labels)
{
	d->m_channel_labels=labels;
}

void SSTimeSeriesPlot::setUniformVerticalChannelSpacing(bool val)
{
	d->m_uniform_vertical_channel_spacing=val;
	this->slot_replot_needed();
}

bool SSTimeSeriesPlot::uniformVerticalChannelSpacing()
{
	return d->m_uniform_vertical_channel_spacing;
}

void SSTimeSeriesPlotPrivate::set_data2() {

	int M = m_num_channels;
	int N = m_max_timepoint+1;

	q->setXRange(vec2(0,N-1));

	int scale0=1;
	while (N/scale0>100) scale0*=MULTISCALE_FACTOR;
	Mda tmp=m_data->loadData(scale0,0,N/scale0+1);
	Mda *data0=&tmp;

	m_minvals.clear();
	m_maxvals.clear();
	for (int ch=0; ch<M; ch++) {
		m_minvals << 0;
		m_maxvals << 0;
	}
	if (data0->size(1)>1) {
		for (int ch = 0; ch < M; ch++) {
			if (N > 0) {
				m_minvals[ch] = data0->value(ch, 0);
				m_maxvals[ch] = data0->value(ch, 0);
			}
			for (int i = 0; i < data0->size(1); i++) {
				double val = data0->value(ch, i);
				if (val < m_minvals[ch]) {
					m_minvals[ch] = val;
				}
				if (val > m_maxvals[ch]) {
					m_maxvals[ch] = val;
				}
			}
		}
	} else {
		for (int ch=0; ch<M; ch++) {
			m_minvals[ch]=0;
			m_maxvals[ch]=1;
		}
	}

	q->update();

}

QColor SSTimeSeriesPlotPrivate::get_channel_color(int ch) {
	if (m_channel_colors.size()==0) return QColor(0,0,0);
	return m_channel_colors[ch % m_channel_colors.size()];
}

void SSTimeSeriesPlotPrivate::setup_plot_area() {

	if (!m_data) return;

	QTime timer; timer.start();

	int M = m_num_channels;

	m_plot_offsets.clear();
	m_plot_minvals.clear();
	m_plot_maxvals.clear();
	m_plot_y1.clear();
	m_plot_y2.clear();
	for (int ch=0; ch<M; ch++) {
		m_plot_minvals << 0;
		m_plot_maxvals << 0;
		m_plot_offsets << 0;
		m_plot_y1 << 0;
		m_plot_y2 << 0;
	}

	if ((m_yrange_maxvals.size()>0)&&(m_yrange_maxvals.size()==M)) {
		for (int i=0; i<M; i++) {
			m_plot_minvals[i]=m_yrange_minvals[i]/q->verticalZoomFactor();
			m_plot_maxvals[i]=m_yrange_maxvals[i]/q->verticalZoomFactor();
		}
	} else if ((M>0)&&(m_minvals.size()>0)) {
		for (int i=0; i<M; i++) {
			m_plot_minvals[i]=m_minvals.value(i)/q->verticalZoomFactor();
			m_plot_maxvals[i]=m_maxvals.value(i)/q->verticalZoomFactor();
		}
	}

	double max00 = 0;
	for (int ch = 0; ch < M; ch++) {
		if (qAbs(m_plot_minvals[ch]) > max00) {
			max00 = qAbs(m_plot_minvals[ch]);
		}
		if (qAbs(m_plot_maxvals[ch]) > max00) {
			max00 = qAbs(m_plot_maxvals[ch]);
		}
	}

	if (m_uniform_vertical_channel_spacing) {
		if (m_plot_minvals.count()>0) {
			float global_plot_minval=m_plot_minvals[0];
			float global_plot_maxval=m_plot_maxvals[0];
			for (int i=0; i<m_plot_minvals.count(); i++) {
				if (m_plot_minvals[i]<global_plot_minval) global_plot_minval=m_plot_minvals[i];
				if (m_plot_maxvals[i]>global_plot_maxval) global_plot_maxval=m_plot_maxvals[i];
			}
			for (int i=0; i<m_plot_minvals.count(); i++) {
				m_plot_minvals[i]=global_plot_minval;
				m_plot_maxvals[i]=global_plot_maxval;
			}
		}
	}

	double offset = 0;
	if (!q->channelFlip()) { // ahb
	  for (int ch = 0; ch < M; ch++) { // jfm's upwards ordering
		m_plot_y1[ch]=offset;
		offset += (-m_plot_minvals[ch]);
		m_plot_offsets[ch] = offset;
		offset += m_plot_maxvals[ch];
		offset += max00 / 20;
		m_plot_y2[ch]=offset;
		}
	} else {
	  for (int ch = M-1; ch>=0; ch--) { // downwards ordering
		m_plot_y1[ch]=offset;
		offset += (-m_plot_minvals[ch]);
		m_plot_offsets[ch] = offset;
		offset += m_plot_maxvals[ch];
		offset += max00 / 20;
		m_plot_y2[ch]=offset;
	  }
	}

	m_plot_area.clearSeries();

	int xrange_min = q->xRange().x;
	int xrange_max = q->xRange().y;

	m_plot_area.setXRange(xrange_min - 1, xrange_max + 1);
	if (M>0) {
	  if (!q->channelFlip())      // ahb, matches above
	  		q->setYRange(vec2(m_plot_offsets[0] + m_plot_minvals[0] - max00 / 20, m_plot_offsets[M - 1] + m_plot_maxvals[M - 1] + max00 / 20));
	  else
       		q->setYRange(vec2(m_plot_offsets[M-1] + m_plot_minvals[M-1] - max00 / 20, m_plot_offsets[0] + m_plot_maxvals[0] + max00 / 20));
        }

	int NN=xrange_max-xrange_min+1;
	int msfactor=1;
	while (NN/msfactor/MULTISCALE_FACTOR>MIN_NUM_PIXELS) {
		msfactor*=MULTISCALE_FACTOR;
	}

	int x1,x2;
	if (msfactor==1) {
		x1=xrange_min;
		x2=xrange_max;
	}
	else {
		x1=(xrange_min/msfactor)*msfactor;
		x2=(xrange_max/msfactor)*msfactor;
	}

	Mda tmp=m_data->loadData(msfactor,x1/msfactor,x2/msfactor);
	for (int ch = 0; ch < M; ch++) {
		if (msfactor==1) {
			Mda xvals;
			xvals.allocate(1, x2 - x1 + 1);
			for (int x = x1; x <= x2; x++) {
				xvals.setValue(x,0, x - x1);
			}
			Mda yvals;
			yvals.allocate(1, x2 - x1 + 1);
			for (int ii = x1; ii <= x2; ii++) {
				double val=tmp.value(ch,ii-x1);
				yvals.setValue(val, 0,ii - x1);
			}
			QColor color = get_channel_color(ch);
			PlotSeries SS;
			SS.xvals=xvals; SS.yvals=yvals; SS.color=color; SS.offset=m_plot_offsets[ch];
			SS.plot_pairs=false;
			QString label0=m_channel_labels.value(ch);
			if (label0.isEmpty()) label0=QString("%1").arg(ch+1);
			SS.name=label0;
			m_plot_area.addSeries(SS);
		}
		else {

			Mda xvals;
			xvals.allocate(1, (x2 - x1)/msfactor*2);
			for (int x = x1; x < x2; x+=msfactor) {
				xvals.setValue(x,0, (x - x1)/msfactor*2);
				xvals.setValue(x,0, (x - x1)/msfactor*2+1);
			}
			Mda yvals;
			yvals.allocate(1, (x2-x1)/msfactor*2);
			for (int ii = x1; ii < x2; ii+=msfactor) {
				double val1=tmp.value(ch,(ii-x1)/msfactor,0);
				double val2=tmp.value(ch,(ii-x1)/msfactor,1);
				yvals.setValue(val1, 0,(ii - x1)/msfactor*2);
				yvals.setValue(val2, 0,(ii - x1)/msfactor*2+1);
			}
			QColor color = get_channel_color(ch);
			PlotSeries SS;
			SS.xvals=xvals; SS.yvals=yvals; SS.color=color; SS.offset=m_plot_offsets[ch];
			SS.plot_pairs=false;
			SS.name=QString("%1").arg(ch+1);
			m_plot_area.addSeries(SS);
		}
	}

	//the label markers
	m_plot_area.clearMarkers();
	int max_range_for_showing_labels=1e6;
	if ((m_labels)&&(x2-x1+1<=max_range_for_showing_labels)) {
		MemoryMda TL=m_labels->getTimepointsLabels(x1,x2);
		int K=TL.size(1);
		for (int i=0; i<K; i++) {
			m_plot_area.addMarker(TL.value(0,i)-1,TL.value(1,i));
		}
	}

	//split the clips
	if ((x2-x1+1<=max_range_for_showing_labels/100)&&(q->timepointMapping().totalSize()>1)) {
		DiskReadMda TM=q->timepointMapping();
		int last_time=0;
		for (int xx=x1; xx<=x2; xx++) {
			int tt=TM.value(0,xx);
			if ((xx>x1)&&(tt!=last_time+1)) {
				m_plot_area.addMarker(xx-0.5,0);
			}
			last_time=tt;
		}
	}

	m_image_needs_update=true;
	q->update();

}

Vec2 SSTimeSeriesPlot::coordToPix(Vec2 coord) {
	return d->m_plot_area.coordToPix(coord);
}

Vec2 SSTimeSeriesPlot::pixToCoord(Vec2 pix) {
	return d->m_plot_area.pixToCoord(pix);
}

int SSTimeSeriesPlot::pixToChannel(Vec2 pix) {
	Vec2 coord=pixToCoord(pix);
	for (int i=0; i<d->m_plot_offsets.count(); i++) {
		double y1=d->m_plot_offsets.value(i)+d->m_plot_minvals.value(i);
		double y2=d->m_plot_offsets.value(i)+d->m_plot_maxvals.value(i);
		if ((y1<=coord.y)&&(coord.y<=y2)) return i;
	}
	return -1;
}


void SSTimeSeriesPlot::setMargins(int left,int right,int top,int bottom) {
	d->m_margins[0]=left;
	d->m_margins[1]=right;
	d->m_margins[2]=top;
	d->m_margins[3]=bottom;
}

/*void SSTimeSeriesPlot::setConnectZeros(bool val)
{
	d->m_plot_area.setConnectZeros(val);
}*/

void SSTimeSeriesPlot::slot_replot_needed()
{

	d->setup_plot_area();

}

void SSTimeSeriesPlot::setXRange(const Vec2 &range) {
	if ((xRange().x!=range.x)||(xRange().y!=range.y)) {
		SSAbstractPlot::setXRange(range);
		d->setup_plot_area();
	}
}
void SSTimeSeriesPlot::setYRange(const Vec2 &range) {
	if ((yRange().x!=range.x)||(yRange().y!=range.y)) {
		SSAbstractPlot::setYRange(range);
		d->m_plot_area.setYRange(range.x,range.y);
	}
}
