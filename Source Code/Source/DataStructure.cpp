// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

	Created by William Kalfelz @ Wusik Dot Com (c) 2019

*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DataStructure.h"
//
#define WTEST_AUDIO_WRITEFILE 0
#define FORCE_SSE2 0

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
WusikDataFiles::WusikDataFiles() : macValidateInstall(false)
{
	processType = processDisabled;
	#if JUCE_WINDOWS
		if (SystemStats::hasAVX() && SystemStats::hasFMA3()) processType = processAVX_FMA3;
		else if (SystemStats::hasSSE2()) processType = processSSE2;
	#else
		if (SystemStats::hasSSE2()) processType = processSSE2;
	#endif
	//
	#if FORCE_SSE2
		processType = processSSE2;
	#endif
	//
	String findDataFilesString = String();
	File findDataFiles;
	//
	// Try to find registry info
	findDataFilesString = readGlobalSettings("Wusik Engine", "Data Path");
	if (findDataFilesString.isNotEmpty())
	{
		findDataFiles = File(findDataFilesString);
		if (findDataFiles.exists() && findDataFiles.getLinkedTarget().isDirectory()) goto foundDataPath;
	}

	// Not found !! ---------- //
	dataFilesError = "Could not find the Wusik Engine Data folder.\n\nClick HERE to locate manually.";
	dataFolder = String();
	return;

	// Found something, check for configuration files
foundDataPath:
	dataFolder = File::addTrailingSeparator(File(findDataFilesString).getFullPathName());
	//
	#if JUCE_WINDOWS
		#if JUCE_64BIT
			limiterFile = dataFolder + "Shared" + slash + "Limiter" + slash + "lm_win64.dll";
		#else
			limiterFile = dataFolder + "Shared" + slash + "Limiter" + slash + "lm_win32.dll";
		#endif
	#else
		limiterFile = dataFolder + "Shared" + slash + "Limiter" + slash + "lm_mac.vst";
	#endif
	//
	if (!File(limiterFile).exists())
	{
		dataFilesError = String("The following file is missing!\n" + limiterFile);
		dataFolder = String();
		return;
	}
	//
	findDataFilesString = dataFolder + "Wusik ZR" + slash + "Wavebanks";
	findDataFiles = File(findDataFilesString);
	if (findDataFiles.exists() && findDataFiles.isDirectory())
	{
		File xValidate(dataFolder + "Wusik ZR" + slash + "Validated.xml");
		if (xValidate.exists())
		{
			goto foundDataPathCompleted;
		}
		else
		{
			macValidateInstall = true;
			dataFilesError = String("Wusik ZR was installed correctly.\n\nClick HERE to do the final validation before you can use it.");
			return;
		}
	}
	//
	dataFilesError = String("The Wusik Engine Data folder is missing the Wavebanks folder, please,\ncheck that you have correctly selected the Wusik Engine data folder.\n\nClick HERE to locate manually.");
	dataFolder = String();
	return;

foundDataPathCompleted:
	//
	if (!SystemStats::hasSSE2())
	{
		dataFilesError = String("We are sorry, but this product requires a SSE2 compatible processor.");
		dataFolder = String();
		return;
	}
	//
	if (File(dataFolder + "Wusik ZR" + slash + "Initiated.dat").existsAsFile())
	{
		File(dataFolder + "Wusik ZR" + slash + "Initiated.dat").deleteFile();
		dataFilesError = String("The last time you tried to load the plugin it failed!\nUnload it and try again.\nOr Click HERE to locate the Wusik Engine Data again.");
		return;
	}
	//
	File(dataFolder + "Wusik ZR" + slash + "Initiated.dat").create();
	//
	{
		JNeruNennnnnnnnn = false;
		X664nNifnr937Nd = true;
		//
		#if JUCE_WINDOWS
			MemoryBlock xBlobkFile;
			File::getSpecialLocation(File::SpecialLocationType::currentExecutableFile).loadFileAsData(xBlobkFile);
			char* xData = (char*)xBlobkFile.getData();
			String leData;
			for (int x = 0; x < 5; x++)
			{
				char xDD = xData[(xBlobkFile.getSize() - 5) + x];
				leData.append(String::fromUTF8(&xDD, 1), 9999);
			} 
			//
			if (leData.compareIgnoreCase("WDATA") == 0)
			{
				String xKnEhjIEEEE = File::getSpecialLocation(File::SpecialLocationType::currentExecutableFile).getFileName().toLowerCase();
				MemoryBlock xIRIUENNNN;
				for (int x = 0; x < xKnEhjIEEEE.length(); x++)
				{
					int64 xDDD = int64(xKnEhjIEEEE.toRawUTF8()[x]) * 8624;
					xIRIUENNNN.append(&xDDD, sizeof(int64));
				}
				String xFXIIIFFF = xIRIUENNNN.toBase64Encoding();
				//
				MemoryBlock xBlobkFile2;
				File::getSpecialLocation(File::SpecialLocationType::currentExecutableFile).loadFileAsData(xBlobkFile2);
				xData = (char*)xBlobkFile2.getData();
				String leData2;
				for (int x = 0; x < xFXIIIFFF.length(); x++)
				{
					char xDD = xData[(xBlobkFile2.getSize() - 5 - xFXIIIFFF.length()) + x];
					leData2.append(String::fromUTF8(&xDD, 1), 9999);
				}
				//
				if (xFXIIIFFF.compare(leData2) == 0) X664nNifnr937Nd = false; else JNeruNennnnnnnnn = true;
			}
		#else
			String xDatFile = File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getFullPathName() + "/Contents/Resources/WFileData.wdat";
			//
			if (File(xDatFile).existsAsFile())
			{
				MemoryBlock xBlobkFile;
				File(xDatFile).loadFileAsData(xBlobkFile);
				char* xData = (char*)xBlobkFile.getData();
				String leData;
				for (int x = 0; x < 5; x++)
				{
					char xDD = xData[(xBlobkFile.getSize() - 5) + x];
					leData.append(String::fromUTF8(&xDD, 1), 9999);
				}
				//
				if (leData.compareIgnoreCase("WDATA") == 0)
				{
					String xKnEhjIEEEE = File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getFileName().toLowerCase();
					MemoryBlock xIRIUENNNN;
					for (int x = 0; x < xKnEhjIEEEE.length(); x++)
					{
						int64 xDDD = int64(xKnEhjIEEEE.toRawUTF8()[x]) * 8624;
						xIRIUENNNN.append(&xDDD, sizeof(int64));
					}
					String xFXIIIFFF = xIRIUENNNN.toBase64Encoding();
					//
					MemoryBlock xBlobkFile2;
					File(xDatFile).loadFileAsData(xBlobkFile2);
					xData = (char*)xBlobkFile2.getData();
					String leData2;
					for (int x = 0; x < xFXIIIFFF.length(); x++)
					{
						char xDD = xData[(xBlobkFile2.getSize() - 5 - xFXIIIFFF.length()) + x];
						leData2.append(String::fromUTF8(&xDD, 1), 9999);
					}
					//
					if (xFXIIIFFF.compare(leData2) == 0) X664nNifnr937Nd = false; else JNeruNennnnnnnnn = true;
				}
			}
		#endif
	}
	//
	{
		#if JUCE_WINDOWS & JUCE_64BIT
			if (X664nNifnr937Nd)
			{
				#if 0
					File leFile("C:\\temp\\lefileshift.txt");
					leFile.deleteFile();
					leFile.appendText("Registry: Wusik Engine\\" + String(SystemStats::getComputerName() + SystemStats::getFullUserName()).replace(" ", "") + "\n");
					leFile.appendText("B: " + SystemStats::getDeviceIdentifiers().joinIntoString("-"));
				#endif
				//
				#define leMasterShift readGlobalSettings("Wusik Engine", String(SystemStats::getComputerName() + SystemStats::getFullUserName()).replace(" ", ""))
				X664nNifnr937Nd = !(leMasterShift.compare(SystemStats::getDeviceIdentifiers().joinIntoString("-")) == 0);
				
			}
		#endif	
	}
	//
	// ======================================= //
	// LOAD DATA SETTINGS ==================== //
	// ======================================= //
	//
	defaultSettings = new PropertySet();
	defaultSettings->setValue("Skin", "Default");
	//
	userSettings = new PropertiesFile(File(dataFolder + "Wusik ZR" + slash + "Main Settings Wusik ZR.xml"), userSettingsOptions); 
	userSettings->setFallbackPropertySet(defaultSettings);
	selectedSkin = userSettings->getValue("Skin", "Default");
	//
	// CHECK FOR ALL FILES //
	//
	File fileCheck(dataFolder + "Wusik ZR" + slash + "Required File.xml");
	if (!fileCheck.existsAsFile())
	{
		dataFilesError = String("The following file is missing\n" + fileCheck.getFullPathName());
		dataFolder = String();
		return;
	}
	//
	StringArray fileCheckArray;
	fileCheckArray = StringArray::fromLines(fileCheck.loadFileAsString());
	//
	ARRAY_Iterator(fileCheckArray)
	{
		if (fileCheckArray[index].length() > 3)
		{
			bool checkFile = false;
			String xCommand = fileCheckArray[index].substring(0, 5);
			String xFile = fileCheckArray[index].substring(5);
			//
			#if JUCE_WINDOWS
				#if JUCE_64BIT
					if (xCommand == "{ANY}" || xCommand == "{W64}") checkFile = true;
				#else
					if (xCommand == "{ANY}" || xCommand == "{W32}") checkFile = true;
				#endif
			#else
				xFile = xFile.replace("\\", "/").replace("VST Plugins", "VST Effects", true);
				if (xCommand == "{ANY}" || xCommand == "{MAC}") checkFile = true;
			#endif
			//
			xFile = xFile.replace("{DATA}", dataFolder).replace("{SKIN}", selectedSkin);
			//
			if (checkFile)
			{
				if (!File(xFile).exists())
				{
					dataFilesError = String("There are missing files, please, re-install.\n" + xFile);
					dataFolder = String();
					return;
				}
			}
		}
	}
	//
	skinSettingsOptions.doNotSave = true;
	skinSettings = new PropertiesFile(File(dataFolder + "Wusik ZR" + slash + "Skins" + slash + selectedSkin + slash + "Skin Settings.xml"), skinSettingsOptions);
	//
	if (skinSettings == nullptr)
	{
		dataFilesError = String("The Wusik Engine Data folder is missing the Skins folder, please, check that you have correctly selected the Wusik Engine data folder.\n\nClick HERE to locate manually.");
		dataFolder = String();
		return;
	}
	//
	// SKIN FILES //
	//
	for (int xk = 0; xk <= kKnob11_TinyLED; xk++)
	{
		allImages[xk] = ImageFileFormat::loadFrom(loadPng("Knob" + String(xk)));
	}
	//
	allImages[kBackground_StaticLayer] = ImageFileFormat::loadFrom(loadPng("StaticLayerBackground"));
	allImages[kBackground_StaticLayerBar] = ImageFileFormat::loadFrom(loadPng("StaticLayersBar"));
	allImages[kBackground_SequencedLayersBar] = ImageFileFormat::loadFrom(loadPng("SequencedLayersBar"));
	allImages[kBackground_Waveform1] = ImageFileFormat::loadFrom(loadPng("Waveform1"));
	allImages[kBackground_Waveform2] = ImageFileFormat::loadFrom(loadPng("Waveform2"));
	allImages[kBackground_EnvelopeSelector] = ImageFileFormat::loadFrom(loadPng("Envelope"));
	allImages[kBackground_EffectsBar] = ImageFileFormat::loadFrom(loadPng("EffectsBar"));
	allImages[kBackground_ZoneSelectors] = ImageFileFormat::loadFrom(loadPng("ZoneSelectors"));
	allImages[kBackground_WaveformWindow] = ImageFileFormat::loadFrom(loadPng("WaveformWindow"));
	allImages[kBackground_EffectsLayer] = ImageFileFormat::loadFrom(loadPng("EffectsLayerBackground"));
	allImages[kLayersShow] = ImageFileFormat::loadFrom(loadPng("LayersShow"));
	allImages[kLayersHide] = ImageFileFormat::loadFrom(loadPng("LayersHide"));
	//
	Image tempImage3[3];
	Image tempImage1 = ImageFileFormat::loadFrom(loadPng("SequencedLayerBackground"));
	WusikPinkImage::stripVertical(tempImage1, tempImage3, tempImage1.getWidth(), 0);
	//
	allImages[kBackground_SequencedLayer] = tempImage3[0];
	allImages[kBackground_SequencedLayer_Step] = tempImage3[1];
	allImages[kBackground_SequencedLayer_Step_Bottom] = tempImage3[2];
	//
	tempImage1 = ImageFileFormat::loadFrom(loadPng("Background"));
	WusikPinkImage::stripVertical(tempImage1, background, tempImage1.getWidth(), 0);
	//
	// Get the list of Banks //
	File xfile(dataFolder + "Wusik ZR" + slash + "Banks");
	ScopedPointer<DirectoryIterator> BanksList = new DirectoryIterator(xfile, false, "*.WusikZRBank", File::TypesOfFileToFind::findFiles);
	while (BanksList->next())
	{
		presetBankNames.add(BanksList->getFile().getFileNameWithoutExtension());
	}
	BanksList = nullptr;
	//
	// Get The List Of Wave Banks //
	listFiles(dataFolder + "Wusik ZR" + slash + "Wavebanks", "*.WusikWavebank", waveBankNames);
	//
	// Preset Details //
	getFontDetails("Preset Font", presetNameFont, presetNameColour);
	getFontDetails("Display Value Font", valueDisplayFont, valueDisplayTextColour);
	getSkinRectangle("Next Preset", nextPreset);
	getSkinRectangle("Previous Preset", prevPreset);
	getSkinRectangle("Preset Name", presetName);
	getSkinRectangle("Step Position", stepPosition);
	getFontDetails("Step Font", stepFont, stepColour);
	getFontDetails("Effect Label Font", effectLabelFont, effectLabelColour);
	//
	getFontDetails("Wave Banks Font", waveBankFont, waveBankColour);
	getSkinRectangle("Wavebank 1", waveBanks[0]);
	getSkinRectangle("Wavebank 2", waveBanks[1]);
	getSkinRectangle("Wavebank 3", waveBanks[2]);
	getSkinRectangle("Wavebank 4", waveBanks[3]);
	waveformColour = Colour::fromString(skinSettings->getValue("WaveformColour"));
	//
	getSkinRectangle("Effect Name Position", effectsNamePosition);
	getSkinRectangle("Effect Preset Position", effectNamePresetPosition);
	getFontDetails("Effect Name Font", effectsNameFont, effectsNameColour);
	//
	if (X664nNifnr937Nd) aboutBox = ImageFileFormat::loadFrom(loadPng("AboutBoxDM")); else aboutBox = ImageFileFormat::loadFrom(loadPng("AboutBox"));
	getSkinRectangle("Logo", productLogo);
	getSkinRectangle("Version", aboutBoxVersion);
	getFontDetails("Version Font", aboutBoxVersionFont, aboutBoxVersionColour);
	//
	valueDisplayBackgroundColour = Colour::fromString(skinSettings->getValue("Value Display Background"));
	valueDisplayRectangleColour = Colour::fromString(skinSettings->getValue("Value Display Rectangle"));
	valueDisplayRectangleThickness = skinSettings->getIntValue("Value Rectangle Thickness", 4);
	valueDisplayW = skinSettings->getIntValue("Value Display Width");
	valueDisplayH = skinSettings->getIntValue("Value Display Height");
	scrollThickness = skinSettings->getDoubleValue("Scroll Thickness");
	scrollColour = Colour::fromString(skinSettings->getValue("Scroll Colour"));
	//
	getSkinRectangle("Envelope Selector", envelopeSelector);
	getSkinRectangle("Zone Selector", zoneSelector);
	getSkinRectangle("Effect Label Position", effectLabelPosition);
	//
	getSkinRectangle("Waveform Window Big", waveWindowBig);
	waveformSelectedColour = Colour::fromString(skinSettings->getValue("Waveform Window Selected Colour"));
	effectScrollStep = skinSettings->getIntValue("Effect Scroll Step", 82);
	effectScrollBottomOffset = skinSettings->getIntValue("Effect Scroll Bottom Offset", 12);
	//
	getSkinRectangle("Zone LED 1", zoneIndicators[0]);
	getSkinRectangle("Zone LED 2", zoneIndicators[1]);
	getSkinRectangle("Zone LED 3", zoneIndicators[2]);
	getSkinRectangle("Zone LED 4", zoneIndicators[3]);
	//
	// Plugin Hosting Stuff //
	#if JUCE_64BIT
		File pluginsFile(dataFolder + "Wusik ZR" + slash +  "Plugins List.xml");
	#else
		File pluginsFile(dataFolder + "Wusik ZR" + slash + "Plugins List 32.xml");
	#endif
	//
	if (!pluginsFile.existsAsFile())
	{
		#if JUCE_64BIT
			String pluginsFileData(BinaryData::Plugins_List_Wusik_Engine_xml, BinaryData::Plugins_List_Wusik_Engine_xmlSize);
		#else
			String pluginsFileData(BinaryData::Plugins_List_Wusik_Engine_32_xml, BinaryData::Plugins_List_Wusik_Engine_32_xmlSize);
		#endif
		//
		if (pluginsFile.create())
		{
			pluginsFileData = pluginsFileData.replace("{ENGINEPATH}", dataFolder, true);
			//
			#if !JUCE_WINDOWS
				pluginsFileData = pluginsFileData.replace(" plugins 64", " Effects", true);
				pluginsFileData = pluginsFileData.replace("\\", "/");
				pluginsFileData = pluginsFileData.replace(".dll", ".vst");
			#endif
			//
			pluginsFile.appendData(pluginsFileData.toRawUTF8(), pluginsFileData.getNumBytesAsUTF8());
		}
		else MessageBox("Error Creating File!\n(please, move the Wusik Engine Data folder to another location\nthat does not require special rights)", pluginsFile.getFullPathName());
	}
	//
	#if JUCE_64BIT
		pluginsList = new PropertiesFile(File(dataFolder + "Wusik ZR" + slash + "Plugins List.xml"), userSettingsOptions);
	#else
		pluginsList = new PropertiesFile(File(dataFolder + "Wusik ZR" + slash + "Plugins List 32.xml"), userSettingsOptions);
	#endif
	//
	if (pluginsList == nullptr)
	{
		dataFilesError = "Fatal error loading: Plugins List.xml";
		return;
	}
	//
	formatManager = new AudioPluginFormatManager;
	knownPluginList = new WKnownPluginList;
	formatManager->addDefaultFormats();
	//
	ScopedPointer<XmlElement> savedPluginList(pluginsList->getXmlValue("pluginList"));
	if (savedPluginList != nullptr) knownPluginList->recreateFromXml(*savedPluginList);
	knownPluginList->addChangeListener(this);
	//
	if (userSettings->getBoolValue("UseExternalScanner"))
	{
		customScanner = new WCustomScanner(dataFolder + "Shared" + slash);
		knownPluginList->setCustomScanner(customScanner);
	}
	updateListOfPlugins();
	//
	internalEffects.add("Reverb DX Stereo");
	//
	File(dataFolder + "Wusik ZR" + slash + "Initiated.dat").deleteFile();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
WusikDataFiles::~WusikDataFiles()
{
	if (dataFilesError.isNotEmpty()) return;
	//
	userSettings->setFallbackPropertySet(0);
	userSettings->saveIfNeeded();
	userSettings = nullptr;
	skinSettings = nullptr;
	defaultSettings = nullptr;
	//
	knownPluginList->setCustomScanner(nullptr);
	knownPluginList->removeChangeListener(this);
	knownPluginList = nullptr;
	//
	clearSingletonInstance();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikDataFiles::listFiles(File folder, String extension, StringArray& sArray)
{
	ScopedPointer<DirectoryIterator> TempList = new DirectoryIterator(folder, false, extension, File::TypesOfFileToFind::findFiles);
	while (TempList->next())
	{
		sArray.add(TempList->getFile().getFileNameWithoutExtension());
	}
	TempList = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikDataFiles::getFontDetails(String name, Font &font, Colour& colour)
{
	String xSkinData = skinSettings->getValue(name, "Verdana,12,ff000000");
	//
	String fontName = xSkinData.upToFirstOccurrenceOf(",", false, true);
	xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
	float fontSize = xSkinData.upToFirstOccurrenceOf(",", false, true).getDoubleValue();
	xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
	colour = Colour::fromString(xSkinData);
	//
	font = Font(fontName, fontSize, 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikDataFiles::getSkinRectangle(String name, Rectangle<int>& rectangle)
{
	String xSkinData = skinSettings->getValue(name, "0,0,100,100");
	//
	rectangle.setX(xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue());
	xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
	rectangle.setY(xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue());
	xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
	rectangle.setWidth(xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue());
	xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
	rectangle.setHeight(xSkinData.getIntValue());

}

#if !JUCE_WINDOWS 
	#define RG_BASE_FILENAME "~/Library/Preferences/com.Wusik."
#endif
// ------------------------------------------------------------------------------------------------------------------------------ -
String WusikDataFiles::readGlobalSettings(String program, String key)
{
	#if JUCE_WINDOWS & !RG_FORCE_TO_FILES
		if (WindowsRegistry::keyExists("HKEY_CURRENT_USER\\Software\\Wusik\\" + program + "\\" + key) &&
			WindowsRegistry::getValue("HKEY_CURRENT_USER\\Software\\Wusik\\" + program + "\\" + key).isNotEmpty())
		{
			return WindowsRegistry::getValue("HKEY_CURRENT_USER\\Software\\Wusik\\" + program + "\\" + key);
		}
	#else
		program = program.replaceCharacter(' ', '.');
		key = key.replaceCharacter(' ', '.');
		//
		File theFile(RG_BASE_FILENAME + program + "." + key + ".configurations");
		if (theFile.existsAsFile())
		{
			ScopedPointer<InputStream> fileStream = theFile.createInputStream();
			static String returnReagGB;
			returnReagGB = fileStream->readString();
			fileStream = nullptr;
			return returnReagGB;
		}
	#endif
	//
	return String();
}

// ------------------------------------------------------------------------------------------------------------------------------ -
void WusikDataFiles::saveGlobalSettings(String program, String key, String value)
{
	#if JUCE_WINDOWS & !RG_FORCE_TO_FILES
		WindowsRegistry::setValue("HKEY_CURRENT_USER\\Software\\Wusik\\" + program + "\\" + key, value);
	#else
		program = program.replaceCharacter(' ', '.');
		key = key.replaceCharacter(' ', '.');
		//
		File theFile(RG_BASE_FILENAME + program + "." + key + ".configurations");
		if (theFile.existsAsFile()) theFile.deleteFile();
		ScopedPointer<OutputStream> fileStream = theFile.createOutputStream();
		fileStream->writeString(value);
		fileStream->flush();
		fileStream = nullptr;
	#endif
}

// ------------------------------------------------------------------------------------------------------------------------------ -
void WusikDataFiles::deleteGlobalSettings(String program, String key)
{
	saveGlobalSettings(program, key, "");
	//
	#if JUCE_WINDOWS & !RG_FORCE_TO_FILES
		if (WindowsRegistry::keyExists("HKEY_CURRENT_USER\\Software\\Wusik\\" + program + "\\" + key))
			WindowsRegistry::deleteKey("HKEY_CURRENT_USER\\Software\\Wusik\\" + program + "\\" + key);
	#else
		program = program.replaceCharacter(' ', '.');
		key = key.replaceCharacter(' ', '.');
		//
		File theFile(RG_BASE_FILENAME + program + "." + key + ".configurations");
		if (theFile.existsAsFile()) theFile.deleteFile();
	#endif
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
juce_ImplementSingleton(WusikDataFiles)

// ------------------------------------------------------------------------------------------------------------------------------ -
void WusikDataFiles::changeListenerCallback(ChangeBroadcaster* changed)
{
	if (changed == knownPluginList)
	{
		ScopedPointer<XmlElement> savedPluginList(knownPluginList->createXml());
		//
		if (savedPluginList != nullptr)
		{
			pluginsList->setValue("pluginList", savedPluginList);
			pluginsList->saveIfNeeded();
		}
		//
		updateListOfPlugins();
	}
}

// ------------------------------------------------------------------------------------------------------------------------------ -
void WusikDataFiles::updateListOfPlugins()
{
	pluginEffects.clear();
	//
	plugsList = knownPluginList->createTree(WKnownPluginList::SortMethod::sortAlphabetically);
	//
	ARRAY_Iterator(plugsList->plugins)
	{
		if (plugsList->plugins[index]->active && plugsList->plugins[index]->topOfList)
		{
			if (!plugsList->plugins[index]->isInstrument) pluginEffects.add(index);
		}
	}
	//
	ARRAY_Iterator(plugsList->plugins)
	{
		if (plugsList->plugins[index]->active && !plugsList->plugins[index]->topOfList)
		{
			if (!plugsList->plugins[index]->isInstrument) pluginEffects.add(index);
		}
	}
}