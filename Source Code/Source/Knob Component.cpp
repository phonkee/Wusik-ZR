// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

	Created by William Kalfelz @ Wusik Dot Com (c) 2019

*/
//
#pragma once
#include "PluginEditor.h"
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
WusikKnob::WusikKnob(WusikZrAudioProcessor* _processor, Image *_theImage, int _zone, int _parameterIndex, int _X, int _Y, int _Width, int _frameHeight, int _numberOfFrames, String extraParSkinName, bool _isWaveform, int _layer, int _step)
	: theImage(_theImage), initialValue(0.0f), frameHeight(_frameHeight), numberOfFrames(_numberOfFrames), processor(_processor), 
	parameterIndex(_parameterIndex), isVertical(true), isPopupMenu(false), hasMoved(false), isWaveform(_isWaveform), zone(_zone), layer(_layer), step(_step)
{
	theParameter = nullptr;
	setComponentID("WusikKnob");
	//
	if (parameterIndex != -1)
	{
		theParameter = (WusikZRParameter*)processor->getParameters().getUnchecked(parameterIndex);
		theParameter->knob = this;
		setTooltip(theParameter->getTooltip());
		//
		parSkinName = extraParSkinName + theParameter->skinName;
		//
		if (theImage != nullptr)
		{
			if (isWaveform)
			{
				if (theParameter->menuOptions.size() > 0) isPopupMenu = true;
				String xSkinData = processor->dataFiles->skinSettings->getValue(parSkinName, String(_X) + "," + String(_Y) + ",100,100,Verdana,12.0,#FF000000");
				//
				_X = xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue();
				xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
				_Y = xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue();
				xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
				_Width = xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue();
				xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
				frameHeight = xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue();
				xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
				String fontName = xSkinData.upToFirstOccurrenceOf(",", false, true);
				xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
				float fontSize = xSkinData.upToFirstOccurrenceOf(",", false, true).getDoubleValue();
				xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
				fontColour = Colour::fromString(xSkinData);
				//
				theFont = Font(fontName, fontSize, 0);
			}
			else
			{
				String xSkinData = processor->dataFiles->skinSettings->getValue(parSkinName, String(_X) + "," + String(_Y) + ",0," + String(frameHeight) + "," + String(numberOfFrames));
				//
				_X = xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue();
				xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
				_Y = xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue();
				xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
				// Width //
				xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
				frameHeight = xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue();
				xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
				numberOfFrames = xSkinData.getIntValue();
			}
		}
		else
		{
			if (theParameter->menuOptions.size() > 0) isPopupMenu = true;
			String xSkinData = processor->dataFiles->skinSettings->getValue(parSkinName, String(_X) + "," + String(_Y) + ",100,100,Verdana,12.0,#FF000000");
			//
			_X = xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue();
			xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
			_Y = xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue();
			xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
			_Width = xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue();
			xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
			frameHeight = xSkinData.upToFirstOccurrenceOf(",", false, true).getIntValue();
			xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
			String fontName = xSkinData.upToFirstOccurrenceOf(",", false, true);
			xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
			float fontSize = xSkinData.upToFirstOccurrenceOf(",", false, true).getDoubleValue();
			xSkinData = xSkinData.fromFirstOccurrenceOf(",", false, true);
			fontColour = Colour::fromString(xSkinData);
			//
			theFont = Font(fontName, fontSize, 0);
		}
	}
	//
	if (theImage != nullptr)
	{
		setBounds(_X, _Y, theImage->getWidth(), frameHeight);
	}
	else
	{
		setBounds(_X, _Y, _Width, frameHeight);
	}
	//
	setWantsKeyboardFocus(true);
	addKeyListener(this);
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::paint(Graphics& g)
{
	if (theImage != nullptr)
	{
		if (theParameter != nullptr)
		{
			if (isWaveform)
			{
				if (theImage->getWidth() > 0)
				{
					if (isMouseOverOrDragging()) g.setOpacity(0.58f); else g.setOpacity(1.0);
					//
					int theWave = jlimit(0, 47, int(theParameter->getValue() * 47.0f));
					g.drawImageAt(*theImage, 0, 0);
					g.drawImage(processor->wavetables[processor->currentZone][theWave].waveform, 0, 0, theImage->getWidth(), theImage->getHeight(), 0, 0, processor->wavetables[processor->currentZone][theWave].waveform.getWidth(), processor->wavetables[processor->currentZone][theWave].waveform.getHeight());
					//
					g.setFont(theFont);
					g.setColour(fontColour);
					int theBank = (processor->getParameters()[parameterIndex]->getValue() * 4.0f);
					String theText;
					//
					int xBank = (theParameter->getValue() * 47.0f) / 12.0f;
					int xWave = (theParameter->getValue() * 47.0f) - (float(xBank) * 12.0f);
					if (xBank == 0) theText = "A";
					if (xBank == 1) theText = "B";
					if (xBank == 2) theText = "C";
					if (xBank == 3) theText = "D";
					theText.append(String(xWave + 1), 99);
					//
					g.drawText(theText, 0, 0, getWidth(), getHeight(), Justification::centred);
				}
			}
			else
			{
				if (theParameter->isBoolean() && isMouseOverOrDragging()) g.setOpacity(0.52f); else g.setOpacity(1.0);
				//
				if (theParameter->name.containsIgnoreCase("L.Add"))
				{
					int xvalue = 0;
					if (theParameter->getValue() >= LAYER_MUL_BOOST1) xvalue = 1;
					//
					g.drawImage(*theImage, 0, 0, getWidth(), getHeight(), 0, xvalue * frameHeight, getWidth(), frameHeight);
				}
				else
				{
					int xvalue = float(theParameter->getValue()) * (numberOfFrames - 1);
					g.drawImage(*theImage, 0, 0, getWidth(), getHeight(), 0, xvalue * frameHeight, getWidth(), frameHeight);
				}
			}
		}
	}
	else
	{
		if (theParameter != nullptr)
		{
			if (processor->editingEnabled && processor->editingEnabledDrawRectangles)
			{
				static Colour rectColour = Colour::fromString(processor->dataFiles->skinSettings->getValue("Edit Skin Draw Text Rectangles Colour"));
				g.setColour(rectColour);
				g.drawRect(0, 0, getWidth(), getHeight());
			}
			//
			g.setFont(theFont);
			g.setColour(Colours::black);
			g.drawText(theParameter->getText(theParameter->getValue(), 255), 0, 0, getWidth(), getHeight(), Justification::centred);
			//
			if (isMouseOverOrDragging())
			{
				g.setColour(Colours::orangered.withAlpha(0.42f));
				g.drawText(theParameter->getText(theParameter->getValue(), 255), 0, 0, getWidth(), getHeight(), Justification::centred);
			}
		}
	}
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::mouseExit(const MouseEvent& e)
{
	((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->displayValue->setVisible(false);
	repaint();
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::mouseDown(const MouseEvent& e)
{
	hasMoved = false;
	//
	if (parameterIndex != -1 && processor->editingEnabled)
	{
		WValueLabel *theLabel = ((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->displayValue;
		theLabel->setText(theParameter->name);
		theLabel->setBounds(getX(), getY() + getHeight() + 4, 120, 20);
		theLabel->setColour(Label::ColourIds::textColourId, Colours::red);
		theLabel->setVisible(true);
	}
	//
	initialValue = theParameter->getValue();
	if (parameterIndex != -1 && !e.mods.isPopupMenu())
	{
		if (theParameter->name.containsIgnoreCase("L.Add"))
		{
			float newValue = 0.0f;
			if (initialValue == LAYER_ADD_BOOST1) newValue = LAYER_MUL_BOOST1;
			else if (initialValue == LAYER_ADD_BOOST2) newValue = LAYER_MUL_BOOST2;
			else if (initialValue == LAYER_ADD_BOOST4) newValue = LAYER_MUL_BOOST4;
			else if (initialValue == LAYER_MUL_BOOST1) newValue = LAYER_ADD_BOOST1;
			else if (initialValue == LAYER_MUL_BOOST2) newValue = LAYER_ADD_BOOST2;
			else if (initialValue == LAYER_MUL_BOOST4) newValue = LAYER_ADD_BOOST4;
			//
			theParameter->setValue(newValue);
			processor->setParameterNotifyingHost(parameterIndex, theParameter->getValue());
		}
		else
		{
			if (theParameter->isBoolean())
			{
				processor->beginParameterChangeGesture(parameterIndex);
				if (theParameter->getValue() > 0.0f) theParameter->setValue(0.0f); else theParameter->setValue(1.0f);
				if (parameterIndex != -1) processor->setParameterNotifyingHost(parameterIndex, theParameter->getValue());
				processor->endParameterChangeGesture(parameterIndex);
				return;
			}
			processor->beginParameterChangeGesture(parameterIndex);
		}
	}
	//
	if (processor->editingEnabled && e.mods.isMiddleButtonDown())
	{
		initialPosition.x = getBounds().getX();
		initialPosition.y = getBounds().getY();
	}
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::mouseEnter(const MouseEvent& e)
{
	if (parameterIndex != -1 && theImage != nullptr && numberOfFrames > 2 && !theParameter->name.containsIgnoreCase("step"))
	{
		WValueLabel *theLabel = ((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->displayValue;
		theLabel->setVisible(false);
		theLabel->setBounds(0, 0, processor->dataFiles->valueDisplayW, processor->dataFiles->valueDisplayH);
		//
		int pX = getX();
		int pY = getY();
		//
		int index = 0;
		Component* xComp = getParentComponent();
		while (xComp != nullptr)
		{
			if (xComp->getComponentID().containsIgnoreCase(" layer") ||
				xComp->getComponentID().containsIgnoreCase("Viewport Master") ||
				xComp->getComponentID().containsIgnoreCase("Viewport Effects") ||
				xComp->getComponentID().containsIgnoreCase("WusikSteps"))
			{
				pX += xComp->getX();
				pY += xComp->getY();
				//
				if (xComp->getComponentID().containsIgnoreCase("Viewport Master"))
				{
					pY -= ((Viewport*)xComp)->getVerticalScrollBar().getCurrentRangeStart();
				}
				//
				if (xComp->getComponentID().containsIgnoreCase("Viewport Effects"))
				{
					pY -= ((Viewport*)xComp)->getVerticalScrollBar().getCurrentRangeStart();
				}
			}
			//
			index++;
			xComp = xComp->getParentComponent();
			if (index > 99) break;
		}		
		//
		theLabel->setTopLeftPosition(
			Point<int>(pX + (getWidth() / 2) - (processor->dataFiles->valueDisplayW / 2), pY + getHeight()));
		//
		theLabel->rectangleBorder = processor->dataFiles->valueDisplayRectangleThickness;
		theLabel->rectangleColour = processor->dataFiles->valueDisplayRectangleColour;
		theLabel->textColour = processor->dataFiles->valueDisplayTextColour;
		theLabel->textFont = processor->dataFiles->valueDisplayFont;
		theLabel->background = processor->dataFiles->valueDisplayBackgroundColour;
		theLabel->setText(theParameter->getText(theParameter->getValue(), 99));
		theLabel->setVisible(true);
		theLabel->toFront(false);
	}
	//
	repaint();
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::mouseDrag(const MouseEvent& e)
{
	hasMoved = true;
	//
	if (processor->editingEnabled && e.mods.isMiddleButtonDown())
	{
		setTopLeftPosition(
			initialPosition.x + e.getDistanceFromDragStartX(),
			initialPosition.y + e.getDistanceFromDragStartY()
			);
		repaint();
		return;
	}
	//
	if (theParameter != nullptr)
	{
		if (theParameter->isBoolean()) return;
		if (theParameter->name.containsIgnoreCase("L.Add")) return;
		//
		float multiplyer = 1.0f;
		if (e.mods.isShiftDown() || e.mods.isPopupMenu()) multiplyer = 0.01f;
		//
		if (isWaveform) multiplyer = 0.4f;
		if (isWaveform && (e.mods.isShiftDown() || e.mods.isPopupMenu())) multiplyer = 0.02f;
		//
		if (theParameter->name.containsIgnoreCase("tune")) multiplyer = 0.1f;
		if (theParameter->name.containsIgnoreCase("tune") && (e.mods.isShiftDown() || e.mods.isPopupMenu())) multiplyer = 2.0f;
		//
		float newValue = jlimit(0.0f, 1.0f, initialValue - (float((isVertical) ? e.getDistanceFromDragStartY() : -e.getDistanceFromDragStartX()) * multiplyer * 0.006f));
		//
		theParameter->setValue(newValue);
		if (parameterIndex != -1) processor->setParameterNotifyingHost(parameterIndex, newValue);
		//
		((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->displayValue->setText(theParameter->getText(theParameter->getValue(), 99));
	}
	//
	repaint();
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::mouseUp(const MouseEvent& e)
{
	if (!hasMoved)
	{
		if (!e.mods.isPopupMenu() && isWaveform)
		{
			((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->displayValue->setVisible(false);
			//
			processor->waveformWindow = nullptr;
			//
			processor->waveformWindow = new WusikKnobWaveformWindow(processor, this, layer, (step != -1) ? " - Step: " + String(step + 1) : "");
			if (parameterIndex != -1) processor->endParameterChangeGesture(parameterIndex);
			//
			return;
		}
		else if (!e.mods.isPopupMenu() && parameterIndex != -1 && theParameter->menuOptions.size() >= 3)
		{
			PopupMenu mp;
			//
			ARRAY_Iterator(theParameter->menuOptions)
			{
				mp.addItem(index + 1, theParameter->menuOptions[index], true, int(theParameter->getValue() * theParameter->max) == (index + 1));
			}
			//
			((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->displayValue->setVisible(false);
			//
			int result = mp.show();
			if (result > 0)
			{
				theParameter->setValue(float(result) * (1.0f / theParameter->max));
				processor->endParameterChangeGesture(parameterIndex);
			}
		}
		else if (e.mods.isPopupMenu() && parameterIndex != -1)
		{
			((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->displayValue->setVisible(false);
			//
			PopupMenu mp;
			//
			if (processor->midiLearn != -1) mp.addItem(4, "Cancel Learn MIDI Remote");
			else if (theParameter->midiRemote == WMidiLearned::kMIDIRemoteNone) mp.addItem(2, "Learn MIDI Remote");
			else if (theParameter->midiRemote == WMidiLearned::kMIDIRemoteATouch) mp.addItem(1, "Remove ATouch Remote");
			else if (theParameter->midiRemote == WMidiLearned::kMIDIRemotePBend) mp.addItem(1, "Remove PBend Remote");
			else mp.addItem(1, "Remove MIDI Remote CC# " + String(theParameter->midiRemote));
			//
			if (theParameter->name.containsIgnoreCase("L.Add"))
			{
				mp.addSeparator();
				mp.addItem(40, "Don't Boost Volume", true, theParameter->getValue() == LAYER_ADD_BOOST1 || theParameter->getValue() == LAYER_MUL_BOOST1);
				mp.addItem(42, "Boost Volume x2", true, theParameter->getValue() == LAYER_ADD_BOOST2 || theParameter->getValue() == LAYER_MUL_BOOST2);
				mp.addItem(44, "Boost Volume x4", true, theParameter->getValue() == LAYER_ADD_BOOST4 || theParameter->getValue() == LAYER_MUL_BOOST4);
			}
			//
			int result = mp.show();
			if (result == 2)
			{
				processor->midiLearn = parameterIndex;
			}
			else if (result == 40)
			{
				if (theParameter->getValue() <= LAYER_ADD_BOOST4) theParameter->setValue(LAYER_ADD_BOOST1); else theParameter->setValue(LAYER_MUL_BOOST1);
			}
			else if (result == 42)
			{
				if (theParameter->getValue() <= LAYER_ADD_BOOST4) theParameter->setValue(LAYER_ADD_BOOST2); else theParameter->setValue(LAYER_MUL_BOOST2);
			}
			else if (result == 44)
			{
				if (theParameter->getValue() <= LAYER_ADD_BOOST4) theParameter->setValue(LAYER_ADD_BOOST4); else theParameter->setValue(LAYER_MUL_BOOST4);
			}
			else if (result == 4)
			{
				processor->midiLearn = -1;
			}
			else if (result == 1)
			{
				theParameter->midiRemote = -1;
				processor->redoMidiLearned();
			}
		}
	}
	else
	{
		if (parameterIndex != -1 && theParameter->name.containsIgnoreCase("L.Add")) return;
		if (parameterIndex != -1 && !theParameter->isBoolean()) processor->endParameterChangeGesture(parameterIndex);
	}
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::mouseDoubleClick(const MouseEvent& e)
{
	if (parameterIndex != -1 && !theParameter->isBoolean())
	{
		if (theParameter->name.containsIgnoreCase("L.Add")) return;
		theParameter->setValue(0.5f);
		if (parameterIndex != -1) processor->setParameterNotifyingHost(parameterIndex, 0.5f);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool WusikKnob::keyPressed(const KeyPress &key, Component* component)
{
	if (processor->editingEnabled)
	{
		if (key.getModifiers().isCtrlDown())
		{
			if (key.getKeyCode() == KeyPress::upKey)
			{
				theFont = theFont.withHeight(theFont.getHeight() + 0.5f);
				repaint();
				return true;
			}
			else if (key.getKeyCode() == KeyPress::downKey)
			{
				theFont = theFont.withHeight(theFont.getHeight() - 0.5f);
				repaint();
				return true;
			}
		}
		else
		{
			if (key.getKeyCode() == KeyPress::upKey)
			{
				if (key.getModifiers().isShiftDown()) setSize(getWidth(), getHeight() + 1);
				else setTopLeftPosition(getBounds().getX(), getBounds().getY() - 1);
				processor->lastYPositionEditing = getBounds().getY();
				repaint();
				return true;
			}
			else if (key.getKeyCode() == KeyPress::downKey)
			{
				if (key.getModifiers().isShiftDown()) setSize(getWidth(), getHeight() - 1);
				else setTopLeftPosition(getBounds().getX(), getBounds().getY() + 1);
				processor->lastYPositionEditing = getBounds().getY();
				repaint();
				return true;
			}
			else if (key.getKeyCode() == KeyPress::leftKey)
			{
				if (key.getModifiers().isShiftDown()) setSize(getWidth() - 1, getHeight());
				else setTopLeftPosition(getBounds().getX() - 1, getBounds().getY());
				processor->lastXPositionEditing = getBounds().getX();
				repaint();
				return true;
			}
			else if (key.getKeyCode() == KeyPress::rightKey)
			{
				if (key.getModifiers().isShiftDown()) setSize(getWidth() + 1, getHeight());
				else setTopLeftPosition(getBounds().getX() + 1, getBounds().getY());
				processor->lastXPositionEditing = getBounds().getX();
				repaint();
				return true;
			}
			else if (key.getKeyCode() == KeyPress::returnKey)
			{
				processor->editingEnabledDrawRectangles = !processor->editingEnabledDrawRectangles;
				processor->dataFiles->skinSettings->setValue("Edit Skin Draw Text Rectangles", processor->editingEnabledDrawRectangles);
				repaint();
				getParentComponent()->repaint();
				return true;
			}
			else if (key.getKeyCode() == KeyPress::spaceKey)
			{
				if (key.getModifiers().isShiftDown()) setTopLeftPosition(processor->lastXPositionEditing, getBounds().getY());
				else setTopLeftPosition(getBounds().getX(), processor->lastYPositionEditing);
				repaint();
				return true;
			}
		}
	}
	else
	{
		if (theParameter != nullptr)
		{
			if (theParameter->name.containsIgnoreCase("L.Add")) return false;
			//
			float newValue = theParameter->getValue();
			//
			float theRate = 0.01f;
			if (key.getModifiers().isShiftDown()) theRate = 0.001f;
			if (theParameter->rate == 1.0f)
			{
				theRate = 1.0f / fabs(theParameter->max - theParameter->min);
			}
			//
			if (key.getKeyCode() == KeyPress::upKey) newValue += theRate;
			else if (key.getKeyCode() == KeyPress::downKey) newValue -= theRate;
			//
			newValue = jlimit(0.0f, 1.0f, newValue);
			theParameter->setValue(newValue);
			if (parameterIndex != -1) processor->setParameterNotifyingHost(parameterIndex, newValue);
			((WusikZrAudioProcessorEditor*)processor->getActiveEditor())->displayValue->setText(theParameter->getText(theParameter->getValue(), 99));
			repaint();
			return true;
		}
	}
	//
	return false;
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::WusikKnobWaveformWindow::paint(Graphics& g)
{
	if (wusikKnob == nullptr || wusikKnob->theParameter == nullptr) return;
	//
	g.drawImageAt(*theImageBackground, 0, 0);
	//
	int theWave = jlimit(0, 47, int(wusikKnob->theParameter->getValue() * 47.0f));
	//
	for (int xw = 0; xw < 48; xw++)
	{
		if (xw == theWave)
		{
			g.setColour(processor->dataFiles->waveformSelectedColour);
			g.fillRect(wavePositions[xw].getX(), wavePositions[xw].getY(), waveformSize.getX(), waveformSize.getY());
		}
		//
		g.setColour(Colours::black);
		g.drawImage(processor->wavetables[processor->currentZone][xw].waveform, wavePositions[xw].getX(), wavePositions[xw].getY(), waveformSize.getX(), waveformSize.getY(),
			0, 0, processor->wavetables[processor->currentZone][xw].waveform.getWidth(), processor->wavetables[processor->currentZone][xw].waveform.getHeight());
	}
	//
	g.drawImage(processor->wavetables[processor->currentZone][theWave].waveform, processor->dataFiles->waveWindowBig.getX(), processor->dataFiles->waveWindowBig.getY(),
		processor->dataFiles->waveWindowBig.getWidth(), processor->dataFiles->waveWindowBig.getHeight(),
		0, 0, processor->wavetables[processor->currentZone][theWave].waveform.getWidth(), processor->wavetables[processor->currentZone][theWave].waveform.getHeight());
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::WusikKnobWaveformWindow::mouseUp(const MouseEvent& e)
{
	if (wusikKnob == nullptr || wusikKnob->theParameter == nullptr) return;
	//
	if (e.mods.isPopupMenu())
	{
		PopupMenu mp;
		mp.addItem(1, "Auto Close On Selection", true, autoClose);
		int result = mp.show();
		if (result == 1)
		{
			autoClose = !autoClose;
			processor->dataFiles->userSettings->setValue("Waveform Window AutoClose", autoClose);
		}
	}
	else
	{
		for (int xw = 0; xw < 48; xw++)
		{
			if (wavePositions[xw].contains(e.getPosition()))
			{
				wusikKnob->theParameter->setValue((double(xw) * (1.0 / 47.0)) + 0.001);
				repaint();
				if (autoClose)
				{
					wusikKnob = nullptr;
					processor->dataFiles->userSettings->setValue("Waveform Window X", getBounds().getX());
					processor->dataFiles->userSettings->setValue("Waveform Window Y", getBounds().getY());
					setVisible(false);
				}
				return;
			}
		}
	}
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::WusikKnobWaveformWindow::mouseDoubleClick(const MouseEvent& e)
{
	closeButtonPressed();
}
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::WusikKnobWaveformWindow::closeButtonPressed()
{
	wusikKnob = nullptr;
	processor->dataFiles->userSettings->setValue("Waveform Window X", getBounds().getX());
	processor->dataFiles->userSettings->setValue("Waveform Window Y", getBounds().getY());
	setVisible(false);
};
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void WusikKnob::WusikKnobWaveformWindow::init()
{
	theImageBackground = &processor->dataFiles->allImages[kBackground_WaveformWindow];
	//
	String theData = processor->dataFiles->skinSettings->getValue("Waveform Window Cell Start");
	waveformStart.setX(theData.upToFirstOccurrenceOf(",", false, true).getIntValue());
	waveformStart.setY(theData.fromFirstOccurrenceOf(",", false, true).getIntValue());
	//
	theData = processor->dataFiles->skinSettings->getValue("Waveform Window Cell Total Size");
	waveformTotalSize.setX(theData.upToFirstOccurrenceOf(",", false, true).getIntValue());
	waveformTotalSize.setY(theData.fromFirstOccurrenceOf(",", false, true).getIntValue());
	//
	theData = processor->dataFiles->skinSettings->getValue("Waveform Window Cell Size");
	waveformSize.setX(theData.upToFirstOccurrenceOf(",", false, true).getIntValue());
	waveformSize.setY(theData.fromFirstOccurrenceOf(",", false, true).getIntValue());
	//
	int pX = waveformStart.getX();
	int pY = waveformStart.getY();
	//
	for (int xw = 0; xw < 48; xw++)
	{
		wavePositions[xw].setBounds(pX, pY, waveformTotalSize.getX(), waveformTotalSize.getY());
		//
		pY += waveformTotalSize.getY();
		if (pY >= (waveformTotalSize.getY() * 6))
		{
			pY = waveformStart.getY();
			pX += waveformTotalSize.getX();
		}
	}
	//
	autoClose = processor->dataFiles->userSettings->getBoolValue("Waveform Window AutoClose", false);
	//
	setUsingNativeTitleBar(true);
	setSize(theImageBackground->getWidth(), theImageBackground->getHeight());
	setAlwaysOnTop(true);
	//
	if (processor->dataFiles->userSettings->getIntValue("Waveform Window X", -1) != -1)
	{
		setBounds(processor->dataFiles->userSettings->getIntValue("Waveform Window X"),
			processor->dataFiles->userSettings->getIntValue("Waveform Window Y"),
			getWidth(),
			getHeight());
	}
	else centreWithSize(getWidth(), getHeight());
	//
	setVisible(true);
}
//
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
WusikKnob::WusikKnobWaveformWindow::~WusikKnobWaveformWindow()
{
	if (isVisible())
	{
		processor->dataFiles->userSettings->setValue("Waveform Window X", getBounds().getX());
		processor->dataFiles->userSettings->setValue("Waveform Window Y", getBounds().getY());
	}
};