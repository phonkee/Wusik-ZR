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
#include "PluginProcessor.h"
#include "PluginEditor.h"
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
WusikZrAudioProcessor::WusikZrAudioProcessor() : currentProgram(0), sampPos(0), midiMessage(0xf4, 0.0), pitchBendValue(0.0f), pitchBendTarget(0.0f), 
												 pitchBendRate(0.0f), globalBPM(120.0), addingParametersPosition(0), 
												editingEnabled(false), presetBankName("Default"), dummyValue(0.0f), lastYPositionEditing(0), lastXPositionEditing(0),
												midiLearn(-1), ownerStandalone(nullptr), editingEnvelope(DEFAULT_ENVELOPE), lastSampleRate(44100.0),
												processingThread(this), isFirstNote(true)
{
	#if WIS_DEBUG
		WCREATE_DEBUG_WINDOW(WIS_DEBUG);
	#endif
	//
	DEBUG_TO_FILE("Starting");
	//
	waveBanksArray.add("A");
	waveBanksArray.add("B");
	waveBanksArray.add("C");
	waveBanksArray.add("D");
	//
	dataFiles = WusikDataFiles::getInstance();
	if (dataFiles == nullptr) { MessageBox("Error", "Internal Error!"); return; }
	//
	DEBUG_TO_FILE("Data Files Created");
	//
	isProcessingSomething.set(0);
	zeromem(filterProcessSignal, sizeof(filterProcessSignal));
	currentZone = 0;
	//
	fadeOutRate = 0.0f;
	fadeOutCounter = 1.0f;
	//
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	//
	if (dataFiles->skinSettings != nullptr)
	{
		editingEnabled = dataFiles->skinSettings->getBoolValue("Edit Skin Enabled", false);
		editingEnabledDrawRectangles = dataFiles->skinSettings->getBoolValue("Edit Skin Draw Text Rectangles", false);
	}
	//
	for (int xp = 0; xp < MAX_PROGRAMS; xp++)
	{
		programs.add(new WusikZRProgram("Init " + String(xp + 1)));
		programs.getLast()->sequencerLayers[0].add(new WusikSequencer(wavetables[0]));
	}
	//
	for (int zones = 0; zones < 4; zones++)
	{
		waveTable[zones] = new WusikWavetable();
		//
		prevOctave[zones] = 0.0f;
		prevTune[zones] = 0.0f;
		retriggerMonoNote[zones] = false;
		extraIdleCounter[zones] = 0;
	}
	//
	valuesList = programs[currentProgram]->values[currentZone];
	//
	DEBUG_TO_FILE("Create Parameters");
	//
	for (int zones = 0; zones < 4; zones++)
	{
		parametersMainStart[zones] = getParameters().size();
		//
		WAddParameterDb ("M.Volume", "", 0.6f);																		WPARTOOLTIP("Master Volume"); WPARSkin("Master Volume");
		WAddParameterPan("M.Stereo Pan", "", 0.0f);																						  WPARSkin("Master Pan");
		WAddParameter   ("MIDI CH Input", "", 0.0f, 0.0f, 16.0f, 1.0f);												WPAR(minText, "Omni");WPARTOOLTIP("MIDI Channel Input"); WPAR_GLOBAL; WPAR(belowThisIsOFF, (1.0f/16.0f));
		WAddParameterM  ("M.Mode", "", 4.0f, 6.0f, "Mono\n4 Voices\n8 Voices\n16 Voices\n24 Voices\n32 Voices");						  WPARSkin("Master Mode");
		WAddParameter   ("Unison", "", 0.0f, 0.0f, 1.0f, 0.0001f);													WPAR(minText, "Off"); WPARTOOLTIP("Use the number of voices");
		WAddParameter   ("U Vol Spread", "", 0.8f, 0.0f, 1.0f, 0.0001f);											WPAR(minText, "Off"); WPARTOOLTIP("Unison Volume Spread");
		WAddParameter   ("U Pan Spread", "", 1.0f, 0.0f, 1.0f, 0.0001f);											WPAR(minText, "Off"); WPARTOOLTIP("Unison Stereo Spread");
		WAddParameterM  ("Quality", "", 2.0f, 4.0f, "Draft\nNormal\nHigh\nHigher");									WPAR_GLOBAL;
		WAddParameter	("Pitch Bend", "", 12.0f, 0.0f, 48.0f, 1.0f);												WPAR(minText, "Off"); WPAR_GLOBAL; WPAR(belowThisIsOFF, (1.0f / 48.0f));
		WAddParameter   ("Octave", "", 0.0f, -6.0f, 6.0f, 1.0f);													WPAR_GLOBAL; WPAR(isOctave, true);
		WAddParameter   ("M.Tune", "", 0.0f, -12.0f, 12.0f, 1.0f);																			WPARSkin("Master Tune");
		WAddParameter   ("M.Fine", "", 0.0f, -1.0f, 1.0f, 0.0001f);																			WPARSkin("Master Fine");
		//
		WAddParameter   ("A.ADSR.A", "Attack", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(isEnvelope, true); WPARSkin("Envelope Attack");
		WAddParameter   ("A.ADSR.D", "Decay", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(isEnvelope, true); WPARSkin("Envelope Decay");
		WAddParameter	("A.ADSR.S", "Sustain", 1.0f, 0.0f, 1.0f, 0.0001f);																	WPARSkin("Envelope Sustain");
		WAddParameter	("A.ADSR.R", "Release", 0.2f, 0.0f, 1.0f, 0.0001f);											WPAR(isEnvelope, true); WPARSkin("Envelope Release");
		WAddParameter	("A.ADSR.V", "Velocity", 0.5f, 0.0f, 1.0f, 0.0001f);										WPAR(minText, "Off");	WPARSkin("Envelope Velocity");
		WAddParameter	("A.ADSR.Cp","Clip", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(minText, "Off");	WPARSkin("Envelope Clip"); WPARTOOLTIP("Results in a HOLD effect on the attack, decay and release stages.");
		WAddParameter	("A.ADSR.C", "Curve", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(minText, "Linear");WPARSkin("Envelope Curve");
		WAddParameterM  ("A.ADSR.Mode", "", 0.0f, 2.0f, "Normal\nLooped");																	WPARSkin("Envelope Mode");
		//
		WAddParameter   ("F.ADSR.A", "Attack", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(isEnvelope, true); WPARSkin("Envelope Attack");
		WAddParameter   ("F.ADSR.D", "Decay", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(isEnvelope, true); WPARSkin("Envelope Decay");
		WAddParameter	("F.ADSR.S", "Sustain", 1.0f, 0.0f, 1.0f, 0.0001f);																	WPARSkin("Envelope Sustain");
		WAddParameter	("F.ADSR.R", "Release", 0.2f, 0.0f, 1.0f, 0.0001f);											WPAR(isEnvelope, true); WPARSkin("Envelope Release");
		WAddParameter	("F.ADSR.V", "Velocity", 0.5f, 0.0f, 1.0f, 0.0001f);										WPAR(minText, "Off");	WPARSkin("Envelope Velocity");
		WAddParameter	("F.ADSR.Cp","Clip", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(minText, "Off");	WPARSkin("Envelope Clip"); WPARTOOLTIP("Results in a HOLD effect on the attack, decay and release stages.");
		WAddParameter	("F.ADSR.C", "Curve", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(minText, "Linear");WPARSkin("Envelope Curve");
		WAddParameterM  ("F.ADSR.Mode", "", 0.0f, 2.0f, "Normal\nLooped");																	WPARSkin("Envelope Mode");
		//
		WAddParameter   ("P.ADSR.A", "Attack", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(isEnvelope, true); WPARSkin("Envelope Attack");
		WAddParameter   ("P.ADSR.D", "Decay", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(isEnvelope, true); WPARSkin("Envelope Decay");
		WAddParameter	("P.ADSR.S", "Sustain", 1.0f, 0.0f, 1.0f, 0.0001f);																	WPARSkin("Envelope Sustain");
		WAddParameter	("P.ADSR.R", "Release", 0.2f, 0.0f, 1.0f, 0.0001f);											WPAR(isEnvelope, true); WPARSkin("Envelope Release");
		WAddParameter	("P.ADSR.V", "Velocity", 0.5f, 0.0f, 1.0f, 0.0001f);										WPAR(minText, "Off");	WPARSkin("Envelope Velocity");
		WAddParameter	("P.ADSR.Cp","Clip", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(minText, "Off");	WPARSkin("Envelope Clip"); WPARTOOLTIP("Results in a HOLD effect on the attack, decay and release stages.");
		WAddParameter	("P.ADSR.C", "Curve", 0.0f, 0.0f, 1.0f, 0.0001f);											WPAR(minText, "Linear");WPARSkin("Envelope Curve");
		WAddParameterM  ("P.ADSR.Mode", "", 0.0f, 2.0f, "Normal\nLooped");																	WPARSkin("Envelope Mode");
		//
		WAddParameterM  ("F.Mode", "", 0.0f, 8.0f, "LowPass\nBandPass\nHighPass\nNotch\nLowPass+\nBandPass+\nHighPass+\nNotch+");			WPARSkin("Filter Mode");
		WAddParameter   ("F.Freq", "", 1.0f, 0.0f, 1.0f, 0.0001f);																			WPARSkin("Filter Frequency");
		WAddParameter   ("F.Rezo", "", 0.2f, 0.0f, 1.0f, 0.0001f);																			WPARSkin("Filter Rezonance");
		WAddParameter   ("F.Smooth", "", 0.2f, 0.0f, 1.0f, 0.0001f);												WPAR(minText, "Off");	WPARSkin("Filter Smooth");
		WAddParameter   ("F.Envelope", "", 0.0f, -1.0f, 1.0f, 0.0001f);																		WPARSkin("Filter Envelope");
		WAddParameter   ("F.Limit", "", 0.0f, 0.0f, 1.0f, 0.0001f);													WPAR(minText, "Off");	WPARSkin("Filter Limit");
		//
		WAddParameter   ("MasterLmt", "", 1.0f, 0.0f, 1.0f, 1.0f);													WPARTOOLTIP("Audio Limiter (one per zone)");
		WAddParameter   ("Glide", "", 0.0f, 0.0f, 1.0f, 0.0001f);													WPAR(minText, "Off"); WPARTOOLTIP("Works best in Monophonic Mode");
		WAddParameter   ("AutoGlide", "", 1.0f, 0.0f, 1.0f, 1.0f);													WPARTOOLTIP("Auto Glide: when on it will not glide the first note");
		//
		for (int xp = 0; xp < WMAX_LAYERS; xp++)
		{
			parametersLayerStart[zones][xp] = getParameters().size();
			//
			// Static Layer Details (also shared by Sequenced Layer) //
			WAddParameter2("L.Add", "", 0.0f, 0.0f, 1.0f, 1.0f);		WPARSkin("Layer Merge Type"); WPARTOOLTIP("Add or Multiply (ring modulation) with the previous layer");
			WAddParameter2("L.Mute", "", 1.0f, 0.0f, 1.0f, 1.0f);	WPARSkin("Layer Mute");
			WAddParameter2("L.Solo", "", 0.0f, 0.0f, 1.0f, 1.0f);
			WAddParameterDb2("L.Volume", "", 0.6f);					WPARSkin("Layer Volume");
			WAddParameterPan2("L.Pan", "", 0.0f);						WPARSkin("Layer Pan");
			WAddParameter2("L.Tune", "", 0.0f, -48.0f, 48.0f, 1.0f);	WPARSkin("Layer Tune");
			WAddParameter2("L.Fine", "", 0.0f, -1.0f, 1.0f, 0.0001f); WPARSkin("Layer Fine");
			WAddParameter2("L.PEnv", "", 0.0f, -48.0f, 48.0f, 1.0f); WPARSkin("Layer Pitch Envelope"); WPARTOOLTIP("Pitch Envelope");
			WAddParameter("L.Wave", "", 0.0f, 0.0f, 48.0f, 1.0f);   WPARSkin("Layer Waveform");
			//
			// Sequencer Details //
			WAddParameterM2("L.Sync", "", 0.0f, 1.0f, "BPM\nHZ");		WPARSkin("Layer Sync"); WPARTOOLTIP("BPM or HZ");
			WAddParameter2("L.Time 1", "", 0.0f, 0.0f, 1.0f, 0.0001f); WPARSkin("Layer Time 1");
			WAddParameter2("L.Time 2", "", 0.0f, 0.0f, 1.0f, 0.0001f); WPARSkin("Layer Time 2");
			WAddParameter2("L.Loop To", "", 1.0f, 1.0f, 128.0f, 1.0f); WPARSkin("Layer Loop To");
			WAddParameterM2("L.Mode", "", 0.0f, 4.0f, ">>\n<<\n><\nRND"); WPARSkin("Layer Mode");
			//
			for (int xs = 0; xs < 128; xs++)
			{
				WAddParameterDb2("S.Vol", "", 0.6f);						WPARSkin("Step Volume");
				WAddParameterPan2("S.Pan", "", 0.0f);						WPARSkin("Step Pan");
				WAddParameter2("S.Tune", "", 0.0f, -48.0f, 48.0f, 1.0f); WPARSkin("Step Tune");
				WAddParameter2("S.Fine", "", 0.0f, -1.0f, 1.0f, 0.0001f); WPARSkin("Step Fine");
				WAddParameter2("S.Filter", "", 0.0f, -1.0f, 1.0f, 0.0001f); WPARSkin("Step Filter");
				WAddParameter2("S.Time", "", 1.0f, 1.0f, 128.0f, 1.0f);	WPARSkin("Step Time"); WPARTOOLTIP("How many times this step will wait");
				WAddParameter2("S.Wave", "", 0.0f, 0.0f, 48.0f, 1.0f);	WPARSkin("Step Waveform");
				WAddParameter2("S.XFade", "", 0.0f, 0.0f, 1.0f, 0.0001f); WPARSkin("Step Crossfade"); WPARTOOLTIP("Crossfade one waveform to another");
			}
		}
		//
		for (int xp = 0; xp < MAX_EFFECTS; xp++)
		{
			parametersEffectsStart[zones][xp] = getParameters().size();
			WAddParameter2("SFX" + String(xp + 1) + "ON", "", 1.0f, 0.0f, 1.0f, 1.0f); WPARSkin("SFX ON");
			WAddParameter2("SFX" + String(xp + 1) + "WetDry", "", 0.5f, 0.0f, 1.0f, 0.0001f); WPARSkin("SFX WetDry"); WPARTOOLTIP("Wet/Dry");
			//
			for (int xs = 0; xs < MAX_EFFECT_PARAMETERS; xs++)
			{
				WAddParameter2("SFX" + String(xp + 1) + "P" + String(xs + 1), "", 0.0f, 0.0f, 1.0f, 0.0001f);
				WPARSkin("SFX Par " + String(xs + 1));
			}
		}
	}
	//
	DEBUG_TO_FILE("Set Default Values");
	//
	// Set the default values to all programs (this can be removed once a Bank of presets is loaded directly //
	for (int xp = 0; xp < MAX_PROGRAMS; xp++)
	{
		for (int xv = 0; xv < MAX_PARAMETERS; xv++)
		{
			for (int zones = 0; zones < 4; zones++)
			{
				programs[xp]->values[zones][xv] = getParameters()[xv + parametersMainStart[zones]]->getValue();
			}
		}
	}
	//
	#if WCREATE_WAVE_BANKS
		createWaveBanks();
	#endif
	//
	DEBUG_TO_FILE("Load Limiter");
	//
	if (dataFiles->dataFolder.isNotEmpty())
	{
		String vstErrorMessage;
		VSTLimiterDescription.fileOrIdentifier = dataFiles->limiterFile;
		VSTLimiterDescription.pluginFormatName = "VST";
		if (File(VSTLimiterDescription.fileOrIdentifier).exists())
		{
			AudioPluginFormatManager xWFormatManager;
			xWFormatManager.addDefaultFormats();
			//
			for (int xl = 0; xl < MAX_ZONES; xl++)
			{
				VSTLimiter[xl] = xWFormatManager.createPluginInstance(VSTLimiterDescription, getSampleRate(), 4, vstErrorMessage);
				if (VSTLimiter[xl] == nullptr)
				{
					MessageBox(dataFiles->limiterFile, vstErrorMessage);
					break;
				}
			}
		}
	}
	//
	DEBUG_TO_FILE("Load Default Bank");
	//
	#if DEFAULT_BANK_LOAD
		File xBank(dataFiles->dataFolder + "Wusik ZR" + slash + "Banks" + slash + "Default.WusikZRBank");
		if (xBank.exists())
		{
			xBank.loadFileAsData(stateBuffer);
			setStateInformationFromBuffer();
		}
		else
		{
			dataFiles->dataFilesError = "Could not find the initial Default.WusikZRBank file!";
		}
	#endif
	//
	DEBUG_TO_FILE("Almost Finished Starting");
	//
	loadAllEffects();
	updateLayers();
	redoMidiLearned();
	//
	DEBUG_TO_FILE("Finished Starting");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
WusikZrAudioProcessor::~WusikZrAudioProcessor()
{
	#if WIS_DEBUG
		WDESTROY_DEBUG_WINDOW(WIS_DEBUG);
	#endif
	//
	DEBUG_TO_FILE("Start Destroying");
	//
	programs.clear();
	//
	for (int xz = 0; xz < MAX_ZONES; xz++)
	{
		waveTable[xz] = nullptr;
	}
	//
	DEBUG_TO_FILE("Finished Destroying");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int WusikZrAudioProcessor::getCurrentProgram()
{
    return currentProgram;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::checkAndSaveEffectData(bool removeEffects)
{
	DEBUG_TO_FILE("Check And Save Effect Data");
	//
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	//
	for (int zones = 0; zones < 4; zones++)
	{
		ARRAY_Iterator(programs[currentProgram]->effects[zones])
		{
			if (programs[currentProgram]->effects[zones][index]->hasChanges &&
				programs[currentProgram]->effects[zones][index]->hasEffect())
			{
				programs[currentProgram]->effects[zones][index]->stateInformation.reset();
				programs[currentProgram]->effects[zones][index]->getStateInformation(programs[currentProgram]->effects[zones][index]->stateInformation);
				programs[currentProgram]->effects[zones][index]->hasChanges = false;
			}
			//
			if (programs[currentProgram]->effects[zones][index]->hasEffect())
			{
				programs[currentProgram]->effects[zones][index]->currentProgram = programs[currentProgram]->effects[zones][index]->getCurrentProgram();
				if (removeEffects) programs[currentProgram]->effects[zones][index]->releaseEffect();
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::setCurrentProgram(int index)
{
	DEBUG_TO_FILE("Set Current Program");
	//
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	//
	newAction(kActionChangeProgram, index);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const String WusikZrAudioProcessor::getProgramName (int index)
{
	if (dataFiles->dataFilesError.isNotEmpty()) return String();
	//
	return programs[index]->name;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::changeProgramName (int index, const String& newName)
{
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	//
	programs[index]->name = newName;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	DEBUG_TO_FILE("Prepare to Play Started");
	//
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	//
	lastSampleRate = sampleRate;
	//
	ARRAY_Iterator(getParameters())
	{
		((WusikZRParameter*)getParameters()[index])->sampleRate = lastSampleRate;
	}
	//
	for (int xz = 0; xz < MAX_ZONES; xz++)
	{
		theFilter[xz][kLeft].SampleRateFloat = sampleRate;
		theFilter[xz][kRight].SampleRateFloat = sampleRate;
		//
		if (VSTLimiter[xz] != nullptr)
		{
			VSTLimiter[xz]->setPlayHead(playHead);
			VSTLimiter[xz]->prepareToPlay(sampleRate, 4);
		}
	}
	//
	hasPrepareToPlay.set(1);
	//
	DEBUG_TO_FILE("Prepare To Play Finished");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	DEBUG_TO_FILE("Get State Information");
	//
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	if (dataFiles->X664nNifnr937Nd) return;
	//
	getStateInformationInternal(destData, true);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::getCurrentProgramStateInformation(MemoryBlock& destData)
{
	DEBUG_TO_FILE("Get Current Program State Information");
	//
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	if (dataFiles->X664nNifnr937Nd) return;
	//
	getStateInformationInternal(destData, false);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::getStateInformationInternal(MemoryBlock& destData, bool wholeBank)
{
	DEBUG_TO_FILE("Get State Information Internal");
	//
	checkAndSaveEffectData(false);
	//
	ScopedPointer<MemoryOutputStream> memStream = new MemoryOutputStream(destData, false);
	//
	memStream->writeString("Wusik ZR");
	memStream->writeInt(6); // Version //
	//
	if (wholeBank)
	{
		memStream->writeInt(MAX_PROGRAMS);
		memStream->writeInt(currentProgram);
		memStream->writeInt(editingEnvelope);
	}
	else  memStream->writeInt('prst'); // Current Preset Only //
	//
	for (int xp = 0; xp < MAX_PROGRAMS; xp++)
	{
		if (!wholeBank) xp = currentProgram;
		//
		memStream->writeString("P" + String(xp)); 
		memStream->writeString(programs[xp]->name);
		//
		for (int zones = 0; zones < 4; zones++)
		{
			memStream->writeString("Z" + String(zones));
			//
			memStream->writeString(programs[xp]->waveBankName[zones][0]);
			memStream->writeString(programs[xp]->waveBankName[zones][1]);
			memStream->writeString(programs[xp]->waveBankName[zones][2]);
			memStream->writeString(programs[xp]->waveBankName[zones][3]);
			//
			memStream->writeBool(programs[xp]->showLayers[zones][0]);
			memStream->writeBool(programs[xp]->showLayers[zones][1]);
			memStream->writeBool(programs[xp]->showLayers[zones][2]);
			//
			memStream->writeInt64(MAX_PROGRAM_VALUES);
			for (int xv = 0; xv < MAX_PROGRAM_VALUES; xv++)
			{
				memStream->writeFloat(programs[xp]->values[zones][xv]);
			}
			//
			memStream->writeInt64(programs[xp]->effects[zones].size());
			ARRAY_Iterator(programs[xp]->effects[zones])
			{
				memStream->writeString(programs[xp]->effects[zones][index]->description);
				memStream->writeString(programs[xp]->effects[zones][index]->name);
				memStream->writeString(programs[xp]->effects[zones][index]->type);
				//
				memStream->writeInt64(programs[xp]->effects[zones][index]->stateInformation.getSize());
				memStream->write(programs[xp]->effects[zones][index]->stateInformation.getData(),
					programs[xp]->effects[zones][index]->stateInformation.getSize());
				//
				memStream->writeFloat(programs[xp]->effects[zones][index]->isActive);
				memStream->writeFloat(programs[xp]->effects[zones][index]->wetDry);
				memStream->writeInt64(programs[xp]->effects[zones][index]->currentProgram);
			}
			//
			memStream->writeInt64(programs[xp]->sequencerLayers[zones].size());
			ARRAY_Iterator(programs[xp]->sequencerLayers[zones])
			{
				memStream->writeString("Sq" + String(index) + ":");
				//
				memStream->writeBool(programs[xp]->sequencerLayers[zones][index]->isStatic);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->isBPM);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->mode);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->time1);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->time2);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->loopTo);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->layer_tune);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->layer_volume);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->layer_pan);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->layer_fine);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->layer_mute);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->layer_solo);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->layer_addMultiply);
				memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->layer_pitchEnvelope);
				//
				memStream->writeInt64(programs[xp]->sequencerLayers[zones][index]->stepsSize);
				//
				for (int xStep = 0; xStep < programs[xp]->sequencerLayers[zones][index]->stepsSize; xStep++)
				{
					memStream->writeString("St" + String(xStep) + ":");
					//
					memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->steps[xStep].volume);
					memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->steps[xStep].pan);
					memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->steps[xStep].fine);
					memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->steps[xStep].waveform);
					memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->steps[xStep].tune);
					memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->steps[xStep].time);
					memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->steps[xStep].crossFade);
					memStream->writeFloat(programs[xp]->sequencerLayers[zones][index]->steps[xStep].filter);
				}
			}
		}
		//
		if (!wholeBank) break;
	}
	//
	memStream->writeInt64(getParameters().size());
	ARRAY_Iterator(getParameters())
	{
		memStream->writeInt(((WusikZRParameter*)getParameters()[index])->midiRemote);
	}
	//
	memStream->writeString("END");
	memStream->flush();
	//
	//File xFile("c:\\temp\\getState" + String(Time::getApproximateMillisecondCounter()) + "txt");
	//xFile.appendData(destData.getData(), destData.getSize());
	//
	memStream = nullptr;
	//
	DEBUG_TO_FILE("Get State Information Internal Finished");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	DEBUG_TO_FILE("Set State Information");
	//
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	//
	stateBuffer.reset();
	stateBuffer.append(data, sizeInBytes);
	newAction(kActionSetState);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::setCurrentProgramStateInformation(const void* data, int sizeInBytes)
{
	DEBUG_TO_FILE("Set Current Program State Information");
	//
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	//
	stateBuffer.reset();
	stateBuffer.append(data, sizeInBytes);
	newAction(kActionSetState);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::setStateInformationFromBuffer()
{
	DEBUG_TO_FILE("Set State Information From Buffer");
	//
	bool wholeBank = true;
	//
	if (stateBuffer.getSize() == 0) return;
	ScopedPointer<MemoryInputStream> memStream = new MemoryInputStream(stateBuffer.getData(), stateBuffer.getSize(), false);
	//
	if (memStream->readString().compare("Wusik ZR") == 0)
	{
		int xVersion = memStream->readInt();
		int xPrograms = memStream->readInt();
		//
		if (xPrograms == 'prst')
		{
			wholeBank = false;
			//
			for (int zones = 0; zones < MAX_ZONES; zones++)
			{
				programs[currentProgram]->effects[zones].clear();
			}
		}
		else
		{
			currentProgram = memStream->readInt();
			editingEnvelope = memStream->readInt();
			//
			for (int xp = 0; xp < MAX_PROGRAMS; xp++)
			{
				for (int zones = 0; zones < MAX_ZONES; zones++)
				{
					programs[xp]->effects[zones].clear();
				}
			}
		}
		//
		for (int xp = 0; xp < xPrograms; xp++)
		{
			if (!wholeBank) xp = currentProgram;
			//
			String Header = memStream->readString();
			if (Header.substring(0,1).compare("P") != 0) goto xerror;
			programs[xp]->name = memStream->readString();
			//
			for (int zones = 0; zones < MAX_ZONES; zones++)
			{
				Header = memStream->readString();
				if (Header.compare("Z" + String(zones)) != 0) goto xerror;
				//
				programs[xp]->waveBankName[zones][0] = memStream->readString();
				programs[xp]->waveBankName[zones][1] = memStream->readString();
				programs[xp]->waveBankName[zones][2] = memStream->readString();
				programs[xp]->waveBankName[zones][3] = memStream->readString();
				//
				if (xVersion >= 6)
				{
					programs[xp]->showLayers[zones][0] = memStream->readBool();
					programs[xp]->showLayers[zones][1] = memStream->readBool();
					programs[xp]->showLayers[zones][2] = memStream->readBool();
				}
				else
				{
					programs[xp]->showLayers[zones][0] =
						programs[xp]->showLayers[zones][1] =
						programs[xp]->showLayers[zones][2] = true;
				}
				//
				int xValues = memStream->readInt64();
				for (int xv = 0; xv < xValues; xv++)
				{
					programs[xp]->values[zones][xv] = memStream->readFloat();
				}
				//
				if (xVersion >= 2)
				{
					int xValues2 = memStream->readInt64();
					for (int xv = 0; xv < xValues2; xv++)
					{
						String xDesc = memStream->readString();
						String xName = memStream->readString();
						String xType = "PLUGIN";
						//
						if (xVersion >= 4) xType = memStream->readString();
						loadEffect(xDesc, xName, xType, xp, zones);
						//
						int xDataSize = memStream->readInt64();
						programs[xp]->effects[zones].getLast()->stateInformation.reset();
						if (xDataSize > 0)
						{
							programs[xp]->effects[zones].getLast()->stateInformation.setSize(xDataSize);
							memStream->read(programs[xp]->effects[zones].getLast()->stateInformation.getData(), xDataSize);
						}
						//
						programs[xp]->effects[zones].getLast()->isActive = memStream->readFloat();
						programs[xp]->effects[zones].getLast()->wetDry = memStream->readFloat();
						programs[xp]->effects[zones].getLast()->currentProgram = memStream->readInt64();
					}
				}
				//
				programs[xp]->sequencerLayers[zones].clear();
				int xLayers = memStream->readInt64();
				//
				for (int xl = 0; xl < xLayers; xl++)
				{
					Header = memStream->readString();
					if (Header.compare("Sq" + String(xl) + ":") != 0) goto xerror;
					//
					programs[xp]->sequencerLayers[zones].add(new WusikSequencer(wavetables[zones]));
					programs[xp]->sequencerLayers[zones].getLast()->isStatic = memStream->readBool();
					programs[xp]->sequencerLayers[zones].getLast()->isBPM = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->mode = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->time1 = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->time2 = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->loopTo = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->layer_tune = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->layer_volume = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->layer_pan = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->layer_fine = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->layer_mute = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->layer_solo = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->layer_addMultiply = memStream->readFloat();
					programs[xp]->sequencerLayers[zones].getLast()->layer_pitchEnvelope = memStream->readFloat();
					//
					if (programs[xp]->sequencerLayers[zones].getLast()->layer_addMultiply == 1.0f)
						programs[xp]->sequencerLayers[zones].getLast()->layer_addMultiply = LAYER_MUL_BOOST1;
					//
					int xSteps = memStream->readInt64();
					programs[xp]->sequencerLayers[zones].getLast()->stepsSize = xSteps;
					//
					for (int xt = 0; xt < xSteps; xt++)
					{
						Header = memStream->readString();
						if (Header.compare("St" + String(xt) + ":") != 0) goto xerror;
						//
						programs[xp]->sequencerLayers[zones].getLast()->steps[xt].volume = memStream->readFloat();
						programs[xp]->sequencerLayers[zones].getLast()->steps[xt].pan = memStream->readFloat();
						programs[xp]->sequencerLayers[zones].getLast()->steps[xt].fine = memStream->readFloat();
						programs[xp]->sequencerLayers[zones].getLast()->steps[xt].waveform = memStream->readFloat();
						programs[xp]->sequencerLayers[zones].getLast()->steps[xt].tune = memStream->readFloat();
						programs[xp]->sequencerLayers[zones].getLast()->steps[xt].time = memStream->readFloat();
						programs[xp]->sequencerLayers[zones].getLast()->steps[xt].crossFade = memStream->readFloat();
						programs[xp]->sequencerLayers[zones].getLast()->steps[xt].filter = memStream->readFloat();
					}
				}
			}
			//
			if (!wholeBank) break;
		}
		//
		int xParameters = memStream->readInt64();
		for (int xp = 0; xp < xParameters; xp++)
		{
			((WusikZRParameter*)getParameters()[xp])->midiRemote = memStream->readInt();
		}
		//
		String Header = memStream->readString();
		if (Header.compare("END") != 0) goto xerror;
		valuesList = programs[currentProgram]->values[currentZone];
		//
		stateBuffer.reset();
	}
	else
	{
		MessageBox("Warning!", "Error Loading State Information");
	}
	//
	memStream = nullptr;
	//
	DEBUG_TO_FILE("Set State Information From Buffer Finished");
	//
	return;

xerror:
	MessageBox("Warning!", "Error Loading State Information");
	memStream = nullptr;
	DEBUG_TO_FILE("Set State Information From Buffer ! ERROR !");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::loadEffect(String desc, String name, String type, int program, int zone)
{
	if (type.compareIgnoreCase("PLUGIN") == 0)
	{
		programs[program]->effects[zone].add(new WEffectVST(desc, name, dataFiles, dataFiles->formatManager.get()));
	}
	else
	{
		programs[program]->effects[zone].add(new WEffectVSTdual(desc, name, dataFiles, dataFiles->formatManager.get()));
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WusikZrAudioProcessor();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AudioProcessorEditor* WusikZrAudioProcessor::createEditor()
{
	DEBUG_TO_FILE("Create Editor");
	//
	return new WusikZrAudioProcessorEditor(*this);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::redoMidiLearned()
{
	DEBUG_TO_FILE("Redo MIDI Learned");
	//
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	midiLearned.clearQuick(true);
	//
	ARRAY_Iterator(getParameters())
	{
		if (((WusikZRParameter*)getParameters()[index])->midiRemote != -1)
		{
			int theZone = ((WusikZRParameter*)getParameters()[index])->zone;
			midiLearned.add(new WMidiLearned(((WusikZRParameter*)getParameters()[index])->midiRemote, index, theZone));
		}
	}
	//
	DEBUG_TO_FILE("Redo MIDI Learned Finished");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::updateLayers()
{
	DEBUG_TO_FILE("Update Layers Start");
	//
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	//
	// Remove all references from parameters //
	ARRAY_Iterator(getParameters())
	{
		((WusikZRParameter*)getParameters()[index])->theValue = &dummyValue;
		((WusikZRParameter*)getParameters()[index])->time1 = nullptr;
		((WusikZRParameter*)getParameters()[index])->time2 = nullptr;
		((WusikZRParameter*)getParameters()[index])->isTimeBPM = nullptr;
		((WusikZRParameter*)getParameters()[index])->effectPlugin = nullptr;
		((WusikZRParameter*)getParameters()[index])->pluginParameterIndex = 0;
		((WusikZRParameter*)getParameters()[index])->sampleRate = lastSampleRate;
	}
	//
	for (int zones = 0; zones < 4; zones++)
	{
		// Add back Master/Global References //
		for (int xv = 0; xv < MAX_PARAMETERS; xv++)
		{
			((WusikZRParameter*)getParameters()[parametersMainStart[zones] + xv])->theValue = &programs[currentProgram]->values[zones][xv];
		}
		//
		ARRAY_Iterator(programs[currentProgram]->effects[zones])
		{
			if (programs[currentProgram]->effects[zones][index]->hasEffect())
			{
				((WusikZRParameter*)getParameters()[parametersEffectsStart[zones][index]])->theValue = &programs[currentProgram]->effects[zones][index]->isActive;
				((WusikZRParameter*)getParameters()[parametersEffectsStart[zones][index] + 1])->theValue = &programs[currentProgram]->effects[zones][index]->wetDry;
				//
				for (int xz = 0; xz < MAX_EFFECT_PARAMETERS; xz++)
				{
					if (xz >= programs[currentProgram]->effects[zones][index]->getNumParameters()) break;
					//
					WusikZRParameter* xPar = ((WusikZRParameter*)getParameters()[parametersEffectsStart[zones][index] + xz + 2]);
					xPar->effectPlugin = programs[currentProgram]->effects[zones][index];
					xPar->pluginParameterIndex = xz;
				}
			}
		}
		//
		ARRAY_Iterator(programs[currentProgram]->sequencerLayers[zones])
		{
			int parCounter = parametersLayerStart[zones][index];
			WusikSequencer* xSeq = programs[currentProgram]->sequencerLayers[zones][index];
			//
			WPARVALSEQ(layer_addMultiply);
			WPARVALSEQ(layer_mute);
			WPARVALSEQ(layer_solo);
			WPARVALSEQ(layer_volume);
			WPARVALSEQ(layer_pan);
			WPARVALSEQ(layer_tune);
			WPARVALSEQ(layer_fine);
			WPARVALSEQ(layer_pitchEnvelope);
			WPARVALSEQ(steps[0].waveform);
			//
			WPARVALSEQ(isBPM);
			WPARVALSEQ(time1);
			WPARVALSEQ(time2);
			//
			((WusikZRParameter*)getParameters()[parCounter - 2])->time1 = &xSeq->time1;
			((WusikZRParameter*)getParameters()[parCounter - 2])->time2 = &xSeq->time2;
			((WusikZRParameter*)getParameters()[parCounter - 2])->isTimeBPM = &xSeq->isBPM;
			//
			((WusikZRParameter*)getParameters()[parCounter - 1])->time1 = &xSeq->time1;
			((WusikZRParameter*)getParameters()[parCounter - 1])->time2 = &xSeq->time2;
			((WusikZRParameter*)getParameters()[parCounter - 1])->isTimeBPM = &xSeq->isBPM;
			//
			WPARVALSEQ(loopTo);
			WPARVALSEQ(mode);
			//
			for (int index2 = 0; index2 < xSeq->stepsSize; index2++)
			{
				WPARVALSEQ(steps[index2].volume);
				WPARVALSEQ(steps[index2].pan);
				WPARVALSEQ(steps[index2].tune);
				WPARVALSEQ(steps[index2].fine);
				WPARVALSEQ(steps[index2].filter);
				WPARVALSEQ(steps[index2].time);
				WPARVALSEQ(steps[index2].waveform);
				WPARVALSEQ(steps[index2].crossFade);
			}
		}
	}
	//
	valuesList = programs[currentProgram]->values[currentZone];
	//
	DEBUG_TO_FILE("Update Layers Part 2");
	//
	for (int zones = 0; zones < 4; zones++)
	{
		waveTable[zones]->wavetableObjects.clear();
		//
		ARRAY_Iterator(programs[currentProgram]->sequencerLayers[zones])
		{
			WusikSequencer* seqLayer = programs[currentProgram]->sequencerLayers[zones][index];
			//
			bool isAddingLayerSignal = true;
			float volumeBoost = 1.0f;
			if (seqLayer->layer_addMultiply == LAYER_ADD_BOOST2) volumeBoost = 2.0f;
			else if (seqLayer->layer_addMultiply == LAYER_ADD_BOOST4) volumeBoost = 4.0f;
			else if (seqLayer->layer_addMultiply == LAYER_MUL_BOOST1) { volumeBoost = 1.0f; isAddingLayerSignal = false; }
			else if (seqLayer->layer_addMultiply == LAYER_MUL_BOOST2) { volumeBoost = 2.0f; isAddingLayerSignal = false; }
			else if (seqLayer->layer_addMultiply == LAYER_MUL_BOOST4) { volumeBoost = 4.0f; isAddingLayerSignal = false; }
			//
			if (seqLayer->isStatic)
			{
				waveTable[zones]->wavetableObjects.add(new WusikWavetable::WusikWavetableObject(*waveTable[zones], wavetables[zones][0].wavetable, isAddingLayerSignal, &seqLayer->layer_pitchEnvelope, volumeBoost));
				seqLayer->setWavetableObjects(waveTable[zones]->wavetableObjects.getLast());
			}
			else
			{
				WusikWavetable::WusikWavetableObject* wave1 = waveTable[zones]->wavetableObjects.add(new WusikWavetable::WusikWavetableObject(*waveTable[zones], wavetables[zones][0].wavetable, isAddingLayerSignal, &seqLayer->layer_pitchEnvelope, volumeBoost));
				waveTable[zones]->wavetableObjects.add(new WusikWavetable::WusikWavetableObject(*waveTable[zones], wavetables[zones][0].wavetable, isAddingLayerSignal, &seqLayer->layer_pitchEnvelope, volumeBoost));
				seqLayer->setWavetableObjects(wave1, waveTable[zones]->wavetableObjects.getLast());
			}
		}
	}
	//
	DEBUG_TO_FILE("Update Layers Load Banks");
	//
	loadWaveBanks();
	//
	DEBUG_TO_FILE("Update Layers Finish");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::loadAllEffects()
{
	DEBUG_TO_FILE("Load All Effects Started");
	//
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	//
	for (int xp = 0; xp < MAX_PROGRAMS; xp++)
	{
		for (int zones = 0; zones < MAX_ZONES; zones++)
		{
			ARRAY_Iterator(programs[xp]->effects[zones])
			{
				if (xp == currentProgram)
				{
					programs[xp]->effects[zones][index]->loadEffect(lastSampleRate, playHead);
					//
					if (programs[xp]->effects[zones][index]->hasEffect())
					{
						if (programs[xp]->effects[zones][index]->stateInformation.getSize() > 0)
						{
							programs[xp]->effects[zones][index]->setStateInformation(programs[xp]->effects[zones][index]->stateInformation.getData(), programs[xp]->effects[zones][index]->stateInformation.getSize());
							programs[xp]->effects[zones][index]->setCurrentProgram(programs[xp]->effects[zones][index]->currentProgram);
						}
					}
				}
				else
				{
					programs[xp]->effects[zones][index]->releaseEffect();
				}
			}
		}
	}
	//
	DEBUG_TO_FILE("Load All Effects Finished");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessor::newAction(int action, int actionValue1, int actionValue2)
{
	if (dataFiles->dataFilesError.isNotEmpty()) return;
	//
	if (hasPrepareToPlay.get() == 1 && hasStartedAnyProcessing.get() == 1)
	{
		int xCounter = 0;
		while (processingThread.isThreadRunning())
		{
			Thread::sleep(10);
			xCounter++;
			if (xCounter > 400) break;
		}
	}
	//
	processingThread.actionValue[0] = actionValue1;
	processingThread.actionValue[1] = actionValue2;
	processingThread.action.set(action);
	//
	if (hasPrepareToPlay.get() == 1 && hasStartedAnyProcessing.get() == 1) processingThread.startThread(); else processingThread.run();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WZRThread::run()
{
	DEBUG_TO_FILE("Action Thread Start");
	//
	if (owner->dataFiles->dataFilesError.isNotEmpty()) return;
	//
	if (owner->hasPrepareToPlay.get() == 1 && owner->hasStartedAnyProcessing.get() == 1)
	{
		// First Fade Out Current Audio //
		owner->fadeOutCounter = 1.0f;
		owner->fadeOutRate = -1.0f / (owner->lastSampleRate / 4.0f);
		//
		int xCounter = 0;
		while (owner->fadeOutCounter > 0.0f) 
		{ 
			sleep(10); 
			xCounter++;
			if (xCounter > 200) break;
		}
	}
	//
	owner->suspendProcessing(true);
	owner->waveformWindow = nullptr;
	//
	if (owner->hasPrepareToPlay.get() == 1 && owner->hasStartedAnyProcessing.get() == 1)
	{
		int xCounter = 0;
		while (!owner->isSuspended()) 
		{ 
			sleep(10); 
			xCounter++;
			if (xCounter > 400) break;
		}
		//
		xCounter = 0;
		while (owner->isProcessingSomething.get() == 1) 
		{
			sleep(10);
			xCounter++;
			if (xCounter > 400) break;
		}
	}
	//
	Point<int> previousViewPortPosition;
	//
	if (owner->getActiveEditor() != nullptr)
	{
		previousViewPortPosition = ((WusikZrAudioProcessorEditor*)owner->getActiveEditor())->layersViewPort[owner->currentZone]->getViewPosition();
		//
		((WusikZrAudioProcessorEditor*)owner->getActiveEditor())->clearInterfaceObjects(0);
		((WusikZrAudioProcessorEditor*)owner->getActiveEditor())->clearInterfaceObjects(1);
		((WusikZrAudioProcessorEditor*)owner->getActiveEditor())->clearInterfaceObjects(2);
		((WusikZrAudioProcessorEditor*)owner->getActiveEditor())->clearInterfaceObjects(3);
	}
	//
	if (action.get() == kActionSetState)
	{
		owner->setStateInformationFromBuffer();
	}
	//
	else if (action.get() == kActionUnloadEffect)
	{
		owner->programs[owner->getCurrentProgram()]->effects[owner->currentZone].remove(actionValue[0]);
	}
	//
	else if (action.get() == kActionMoveEffect)
	{
		if (actionValue[1] == 1) owner->programs[owner->getCurrentProgram()]->effects[owner->currentZone].swap(actionValue[0], actionValue[0] + 1);
		if (actionValue[1] == -1) owner->programs[owner->getCurrentProgram()]->effects[owner->currentZone].swap(actionValue[0], actionValue[0] - 1);
		if (actionValue[1] == 99) owner->programs[owner->getCurrentProgram()]->effects[owner->currentZone].swap(actionValue[0], 0);
	}
	//
	else if (action.get() == kActionChangeProgram)
	{
		owner->checkAndSaveEffectData(true);
		owner->currentProgram = actionValue[0];
	}
	//
	else if (action.get() == kActionRemoveLayer)
	{
		owner->programs[owner->getCurrentProgram()]->sequencerLayers[owner->currentZone].remove(actionValue[0]);
	}
	else if (action.get() == kActionRemoveStep)
	{
		if (actionValue[1] != 127)
		{
			for (int xs = actionValue[1]; xs < 127; xs++)
			{
				owner->programs[owner->getCurrentProgram()]->sequencerLayers[owner->currentZone][actionValue[0]]->steps[xs] =
					owner->programs[owner->getCurrentProgram()]->sequencerLayers[owner->currentZone][actionValue[0]]->steps[xs + 1];
			}
		}
		owner->programs[owner->getCurrentProgram()]->sequencerLayers[owner->currentZone][actionValue[0]]->stepsSize--;
	}
	//
	else if (action.get() == kActionLoadNewEffect)
	{
		owner->loadEffect(owner->loadNewEffectDescription, owner->loadNewEffectName, owner->loadNewEffectType, owner->getCurrentProgram(), owner->currentZone);
	}
	//
	else if (action.get() == kActionAddNewStep)
	{
		owner->programs[owner->getCurrentProgram()]->sequencerLayers[owner->currentZone][actionValue[0]]->stepsSize++;
		owner->programs[owner->getCurrentProgram()]->sequencerLayers[owner->currentZone][actionValue[0]]->steps[owner->programs[owner->getCurrentProgram()]->sequencerLayers[owner->currentZone][actionValue[0]]->stepsSize - 1].reset();
	}
	//
	else if (action.get() == kActionAddNewLayer)
	{
		if (actionValue[0] == kAddStaticLayer)
		{
			owner->programs[owner->currentProgram]->sequencerLayers[owner->currentZone].add(new WusikSequencer(owner->wavetables[owner->currentZone]));
		}
		else
		{
			owner->programs[owner->currentProgram]->sequencerLayers[owner->currentZone].add(new WusikSequencer(owner->wavetables[owner->currentZone]));
			owner->programs[owner->currentProgram]->sequencerLayers[owner->currentZone].getLast()->isStatic = false;
		}
	}
	//
	// ===================================================== //
	//
	owner->loadAllEffects();
	//
	if (owner->hasPrepareToPlay.get() == 1 && owner->hasStartedAnyProcessing.get() == 1)
	{
		MessageManagerLock lock;
		owner->updateLayers();
		owner->redoMidiLearned();
		if (owner->getActiveEditor() != nullptr)
		{
			((WusikZrAudioProcessorEditor*)owner->getActiveEditor())->updateEntireInterface();
			if (previousViewPortPosition.getY() != 0) ((WusikZrAudioProcessorEditor*)owner->getActiveEditor())->layersViewPort[owner->currentZone]->setViewPosition(previousViewPortPosition);
			((WusikZrAudioProcessorEditor*)owner->getActiveEditor())->cachedImageButton->setVisible(false);
		}
	}
	else
	{
		owner->updateLayers();
		owner->redoMidiLearned();
		if (owner->getActiveEditor() != nullptr)
		{
			((WusikZrAudioProcessorEditor*)owner->getActiveEditor())->updateEntireInterface();
			if (previousViewPortPosition.getY() != 0) ((WusikZrAudioProcessorEditor*)owner->getActiveEditor())->layersViewPort[owner->currentZone]->setViewPosition(previousViewPortPosition);
			((WusikZrAudioProcessorEditor*)owner->getActiveEditor())->cachedImageButton->setVisible(false);
		}
	}
	//
	owner->fadeOutRate = 1.0f / (owner->lastSampleRate / 8.0f);
	owner->fadeOutCounter = 0.0f;
	//
	action.set(kActionNone);
	owner->suspendProcessing(false);
	//
	DEBUG_TO_FILE("Action Thread Finished");
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
void WusikZrAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	DEBUG_TO_FILE("Process Block Started - Channels: " + String(buffer.getNumChannels()) + " - Samples: " + String(buffer.getNumSamples()));
	//
	isProcessingSomething.set(1);
	hasStartedAnyProcessing.set(1);
	//
	ScopedNoDenormals noDenormals;
	buffer.clear();
	if (dataFiles->processType == processDisabled || dataFiles->dataFilesError.isNotEmpty() || buffer.getNumSamples() == 0 || buffer.getNumChannels() != 2)
	{
		isProcessingSomething.set(0);
		return;
	}
	//
	if (waveTable[0]->wavetableObjects.size() == 0 &&
		waveTable[1]->wavetableObjects.size() == 0 &&
		waveTable[2]->wavetableObjects.size() == 0 &&
		waveTable[3]->wavetableObjects.size() == 0)
	{
		isProcessingSomething.set(0);
		return;
	}
	//
	DEBUG_TO_FILE("Process Block Setup Zones");
	//
	for (int zones = 0; zones < 4; zones++)
	{
		limiterBuffer[zones].setSize(2, buffer.getNumSamples(), false, false, true);
		limiterBuffer[zones].clear();
		//
		if (waveTable[zones]->wavetableObjects.size() > 0)
		{
			valuesList = programs[currentProgram]->values[zones];
			//
			if (prevOctave[zones] != valuesList[kOctave])
			{
				prevOctave[zones] = valuesList[kOctave];
				ADSREnvelope_Amplitude[zones].reset();
			}
			//
			if (prevTune[zones] != valuesList[kTune])
			{
				prevTune[zones] = valuesList[kTune];
				ADSREnvelope_Amplitude[zones].reset();
			}
			//
			ADSREnvelope_Amplitude[zones].changeMaxVoices(valuesList[kMode]);
			ADSREnvelope_Amplitude[zones].startProcess(&valuesList[kADSR_Velocity], &valuesList[kADSR_Curve], &valuesList[kADSR_Clip], &valuesList[kADSR_Attack], &valuesList[kADSR_Decay], &valuesList[kADSR_Sustain], &valuesList[kADSR_Release], float(getSampleRate()), &valuesList[kUnison], valuesList[kADSR_Mode]);
			//
			ADSREnvelope_Filter[zones].changeMaxVoices(valuesList[kMode]);
			ADSREnvelope_Filter[zones].startProcess(&valuesList[kFilter_ADSR_Velocity], &valuesList[kFilter_ADSR_Curve], &valuesList[kFilter_ADSR_Clip], &valuesList[kFilter_ADSR_Attack], &valuesList[kFilter_ADSR_Decay], &valuesList[kFilter_ADSR_Sustain], &valuesList[kFilter_ADSR_Release], float(getSampleRate()), &valuesList[kUnison], valuesList[kFilter_ADSR_Mode]);
			//
			ADSREnvelope_Pitch[zones].changeMaxVoices(valuesList[kMode]);
			ADSREnvelope_Pitch[zones].startProcess(&valuesList[kPitch_ADSR_Velocity], &valuesList[kPitch_ADSR_Curve], &valuesList[kPitch_ADSR_Clip], &valuesList[kPitch_ADSR_Attack], &valuesList[kPitch_ADSR_Decay], &valuesList[kPitch_ADSR_Sustain], &valuesList[kPitch_ADSR_Release], float(getSampleRate()), &valuesList[kUnison], valuesList[kPitch_ADSR_Mode]);
			//
			waveTable[zones]->startProcess(getSampleRate(), &valuesList[kFine], &valuesList[kPitchBend], &pitchBendValue, valuesList[kQuality], ADSREnvelope_Amplitude[zones].maxVoices, &valuesList[kGlide]);
			//
			ARRAY_Iterator(programs[currentProgram]->sequencerLayers[zones])
			{
				float theValue = programs[currentProgram]->sequencerLayers[zones][index]->layer_addMultiply;
				bool isAddingLayerSignal = true;
				float volumeBoost = 1.0f;
				if (theValue == LAYER_ADD_BOOST2) volumeBoost = 2.0f;
				else if (theValue == LAYER_ADD_BOOST4) volumeBoost = 4.0f;
				else if (theValue == LAYER_MUL_BOOST1) { volumeBoost = 1.0f; isAddingLayerSignal = false; }
				else if (theValue == LAYER_MUL_BOOST2) { volumeBoost = 2.0f; isAddingLayerSignal = false; }
				else if (theValue == LAYER_MUL_BOOST4) { volumeBoost = 4.0f; isAddingLayerSignal = false; }
				//
				if (programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[0] != nullptr)
				{
					programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[0]->addThisLayerSignal = isAddingLayerSignal;
					programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[0]->boostVolume = volumeBoost;
				}
				//
				if (programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[1] != nullptr)
				{
					programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[1]->addThisLayerSignal = isAddingLayerSignal;
					programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[1]->boostVolume = volumeBoost;
				}
			}
			//
			for (int xf = 0; xf < 2; xf++)
			{
				theFilter[zones][xf].setLimit(valuesList[kFilter_Limit]);
				theFilter[zones][xf].setSmoothfrequency(valuesList[kFilter_Smooth]);
				theFilter[zones][xf].setRezonance(valuesList[kFilter_Rezonance], ADSREnvelope_Amplitude[zones].maxVoices);
				//
				int filterType = int(valuesList[kFilter_Mode] * 7.0f);
				if (filterType >= 4)
				{
					theFilter[zones][xf].setPole(1);
					theFilter[zones][xf].setType(filterType - 4);
				}
				else
				{
					theFilter[zones][xf].setPole(0);
					theFilter[zones][xf].setType(filterType);
				}
			}
		}
	}
	//
	AudioPlayHead::CurrentPositionInfo curPosInfo;
	if (getPlayHead() != nullptr && getPlayHead()->getCurrentPosition(curPosInfo)) globalBPM = curPosInfo.bpm;
	//
	MidiBuffer::Iterator midiIterator(midiMessages);
	for (int xsample = 0; xsample < buffer.getNumSamples(); xsample++)
	{
		#if DEBUG_TOFILE
			if (xsample == 0) { DEBUG_TO_FILE("Process Block Process MIDI Notes"); }
		#endif
		//
		// Process MIDI Notes and other MIDI stuff //
		midiIterator.setNextSamplePosition(xsample);
		while (midiIterator.getNextEvent(midiMessage, sampPos) && sampPos == xsample)
		{
			for (int zones = 0; zones < 4; zones++)
			{
				if (waveTable[zones]->wavetableObjects.size() > 0)
				{
					valuesList = programs[currentProgram]->values[zones];
					int midiChannel = int(valuesList[kMIDICHInput] * 16.0f);
					//
					if (midiChannel == 0 || midiMessage.getChannel() == midiChannel)
					{
						MidiMessage copyMessage = midiMessage;
						//
						if (copyMessage.isNoteOnOrOff())
						{
							int note = copyMessage.getNoteNumber() + int((valuesList[kOctave] * 6.0f * 24.0f) - (6.0f * 12.0f));
							note += int((valuesList[kTune] * 24.0f) - 12.0f);
							copyMessage.setNoteNumber(jlimit(0, 127, note));
						}
						//
						if (copyMessage.isNoteOn())
						{
							if (ADSREnvelope_Amplitude[zones].maxVoices == 1 || *ADSREnvelope_Amplitude[zones].unison > 0.0f)
							{
								if (ADSREnvelope_Amplitude[zones].holdingNotes.size() == 0 && (
									(ADSREnvelope_Amplitude[zones].isLooped == 0.0f && ADSREnvelope_Amplitude[zones].position[0] == envRelease && ADSREnvelope_Amplitude[zones].value[0] > 0.0f
									|| (ADSREnvelope_Amplitude[zones].isLooped != 0.0f && ADSREnvelope_Amplitude[zones].loopedRelease[0] != 0.0f))))
								{
									if (*ADSREnvelope_Amplitude[zones].unison > 0.0f)
									{
										for (int xmv = 0; xmv < ADSREnvelope_Amplitude[zones].maxVoices; xmv++)
										{
											ADSREnvelope_Amplitude[zones].endVoiceMonoSteal(xmv);
										}
									}
									else ADSREnvelope_Amplitude[zones].endVoiceMonoSteal(0);
									//
									retriggerMonoNote[zones] = true;
									retriggerMonoNoteMessage[zones] = copyMessage;
								}
								else
								{
									int xVoice = ADSREnvelope_Amplitude[zones].noteOn(copyMessage);
									ADSREnvelope_Filter[zones].noteOn(copyMessage, xVoice);
									ADSREnvelope_Pitch[zones].noteOn(copyMessage, xVoice);
								}
							}
							else
							{
								int xVoice = ADSREnvelope_Amplitude[zones].noteOn(copyMessage);
								ADSREnvelope_Filter[zones].noteOn(copyMessage, xVoice);
								ADSREnvelope_Pitch[zones].noteOn(copyMessage, xVoice);
							}
						}
						else if (copyMessage.isNoteOff())
						{
							ADSREnvelope_Amplitude[zones].noteOff(copyMessage);
							ADSREnvelope_Filter[zones].noteOff(copyMessage);
							ADSREnvelope_Pitch[zones].noteOff(copyMessage);
						}
						else if (copyMessage.isAllNotesOff())
						{
							ADSREnvelope_Amplitude[zones].reset();
							ADSREnvelope_Filter[zones].reset();
							ADSREnvelope_Pitch[zones].reset();
						}
						else if (copyMessage.isController())
						{
							if (midiLearn == -1)
							{
								ARRAY_Iterator(midiLearned)
								{
									if (midiLearned[index]->midiRemote == copyMessage.getControllerNumber())
									{
										((WusikZRParameter*)getParameters()[midiLearned[index]->parameterIndex])->setValue(float(copyMessage.getControllerValue()) / 127.0f);
									}
								}
							}
							else
							{
								((WusikZRParameter*)getParameters()[midiLearn])->midiRemote = copyMessage.getControllerNumber();
								((WusikZRParameter*)getParameters()[midiLearn])->setValue(float(copyMessage.getControllerValue()) / 127.0f);
								midiLearn = -1;
								redoMidiLearned();
							}
						}
						else if (copyMessage.isAftertouch())
						{
							if (midiLearn == -1)
							{
								ARRAY_Iterator(midiLearned)
								{
									if (midiLearned[index]->midiRemote == WMidiLearned::kMIDIRemoteATouch)
									{
										((WusikZRParameter*)getParameters()[midiLearned[index]->parameterIndex])->setValue(float(copyMessage.getAfterTouchValue()) / 127.0f);
									}
								}
							}
							else
							{
								((WusikZRParameter*)getParameters()[midiLearn])->midiRemote = WMidiLearned::kMIDIRemoteATouch;
								((WusikZRParameter*)getParameters()[midiLearn])->setValue(float(copyMessage.getAfterTouchValue()) / 127.0f);
								midiLearn = -1;
								redoMidiLearned();
							}
						}
						else if (copyMessage.isPitchWheel())
						{
							if (midiLearn == -1)
							{
								ARRAY_Iterator(midiLearned)
								{
									if (midiLearned[index]->midiRemote == WMidiLearned::kMIDIRemotePBend)
									{
										((WusikZRParameter*)getParameters()[midiLearned[index]->parameterIndex])->setValue(float(copyMessage.getPitchWheelValue()) / 16383.0f);
									}
								}
							}
							else
							{
								((WusikZRParameter*)getParameters()[midiLearn])->midiRemote = WMidiLearned::kMIDIRemotePBend;
								((WusikZRParameter*)getParameters()[midiLearn])->setValue(float(copyMessage.getPitchWheelValue()) / 16383.0f);
								midiLearn = -1;
								redoMidiLearned();
							}
							//
							pitchBendTarget = float(int32(jlimit(-8191, 8191, copyMessage.getPitchWheelValue() - 8192) * (127.0 / 8191.0))) / 127.0f;
							pitchBendRate = fabs(pitchBendValue - pitchBendTarget) / (getSampleRate() * 0.08);
						}
					}
				}
			}
		}
		//
		// ------------------------------------------------------------------------------------ //
		//
		for (int zones = 0; zones < 4; zones++)
		{
			if (waveTable[zones]->wavetableObjects.size() > 0)
			{
				if ((ADSREnvelope_Amplitude[zones].maxVoices == 1 || *ADSREnvelope_Amplitude[zones].unison > 0.0f) && retriggerMonoNote[zones])
				{
					if ((ADSREnvelope_Amplitude[zones].isLooped == 0.0f && ADSREnvelope_Amplitude[zones].position[0] == envRelease && ADSREnvelope_Amplitude[zones].value[0] == 0.0f
						|| (ADSREnvelope_Amplitude[zones].isLooped != 0.0f && ADSREnvelope_Amplitude[zones].loopedRelease[0] == 0.0f)))
					{
						int xVoice = ADSREnvelope_Amplitude[zones].noteOn(retriggerMonoNoteMessage[zones]);
						ADSREnvelope_Filter[zones].noteOn(retriggerMonoNoteMessage[zones], xVoice);
						ADSREnvelope_Pitch[zones].noteOn(retriggerMonoNoteMessage[zones], xVoice);
						//
						retriggerMonoNote[zones] = false;
					}
				}
			}
		}
		//
		// ------------------------------------------------------------------------------------ //
		//
		// Process Pitch Bend //
		//
		#if DEBUG_TOFILE
			if (xsample == 0) { DEBUG_TO_FILE("Process Block Pitch Bend"); }
		#endif
		//
		if (pitchBendTarget != pitchBendValue)
		{
			if (pitchBendValue < pitchBendTarget)
			{
				pitchBendValue += pitchBendRate;
				if (pitchBendValue >= pitchBendTarget)
				{
					pitchBendValue = pitchBendTarget;
				}
			}
			else
			{
				pitchBendValue -= pitchBendRate;
				if (pitchBendValue <= pitchBendTarget)
				{
					pitchBendValue = pitchBendTarget;
				}
			}
		}
		//
		static String showThis;
		//
		// Process Sequencers (Static or Not) //
		//
		#if DEBUG_TOFILE
			if (xsample == 0) { DEBUG_TO_FILE("Process Block Sequencers"); }
		#endif
		//
		for (int zones = 0; zones < 4; zones++)
		{
			if (waveTable[zones]->wavetableObjects.size() > 0)
			{
				valuesList = programs[currentProgram]->values[zones];
				//
				unisonVolume = 1.0f;
				unisonPan = unisonFine = 0.5f;
				unisonPosition = 0.0f;
				//
				ARRAY_Iterator(ADSREnvelope_Amplitude[zones].midiMessagesPlay)
				{
					ARRAY__Iterator(programs[currentProgram]->sequencerLayers[zones], xSeq)
					{
						if (ADSREnvelope_Amplitude[zones].midiMessagesPlay[index].isNoteOnOrOff())
						{
							if (valuesList[kUnison] > 0.0f)
							{
								unisonVolume -= unisonVolumeRate;
								unisonPan += unisonPanRate;
								unisonPosition += unisonPositionRate;
								unisonFine += unisonFineRate;
							}
							//
							programs[currentProgram]->sequencerLayers[zones][xSeq]->startSequence(ADSREnvelope_Amplitude[zones].midiMessagesPlay[index], unisonFine, unisonVolume, unisonPan,
								unisonPosition,
								(isFirstNote || (ADSREnvelope_Amplitude[zones].maxVoices > 1 && valuesList[kUnison] == 0.0f)),
								(valuesList[kUnison] > 0.0f));
							//
							if ((ADSREnvelope_Amplitude[zones].maxVoices == 1 || valuesList[kUnison] > 0.0f) && isFirstNote && valuesList[kGlideAuto] > 0.0f)
									programs[currentProgram]->sequencerLayers[zones][xSeq]->wavetableObjects[0]->copyGlideRate(int(ADSREnvelope_Amplitude[zones].midiMessagesPlay[index].getTimeStamp()));
						}
						else if (ADSREnvelope_Amplitude[zones].midiMessagesPlay[index].isController() && ADSREnvelope_Amplitude[zones].midiMessagesPlay[index].getControllerNumber() == 100)
						{
							if (ADSREnvelope_Amplitude[zones].midiMessagesPlay[index].getControllerValue() == kEnv_FirstNote)
							{
								isFirstNote = true;
							}
							else if (ADSREnvelope_Amplitude[zones].midiMessagesPlay[index].getControllerValue() == kEnv_NotFirstNote)
							{
								isFirstNote = false;
							}
							else if (ADSREnvelope_Amplitude[zones].midiMessagesPlay[index].getControllerValue() == kEnv_Reset_Unison)
							{
								float xUniVal = valuesList[kUnison] * 0.9f;
								//
								unisonVolume = 1.0f;
								unisonPosition = 0.0f;
								unisonFine = xUniVal - (xUniVal * 0.5f);
								unisonPan = 0.2f;
								//
								unisonVolumeRate = 0.5f / float(ADSREnvelope_Amplitude[zones].maxVoices);
								unisonPositionRate = 0.8f / float(ADSREnvelope_Amplitude[zones].maxVoices);
								unisonFineRate = xUniVal / float(ADSREnvelope_Amplitude[zones].maxVoices);
								unisonPanRate = 0.8f / float(ADSREnvelope_Amplitude[zones].maxVoices);
							}
						}
					}
				}
				ADSREnvelope_Amplitude[zones].midiMessagesPlay.clear();
				ADSREnvelope_Filter[zones].midiMessagesPlay.clear();
				ADSREnvelope_Pitch[zones].midiMessagesPlay.clear();
				//
				//  Process All Sequencers //
				zeromem(sequencedFilter, sizeof(sequencedFilter));
				//
				ARRAY_Iterator(programs[currentProgram]->sequencerLayers[zones])
				{
					programs[currentProgram]->sequencerLayers[zones][index]->processNext(ADSREnvelope_Amplitude[zones].maxVoices, globalBPM);
					//
					if (!programs[currentProgram]->sequencerLayers[zones][index]->isStatic)
					{
						for (int xv = 0; xv < ADSREnvelope_Amplitude[zones].maxVoices; xv++)
						{
							sequencedFilter[xv] += (programs[currentProgram]->sequencerLayers[zones][index]->steps[programs[currentProgram]->sequencerLayers[zones][index]->currentStep[xv]].filter * 2.0f) - 1.0f;
						}
					}
				}
				//
				if (!ADSREnvelope_Amplitude[zones].isIdle) extraIdleCounter[zones] = 0;
				else if (extraIdleCounter[zones] < 999999) extraIdleCounter[zones]++;
				//
				if (!ADSREnvelope_Amplitude[zones].isIdle || extraIdleCounter[zones] < 199999)
				{
					if (dataFiles->processType == processSSE2)
					{
						// ------------------------------------------------------------------------------------ //
						// ------------ SSE ------------------------------------------------------------------- //
						// ------------------------------------------------------------------------------------ //
						//
						ADSREnvelope_Amplitude[zones].processVoices<wSSE>(4);
						ADSREnvelope_Filter[zones].processVoices<wSSE>(4);
						ADSREnvelope_Pitch[zones].processVoices<wSSE>(4);
						//
						ADSREnvelope_Amplitude[zones].checkStealingVoices();
						ADSREnvelope_Filter[zones].checkStealingVoices();
						ADSREnvelope_Pitch[zones].checkStealingVoices();
						//
						ADSREnvelope_Amplitude[zones].checkIdle(4);
						//
						for (int xvoice = 0; xvoice < ADSREnvelope_Amplitude[zones].maxVoices; xvoice += 4)
						{
							for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
							{
								_mm_store_ps(allVoicesOutput[kRight][ov] + xvoice, _mm_setzero_ps());
								_mm_store_ps(allVoicesOutput[kLeft][ov] + xvoice, _mm_setzero_ps());
							}
						}
						//
						ARRAY_Iterator(programs[currentProgram]->sequencerLayers[zones])
						{
							if (programs[currentProgram]->sequencerLayers[zones][index]->isStatic)
							{
								WusikWavetable::WusikWavetableObject* waveT = programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[0];
								//
								waveT->updateRate(ADSREnvelope_Pitch[zones].output);
								waveT->processVoices_SSE();
								//
								for (int xvoice = 0; xvoice < ADSREnvelope_Amplitude[zones].maxVoices; xvoice += 4)
								{
									if (waveT->addThisLayerSignal)
									{
										for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
										{
											_mm_store_ps(allVoicesOutput[kLeft][ov] + xvoice, _mm_add_ps(_mm_load_ps(allVoicesOutput[kLeft][ov] + xvoice), _mm_mul_ps(_mm_set1_ps(waveT->boostVolume), _mm_load_ps(waveT->output[kLeft][ov] + xvoice))));
											_mm_store_ps(allVoicesOutput[kRight][ov] + xvoice, _mm_add_ps(_mm_load_ps(allVoicesOutput[kRight][ov] + xvoice), _mm_mul_ps(_mm_set1_ps(waveT->boostVolume), _mm_load_ps(waveT->output[kRight][ov] + xvoice))));
										}
									}
									else
									{
										for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
										{
											_mm_store_ps(allVoicesOutput[kLeft][ov] + xvoice, _mm_mul_ps(_mm_load_ps(allVoicesOutput[kLeft][ov] + xvoice), _mm_mul_ps(_mm_set1_ps(waveT->boostVolume), _mm_load_ps(waveT->output[kLeft][ov] + xvoice))));
											_mm_store_ps(allVoicesOutput[kRight][ov] + xvoice, _mm_mul_ps(_mm_load_ps(allVoicesOutput[kRight][ov] + xvoice), _mm_mul_ps(_mm_set1_ps(waveT->boostVolume), _mm_load_ps(waveT->output[kRight][ov] + xvoice))));
										}
									}
								}
							}
							else
							{
								WusikWavetable::WusikWavetableObject* waveT[2] = { programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[0], programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[1] };
								//
								waveT[0]->updateRate(ADSREnvelope_Pitch[zones].output);
								waveT[0]->processVoices_SSE();
								//
								waveT[1]->updateRate(ADSREnvelope_Pitch[zones].output);
								waveT[1]->processVoices_SSE();
								//
								for (int xvoice = 0; xvoice < ADSREnvelope_Amplitude[zones].maxVoices; xvoice += 4)
								{
									if (waveT[0]->addThisLayerSignal)
									{
										for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
										{
											_mm_store_ps(allVoicesOutput[kLeft][ov] + xvoice, _mm_add_ps(_mm_load_ps(allVoicesOutput[kLeft][ov] + xvoice), _mm_mul_ps(_mm_set1_ps(waveT[0]->boostVolume), _mm_add_ps(_mm_load_ps(waveT[0]->output[kLeft][ov] + xvoice), _mm_load_ps(waveT[1]->output[kLeft][ov] + xvoice)))));
											_mm_store_ps(allVoicesOutput[kRight][ov] + xvoice, _mm_add_ps(_mm_load_ps(allVoicesOutput[kRight][ov] + xvoice), _mm_mul_ps(_mm_set1_ps(waveT[0]->boostVolume), _mm_add_ps(_mm_load_ps(waveT[0]->output[kRight][ov] + xvoice), _mm_load_ps(waveT[1]->output[kRight][ov] + xvoice)))));
										}
									}
									else
									{
										for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
										{
											_mm_store_ps(allVoicesOutput[kLeft][ov] + xvoice, _mm_mul_ps(_mm_load_ps(allVoicesOutput[kLeft][ov] + xvoice), _mm_mul_ps(_mm_set1_ps(waveT[0]->boostVolume), _mm_add_ps(_mm_load_ps(waveT[0]->output[kLeft][ov] + xvoice), _mm_load_ps(waveT[1]->output[kLeft][ov] + xvoice)))));
											_mm_store_ps(allVoicesOutput[kRight][ov] + xvoice, _mm_mul_ps(_mm_load_ps(allVoicesOutput[kRight][ov] + xvoice), _mm_mul_ps(_mm_set1_ps(waveT[0]->boostVolume), _mm_add_ps(_mm_load_ps(waveT[0]->output[kRight][ov] + xvoice), _mm_load_ps(waveT[1]->output[kRight][ov] + xvoice)))));
										}
									}
								}
							}
						}
						//
						// Down Sample //
						for (int xvoice = 0; xvoice < ADSREnvelope_Amplitude[zones].maxVoices; xvoice += 4)
						{
							for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
							{
								waveTable[zones]->wavetableObjects.getFirst()->downSample<wSSE>(xvoice, allVoicesOutput[kLeft][ov], allVoicesOutput[kRight][ov]);
							}
							//
							_mm_store_ps(filterProcessSignal[kLeft] + xvoice, _mm_load_ps(allVoicesOutput[kLeft][waveTable[zones]->overSample - 1] + xvoice));
							_mm_store_ps(filterProcessSignal[kRight] + xvoice, _mm_load_ps(allVoicesOutput[kRight][waveTable[zones]->overSample - 1] + xvoice));
						}
						//
						// Filter Processing //
						for (int xvoice = 0; xvoice < ADSREnvelope_Amplitude[zones].maxVoices; xvoice += 4)
						{
							for (int xv = 0; xv < 4; xv++)
							{
								float ffreq = jlimit(0.0f, 1.0f, valuesList[kFilter_Frequency] + sequencedFilter[xvoice + xv] + (((valuesList[kFilter_Envelope] * 2.0f) - 1.0f) * ADSREnvelope_Filter[zones].output[xvoice + xv]));
								theFilter[zones][kLeft].setFrequency(ffreq, xvoice + xv);
								theFilter[zones][kRight].setFrequency(ffreq, xvoice + xv);
								//
								if (ADSREnvelope_Amplitude[zones].resetFilter[xvoice + xv])
								{
									ADSREnvelope_Amplitude[zones].resetFilter[xvoice + xv] = false;
									theFilter[zones][kLeft].reset(xvoice + xv);
									theFilter[zones][kRight].reset(xvoice + xv);
								}
							}
							//
							theFilter[zones][kLeft].process_SSE(filterProcessSignal[kLeft], xvoice);
							theFilter[zones][kRight].process_SSE(filterProcessSignal[kRight], xvoice);
						}
					}
					else
					{
						// ------------------------------------------------------------------------------------ //
						// ------------ AVX ------------------------------------------------------------------- //
						// ------------------------------------------------------------------------------------ //
						//
						#if JUCE_WINDOWS
						ADSREnvelope_Amplitude[zones].processVoices<wAVXFMA3>(8);
						ADSREnvelope_Filter[zones].processVoices<wAVXFMA3>(8);
						ADSREnvelope_Pitch[zones].processVoices<wAVXFMA3>(8);
						//
						ADSREnvelope_Amplitude[zones].checkStealingVoices();
						ADSREnvelope_Filter[zones].checkStealingVoices();
						ADSREnvelope_Pitch[zones].checkStealingVoices();
						//
						ADSREnvelope_Amplitude[zones].checkIdle(8);
						//
						for (int xvoice = 0; xvoice < ADSREnvelope_Amplitude[zones].maxVoices; xvoice += 8)
						{
							for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
							{
								_mm256_store_ps(allVoicesOutput[kRight][ov] + xvoice, _mm256_setzero_ps());
								_mm256_store_ps(allVoicesOutput[kLeft][ov] + xvoice, _mm256_setzero_ps());
							}
						}
						//
						ARRAY_Iterator(programs[currentProgram]->sequencerLayers[zones])
						{
							if (programs[currentProgram]->sequencerLayers[zones][index]->isStatic)
							{
								WusikWavetable::WusikWavetableObject* waveT = programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[0];
								//
								waveT->updateRate(ADSREnvelope_Pitch[zones].output);
								waveT->processVoices_AVX();
								//
								for (int xvoice = 0; xvoice < ADSREnvelope_Amplitude[zones].maxVoices; xvoice += 8)
								{
									if (waveT->addThisLayerSignal)
									{
										for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
										{
											_mm256_store_ps(allVoicesOutput[kLeft][ov] + xvoice, _mm256_add_ps(_mm256_load_ps(allVoicesOutput[kLeft][ov] + xvoice), _mm256_mul_ps(_mm256_set1_ps(waveT->boostVolume), _mm256_load_ps(waveT->output[kLeft][ov] + xvoice))));
											_mm256_store_ps(allVoicesOutput[kRight][ov] + xvoice, _mm256_add_ps(_mm256_load_ps(allVoicesOutput[kRight][ov] + xvoice), _mm256_mul_ps(_mm256_set1_ps(waveT->boostVolume), _mm256_load_ps(waveT->output[kRight][ov] + xvoice))));
										}
									}
									else
									{
										for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
										{
											_mm256_store_ps(allVoicesOutput[kLeft][ov] + xvoice, _mm256_mul_ps(_mm256_load_ps(allVoicesOutput[kLeft][ov] + xvoice), _mm256_mul_ps(_mm256_set1_ps(waveT->boostVolume), _mm256_load_ps(waveT->output[kLeft][ov] + xvoice))));
											_mm256_store_ps(allVoicesOutput[kRight][ov] + xvoice, _mm256_mul_ps(_mm256_load_ps(allVoicesOutput[kRight][ov] + xvoice), _mm256_mul_ps(_mm256_set1_ps(waveT->boostVolume), _mm256_load_ps(waveT->output[kRight][ov] + xvoice))));
										}
									}
								}
							}
							else
							{
								WusikWavetable::WusikWavetableObject* waveT[2] = { programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[0], programs[currentProgram]->sequencerLayers[zones][index]->wavetableObjects[1] };
								//
								waveT[0]->updateRate(ADSREnvelope_Pitch[zones].output);
								waveT[0]->processVoices_AVX();
								//
								waveT[1]->updateRate(ADSREnvelope_Pitch[zones].output);
								waveT[1]->processVoices_AVX();
								//
								for (int xvoice = 0; xvoice < ADSREnvelope_Amplitude[zones].maxVoices; xvoice += 8)
								{
									if (waveT[0]->addThisLayerSignal)
									{
										for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
										{
											_mm256_store_ps(allVoicesOutput[kLeft][ov] + xvoice, _mm256_add_ps(_mm256_load_ps(allVoicesOutput[kLeft][ov] + xvoice), _mm256_mul_ps(_mm256_set1_ps(waveT[0]->boostVolume), _mm256_add_ps(_mm256_load_ps(waveT[0]->output[kLeft][ov] + xvoice), _mm256_load_ps(waveT[1]->output[kLeft][ov] + xvoice)))));
											_mm256_store_ps(allVoicesOutput[kRight][ov] + xvoice, _mm256_add_ps(_mm256_load_ps(allVoicesOutput[kRight][ov] + xvoice), _mm256_mul_ps(_mm256_set1_ps(waveT[0]->boostVolume), _mm256_add_ps(_mm256_load_ps(waveT[0]->output[kRight][ov] + xvoice), _mm256_load_ps(waveT[1]->output[kRight][ov] + xvoice)))));
										}
									}
									else
									{
										for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
										{
											_mm256_store_ps(allVoicesOutput[kLeft][ov] + xvoice, _mm256_mul_ps(_mm256_load_ps(allVoicesOutput[kLeft][ov] + xvoice), _mm256_mul_ps(_mm256_set1_ps(waveT[0]->boostVolume), _mm256_add_ps(_mm256_load_ps(waveT[0]->output[kLeft][ov] + xvoice), _mm256_load_ps(waveT[1]->output[kLeft][ov] + xvoice)))));
											_mm256_store_ps(allVoicesOutput[kRight][ov] + xvoice, _mm256_mul_ps(_mm256_load_ps(allVoicesOutput[kRight][ov] + xvoice), _mm256_mul_ps(_mm256_set1_ps(waveT[0]->boostVolume), _mm256_add_ps(_mm256_load_ps(waveT[0]->output[kRight][ov] + xvoice), _mm256_load_ps(waveT[1]->output[kRight][ov] + xvoice)))));
										}
									}
								}
							}
						}
						//
						// Down Sample //
						for (int xvoice = 0; xvoice < ADSREnvelope_Amplitude[zones].maxVoices; xvoice += 8)
						{
							for (int ov = 0; ov < waveTable[zones]->overSample; ov++)
							{
								waveTable[zones]->wavetableObjects.getFirst()->downSample<wAVXFMA3>(xvoice, allVoicesOutput[kLeft][ov], allVoicesOutput[kRight][ov]);
							}
							//
							_mm256_store_ps(filterProcessSignal[kLeft] + xvoice, _mm256_load_ps(allVoicesOutput[kLeft][waveTable[zones]->overSample - 1] + xvoice));
							_mm256_store_ps(filterProcessSignal[kRight] + xvoice, _mm256_load_ps(allVoicesOutput[kRight][waveTable[zones]->overSample - 1] + xvoice));
						}
						//
						// Filter Processing //
						for (int xvoice = 0; xvoice < ADSREnvelope_Amplitude[zones].maxVoices; xvoice += 8)
						{
							for (int xv = 0; xv < 8; xv++)
							{
								float ffreq = jlimit(0.0f, 1.0f, valuesList[kFilter_Frequency] + sequencedFilter[xvoice + xv] + (((valuesList[kFilter_Envelope] * 2.0f) - 1.0f) * ADSREnvelope_Filter[zones].output[xvoice + xv]));
								theFilter[zones][kLeft].setFrequency(ffreq, xvoice + xv);
								theFilter[zones][kRight].setFrequency(ffreq, xvoice + xv);
								//
								if (ADSREnvelope_Amplitude[zones].resetFilter[xvoice + xv])
								{
									ADSREnvelope_Amplitude[zones].resetFilter[xvoice + xv] = false;
									theFilter[zones][kLeft].reset(xvoice + xv);
									theFilter[zones][kRight].reset(xvoice + xv);
								}
							}
							//
							theFilter[zones][kLeft].process_AVX(filterProcessSignal[kLeft], xvoice);
							theFilter[zones][kRight].process_AVX(filterProcessSignal[kRight], xvoice);
						}
					#endif
					}
				}
			}
			//
			// -------------- FINAL MIX DOWN ------------------------------------------------------------------------------- // 
			//
			#if WIS_DEBUG
			if (zones == 0)
			{
				static int xCounter = 0;
				if (xCounter == 0)
				{
					if (debugWindow->newMessage.isEmpty())
					{
						debugWindow->newMessage.add(showThis);
						if (ADSREnvelope_Amplitude[zones].isIdle) debugWindow->newMessage.add("Envelope IDLE");
						if (isFirstNote) debugWindow->newMessage.add("AMP First Note");
						debugWindow->newMessage.add(extraDebug + "\n");
						debugWindow->newMessage.add("Envelope Voices: " + String(ADSREnvelope_Amplitude[zones].maxVoices) + " | Over Sample: " + String(waveTable[zones]->overSample));
						debugWindow->newMessage.add("Fade Out: " + String(fadeOutCounter, 6));
						debugWindow->newMessage.add("\n");
						//
						debugWindow->newMessage.add("Seq Note: " + String(programs[currentProgram]->sequencerLayers[0][0]->initialMidiMessage[0].getNoteNumber()));
						debugWindow->newMessage.add("Seq Position: " + String(programs[currentProgram]->sequencerLayers[0][0]->currentStep[0]));
						debugWindow->newMessage.add("Seq Initial Note: " + String(programs[currentProgram]->sequencerLayers[0][0]->initialMidiMessage[0].getNoteNumber()));
						debugWindow->newMessage.add("\n");
						//
						ARRAY_Iterator(ADSREnvelope_Amplitude[zones].holdingNotes)
						{
							debugWindow->newMessage.add("AMP " + String(ADSREnvelope_Amplitude[zones].holdingNotes[index]));
						}
						//
						debugWindow->newMessage.add("\n");
						//
						ARRAY_Iterator(ADSREnvelope_Filter[zones].holdingNotes)
						{
							debugWindow->newMessage.add("FILTER " + String(ADSREnvelope_Filter[zones].holdingNotes[index]));
						}
						debugWindow->newMessage.add("\n");

						//
						/*for (int xx = 0; xx < ADSREnvelope_Amplitude[zones].maxVoices; xx++)
						{
							//debugWindow->newMessage.add("Wavetable #" + String(xx) + " : " + String(waveTable[zones]->wavetableObjects.getFirst()->output[0][xx], 6));
							debugWindow->newMessage.add("Env #" + String(xx) + " : " + String(ADSREnvelope_Amplitude[zones].output[xx], 6));
							debugWindow->newMessage.add("Filter #" + String(xx) + " : " + String(ADSREnvelope_Filter[zones].output[xx], 6));
						}*/
						//
						debugWindow->newMessage.add("\n");
						debugWindow->readyToDraw = true;
					}
				}
				//
				xCounter++;
				if (xCounter > 2000) xCounter = 0;
			}
			#endif
			//
			float volumeReduction = 0.8f;
			if (valuesList[kUnison] > 0.0f) volumeReduction = 0.8f - ((0.01f * float(ADSREnvelope_Amplitude[zones].maxVoices)));
			float panR = jlimit(0.0f, 1.0f, valuesList[kPan]) * valuesList[kVolume] * volumeReduction;
			float panL = jlimit(0.0f, 1.0f, 1.0f - valuesList[kPan]) * valuesList[kVolume] * volumeReduction;
			//
			for (int xvoice = 0; xvoice < ADSREnvelope_Amplitude[zones].maxVoices; xvoice++)
			{
				limiterBuffer[zones].addSample(kLeft, xsample, filterProcessSignal[kLeft][xvoice] * ADSREnvelope_Amplitude[zones].output[xvoice] * panL);
				limiterBuffer[zones].addSample(kRight, xsample, filterProcessSignal[kRight][xvoice] * ADSREnvelope_Amplitude[zones].output[xvoice] * panR);
			}
		}
	}
	//
	DEBUG_TO_FILE("Process Block Effects");
	//
	for (int zones = 0; zones < 4; zones++)
	{
		if (waveTable[zones]->wavetableObjects.size() > 0)
		{
			ARRAY_Iterator(programs[currentProgram]->effects[zones])
			{
				programs[currentProgram]->effects[zones][index]->processBlock(limiterBuffer[zones]);
			}
			//
			if (programs[currentProgram]->values[zones][kMasterLimiter] == 1.0f && VSTLimiter[zones] != nullptr) VSTLimiter[zones]->processBlock(limiterBuffer[zones], emptyMIDIMessageBuffer);
			//
			buffer.addFrom(kLeft, 0, limiterBuffer[zones], kLeft, 0, limiterBuffer[zones].getNumSamples());
			buffer.addFrom(kRight, 0, limiterBuffer[zones], kRight, 0, limiterBuffer[zones].getNumSamples());
		}
	}
	//
	// Fade Out //
	if (fadeOutRate != 0.0f)
	{
		for (int xsmp = 0; xsmp < buffer.getNumSamples(); xsmp++)
		{
			buffer.setSample(kLeft, xsmp, buffer.getSample(kLeft, xsmp) * fadeOutCounter);
			buffer.setSample(kRight, xsmp, buffer.getSample(kRight, xsmp) * fadeOutCounter);
			//
			fadeOutCounter += fadeOutRate;
			//
			if (fadeOutCounter < 0.0f && fadeOutRate < 0.0f) { fadeOutCounter = 0.0f; }
			if (fadeOutCounter > 1.0f && fadeOutRate > 0.0f) { fadeOutCounter = 1.0f;  fadeOutRate = 0.0f; }
		}
	}
	//
	isProcessingSomething.set(0);
	//
	DEBUG_TO_FILE("Process Block Finished");
}