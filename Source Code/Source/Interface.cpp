// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

	Created by William Kalfelz @ Wusik Dot Com (c) 2019

*/

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "PluginEditor.h"
//
#define ZN processor.parametersMainStart[zones] +
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessorEditor::updateEntireInterface()
{
	for (int zones = 0; zones < 4; zones++)
	{
		if (zoneComponents[zones] == nullptr)
		{
			// Main Stuff (never gets removed) //
			//
			if (zones == 0)
			{
				displayValue = new WValueLabel;
				addChildComponent(displayValue);
			}
			//
			zoneComponents[zones] = new Component("Zone " + String(zones + 1));
			zoneComponents[zones]->setBounds(getBounds());
			zoneComponents[zones]->setInterceptsMouseClicks(false, true);
			zoneComponents[zones]->setComponentID("Zone " + String(zones + 1));
			addAndMakeVisible(zoneComponents[zones]);
			//
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kADSR_Attack, 500, 0, 0, 128, 88));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kADSR_Decay, 590, 0, 0, 128, 88));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kADSR_Sustain, 660, 0, 0, 128, 88));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kADSR_Release, 720, 0, 0, 128, 88));
			//
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kADSR_Mode, 0, 0, 0));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kADSR_Clip, 0, 0, 0));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kADSR_Velocity, 0, 0, 0));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kADSR_Curve, 0, 0, 0));
			//
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kFilter_ADSR_Attack, 500, 0, 0, 128, 88));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kFilter_ADSR_Decay, 590, 0, 0, 128, 88));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kFilter_ADSR_Sustain, 660, 0, 0, 128, 88));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kFilter_ADSR_Release, 720, 0, 0, 128, 88));
			//
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kFilter_ADSR_Mode, 0, 0, 0));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kFilter_ADSR_Clip, 0, 0, 0));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kFilter_ADSR_Velocity, 0, 0, 0));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kFilter_ADSR_Curve, 0, 0, 0));
			//
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kPitch_ADSR_Attack, 500, 0, 0, 128, 88));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kPitch_ADSR_Decay, 590, 0, 0, 128, 88));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kPitch_ADSR_Sustain, 660, 0, 0, 128, 88));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob3_Vertical_Slider], zones, ZN kPitch_ADSR_Release, 720, 0, 0, 128, 88));
			//
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kPitch_ADSR_Mode, 0, 0, 0));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kPitch_ADSR_Clip, 0, 0, 0));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kPitch_ADSR_Velocity, 0, 0, 0));
			wusikKnobs_Envelopes[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kPitch_ADSR_Curve, 0, 0, 0));
			//
			wusikKnobs[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob2_LargeRed], zones, ZN kVolume, 0, 0, 0, 74, 101));
			wusikKnobs[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob2_LargeRed], zones, ZN kPan, 92, 0, 0, 74, 101));
			wusikKnobs[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob1_Regular], zones, ZN kTune, 192, 0, 0, 64, 101));
			wusikKnobs[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob1_Regular], zones, ZN kFine, 292, 0, 0, 64, 101));
			//
			wusikKnobs[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob2_LargeRed], zones, ZN kFilter_Frequency, 0, 0, 0, 74, 101));
			wusikKnobs[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob2_LargeRed], zones, ZN kFilter_Rezonance, 92, 0, 0, 74, 101));
			wusikKnobs[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kFilter_Mode, 0, 0, 0));
			wusikKnobs[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kFilter_Smooth, 0, 0, 0));
			wusikKnobs[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kFilter_Envelope, 0, 0, 0));
			wusikKnobs[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kFilter_Limit, 0, 0, 0));
			//
			wusikKnobs[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob6_LED], zones, ZN kMasterLimiter, 92, 0, 0, 22, 2));
			wusikKnobs[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob10_TinyDark], zones, ZN kGlide, 92, 0, 0, 22, 101));
			wusikKnobs[zones].add(new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob6_LED], zones, ZN kGlideAuto, 92, 0, 0, 22, 2));
			wusikKnobs[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kMIDICHInput, 0, 0, 0));
			wusikKnobs[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kPitchBend, 0, 0, 0));
			wusikKnobs[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kOctave, 0, 0, 0));
			wusikKnobs[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kQuality, 0, 0, 0));
			wusikKnobs[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kMode, 0, 0, 0));
			wusikKnobs[zones].add(new WusikKnob(&processor, nullptr, zones, ZN kUnison, 0, 0, 0));
			//
			ARRAY_Iterator(wusikKnobs[zones]) { zoneComponents[zones]->addAndMakeVisible(wusikKnobs[zones][index]); }
			ARRAY_Iterator(wusikKnobs_Envelopes[zones]) { zoneComponents[zones]->addAndMakeVisible(wusikKnobs_Envelopes[zones][index]); }
			//
			layersViewPort[zones] = new Viewport("Viewport Master");
			layersViewPort[zones]->setComponentID("Viewport Master");
			Rectangle<int> viewRect;
			processor.dataFiles->getSkinRectangle("Scroll Area", viewRect);
			viewRect.setBottom(getBounds().getBottom());
			layersViewPort[zones]->setBounds(viewRect);
			layersViewPort[zones]->setScrollBarThickness(processor.dataFiles->scrollThickness);
			getLookAndFeel().setColour(ScrollBar::ColourIds::thumbColourId, processor.dataFiles->scrollColour);
			layersViewPort[zones]->setScrollBarsShown(true, false, true, false);
			zoneComponents[zones]->addAndMakeVisible(layersViewPort[zones]);
		}
		//
		clearInterfaceObjects(zones);
		layersViewPortComponent[zones] = new Component("Viewport Component");
		layersViewPortComponent[zones]->setComponentID("Viewport Component");
		layersViewPortComponent[zones]->setBounds(0, 0, layersViewPort[zones]->getWidth(), 9999);
		layersViewPort[zones]->setViewedComponent(layersViewPortComponent[zones]);
		//
		// ============================================ //
		// ADD STATIC LAYERS							//
		// ============================================ //
		//
		OwnedArray<WLayerComponent> layersList;
		//
		String deleteButton = processor.dataFiles->skinSettings->getValue("StaticDelete", "0,0");
		int delX = deleteButton.upToFirstOccurrenceOf(",", false, false).getIntValue();
		int delY = deleteButton.fromLastOccurrenceOf(",", false, false).getIntValue();
		Rectangle<int> deletePosition(delX, delY, processor.dataFiles->allImages[kKnob5_OnOff].getWidth(), processor.dataFiles->allImages[kKnob5_OnOff].getHeight() / 2);
		//
		ARRAY_Iterator(processor.programs[processor.currentProgram]->sequencerLayers[zones])
		{
			int parameterPosition = processor.parametersLayerStart[zones][index];
			WusikSequencer* xSeq = processor.programs[processor.currentProgram]->sequencerLayers[zones][index];
			//
			if (xSeq->isStatic)
			{
				WLayerComponent *newLayer = layersList.add(new WLayerComponent(WLayerComponent::kStaticLayer, processor.dataFiles->allImages[kBackground_StaticLayer], index, &processor, zones, deletePosition));
				if (firstStaticLayer == nullptr) firstStaticLayer = newLayer;
				//
				WKNOB(kKnob9_AddMultiply, 22, 2, "Static");
				WKNOB(kKnob6_LED, 22, 2, "Static"); parameterPosition++;
				WKNOB(kKnob1_Regular, 64, 101, "Static");
				WKNOB(kKnob1_Regular, 64, 101, "Static");
				WKNOB(kKnob1_Regular, 64, 101, "Static");
				WKNOB(kKnob1_Regular, 64, 101, "Static");
				WKNOB(kKnob10_TinyDark, 22, 101, "Static");
				newLayer->addAndMakeVisible(new WusikKnob(&processor, &processor.dataFiles->allImages[kBackground_Waveform1], zones, parameterPosition, 10, 10, 0, 0, 0, "Static", true, index)); parameterPosition++;
				newLayer->setComponentID("Static Layer");
			}
		}
		//
		// ============================================ //
		// ADD SEQUENCED LAYERS							//
		// ============================================ //
		//
		deleteButton = processor.dataFiles->skinSettings->getValue("SequencedDelete", "0,0");
		delX = deleteButton.upToFirstOccurrenceOf(",", false, false).getIntValue();
		delY = deleteButton.fromLastOccurrenceOf(",", false, false).getIntValue();
		deletePosition = Rectangle<int>(delX, delY, processor.dataFiles->allImages[kKnob5_OnOff].getWidth(), processor.dataFiles->allImages[kKnob5_OnOff].getHeight() / 2);
		//
		ARRAY_Iterator(processor.programs[processor.currentProgram]->sequencerLayers[zones])
		{
			int parameterPosition = processor.parametersLayerStart[zones][index];
			WusikSequencer* xSeq = processor.programs[processor.currentProgram]->sequencerLayers[zones][index];
			//
			if (!xSeq->isStatic)
			{
				WLayerComponent *newLayer = layersList.add(new WLayerComponent(WLayerComponent::kSequencedLayer, processor.dataFiles->allImages[kBackground_SequencedLayer], index, &processor, zones, deletePosition));
				if (firstSequencedLayer == nullptr) firstSequencedLayer = newLayer;
				processor.dataFiles->getSkinRectangle("SeqAddStep", newLayer->buttonPos2);
				//
				WKNOB(kKnob9_AddMultiply, 22, 2, "Seq");
				WKNOB(kKnob6_LED, 22, 2, "Seq"); parameterPosition++;
				WKNOB(kKnob1_Regular, 64, 101, "Seq");
				WKNOB(kKnob1_Regular, 64, 101, "Seq");
				WKNOB(kKnob1_Regular, 64, 101, "Seq");
				WKNOB(kKnob1_Regular, 64, 101, "Seq");
				WKNOB(kKnob10_TinyDark, 22, 101, "Seq");
				parameterPosition++;
				WKNOB(kKnob7_OnOff, 26, 2, "Seq");
				WKNOB(kKnob1_Regular, 64, 101, "Seq");
				WKNOB(kKnob1_Regular, 64, 101, "Seq");
				WKNOB(kKnob1_Regular, 64, 101, "Seq");
				WKNOBTEXT("Seq");
				//
				int pY = processor.dataFiles->allImages[kBackground_SequencedLayer].getHeight();
				//
				for (int index2 = 0; index2 < xSeq->stepsSize; index2++)
				{
					WLayerComponent *tempComponent = new WLayerComponent(WLayerComponent::kSequencedStep, processor.dataFiles->allImages[kBackground_SequencedLayer_Step], index, &processor, zones);
					//
					tempComponent->setBounds(0, pY, tempComponent->getWidth(), tempComponent->getHeight());
					tempComponent->setComponentID("WusikSteps");
					tempComponent->step = index2;
					processor.dataFiles->getSkinRectangle("SeqRemoveStep", tempComponent->buttonPos2);
					newLayer->addAndMakeVisible(tempComponent);
					//
					WKNOB2(kKnob8_Tiny, 28, 101, "Seq");
					WKNOB2(kKnob8_Tiny, 28, 101, "Seq");
					WKNOBTEXT2("Seq");
					WKNOBTEXT2("Seq");
					WKNOBTEXT2("Seq");
					WKNOBTEXT2("Seq");
					tempComponent->addAndMakeVisible(new WusikKnob(&processor, &processor.dataFiles->allImages[kBackground_Waveform2], zones, parameterPosition, 10, 10, 0, 0, 0, "Seq", true, index, index2)); parameterPosition++;
					WKNOB2(kKnob8_Tiny, 28, 101, "Seq");
					//
					pY += processor.dataFiles->allImages[kBackground_SequencedLayer_Step].getHeight();
				}
				//
				newLayer->setBounds(0, 0, newLayer->getWidth(), pY + processor.dataFiles->allImages[kBackground_SequencedLayer_Step_Bottom].getHeight());
				WLayerComponent *tempComponent = new WLayerComponent(WLayerComponent::kSequencedStep, processor.dataFiles->allImages[kBackground_SequencedLayer_Step_Bottom], index, &processor, zones);
				tempComponent->setTopLeftPosition(0, pY);
				newLayer->addAndMakeVisible(tempComponent);
				newLayer->setComponentID("Sequenced Layer");
			}
		}
		//
		//
		// ============================================ //
		// ADD EFFECTS LAYERS							//
		// ============================================ //
		//
		OwnedArray<WLayerComponent> effectsList;
		//
		deleteButton = processor.dataFiles->skinSettings->getValue("EffectDelete", "0,0");
		delX = deleteButton.upToFirstOccurrenceOf(",", false, false).getIntValue();
		delY = deleteButton.fromLastOccurrenceOf(",", false, false).getIntValue();
		Rectangle<int> deletePositionFX(delX, delY, processor.dataFiles->allImages[kKnob5_OnOff].getWidth(), processor.dataFiles->allImages[kKnob5_OnOff].getHeight() / 2);
		//
		ARRAY_Iterator(processor.programs[processor.currentProgram]->effects[zones])
		{
			if (processor.programs[processor.currentProgram]->effects[zones][index]->hasEffect())
			{
				int parameterPosition = processor.parametersEffectsStart[zones][index];
				//
				WLayerComponent *newLayer = effectsList.add(new WLayerComponent(WLayerComponent::kEffectLayer, processor.dataFiles->allImages[kBackground_EffectsLayer], index, &processor, zones, deletePositionFX));
				WEffectsDetails* plugin = processor.programs[processor.currentProgram]->effects[zones][index];
				newLayer->pluginName = processor.programs[processor.currentProgram]->effects[zones][index]->name;
				newLayer->buttonPos2 = processor.dataFiles->effectNamePresetPosition;
				//
				WKNOB(kKnob6_LED, 22, 2, "");
				WKNOB(kKnob10_TinyDark, 22, 101, "");
				//
				WFXViewPort* viewPort = new WFXViewPort();
				viewPort->stepSize = processor.dataFiles->effectScrollStep;
				viewPort->setComponentID("Viewport Effects");
				viewPort->setScrollBarsShown(true, false, true, false);
				//
				Component* dummyComponent = new Component;
				int dumH = 0;
				//
				for (int xz = 0; xz < MAX_EFFECT_PARAMETERS; xz++)
				{
					if (xz >= plugin->getNumParameters()) break;
					//
					tempWButton = new WusikKnob(&processor, &processor.dataFiles->allImages[kKnob1_Regular], zones, parameterPosition + xz, 10, 10, 0, 64, 101);
					//
					Label* tempLabel = new Label();
					tempLabel->setJustificationType(Justification::centred);
					tempLabel->setFont(processor.dataFiles->effectLabelFont);
					tempLabel->setColour(Label::ColourIds::textColourId, processor.dataFiles->effectLabelColour);
					Rectangle<int> xBounds = tempWButton->getBounds();
					//				
					tempLabel->setBounds(
						xBounds.getX() + processor.dataFiles->effectLabelPosition.getX(),
						xBounds.getY() + processor.dataFiles->effectLabelPosition.getY(),
						processor.dataFiles->effectLabelPosition.getWidth(), processor.dataFiles->effectLabelPosition.getHeight());
					//
					tempLabel->setText(plugin->getParameterName(xz), NotificationType::dontSendNotification);
					//
					dumH = tempLabel->getBounds().getBottom() + processor.dataFiles->effectScrollBottomOffset;
					dummyComponent->addAndMakeVisible(tempLabel);
					dummyComponent->addAndMakeVisible(tempWButton);
				}
				//
				viewPort->setBounds(70, 7, 326, 92);
				dummyComponent->setBounds(0, 0, viewPort->getWidth(), dumH);
				viewPort->setViewedComponent(dummyComponent);
				newLayer->addAndMakeVisible(viewPort);
				newLayer->setComponentID("Effect Layer");
			}
		}
		//
		// ------------------------------- //
		//
		int pX = 0;
		int pY = processor.dataFiles->allImages[kBackground_SequencedLayersBar].getHeight();
		int cCel = 0;
		int maxHeigth = pY;
		//
		// Add All Static Layers //
		WADD_BUTTON_IMAGE(0, tempLayersButton, kBackground_StaticLayerBar, WImageButton::kNewStaticLayer);
		//
		if (processor.programs[processor.currentProgram]->showLayers[zones][0])
		{
			WADD_BUTTON_IMAGE2(0, tempLayersButton, kLayersHide, WImageButton::kShowHideStaticLayers);
		}
		else
		{
			WADD_BUTTON_IMAGE2(0, tempLayersButton, kLayersShow, WImageButton::kShowHideStaticLayers);
		}
		//
		ARRAY_Iterator(layersList)
		{
			if (layersList[index]->type == WLayerComponent::kStaticLayer)
			{
				if (processor.programs[processor.currentProgram]->showLayers[zones][0])
				{
					layersList[index]->setTopLeftPosition(pX, pY);
					layersViewPortComponent[zones]->addAndMakeVisible(layersList[index]);
					//
					pX += layersList[index]->getWidth();
					cCel++;
					maxHeigth = pY + layersList[index]->getHeight();
					//
					if (cCel >= 3)
					{
						pX = cCel = 0;
						pY += layersList[index]->getHeight();
					}
				}
				else layersViewPortComponent[zones]->addChildComponent(layersList[index]);
			}
		}
		//
		// Add All Sequenced Layers //
		WADD_BUTTON_IMAGE(maxHeigth, tempLayersButton, kBackground_SequencedLayersBar, WImageButton::kNewSequencedLayer);
		//
		if (processor.programs[processor.currentProgram]->showLayers[zones][1])
		{
			WADD_BUTTON_IMAGE2(maxHeigth, tempLayersButton, kLayersHide, WImageButton::kShowHideSequencedLayers);
		}
		else
		{
			WADD_BUTTON_IMAGE2(maxHeigth, tempLayersButton, kLayersShow, WImageButton::kShowHideSequencedLayers);
		}
		//
		pX = 0;
		int SY[2] = { maxHeigth + processor.dataFiles->allImages[kBackground_SequencedLayersBar].getHeight(), maxHeigth + processor.dataFiles->allImages[kBackground_SequencedLayersBar].getHeight() };
		cCel = 0;
		maxHeigth = SY[0];
		//
		ARRAY_Iterator(layersList)
		{
			if (layersList[index]->type == WLayerComponent::kSequencedLayer)
			{
				if (processor.programs[processor.currentProgram]->showLayers[zones][1])
				{
					layersList[index]->setTopLeftPosition(pX, SY[cCel]);
					layersViewPortComponent[zones]->addAndMakeVisible(layersList[index]);
					//
					pX += layersList[index]->getWidth();
					SY[cCel] += layersList[index]->getHeight();
					cCel++;
					//
					if (cCel >= 2)
					{
						pX = 0;
						cCel = 0;
					}
				}
				else layersViewPortComponent[zones]->addChildComponent(layersList[index]);
			}
		}
		//
		if (SY[0] > SY[1]) maxHeigth = SY[0]; else maxHeigth = SY[1];
		//
		//
		// Add Effects //
		WADD_BUTTON_IMAGE(maxHeigth, tempLayersButton, kBackground_EffectsBar, WImageButton::kNewEffect);
		//
		if (processor.programs[processor.currentProgram]->showLayers[zones][2])
		{
			WADD_BUTTON_IMAGE2(maxHeigth, tempLayersButton, kLayersHide, WImageButton::kShowHideEffects);
		}
		else
		{
			WADD_BUTTON_IMAGE2(maxHeigth, tempLayersButton, kLayersShow, WImageButton::kShowHideEffects);
		}
		//
		maxHeigth += processor.dataFiles->allImages[kBackground_EffectsBar].getHeight();
		//
		pX = 0;
		pY = maxHeigth;
		cCel = 0;
		//
		ARRAY_Iterator(effectsList)
		{
			if (processor.programs[processor.currentProgram]->showLayers[zones][2])
			{
				effectsList[index]->setTopLeftPosition(pX, pY);
				layersViewPortComponent[zones]->addAndMakeVisible(effectsList[index]);
				//
				pX += effectsList[index]->getWidth();
				cCel++;
				maxHeigth = pY + effectsList[index]->getHeight();
				//
				if (cCel >= 3)
				{
					pX = cCel = 0;
					pY += effectsList[index]->getHeight();
				}
			}
			else layersViewPortComponent[zones]->addChildComponent(effectsList[index]);
		}
		//
		layersViewPortComponent[zones]->setSize(layersViewPortComponent[zones]->getWidth(), maxHeigth);
		layersList.clear(false);
		effectsList.clear(false);
	}
	//
	updateEnvelopesVisible();
	updateZonesVisible();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessorEditor::closeTheInterface()
{
	updateZonesVisible(true);
	//
	//  remove knob references from all parameters //
	ARRAY_Iterator(processor.getParameters())
	{
		((WusikZRParameter*)processor.getParameters()[index])->knob = nullptr;
	}
	//
	wusikKnobs[0].clear();
	wusikKnobs[1].clear();
	wusikKnobs[2].clear();
	wusikKnobs[3].clear();
	//
	wusikKnobs_Envelopes[0].clear();
	wusikKnobs_Envelopes[1].clear();
	wusikKnobs_Envelopes[2].clear();
	wusikKnobs_Envelopes[3].clear();
	//
	layersViewPortComponent[0].release();
	layersViewPortComponent[1].release();
	layersViewPortComponent[2].release();
	layersViewPortComponent[3].release();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessorEditor::clearInterfaceObjects(int zone)
{
	MessageManagerLock lock;
	//
	updateZonesVisible(zone);
	//
	if (layersViewPortComponent[zone] != nullptr) layersViewPortComponent[zone]->deleteAllChildren();
	layersViewPortComponent[zone] = nullptr;
	firstStaticLayer = nullptr;
	firstSequencedLayer = nullptr;
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WLayerComponent::mouseDown(const MouseEvent& e)
{
	WusikZrAudioProcessor* processor = (WusikZrAudioProcessor*)owner;
	//
	if (buttonPos.contains(e.getPosition()))
	{
		if (type == WLayerComponent::kStaticLayer)
		{
			if (((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->confirmBox("Remove Static Layer"))
			{
				((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface();
				processor->newAction(kActionRemoveLayer, layer);
				return;
			}
		}
		else if (type == WLayerComponent::kSequencedLayer)
		{
			if (((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->confirmBox("Remove Sequenced Layer"))
			{
				((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface();
				processor->newAction(kActionRemoveLayer, layer);
				return;
			}
		}
		else if (type == WLayerComponent::kEffectLayer)
		{
			if (((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->confirmBox("Remove Effects"))
			{
				((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface();
				processor->newAction(kActionUnloadEffect, layer);
				return;
			}
		}
	}
	//
	if (buttonPos2.contains(e.getPosition()))
	{
		if (type == WLayerComponent::kEffectLayer)
		{
			if (processor->programs[processor->currentProgram]->effects[processor->currentZone][layer]->hasEffect())
			{
				PopupMenu mp;
				//
				if (processor->programs[processor->currentProgram]->effects[processor->currentZone].size() > 1)
				{
					if (layer > 0) mp.addItem(-4, "Move To Start");
					if (layer < (processor->programs[processor->currentProgram]->effects[processor->currentZone].size() - 1)) mp.addItem(-1, "Move Forward");
					if (layer > 0) mp.addItem(-2, "Move Backwards");
				}
				//
				int nPrograms = processor->programs[processor->currentProgram]->effects[processor->currentZone][layer]->getNumPrograms();
				int nCurProgram = processor->programs[processor->currentProgram]->effects[processor->currentZone][layer]->getCurrentProgram();
				//
				if (nPrograms > 1)
				{
					mp.addSeparator();

					for (int xp = 0; xp < nPrograms; xp++)
					{
						mp.addItem(xp + 1, processor->programs[processor->currentProgram]->effects[processor->currentZone][layer]->getProgramName(xp), true, nCurProgram == xp);
					}
				}
				//
				int result = mp.show();
				if (result != 0)
				{
					if (result == -1)
					{
						((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface();
						processor->newAction(kActionMoveEffect, layer, 1);
					}
					else if (result == -2)
					{
						((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface(); 
						processor->newAction(kActionMoveEffect, layer, -1);
					}
					else if (result == -4)
					{
						((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface(); 
						processor->newAction(kActionMoveEffect, layer, 99);
					}
					else
					{
						processor->programs[processor->currentProgram]->effects[processor->currentZone][layer]->setCurrentProgram(result - 1);
						processor->programs[processor->currentProgram]->effects[processor->currentZone][layer]->hasChanges = true;
						repaint();
					}
				}
			}
		}
		else if (type == WLayerComponent::kSequencedLayer)
		{
			if (processor->programs[processor->currentProgram]->sequencerLayers[processor->currentZone][layer]->stepsSize == MAX_SEQUENCE_STEPS)
			{
				MessageBox("Warning", "Maximum number of steps reached (128).");
			}
			else
			{
				if (((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->confirmBox("Add New Step"))
				{
					((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface();
					processor->newAction(kActionAddNewStep, layer);
					return;
				}
			}
		}
		else if (type == WLayerComponent::kSequencedStep)
		{
			if (processor->programs[processor->currentProgram]->sequencerLayers[processor->currentZone][layer]->stepsSize == 1)
			{
				MessageBox("Warning", "Must have at least one step.");
			}
			else
			{
				if (((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->confirmBox("Remove Step"))
				{
					((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface();
					processor->newAction(kActionRemoveStep, layer, step);
					return;
				}
			}
		}
	}
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WImageButton::mouseUp(const MouseEvent& e)
{
	// Add New Layer (Static or Sequenced) //
	WusikZrAudioProcessor* processor = (WusikZrAudioProcessor*)owner;
	//
	if (type >= WImageButton::kShowHideStaticLayers)
	{
		int nTotal = 0;
		//
		if (type == WImageButton::kShowHideStaticLayers || type == WImageButton::kShowHideSequencedLayers)
		{
			ARRAY_Iterator(processor->programs[processor->currentProgram]->sequencerLayers[processor->currentZone])
			{
				if (type == WImageButton::kShowHideStaticLayers &&
					processor->programs[processor->currentProgram]->sequencerLayers[processor->currentZone][index]->isStatic) nTotal++;
				//
				if (type == WImageButton::kShowHideSequencedLayers &&
					!processor->programs[processor->currentProgram]->sequencerLayers[processor->currentZone][index]->isStatic) nTotal++;
			}
		}
		else
		{
			nTotal = processor->programs[processor->currentProgram]->effects[processor->currentZone].size();
		}
		//
		if ((processor->programs[processor->currentProgram]->showLayers[processor->currentZone][type - WImageButton::kShowHideStaticLayers] && nTotal > 0)
			|| !processor->programs[processor->currentProgram]->showLayers[processor->currentZone][type - WImageButton::kShowHideStaticLayers])
		{
			processor->programs[processor->currentProgram]->showLayers[processor->currentZone][type - WImageButton::kShowHideStaticLayers] =
				!processor->programs[processor->currentProgram]->showLayers[processor->currentZone][type - WImageButton::kShowHideStaticLayers];
		}
		//
		((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface();
		processor->newAction(kActionUpdateEverything);
		return;
	}
	//
	if (processor->programs[processor->currentProgram]->sequencerLayers[processor->currentZone].size() == WMAX_LAYERS)
	{
		MessageBox("Warning!", "Max number of " + String(WMAX_LAYERS) + " layers + effects reached!");
	}
	else
	{
		if (type == WImageButton::kNewStaticLayer)
		{
			if (((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->confirmBox("Add New Static Layer"))
			{
				processor->programs[processor->currentProgram]->showLayers[processor->currentZone][0] = true;
				((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface();
				processor->newAction(kActionAddNewLayer, kAddStaticLayer);
				return;
			}
		}
		else if (type == WImageButton::kNewSequencedLayer)
		{
			if (((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->confirmBox("Add New Sequenced Layer"))
			{
				processor->programs[processor->currentProgram]->showLayers[processor->currentZone][1] = true;
				((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface();
				processor->newAction(kActionAddNewLayer, kAddSequencedLayer);
				return;
			}
		}
		else if (type == WImageButton::kNewEffect)
		{
			PopupMenu mp;
			//
			ARRAY_Iterator(processor->dataFiles->pluginEffects)
			{
				#if JUCE_64BIT
					if (!processor->dataFiles->plugsList->plugins[processor->dataFiles->pluginEffects[index]]->is32Bits &&
				#else	
					if (processor->dataFiles->plugsList->plugins[processor->dataFiles->pluginEffects[index]]->is32Bits &&
				#endif
					!processor->dataFiles->plugsList->plugins[processor->dataFiles->pluginEffects[index]]->isInstrument)
					{
						String theName = processor->dataFiles->plugsList->plugins[processor->dataFiles->pluginEffects[index]]->customName;
						if (theName.isEmpty()) theName = processor->dataFiles->plugsList->plugins[processor->dataFiles->pluginEffects[index]]->name;
						//
						mp.addItem(index + 1, theName);
					}
			}
			//
			mp.addSeparator();
			//
			ARRAY_Iterator(processor->dataFiles->internalEffects)
			{
				mp.addItem(index + 99999, processor->dataFiles->internalEffects[index]);
			}
			//
			if (mp.getNumItems() > 0)
			{
				int result = mp.show();
				//
				if (result != 0)
				{
					((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->clearAndPrepareInterface();
					//
					if (result >= 99999)
					{
						processor->loadNewEffectDescription = processor->loadNewEffectName = processor->dataFiles->internalEffects[result - 99999];
						processor->loadNewEffectType = "INTERNAL";
					}
					else
					{
						processor->loadNewEffectDescription = processor->dataFiles->plugsList->plugins[processor->dataFiles->pluginEffects[result - 1]]->createIdentifierString();
						processor->loadNewEffectName = processor->dataFiles->plugsList->plugins[processor->dataFiles->pluginEffects[result - 1]]->name;
						processor->loadNewEffectType = "PLUGIN";
					}
					//
					processor->programs[processor->currentProgram]->showLayers[processor->currentZone][2] = true;
					processor->newAction(kActionLoadNewEffect);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WLayerComponent::paint(Graphics& g)
{
	WusikZrAudioProcessor* processor = (WusikZrAudioProcessor*)owner;
	//
	g.drawImageAt(background, 0, 0);
	//
	if (step != -1)
	{
		g.setColour(processor->dataFiles->stepColour);
		g.setFont(processor->dataFiles->stepFont);
		g.drawText(String(step + 1), processor->dataFiles->stepPosition, Justification::centred);
	}
	//
	Point<int> e = getMouseXYRelative();
	//
	if (buttonPos.contains(e))
	{
		g.setColour(Colours::red.withAlpha(0.18f));
		g.fillRect(buttonPos);
	}
	else if (buttonPos2.contains(e))
	{
		g.setColour(Colours::red.withAlpha(0.18f));
		g.fillRect(buttonPos2);
	}
	//
	if (type == kEffectLayer)
	{
		g.addTransform(AffineTransform::rotation(MathConstants<float>::halfPi, processor->dataFiles->effectsNamePosition.getX(), processor->dataFiles->effectsNamePosition.getY()));
		//
		g.setColour(processor->dataFiles->effectsNameColour);
		g.setFont(processor->dataFiles->effectsNameFont);
		g.drawText(pluginName, processor->dataFiles->effectsNamePosition, Justification::centredTop);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessorEditor::updateEnvelopesVisible()
{
	MessageManagerLock lock;
	//
	for (int zones = 0; zones < MAX_ZONES; zones++)
	{
		for (int xx = 0; xx < 8; xx++)
		{
			wusikKnobs_Envelopes[zones][xx]->setVisible(processor.editingEnvelope == 0);
			wusikKnobs_Envelopes[zones][xx + 8]->setVisible(processor.editingEnvelope == 1);
			wusikKnobs_Envelopes[zones][xx + 16]->setVisible(processor.editingEnvelope == 2);
		}
	}
	repaint();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikZrAudioProcessorEditor::updateZonesVisible(bool hideAll)
{
	MessageManagerLock lock;
	//
	for (int zones = 0; zones < MAX_ZONES; zones++)
	{
		if (zoneComponents[zones] != nullptr) zoneComponents[zones]->setVisible(!hideAll && processor.currentZone == zones);
	}
	repaint();
}