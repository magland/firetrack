#ifndef SSLABELVIEW_H
#define SSLABELVIEW_H

#include "ssabstractview.h"

class SSLabelViewPrivate;
class SSLabelView : public SSAbstractView
{
	Q_OBJECT
public:
	friend class SSLabelViewPrivate;
	SSLabelView();
	~SSLabelView();

	//Q_INVOKABLE void setLabels(DiskReadMda T,DiskReadMda L);
	Q_INVOKABLE void setLabels(DiskReadMda *TL,bool is_owner=false);
	Q_INVOKABLE void setLabels(SSLabelsModel *TL,bool is_owner=false);

	QString viewType();

private:

protected:
	SSAbstractPlot *plot();

private:
	SSLabelViewPrivate *d;
};

#endif // SSLABELVIEW_H
