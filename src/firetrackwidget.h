#ifndef FIRETRACKWIDGET_H
#define FIRETRACKWIDGET_H

#include <QMainWindow>
#include "diskreadmda.h"
#include "mda.h"

class FireTrackWidgetPrivate;

class FireTrackWidget : public QMainWindow
{
	Q_OBJECT
public:
	friend class FireTrackWidgetPrivate;
	explicit FireTrackWidget(QWidget *parent = 0);

	void setElectrodeLocations(const Mda &L);
	void setWaveforms(const Mda &X);

	Q_INVOKABLE void setWaveforms(DiskReadMda *X);
	Q_INVOKABLE void setElectrodeLocations(DiskReadMda *X);

private slots:
	void slot_current_waveform_changed();

private:
	FireTrackWidgetPrivate *d;

	~FireTrackWidget();

signals:

public slots:
};

#endif // FIRETRACKWIDGET_H
