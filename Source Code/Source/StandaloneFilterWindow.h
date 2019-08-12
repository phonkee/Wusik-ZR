/*

	Wusik SQ200 (c) Wusik Dot Com 2018
	William Kalfelz
	www.Wusik.com
	www.Kalfelz.com

*/

#ifndef JUCE_STANDALONEFILTERWINDOW_H_INCLUDED
#define JUCE_STANDALONEFILTERWINDOW_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WAudioDeviceSelectorComponent.h"

extern AudioProcessor* JUCE_CALLTYPE createPluginFilter();  

// -------------------------------------------------------------------------------------------------------------------------------
class StandalonePluginHolder
   #if JUCE_IOS || JUCE_ANDROID
    : private Timer
   #endif
{
public:
    StandalonePluginHolder (const String& preferredDefaultDeviceName = String(),
                            const AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions = nullptr)
    {
        createPlugin();
        setupAudioDevices (preferredDefaultDeviceName, preferredSetupOptions);
		//
        startPlaying();
		//
        #if JUCE_IOS || JUCE_ANDROID
			startTimer (500);
        #endif
    }

	// -------------------------------------------------------------------------------------------------------------------------------
    virtual ~StandalonePluginHolder()
    {
		#if JUCE_IOS || JUCE_ANDROID
			stopTimer();
		#endif
		//
        deletePlugin();
        shutDownAudioDevices();
    }

	// -------------------------------------------------------------------------------------------------------------------------------
    virtual void createPlugin()
    {

		#if !JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
			processor = WusikZrAudioProcessor::createPluginFilterOfType(AudioProcessor::wrapperType_Standalone);
		#else
			AudioProcessor::setTypeOfNextNewPlugin (AudioProcessor::wrapperType_Standalone);
			processor = createPluginFilter();
			AudioProcessor::setTypeOfNextNewPlugin (AudioProcessor::wrapperType_Undefined);
		#endif
        jassert (processor != nullptr); // Your createPluginFilter() function must return a valid object!
		//
        processor->disableNonMainBuses();
        processor->setRateAndBufferSizeDetails(44100, 512);
		((WusikZrAudioProcessor*)processor.get())->ownerStandalone = (void*)this;
    }

	// -------------------------------------------------------------------------------------------------------------------------------
    virtual void deletePlugin()
    {
        stopPlaying();
        processor = nullptr;
    }

	// -------------------------------------------------------------------------------------------------------------------------------
    void startPlaying()
    {
        player.setProcessor (processor);
    }

	// -------------------------------------------------------------------------------------------------------------------------------
    void stopPlaying()
    {
        player.setProcessor (nullptr);
    }

	// -------------------------------------------------------------------------------------------------------------------------------
    void showAudioSettingsDialog(String title)
    {
		processor->suspendProcessing(true);
		//
		ScopedPointer<DialogWindow::LaunchOptions> o = new DialogWindow::LaunchOptions;
		//
		WAudioDeviceSelectorComponent* oA = new WAudioDeviceSelectorComponent(deviceManager, 2, 2, 2, 2, true, false, true, false);
		//
		o->content.setOwned(oA);
		o->content->setSize(520, 620);
		o->dialogTitle = title;
		o->dialogBackgroundColour = Colour(0xFF000000);
		o->escapeKeyTriggersCloseButton = false;
		o->useNativeTitleBar = true;
		o->resizable = true;
		//
		oA->owner = (void*)this;
		oA->parentWindow = o->launchAsync();
	}

	// -------------------------------------------------------------------------------------------------------------------------------
    void saveAudioDeviceState()
    {
		if (((WusikZrAudioProcessor*) processor.get())->dataFiles != nullptr
			&& ((WusikZrAudioProcessor*) processor.get())->dataFiles->dataFilesError.isEmpty())
        {
            ScopedPointer<XmlElement> xml (deviceManager.createStateXml());
			((WusikZrAudioProcessor*) processor.get())->dataFiles->userSettings->setValue("audioSetup", xml);
        }
		//
		processor->suspendProcessing(false);
    }

	// -------------------------------------------------------------------------------------------------------------------------------
    void reloadAudioDeviceState (const String& preferredDefaultDeviceName,
                                 const AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions)
    {
        ScopedPointer<XmlElement> savedState;
		if (((WusikZrAudioProcessor*) processor.get())->dataFiles != nullptr
			&& ((WusikZrAudioProcessor*) processor.get())->dataFiles->dataFilesError.isEmpty())
				savedState = ((WusikZrAudioProcessor*) processor.get())->dataFiles->userSettings->getXmlValue("audioSetup");
		//
        deviceManager.initialise (0,
                                  2,
                                  savedState,
                                  true,
                                  preferredDefaultDeviceName,
                                  preferredSetupOptions);
		//
		if (((WusikZrAudioProcessor*) processor.get())->dataFiles != nullptr
			&& ((WusikZrAudioProcessor*) processor.get())->dataFiles->dataFilesError.isEmpty())
		{
			AudioDeviceManager::AudioDeviceSetup xSetup;
			deviceManager.getAudioDeviceSetup(xSetup);
			//if (xSetup.outputChannels == 0) ((WusikZrAudioProcessor*)processor.get())->noAudioDevice = true;
		}
		//
		processor->suspendProcessing(false);
    }

	// -------------------------------------------------------------------------------------------------------------------------------
    ScopedPointer<AudioProcessor> processor;
    AudioDeviceManager deviceManager;
    AudioProcessorPlayer player;

   #if JUCE_IOS || JUCE_ANDROID
    StringArray lastMidiDevices;
   #endif

private:
	// -------------------------------------------------------------------------------------------------------------------------------
    void setupAudioDevices (const String& preferredDefaultDeviceName,
                            const AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions)
    {
        deviceManager.addAudioCallback (&player);
        deviceManager.addMidiInputCallback (String(), &player);
		//
        reloadAudioDeviceState (preferredDefaultDeviceName, preferredSetupOptions);
    }

	// -------------------------------------------------------------------------------------------------------------------------------
    void shutDownAudioDevices()
    {
        deviceManager.removeMidiInputCallback (String(), &player);
        deviceManager.removeAudioCallback (&player);
    }

    #if JUCE_IOS || JUCE_ANDROID
	// -------------------------------------------------------------------------------------------------------------------------------
    void timerCallback() override
    {
        StringArray midiInputDevices = MidiInput::getDevices();
        if (midiInputDevices != lastMidiDevices)
        {
            {
                const int n = lastMidiDevices.size();
                for (int i = 0; i < n; ++i)
                {
                    const String& oldDevice = lastMidiDevices[i];

                    if (! midiInputDevices.contains (oldDevice))
                    {
                        deviceManager.setMidiInputEnabled (oldDevice, false);
                        deviceManager.removeMidiInputCallback (oldDevice, &player);
                    }
                }
            }

            {
                const int n = midiInputDevices.size();
                for (int i = 0; i < n; ++i)
                {
                    const String& newDevice = midiInputDevices[i];

                    if (! lastMidiDevices.contains (newDevice))
                    {
                        deviceManager.addMidiInputCallback (newDevice, &player);
                        deviceManager.setMidiInputEnabled (newDevice, true);
                    }
                }
            }
        }
    }
   #endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandalonePluginHolder)
};
//
// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------
//
class StandaloneFilterWindow : public DocumentWindow
{
public:
    StandaloneFilterWindow (const String& title,
                            Colour backgroundColour,
                            const String& preferredDefaultDeviceName = String(),
                            const AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions = nullptr)
							: DocumentWindow(title, backgroundColour, DocumentWindow::minimiseButton | DocumentWindow::closeButton)
	{
		setTitleBarButtonsRequired(DocumentWindow::minimiseButton | DocumentWindow::closeButton, false);
        pluginHolder = new StandalonePluginHolder (preferredDefaultDeviceName, preferredSetupOptions);
		setUsingNativeTitleBar(true);
		xEditor = getAudioProcessor()->createEditorIfNeeded();
		setContentNonOwned(xEditor, true);
		centreWithSize(getWidth(), getHeight());
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void deleteWindow()
	{
		clearContentComponent();
		xEditor = nullptr;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    ~StandaloneFilterWindow()
    {
        pluginHolder->stopPlaying();
		deleteWindow();
        pluginHolder = nullptr;
    }

	// -------------------------------------------------------------------------------------------------------------------------------
    AudioProcessor* getAudioProcessor() const noexcept      { return pluginHolder->processor; }
    AudioDeviceManager& getDeviceManager() const noexcept   { return pluginHolder->deviceManager; }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    void closeButtonPressed() override
    {
		if (ConfirmBox("Close Wusik ZR?", "Are you sure you want to quit?"))
		{
			JUCEApplication::quit();
		}
    }
	//
    void resized() override
    {
        DocumentWindow::resized();
    }
	//
    ScopedPointer<StandalonePluginHolder> pluginHolder;
	ScopedPointer<AudioProcessorEditor> xEditor;
	WusikZrAudioProcessor* xEffect;
	//
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandaloneFilterWindow)
};
//
#endif   // JUCE_STANDALONEFILTERWINDOW_H_INCLUDED