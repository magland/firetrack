#ifndef FTELECTRODEARRAYVIEW_H
#define FTELECTRODEARRAYVIEW_H

#include <QWidget>
#include "mda.h"
#include "diskreadmda.h"


class FTElectrodeArrayViewPrivate;

class FTElectrodeArrayView : public QWidget
{
	Q_OBJECT
public:
	friend class FTElectrodeArrayViewPrivate;
	explicit FTElectrodeArrayView(QWidget *parent = 0);

	void setElectrodeLocations(const Mda &L);
	void setWaveform(const Mda &X);

	int timepoint();
	void setTimepoint(int val);

	void animate();
	void pauseAnimation();
	void rewindAnimation();
	void stopAnimation();
	bool isAnimating();

signals:
	void signalTimepointChanged();

protected:
	virtual void paintEvent(QPaintEvent *evt);
	virtual void mouseMoveEvent(QMouseEvent *evt);
	virtual void mousePressEvent(QMouseEvent *evt);

private slots:
	void slot_timer();

private:
	FTElectrodeArrayViewPrivate *d;

	~FTElectrodeArrayView();

signals:

public slots:
};

#endif // FTELECTRODEARRAYVIEW_H
