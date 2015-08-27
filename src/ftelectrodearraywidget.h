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
	void setGlobalAbsMax(float val);

	void animate();
    void setAnimationSpeed(float hz);
    void setLoopAnimation(bool val);

	int timepoint();
	void setTimepoint(int t);
	QList<int> selectedElectrodeIndices();
	void setSelectedElectrodeIndices(const QList<int> &X);

	void setBrightness(float val);

	QWidget *optionsWidget();

protected:
	void wheelEvent(QWheelEvent *evt);

signals:
	void signalSelectedElectrodesChanged();
	void signalTimepointChanged();
	void signalElectrodeLeftClicked(int);
    void signalLoop();

private slots:
	void slot_animate();
	void slot_pause();
	void slot_timepoint_changed();
	void slot_slider_moved();
	void slot_slider_action_triggered();
	void slot_help();
	void slot_options_changed();

private:
	FTElectrodeArrayWidgetPrivate *d;

	~FTElectrodeArrayWidget();

signals:

public slots:
};

#endif // FTELECTRODEARRAYWIDGET_H
