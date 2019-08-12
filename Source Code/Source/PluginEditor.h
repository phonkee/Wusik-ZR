// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

	Created by William Kalfelz @ Wusik Dot Com (c) 2019

*/

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WPluginListWindow : public DocumentWindow
{
public:
	WPluginListWindow(void* _owner, WusikDataFiles* _dataFiles);
	~WPluginListWindow();
	void closeButtonPressed();
	//
	WusikDataFiles* dataFiles;
	void* wowner;
	//
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WPluginListWindow)
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WLookAndFeel : public LookAndFeel_V4
{
public:
	void drawScrollbar(Graphics& g, ScrollBar& scrollbar, int x, int y, int width, int height,
		bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown)
	{
		ignoreUnused(isMouseDown);

		Rectangle<int> thumbBounds;

		if (isScrollbarVertical)
			thumbBounds = { x, thumbStartPosition + scrollBarVReduce, width, thumbSize - (scrollBarVReduce * 2)};
		else
			thumbBounds = { thumbStartPosition, y, thumbSize, height };

		auto c = scrollbar.findColour(ScrollBar::ColourIds::thumbColourId);
		g.setColour(isMouseOver ? c.brighter(0.25f) : c);
		g.fillRoundedRectangle(thumbBounds.reduced(1).toFloat(), 4.0f);
	}
	//
	int scrollBarVReduce;
};

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class WusikZrAudioProcessorEditor  : public AudioProcessorEditor, public Timer
{
public:
    WusikZrAudioProcessorEditor (WusikZrAudioProcessor&);
    ~WusikZrAudioProcessorEditor();

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    void paint (Graphics&) override;
    void resized() override;
	void mouseUp(const MouseEvent& event);
	void mouseDown(const MouseEvent& event);
	void mouseEnter(const MouseEvent& event) { repaint(); };
	void mouseExit(const MouseEvent& event) { repaint(); };
	void mouseMove(const MouseEvent& event) { repaint(); };
	void updateEntireInterface();
	void closeTheInterface();
	void clearInterfaceObjects(int zone);
	void timerCallback();
	void updateEnvelopesVisible();
	void updateZonesVisible(bool hideAll = false);
	void clearAndPrepareInterface();
	bool confirmBox(String message);
	//
    WusikZrAudioProcessor& processor;
	OwnedArray<WusikKnob> wusikKnobs[MAX_ZONES];
	OwnedArray<WusikKnob> wusikKnobs_Envelopes[MAX_ZONES];
	bool updateInterface, interfaceCleared, clearInterface, firstInterface, checkAudioInterface;
	ScopedPointer<WPluginListWindow> pluginListWindow;
	//
	ScopedPointer<WLookAndFeel> wLookAndFeel;
	ScopedPointer<TooltipWindow> toolTipWindow;
	Viewport* layersViewPort[MAX_ZONES];
	ScopedPointer<Component> layersViewPortComponent[MAX_ZONES];
	WImageButton* tempLayersButton;
	WImageButton* tempImage;
	WLayerComponent* firstStaticLayer, *firstSequencedLayer;
	WAboutBox* aboutBox;
	Component* zoneComponents[MAX_ZONES];
	WValueLabel* displayValue;
	WusikKnob* tempWButton;
	ResizableCornerComponent* resizer;
	ComponentBoundsConstrainer resizeLimits;
	Image cachedImage;
	ImageButton* cachedImageButton;
	bool showDM;
	//
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WusikZrAudioProcessorEditor)
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define WKNOB(image, frameH, frames, skinName) tempWButton = new WusikKnob(&processor, &processor.dataFiles->allImages[image], zones, parameterPosition, 10, 10, 0, frameH, frames, skinName); newLayer->addAndMakeVisible(tempWButton); parameterPosition++
#define WKNOBTEXT(skinName) tempWButton = new WusikKnob(&processor, nullptr, zones, parameterPosition, 10, 10, 0, 32, 0, skinName); newLayer->addAndMakeVisible(tempWButton); parameterPosition++
#define WKNOB2(image, frameH, frames, skinName) tempWButton = new WusikKnob(&processor, &processor.dataFiles->allImages[image], zones, parameterPosition, 10, 10, 0, frameH, frames, skinName); tempComponent->addAndMakeVisible(tempWButton); parameterPosition++
#define WKNOBTEXT2(skinName) tempWButton = new WusikKnob(&processor, nullptr, zones, parameterPosition, 10, 10, 0, 32, 0, skinName); tempComponent->addAndMakeVisible(tempWButton); parameterPosition++
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define WADD_BUTTON_IMAGE(yPos, variable, image_definition, type) 	variable = new WImageButton(type, &processor, zones);\
	variable->setBounds(0, yPos, processor.dataFiles->allImages[image_definition].getWidth(), processor.dataFiles->allImages[image_definition].getHeight());\
	variable->setImages(true, false, true,\
	processor.dataFiles->allImages[image_definition], 1.0f, Colours::transparentBlack,\
	processor.dataFiles->allImages[image_definition], 1.0f, Colours::red.withAlpha(0.14f),\
	processor.dataFiles->allImages[image_definition], 1.0f, Colours::transparentBlack);\
	layersViewPortComponent[zones]->addAndMakeVisible(variable)
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define WADD_BUTTON_IMAGE2(yPos, variable, image_definition, type) 	variable = new WImageButton(type, &processor, zones);\
	variable->setBounds(0, yPos, processor.dataFiles->allImages[image_definition].getWidth(), processor.dataFiles->allImages[image_definition].getHeight());\
	variable->setImages(true, false, true,\
	processor.dataFiles->allImages[image_definition], 1.0f, Colours::transparentBlack,\
	processor.dataFiles->allImages[image_definition], 0.92f, Colours::red.withAlpha(0.92f),\
	processor.dataFiles->allImages[image_definition], 1.0f, Colours::transparentBlack);\
	layersViewPortComponent[zones]->addAndMakeVisible(variable)
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define WIMAGE(image_definition) processor.dataFiles->allImages[image_definition]