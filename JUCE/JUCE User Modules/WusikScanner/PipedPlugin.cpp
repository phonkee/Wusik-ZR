/*

	Wusik SQ200 (c) Wusik Dot Com 2019
	William Kalfelz
	www.Wusik.com
	www.Kalfelz.com

*/

#ifndef WUSIKSCANNER_CPP_INCLUDED
#define WUSIKSCANNER_CPP_INCLUDED

#include "WusikScanner.h"

// -------------------------------------------------------------------------------------------------------------------------------
class PipedPluginInstance : public AudioPluginInstance,
                            private AsyncUpdater,
							public Timer
{
public:
	PipedPluginInstance(double _sampleRate, String _filename, int _uID, String _fontDetails, String _dataFolderLocale, bool _editorsOnTop, bool _is32Bits, bool _autoReloadOnCrash, AudioProcessor* _audioOwner)
	{
		hasChangedPresetsList = false;
		audioOwner = _audioOwner;
		sendAutomationSelection.set(0);
		receivedAutomationSelection.set(0);
		midiIN = true;
		midiOUT = false;
		hasReStarted = hasCrashed = false;
		samplerate = _sampleRate;
		pluginID = _uID;
		loadPlugin(_filename, _uID, fontDetails, _dataFolderLocale, _editorsOnTop, _is32Bits, _autoReloadOnCrash);
	}
	//
	void loadPlugin(String _filename, int uID, String _fontDetails, String _dataFolderLocale, bool _editorsOnTop, bool _is32Bits, bool _autoReloadOnCrash)
	{
		isProcessing = true;
		//
		getStateCounter = 0;
		filename = _filename;
		fontDetails = _fontDetails;
		dataFolderLocale = _dataFolderLocale;
		editorsOnTop = _editorsOnTop;
		is32Bits = _is32Bits;
		autoReloadOnCrash = _autoReloadOnCrash;
		//
		lastSamplesPerBlockExpected = 128;
		setPrepareToPlay = true;
		pluginHasEditor = false;
		parameterIndex = 0;
		numParameters = numPrograms = 9999;
		sendingEvent = false;
		//
		BusesLayout busProp;
		busProp.outputBuses.add(AudioChannelSet::stereo());
		busProp.inputBuses.add(AudioChannelSet::stereo());
		setBusesLayout(busProp);
		//
		String sheelBridge;
		String sheelBridge2 = String(Time::getHighResolutionTicks());
		//
		pipeProcessing.createNewPipe(sheelBridge2);
		pipeOthers.createNewPipe(sheelBridge2 + "XT");
		pipePing.createNewPipe(sheelBridge2 + "P");
		//
		String xTitleMessage = "Bridged";
		if (isWusikX42) xTitleMessage = "WusikX 42";
		//
		StringArray allCommandsSheel;
		#define CREATE_SHEEL_STRING allCommandsSheel.clear(); allCommandsSheel.add(sheelBridge);\
									allCommandsSheel.add("{" + sheelBridge2 + "}");\
								    allCommandsSheel.add("{" + filename + "}");\
									allCommandsSheel.add("{" + String(uID) + "}");\
									allCommandsSheel.add("{" + String(int(samplerate)) + "}");\
									allCommandsSheel.add("{" + fontDetails + "}");\
									allCommandsSheel.add("{" + dataFolderLocale + "}");\
									allCommandsSheel.add("{" + xTitleMessage + "}")
		//
		String xFileStart = "WusikEngineBridge";
		if (isWusikX42) xFileStart = "WusikX42Bridge";
		//
		#if JUCE_WINDOWS
			if (is32Bits)
			{
				sheelBridge = dataFolderLocale + "Bridge" + slash + xFileStart + "32_na.exe";
				CREATE_SHEEL_STRING;
				if (!childProcess.start(allCommandsSheel))
				{
					sheelBridge = String(dataFolderLocale + "Bridge" + slash + xFileStart + "32.exe");
					CREATE_SHEEL_STRING;
					if (!childProcess.start(allCommandsSheel))
					{
						MessageBox("Fatal Error!", "Internal Error Loading The Bridge File\n" + sheelBridge);
						return;
					}
				}
			}
			else
			{
				sheelBridge = dataFolderLocale + "Bridge" + slash + xFileStart + "64_na.exe";
				CREATE_SHEEL_STRING;
				if (!childProcess.start(allCommandsSheel))
				{
					sheelBridge = String(dataFolderLocale + "Bridge" + slash + xFileStart +  "64.exe");
					CREATE_SHEEL_STRING;
					if (!childProcess.start(allCommandsSheel))
					{
						MessageBox("Fatal Error!", "Internal Error Loading The Bridge File\n" + sheelBridge);
						return;
					}
				}
			}
		#else
			sheelBridge = String(dataFolderLocale + "Bridge" + slash + xFileStart);
			CREATE_SHEEL_STRING;
			if (!childProcess.start(allCommandsSheel))
			{
				MessageBox("Fatal Error!", "Internal Error Loading The Bridge File\n" + sheelBridge);
				return;
			}
		#endif
		//
		currentPositionInfo.bpm = 120.0;
		currentPositionInfo.ppqPosition = 0;
		currentPositionInfo.isPlaying = false;
		//
		int64 xCommand = 0; 
		charCommand = 0;
		//
		pipeProcessing.read(&charCommand, sizeof(char), 2000);
		pluginHasEditor = ((charCommand & 1) == 1);
		midiIN = (((charCommand >> 1) & 1) == 1);
		midiOUT = (((charCommand >> 2) & 1) == 1);
		//
		// ------- Get Basic Info ------- //
		//
		pipeProcessing.read(&numParameters, sizeof(int16), 2000);
		pipeProcessing.read(&numPrograms, sizeof(int16), 2000);
		//
		MemoryBlock theDataBlock;
		int64 dataSize = 0;
		pipeProcessing.read(&dataSize, sizeof(int64), 2000);
		theDataBlock.setSize(dataSize);
		pipeProcessing.read(theDataBlock.getData(), dataSize, 12000);
		//
		MemoryInputStream inputStreamBlock(theDataBlock, true);
		GZIPDecompressorInputStream zipOutput(inputStreamBlock);
		//
		for (int x = 0; x < numParameters; x++) 
		{ 
			parameterNames.add(zipOutput.readString());
			parameterLabel.add(zipOutput.readString());
		}
		for (int x = 0; x < numPrograms; x++) 
		{ 
			programNames.add(zipOutput.readString());
		}
		//
		theDataBlock.reset();
		//
		if (editorsOnTop) sendEvent(kEvent_SetEditorAlwaysOnTop);
		//
		if (lastState.getSize() > 0)
		{
			sendControllByte(true);
			//
			char theEvent = kEvent_SetState;
			pipeOthers.write(&theEvent, sizeof(char), 8000);
			//
			int64 xSize = lastState.getSize();
			pipeOthers.write(&xSize, sizeof(int64), 8000);
			pipeOthers.write(lastState.getData(), xSize, 8000);
			//
			sendControllByte(false);
		}
		//
		pingThread = new PingThread((void*) this);
		pingThread->startThread(4);
		//
		startTimer(1000);
		isProcessing = false;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void timerCallback()
	{
		stopTimer();
		//
		if (!childProcess.isRunning())
		{
			if (autoReloadOnCrash)
			{
				loadPlugin(filename, pluginID, fontDetails, dataFolderLocale, editorsOnTop, is32Bits, autoReloadOnCrash);
				hasReStarted = true;
				hasCrashed = false;
			}
			else hasCrashed = true;
			//
			return;
		}
		//
		getStateCounter++;
		if (getStateCounter > 20000)
		{
			getStateCounter = 0;
			//
			if (childProcess.isRunning() && getActiveEditor() != nullptr)
			{
				MemoryBlock xState;
				getStateInformation(xState);
				if (xState.getSize() > 0) lastState.swapWith(xState);
			}
		}
		//
		int theAutomationSet = 0;
		if (receivedAutomationSelection.get() == 1) theAutomationSet = 1;
		//
		if (receivedAutomations[theAutomationSet].size() > 0)
		{
			ARRAY_Iterator(receivedAutomations[theAutomationSet])
			{
				audioOwner->beginParameterChangeGesture(receivedAutomations[theAutomationSet].getUnchecked(index)->parameter);
				audioOwner->setParameterNotifyingHost(receivedAutomations[theAutomationSet].getUnchecked(index)->parameter, receivedAutomations[theAutomationSet].getUnchecked(index)->value);
				audioOwner->endParameterChangeGesture(receivedAutomations[theAutomationSet].getUnchecked(index)->parameter);
			}
			//
			receivedAutomations[theAutomationSet].clear();
		}
		//
		startTimer(1);
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	~PipedPluginInstance()
    {
		isProcessing = true;
		stopTimer();
		pingThread->signalThreadShouldExit();
		Thread::sleep(600);
		//
		if (childProcess.isRunning()) childProcess.kill();
		//
		pipeProcessing.close();
		pipeOthers.close();
		pipePing.close();
    }
	//
	void fillInPluginDescription(PluginDescription& desc) const override { };
	double getTailLengthSeconds() const override { return 0.0; };
	void handleAsyncUpdate() override { };
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void sendControllByte(bool _start)
	{
		if (childProcess.isRunning())
		{
			if (_start)
			{
				int xCounter = 0;
				while (sendingEvent.get())
				{
					xCounter++;
					if (xCounter > 1000)
					{
						if (childProcess.isRunning()) childProcess.kill();
						sendingEvent = false;
						return;
					}
					//
					if (!childProcess.isRunning()) break;
					//
					Thread::sleep(10);
				};
				//
				if (!childProcess.isRunning()) return;
				//
				sendingEvent = true;
				char startCommand = 42;
				pipeOthers.write(&startCommand, sizeof(char), 12000);
			}
			else
			{
				char endCommand = 0;
				while (endCommand != 48)
				{
					if (pipeOthers.read(&endCommand, sizeof(char), 12000) == -1 || !childProcess.isRunning())
					{
						sendingEvent = false;
						return;
					}
				}
				sendingEvent = false;
			}
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	float getEventFloat(char theEvent, int16 sendValue = -1)
	{
		sendControllByte(true);
		//
		float returnFloat = 0.0f;
		//
		if (childProcess.isRunning())
		{
			pipeOthers.write(&theEvent, sizeof(char), 2000);
			if (sendValue != -1) pipeOthers.write(&sendValue, sizeof(int16), 2000);
			pipeOthers.read(&returnFloat, sizeof(float), 2000);
		}
		//
		sendControllByte(false);
		return returnFloat;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void sendEventFloat(char theEvent, int16 sendValue, float sendFloat)
	{
		sendControllByte(true);
		//
		if (childProcess.isRunning())
		{
			pipeOthers.write(&theEvent, sizeof(char), 2000);
			pipeOthers.write(&sendValue, sizeof(int16), 2000);
			pipeOthers.write(&sendFloat, sizeof(float), 2000);
		}
		//
		sendControllByte(false);
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void sendEvent(char theEvent)
	{
		sendControllByte(true);
		//
		if (childProcess.isRunning()) pipeOthers.write(&theEvent, sizeof(char), 2000);
		//
		sendControllByte(false);
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void sendEvent(char theEvent, int16 xValue)
	{
		sendControllByte(true);
		//
		if (childProcess.isRunning())
		{
			pipeOthers.write(&theEvent, sizeof(char), 2000);
			pipeOthers.write(&xValue, sizeof(int16), 2000);
		}
		//
		sendControllByte(false);
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	int16 getEvent(char theEvent)
	{
		sendControllByte(true); 
		//
		int16 xValue = 0;
		if (childProcess.isRunning())
		{
			pipeOthers.write(&theEvent, sizeof(char), 2000);
			pipeOthers.read(&xValue, sizeof(int16), 2000);
		}
		//
		sendControllByte(false);
		return xValue;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    const String getName() const override
    {
		return String();
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    int getUID() const
    {
        return 0;
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    bool acceptsMidi() const override    
	{ 
		return midiIN;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    bool producesMidi() const override   
	{ 
		return midiOUT; 
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void prepareToPlay(double _rate, int _samplesPerBlockExpected) override
	{
		samplerate = _rate;
		lastSamplesPerBlockExpected = _samplesPerBlockExpected;
		setPrepareToPlay = true;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    void releaseResources() override
    {
		if (childProcess.isRunning()) sendEvent(kEvent_ReleaseResources);
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    void reset() override
    {
		if (childProcess.isRunning()) sendEvent(kEvent_Reset);
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	inline void sendProcessCommand(char theCommand)
	{
		pipeProcessing.write(&theCommand, sizeof(char), 1000);
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override
    {
		if (isProcessing || !childProcess.isRunning()) return;
		isProcessing = true;
		//
		if (setPrepareToPlay)
		{
			setPrepareToPlay = false;
			//
			sendProcessCommand(kPrepareToPlay);
			if (!childProcess.isRunning()) return;
			pipeProcessing.write(&samplerate, sizeof(double), 1000);
			if (!childProcess.isRunning()) return;
			pipeProcessing.write(&lastSamplesPerBlockExpected, sizeof(int64), 1000);
			if (!childProcess.isRunning()) return;
		}
		//
		float xMagnitude = 0.0f;
		if (buffer.getNumChannels() > 0) xMagnitude = buffer.getMagnitude(0, buffer.getNumSamples()); else buffer.clear();
		//
		sendProcessCommand(kProcess);
		if (!childProcess.isRunning()) return;
		//
		int theAutomation = sendAutomationSelection.get();
		if (sendAutomationSelection.get() == 0) sendAutomationSelection.set(1); else sendAutomationSelection.set(0);
		//
		int16 xSize = sendAutomations[theAutomation].size();
		pipeProcessing.write(&xSize, sizeof(int16), 1000);
		for (int xa = 0; xa < xSize; xa++)
		{
			pipeProcessing.write(sendAutomations[theAutomation].getUnchecked(xa), sizeof(AutoMations), 1000);
			if (!childProcess.isRunning()) return;
		}
		sendAutomations[theAutomation].clear();
		//
		xSize = buffer.getNumSamples();
		pipeProcessing.write(&xSize, sizeof(int16), 1000);
		if (!childProcess.isRunning()) return;
		//
		char xChannels = buffer.getNumChannels();
		pipeProcessing.write(&xChannels, sizeof(char), 1000);
		if (!childProcess.isRunning()) return;
		//
		char hasAudioInput = 0;
		if (xMagnitude > 0.0f) hasAudioInput = 1;
		pipeProcessing.write(&hasAudioInput, sizeof(char), 1000);
		//
		if (xMagnitude > 0.0f)
		{
			for (int xc = 0; xc < xChannels; xc++)
			{
				pipeProcessing.write(buffer.getReadPointer(xc), sizeof(float) * buffer.getNumSamples(), 1000);
			}
		}
		//
		xSize = midiMessages.data.size();
		pipeProcessing.write(&xSize, sizeof(int16), 1000);
		if (!childProcess.isRunning()) return;
		pipeProcessing.write(midiMessages.data.getRawDataPointer(), xSize * sizeof(uint8), 1000);
		if (!childProcess.isRunning()) return;
		//
		if (getPlayHead() != nullptr) getPlayHead()->getCurrentPosition(currentPositionInfo);
		pipeProcessing.write(&currentPositionInfo.bpm, sizeof(double), 1000);
		if (!childProcess.isRunning()) return;
		pipeProcessing.write(&currentPositionInfo.ppqPosition, sizeof(double), 1000);
		if (!childProcess.isRunning()) return;
		pipeProcessing.write(&currentPositionInfo.isPlaying, sizeof(bool), 1000);
		if (!childProcess.isRunning()) return;
		//
		xSize = buffer.getNumSamples();
		for (int xc = 0; xc < xChannels; xc++)
		{
			pipeProcessing.read(buffer.getWritePointer(xc, 0), xSize * sizeof(float), 1000);
			if (!childProcess.isRunning()) return;
		}
		//
		int theAutomationSet = 0;
		if (receivedAutomationSelection.get() == 0) theAutomationSet = 1;
		//
		AutoMations automation(0.0f, 0);
		pipeProcessing.read(&xSize, sizeof(int16), 1000);
		for (int x = 0; x < xSize; x++)
		{
			pipeProcessing.read(&automation, sizeof(AutoMations), 1000);
			//
			if (audioOwner != nullptr)
			{
				receivedAutomations[theAutomationSet].add(new AutoMations(automation.value, automation.parameter));
			}
			else
			{
				beginParameterChangeGesture(automation.parameter);
				setParameterNotifyingHost(automation.parameter, automation.value);
				endParameterChangeGesture(automation.parameter);
			}
			//
			if (!childProcess.isRunning()) return;
		}
		//
		if (xSize > 0)
		{
			if (receivedAutomationSelection.get() == 0) receivedAutomationSelection.set(1); else receivedAutomationSelection.set(0);
		}
		//
		isProcessing = false;
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	bool supportsDoublePrecisionProcessing() const override { return false; }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    bool canAddBus (bool) const override                                       { return false; }
    bool canRemoveBus (bool) const override                                    { return false; }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        const int numInputBuses  = getBusCount (true);
        const int numOutputBuses = getBusCount (false);

        // it's not possible to change layout if there are sidechains/aux buses
        if (numInputBuses > 1 || numOutputBuses > 1)
            return (layouts == getBusesLayout());

        return (layouts.getNumChannels (true,  0) <= 2
             && layouts.getNumChannels (false, 0) <= 2);
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    bool hasEditor() const override                  
	{ 
		return pluginHasEditor;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	AudioProcessorEditor* createEditor() override
	{
		sendEvent(kEvent_OpenEditor);
		return nullptr;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    const String getInputChannelName (int index) const override
    {
        return "Input";
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    bool isInputChannelStereoPair (int index) const override
    {
        return true;
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    const String getOutputChannelName (int index) const override
    {
        return "Out";
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    bool isOutputChannelStereoPair (int index) const override
    {
        return true;
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    bool isValidChannel (int index, bool isInput) const noexcept
    {
        return isPositiveAndBelow (index, isInput ? 2 : 2);
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	int getNumParameters() override
	{
		return numParameters;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    float getParameter (int index) override
    {
		if (index < 0)
		{
			if (index == PipedPluginFormat::kCloseEditor)
			{
				closeEditor();
			}
			else if (index == PipedPluginFormat::kReloadPlugin)
			{
				hasCrashed = false;
				loadPlugin(filename, pluginID, fontDetails, dataFolderLocale, editorsOnTop, is32Bits, autoReloadOnCrash);
				hasReStarted = true;
			}
			else if (index == PipedPluginFormat::kHasReStarted)
			{
				if (hasReStarted)
				{
					hasReStarted = false;
					return 1.0f;
				}
				return 0.0f;
			}
			else if (index == PipedPluginFormat::kHasCrashed)
			{
				if (hasCrashed)
				{
					hasCrashed = false;
					return 1.0f;
				}
				return 0.0f;
			}
			else if (index == PipedPluginFormat::kOpenEditor)
			{
				sendEvent(kEvent_OpenEditor);
				return 1.0f;
			}
			else if (index == PipedPluginFormat::kHasChangedPresetsList)
			{
				if (hasChangedPresetsList)
				{
					hasChangedPresetsList = false;
					return 1.0f;
				}
				else 0.0f;
			}
			else if (index == PipedPluginFormat::kSaveFXP) { sendEvent(kEvent_SaveFXP); return 0.0f; }
			else if (index == PipedPluginFormat::kSaveFXB) { sendEvent(kEvent_SaveFXB); return 0.0f; }
			else if (index == PipedPluginFormat::kLoadFXP) { sendEvent(kEvent_LoadFXP); return 0.0f; }
			else if (index == PipedPluginFormat::kLoadFXB) { sendEvent(kEvent_LoadFXB); return 0.0f; }
			//
			else if (index == PipedPluginFormat::kGetWindowsX) return getEvent(kEvent_GetWindowPosX);
			else if (index == PipedPluginFormat::kGetWindowsY) return getEvent(kEvent_GetWindowPosY);
			else if (index == PipedPluginFormat::kGetEditorWidth) return getEvent(kEvent_GetEditorWidth);
			else if (index == PipedPluginFormat::kGetEditorHeigth) return getEvent(kEvent_GetEditorHeigth);
			else if (index == PipedPluginFormat::kGetEditorHandle)
			{
				sendControllByte(true);
				//
				uint32_t returnFloat = 0.0f;
				//
				if (childProcess.isRunning())
				{
					char theEvent = kEvent_GetEditorHandle;
					pipeOthers.write(&theEvent, sizeof(char), 2000);
					pipeOthers.read(&returnFloat, sizeof(uint32_t), 2000);
				}
				//
				sendControllByte(false);
				return (float) returnFloat;
			}
		}
		else
		{
			if (childProcess.isRunning()) return getEventFloat(kEvent_GetParameterValue, index);
		}
		//
		return 0.0f;
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    void setParameter (int index, float newValue) override
    {
		if (index < 0)
		{
			if (index == PipedPluginFormat::kSetWindowsX)
			{
				sendEvent(kEvent_SetWindowPosX, int16(newValue));
				return;
			}
			else if (index == PipedPluginFormat::kSetWindowsY)
			{
				sendEvent(kEvent_SetWindowPosY, int16(newValue));
				return;
			}
			else if (index == PipedPluginFormat::kSetOverSamplingTimes)
			{
				sendEvent(kEvent_SetOversamplingTimes, int16(newValue));
				return;
			}
		}
		else
		{	
			int theAutomation = sendAutomationSelection.get();
			sendAutomations[theAutomation].add(new AutoMations(newValue, index));
		}
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	const String getParameterName(int index) override      
	{ 
		if (index < parameterNames.size()) return parameterNames[index];
		//
		return String();
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    const String getParameterText (int index) override       
	{ 
		sendControllByte(true);
		//
		int16 xSize = 0;
		String returnString;
		//
		if (childProcess.isRunning())
		{
			int16 index16 = int16(index);
			//
			charCommand = kEvent_GetParameterText;
			pipeOthers.write(&charCommand, sizeof(char), 2000);
			pipeOthers.write(&index16, sizeof(int16), 2000);
			pipeOthers.read(&xSize, sizeof(int16), 2000);
			//
			for (int x = 0; x < xSize; x++)
			{
				char xS[1];
				pipeOthers.read(&xS, 1, 400);
				returnString.append(String::fromUTF8(xS, 1), 999);
				if (!childProcess.isRunning()) break;
			}
		}
		//
		sendControllByte(false);
		//
		return returnString;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    String getParameterLabel (int index) const override      
	{ 
		if (index < parameterLabel.size()) return parameterLabel[index];
		//
		return String();
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    bool isParameterAutomatable (int index) const override
    {
        return true;
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    int getNumPrograms() override         
	{
		return numPrograms;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    int getCurrentProgram() override       
	{ 
		if (childProcess.isRunning()) return getEvent(kEvent_GetProgram);
		//
		return 0;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    void setCurrentProgram (int newIndex) override
    {
		if (childProcess.isRunning()) sendEvent(kEvent_SetProgram, int16(newIndex));
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    const String getProgramName (int index) override
    {
		if (index < programNames.size()) return programNames[index];
		//
		return String();
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    void changeProgramName (int index, const String& newName) override
    {
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void getStateInformation(MemoryBlock& mb) override
	{
		if (childProcess.isRunning())
		{
			sendControllByte(true);
			//
			char theEvent = kEvent_GetState;
			pipeOthers.write(&theEvent, sizeof(char), 8000);
			//
			int64 xSize = 0;
			pipeOthers.read(&xSize, sizeof(int64), 8000);
			//
			if (xSize > 0)
			{
				mb.setSize(xSize);
				pipeOthers.read(mb.getData(), xSize, 8000);
			}
			//
			sendControllByte(false);
		}
		else if (lastState.getSize() > 0)
		{
			mb.append(lastState.getData(), lastState.getSize());
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    void setStateInformation (const void* data, int size) override               
	{ 
		lastState.reset();
		lastState.append(data, size);
		//
		if (childProcess.isRunning())
		{
			sendControllByte(true);
			//
			char theEvent = kEvent_SetState;
			pipeOthers.write(&theEvent, sizeof(char), 8000);
			//
			int64 xSize = size;
			pipeOthers.write(&xSize, sizeof(int64), 8000);
			pipeOthers.write(data, xSize, 8000);
			//
			sendControllByte(false);
		}
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    int getVersionNumber() const noexcept  
	{ 
		return 0;
	}
	//
	// -------------------------------------------------------------------------------------------------------------------------------
    String getVersion() const
    {
        return String();
    }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	void closeEditor() 
	{
		if (childProcess.isRunning())
		{
			if (getEvent(kEvent_IsEditorOpen) == 1)
			{
				stopTimer();
				Thread::sleep(200);
				sendEvent(kEvent_CloseEditor);
				getStateCounter = 0;
				//
				MemoryBlock xState;
				getStateInformation(xState);
				if (xState.getSize() > 0) lastState.swapWith(xState);
				//
				startTimer(500);
			}
		}
	};
	//
	char charCommand;
	void* owner;
	NamedPipe pipeProcessing, pipeOthers, pipePing;
	bool isProcessing;
	double samplerate;
	int64 lastSamplesPerBlockExpected;
	bool setPrepareToPlay;
	bool pluginHasEditor;
	int64 parameterIndex, numParameters, numPrograms;
	StringArray parameterNames;
	StringArray programNames;
	StringArray parameterLabel;
	Atomic<bool> sendingEvent;
	AudioPlayHead::CurrentPositionInfo currentPositionInfo;
	ChildProcess childProcess;
	bool autoReloadOnCrash;
	String filename;
	String fontDetails;
	String dataFolderLocale;
	bool editorsOnTop;
	bool is32Bits;
	MemoryBlock lastState;
	int getStateCounter;
	bool hasReStarted, hasCrashed;
	bool midiIN, midiOUT;
	ScopedPointer<PingThread> pingThread;
	OwnedArray<AutoMations> sendAutomations[2];
	Atomic<int> sendAutomationSelection;
	AudioProcessor* audioOwner;
	OwnedArray<AutoMations> receivedAutomations[2];
	Atomic<int> receivedAutomationSelection;
	int pluginID;
	bool hasChangedPresetsList;
	//
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PipedPluginInstance)
};

// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------------------------------
PipedPluginFormat::PipedPluginFormat(String _theFormat, String _fontDetails, String _dataFolderLocale, bool _editorsOnTop, bool _autoReloadOnCrash, AudioProcessor* _audioOwner) :
	fontDetails(_fontDetails), dataFolderLocale(_dataFolderLocale), editorsOnTop(_editorsOnTop), autoReloadOnCrash(_autoReloadOnCrash), audioOwner(_audioOwner), theFormat(_theFormat)
{
	//
}
//
// -------------------------------------------------------------------------------------------------------------------------------
PipedPluginFormat::~PipedPluginFormat()
{

}
//
// -------------------------------------------------------------------------------------------------------------------------------
void PipedPluginFormat::findAllTypesForFile(OwnedArray<PluginDescription>& results,
                                           const String& fileOrIdentifier)
{
	//MessageBox("findAllTypesForFile", fileOrIdentifier);
}
//
// -------------------------------------------------------------------------------------------------------------------------------
void PipedPluginFormat::createPluginInstance (const PluginDescription& desc,
                                            double sampleRate,
                                            int blockSize,
                                            void* userData,
                                            void (*callback) (void*, AudioPluginInstance*, const String&))
{
	String fileNameDesc = desc.fileOrIdentifier.replace(".32bits", "", true).replace(".64bits", "", true);
	ScopedPointer<PipedPluginInstance> result = new PipedPluginInstance(sampleRate, fileNameDesc, desc.uid, fontDetails, dataFolderLocale, editorsOnTop, desc.fileOrIdentifier.containsIgnoreCase(".32bits"), autoReloadOnCrash, audioOwner);
	//
    String errorMsg;
    callback (userData, result.release(), errorMsg);
}
//
// -------------------------------------------------------------------------------------------------------------------------------
bool PipedPluginFormat::fileMightContainThisPluginType(const String& fileOrIdentifier)
{
	return fileOrIdentifier.containsIgnoreCase(".32bits") || fileOrIdentifier.containsIgnoreCase(".64bits");
}
//
// -------------------------------------------------------------------------------------------------------------------------------
bool PipedPluginFormat::pluginNeedsRescanning(const PluginDescription& desc)
{
	String fileOriginal = desc.fileOrIdentifier.replace(".32bits", "", true).replace(".64bits", "", true);
    return File (fileOriginal).getLastModificationTime() != desc.lastFileModTime;
}
//
// -------------------------------------------------------------------------------------------------------------------------------
bool PipedPluginFormat::doesPluginStillExist(const PluginDescription& desc)
{
	String fileOriginal = desc.fileOrIdentifier.replace(".32bits", "", true).replace(".64bits", "", true);
    return File (fileOriginal).exists();
}
//
// -------------------------------------------------------------------------------------------------------------------------------
void PingThread::run()
{
	int64 presetsSize = 0;
	char pingValue = 88;
	while (!threadShouldExit())
	{
		sleep(400);
		pingValue = 88;
		((PipedPluginInstance*)owner)->pipePing.write(&pingValue, sizeof(char), 400);
		((PipedPluginInstance*)owner)->pipePing.read(&pingValue, sizeof(char), 2000);
		//
		if (pingValue == 1)
		{
			sleep(400);
			MemoryBlock theDataBlock;
			((PipedPluginInstance*)owner)->pipePing.read(&presetsSize, sizeof(int64), 4000);
			theDataBlock.setSize(presetsSize);
			((PipedPluginInstance*)owner)->pipePing.read(theDataBlock.getData(), presetsSize, 12000);
			//
			MemoryInputStream inputStreamBlock(theDataBlock, true);
			GZIPDecompressorInputStream zipOutput(inputStreamBlock);
			//
			for (int x = 0; x < ((PipedPluginInstance*)owner)->numPrograms; x++)
			{
				((PipedPluginInstance*)owner)->programNames.set(x, zipOutput.readString());
			}
			//
			theDataBlock.reset();
			((PipedPluginInstance*)owner)->hasChangedPresetsList = true;
		}
	}
}
//
#endif