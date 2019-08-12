/*
BEGIN_JUCE_MODULE_DECLARATION
    ID:               WusikScanner
    vendor:           Wusik
    version:          1.0.0
    name:             Wusik Scanner
    description:      
    website:          https://www.Wusik.com
    dependencies:     
	license:          Open Source
END_JUCE_MODULE_DECLARATION 
*/

#ifndef MYPLUGINLIST_H_INCLUDED
#define MYPLUGINLIST_H_INCLUDED

// -------------------------------------------------------------------------------------------------------------------------------
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_data_structures/juce_data_structures.h>
#include "PipedPlugin.h"
#include <../JUCE 5 User Modules/WusikUtils/WusikUtils.h>

using namespace juce;

// -------------------------------------------------------------------------------------------------------------------------------
class WKnownPluginList : public ChangeBroadcaster
{
public:
	WKnownPluginList();
	~WKnownPluginList();
	void clear();
	int getNumTypes() const noexcept { return types.size(); }
	PluginDescription* getType(int index) const noexcept { return types[index]; }
	PluginDescription** begin() const noexcept { return types.begin(); }
	PluginDescription** end() const noexcept { return types.end(); }
	PluginDescription* getTypeForFile(const String& fileOrIdentifier) const;
	PluginDescription* getTypeForIdentifierString(const String& identifierString) const;
	bool addType(const PluginDescription& type);
	void removeType(int index);
	bool scanAndAddFile(const String& possiblePluginFileOrIdentifier,
		bool dontRescanIfAlreadyInList,
		OwnedArray <PluginDescription>& typesFound,
		AudioPluginFormat& formatToUse,
		String& pluginBeingScanned, PropertiesFile& propertiesToUse);
	void scanFinished();
	bool isListingUpToDate(const String& possiblePluginFileOrIdentifier,
		AudioPluginFormat& formatToUse) const;
	void scanAndAddDragAndDroppedFiles(AudioPluginFormatManager& formatManager,
		const StringArray& filenames,
		OwnedArray <PluginDescription>& typesFound, PropertiesFile& propertiesToUse);
	const StringArray& getBlacklistedFiles() const;
	void addToBlacklist(const String& pluginID);
	void removeFromBlacklist(const String& pluginID);
	void clearBlacklistedFiles();
	//
	enum SortMethod
	{
		defaultOrder = 0,
		sortAlphabetically,
		sortByCategory,
		sortByManufacturer,
		sortByFormat,
		sortByFileSystemLocation,
		sortByInfoUpdateTime
	};
	void addToMenu(PopupMenu& menu, SortMethod sortMethod, const String& currentlyTickedPluginID = String()) const;
	int getIndexChosenByMenu(int menuResultCode) const;
	void sort(SortMethod method, bool forwards);
	XmlElement* createXml() const;
	void recreateFromXml(const XmlElement& xml);
	struct PluginTree
	{
		String folder; /**< The name of this folder in the tree */
		OwnedArray<PluginTree> subFolders;
		Array<const PluginDescription*> plugins;
	};
	PluginTree* createTree(const SortMethod sortMethod) const;
	//
	class CustomScanner
	{
	public:
		CustomScanner();
		virtual ~CustomScanner();
		virtual bool findPluginTypesFor(AudioPluginFormat& format,
			OwnedArray <PluginDescription>& result,
			const String& fileOrIdentifier,
			String& pluginNameBeenScanned,
			PropertiesFile& propertiesToUse) = 0;
		virtual void scanFinished();
		bool shouldExit() const noexcept;
	};
	void setCustomScanner(CustomScanner*);

private:
	OwnedArray<PluginDescription> types;
	StringArray blacklist;
	ScopedPointer<CustomScanner> scanner;
	CriticalSection scanLock, typesArrayLock;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WKnownPluginList)
};

// -------------------------------------------------------------------------------------------------------------------------------
class WCustomScanner : public WKnownPluginList::CustomScanner
{
public:
	WCustomScanner(String _dataFolderLocale);
	~WCustomScanner() override;
	bool findPluginTypesFor(AudioPluginFormat& format, OwnedArray <PluginDescription>& result, const String& fileOrIdentifier, String& pluginNameBeenScanned, PropertiesFile& propertiesToUse) override;

	void scanFinished() override;
	bool shouldExit();
	//
	bool exitScan;
	String dataFolderLocale;
};

// -------------------------------------------------------------------------------------------------------------------------------
class WPluginDirectoryScanner
{
public:
	WPluginDirectoryScanner(WKnownPluginList& listToAddResultsTo, AudioPluginFormat& formatToLookFor, FileSearchPath directoriesToSearch, bool searchRecursively,
		const File& deadMansPedalFile, bool allowPluginsWhichRequireAsynchronousInstantiation = false);
	~WPluginDirectoryScanner();
	bool scanNextFile(bool dontRescanIfAlreadyInList, String& nameOfPluginBeingScanned, PropertiesFile& propertiesToUse);
	bool skipNextFile();
	String getNextPluginFileThatWillBeScanned() const;
	float getProgress() const { return progress; }
	const StringArray& getFailedFiles() const noexcept { return failedFiles; }
	static void applyBlacklistingsFromDeadMansPedal(WKnownPluginList& listToApplyTo,
		const File& deadMansPedalFile);
private:
	WKnownPluginList& list;
	AudioPluginFormat& format;
	StringArray filesOrIdentifiersToScan;
	File deadMansPedalFile;
	StringArray failedFiles;
	Atomic<int> nextIndex;
	float progress;
	bool allowAsync;
	void updateProgress();
	void setDeadMansPedalFile(const StringArray& newContents);
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WPluginDirectoryScanner)
};

// -------------------------------------------------------------------------------------------------------------------------------
class PluginEditorWindowTable : public DocumentWindow
{
public:
	PluginEditorWindowTable(const String& name, void* _owner) :
		owner(_owner), DocumentWindow(name, Colours::black, DocumentWindow::closeButton, true) {};
	//
	void closeButtonPressed();
	void* owner;
};

// -------------------------------------------------------------------------------------------------------------------------------
class WPluginListComponent : public Component,
	private ChangeListener,
	private Button::Listener
{
public:
	WPluginListComponent(AudioPluginFormatManager& formatManager,
		WKnownPluginList& listToRepresent,
		const File& deadMansPedalFile,
		PropertiesFile* propertiesToUse,
		String _dataFolderLocale,
		bool allowPluginsWhichRequireAsynchronousInstantiation = false,
		void* _effectOwner = nullptr,
		bool _showShortNames = false, 
		bool _addExtraPaths = true,
		bool _showTopOfList = true,
		bool _showBridge = false);
	//
	~WPluginListComponent();
	//
	void setOptionsButtonText(const String& newText);
	void setScanDialogText(const String& textForProgressWindowTitle, const String& textForProgressWindowDescription);
	void setNumberOfThreadsForScanning(int numThreads);
	static FileSearchPath getLastSearchPath(PropertiesFile&, AudioPluginFormat&);
	static void setLastSearchPath(PropertiesFile&, AudioPluginFormat&, const FileSearchPath&);
	void scanFor(AudioPluginFormat&);
	bool isScanning() const noexcept;
	void removeSelectedPlugins();
	void setTableModel(TableListBoxModel* model);
	TableListBox& getTableListBox() noexcept { return table; }
	//
	// -------------------------------------------------------------------------------------------------------------------------------
	AudioPluginFormatManager& formatManager;
	WKnownPluginList& list;
	File deadMansPedalFile;
	TableListBox table;
	TextButton optionsButton;
	PropertiesFile* propertiesToUse;
	String dialogTitle, dialogText;
	bool allowAsync;
	int numThreads;
	class TableModel;
	ScopedPointer<TableListBoxModel> tableModel;
	class Scanner;
	friend class Scanner;
	friend struct ContainerDeletePolicy<Scanner>;
	ScopedPointer<Scanner> currentScanner;
	void scanFinished(const StringArray&);
	static void optionsMenuStaticCallback(int, WPluginListComponent*);
	void optionsMenuCallback(int);
	void updateList();
	void showSelectedFolder();
	bool canShowSelectedFolder() const;
	void removeMissingPlugins();
	void removePluginItem(int index);
	void resized() override;
	void buttonClicked(Button*) override;
	void changeListenerCallback(ChangeBroadcaster*) override;
	String *nameOfPluginBeingScanned;
	bool showShortNames;
	bool showBridge;
	bool showTopOfList;
	bool addExtraPaths;
	//
	ScopedPointer<AudioPluginInstance> pluginLoad;
	AudioProcessorEditor* pluginEditor;
	ScopedPointer<PluginEditorWindowTable> pluginEditorWindow;
	//
	void* effectOwner;
	String dataFolderLocale;
	//
	Colour cNormal, cSelected, cBad, cRowBack, cUnActive, cBackColour;
	bool useBackColour;
	//
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WPluginListComponent)
};

// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------
//
#define DelayedMessageBox(message, seconds) static int32 counter = Time::getApproximateMillisecondCounter(); if (Time::getApproximateMillisecondCounter() - counter > (seconds * 1000)) { MessageBox("!", message); counter = Time::getApproximateMillisecondCounter(); }
//
class WUTILS
{
public:
	static String DisplayNumber1Zero(int number)
	{
		String xZero = "";
		if (number < 10) xZero = "0";
		return xZero + String(number);
	};
	//
	static String DisplayNumber2Zeros(int number)
	{
		if (number < 0)
		{
			number = -1 * number;
			if (number > 99) number = 99;
			String xZero = "";
			if (number < 10) xZero = "0";
			else if (number < 100) xZero = "";
			return "-" + xZero + String(number);
		}
		else
		{
			String xZero = "";
			if (number < 10) xZero = "00";
			else if (number < 100) xZero = "0";
			return xZero + String(number);
		}
	};
	//
	static String DisplayNumber3Zeros(int number)
	{
		String xZero = "";
		if (number < 10) xZero = "000";
		else if (number < 100) xZero = "00";
		else if (number < 1000) xZero = "0";
		return xZero + String(number);
	};
	//
	static String DisplayN2ZOff(int number)
	{
		if (number == 0) return "Off"; else return DisplayNumber2Zeros(number);
	};
	//
	static String DisplayEQLow(float value)
	{
		if (value == 1.0f) return paddWithSpaces("Off", 6, true);
		else return paddWithSpaces(String(10 + int((value * value * value) * (24000.0f - 10.0f))), 6, true);
	};
	//
	static String DisplayEQHigh(float value)
	{
		if (value == 0.0f) return paddWithSpaces("Off", 6, true);
		else return paddWithSpaces(String(10 + int((value * value * value) * (24000.0f - 10.0f))), 6, true);
	};
	//
	static String DisplayEQBand(float value)
	{
		return paddWithSpaces(String(10 + int((value * value * value) * (24000.0f - 10.0f))), 5, true);
	};
	//
	static String DisplayOnOff(bool isOn)
	{
		if (isOn) return "On";
		return "Off";
	};
	//
	static String DisplayOnOffPadded(bool isOn)
	{
		if (isOn) return "On ";
		return "Off";
	};
	//
	static void ChangeValue(int &value, bool add, bool isShift, int min, int max, int rate = 1, int center = 99, bool isCtrl = false, int rateCtrl = 10)
	{
		if (isCtrl)
		{
			int xadd = rateCtrl;
			if (!add) xadd = -rateCtrl;
			value = jlimit(min, max, value + xadd);
		}
		else if (isShift)
		{
			if (center != 99)
			{
				if (add && value == center) value = max;
				else if (!add && value == center) value = min;
				else value = center;
			}
			else
			{
				if (add) value = max; else value = min;
			}
		}
		else
		{
			int xadd = rate;
			if (!add) xadd = -rate;
			value = jlimit(min, max, value + xadd);
		}
	};
	//
	static void ChangeValue(uint16_t &value, bool add, bool isShift, int min, int max, int rate = 1, int center = 99, bool isCtrl = false, int rateCtrl = 10)
	{
		if (isCtrl)
		{
			uint16_t xadd = rateCtrl;
			if (!add) xadd = -rateCtrl;
			value = jlimit(min, max, value + xadd);
		}
		else if (isShift)
		{
			if (center != 99)
			{
				if (add && value == center) value = max;
				else if (!add && value == center) value = min;
				else value = center;
			}
			else
			{
				if (add) value = max; else value = min;
			}
		}
		else
		{
			uint16_t xadd = rate;
			if (!add) xadd = -rate;
			value = jlimit(min, max, value + xadd);
		}
	};
	//
	static void ChangeValue(float &value, bool add, bool isShift, float min, float max, float rate, float center = 99.0f, bool isCtrl = false, float rateCtrl = 0.1f)
	{
		if (isCtrl)
		{
			float xadd = rateCtrl;
			if (!add) xadd = -rateCtrl;
			value = jlimit(min, max, value + xadd);
		}
		else if (isShift)
		{
			if (center != 99.0f)
			{
				if (add && value == center) value = max;
				else if (!add && value == center) value = min;
				else value = center;
			}
			else
			{
				if (add) value = max; else value = min;
			}
		}
		else
		{
			float xadd = rate;
			if (!add) xadd = -rate;
			value = jlimit(min, max, value + xadd);
		}
	};
	//
	static void ChangeValue(double &value, bool add, bool isShift, double min, double max, double rate, double center = 99.0f, bool isCtrl = false, double rateCtrl = 0.1f)
	{
		if (isCtrl)
		{
			double xadd = rateCtrl;
			if (!add) xadd = -rateCtrl;
			value = jlimit(min, max, value + xadd);
		}
		else if (isShift)
		{
			if (center != 99.0f)
			{
				if (add && value == center) value = max;
				else if (!add && value == center) value = min;
				else value = center;
			}
			else
			{
				if (add) value = max; else value = min;
			}
		}
		else
		{
			double xadd = rate;
			if (!add) xadd = -rate;
			value = jlimit(min, max, value + xadd);
		}
	};
	//
	static String showStereoPan(float pan)
	{
		String stereoPan;
		//
		if (pan > -0.01f && pan < 0.01f) stereoPan = "CT";
			else if (pan < 0.0f) stereoPan = "L" + String(int((pan * -1.0f) * 9.0f));
			else stereoPan = "R" + String(int(pan * 9.0f));
		//
		return stereoPan;
	};
	//
	static String paddWithSpaces(String msg, int spaces, bool onFront)
	{
		static String pwRt;
		pwRt = String();
		//
		for (int x = 0; x < spaces - msg.length(); x++)
		{
			pwRt.append(" ", 99);
		}
		//
		if (onFront) return pwRt + msg; else return msg + pwRt;
	};
	//
	static String updateName(String name)
	{
		return File::createLegalFileName(name.replace("_", " ").replace("x64", "").replace(" 64", "").replace("x32", "").replace(" 32", "").replace("x86", "").replace("\\", "-").replace("/", "-"));
	};
	//
	static float floatToDecibel(float value)
	{
		if (value != 0.0f) return 20.0f * log10(value);
		return -144.0f;  // effectively minus infinity
	};
	//
	static String midiChannel(int channel)
	{
		if (channel == 0) return "**";
		else return WUTILS::DisplayNumber1Zero(channel);
	};
	//
	static String toDecibelFormated(float value, int spaces)
	{
		String xReturn = "-Inf";
		//
		if (value > 0.001) xReturn = String(floatToDecibel(value) , 1);
		//
		if (value > 1.0f) xReturn = "+" + xReturn;
		else if (value == 1.0f) xReturn = "0 dB";
		return paddWithSpaces(xReturn, spaces, true);
	};
	//
	static String formatParameterNameAndValue(AudioPluginInstance &thePlugin, int parameter, String forceName = String())
	{
		String output;
		//
		String name = thePlugin.getParameterName(parameter);
		if (forceName.isNotEmpty()) name = forceName;
		if (name.isEmpty()) name = "Param." + String(parameter + 1);
		String text = thePlugin.getParameterText(parameter);
		String label = thePlugin.getParameterLabel(parameter);
		float value = thePlugin.getParameter(parameter);
		String valueString = WUTILS::DisplayNumber2Zeros(int(value * 100.0f));
		//
		if (text.isNotEmpty()) valueString = text + label;
		if (valueString.length() < 16)
		{
			output = name.substring(0, 16 - valueString.length());
			output = WUTILS::paddWithSpaces(output, 16 - valueString.length(), false) + valueString;
		} 
		else output = valueString;
		//
		return output;
	};
	//
	static void vuMeterCalculate(float audio, String &top, String &bottom)
	{
		if (audio > 1.0f)
		{
			top = "*";
			bottom = "{7";
		}
		else
		{
			int vuValue = jmin(16, int(fabs(audio) * 16.0f));
			//
			if (vuValue == 0)
			{
				top = " ";
				bottom = " ";
			}
			else
			{
				if (jmax(0, vuValue - 8) == 0) top = " "; else top = "{" + String(jmax(0, vuValue - 9));
				bottom = "{" + String(jmin(7, vuValue - 1));
			}
		}
	};
	//
	static void deleteFiles(File folder, String wildcard)
	{
		DirectoryIterator xFiles(folder, false, wildcard, File::TypesOfFileToFind::findFiles);
		while (xFiles.next())
		{
			xFiles.getFile().deleteFile();
		}
	}
	//
	static bool folderHasFiles(File folder)
	{
		DirectoryIterator xFiles(folder, true, "*", File::TypesOfFileToFind::findFiles);
		return xFiles.next();
	}
	//
	static StringArray getVersionSegments(String p)
	{
		auto segments = StringArray::fromTokens(p, ",.", "");
		segments.trim();
		segments.removeEmptyStrings();
		return segments;
	}
	//
	static int getVersionAsHexInteger(String p)
	{
		auto segments = getVersionSegments(p);

		auto value = (segments[0].getIntValue() << 16)
			+ (segments[1].getIntValue() << 8)
			+ segments[2].getIntValue();

		if (segments.size() > 3)
			value = (value << 8) + segments[3].getIntValue();

		return value;
	}
};


#endif  // MYPLUGINLIST_H_INCLUDED
