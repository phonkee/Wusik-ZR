// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

	Created by William Kalfelz @ Wusik Dot Com (c) 2019

	This product is not FREE. The source code provided here is only for educational means. To use this product you still need a valid license purchased from Wusik.com
	Also, to re-use this code you need a commercial license from Wusik.com, contact support for details.
	Do not share binaries of this code in any way, unless you have a license from Wusik.com that fits this need.
	Do not distribute this code in any way, but feel free to learn from it.
	In order to compile this code you need to download JUCE and understand how to use it.
	https://github.com/WeAreROLI/JUCE
	I have provided videos on how to do that on my Youtube Channel, starting with the video below.
	https://youtu.be/yqAt3Qo1Lv8

*/

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "DataStructure.h"
#include "Sequencer.h"
//
#define WUSIKZR_VERSION "V1.0.0"
//
#define DEFAULT_BANK_LOAD 1
#define DEFAULT_ENVELOPE kEditEnvelope_Amplitude
#define WDEBUGTOFILE 0
//
#if JUCE_WINDOWS
	/** Special Macro to declare a class aligned - specially used for SSE support */
	#define DECLARE_ALIGNED(bits) void *operator new(size_t size){ return _aligned_malloc(size, bits); }\
		void operator delete(void *ptr) { _aligned_free(ptr); }
#else
	#define DECLARE_ALIGNED(bits) 
#endif
//
#define LAYER_ADD_BOOST1 0.0f
#define LAYER_ADD_BOOST2 0.12f
#define LAYER_ADD_BOOST4 0.22f
#define LAYER_MUL_BOOST1 0.32f
#define LAYER_MUL_BOOST2 0.42f
#define LAYER_MUL_BOOST4 0.62f
//
#if !WDEBUGTOFILE
	#define DEBUG_TO_FILE(message)
#else
	#define DEBUG_TO_FILE(message)\
	if (1)\
	{\
		String threadDebugName;\
		if (Thread::getCurrentThread() != nullptr) threadDebugName = Thread::getCurrentThread()->getThreadName() + " | ";\
		File debugFile("c:\\temp\\debug.txt");\
		debugFile.appendText(threadDebugName + Time::getCurrentTime().toISO8601(true) + " >> " + message + "\n");\
	}
#endif
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum
{
	kAddStaticLayer = 0,
	kAddSequencedLayer,
	//
	kActionNone = 0,
	kActionUpdateEverything,
	kActionSetState,
	kActionLoadNewEffect,
	kActionUnloadEffect,
	kActionMoveEffect,
	kActionAddNewLayer,
	kActionAddNewStep,
	kActionChangeProgram,
	kActionRemoveLayer,
	kActionRemoveStep
};
//
enum
{
	kVolume,
	kPan,
	kMIDICHInput,
	kMode,
	kUnison,
	kUnisonVolumeSpread,
	kUnisonPanSpread,
	kQuality,
	kPitchBend,
	kOctave,
	kTune,
	kFine,
		kADSR_Attack,
		kADSR_Decay,
		kADSR_Sustain,
		kADSR_Release,
		kADSR_Velocity,
		kADSR_Clip,
		kADSR_Curve,
		kADSR_Mode,
		//
		kFilter_ADSR_Attack,
		kFilter_ADSR_Decay,
		kFilter_ADSR_Sustain,
		kFilter_ADSR_Release,
		kFilter_ADSR_Velocity,
		kFilter_ADSR_Clip,
		kFilter_ADSR_Curve,
		kFilter_ADSR_Mode,
		//
		kPitch_ADSR_Attack,
		kPitch_ADSR_Decay,
		kPitch_ADSR_Sustain,
		kPitch_ADSR_Release,
		kPitch_ADSR_Velocity,
		kPitch_ADSR_Clip,
		kPitch_ADSR_Curve,
		kPitch_ADSR_Mode,
	//
	kFilter_Mode,
	kFilter_Frequency,
	kFilter_Rezonance,
	kFilter_Smooth, 
	kFilter_Envelope,
	kFilter_Limit,
	//
	kMasterLimiter,
	kGlide,
	kGlideAuto,
	//
	MAX_PARAMETERS
};
//
#define WCREATE_WAVE_BANKS 0
#define WIS_DEBUG 0
//
#define MAX_WAVES 48
#define MAX_WAVE_BANKS 4
#define WMAX_LAYERS 16
#define MAX_ZONES 4
#define MAX_SEQUENCE_STEPS 128
#define MAX_PROGRAM_VALUES 256
#define MAX_PROGRAMS 128
#define MAX_EFFECTS 12
#define MAX_EFFECT_PARAMETERS 22
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WusikZrAudioProcessor;
class WusikZRParameter;
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WZRThread : public Thread
{
public:
	WZRThread(WusikZrAudioProcessor* _owner) : Thread("Wusik ZR Processing"), owner(_owner) 
	{ 
		actionValue[0] = actionValue[1] = 0;
	};
	void run();
	//
	WusikZrAudioProcessor* owner;
	Atomic<int> action;
	int actionValue[2];
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WFXViewPort : public Viewport
{
public:
	void mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel)
	{
		auto pos = getViewPosition();
		if (wheel.deltaY > 0) if (wheel.isReversed) pos.y += stepSize; else pos.y -= stepSize;
		else if (wheel.isReversed) pos.y -= stepSize; else pos.y += stepSize;
		setViewPosition(pos);
	}
	//
	int stepSize;
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WMidiLearned
{
public:
	WMidiLearned(int _midiRemote, int _parameterIndex, int _zone) : midiRemote(_midiRemote), parameterIndex(_parameterIndex) { };
	//
	int midiRemote, parameterIndex, zone;
	//
	enum
	{
		kMIDIRemoteNone = -1,
		kMIDIRemoteATouch = 200,
		kMIDIRemotePBend = 400
	};
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WLayerComponent : public Component
{
public:
	WLayerComponent(int _type, Image& _background, int _layer, void* _owner, int _zone, Rectangle<int> _buttonPos = Rectangle<int>(0,0,0,0), int _step = -1) :
		layer(_layer), background(_background), buttonPos(_buttonPos), owner(_owner), type(_type), step(_step), zone(_zone)
	{ 
		setSize(background.getWidth(), background.getHeight());
	};
	//
	~WLayerComponent()
	{
		deleteAllChildren();
	};
	//
	void mouseExit(const MouseEvent& e) { repaint(); };
	void mouseMove(const MouseEvent& e) { repaint(); };
	void mouseEnter(const MouseEvent& e) { repaint(); };
	void mouseDown(const MouseEvent& e);
	void paint(Graphics& g);
	//
	void* owner;
	Image& background;
	int layer, type, step, zone;
	Rectangle<int> buttonPos;
	Rectangle<int> buttonPos2;
	String pluginName;
	//
	enum
	{
		kStaticLayer,
		kSequencedLayer,
		kSequencedStep,
		kEffectLayer
	};
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WImageButton : public ImageButton
{
public:
	WImageButton(int _type, void* _owner, int _zone) : zone(_zone), owner(_owner), type(_type), ImageButton() { };
	void mouseUp(const MouseEvent& e);
	int type;
	void* owner;
	int zone;
	//
	enum
	{
		kNoMouse,
		kNewStaticLayer,
		kNewSequencedLayer,
		kNewEffect,
		kShowHideStaticLayers,
		kShowHideSequencedLayers,
		kShowHideEffects
	};
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WValueLabel : public Component
{
public:
	void paint(Graphics& g)
	{
		g.fillAll(background);
		//
		g.setColour(rectangleColour);
		g.drawRect(0, 0, getWidth(), getHeight(), rectangleBorder);
		//
		g.setFont(textFont);
		g.setColour(textColour);
		g.drawText(text, Rectangle<int>(0, 0, getWidth(), getHeight()), Justification::centred);
	};
	//
	void setText(String _text) 
	{ 
		text = _text;
		repaint(); 
	};
	//
	String text;
	Colour background;
	Colour rectangleColour;
	Colour textColour;
	Font textFont;
	int rectangleBorder;
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WAboutBox : public Component
{
public:
	WAboutBox(Image& _background, Rectangle<int> _versionPosition) : background(_background), versionPosition(_versionPosition)
	{
		setSize(background.getWidth(), background.getHeight());
	};
	//
	void paint(Graphics& g)
	{
		g.drawImageAt(background, 0, 0);
		//
		g.setFont(textFont);
		g.setColour(textColour);
		g.drawText(version, versionPosition, Justification::centred);
	};
	//
	void mouseUp(const MouseEvent& e)
	{
		setVisible(false);
	};
	//
	void setVersion(String _version)
	{
		version = _version;
		repaint();
	};
	//
	String version;
	Colour textColour;
	Font textFont;
	Image background;
	Rectangle<int> versionPosition;
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WusikKnob : public Component, public SettableTooltipClient, public KeyListener
{
public:
	WusikKnob(WusikZrAudioProcessor* _processor, Image *_theImage, int _zone, int _parameterIndex, int _X, int _Y, int _Width = 0, int _frameHeight = 0, int _numberOfFrames = 1, String extraParSkinName = String(), bool _isWaveform = false, int _layer = 0, int _step = -1);
	~WusikKnob() 
	{ 
		removeKeyListener(this); 
	};
	//
	class WusikKnobWaveformWindow : public DocumentWindow
	{
	public:
		WusikKnobWaveformWindow(WusikZrAudioProcessor* _processor, WusikKnob* _wusikKnob, int _layer, String extra) :
			DocumentWindow("Waveform Selector - Layer: " + String(_layer + 1) + extra,
			Colours::black, DocumentWindow::closeButton, true), processor(_processor), wusikKnob(_wusikKnob), autoClose(false)
		{
			init();
		};
		//
		~WusikKnobWaveformWindow();
		//
		void closeButtonPressed() override;
		void init();
		void paint(Graphics& g);
		void mouseUp(const MouseEvent& e);
		void mouseDoubleClick(const MouseEvent& e);
		//
		WusikZrAudioProcessor* processor;
		Image* theImageBackground;
		WusikKnob* wusikKnob;
		Point<int> waveformStart, waveformSize, waveformTotalSize;
		Rectangle<int> wavePositions[48];
		bool autoClose;
	};
	//
	void paint(Graphics& g);
	void mouseExit(const MouseEvent& e);
	void mouseDown(const MouseEvent& e);
	void mouseEnter(const MouseEvent& e);
	void mouseDrag(const MouseEvent& e);
	void mouseUp(const MouseEvent& e);
	void mouseDoubleClick(const MouseEvent& e);
	bool keyPressed(const KeyPress &key, Component* component);
	//
	WusikZrAudioProcessor* processor;
	Image *theImage;
	WusikZRParameter* theParameter;
	float numberOfFrames;
	int frameHeight, parameterIndex;
	float initialValue;
	Point<int> initialPosition;
	bool isVertical;
	bool isPopupMenu;
	Font theFont;
	Colour fontColour;
	String parSkinName;
	bool hasMoved;
	bool isWaveform;
	int zone, layer, step;
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WEffectsDetails
{
public:
	WEffectsDetails(String _description, String _name, String _type) : name(_name), description(_description), type(_type),
		isActive(1.0f), currentProgram(0), hasChanges(false), wetDry(0.5f) { };
	~WEffectsDetails() { releaseEffect(); };
	//
	virtual int getCurrentProgram() { return 0; };
	virtual void setCurrentProgram(int index) { };
	virtual String getProgramName(int index) { return String(); };
	virtual int getNumParameters() { return 0; };
	virtual int getNumPrograms() { return 0; };
	virtual float getParameter(int index) { return 0.0f; };
	virtual void setParameter(int index, float value) { };
	virtual String getParameterName(int index) { return String(); };
	virtual String getParameterLabel(int index) { return String(); };
	virtual String getParameterText(int index) { return String(); };
	virtual void processBlock(AudioSampleBuffer& buffer) { };
	virtual void setStateInformation(const void* data, int sizeInBytes) { };
	virtual void getStateInformation(MemoryBlock& data) { };
	virtual void releaseEffect() { };
	virtual bool hasEffect() { return false; };
	virtual void loadEffect(int sampleRate, AudioPlayHead* playHead = nullptr) { };
	//
	MidiBuffer emptyMIDI;
	String description, name;
	MemoryBlock stateInformation;
	float isActive, wetDry;
	int currentProgram;
	bool hasChanges;
	String type;
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WEffectVST : public WEffectsDetails
{
public:
	WEffectVST(String _description, String _name, WusikDataFiles* _DataFiles, AudioPluginFormatManager* _formatManager) : WEffectsDetails(_description, _name, "PLUGIN"), DataFiles(_DataFiles), formatManager(_formatManager) { };
	~WEffectVST() { releaseEffect(); };
	//
	int getCurrentProgram() override { if (plugin != nullptr) return plugin->getCurrentProgram(); return 0; };
	void setCurrentProgram(int index) override { if (plugin != nullptr) plugin->setCurrentProgram(index); };
	String getProgramName(int index) override { if (plugin != nullptr) return plugin->getProgramName(index); return String(); };
	int getNumParameters() override { if (plugin != nullptr) return plugin->getNumParameters(); return 0; };
	int getNumPrograms() override { if (plugin != nullptr) return plugin->getNumPrograms(); return 0; };
	float getParameter(int index) override { if (plugin != nullptr) return plugin->getParameters()[index]->getValue(); return 0.0f; };
	void setParameter(int index, float value) override { if (plugin != nullptr) plugin->getParameters()[index]->setValue(value); };
	String getParameterName(int index) override { if (plugin != nullptr) return plugin->getParameters()[index]->getName(999); return String(); };
	String getParameterLabel(int index) override { if (plugin != nullptr) return plugin->getParameters()[index]->getLabel(); return String(); };
	String getParameterText(int index) override { if (plugin != nullptr) return plugin->getParameters()[index]->getText(plugin->getParameters()[index]->getValue(), 999); return String(); };
	void processBlock(AudioSampleBuffer& buffer) override
	{
		if (isActive == 1.0f && plugin != nullptr)
		{
			originalBuffer.setSize(2, buffer.getNumSamples(), false, false, true);
			//
			originalBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
			originalBuffer.copyFrom(1, 0, buffer, 1, 0, buffer.getNumSamples());
			//
			plugin->processBlock(buffer, emptyMIDI);
			//
			buffer.applyGain(1.0f - wetDry);
			buffer.addFrom(0, 0, originalBuffer, 0, 0, originalBuffer.getNumSamples(), wetDry);
			buffer.addFrom(1, 0, originalBuffer, 1, 0, originalBuffer.getNumSamples(), wetDry);

		}
	};
	void setStateInformation(const void* data, int sizeInBytes) override { if (plugin != nullptr) plugin->setStateInformation(data, sizeInBytes); };
	void getStateInformation(MemoryBlock& data) override { if (plugin != nullptr) plugin->getStateInformation(data); };
	void releaseEffect() override { plugin = nullptr; };
	bool hasEffect() override { return plugin != nullptr; };
	void loadEffect(int sampleRate, AudioPlayHead* playHead = nullptr) override
	{ 
		if (plugin != nullptr) return;
		//
		String pluginName = description.fromFirstOccurrenceOf("-", false, true).upToFirstOccurrenceOf("-", false, true);
		int thePluginIndex = -1;
		//
		ARRAY_Iterator(DataFiles->plugsList->plugins)
		{
			if (DataFiles->plugsList->plugins[index]->matchesIdentifierString(description))
			{
				thePluginIndex = index;
				break;
			}
		}
		if (thePluginIndex == -1)
		{
			ARRAY_Iterator(DataFiles->plugsList->plugins)
			{
				if (DataFiles->plugsList->plugins[index]->name.compareIgnoreCase(pluginName) == 0)
				{
					thePluginIndex = index;
					break;
				}
			}
			//
			if (thePluginIndex == -1)
			{
				MessageBox("Error! Could Not Find Plugin", description);
				return;
			}
		}
		//
		PluginDescription *d = (PluginDescription *)DataFiles->plugsList->plugins[thePluginIndex];
		String errorMessage;
		plugin = formatManager->createPluginInstance(*d, sampleRate, 4, errorMessage);
		if (plugin == nullptr || plugin->getTotalNumOutputChannels() < 1)
		{
			AlertWindow::showMessageBox(AlertWindow::NoIcon, "Error Loading Plugin", errorMessage + "\n" + d->fileOrIdentifier);
			return;
		}
		//
		if (playHead != nullptr) plugin->setPlayHead(playHead);
		plugin->prepareToPlay(sampleRate, 4);
	};
	//
private:
	MidiBuffer emptyMIDI;
	ScopedPointer<AudioPluginInstance> plugin;
	WusikDataFiles* DataFiles;
	AudioPluginFormatManager* formatManager;
	AudioSampleBuffer originalBuffer;
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WEffect_Parameter
{
public:
	WEffect_Parameter(float _value, String _name) : value(_value), name(_name) { };
	String name;
	float value;
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WEffectVSTdual : public WEffectsDetails
{
public:
	WEffectVSTdual(String _description, String _name, WusikDataFiles* _DataFiles, AudioPluginFormatManager* _formatManager) :
		WEffectsDetails(_description, _name, "INTERNAL"), DataFiles(_DataFiles), formatManager(_formatManager) 
	{ 
		if (name.compareIgnoreCase("Reverb DX Stereo") == 0)
		{
			description = "Reverb DX Light";
			parameters.add(new WEffect_Parameter(0.18f, "PreDelay"));
			parameters.add(new WEffect_Parameter(0.4f, "Decay"));
			parameters.add(new WEffect_Parameter(0.84f, "FCut"));
			parameters.add(new WEffect_Parameter(0.1f, "Deviate"));
		}
	};
	//
	~WEffectVSTdual() { releaseEffect(); };
	//
	int getNumParameters() override { return parameters.size(); };
	float getParameter(int index) override 
	{ 
		return parameters[index]->value; 
	};
	void setParameter(int index, float value) override 
	{ 
		parameters[index]->value = value;
	};
	String getParameterName(int index) override { return parameters[index]->name; };
	String getParameterLabel(int index) override { return String(); };
	String getParameterText(int index) override { return String(getParameter(index), 6); };
	void processBlock(AudioSampleBuffer& buffer) override
	{
		if (isActive == 1.0f && plugin[0] != nullptr)
		{
			for (int xx = 0; xx < 3; xx++)
			{
				if (xx < 2)
				{
					plugin[xx]->setParameter(1, jlimit(0.0f, 1.0f, parameters[0]->value + (parameters[3]->value * 0.5f * float(xx)))); // PreDelay
					plugin[xx]->setParameter(2, jlimit(0.0f, 1.0f, parameters[1]->value + (parameters[3]->value * 0.5f * float(xx)))); // Decay
					plugin[xx]->setParameter(3, jlimit(0.0f, 1.0f, parameters[2]->value + (parameters[3]->value * 0.5f * float(xx)))); // FCut
				}
				//
				originalBuffer[xx].setSize(2, buffer.getNumSamples(), false, false, true);
				//
				originalBuffer[xx].copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
				originalBuffer[xx].copyFrom(1, 0, buffer, 1, 0, buffer.getNumSamples());
			}
			//
			//
			plugin[0]->processBlock(originalBuffer[0], emptyMIDI);
			plugin[1]->processBlock(originalBuffer[1], emptyMIDI);
			//
			buffer.clear();
			//
			buffer.copyFrom(0, 0, originalBuffer[0], 0, 0, buffer.getNumSamples());
			buffer.copyFrom(1, 0, originalBuffer[1], 1, 0, buffer.getNumSamples());
			//
			buffer.applyGain(1.0f - wetDry);
			buffer.addFrom(0, 0, originalBuffer[2], 0, 0, originalBuffer[2].getNumSamples(), wetDry);
			buffer.addFrom(1, 0, originalBuffer[2], 1, 0, originalBuffer[2].getNumSamples(), wetDry);
		}
	};
	void setStateInformation(const void* data, int sizeInBytes) override 
	{ 
		if (plugin[0] != nullptr && plugin[1] != nullptr)
		{
			MemoryInputStream stream(data, sizeInBytes, false);
			int xSize = stream.readInt();
			//
			for (int xp = 0; xp < xSize; xp++)
			{
				parameters[xp]->value = stream.readFloat();
			}
		}
	};
	void getStateInformation(MemoryBlock& data) override 
	{ 
		if (plugin[0] != nullptr && plugin[1] != nullptr)
		{
			MemoryOutputStream stream(data, false);
			stream.writeInt(parameters.size());
			//
			ARRAY_Iterator(parameters)
			{
				stream.writeFloat(parameters[index]->value);
			}
		}
	};
	void releaseEffect() override { plugin[0] = nullptr; plugin[1] = nullptr; };
	bool hasEffect() override { return (plugin[0] != nullptr && plugin[1] != nullptr); };
	void loadEffect(int sampleRate, AudioPlayHead* playHead = nullptr) override
	{ 
		if (plugin[0] != nullptr && plugin[1] != nullptr) return;
		//
		int thePluginIndex = -1;
		ARRAY_Iterator(DataFiles->plugsList->plugins)
		{
			if (DataFiles->plugsList->plugins[index]->name.compareIgnoreCase(description) == 0)
			{
				thePluginIndex = index;
				break;
			}
		}
		//
		if (thePluginIndex == -1)
		{
			MessageBox("Error! Could Not Find Plugin", description);
			return;
		}
		//
		PluginDescription *d = (PluginDescription *)DataFiles->plugsList->plugins[thePluginIndex];
		for (int xx = 0; xx < 2; xx++)
		{
			String errorMessage;
			plugin[xx] = formatManager->createPluginInstance(*d, sampleRate, 4, errorMessage);
			if (plugin[xx] == nullptr || plugin[xx]->getTotalNumOutputChannels() < 1)
			{
				AlertWindow::showMessageBox(AlertWindow::NoIcon, "Error Loading Plugin", errorMessage + "\n" + d->fileOrIdentifier);
				return;
			}
			//
			if (playHead != nullptr) plugin[xx]->setPlayHead(playHead);
			plugin[xx]->prepareToPlay(sampleRate, 4);
			plugin[xx]->setParameter(0, 1.0f); // Mix
			plugin[xx]->setParameter(4, 1.0f); // Volume
		}
	};
	//
private:
	MidiBuffer emptyMIDI;
	ScopedPointer<AudioPluginInstance> plugin[2];
	WusikDataFiles* DataFiles;
	AudioPluginFormatManager* formatManager;
	AudioSampleBuffer originalBuffer[3];
	OwnedArray<WEffect_Parameter> parameters;
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define WAddParameter(par1, par2, par3, par4, par5, par6)  addParameter(new WusikZRParameter(zones, &programs.getFirst()->values[zones][addingParametersPosition], par1, par2, par3, par4, par5, par6)); addingParametersPosition++
#define WAddParameterDb(par1, par2, par3)  addParameter(new WusikZRParameter(zones, &programs.getFirst()->values[zones][addingParametersPosition], par1, par2, par3, 0.0f, 1.0f, 0.00001f, 2, false, true, true)); addingParametersPosition++
#define WAddParameterPan(par1, par2, par3)  addParameter(new WusikZRParameter(zones, &programs.getFirst()->values[zones][addingParametersPosition], par1, par2, par3, -1.0f, 1.0f, 0.00001f, 2, false, true, false, true)); addingParametersPosition++
#define WAddParameterM(par1, par2, par3, par4, par5);  addParameter(new WusikZRParameter(zones, &programs.getFirst()->values[zones][addingParametersPosition], par1, par2, par3, 0.0f, par4, 1.0f)); ((WusikZRParameter*)getParameters().getLast())->menuOptions = StringArray::fromLines(par5); addingParametersPosition++
#define WPAR(parameter, value) ((WusikZRParameter*)getParameters().getLast())->parameter = value
#define WPARTOOLTIP(value) ((WusikZRParameter*)getParameters().getLast())->setTooltip(value)
#define WPARSkin(skinname) ((WusikZRParameter*)getParameters().getLast())->skinName = skinname
#define WPAR_GLOBAL WPAR(isGlobal, true)
//
#define WAddParameter2(par1, par2, par3, par4, par5, par6)  addParameter(new WusikZRParameter(zones, &dummyValue, par1, par2, par3, par4, par5, par6))
#define WAddParameterDb2(par1, par2, par3)  addParameter(new WusikZRParameter(zones, &dummyValue, par1, par2, par3, 0.0f, 1.0f, 0.00001f, 2, false, true, true))
#define WAddParameterPan2(par1, par2, par3)  addParameter(new WusikZRParameter(zones, &dummyValue, par1, par2, par3, -1.0f, 1.0f, 0.00001f, 2, false, true, false, true))
#define WAddParameterM2(par1, par2, par3, par4, par5);  addParameter(new WusikZRParameter(zones, &dummyValue, par1, par2, par3, 0.0f, par4, 1.0f)); ((WusikZRParameter*)getParameters().getLast())->menuOptions = StringArray::fromLines(par5)
//
#define WPARVALSEQ(source) ((WusikZRParameter*)getParameters()[parCounter])->theValue = &xSeq->source; parCounter++
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WusikZRParameter : public AudioProcessorParameter, public SettableTooltipClient
{
public:
	WusikZRParameter(int _zone, float* theVariable, String _name, String _label, float _value, float _min, float _max, float _rate = 0.00001f, int _decimals = 2, bool _onAndOff = false, bool _automatable = true, bool _isDecibels = false, bool _isPan = false, String _minText = String(), String _maxText = String()) :
		name(_name), theValue(theVariable), min(_min), max(_max), rate(_rate), label(_label), onAndOff(_onAndOff), automatable(_automatable), decimals(_decimals), isDecibels(_isDecibels), isPan(_isPan), minText(_minText), maxText(_maxText), 
		isEnvelope(false), sampleRate(44100.0f), isGlobal(false), knob(nullptr), belowThisIsOFF(_min), isOctave(false), time1(nullptr), time2(nullptr), isTimeBPM(nullptr), midiRemote(-1), zone(_zone), effectPlugin(nullptr), pluginParameterIndex(0)
	{
		defaultValue = 0.0f;
		*theValue = _value;
		if (isPan) *theValue = (_value + 1.0f) / 2.0f; else *theValue = jmap(_value, min, max, 0.0f, 1.0f);
		skinName = _name;
		if (min == 0.0f && max == 1.0f && rate == 1.0f) onAndOff = true;
	};
	//
	float getValue() const override 
	{ 
		if (effectPlugin != nullptr) return effectPlugin->getParameter(pluginParameterIndex);
		return *theValue; 
	};
	void setValue(float newValue) override 
	{ 
		if (effectPlugin != nullptr)
		{
			effectPlugin->setParameter(pluginParameterIndex, newValue); 
			effectPlugin->hasChanges = true;
		}
		else *theValue = newValue;
		//
		if (knob != nullptr) knob->repaint();
	};
	float getDefaultValue() const override { return defaultValue; };
	String getName(int maxLen) const override 
	{ 
		if (effectPlugin != nullptr) return effectPlugin->getParameterName(pluginParameterIndex);
		return name; 
	};
	String getLabel() const override 
	{ 
		if (effectPlugin != nullptr) return effectPlugin->getParameterLabel(pluginParameterIndex);
		return label; 
	};
	int getNumSteps() const override 
	{ 
		if (isBoolean()) return 2;
		return AudioProcessor::getDefaultNumParameterSteps();
	};
	bool isDiscrete() const override { return getNumSteps() != AudioProcessor::getDefaultNumParameterSteps(); };
	bool isBoolean() const override { if (menuOptions.size() == 2) return true; return onAndOff; };
	bool isAutomatable() const override { return automatable; };
	//
	String getText(float value, int maximumLength) const override
	{
		if (effectPlugin != nullptr) return effectPlugin->getParameterText(pluginParameterIndex);
		//
		if (time1 != nullptr && time2 != nullptr && isTimeBPM != nullptr)
		{
			if (*isTimeBPM == 1.0f)
			{
				return String(int(*time1 * 127.0f) + 1) + "/" + String(int(*time2 * 127.0f) + 1) + " BPM";
			}
			else
			{
				return String(*time1 * 1000.0f * *time2, 4) + " HZ";
			}
		}
		else if (isEnvelope)
		{
			return String((sampleRate * 12.0f * ((value) * (value))) / sampleRate, 2) + "s";
		}
		else if (minText.isNotEmpty() && value <= belowThisIsOFF) return minText;
		else if (maxText.isNotEmpty() && value == max) return maxText;
		else if (menuOptions.size() > 0) return menuOptions[(int)(value * (max - 1.0f))];
		else if (isPan)
		{
			if (value <= 0.51f && value >= 0.49f) return "Center";
			else if (value < 0.5f) return "L" + String(int(((0.5f - value) * 2.0f) * 9.0f));
			else return "R" + String(int(((value - 0.5f) * 2.0f) * 9.0f));
		}
		else if (isOctave)
		{
			int iValue = int(value * 12.0f) - 6;
			if (iValue == 0) return "Center";
			else return String(iValue);
		}
		else if (isDecibels)
		{
			return Decibels::toString(Decibels::gainToDecibels(value));
		}
		else if (rate == 1.0f)
		{
			return String(int(jmap(value, min, max)));
		}
		else if (rate >= 1.0f)
		{
			return "";
		}
		else
		{
			return String(jmap(value, min, max), decimals);
		}
		//
		return "Empty";
	};
	//
	float getValueForText(const String& text) const override
	{
		return 1.0f;
	};
	//
	String name, label, minText, maxText, skinName;
	float *theValue, sampleRate, *time1, *time2, *isTimeBPM;
	float max, min, rate, defaultValue, belowThisIsOFF;
	bool onAndOff, automatable, isDecibels, isPan, isEnvelope, isGlobal, isOctave;
	int decimals, midiRemote, zone;
	StringArray menuOptions;
	WusikKnob* knob;
	WEffectsDetails* effectPlugin;
	int pluginParameterIndex;
	//
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WusikZRParameter)
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WusikZRProgram
{
public:
	WusikZRProgram(String _name) : name(_name) 
	{ 
		for (int x = 0; x < MAX_ZONES; x++)
		{
			showLayers[x][0] = showLayers[x][1] = showLayers[x][2] = true;
			//
			waveBankName[x][0] = "Basic";
			waveBankName[x][1] = "Saws 1";
			waveBankName[x][2] = "Game 1";
			waveBankName[x][3] = "Vox 1";
			//
			for (int xp = 0; xp < MAX_PROGRAM_VALUES; xp++)
			{
				values[x][xp] = 0.0f;
			}
		}
	};
	~WusikZRProgram() 
	{ 
		sequencerLayers[0].clear();
		sequencerLayers[1].clear();
		sequencerLayers[2].clear();
		sequencerLayers[3].clear();
		//
		effects[0].clear();
		effects[1].clear();
		effects[2].clear();
		effects[3].clear();
	};
	//
	OwnedArray<WEffectsDetails> effects[MAX_ZONES];
	String waveBankName[MAX_ZONES][4];
	String name;
	float values[MAX_ZONES][MAX_PROGRAM_VALUES];
	bool showLayers[MAX_ZONES][3];
	//
	OwnedArray<WusikSequencer> sequencerLayers[MAX_ZONES];
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WusikZRProgram)
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WusikZrAudioProcessor  : public AudioProcessor
{
public:
	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	DECLARE_ALIGNED(32);
	//
    WusikZrAudioProcessor();
    ~WusikZrAudioProcessor();

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
	void releaseResources() override {};
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override { return true; };
	const String getName() const override { return "Wusik ZR"; };
	bool acceptsMidi() const override { return true; }
	bool producesMidi() const override { return false; }
	bool isMidiEffect() const override { return false; }
	double getTailLengthSeconds() const override { return 0.0; }
	int getNumPrograms() override { return MAX_PROGRAMS; }
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    void getStateInformation (MemoryBlock& destData) override;
	void getCurrentProgramStateInformation(MemoryBlock& destData) override;
	void getStateInformationInternal(MemoryBlock& destData, bool wholeBank);
    void setStateInformation (const void* data, int sizeInBytes) override;
	void setCurrentProgramStateInformation(const void* data, int sizeInBytes) override;
	void setStateInformationFromBuffer();
	void updateLayers(); // will fetch data from the sequencers and create the required layers //
	void loadWaveBanks(char bankSlot = 99, String name = String(), int Zone = -1);
	void redoMidiLearned();
	void loadAllEffects();
	void loadEffect(String desc, String name, String type, int program, int zone);
	void checkAndSaveEffectData(bool removeEffects);
	void newAction(int action, int actionValue1 = 0, int actionValue2 = 0);
	//
	#if WCREATE_WAVE_BANKS
		void loadWave(String filename, int target, int offset = 0, bool fade = false, bool applyLimiter = false);
		void createWaveBanks();
		void saveBinary(String filename, String name);
		StringArray waveformNames;
	#endif
	//
	#if WIS_DEBUG
		WCREATE_DEBUG_WINDOW_VARIABLE;
		String extraDebug;
	#endif
	WusikFilter theFilter[MAX_ZONES][2];
	JUCE_ALIGN(32) WusikWaveform wavetables[MAX_ZONES][MAX_WAVES];
	OwnedArray<WusikZRProgram> programs;
	int currentProgram;
	float* valuesList;
	WusikDataFiles* dataFiles;
	WusikADSREnvelope ADSREnvelope_Amplitude[MAX_ZONES], ADSREnvelope_Filter[MAX_ZONES], ADSREnvelope_Pitch[MAX_ZONES];
	ScopedPointer<WusikWavetable> waveTable[MAX_ZONES];
	MidiMessage midiMessage;
	int sampPos;
	float pitchBendValue, pitchBendTarget, pitchBendRate;
	double globalBPM;
	int addingParametersPosition;
	bool editingEnabled, editingEnabledDrawRectangles;
	String presetBankName;
	int parametersLayerStart[MAX_ZONES][WMAX_LAYERS], parametersMainStart[MAX_ZONES], parametersEffectsStart[MAX_ZONES][MAX_EFFECTS];
	float dummyValue;
	int lastYPositionEditing, lastXPositionEditing;
	StringArray waveBanksArray;
	int midiLearn;
	OwnedArray<WMidiLearned> midiLearned;
	void* ownerStandalone;
	String curWaveBankName[MAX_ZONES][4];
	MemoryBlock stateBuffer;
	int editingEnvelope;
	double lastSampleRate;
	Atomic<int> isProcessingSomething;
	Atomic<int> hasPrepareToPlay;
	Atomic<int> hasStartedAnyProcessing;
	String loadNewEffectDescription, loadNewEffectName, loadNewEffectType;
	ScopedPointer<WusikKnob::WusikKnobWaveformWindow> waveformWindow;
	JUCE_ALIGN(32) float allVoicesOutput[2][MAX_OVERSAMPLE][MAX_WAVETABLE_VOICES]; // STEREO //
	JUCE_ALIGN(32) float filterProcessSignal[2][MAX_WAVETABLE_VOICES]; // STEREO //
	JUCE_ALIGN(32) float sequencedFilter[MAX_WAVETABLE_VOICES];
	int currentZone;
	ScopedPointer<AudioPluginInstance> VSTLimiter[MAX_ZONES];
	PluginDescription VSTLimiterDescription;
	AudioSampleBuffer limiterBuffer[MAX_ZONES];
	MidiBuffer emptyMIDIMessageBuffer;
	float unisonVolume, unisonPan, unisonPosition, unisonFine, unisonVolumeRate, unisonPanRate, unisonPositionRate, unisonFineRate;
	WZRThread processingThread;
	float fadeOutRate, fadeOutCounter;
	float prevOctave[MAX_ZONES];
	float prevTune[MAX_ZONES];
	bool retriggerMonoNote[MAX_ZONES];
	MidiMessage retriggerMonoNoteMessage[MAX_ZONES];
	int extraIdleCounter[MAX_ZONES];
	bool isFirstNote;
	//
	enum
	{
		kLeft = 0, kRight,
		kAddStaticLayer, kAddSequencedLayer,
		kEditEnvelope_Amplitude = 0,
		kEditEnvelope_Filter,
		kEditEnvelope_Pitch
	};
	//
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WusikZrAudioProcessor)
};