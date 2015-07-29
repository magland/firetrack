#ifndef FTHELPWIDGET_H
#define FTHELPWIDGET_H

#include <QTextBrowser>
#include <QWidget>


class FTHelpWidgetPrivate;

class FTHelpWidget : public QTextBrowser
{
	Q_OBJECT
public:
	friend class FTHelpWidgetPrivate;
	explicit FTHelpWidget(QWidget *parent = 0);
	~FTHelpWidget();

private:
	FTHelpWidgetPrivate *d;

signals:

public slots:
};

#endif // FTHELPWIDGET_H
