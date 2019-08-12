// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

	Created by William Kalfelz @ Wusik Dot Com (c) 2019

*/

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "StandaloneFilterWindow.h"

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
WusikZrAudioProcessorEditor::WusikZrAudioProcessorEditor (WusikZrAudioProcessor& p) : AudioProcessorEditor (&p), processor (p), 
	updateInterface(false), firstStaticLayer(nullptr), firstSequencedLayer(nullptr), interfaceCleared(false), 
	clearInterface(false), firstInterface(true), aboutBox(nullptr), checkAudioInterface(false), showDM(true)
{
	DEBUG_TO_FILE("Plugin Editor Started");
	//
	layersViewPort[0] = nullptr;
	layersViewPort[1] = nullptr;
	layersViewPort[2] = nullptr;
	layersViewPort[3] = nullptr;
	//
	zoneComponents[0] = nullptr;
	zoneComponents[1] = nullptr;
	zoneComponents[2] = nullptr;
	zoneComponents[3] = nullptr;
	//
	if (processor.dataFiles->dataFilesError.isEmpty())
	{
		// Resizer 
		int xW = processor.dataFiles->background[0].getWidth();
		int xH = processor.dataFiles->background[0].getHeight() + processor.dataFiles->background[2].getHeight();
		//
		Rectangle<int> r = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
		resizeLimits.setSizeLimits(xW, xH , xW, r.getHeight());
		addAndMakeVisible(resizer = new ResizableCornerComponent(this, &resizeLimits));
		//
		wLookAndFeel = new WLookAndFeel;
		setLookAndFeel(wLookAndFeel);
		wLookAndFeel->scrollBarVReduce = processor.dataFiles->skinSettings->getIntValue("Scroll Bar Vertical Y Offset");
		//
		setSize(xW, processor.dataFiles->userSettings->getIntValue("GUI Height", 740));
		//
		if (processor.editingEnabled)
		{
			processor.dataFiles->skinSettingsOptions.doNotSave = false;
			processor.dataFiles->skinSettings = new PropertiesFile(File(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Skins" + slash + processor.dataFiles->selectedSkin + slash + "Skin Settings.xml"), processor.dataFiles->skinSettingsOptions);
		}
		//
		cachedImageButton = new ImageButton();
		cachedImageButton->setVisible(false);
		addChildComponent(cachedImageButton);
		//
		if (processor.ownerStandalone != nullptr && !processor.dataFiles->userSettings->getBoolValue("HasShownAudioInterface")) checkAudioInterface = true;
		toolTipWindow = new TooltipWindow(this);
		updateEntireInterface();
		startTimer(1200);
	}
	else
	{
		setSize(600, 600);
	}
	//
	DEBUG_TO_FILE("Plugin Editor Created");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
WusikZrAudioProcessorEditor::~WusikZrAudioProcessorEditor()
{
	DEBUG_TO_FILE("Plugin Editor Destroy Started");
	//
	if (processor.dataFiles->dataFilesError.isNotEmpty()) return;
	//
	if (processor.editingEnabled)
	{
		if (firstStaticLayer != nullptr)
		{
			for (int xc = 0; xc < firstStaticLayer->getNumChildComponents(); xc++)
			{
				if (firstStaticLayer->getChildComponent(xc)->getComponentID().compareIgnoreCase("WusikKnob") == 0)
					wusikKnobs[0].add((WusikKnob*)firstStaticLayer->getChildComponent(xc));
			}
		}
		//
		if (firstSequencedLayer != nullptr)
		{
			for (int xc = 0; xc < firstSequencedLayer->getNumChildComponents(); xc++)
			{
				if (firstSequencedLayer->getChildComponent(xc)->getComponentID().compareIgnoreCase("WusikKnob") == 0)
					wusikKnobs[0].add((WusikKnob*)firstSequencedLayer->getChildComponent(xc));
				//
				if (firstSequencedLayer->getChildComponent(xc)->getComponentID().compareIgnoreCase("WusikSteps") == 0)
				{
					for (int xcs = 0; xcs < firstSequencedLayer->getChildComponent(xc)->getNumChildComponents(); xcs++)
					{
						if (firstSequencedLayer->getChildComponent(xc)->getChildComponent(xcs)->getComponentID().compareIgnoreCase("WusikKnob") == 0)
							wusikKnobs[0].add((WusikKnob*)firstSequencedLayer->getChildComponent(xc)->getChildComponent(xcs));
					}
				}
			}
		}
		//
		ARRAY_Iterator(wusikKnobs[0])
		{
			if (wusikKnobs[0][index]->theParameter != nullptr)
			{
				if (wusikKnobs[0][index]->theImage != nullptr)
				{
					if (wusikKnobs[0][index]->isWaveform)
					{
						processor.dataFiles->skinSettings->setValue(wusikKnobs[0][index]->parSkinName,
							String(wusikKnobs[0][index]->getBounds().getX()) + "," +
							String(wusikKnobs[0][index]->getBounds().getY()) + "," +
							String(wusikKnobs[0][index]->getWidth()) + "," +
							String(wusikKnobs[0][index]->getHeight()) + "," +
							String(wusikKnobs[0][index]->theFont.getTypefaceName()) + "," +
							String(wusikKnobs[0][index]->theFont.getHeight()) + "," +
							String(wusikKnobs[0][index]->fontColour.toString()));
					}
					else
					{
						processor.dataFiles->skinSettings->setValue(wusikKnobs[0][index]->parSkinName,
							String(wusikKnobs[0][index]->getBounds().getX()) + "," +
							String(wusikKnobs[0][index]->getBounds().getY()) + "," +
							"0," +
							String(wusikKnobs[0][index]->frameHeight) + "," +
							String(wusikKnobs[0][index]->numberOfFrames));
					}
				}
				else
				{
					processor.dataFiles->skinSettings->setValue(wusikKnobs[0][index]->parSkinName,
						String(wusikKnobs[0][index]->getBounds().getX()) + "," +
						String(wusikKnobs[0][index]->getBounds().getY()) + "," +
						String(wusikKnobs[0][index]->getWidth()) + "," +
						String(wusikKnobs[0][index]->getHeight()) + "," +
						String(wusikKnobs[0][index]->theFont.getTypefaceName()) + "," +
						String(wusikKnobs[0][index]->theFont.getHeight()) + "," +
						String(wusikKnobs[0][index]->fontColour.toString()));
				}
			}
		}
		//
		processor.dataFiles->skinSettings->save();
	}
	//
	int xCounter = 0;
	while (processor.processingThread.isThreadRunning())
	{
		Thread::sleep(10);
		xCounter++;
		if (xCounter > 400) break;
	}
	//
	toolTipWindow = nullptr;
	closeTheInterface();
	setLookAndFeel(nullptr);
	wLookAndFeel = nullptr;
	processor.waveformWindow = nullptr;
	//
	DEBUG_TO_FILE("Plugin Editor Destroy Finished");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessorEditor::timerCallback()
{
	DEBUG_TO_FILE("Editor Timer");
	//
	stopTimer();
	//
	if (processor.dataFiles->dataFilesError.isNotEmpty()) return;
	//
	if (checkAudioInterface)
	{
		if (processor.ownerStandalone != nullptr)
		{
			checkAudioInterface = false;
			processor.dataFiles->userSettings->setValue("HasShownAudioInterface", true);
			//
			((StandalonePluginHolder*)processor.ownerStandalone)->showAudioSettingsDialog("Select Audio/Midi Devices");
		}
	}
	//
	if (processor.dataFiles->X664nNifnr937Nd && showDM)
	{
		showDM = false;
		//
		if (aboutBox != nullptr) aboutBox->setVisible(true);
		else
		{
			String xProcess = " SSE2";
			if (processor.dataFiles->processType == processAVX_FMA3) xProcess = " AVX/FMA";
			//
			aboutBox = new WAboutBox(processor.dataFiles->aboutBox, processor.dataFiles->aboutBoxVersion);
			aboutBox->setAlwaysOnTop(true);
			aboutBox->textColour = processor.dataFiles->aboutBoxVersionColour;
			aboutBox->textFont = processor.dataFiles->aboutBoxVersionFont;
			aboutBox->version = WUSIKZR_VERSION + xProcess + (processor.dataFiles->X664nNifnr937Nd ? " DM" : "");
			aboutBox->setBounds((getWidth() / 2) - (aboutBox->getWidth() / 2), (getHeight() / 2) - (aboutBox->getHeight() / 2), aboutBox->getWidth(), aboutBox->getHeight());
			addAndMakeVisible(aboutBox);
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessorEditor::paint (Graphics& g)
{
	if (processor.dataFiles->dataFilesError.isNotEmpty())
	{
		g.fillAll(Colours::black);
		g.setColour(Colours::white);
		g.setFont(24.0f);
		g.drawFittedText(processor.dataFiles->dataFilesError, getLocalBounds(), Justification::centred, 1);
	}
	else
	{
		WusikPinkImage::drawImageVertical(g, processor.dataFiles->background, 0, 0, getHeight(), false, true);
		//
		g.drawImage(
			processor.dataFiles->allImages[kBackground_EnvelopeSelector],
			processor.dataFiles->envelopeSelector.getX(),
			processor.dataFiles->envelopeSelector.getY(),
			processor.dataFiles->allImages[kBackground_EnvelopeSelector].getWidth(),
			processor.dataFiles->allImages[kBackground_EnvelopeSelector].getHeight() / 3,
			0, (processor.dataFiles->allImages[kBackground_EnvelopeSelector].getHeight() / 3) * processor.editingEnvelope,
			processor.dataFiles->allImages[kBackground_EnvelopeSelector].getWidth(),
			processor.dataFiles->allImages[kBackground_EnvelopeSelector].getHeight() / 3);
		//
		g.drawImage(
			processor.dataFiles->allImages[kBackground_ZoneSelectors],
			processor.dataFiles->zoneSelector.getX(),
			processor.dataFiles->zoneSelector.getY(),
			processor.dataFiles->allImages[kBackground_ZoneSelectors].getWidth(),
			processor.dataFiles->allImages[kBackground_ZoneSelectors].getHeight() / 4,
			0, (processor.dataFiles->allImages[kBackground_ZoneSelectors].getHeight() / 4) * processor.currentZone,
			processor.dataFiles->allImages[kBackground_ZoneSelectors].getWidth(),
			processor.dataFiles->allImages[kBackground_ZoneSelectors].getHeight() / 4);
		//
		g.setFont(processor.dataFiles->presetNameFont);
		g.setColour(processor.dataFiles->presetNameColour);
		g.drawText(processor.presetBankName + " / " + processor.getProgramName(processor.currentProgram), processor.dataFiles->presetName, Justification::centred);
		//
		g.setFont(processor.dataFiles->waveBankFont);
		//
		for (int xx = 0; xx < 4; xx++)
		{
			g.setColour(processor.dataFiles->waveBankColour); 
			if (isMouseOver() && processor.dataFiles->waveBanks[xx].contains(getMouseXYRelative())) g.setColour(Colours::red.withAlpha(0.68f));
			//
			g.drawText(processor.programs[processor.currentProgram]->waveBankName[processor.currentZone][xx], processor.dataFiles->waveBanks[xx], Justification::centred);
			//
			g.drawImage(
				processor.dataFiles->allImages[kKnob11_TinyLED],
				processor.dataFiles->zoneIndicators[xx].getX(),
				processor.dataFiles->zoneIndicators[xx].getY(),
				processor.dataFiles->allImages[kKnob11_TinyLED].getWidth(),
				processor.dataFiles->allImages[kKnob11_TinyLED].getHeight() / 2,
				0, (processor.dataFiles->allImages[kKnob11_TinyLED].getHeight() / 2) * ((processor.programs[processor.currentProgram]->sequencerLayers[xx].size() > 0) ? 1 : 0),
				processor.dataFiles->allImages[kKnob11_TinyLED].getWidth(),
				processor.dataFiles->allImages[kKnob11_TinyLED].getHeight() / 2);
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessorEditor::mouseDown(const MouseEvent& event)
{
	if (!SystemStats::hasSSE2()) return;
	if (processor.dataFiles->dataFilesError.isNotEmpty() && processor.dataFiles->dataFolder.isEmpty()) return;
	//
	for (int xx = 0; xx < 4; xx++)
	{
		if (processor.dataFiles->waveBanks[xx].contains(event.getPosition()))
		{
			PopupMenu mp;
			//
			ARRAY_Iterator(processor.dataFiles->waveBankNames)
			{
				mp.addItem(index + 1, processor.dataFiles->waveBankNames[index], true, processor.programs[processor.currentProgram]->waveBankName[processor.currentZone][xx].compare(processor.dataFiles->waveBankNames[index]) == 0);
			}
			int result = mp.show();
			if (result > 0)
			{
				processor.loadWaveBanks(xx, processor.dataFiles->waveBankNames[result -1], processor.currentZone);
			}
			//
			return;
		}
	}
	//
	if (processor.dataFiles->prevPreset.contains(event.getPosition()) && processor.currentProgram > 0)
	{
		clearAndPrepareInterface();
		processor.setCurrentProgram(processor.currentProgram - 1);
	}
	else if (processor.dataFiles->nextPreset.contains(event.getPosition()) && processor.currentProgram < MAX_PROGRAMS)
	{
		clearAndPrepareInterface();
		processor.setCurrentProgram(processor.currentProgram + 1);
	}
	else if (processor.dataFiles->envelopeSelector.contains(event.getPosition()))
	{
		int yPos = event.getPosition().getY() - processor.dataFiles->envelopeSelector.getY();
		int xHOne = processor.dataFiles->envelopeSelector.getHeight() / 3;
		if (yPos <= xHOne) processor.editingEnvelope = 0;
		else if (yPos <= (xHOne * 2)) processor.editingEnvelope = 1;
		else processor.editingEnvelope = 2;
		updateEnvelopesVisible();
		repaint();
	}
	else if (processor.dataFiles->zoneSelector.contains(event.getPosition()))
	{
		int xPos = event.getPosition().getX() - processor.dataFiles->zoneSelector.getX();
		int xHOne = processor.dataFiles->zoneSelector.getWidth() / 4;
		if (xPos <= xHOne) processor.currentZone = 0;
		else if (xPos <= (xHOne * 2)) processor.currentZone = 1;
		else if (xPos <= (xHOne * 3)) processor.currentZone = 2;
		else processor.currentZone = 3;
		updateZonesVisible();
		repaint();
	}
	else if (processor.dataFiles->productLogo.contains(event.getPosition()))
	{
		if (event.mods.isPopupMenu())
		{
			processor.ADSREnvelope_Amplitude[0].reset();
			processor.ADSREnvelope_Amplitude[1].reset();
			processor.ADSREnvelope_Amplitude[2].reset();
			processor.ADSREnvelope_Amplitude[3].reset();
			return;
		}
		//
		PopupMenu moptions;
		moptions.addItem(12, "Show Confirmation Boxes", true, processor.dataFiles->userSettings->getBoolValue("Ask Confirm Box"));
		//
		PopupMenu mp;
		if (processor.ownerStandalone != nullptr) mp.addItem(-22, "Audio/Midi Settings");
		//
		mp.addItem(2, "Plugins Manager");
		mp.addItem(4, "Panic");
		mp.addSubMenu("Options", moptions);
		mp.addSeparator();
		mp.addItem(1, "About Wusik ZR");
		int result = mp.show();
		//
		if (result == 1)
		{
			if (aboutBox != nullptr) aboutBox->setVisible(true);
			else
			{
				String xProcess = " SSE2";
				if (processor.dataFiles->processType == processAVX_FMA3) xProcess = " AVX/FMA";
				//
				aboutBox = new WAboutBox(processor.dataFiles->aboutBox, processor.dataFiles->aboutBoxVersion);
				aboutBox->setAlwaysOnTop(true);
				aboutBox->textColour = processor.dataFiles->aboutBoxVersionColour;
				aboutBox->textFont = processor.dataFiles->aboutBoxVersionFont;
				aboutBox->version = WUSIKZR_VERSION + xProcess + (processor.dataFiles->X664nNifnr937Nd ? " DM" : "");
				aboutBox->setBounds((getWidth() / 2) - (aboutBox->getWidth() / 2), (getHeight() / 2) - (aboutBox->getHeight() / 2), aboutBox->getWidth(), aboutBox->getHeight());
				addAndMakeVisible(aboutBox);
			}
		}
		else if (result == 12)
		{
			processor.dataFiles->userSettings->setValue("Ask Confirm Box", !processor.dataFiles->userSettings->getBoolValue("Ask Confirm Box"));
		}
		else if (result == 4)
		{
			processor.ADSREnvelope_Amplitude[0].reset();
			processor.ADSREnvelope_Amplitude[1].reset();
			processor.ADSREnvelope_Amplitude[2].reset();
			processor.ADSREnvelope_Amplitude[3].reset();
		}
		else if (result == 2)
		{
			pluginListWindow = new WPluginListWindow((void*)this, processor.dataFiles);
			pluginListWindow->toFront(true);
		}
		else if (result == -22)
		{
			if (processor.ownerStandalone != nullptr)
			{
				((StandalonePluginHolder*)processor.ownerStandalone)->showAudioSettingsDialog("Audio/Midi Settings");
			}
		}
	}
	else if (processor.dataFiles->presetName.contains(event.getPosition()))
	{
		PopupMenu mp;
		//
		PopupMenu mb;
		mb.addItem(-12, "Save Bank");
		mb.addItem(-14, "Rename Bank");
		mb.addItem(-16, "Delete Bank");
		mb.addSeparator();
		//
		ARRAY_Iterator(processor.dataFiles->presetBankNames)
		{
			mb.addItem(1000 + index, processor.dataFiles->presetBankNames[index], true, processor.presetBankName.compare(processor.dataFiles->presetBankNames[index]) == 0);
		}
		//
		bool hasOneProgramAtLeast = false;
		File theFolder(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Programs");
		ScopedPointer<DirectoryIterator> BanksList = new DirectoryIterator(theFolder, false, "*.WusikZRProgram", File::TypesOfFileToFind::findFiles);
		if (BanksList->next()) hasOneProgramAtLeast = true;
		BanksList = nullptr;
		//
		mp.addSubMenu("Banks", mb);
		mp.addSeparator();
		mp.addItem(-10, "Rename Program");
		mp.addItem(-92, "Copy Program");
		mp.addItem(-94, "Paste Program", File(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Extras" + slash + "Copy.WusikZRProgram").existsAsFile());
		mp.addItem(-96, "Save Program");
		mp.addItem(-98, "Load Program", hasOneProgramAtLeast);
		mp.addSeparator();
		//
		for (int xp = 0; xp < MAX_PROGRAMS; xp++)
		{
			mp.addItem(xp + 1, processor.getProgramName(xp), true, xp == processor.currentProgram);
		}
		//
		int result = mp.show();
		if (result != 0)
		{
			if (result == -92)
			{
				if (processor.dataFiles->X664nNifnr937Nd)
				{
					MessageBox("Demonstration Version!", "We are sorry, but this Demonstration version does\nnot save presets/song information.");
					return;
				}
				//
				MemoryBlock xData;
				processor.getStateInformationInternal(xData, false);
				File xFile = File(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Extras" + slash + "Copy.WusikZRProgram");
				xFile.createDirectory();
				xFile.deleteFile();
				xFile.appendData(xData.getData(), xData.getSize());
			}
			else if (result == -94)
			{
				if (confirmBox("Paste Program"))
				{
					MemoryBlock xData;
					File xFile = File(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Extras" + slash + "Copy.WusikZRProgram");
					if (xFile.existsAsFile())
					{
						xFile.loadFileAsData(xData);
						processor.setStateInformation(xData.getData(), xData.getSize());
					}
				}
			}
			else if (result == -96)
			{
				if (processor.dataFiles->X664nNifnr937Nd)
				{
					MessageBox("Demonstration Version!", "We are sorry, but this Demonstration version does\nnot save presets/song information.");
					return;
				}
				//
				File(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Programs").createDirectory();
				//
				FileChooser saveFile("Save Current Program", processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Programs", "*.WusikZRProgram");
				if (saveFile.browseForFileToSave(true))
				{
					File xFile = saveFile.getResult();
					MemoryBlock xData;
					processor.getStateInformationInternal(xData, false);
					xFile.deleteFile();
					xFile.appendData(xData.getData(), xData.getSize());
				}
			}
			else if (result == -98)
			{
				File(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Programs").createDirectory();
				//
				FileChooser loadFile("Load Program", processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Programs", "*.WusikZRProgram");
				if (loadFile.browseForFileToOpen())
				{
					File xFile = loadFile.getResult();
					MemoryBlock xData;
					xFile.loadFileAsData(xData);
					processor.setStateInformation(xData.getData(), xData.getSize());
				}
			}
			else if (result >= 1000)
			{
				if (confirmBox("Load New Bank File"))
				{
					File xBank(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Banks" + slash + processor.dataFiles->presetBankNames[result - 1000] + ".WusikZRBank");
					if (xBank.exists())
					{
						clearAndPrepareInterface();
						updateZonesVisible(true);
						processor.stateBuffer.reset();
						processor.presetBankName = processor.dataFiles->presetBankNames[result - 1000];
						xBank.loadFileAsData(processor.stateBuffer);
						processor.newAction(kActionSetState);
					}
					else
					{
						MessageBox("Could not find the bank file!", xBank.getFullPathName());
					}
				}
			}
			else if (result == -12)
			{
				if (processor.dataFiles->X664nNifnr937Nd)
				{
					MessageBox("Demonstration Version!", "We are sorry, but this Demonstration version does\nnot save presets/song information.");
					return;
				}
				//
				AlertWindow w("Save Bank Of Programs", "", AlertWindow::NoIcon);
				w.addTextEditor("Name", processor.getProgramName(processor.currentProgram), "Name");
				w.addButton("Save", 1, KeyPress(KeyPress::returnKey, 0, 0));
				w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));
				//
				if (w.runModalLoop() == 1)
				{
					File xBank(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Banks" + slash + w.getTextEditorContents("Name") + ".WusikZRBank");
					//
					if (!xBank.existsAsFile() || ConfirmBox("Bank File Exists", "Replace File?"))
					{
						MemoryBlock bankData;
						processor.getStateInformation(bankData);
						processor.dataFiles->presetBankNames.add(w.getTextEditorContents("Name"));
						xBank.createDirectory();
						xBank.deleteFile();
						xBank.appendData(bankData.getData(), bankData.getSize());
						processor.presetBankName = w.getTextEditorContents("Name");
						repaint();
					}
				}
			}
			else if (result == -16)
			{
				if (ConfirmBox("Delete Bank File: " + processor.presetBankName, "Are You Sure?"))
				{
					File xBank(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Banks" + slash + processor.presetBankName + ".WusikZRBank");
					xBank.deleteFile();
					processor.dataFiles->presetBankNames.removeString(processor.dataFiles->dataFolder, true);
					repaint();
				}
			}
			else if (result == -14)
			{
				AlertWindow w("Rename Bank", "", AlertWindow::NoIcon);
				w.addTextEditor("Name", processor.getProgramName(processor.currentProgram), "Name");
				w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
				w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));
				//
				if (w.runModalLoop() == 1)
				{
					File xBank(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Banks" + slash + w.getTextEditorContents("Name") + ".WusikZRBank");
					if (xBank.exists())
					{
						MessageBox("Bank Already Exists!", "Can't continue...");
					}
					else
					{
						File(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Banks" + slash + processor.presetBankName + ".WusikZRBank").moveFileTo(xBank);
						int xInd = processor.dataFiles->presetBankNames.indexOf(processor.presetBankName, true);
						processor.dataFiles->presetBankNames.set(xInd, w.getTextEditorContents("Name"));
						repaint();
					}
				}
			}
			else if (result == -10)
			{
				AlertWindow w("Rename Program Name", "", AlertWindow::NoIcon);
				w.addTextEditor("Name", processor.getProgramName(processor.currentProgram), "Name");
				w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
				w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));
				//
				if (w.runModalLoop() == 1)
				{
					processor.changeProgramName(processor.currentProgram, w.getTextEditorContents("Name"));
					repaint();
				}
			}
			else
			{
				clearAndPrepareInterface();
				updateZonesVisible(true);
				processor.setCurrentProgram(result - 1);
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessorEditor::mouseUp(const MouseEvent& event)
{
	if (!SystemStats::hasSSE2()) return;
	//
	if (processor.dataFiles->dataFilesError.isNotEmpty())
	{
		if (processor.dataFiles->macValidateInstall)
		{
			File xValidate(processor.dataFiles->dataFolder + "Wusik ZR" + slash + "Validated.xml");
			xValidate.deleteFile();
			xValidate.create();
			if (xValidate.existsAsFile())
			{
				xValidate.appendText(SystemStats::getDeviceIdentifiers().joinIntoString("|") + ">>>mEFOEOEWOoncowNOWONcoNOenceNCEOOCONEOCNec98ec9>>>7ec97ec97ec86282cc8bec8becb9c92bc29cb7ec78ebc797279bcwcFR>EWQNcnQCQCJWE#*$(!&#$&!&!($912<<<749732");
				processor.dataFiles->dataFilesError = "Thank you! Validation Completed!\n\nPlease, close and reload Wusik ZR to continue...";
			}
			else
			{
				processor.dataFiles->dataFilesError = "Could not save to the Wusik Engine Data folder!\n\nPlease, make sure it has the right permissions!";
			}
			repaint();
		}
		else
		{
			FileChooser browseFolder("Locate the Wusik Engine Data Folder\n(where you find folders for each Wusik Product)");
			if (!browseFolder.browseForDirectory()) return;
			File findDataFiles = File(browseFolder.getResult().getFullPathName() + slash + "Wusik ZR" + slash + "Wavebanks");
			if (findDataFiles.exists() && findDataFiles.isDirectory())
			{
				processor.dataFiles->saveGlobalSettings("Wusik Engine", "Data Path", browseFolder.getResult().getFullPathName());
				processor.dataFiles->dataFilesError = "Data Folder OK!\n\nPlease, restart the application to continue.";
				repaint();
			}
			else
			{
				MessageBox("Error", "The selected folder doesn't have a Shared\\Wavebanks folder");
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessorEditor::resized()
{
	DEBUG_TO_FILE("Editor Resized");
	//
	if (processor.dataFiles->dataFilesError.isEmpty())
	{
		resizer->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
		processor.dataFiles->userSettings->setValue("GUI Height", getHeight());
		cachedImage = Image(Image::PixelFormat::ARGB, getWidth(), getHeight(), true);
		//
		for (int zones = 0; zones < 4; zones++)
		{
			if (layersViewPort[zones] != nullptr)
			{
				Rectangle<int> viewRect;
				processor.dataFiles->getSkinRectangle("Scroll Area", viewRect);
				viewRect.setBottom(getBounds().getBottom());
				layersViewPort[zones]->setBounds(viewRect);
				zoneComponents[zones]->setBounds(getBounds());
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
WPluginListWindow::WPluginListWindow(void* _owner, WusikDataFiles* _dataFiles)
	: DocumentWindow("List Of Plugins", Colours::white, DocumentWindow::closeButton),
	dataFiles(_dataFiles), wowner(_owner)
{
	this->setUsingNativeTitleBar(true);
	//
	const File deadMansPedalFile(dataFiles->dataFolder + File::separatorString + "Shared" + File::separatorString + "Recently Crashed Plugins");
	//
	WPluginListComponent* plugComp = new WPluginListComponent(*dataFiles->formatManager,
		*dataFiles->knownPluginList,
		deadMansPedalFile,
		dataFiles->pluginsList, dataFiles->dataFolder + "Shared" + slash, true, (void*) ((WusikZrAudioProcessorEditor*) wowner)->getAudioProcessor(),
		false, false, true, false);
	//
	plugComp->cNormal = Colours::black.withAlpha(0.82f);
	plugComp->cBackColour = Colours::white.withAlpha(0.72f);
	plugComp->useBackColour = true;
	//
	setContentNonOwned(plugComp, true);
	//
	setResizable(true, false);
	setResizeLimits(280, 280, 1900, 1900);
	setTopLeftPosition(60, 60);
	//
	restoreWindowStateFromString(dataFiles->userSettings->getValue("Plugins Window Position", "80 80 1022 682"));
	setVisible(true);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WPluginListWindow::closeButtonPressed()
{
	((WusikZrAudioProcessor*)((WusikZrAudioProcessorEditor*)wowner)->getAudioProcessor())->dataFiles->updateListOfPlugins();
	((WusikZrAudioProcessor*)((WusikZrAudioProcessorEditor*)wowner)->getAudioProcessor())->suspendProcessing(false);
	((WusikZrAudioProcessorEditor*)wowner)->pluginListWindow = nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
WPluginListWindow::~WPluginListWindow()
{
	if (dataFiles != nullptr) dataFiles->userSettings->setValue("Plugins Window Position", getWindowStateAsString());
	clearContentComponent();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessorEditor::clearAndPrepareInterface()
{
	DEBUG_TO_FILE("Editor Clear And Prepare Interface");
	//
	MessageManagerLock lock;
	//
	Graphics xGG(cachedImage);
	paintEntireComponent(xGG, false);
	cachedImageButton->setBounds(0, 0, getWidth(), getHeight());
	cachedImageButton->setImages(false, false, false, cachedImage, 1.0f, Colours::transparentBlack, cachedImage, 1.0f, Colours::transparentBlack, cachedImage, 1.0f, Colours::transparentBlack);
	cachedImageButton->setVisible(true);
	cachedImageButton->setAlwaysOnTop(true);
	//
	updateZonesVisible(true);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool WusikZrAudioProcessorEditor::confirmBox(String message)
{
	if (processor.dataFiles->userSettings->getBoolValue("Ask Confirm Box", true))
	{
		AlertWindow xconfirmBox(message, "Are You Sure?", AlertWindow::NoIcon);
		xconfirmBox.addButton("Yes", 1, KeyPress(KeyPress::returnKey));
		xconfirmBox.addButton("No", 0, KeyPress(KeyPress::escapeKey));
		xconfirmBox.addButton("Always", 2);
		//
		int result = xconfirmBox.runModalLoop();
		//
		if (result == 2) processor.dataFiles->userSettings->setValue("Ask Confirm Box", false);
		//
		if (result == 1 || result == 2) return true; else return false;
	}
	//
	return true;
}