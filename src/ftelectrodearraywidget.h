#ifndef FTELECTRODEARRAYWIDGET_H
#define FTELECTRODEARRAYWIDGET_H

#include <QWheelEvent>
#include <QWidget>
#include "diskreadmda.h"
#include "mda.h"

class FTElectrodeArrayWidgetPrivate;

class FTElectrodeArrayWidget : public QWidget
{
	Q_OBJECT
public:
	friend class FTElectrodeArrayWidgetPrivate;
	explicit FTElectrodeArrayWidget(QWidget *parent = 0);

	void setElectrodeLocations(const Mda &L);
	void setWaveform(const Mda &X);

	void animate();

	int timepoint();
	void setTimepoint(int t);
	QList<int> selectedElectrodeIndices();
	void setSelectedElectrodeIndices(const QList<int> &X);

protected:
	void wheelEvent(QWheelEvent *evt);

signals:
	void signalSelectedElectrodesChanged();
	void signalTimepointChanged();
	void signalElectrodeLeftClicked(int);

private slots:
	void slot_animate();
	void slot_pause();
	void slot_rewind();
	void slot_timepoint_changed();
	void slot_slider_moved();
	void slot_slider_action_triggered();
	void slot_options();
	void slot_help();
	void slot_options_changed();

private:
	FTElectrodeArrayWidgetPrivate *d;

	~FTElectrodeArrayWidget();

signals:

public slots:
};

#endif // FTELECTRODEARRAYWIDGET_H
