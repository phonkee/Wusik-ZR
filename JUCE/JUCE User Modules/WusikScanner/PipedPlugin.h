/*

	Wusik SQ200 (c) Wusik Dot Com 2019
	William Kalfelz
	www.Wusik.com
	www.Kalfelz.com

*/

#ifndef WUSIKPIPELINE_H_INCLUDED
#define WUSIKPIPELINE_H_INCLUDED
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
#if WUSIK_X42
	static bool isWusikX42 = true;
#else
	static bool isWusikX42 = false;
#endif
//
// -------------------------------------------------------------------------------------------------------------------------------
enum
{
	kProcess = 77,
	kPrepareToPlay,
	//
	kEvent_OpenEditor = 52,
	kEvent_Reset,
	kEvent_ReleaseResources,
	kEvent_GetParameterValue,
	kEvent_GetProgram,
	kEvent_SetProgram,
	kEvent_GetState,
	kEvent_SetState,
	kEvent_GetParameterText,
	kEvent_SetEditorAlwaysOnTop,
	kEvent_CloseEditor,
	kEvent_IsEditorOpen,
	kEvent_SetWindowPosX,
	kEvent_SetWindowPosY,
	kEvent_GetWindowPosX,
	kEvent_GetWindowPosY,
	kEvent_GetEditorWidth,
	kEvent_GetEditorHeigth,
	kEvent_SaveFXP,
	kEvent_SaveFXB,
	kEvent_LoadFXP,
	kEvent_LoadFXB,
	kEvent_SetOversamplingTimes,
	kEvent_GetEditorHandle
};
//
// -------------------------------------------------------------------------------------------------------------------------------
class AutoMations
{
public:
	AutoMations(float _value, int _parameter) : value(_value), parameter(_parameter) { };
	float value;
	int parameter;
};
//
// -------------------------------------------------------------------------------------------------------------------------------
class PingThread : public Thread
{
public:
	PingThread(void* _owner) : Thread("WusikPipe"), owner(_owner) { };
	void run();
	//
	void* owner;
};
//
// -------------------------------------------------------------------------------------------------------------------------------
class JUCE_API  PipedPluginFormat   : public AudioPluginFormat
{
public:
	// -------------------------------------------------------------------------------------------------------------------------------
	enum
	{
		kCloseEditor = -99,
		kGetWindowsX,
		kGetWindowsY,
		kHasStarted,
		kHasReStarted,
		kSetWindowsX,
		kSetWindowsY,
		kOpenEditor,
		kGetEditorWidth,
		kGetEditorHeigth,
		kSaveFXP,
		kSaveFXB,
		kLoadFXP,
		kLoadFXB,
		kSetOverSamplingTimes,
		kHasCrashed,
		kReloadPlugin,
		kGetEditorHandle,
		kHasChangedPresetsList
	};
	// -------------------------------------------------------------------------------------------------------------------------------
	PipedPluginFormat(String _theFormat, String _fontDetails, String _dataFolderLocale, bool _editorsOnTop, bool _autoReloadOnCrash = true, AudioProcessor* _audioOwner = nullptr);
	~PipedPluginFormat();

	// -------------------------------------------------------------------------------------------------------------------------------
    static bool getChunkData (AudioPluginInstance* plugin, MemoryBlock& result, bool isPreset);
    static bool setChunkData (AudioPluginInstance* plugin, const void* data, int size, bool isPreset);
	//
    String getName() const override                { return theFormat; }
    void findAllTypesForFile (OwnedArray<PluginDescription>&, const String& fileOrIdentifier) override;
    bool fileMightContainThisPluginType (const String& fileOrIdentifier) override;
    bool doesPluginStillExist (const PluginDescription&) override;
    bool canScanForPlugins() const override        { return false; }
	//
    void createPluginInstance (const PluginDescription&, double initialSampleRate,
                               int initialBufferSize, void* userData,
                               void (*callback) (void*, AudioPluginInstance*, const String&)) override;
	//
	bool requiresUnblockedMessageThreadDuringCreation(const PluginDescription&) const noexcept override { return false; };
	//
	String getNameOfPluginFromIdentifier(const String&) override { return String(); };
	bool pluginNeedsRescanning(const PluginDescription&) override;
	StringArray searchPathsForPlugins(const FileSearchPath&, bool, bool) override { return StringArray(); };
	FileSearchPath getDefaultLocationsToSearch() override { return FileSearchPath(); };
	//
	String fontDetails, dataFolderLocale;
	bool editorsOnTop;
	bool autoReloadOnCrash;
	AudioProcessor* audioOwner;
	String theFormat;
	//
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PipedPluginFormat)
};
//
#endif