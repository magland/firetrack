#include "fthelpwidget.h"

class FTHelpWidgetPrivate {
public:
	FTHelpWidget *q;
};


FTHelpWidget::FTHelpWidget(QWidget *parent) : QTextBrowser(parent)
{
	d=new FTHelpWidgetPrivate;
	d->q=this;

	QString html;
	html+="<h2>FireTrack Help</h2>";
	html+="<ul>";

	html+="<li>";
	html+="Left-click an electrode to select the waveform that has the highest loading on that channel.";
	html+="</li>";

	html+="<li>";
	html+="Left-click that electrode again to select the next best waveform, and so on.";
	html+="</li>";

	html+="<li>";
	html+="Right-click an electrode to select/deselect. This controls which waveforms appear in the plot.";
	html+="</li>";

	html+="<li>";
	html+="Click animate to animate the selected waveform. Click rewind to return to the beginning of the animation.";
	html+="</li>";

	html+="<li>";
	html+="Scroll up/down using the mouse wheel to manually move through the animation. Or move the slider. You can also click the waveforms to go to the corresponding point in the waveform.";
	html+="</li>";

	html+="<li>";
	html+="Click to select a waveform from the list.";
	html+="</li>";

	html+="<li>";
	html+="Use the options button to get some self-explanatory options.";
	html+="</li>";

	html+="<li>";
	html+="Use the help button to open this window.";
	html+="</li>";

	html+="</ul>";
	this->setHtml(html);

	this->setWindowTitle("FireTrack Help");

}

FTHelpWidget::~FTHelpWidget()
{
	delete d;
}

