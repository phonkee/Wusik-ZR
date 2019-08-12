/*
BEGIN_JUCE_MODULE_DECLARATION
    ID:               WusikDEBUG
    vendor:           Wusik Dot Com
    version:          1.0.0
    name:             WusikDEBUG
    description:      Wusik Debug Window
    website:          https://www.Wusik.com
    dependencies:     
	license:          Open-Source
END_JUCE_MODULE_DECLARATION 
*/
//
#ifndef WUSIKDEBUG_H_INCLUDED
#define WUSIKDEBUG_H_INCLUDED
//
// -------------------------------------------------------------------------------------------------------------------------------
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_data_structures/juce_data_structures.h> 
//
using namespace juce;
//
#define WCREATE_DEBUG_WINDOW(ACTIVE) if (ACTIVE) debugWindow = new WDebug_Window(600, 600)
#define WDESTROY_DEBUG_WINDOW(ACTIVE) if (ACTIVE) debugWindow = nullptr
#define WCREATE_DEBUG_WINDOW_VARIABLE ScopedPointer<WDebug_Window> debugWindow

class WDebug_Window : public DocumentWindow, public Timer
{
public:
	WDebug_Window(int width, int heigth) : DocumentWindow("Wusik Debug", Colours::black.withAlpha(0.62f), 0, true), clearAll(false)
	{ 
		setSize(width, heigth);
		startTimer(10);
		setVisible(true);
		setAlwaysOnTop(true);
	};
	//
	void paint(Graphics& g) override
	{
		g.setColour(Colours::black.withAlpha(0.62f));
		g.fillAll();
		//
		g.setColour(Colours::white);
		g.drawFittedText(allMessage, getBounds(), Justification::centredTop, 999);
	}
	//
	void timerCallback() override
	{
		stopTimer();
		//
		if (clearAll)
		{
			clearAll = false;
			allMessage.clear();
		}
		else if (readyToDraw)
		{
			readyToDraw = false;
			allMessage = newMessage.joinIntoString("\n");
			newMessage.clear();
		}
		//
		repaint();
		startTimer(10);
	}
	//
	String allMessage;
	StringArray newMessage;
	bool clearAll;
	bool readyToDraw;
};
#endif