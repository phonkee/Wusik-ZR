/*

	Wusik SQ200 (c) Wusik Dot Com 2018
	William Kalfelz
	www.Wusik.com
	www.Kalfelz.com

*/

#include "StandaloneFilterWindow.h"
#include "../JuceLibraryCode/JuceHeader.h"

class StandaloneFilter : public JUCEApplication
{
public:
	StandaloneFilter() { }
	//
	void initialise(const String& commandLineParameters)
	{
		filterWindow = new StandaloneFilterWindow("Wusik ZR", Colours::black);
		juce::Thread::setCurrentThreadName("Wusik ZR");
		//
		filterWindow->setTitleBarButtonsRequired(DocumentWindow::minimiseButton | DocumentWindow::closeButton, false);
		filterWindow->setVisible (true);
		filterWindow->setResizable(false, false);
	}
	void shutdown()
	{
		filterWindow = nullptr;
	}
	const String getApplicationName() 
	{
		return String("Wusik ZR");
	}
	const String getApplicationVersion()
	{
		return String(JucePlugin_VersionString);
	}
	bool moreThanOneInstanceAllowed()
	{
		return false;
	}
	//
private:
	ScopedPointer<StandaloneFilterWindow> filterWindow;
};
//
START_JUCE_APPLICATION (StandaloneFilter)