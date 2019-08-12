// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

	Created by William Kalfelz @ Wusik Dot Com (c) 2019

*/

#pragma once
//
#define loadPng(a) File(dataFolder + "Wusik ZR" + slash + "Skins" + slash + selectedSkin + slash + a + ".png")
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum
{
	processDisabled,
	processSSE2,
	processAVX_FMA3,
	//
	kKnob0_Horizontal_Slider = 0,
	kKnob1_Regular,
	kKnob2_LargeRed,
	kKnob3_Vertical_Slider,
	kKnob4_Small,
	kKnob5_OnOff,
	kKnob6_LED,
	kKnob7_OnOff,
	kKnob8_Tiny,
	kKnob9_AddMultiply,
	kKnob10_TinyDark,
	kKnob11_TinyLED,
	kBackground_EnvelopeSelector,
	kBackground_SequencedLayersBar,
	kBackground_SequencedLayer,
	kBackground_SequencedLayer_Step,
	kBackground_SequencedLayer_Step_Bottom,
	kBackground_StaticLayer,
	kBackground_StaticLayerBar,
	kBackground_Waveform1,
	kBackground_Waveform2,
	kBackground_EffectsBar,
	kBackground_ZoneSelectors,
	kBackground_WaveformWindow,
	kBackground_EffectsLayer,
	kLayersShow,
	kLayersHide,
	kMaxKnobs
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WusikDataFiles :	public DeletedAtShutdown,
						public ChangeListener
{
public:
	WusikDataFiles();
	~WusikDataFiles();
	//
	String readGlobalSettings(String program, String key);
	void deleteGlobalSettings(String program, String key);
	void saveGlobalSettings(String program, String key, String value);
	void getSkinRectangle(String name, Rectangle<int>& rectangle);
	void getFontDetails(String name, Font &font, Colour& colour);
	void listFiles(File folder, String extension, StringArray& sArray);
	void updateListOfPlugins();
	void changeListenerCallback(ChangeBroadcaster* changed);
	//
	PropertiesFile::Options skinSettingsOptions;
	ScopedPointer<PropertiesFile> skinSettings;
	//
	ScopedPointer<PropertySet> defaultSettings;
	PropertiesFile::Options userSettingsOptions;
	ScopedPointer<PropertiesFile> userSettings;
	//
	int processType;
	String dataFolder, dataFilesError, selectedSkin;
	String limiterFile;
	//
	ScopedPointer<AudioPluginFormatManager> formatManager;
	ScopedPointer<PropertiesFile> pluginsList;
	ScopedPointer<WKnownPluginList> knownPluginList;
	WCustomScanner* customScanner;
	ScopedPointer<WKnownPluginList::PluginTree> plugsList;
	Array<int> pluginEffects;
	bool macValidateInstall;
	//
	/*#if JUCE_WINDOWS & JUCE_64BIT
		PipedPluginFormat* pipedPluginFormat;
	#endif*/
	//
	// SKIN FILES //
	Image background[3], aboutBox;
	Image allImages[kMaxKnobs];
	Font presetNameFont, waveBankFont, valueDisplayFont, aboutBoxVersionFont, stepFont, effectLabelFont, effectsNameFont;
	Colour presetNameColour, waveBankColour, valueDisplayTextColour, valueDisplayBackgroundColour, valueDisplayRectangleColour, aboutBoxVersionColour, waveformColour, stepColour, scrollColour, waveformSelectedColour, effectLabelColour, effectsNameColour;
	StringArray presetBankNames, waveBankNames, internalEffects;
	Rectangle<int> prevPreset, nextPreset, presetName, waveBanks[4], productLogo, aboutBoxVersion, stepPosition, envelopeSelector, zoneSelector, waveWindowBig, effectLabelPosition, effectsNamePosition, effectNamePresetPosition;
	Rectangle<int> zoneIndicators[4];
	int valueDisplayRectangleThickness, valueDisplayW, valueDisplayH, effectScrollStep, effectScrollBottomOffset;
	double scrollThickness;
	String pipedPluginFontString;
	//
	bool X664nNifnr937Nd;					// Demonstration Version //
	bool JNeruNennnnnnnnn;
	//
	juce_DeclareSingleton(WusikDataFiles, false)
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WusikDataFiles);
};