#ifndef SSTIMESERIESPLOT_H
#define SSTIMESERIESPLOT_H

#include <QWidget>
#include "plotarea.h" //for Vec2
#include "sscommon.h"
#include "sslabelsmodel.h"
#include "ssabstractplot.h"
#include "sslabelsmodel1.h"

class SSTimeSeriesPlotPrivate;
class OverlayPainter;
class SSTimeSeriesPlot : public SSAbstractPlot
{
	Q_OBJECT
public:
	friend class SSTimeSeriesPlotPrivate;
	explicit SSTimeSeriesPlot(QWidget *parent = 0);
	~SSTimeSeriesPlot();

	//virtual from base class
	void updateSize();
	Vec2 coordToPix(Vec2 coord);
	Vec2 pixToCoord(Vec2 pix);
	void setXRange(const Vec2 &range);
	void setYRange(const Vec2 &range);
	void initialize();

	DiskArrayModel *data();

	void setChannelLabels(const QStringList &labels);
	void setUniformVerticalChannelSpacing(bool val);
	bool uniformVerticalChannelSpacing();


	void setData(SSARRAY *data);
	void setLabels(SSLabelsModel1 *L,bool is_owner=false);
	int pixToChannel(Vec2 pix);
	void setMargins(int left,int right,int top,int bottom);
	void setConnectZeros(bool val);

private slots:
	void slot_replot_needed();

private:
	SSTimeSeriesPlotPrivate *d;

protected:
	virtual void paintPlot(QPaintEvent *evt);

signals:

public slots:
};


#endif // SSTIMESERIESPLOT_H
