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
	html+="Left-click an electrode to select the neuron that has the highest loading on that channel.";
	html+="</li>";

	html+="<li>";
	html+="Left-click that electrode again to select the next best neuron, and so on.";
	html+="</li>";

	html+="<li>";
	html+="Right-click an electrode to select/deselect. This controls which waveforms appear in the plot.";
	html+="</li>";

	html+="<li>";
	html+="Click animate to animate the selected waveform.";
	html+="</li>";

	html+="<li>";
	html+="Scroll up/down using the mouse wheel to manually move through the animation. Or move the slider. You can also click the waveforms to go to the corresponding timepoint.";
	html+="</li>";

	html+="<li>";
	html+="Click to select a neuron from the list.";
	html+="</li>";

	html+="<li>";
	html+="Show electrode numbers: Controls whether electrode channel numbers are displayed inside the circles.";
	html+="</li>";

	html+="<li>";
	html+="Auto select electrodes: Automatically select new electrodes when the current neuron changes.";
	html+="</li>";

	html+="<li>";
	html+="Normalize intensity per neuron: The intensity/brightness of the display is normalized based on the maximum absolute amplitude of the selected neuron.";
	html+="</li>";

	html+="<li>";
	html+="Brightness: Controls the window level for the color display.";
	html+="</li>";

	html+="<li>";
	html+="Uniform vertical spacing: The waveform plots are distributed uniformly in the vertical direction.";
	html+="</li>";

	html+="<li>";
	html+="Vertical scaling: Control the vertical scaling of the waveform plots.";
	html+="</li>";

	html+="</ul>";

	this->setHtml(html);

	this->setWindowTitle("FireTrack Help");

}

FTHelpWidget::~FTHelpWidget()
{
	delete d;
}

