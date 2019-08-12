/*

	Wusik SQ200 (c) Wusik Dot Com 2019
	William Kalfelz
	www.Wusik.com
	www.Kalfelz.com

*/

#ifndef MYPLUGLIST_CPP_INCLUDED
#define MYPLUGLIST_CPP_INCLUDED

#include "WusikScanner.h"
#include "PipedPlugin.cpp"
//
// -------------------------------------------------------------------------------------------------------------------------------
#define ANERROR { if (scannerProcess.isRunning()) scannerProcess.kill(); continue; }
//
bool WCustomScanner::findPluginTypesFor(AudioPluginFormat& format, OwnedArray <PluginDescription>& result, const String& fileOrIdentifier, String& pluginNameBeenScanned, PropertiesFile& propertiesToUse)
{
	String originalName = pluginNameBeenScanned;
	int64 isInstrument = 0;
	//
	#if JUCE_WINDOWS & JUCE_64BIT
		for (int x = 0; x < 4; x++)
		{
			bool scanning64Bits = true;
			pluginNameBeenScanned = originalName + "\n\nAttempt " + String(x + 1) + "/4";
	#else
		for (int x = 0; x < 2; x++)
		{
			//
			pluginNameBeenScanned = originalName + "\n\nAttempt " + String(x + 1) + "/2";
	#endif
		//
		#if WUSIK_X42
			#if JUCE_WINDOWS
				#if JUCE_64BIT
					File sheelFile;
					if (x == 0 || x == 2)			sheelFile = File(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerX.exe");
					else if (x == 1 || x == 3)		{	sheelFile = File(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerX32.exe"); scanning64Bits = false; }
				#else
					File sheelFile;
					sheelFile = File(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerX32.exe");
				#endif
			#else
				File sheelFile(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerX");
			#endif
		#else
			#if JUCE_WINDOWS
				#if JUCE_64BIT
					File sheelFile;
					if (x == 0 || x == 2)			sheelFile = File(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerD.exe");
					else if (x == 1 || x == 3)		{	sheelFile = File(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerD32.exe"); scanning64Bits = false; }
				#else
					File sheelFile;
					sheelFile = File(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerD32.exe");
				#endif
			#else
				File sheelFile(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerD");
			#endif
		#endif
		//
		int position = 0;
		while (1)
		{
			File tempFileResultDelete(dataFolderLocale + "Scanner" + slash + "ScanResult" + String(position) + ".dat");
			if (tempFileResultDelete.exists()) tempFileResultDelete.deleteFile(); else break;
			position++;
		}
		//
		File tempFile(dataFolderLocale + "Scanner" + slash + "ScanThis.dat");
		File tempFileResult(dataFolderLocale + "Scanner" + slash + "ScanResult0.dat");
		tempFile.deleteFile();
		tempFileResult.deleteFile();
		tempFile.create();
		tempFile.appendText(fileOrIdentifier);
		//
		ChildProcess scannerProcess;
		StringArray theProcess;
		theProcess.add(sheelFile.getFullPathName());
		if (!scannerProcess.start(theProcess)) ANERROR
		//
		int xCounter = 0;
		while (scannerProcess.isRunning())
		{
			Thread::sleep(100);
			xCounter++;
			if (xCounter > 1200) break;
		}
		//
			   		 
		// !!!!!!!!!!!!!!!!!!!! ADD SELECTION BRIDGE/NOT-BRIDGED OPTION

		if (!tempFileResult.existsAsFile()) ANERROR
		else
		{
			position = 0;
			while (1)
			{
				tempFileResult = File(dataFolderLocale + "Scanner" + slash + "ScanResult" + String(position) + ".dat");
				if (!tempFileResult.exists()) break;
				position++;
				//
				XmlElement* myXML = nullptr;
				XmlDocument xDoc(tempFileResult);
				myXML = xDoc.getDocumentElement();
				//
				if (myXML != nullptr)
				{
					XmlElement* myXMLDESC = myXML->getFirstChildElement();
					//
					PluginDescription* xDesc = new PluginDescription;
					xDesc->loadFromXml(*myXMLDESC);
					//
					#if JUCE_WINDOWS & JUCE_64BIT
						xDesc->is32Bits = !scanning64Bits;
					#else
						xDesc->is32Bits = true;
					#endif
					//
					xDesc->customName = WUTILS::updateName(xDesc->name);
					if (xDesc->name.containsIgnoreCase("fx") && !xDesc->customName.containsIgnoreCase("fx")) xDesc->customName.append(" FX", 999);
					//
					result.add(xDesc);
					deleteAndZero(myXML);
					tempFile.deleteFile();
					tempFileResult.deleteFile();
				}
				else
				{
					ANERROR
				}
			}
 		}
		//
		if (scannerProcess.isRunning()) scannerProcess.kill();
		propertiesToUse.saveIfNeeded();
		return true;
		//
		AnError:
			if (scannerProcess.isRunning()) scannerProcess.kill();
	}
	//
	#if WUSIK_X42
		PluginDescription* xDesc = new PluginDescription;
		xDesc->name = File(fileOrIdentifier).getFileNameWithoutExtension();
		xDesc->customName = xDesc->name;
		xDesc->manufacturerName = "Failed To Scan";
		xDesc->dontBridge = true;
		xDesc->fileOrIdentifier = fileOrIdentifier;
		xDesc->isInstrument = true;
		xDesc->is32Bits = true;
		if (xDesc->name.containsIgnoreCase("64")) xDesc->is32Bits = false;
		xDesc->pluginFormatName = format.getName();;
		xDesc->numInputChannels = xDesc->numOutputChannels = 2;
		xDesc->numParameters = xDesc->numPrograms = 1;
		result.add(xDesc);
		//
		exitScan = true;
		propertiesToUse.saveIfNeeded();
		return true;
	#else
		exitScan = true;
		propertiesToUse.saveIfNeeded();
		return false;
	#endif
};

// -------------------------------------------------------------------------------------------------------------------------------
WCustomScanner::WCustomScanner(String _dataFolderLocale) : dataFolderLocale(_dataFolderLocale)
{
	exitScan = false;
};


// -------------------------------------------------------------------------------------------------------------------------------
WCustomScanner::~WCustomScanner()
{

};

// -------------------------------------------------------------------------------------------------------------------------------
void WCustomScanner::scanFinished()
{

};

// -------------------------------------------------------------------------------------------------------------------------------
bool WCustomScanner::shouldExit()
{
	return false;
};

// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------

static StringArray WreadDeadMansPedalFile(const File& file)
{
	StringArray lines;
	file.readLines(lines);
	lines.removeEmptyStrings();
	return lines;
}

void PluginEditorWindowTable::closeButtonPressed()
{
	if (((WPluginListComponent*) owner)->pluginEditorWindow != nullptr) ((WPluginListComponent*) owner)->pluginEditorWindow->setVisible(false);
	((WPluginListComponent*) owner)->pluginEditor = nullptr;
	((WPluginListComponent*) owner)->pluginLoad = nullptr;
	((WPluginListComponent*) owner)->pluginEditorWindow = nullptr;
}

class WPluginListComponent::TableModel  : public TableListBoxModel
{
public:
    TableModel (WPluginListComponent& c, WKnownPluginList& l)  : owner (c), list (l) {}

    int getNumRows() override
    {
        return list.getNumTypes() + list.getBlacklistedFiles().size();
    }

	void returnKeyPressed(int currentSelectedRow)
	{
		if (currentSelectedRow >= list.getNumTypes()) return;
		//
		PluginDescription* const desc = list.getType(currentSelectedRow);
		AlertWindow w("Edit Custom Name", "", AlertWindow::NoIcon);
		w.addTextEditor("Value1", desc->customName, "Normal: ");
		if (owner.showShortNames) w.addTextEditor("Value2", desc->shortCustomName, "Short: ");
		w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
		w.addButton("CANCEL", 0, KeyPress(KeyPress::escapeKey, 0, 0));
		//
		if (w.runModalLoop() == 1)
		{
			desc->customName = w.getTextEditorContents("Value1");
			desc->shortCustomName = w.getTextEditorContents("Value2");
			//
			if (desc->customName.isEmpty()) desc->customName = desc->name;
			owner.repaint();
			list.sendChangeMessage();
		}
	}

	void cellClicked(int row, int columnId, const MouseEvent& e)  override     
	{ 
		if (row >= list.getNumTypes()) return;
		//
		if (e.mods.isPopupMenu())
		{
			PluginDescription* const desc = list.getType(row);
			AlertWindow w("Edit Custom Name", "", AlertWindow::NoIcon);
			w.addTextEditor("Value1", desc->customName, "Normal: ");
			if (owner.showShortNames) w.addTextEditor("Value2", desc->shortCustomName, "Short: ");
			w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
			w.addButton("CANCEL", 0, KeyPress(KeyPress::escapeKey, 0, 0));
			//
			if (w.runModalLoop() == 1)
			{
				desc->customName = w.getTextEditorContents("Value1");
				desc->shortCustomName = w.getTextEditorContents("Value2");
				//
				if (desc->customName.isEmpty()) desc->customName = desc->name;
				owner.repaint();
				list.sendChangeMessage();
			}
		}
		else if (e.mods.isShiftDown() || e.mods.isMiddleButtonDown())
		{
			PluginDescription* const desc = list.getType(row);
			desc->active = !desc->active;
			owner.repaint();
			list.sendChangeMessage();
		}
		else if (e.mods.isCtrlDown())
		{
			PluginDescription* const desc = list.getType(row);
			desc->topOfList = !desc->topOfList;
			owner.repaint();
			list.sendChangeMessage();
		}
		else
		{
			PluginDescription* const desc = list.getType(row);
			PopupMenu m;
			m.addItem(1, "Edit Custom Name");
			//
			#if !WUSIK_P2000 & !WUSIK_SQ200 & !WUSIK_ENGINE
				if (owner.showBridge) m.addItem(66, "Edit Extra SubFolder(s)");
			#endif
			if (owner.showBridge) m.addItem(6, "Ignored", true, !desc->active); else m.addItem(6, "Is Hidden", true, !desc->active);
			//
			if (!desc->manufacturerName.containsIgnoreCase("Failed To Scan"))
			{
				if (owner.showBridge) m.addItem(60, "Bridged", true, !desc->dontBridge);
				m.addItem(4, "Set As Instrument", true, desc->isInstrument);
			}
			if (owner.showTopOfList) m.addItem(12, "Show First On List", true, desc->topOfList);
			if (owner.table.getSelectedRows().getTotalRange().getLength() > 1) m.addItem(2, "Remove Selected From List"); else m.addItem(2, "Remove From List");
			if (File(desc->fileOrIdentifier).exists()) m.addItem(992, "Reveal File");
			//
			int result = m.show();
			if (result == 2) deleteKeyPressed(0);
			else if (result == 992)
			{
				File(desc->fileOrIdentifier).revealToUser();
			}
			else if (result == 6)
			{
				desc->active = !desc->active;
				owner.repaint();
				list.sendChangeMessage();
			}
			else if (result == 60)
			{
				desc->dontBridge = !desc->dontBridge;
				//
				for (int i = 0; i < list.getNumTypes(); i++)
				{
					PluginDescription* const xdesc = list.getType(i);
					if (xdesc->fileOrIdentifier.compare(desc->fileOrIdentifier) == 0) xdesc->dontBridge = desc->dontBridge;
				}
				//
				owner.repaint();
				list.sendChangeMessage();
			}
			else if (result == 4)
			{
				desc->isInstrument = !desc->isInstrument;
				owner.repaint();
				list.sendChangeMessage();
			}
			else if (result == 12)
			{
				desc->topOfList = !desc->topOfList;
				owner.repaint();
				list.sendChangeMessage();
			}
			else if (result == 66)
			{
				AlertWindow w("Edit Extra Folder(s)", "Insert just the name of the extra subfolder, not a complete path. You can also add \"\\\" or / to create a multi-level subfolder.", AlertWindow::NoIcon);
					w.addTextEditor("Value1", desc->extraFolder, "Normal: ");
					w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
					w.addButton("CANCEL", 0, KeyPress(KeyPress::escapeKey, 0, 0));
				//
				if (w.runModalLoop() == 1)
				{
					desc->extraFolder = w.getTextEditorContents("Value1");
					//
					owner.repaint();
					list.sendChangeMessage();
				}
			}
			else if (result == 1)
			{
				AlertWindow w("Edit Custom Name", "", AlertWindow::NoIcon);
					w.addTextEditor("Value1", desc->customName, "Normal: ");
					if (owner.showShortNames) w.addTextEditor("Value2", desc->shortCustomName, "Short: ");
					w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
					w.addButton("CANCEL", 0, KeyPress(KeyPress::escapeKey, 0, 0));
				//
				if (w.runModalLoop() == 1)
				{
					desc->customName = w.getTextEditorContents("Value1");
					desc->shortCustomName = w.getTextEditorContents("Value2");
					//
					if (desc->customName.isEmpty()) desc->customName = desc->name;
					owner.repaint();
					list.sendChangeMessage();
				}
			}
			else if (result == 8)
			{
				owner.pluginEditorWindow = nullptr;
				owner.pluginEditor = nullptr;
				owner.pluginLoad = nullptr;
				//
				String errorMessage;
				owner.pluginLoad = owner.formatManager.createPluginInstance(*desc, 44100.0f, 128, errorMessage);
				if (owner.pluginLoad == nullptr)
				{
					MessageBox("Error Opening Plugin", errorMessage);
				}
				else
				{
					if (owner.pluginLoad->hasEditor())
					{
						owner.pluginEditor = owner.pluginLoad->createEditor();
						owner.pluginEditor->setBounds(0, 0, owner.pluginEditor->getWidth(), owner.pluginEditor->getHeight());
						owner.pluginEditorWindow = new PluginEditorWindowTable(desc->customName, (void*)&owner);
						owner.pluginEditorWindow->setContentNonOwned(owner.pluginEditor, true);
						owner.pluginEditorWindow->centreWithSize(owner.pluginEditor->getWidth(), owner.pluginEditor->getHeight());
						owner.pluginEditorWindow->setUsingNativeTitleBar(true);
						owner.pluginEditorWindow->setVisible(true);
						owner.pluginEditorWindow->setAlwaysOnTop(true);
					}
					else
					{
						MessageBox("Can't open the editor!", "The selected plugin has no Editor.");
					}
				}
			}
		}
	}

    void paintRowBackground (Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
		if (rowIsSelected) g.fillAll(owner.cRowBack); 
		else if (owner.useBackColour) g.fillAll(owner.cBackColour);
    }

    enum
    {
        nameCol = 1,
		extraFolderCol1,
		customCol1,
		customCol2,
		bridgeCol,
        typeCol,
		bitsCol,
        categoryCol,
        manufacturerCol,
		numPrograms,
		numParameters,
		//
		#if WUSIK_X42
			IOCol,
			channelLayoutsCol,
		#endif
		//
		midiIO,
        descCol
    };

    void paintCell (Graphics& g, int row, int columnId, int width, int height, bool /*rowIsSelected*/) override
    {
        String text;
		bool isActive = true;
        bool isBlacklisted = row >= list.getNumTypes();
		bool isUnscanned = false;

        if (isBlacklisted)
        {
            if (columnId == nameCol)
                text = File(list.getBlacklistedFiles() [row - list.getNumTypes()]).getFileName();
            else if (columnId == descCol)
                text = TRANS("Deactivated ! Bad Init");
        }
		else if (const PluginDescription* const desc = list.getType(row))
        {
			isActive = desc->active;
			#if WUSIK_X42
				isUnscanned = desc->manufacturerName.containsIgnoreCase("Failed To Scan");
			#endif
			//
            switch (columnId)
            {
                case nameCol:         text = desc->name; break;
                case typeCol:         text = desc->pluginFormatName; break;
				case categoryCol:     if (desc->manufacturerName.containsIgnoreCase("Failed To Scan")) text = "????"; else text = (desc->isInstrument) ? "Instrument" : "Effect"; break;
                case manufacturerCol: text = desc->manufacturerName; break;
				case customCol1:	  text = desc->customName; break;
				case extraFolderCol1: text = desc->extraFolder; break;
				case customCol2:	  text = desc->shortCustomName; break;
				case bridgeCol:		  if (!desc->dontBridge) text = "Bridged"; else text = ""; break;
				#if JUCE_WINDOWS & JUCE_64BIT
				case bitsCol:		  if (desc->manufacturerName.containsIgnoreCase("Failed To Scan")) text = "??"; else if (desc->is32Bits) text = "32"; else text = "64"; break;
				#endif
				case numParameters:	  text = String(desc->numParameters); break;
				case numPrograms:	  text = String(desc->numPrograms); break;
				case midiIO:		  if (desc->midiIN) text.append("I", 9); if (desc->midiOUT) text.append("O", 9); break;
                case descCol:         text = getPluginDescription (*desc); break;
				//
				#if WUSIK_X42
					case channelLayoutsCol: text = desc->possibleLayouts; break;
					case IOCol:			  text = String(desc->numInputChannels) + "/" + String(desc->numOutputChannels); break;
				#endif
				//
                default: jassertfalse; break;
            }
        }

        if (text.isNotEmpty())
        {
            g.setColour (isBlacklisted ? owner.cBad: columnId == nameCol ? owner.cNormal: owner.cSelected);
			//
			if (!isActive) g.setColour(owner.cUnActive);
			if (isUnscanned)
			{
				g.setColour(Colours::red.withAlpha(0.62f));
				if (!isActive) g.setColour(Colours::red.withAlpha(0.22f));
			}
			//
			g.setFont (Font (height * 0.7f, Font::bold));
            g.drawFittedText (text, 4, 0, width - 6, height, Justification::centredLeft, 1, 0.9f);
        }
    }

    void deleteKeyPressed (int) override
    {
		if (ConfirmBox("Remove Plugin From List\n(this does not delete the actual file)", "Are you sure?")) owner.removeSelectedPlugins();
    }

    void sortOrderChanged (int newSortColumnId, bool isForwards) override
    {
        switch (newSortColumnId)
        {
            case nameCol:         list.sort (WKnownPluginList::sortAlphabetically, isForwards); break;
            case typeCol:         list.sort (WKnownPluginList::sortByFormat, isForwards); break;
            case manufacturerCol: list.sort (WKnownPluginList::sortByManufacturer, isForwards); break;
            case descCol:         break;

            default: jassertfalse; break;
        }
    }

    static String getPluginDescription (const PluginDescription& desc)
    {
        StringArray items;

        if (desc.descriptiveName != desc.name)
            items.add (desc.descriptiveName);

        items.add (desc.version);

        items.removeEmptyStrings();
        return items.joinIntoString (" - ");
    }

    WPluginListComponent& owner;
    WKnownPluginList& list;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TableModel)
};

// -------------------------------------------------------------------------------------------------------------------------------
WPluginListComponent::WPluginListComponent (AudioPluginFormatManager& manager, WKnownPluginList& listToEdit,
                                          const File& deadMansPedal, PropertiesFile* const props,
										  String _dataFolderLocale,
										  bool allowPluginsWhichRequireAsynchronousInstantiation, void* _effectOwner, bool _showShortNames, bool _addExtraPaths, bool _showTopOfList, bool _showBridge)
    : formatManager (manager),
      list (listToEdit),
      deadMansPedalFile (deadMansPedal),
      optionsButton ("Options..."),
      propertiesToUse (props),
      allowAsync (allowPluginsWhichRequireAsynchronousInstantiation),
      numThreads (allowAsync ? 1 : 0),
	  effectOwner(_effectOwner),
	  showShortNames(_showShortNames),
	  dataFolderLocale(_dataFolderLocale),
	  addExtraPaths(_addExtraPaths),
	  showTopOfList(_showTopOfList),
	  showBridge(_showBridge)
{
	#if !JUCE_WINDOWS
		showBridge = false;
	#endif
	//
	cNormal = Colours::black;
	cSelected = Colours::black.withAlpha(0.62f); 
	cBad = Colours::red;
	cRowBack = Colours::black.withAlpha(0.18f);
	cUnActive = Colours::black.withAlpha(0.12f);
	useBackColour = false;
	//
    tableModel = new TableModel (*this, listToEdit);

    TableHeaderComponent& header = table.getHeader();

    header.addColumn (TRANS("Name"),           TableModel::nameCol,         200, 100, 700, TableHeaderComponent::defaultFlags | TableHeaderComponent::sortedForwards);

	header.addColumn(TRANS("Custom"),		   TableModel::customCol1, 120, 100, 700, TableHeaderComponent::notSortable);

	#if WUSIK_P2000 | WUSIK_SQ200 | WUSIK_ENGINE
		header.addColumn(TRANS(""), TableModel::nameCol, 0, 0, 0, TableHeaderComponent::defaultFlags | TableHeaderComponent::sortedForwards);
	#else
		if (showBridge) header.addColumn(TRANS("SubFolder(s)"), TableModel::extraFolderCol1, 200, 100, 700, TableHeaderComponent::defaultFlags | TableHeaderComponent::sortedForwards);
		else header.addColumn(TRANS(""), TableModel::nameCol, 0, 0, 0, TableHeaderComponent::defaultFlags | TableHeaderComponent::sortedForwards);
	#endif

	//
	if (showShortNames) header.addColumn(TRANS("Short"), TableModel::customCol2, 100, 100, 700, TableHeaderComponent::notSortable);
		else header.addColumn(TRANS(""), TableModel::customCol2, 0, 0, 0, TableHeaderComponent::notSortable);
	//
	if (showBridge) header.addColumn(TRANS("Bridged"), TableModel::bridgeCol, 72, 40, 120, TableHeaderComponent::notSortable);
		else header.addColumn(TRANS(""), TableModel::bridgeCol, 0, 0, 0, TableHeaderComponent::notSortable);
    //
	header.addColumn (TRANS("Format"),         TableModel::typeCol,         50, 80, 80,    TableHeaderComponent::notResizable);
	#if JUCE_WINDOWS & JUCE_64BIT
		header.addColumn(TRANS("Bits"),			TableModel::bitsCol, 40, 80, 80, TableHeaderComponent::notResizable);
	#endif
    header.addColumn (TRANS("Type"),     TableModel::categoryCol,     120, 100, 200, TableHeaderComponent::notSortable);
    header.addColumn (TRANS("Manufacturer"),   TableModel::manufacturerCol, 180, 100, 300);
	header.addColumn(TRANS("Programs"), TableModel::numPrograms, 70, 20, 400);
	header.addColumn(TRANS("Parameters"), TableModel::numParameters, 70, 20, 400);

	#if WUSIK_X42
		header.addColumn(TRANS("IO"), TableModel::IOCol, 50, 20, 400);
		header.addColumn(TRANS("Layouts"), TableModel::channelLayoutsCol, 120, 20, 400);
	#endif

	header.addColumn(TRANS("MIDI"), TableModel::midiIO, 50, 20, 400);
    header.addColumn (TRANS(""),    TableModel::descCol,         300, 100, 500, TableHeaderComponent::notSortable);

    table.setHeaderHeight (22);
    table.setRowHeight (20);
    table.setModel (tableModel);
    table.setMultipleSelectionEnabled (true);
    addAndMakeVisible (table);

    addAndMakeVisible (optionsButton);
    optionsButton.addListener (this);
    optionsButton.setTriggeredOnMouseDown (true);

    setSize (400, 600);
    list.addChangeListener (this);
    updateList();
    table.getHeader().reSortTable();

	WPluginDirectoryScanner::applyBlacklistingsFromDeadMansPedal(list, deadMansPedalFile);
    deadMansPedalFile.deleteFile();
}

WPluginListComponent::~WPluginListComponent()
{
	propertiesToUse->saveIfNeeded();
	//
	pluginEditorWindow = nullptr;
	pluginEditor = nullptr;
	pluginLoad = nullptr;
	//
    list.removeChangeListener (this);
}

void WPluginListComponent::setOptionsButtonText (const String& newText)
{
    optionsButton.setButtonText (newText);
    resized();
}

void WPluginListComponent::setScanDialogText (const String& title, const String& content)
{
    dialogTitle = title;
    dialogText = content;
}

void WPluginListComponent::setNumberOfThreadsForScanning (int num)
{
    numThreads = num;
}

void WPluginListComponent::resized()
{
    Rectangle<int> r (getLocalBounds().reduced (2));

    optionsButton.setBounds (r.removeFromBottom (24));
    optionsButton.changeWidthToFitText (24);

    r.removeFromBottom (3);
    table.setBounds (r);
}

void WPluginListComponent::changeListenerCallback (ChangeBroadcaster*)
{
    table.getHeader().reSortTable();
    updateList();
}

void WPluginListComponent::updateList()
{
    table.updateContent();
    table.repaint();
}

void WPluginListComponent::removeSelectedPlugins()
{
    const SparseSet<int> selected (table.getSelectedRows());

    for (int i = table.getNumRows(); --i >= 0;)
        if (selected.contains (i))
            removePluginItem (i);

	SparseSet<int> noneSelected;
	table.setSelectedRows(noneSelected, NotificationType::dontSendNotification);
	table.repaint();
}

void WPluginListComponent::setTableModel (TableListBoxModel* model)
{
    table.setModel (nullptr);
    tableModel = model;
    table.setModel (tableModel);

    table.getHeader().reSortTable();
    table.updateContent();
    table.repaint();
}

bool WPluginListComponent::canShowSelectedFolder() const
{
	if (const PluginDescription* const desc = list.getType(table.getSelectedRow()))
        return File::createFileWithoutCheckingPath (desc->fileOrIdentifier).exists();

    return false;
}

void WPluginListComponent::showSelectedFolder()
{
    if (canShowSelectedFolder())
		if (const PluginDescription* const desc = list.getType(table.getSelectedRow()))
            File (desc->fileOrIdentifier).getParentDirectory().startAsProcess();
}

void WPluginListComponent::removeMissingPlugins()
{
	for (int i = list.getNumTypes(); --i >= 0;)
	{
		PluginDescription* d = list.getType(i);
		if (!formatManager.doesPluginStillExist(*d))
		{
			list.removeType(i);
		}
	}
	list.sendChangeMessage();
}

void WPluginListComponent::removePluginItem (int index)
{
    if (index < list.getNumTypes())
        list.removeType (index);
    else
        list.removeFromBlacklist (list.getBlacklistedFiles() [index - list.getNumTypes()]);
	//
	list.sendChangeMessage();
}

void WPluginListComponent::optionsMenuStaticCallback (int result, WPluginListComponent* pluginList)
{
    if (pluginList != nullptr)
        pluginList->optionsMenuCallback (result);
}

void WPluginListComponent::optionsMenuCallback (int result)
{
    switch (result)
    {
        case 0:   break;
        case 1:   
			list.clear(); 
			list.clearBlacklistedFiles();
			break;
        case 2:   removeSelectedPlugins(); break;
        case 3:   showSelectedFolder(); break;
        case 4:   removeMissingPlugins(); break;
		//
		case 28:
			{
				for (int i = 0; i < list.getNumTypes(); i++)
				{
					PluginDescription* const desc = list.getType(i);
					desc->dontBridge = !desc->is32Bits;
				}
				table.repaint();
				list.sendChangeMessage();
			}
			break;
		//
		case 6:
			{
				for (int i = 0; i < list.getNumTypes(); i++)
				{
					PluginDescription* const desc = list.getType(i);
					desc->active = true;
				}
				table.repaint();
				list.sendChangeMessage();
			}
			break;
		//
		case 7:
			{
				const SparseSet<int> selected(table.getSelectedRows());
				//
				for (int i = table.getNumRows(); --i >= 0;)
				{
					if (selected.contains(i) && i < list.getNumTypes())
					{
						PluginDescription* const desc = list.getType(i);
						desc->active = true;
					}
				}
				table.repaint();
				list.sendChangeMessage();
			}
			break;
		//
		//
		case 8:
			{
				for (int i = 0; i < list.getNumTypes(); i++)
				{
					PluginDescription* const desc = list.getType(i);
					desc->active = false;
				}
				table.repaint();
				list.sendChangeMessage();
			}
			break;
		//
		case 9:
			{
				const SparseSet<int> selected(table.getSelectedRows());
				//
				for (int i = table.getNumRows(); --i >= 0;)
				{
					if (selected.contains(i) && i < list.getNumTypes())
					{
						PluginDescription* const desc = list.getType(i);
						desc->active = false;
					}
				}
				table.repaint();
				list.sendChangeMessage();
			}
			break;
		//
		case 14:
			{
				propertiesToUse->setValue("UseExternalScanner", !propertiesToUse->getBoolValue("UseExternalScanner"));
				MessageBox("External Scanner", "Please, restart the application so changes are used.");
			}
			break;
		//
        default:
            if (AudioPluginFormat* format = formatManager.getFormat (result - 10))
                scanFor (*format);

            break;
    }
}

void WPluginListComponent::buttonClicked (Button* button)
{
    if (button == &optionsButton)
    {
        PopupMenu menu;
        menu.addItem (1, TRANS("Clear list"));
		menu.addSeparator();
		menu.addItem (6, TRANS("Show All"));
		menu.addItem (7, TRANS("Show Selected"));
		//
		if (showBridge)
		{
			menu.addItem(8, TRANS("Ignore All"));
			menu.addItem(9, TRANS("Ignore Selected"));
		}
		else
		{
			menu.addItem(8, TRANS("Hide All"));
			menu.addItem(9, TRANS("Hide Selected"));
		}
		menu.addSeparator();
        menu.addItem (2, TRANS("Remove selected plug-in from list"), table.getNumSelectedRows() > 0);
        menu.addItem (3, TRANS("Show folder containing selected plug-in"), canShowSelectedFolder());
        menu.addItem (4, TRANS("Remove any plug-ins whose files no longer exist"));
		menu.addItem(14, TRANS("Use External Scanner"), true, propertiesToUse->getBoolValue("UseExternalScanner"));
		#if JUCE_64BIT
			if (showBridge) menu.addItem(28, TRANS("Only Bridge 32 Bit Plugins"));
		#endif
        menu.addSeparator();

        for (int i = 0; i < formatManager.getNumFormats(); ++i)
        {
            AudioPluginFormat* const format = formatManager.getFormat (i);

            if (format->canScanForPlugins())
                menu.addItem (10 + i, "Scan for new or updated " + format->getName() + " plug-ins");
        }

        menu.showMenuAsync (PopupMenu::Options().withTargetComponent (&optionsButton),
                            ModalCallbackFunction::forComponent (optionsMenuStaticCallback, this));
    }
}

FileSearchPath WPluginListComponent::getLastSearchPath (PropertiesFile& properties, AudioPluginFormat& format)
{
    return FileSearchPath (properties.getValue ("lastPluginScanPath_" + format.getName(),
                                                format.getDefaultLocationsToSearch().toString()));
}

void WPluginListComponent::setLastSearchPath (PropertiesFile& properties, AudioPluginFormat& format,
                                             const FileSearchPath& newPath)
{
    properties.setValue ("lastPluginScanPath_" + format.getName(), newPath.toString());
}

// -------------------------------------------------------------------------------------------------------------------------------
class WPluginListComponent::Scanner    : private Timer
{
public:
    Scanner (WPluginListComponent& plc, AudioPluginFormat& format, PropertiesFile* properties,
             bool allowPluginsWhichRequireAsynchronousInstantiation, int threads,
			 const String& title, const String& text, void* _effectOwner, String _dataFolderLocale)
        : owner (plc), formatToScan (format), propertiesToUse (properties), dataFolderLocale(_dataFolderLocale),
		#if JUCE_WINDOWS
			pathChooserWindow (TRANS("Select folders to scan...\n(some plugins requires administrator rights)"), String(), AlertWindow::NoIcon),
		#else
			pathChooserWindow(TRANS("Select folders to scan..."), String(), AlertWindow::NoIcon),
		#endif
          progressWindow (title, text, AlertWindow::NoIcon),
          progress (0.0), numThreads (threads), allowAsync (allowPluginsWhichRequireAsynchronousInstantiation),
		  finished(false), progressCounter(0), effectOwner(_effectOwner)
    {
        FileSearchPath path (formatToScan.getDefaultLocationsToSearch());

        // You need to use at least one thread when scanning plug-ins asynchronously
        jassert (! allowAsync || (numThreads > 0));
		//
		#if WUSIK_X42
			#if JUCE_WINDOWS
				#if JUCE_64BIT
					File sheelFile = File(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerX.exe");
				#else
					File sheelFile(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerX32.exe");
				#endif
			#else
				File sheelFile(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerX");
			#endif
		#else
			#if JUCE_WINDOWS
				#if JUCE_64BIT
					File sheelFile = File(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerD.exe");
				#else
					File sheelFile(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerD32.exe");
				#endif
			#else
				File sheelFile(dataFolderLocale + "Scanner" + slash + "WusikPluginScannerD");
			#endif
		#endif
		//
		if (!sheelFile.existsAsFile()) MessageBox("Error: the following file is missing and is required by the scanner process!", sheelFile.getFullPathName());
		//
        if (path.getNumPaths() > 0) // if the path is empty, then paths aren't used for this format.
        {
           #if ! JUCE_IOS
            if (propertiesToUse != nullptr)
                path = getLastSearchPath (*propertiesToUse, formatToScan);
           #endif

            pathList.setSize (500, 300);
            pathList.setPath (path);

            pathChooserWindow.addCustomComponent (&pathList);
            pathChooserWindow.addButton (TRANS("Scan"),   1, KeyPress (KeyPress::returnKey));
            pathChooserWindow.addButton (TRANS("Cancel"), 0, KeyPress (KeyPress::escapeKey));

            pathChooserWindow.enterModalState (true,
                                               ModalCallbackFunction::forComponent (startScanCallback,
                                                                                    &pathChooserWindow, this),
                                               false);
        }
        else
        {
            startScan();
        }
    }

    ~Scanner()
    {
        if (pool != nullptr)
        {
            pool->removeAllJobs (true, 60000);
            pool = nullptr;
        }
    }

    WPluginListComponent& owner;
    AudioPluginFormat& formatToScan;
    PropertiesFile* propertiesToUse;
    ScopedPointer<WPluginDirectoryScanner> scanner;
    AlertWindow pathChooserWindow, progressWindow;
    FileSearchPathListComponent pathList;
    String pluginBeingScanned;
    double progress;
    int numThreads;
    bool allowAsync, finished;
    ScopedPointer<ThreadPool> pool;
	int progressCounter;
	void* effectOwner;
	String dataFolderLocale;

    static void startScanCallback (int result, AlertWindow* alert, Scanner* scanner)
    {
        if (alert != nullptr && scanner != nullptr)
        {
            if (result != 0)
                scanner->warnUserAboutStupidPaths();
            else
                scanner->finishedScan();
        }
    }

    // Try to dissuade people from to scanning their entire C: drive, or other system folders.
    void warnUserAboutStupidPaths()
    {
        for (int i = 0; i < pathList.getPath().getNumPaths(); ++i)
        {
            const File f (pathList.getPath()[i]);

            if (isStupidPath (f))
            {
                AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
                                              TRANS("Plugin Scanning"),
                                              TRANS("If you choose to scan folders that contain non-plugin files, "
                                                    "then scanning may take a long time, and can cause crashes when "
                                                    "attempting to load unsuitable files.")
                                                + newLine
                                                + TRANS ("Are you sure you want to scan the folder \"XYZ\"?")
                                                   .replace ("XYZ", f.getFullPathName()),
                                              TRANS ("Scan"),
                                              String(),
                                              nullptr,
                                              ModalCallbackFunction::create (warnAboutStupidPathsCallback, this));
                return;
            }
        }

        startScan();
    }

    static bool isStupidPath (const File& f)
    {
        Array<File> roots;
        File::findFileSystemRoots (roots);

        if (roots.contains (f))
            return true;

        File::SpecialLocationType pathsThatWouldBeStupidToScan[]
            = { File::globalApplicationsDirectory,
                File::userHomeDirectory,
                File::userDocumentsDirectory,
                File::userDesktopDirectory,
                File::tempDirectory,
                File::userMusicDirectory,
                File::userMoviesDirectory,
                File::userPicturesDirectory };

        for (int i = 0; i < numElementsInArray (pathsThatWouldBeStupidToScan); ++i)
        {
            const File sillyFolder (File::getSpecialLocation (pathsThatWouldBeStupidToScan[i]));

            if (f == sillyFolder || sillyFolder.isAChildOf (f))
                return true;
        }

        return false;
    }

    static void warnAboutStupidPathsCallback (int result, Scanner* scanner)
    {
        if (result != 0)
            scanner->startScan();
        else
            scanner->finishedScan();
    }

    void startScan()
    {
        pathChooserWindow.setVisible (false);
		FileSearchPath & searchPath = (FileSearchPath&)pathList.getPath();
		//
		if (owner.addExtraPaths)
		{
			#if JUCE_WINDOWS & JUCE_64BIT
				File dataFolderPlugins(dataFolderLocale + "VST Plugins 64");
			#elif JUCE_WINDOWS & JUCE_32BIT
				File dataFolderPlugins(dataFolderLocale + "VST Plugins 32");
			#else
				File dataFolderPlugins(dataFolderLocale + "VST Plugins");
			#endif
			//
			searchPath.add(dataFolderPlugins);
		}
		//
		scanner = new WPluginDirectoryScanner (owner.list, formatToScan, pathList.getPath(),
                                              true, owner.deadMansPedalFile, allowAsync);
		//
		// Remove the last Data (internal extra one) so it doesn't save //
		if (owner.addExtraPaths) searchPath.remove(searchPath.getNumPaths() - 1);

        if (propertiesToUse != nullptr)
        {
            setLastSearchPath (*propertiesToUse, formatToScan, pathList.getPath());
            propertiesToUse->saveIfNeeded();
        }

        progressWindow.addButton (TRANS("Cancel"), 0, KeyPress (KeyPress::escapeKey));
        progressWindow.addProgressBarComponent (progress);
        progressWindow.enterModalState();

        if (numThreads > 0)
        {
            pool = new ThreadPool (numThreads);

            for (int i = numThreads; --i >= 0;)
                pool->addJob (new ScanJob (*this), true);
        }

        startTimer (60);
    }

    void finishedScan()
    {
        owner.scanFinished (scanner != nullptr ? scanner->getFailedFiles()
                                               : StringArray());
    }

    void timerCallback() override
    {
        if (pool == nullptr)
        {
            if (doNextScan())
                startTimer (60);
        }

        if (! progressWindow.isCurrentlyModal())
            finished = true;

		String xExtra = "Processing";
		progressCounter++;
		if (progressCounter > 10) xExtra = ".Processing.";
		else if (progressCounter > 8) xExtra = "..Processing..";
		else if (progressCounter > 6) xExtra = "...Processing...";
		else if (progressCounter > 2) xExtra = "....Processing....";
		if (progressCounter > 14) progressCounter = 0;
		//
        if (finished)
            finishedScan();
        else
			progressWindow.setMessage(xExtra + "\n\n" + pluginBeingScanned);
    }

    bool doNextScan()
    {
        if (scanner->scanNextFile (true, pluginBeingScanned, *propertiesToUse))
        {
            progress = scanner->getProgress();
            return true;
        }

        finished = true;
        return false;
    }

    struct ScanJob  : public ThreadPoolJob
    {
        ScanJob (Scanner& s)  : ThreadPoolJob ("pluginscan"), scanner (s) {}

        JobStatus runJob()
        {
            while (scanner.doNextScan() && ! shouldExit())
            {}

            return jobHasFinished;
        }

        Scanner& scanner;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScanJob)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Scanner)
};

void WPluginListComponent::scanFor (AudioPluginFormat& format)
{
    currentScanner = new Scanner (*this, format, propertiesToUse, allowAsync, numThreads,
                                  dialogTitle.isNotEmpty() ? dialogTitle : TRANS("Scanning for plug-ins..."),
                                  dialogText.isNotEmpty()  ? dialogText  : TRANS("Searching for all possible plug-in files..."), effectOwner, dataFolderLocale);
}

bool WPluginListComponent::isScanning() const noexcept
{
    return currentScanner != nullptr;
}

void WPluginListComponent::scanFinished (const StringArray& failedFiles)
{
    StringArray shortNames;

    for (int i = 0; i < failedFiles.size(); ++i)
        shortNames.add (File::createFileWithoutCheckingPath (failedFiles[i]).getFileName());

    currentScanner = nullptr; // mustn't delete this before using the failed files array

    if (shortNames.size() > 0)
        AlertWindow::showMessageBoxAsync (AlertWindow::InfoIcon,
                                          TRANS("Scan complete"),
                                          TRANS("Note that the following files appeared to be plugin files, but failed to load correctly")
                                            + ":\n\n"
                                            + shortNames.joinIntoString (", "));
}

// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------------------------------
WKnownPluginList::WKnownPluginList() { }
WKnownPluginList::~WKnownPluginList() { }

void WKnownPluginList::clear()
{
    ScopedLock lock (typesArrayLock);

    if (types.size() > 0)
    {
        types.clear();
        sendChangeMessage();
    }
}

PluginDescription* WKnownPluginList::getTypeForFile (const String& fileOrIdentifier) const
{
    ScopedLock lock (typesArrayLock);

    for (int i = 0; i < types.size(); ++i)
        if (types.getUnchecked(i)->fileOrIdentifier == fileOrIdentifier)
            return types.getUnchecked(i);

    return nullptr;
}

PluginDescription* WKnownPluginList::getTypeForIdentifierString (const String& identifierString) const
{
    ScopedLock lock (typesArrayLock);

    for (int i = 0; i < types.size(); ++i)
        if (types.getUnchecked(i)->matchesIdentifierString (identifierString))
            return types.getUnchecked(i);

    return nullptr;
}

bool WKnownPluginList::addType (const PluginDescription& type)
{
    {
        ScopedLock lock (typesArrayLock);

        for (int i = types.size(); --i >= 0;)
        {
            if (types.getUnchecked(i)->isDuplicateOf (type))
            {
                jassert (types.getUnchecked(i)->name == type.name);
                jassert (types.getUnchecked(i)->isInstrument == type.isInstrument);
                *types.getUnchecked(i) = type;
                return false;
            }
        }

		types.insert(0, new PluginDescription(type));
    }

    sendChangeMessage();
    return true;
}

void WKnownPluginList::removeType (const int index)
{
    {
        ScopedLock lock (typesArrayLock);

        types.remove (index);
    }

    sendChangeMessage();
}

bool WKnownPluginList::isListingUpToDate (const String& fileOrIdentifier,
                                         AudioPluginFormat& formatToUse) const
{
    if (getTypeForFile (fileOrIdentifier) == nullptr)
        return false;

    ScopedLock lock (typesArrayLock);

    for (int i = types.size(); --i >= 0;)
    {
		const PluginDescription* const d = types.getUnchecked(i);

        if (d->fileOrIdentifier == fileOrIdentifier)
             //&& formatToUse.pluginNeedsRescanning (descOld))
            return false;
    }

    return true;
}

void WKnownPluginList::setCustomScanner (CustomScanner* newScanner)
{
    scanner = newScanner;
}

bool WKnownPluginList::scanAndAddFile (const String& fileOrIdentifier,
                                      const bool dontRescanIfAlreadyInList,
									  OwnedArray<PluginDescription>& typesFound,
									  AudioPluginFormat& format, String& pluginBeingScanned, PropertiesFile& propertiesToUse)
{
    const ScopedLock sl (scanLock);

    if (dontRescanIfAlreadyInList
         && getTypeForFile (fileOrIdentifier) != nullptr)
    {
        bool needsRescanning = false;

        ScopedLock lock (typesArrayLock);

        for (int i = types.size(); --i >= 0;)
        {
			const PluginDescription* const d = types.getUnchecked(i);

            if (d->fileOrIdentifier == fileOrIdentifier && d->pluginFormatName == format.getName())
            {
                //if (format.pluginNeedsRescanning (descOld))
                 //   needsRescanning = true;
                //else
					typesFound.add(new PluginDescription(*d));
            }
        }

        if (! needsRescanning)
            return false;
    }

    if (blacklist.contains (fileOrIdentifier))
        return false;

	int xsize = 0;
    {
        const ScopedUnlock sl2 (scanLock);
		//
        if (scanner != nullptr)
        {
			OwnedArray<PluginDescription> found;
			//
			if (!scanner->findPluginTypesFor(format, found, fileOrIdentifier, pluginBeingScanned, propertiesToUse))
                addToBlacklist (fileOrIdentifier);
			//
			for (int i = 0; i < found.size(); ++i)
			{
				PluginDescription* const desc = found.getUnchecked(i);
				jassert(desc != nullptr);

				addType(*desc);
				typesFound.add(new PluginDescription(*desc));
			}
        }
        else
        {
			OwnedArray<PluginDescription> found;
            format.findAllTypesForFile (found, fileOrIdentifier);
			//
			for (int i = 0; i < found.size(); ++i)
			{
				PluginDescription* const desc = found.getUnchecked(i);
				jassert(desc != nullptr);
				//
				addType(*desc);
				typesFound.add(new PluginDescription(*desc));
			}
        }
    }

	return xsize > 0; // found.size() > 0;
}

void WKnownPluginList::scanAndAddDragAndDroppedFiles (AudioPluginFormatManager& formatManager,
                                                     const StringArray& files,
													 OwnedArray<PluginDescription>& typesFound,
													 PropertiesFile& propertiesToUse)
{
    for (int i = 0; i < files.size(); ++i)
    {
        const String filenameOrID (files[i]);
        bool found = false;

        for (int j = 0; j < formatManager.getNumFormats(); ++j)
        {
            AudioPluginFormat* const format = formatManager.getFormat (j);
			String pluginName = filenameOrID;
            if (format->fileMightContainThisPluginType (filenameOrID)
                 && scanAndAddFile (filenameOrID, true, typesFound, *format, pluginName, propertiesToUse))
            {
                found = true;
                break;
            }
        }

        if (! found)
        {
            const File f (filenameOrID);

            if (f.isDirectory())
            {
                StringArray s;

                {
                    Array<File> subFiles;
                    f.findChildFiles (subFiles, File::findFilesAndDirectories, false);

                    for (int j = 0; j < subFiles.size(); ++j)
                        s.add (subFiles.getReference(j).getFullPathName());
                }

                scanAndAddDragAndDroppedFiles (formatManager, s, typesFound, propertiesToUse);
            }
        }
    }

    scanFinished();
}

void WKnownPluginList::scanFinished()
{
    if (scanner != nullptr)
        scanner->scanFinished();
}

const StringArray& WKnownPluginList::getBlacklistedFiles() const
{
    return blacklist;
}

void WKnownPluginList::addToBlacklist (const String& pluginID)
{
    if (! blacklist.contains (pluginID))
    {
        blacklist.add (pluginID);
        sendChangeMessage();
    }
}

void WKnownPluginList::removeFromBlacklist (const String& pluginID)
{
    const int index = blacklist.indexOf (pluginID);

    if (index >= 0)
    {
        blacklist.remove (index);
        sendChangeMessage();
    }
}

void WKnownPluginList::clearBlacklistedFiles()
{
    if (blacklist.size() > 0)
    {
        blacklist.clear();
        sendChangeMessage();
    }
}

// -------------------------------------------------------------------------------------------------------------------------------
struct PluginSorter
{
    PluginSorter (WKnownPluginList::SortMethod sortMethod, bool forwards) noexcept
        : method (sortMethod), direction (forwards ? 1 : -1) {}

	int compareElements(const PluginDescription* const first,
		const PluginDescription* const second) const
    {
        int diff = 0;

        switch (method)
        {
            case WKnownPluginList::sortByManufacturer:       diff = first->manufacturerName.compareNatural (second->manufacturerName); break;
            case WKnownPluginList::sortByFormat:             diff = first->pluginFormatName.compare (second->pluginFormatName); break;
            case WKnownPluginList::sortByFileSystemLocation: diff = lastPathPart (first->fileOrIdentifier).compare (lastPathPart (second->fileOrIdentifier)); break;
            case WKnownPluginList::sortByInfoUpdateTime:     diff = compare (first->lastInfoUpdateTime, second->lastInfoUpdateTime); break;
            default: break;
        }

        if (diff == 0)
            diff = first->name.compareNatural (second->name);

        return diff * direction;
    }

private:
    static String lastPathPart (const String& path)
    {
        return path.replaceCharacter ('\\', '/').upToLastOccurrenceOf ("/", false, false);
    }

    static int compare (Time a, Time b) noexcept
    {
        if (a < b)   return -1;
        if (b < a)   return 1;

        return 0;
    }

    const WKnownPluginList::SortMethod method;
    const int direction;

    JUCE_DECLARE_NON_COPYABLE (PluginSorter)
};

void WKnownPluginList::sort (const SortMethod method, bool forwards)
{
    if (method != defaultOrder)
    {
		Array<PluginDescription*> oldOrder, newOrder;

        {
            ScopedLock lock (typesArrayLock);

            oldOrder.addArray (types);

            PluginSorter sorter (method, forwards);
            types.sort (sorter, true);

            newOrder.addArray (types);
        }

        if (oldOrder != newOrder)
            sendChangeMessage();
    }
}

// -------------------------------------------------------------------------------------------------------------------------------
XmlElement* WKnownPluginList::createXml() const
{
    XmlElement* const e = new XmlElement ("KNOWNPLUGINS");

    {
        ScopedLock lock (typesArrayLock);

        for (int i = types.size(); --i >= 0;)
            e->prependChildElement (types.getUnchecked(i)->createXml());
    }

    for (int i = 0; i < blacklist.size(); ++i)
        e->createNewChildElement ("BLACKLISTED")->setAttribute ("id", blacklist[i]);

    return e;
}

void WKnownPluginList::recreateFromXml (const XmlElement& xml)
{
    clear();
    clearBlacklistedFiles();

    if (xml.hasTagName ("KNOWNPLUGINS"))
    {
        forEachXmlChildElement (xml, e)
        {
			PluginDescription info;

            if (e->hasTagName ("BLACKLISTED"))
                blacklist.add (e->getStringAttribute ("id"));
            else if (info.loadFromXml (*e))
                addType (info);
        }
    }
}

// -------------------------------------------------------------------------------------------------------------------------------
struct PluginTreeUtils
{
    enum { menuIdBase = 0x324503f4 };

	static void buildTreeByFolder(WKnownPluginList::PluginTree& tree, const Array<PluginDescription*>& allPlugins)
    {
        for (int i = 0; i < allPlugins.size(); ++i)
        {
			PluginDescription* const pd = allPlugins.getUnchecked(i);

            String path (pd->fileOrIdentifier.replaceCharacter ('\\', '/')
                                             .upToLastOccurrenceOf ("/", false, false));

            if (path.substring (1, 2) == ":")
                path = path.substring (2);

            addPlugin (tree, pd, path);
        }

        optimiseFolders (tree, false);
    }

    static void optimiseFolders (WKnownPluginList::PluginTree& tree, bool concatenateName)
    {
        for (int i = tree.subFolders.size(); --i >= 0;)
        {
            WKnownPluginList::PluginTree& sub = *tree.subFolders.getUnchecked(i);
            optimiseFolders (sub, concatenateName || (tree.subFolders.size() > 1));

            if (sub.plugins.size() == 0)
            {
                for (int j = 0; j < sub.subFolders.size(); ++j)
                {
                    WKnownPluginList::PluginTree* const s = sub.subFolders.getUnchecked(j);

                    if (concatenateName)
                        s->folder = sub.folder + "/" + s->folder;

                    tree.subFolders.add (s);
                }

                sub.subFolders.clear (false);
                tree.subFolders.remove (i);
            }
        }
    }

    static void buildTreeByCategory (WKnownPluginList::PluginTree& tree,
		const Array<PluginDescription*>& sorted,
                                     const WKnownPluginList::SortMethod sortMethod)
    {
        String lastType;
        ScopedPointer<WKnownPluginList::PluginTree> current (new WKnownPluginList::PluginTree());

        for (int i = 0; i < sorted.size(); ++i)
        {
			const PluginDescription* const pd = sorted.getUnchecked(i);
            String thisType (sortMethod == WKnownPluginList::sortByCategory ? pd->category
                                                                           : pd->manufacturerName);

            if (! thisType.containsNonWhitespaceChars())
                thisType = "Other";

            if (thisType != lastType)
            {
                if (current->plugins.size() + current->subFolders.size() > 0)
                {
                    current->folder = lastType;
                    tree.subFolders.add (current.release());
                    current = new WKnownPluginList::PluginTree();
                }

                lastType = thisType;
            }

            current->plugins.add (pd);
        }

        if (current->plugins.size() + current->subFolders.size() > 0)
        {
            current->folder = lastType;
            tree.subFolders.add (current.release());
        }
    }

	static void addPlugin(WKnownPluginList::PluginTree& tree, PluginDescription* const pd, String path)
    {
        if (path.isEmpty())
        {
            tree.plugins.add (pd);
        }
        else
        {
           #if JUCE_MAC
            if (path.containsChar (':'))
                path = path.fromFirstOccurrenceOf (":", false, false); // avoid the special AU formatting nonsense on Mac..
           #endif

            const String firstSubFolder (path.upToFirstOccurrenceOf ("/", false, false));
            const String remainingPath  (path.fromFirstOccurrenceOf ("/", false, false));

            for (int i = tree.subFolders.size(); --i >= 0;)
            {
                WKnownPluginList::PluginTree& subFolder = *tree.subFolders.getUnchecked(i);

                if (subFolder.folder.equalsIgnoreCase (firstSubFolder))
                {
                    addPlugin (subFolder, pd, remainingPath);
                    return;
                }
            }

            WKnownPluginList::PluginTree* const newFolder = new WKnownPluginList::PluginTree();
            newFolder->folder = firstSubFolder;
            tree.subFolders.add (newFolder);
            addPlugin (*newFolder, pd, remainingPath);
        }
    }

	static bool containsDuplicateNames(const Array<const PluginDescription*>& plugins, const String& name)
    {
        int matches = 0;

        for (int i = 0; i < plugins.size(); ++i)
            if (plugins.getUnchecked(i)->name == name)
                if (++matches > 1)
                    return true;

        return false;
    }

    static bool addToMenu (const WKnownPluginList::PluginTree& tree, PopupMenu& m,
		const OwnedArray<PluginDescription>& allPlugins,
                           const String& currentlyTickedPluginID)
    {
        bool isTicked = false;

        for (int i = 0; i < tree.subFolders.size(); ++i)
        {
            const WKnownPluginList::PluginTree& sub = *tree.subFolders.getUnchecked(i);

            PopupMenu subMenu;
            const bool isItemTicked = addToMenu (sub, subMenu, allPlugins, currentlyTickedPluginID);
            isTicked = isTicked || isItemTicked;

            m.addSubMenu (sub.folder, subMenu, true, nullptr, isItemTicked, 0);
        }

        for (int i = 0; i < tree.plugins.size(); ++i)
        {
			const PluginDescription* const plugin = tree.plugins.getUnchecked(i);

            String name (plugin->name);

            if (containsDuplicateNames (tree.plugins, name))
                name << " (" << plugin->pluginFormatName << ')';

            const bool isItemTicked = plugin->matchesIdentifierString (currentlyTickedPluginID);
            isTicked = isTicked || isItemTicked;

            m.addItem (allPlugins.indexOf (plugin) + menuIdBase, name, true, isItemTicked);
        }

        return isTicked;
    }
};

WKnownPluginList::PluginTree* WKnownPluginList::createTree (const SortMethod sortMethod) const
{
	Array<PluginDescription*> sorted;

    {
        ScopedLock lock (typesArrayLock);
        PluginSorter sorter (sortMethod, true);

        for (int i = 0; i < types.size(); ++i)
            sorted.addSorted (sorter, types.getUnchecked(i));
    }

    PluginTree* tree = new PluginTree();

    if (sortMethod == sortByCategory || sortMethod == sortByManufacturer || sortMethod == sortByFormat)
    {
        PluginTreeUtils::buildTreeByCategory (*tree, sorted, sortMethod);
    }
    else if (sortMethod == sortByFileSystemLocation)
    {
        PluginTreeUtils::buildTreeByFolder (*tree, sorted);
    }
    else
    {
        for (int i = 0; i < sorted.size(); ++i)
            tree->plugins.add (sorted.getUnchecked(i));
    }

    return tree;
}

// -------------------------------------------------------------------------------------------------------------------------------
void WKnownPluginList::addToMenu (PopupMenu& menu, const SortMethod sortMethod,
                                 const String& currentlyTickedPluginID) const
{
    ScopedPointer<PluginTree> tree (createTree (sortMethod));
    PluginTreeUtils::addToMenu (*tree, menu, types, currentlyTickedPluginID);
}

int WKnownPluginList::getIndexChosenByMenu (const int menuResultCode) const
{
    const int i = menuResultCode - PluginTreeUtils::menuIdBase;
    return isPositiveAndBelow (i, types.size()) ? i : -1;
}

// -------------------------------------------------------------------------------------------------------------------------------
WKnownPluginList::CustomScanner::CustomScanner() {}
WKnownPluginList::CustomScanner::~CustomScanner() {}

void WKnownPluginList::CustomScanner::scanFinished() {}

bool WKnownPluginList::CustomScanner::shouldExit() const noexcept
{
    if (ThreadPoolJob* job = ThreadPoolJob::getCurrentThreadPoolJob())
        return job->shouldExit();

    return false;
}

WPluginDirectoryScanner::WPluginDirectoryScanner (WKnownPluginList& listToAddTo,
                                                AudioPluginFormat& formatToLookFor,
                                                FileSearchPath directoriesToSearch,
                                                const bool recursive,
                                                const File& deadMansPedal,
                                                bool allowPluginsWhichRequireAsynchronousInstantiation)
    : list (listToAddTo),
      format (formatToLookFor),
      deadMansPedalFile (deadMansPedal),
      progress (0),
      allowAsync (allowPluginsWhichRequireAsynchronousInstantiation)
{
    directoriesToSearch.removeRedundantPaths();

    filesOrIdentifiersToScan = format.searchPathsForPlugins (directoriesToSearch, recursive, allowAsync);

    // If any plugins have crashed recently when being loaded, move them to the
    // end of the list to give the others a chance to load correctly..
    const StringArray crashedPlugins (WreadDeadMansPedalFile (deadMansPedalFile));

    for (int i = 0; i < crashedPlugins.size(); ++i)
    {
        const String f = crashedPlugins[i];

        for (int j = filesOrIdentifiersToScan.size(); --j >= 0;)
            if (f == filesOrIdentifiersToScan[j])
                filesOrIdentifiersToScan.move (j, -1);
    }

    applyBlacklistingsFromDeadMansPedal (listToAddTo, deadMansPedalFile);
    nextIndex.set (filesOrIdentifiersToScan.size());
}

WPluginDirectoryScanner::~WPluginDirectoryScanner()
{
    list.scanFinished();
}

// -------------------------------------------------------------------------------------------------------------------------------
String WPluginDirectoryScanner::getNextPluginFileThatWillBeScanned() const
{
    return format.getNameOfPluginFromIdentifier (filesOrIdentifiersToScan [nextIndex.get() - 1]);
}

void WPluginDirectoryScanner::updateProgress()
{
    progress = (1.0f - nextIndex.get() / (float) filesOrIdentifiersToScan.size());
}

bool WPluginDirectoryScanner::scanNextFile (const bool dontRescanIfAlreadyInList,
	String& nameOfPluginBeingScanned, PropertiesFile& propertiesToUse)
{
    const int index = --nextIndex;

    if (index >= 0)
    {
        const String file (filesOrIdentifiersToScan [index]);

        if (file.isNotEmpty() && ! list.isListingUpToDate (file, format))
        {
			#if JUCE_WINDOWS
				nameOfPluginBeingScanned = File(format.getNameOfPluginFromIdentifier(file)).getFileNameWithoutExtension();
			#else
				nameOfPluginBeingScanned = format.getNameOfPluginFromIdentifier (file);
			#endif

            OwnedArray <PluginDescription> typesFound;

            // Add this plugin to the end of the dead-man's pedal list in case it crashes...
            StringArray crashedPlugins (WreadDeadMansPedalFile (deadMansPedalFile));
            crashedPlugins.removeString (file);
            crashedPlugins.add (file);
            setDeadMansPedalFile (crashedPlugins);

            list.scanAndAddFile (file, dontRescanIfAlreadyInList, typesFound, format, nameOfPluginBeingScanned, propertiesToUse);

            // Managed to load without crashing, so remove it from the dead-man's-pedal..
            crashedPlugins.removeString (file);
            setDeadMansPedalFile (crashedPlugins);

            if (typesFound.size() == 0 && ! list.getBlacklistedFiles().contains (file))
                failedFiles.add (file);
        }
    }

    updateProgress();
    return index > 0;
}

bool WPluginDirectoryScanner::skipNextFile()
{
    updateProgress();
    return --nextIndex > 0;
}

void WPluginDirectoryScanner::setDeadMansPedalFile (const StringArray& newContents)
{
    if (deadMansPedalFile.getFullPathName().isNotEmpty())
        deadMansPedalFile.replaceWithText (newContents.joinIntoString ("\n"), true, true);
}

void WPluginDirectoryScanner::applyBlacklistingsFromDeadMansPedal (WKnownPluginList& list, const File& file)
{
    // If any plugins have crashed recently when being loaded, move them to the
    // end of the list to give the others a chance to load correctly..
    const StringArray crashedPlugins (WreadDeadMansPedalFile (file));

    for (int i = 0; i < crashedPlugins.size(); ++i)
        list.addToBlacklist (crashedPlugins[i]);
}

#endif