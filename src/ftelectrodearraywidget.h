#ifndef FTELECTRODEARRAYWIDGET_H
#define FTELECTRODEARRAYWIDGET_H

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

private slots:
	void slot_animate();
	void slot_pause();
	void slot_rewind();
	void slot_timepoint_changed();
	void slot_slider_moved();
	void slot_slider_action_triggered();

private:
	FTElectrodeArrayWidgetPrivate *d;

	~FTElectrodeArrayWidget();

signals:

public slots:
};

#endif // FTELECTRODEARRAYWIDGET_H
