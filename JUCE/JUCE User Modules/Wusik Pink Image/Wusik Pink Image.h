/*
BEGIN_JUCE_MODULE_DECLARATION
    ID:               WPImage
    vendor:           Wusik Dot Com
    version:          1.0.0
    name:             Wusik Pink Image
    description:      Creates multiple images from a single image using pink-lines to determinate where to cut.
    website:          https://www.Wusik.com
    dependencies:     
	license:          Open-Source
END_JUCE_MODULE_DECLARATION 
*/

#ifndef WPIMAGE_H_INCLUDED
#define WPIMAGE_H_INCLUDED

// -------------------------------------------------------------------------------------------------------------------------------
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_data_structures/juce_data_structures.h>
#define PINK_COLOUR juce::Colour::fromString("ffff00ff")

class WusikPinkImage
{
public:

	// -------------------------------------------------------------------------------------------------------------------------------
	static void stripSquare(juce::Image original, juce::Image destination[9], int height = 0, int offsetY = 0)
	{
		if (height == 0) height = original.getHeight();
		int Corners[4] = {0,0,0,0}; // Top-Left X and Y, Bottom-Right X and Y //

		for (int x=0; x<original.getWidth(); x++) { if (original.getPixelAt(x,offsetY) != PINK_COLOUR) break; Corners[0] = x-1; }
		for (int x=0; x<height; x++) { if (original.getPixelAt(0,x+offsetY) != PINK_COLOUR) break; Corners[1] = x-1; }
				
		for (int x=original.getWidth()-1; x>=0; x--) { if (original.getPixelAt(x,height-1+offsetY) != PINK_COLOUR) break; Corners[2] = x-2; }
		for (int x=(height-1); x>=0; x--) { if (original.getPixelAt(original.getWidth()-1,x+offsetY) != PINK_COLOUR) break; Corners[3] = x-2; }
							
		/*	
			0 1 2
			3 4 5
			6 7 8
		*/

		// Create Buttons //
		int bW = original.getWidth()-2;
		int bH = height-2;
		#define CreateBT(BT,W,H) destination[BT] = juce::Image(juce::Image::ARGB,W,H,true);

		CreateBT(0, Corners[0], Corners[1]);
		CreateBT(1, Corners[2]-Corners[0], Corners[1]);
		CreateBT(2, bW-Corners[2], Corners[1]);
		//
		CreateBT(3, Corners[0], Corners[3]-Corners[1]);
		CreateBT(4, Corners[2]-Corners[0], Corners[3]-Corners[1]);
		CreateBT(5, bW-Corners[2], Corners[3]-Corners[1]);
		//
		CreateBT(6, Corners[0], bH-Corners[3]);
		CreateBT(7, Corners[2]-Corners[0], bH-Corners[3]);
		CreateBT(8, bW-Corners[2], bH-Corners[3]);

		// Perform Copies //
		int pX = 0;
		int pY = 0;

		for (int bt=0; bt<9; bt++)
		{
			switch(bt)
			{
				case 1:	pX = Corners[0];	pY = 0;				break;
				case 2:	pX = Corners[2];	pY = 0;				break;
				case 3:	pX = 0;           pY = Corners[1];	break;
				case 4:	pX = Corners[0];	pY = Corners[1];	break;
				case 5:	pX = Corners[2];	pY = Corners[1];	break;
				case 6:	pX = 0;           pY = Corners[3];	break;
				case 7:	pX = Corners[0];	pY = Corners[3];	break;
				case 8:	pX = Corners[2];	pY = Corners[3];	break;
			}

			for (int y=0; y<destination[bt].getHeight(); y++)
			{
				for (int x=0; x<destination[bt].getWidth(); x++)
				{
					destination[bt].setPixelAt(x,y,original.getPixelAt(x+1+pX,y+1+pY+offsetY));
				}
			}
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------------
	static void stripHorizontal(juce::Image original, juce::Image destination[3], int height = 0, int offsetY = 0)
	{
		if (height == 0) height = original.getHeight();
		int Corners[2] = {0,0}; // Left X  Right X //

		for (int x=0; x<original.getWidth(); x++) { if (original.getPixelAt(x,offsetY) != PINK_COLOUR) break; Corners[0] = x-1; }
		for (int x=original.getWidth()-1; x>=0; x--) { if (original.getPixelAt(x,offsetY) != PINK_COLOUR) break; Corners[1] = x-2; }
							
		/*
				0		1		2 
		*/

		// Create Buttons //
		int bW = original.getWidth()-2;
		#define CreateBT2(BT,W,H) destination[BT] = juce::Image(juce::Image::ARGB,W,H,true);

		CreateBT2(0, Corners[0], height-2);
		CreateBT2(1, Corners[1]-Corners[0], height-2);
		CreateBT2(2, bW-Corners[1], height-2);

		// Perform Copies //
		int pX = 0;

		for (int bt=0; bt<3; bt++)
		{
			switch(bt)
			{
				case 1:	pX = Corners[0]; break;
				case 2:	pX = Corners[1]; break;
			}

			for (int y=0; y<destination[bt].getHeight(); y++)
			{
				for (int x=0; x<destination[bt].getWidth(); x++)
				{
					destination[bt].setPixelAt(x,y,original.getPixelAt(x+1+pX,y+1+offsetY));
				}
			}
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------------
	static void stripVertical(juce::Image original, juce::Image destination[3], int width = 0, int offsetX = 0)
	{
		if (width == 0) width = original.getWidth();
		int Corners[2] = {0,0}; // Top Y  Bottom Y //

		for (int x=0; x<original.getHeight(); x++) { if (original.getPixelAt(offsetX,x) != PINK_COLOUR) break; Corners[0] = x-1; }
		for (int x=original.getHeight()-1; x>=0; x--) { if (original.getPixelAt(offsetX,x) != PINK_COLOUR) break; Corners[1] = x-2; }
							
		/* 
			0
			
			1
			
			2
		*/

		// Create Buttons //
		int bH = original.getHeight()-2;
		#define CreateBT3(BT,W,H) destination[BT] = juce::Image(juce::Image::ARGB,W,H,true);
		
		CreateBT3(0, width-2, Corners[0]);
		CreateBT3(1, width-2, Corners[1]-Corners[0]);
		CreateBT3(2, width-2, bH-Corners[1]);

		// Perform Copies //
		int pX = 0;

		for (int bt=0; bt<3; bt++)
		{
			switch(bt)
			{
				case 1:	pX = Corners[0]; break;
				case 2:	pX = Corners[1]; break;
			}

			for (int y=0; y<destination[bt].getHeight(); y++)
			{
				for (int x=0; x<destination[bt].getWidth(); x++)
				{
					destination[bt].setPixelAt(x,y,original.getPixelAt(x+1+offsetX,y+1+pX));
				}
			}
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------------
	static void drawImageSquare(juce::Graphics& g, juce::Image images[9], int x, int y, int width, int height, bool resampleImages = false, bool skinTiled = false, int* offSets = nullptr)
	{
		int thisOffset[2] = { 0,0 };
		if (offSets == nullptr) offSets = thisOffset;
		//
		if (resampleImages)
		{
			juce::Image xg(juce::Image::PixelFormat::ARGB, width, height, true);
			juce::Graphics gg(xg);
			//
			if (skinTiled)
			{
				gg.setTiledImageFill(images[4], images[3].getWidth(), images[1].getHeight(), 1.0f);
				gg.fillRect(images[3].getWidth() - offSets[0], images[1].getHeight() - offSets[0], width + offSets[1] - images[3].getWidth() - images[5].getWidth(), height + offSets[1] - images[1].getHeight() - images[7].getHeight());
				gg.setTiledImageFill(images[1], images[0].getWidth(), 0, 1.0f);
				gg.fillRect(images[0].getWidth() - offSets[0], 0, width + offSets[1] - images[0].getWidth() - images[2].getWidth(), images[1].getHeight());
				gg.setTiledImageFill(images[7], images[6].getWidth(), height - images[7].getHeight(), 1.0f);
				gg.fillRect(images[0].getWidth() - offSets[0], height - images[7].getHeight(), width + offSets[1] - images[6].getWidth() - images[8].getWidth(), images[7].getHeight());
				gg.setTiledImageFill(images[3], 0, images[0].getHeight(), 1.0f);
				gg.fillRect(0, images[0].getHeight() - offSets[0], images[3].getWidth(), height - images[0].getHeight() + offSets[1] - images[6].getHeight());
				gg.setTiledImageFill(images[5], width - images[5].getWidth(), 0, 1.0f);
				gg.fillRect(width - images[5].getWidth(), images[2].getHeight() - offSets[0], images[5].getWidth(), height + offSets[1] - images[2].getHeight() - images[8].getHeight());
			}
			else
			{
				gg.drawImage(images[4], images[3].getWidth() - offSets[0], images[1].getHeight() - offSets[0], width + offSets[1] - images[3].getWidth() - images[5].getWidth(), height + offSets[1] - images[1].getHeight() - images[7].getHeight(), 0, 0, images[4].getWidth(), images[4].getHeight());
				gg.drawImage(images[1], images[0].getWidth() - offSets[0], 0, width + offSets[1] - images[0].getWidth() - images[2].getWidth(), images[1].getHeight(), 0, 0, images[1].getWidth(), images[1].getHeight());
				gg.drawImage(images[7], images[0].getWidth() - offSets[0], height - images[7].getHeight(), width + offSets[1] - images[6].getWidth() - images[8].getWidth(), images[7].getHeight(), 0, 0, images[7].getWidth(), images[7].getHeight());
				gg.drawImage(images[3], 0, images[0].getHeight() - offSets[0], images[3].getWidth(), height - images[0].getHeight() + offSets[1] - images[6].getHeight(), 0, 0, images[3].getWidth(), images[3].getHeight());
				gg.drawImage(images[5], width - images[5].getWidth(), images[2].getHeight() - offSets[0], images[5].getWidth(), height + offSets[1] - images[2].getHeight() - images[8].getHeight(), 0, 0, images[5].getWidth(), images[5].getHeight());
			}
			gg.drawImageAt(images[0], 0, 0);
			gg.drawImageAt(images[2], width - images[2].getWidth(), 0);
			gg.drawImageAt(images[6], 0, height - images[6].getHeight());
			gg.drawImageAt(images[8], width - images[8].getWidth(), height - images[8].getHeight());

			g.drawImageAt(xg, x, y);
		}
		else
		{
			if (skinTiled)
			{
				g.setTiledImageFill(images[4], x + images[3].getWidth(), y + images[1].getHeight(), 1.0f);
				g.fillRect(x + images[3].getWidth(), y + images[1].getHeight(), width - images[3].getWidth() - images[5].getWidth(), height - images[1].getHeight() - images[7].getHeight());
				g.setTiledImageFill(images[1], x + images[0].getWidth(), y, 1.0f);
				g.fillRect(x + images[0].getWidth(), y, width - images[0].getWidth() - images[2].getWidth(), images[1].getHeight());
				g.setTiledImageFill(images[7], x + images[6].getWidth(), y + height - images[7].getHeight(), 1.0f);
				g.fillRect(x + images[6].getWidth(), y + height - images[7].getHeight(), width - images[6].getWidth() - images[8].getWidth(), images[7].getHeight());
				g.setTiledImageFill(images[3], x, y + images[0].getHeight(), 1.0f);
				g.fillRect(x, y + images[0].getHeight(), images[3].getWidth(), height - images[0].getHeight() - images[6].getHeight());
				g.setTiledImageFill(images[5], x + width - images[5].getWidth(), y, 1.0f);
				g.fillRect(x + width - images[5].getWidth(), y + images[2].getHeight(), images[5].getWidth(), height - images[2].getHeight() - images[8].getHeight());
			}
			else
			{
				g.drawImage(images[4], x + images[3].getWidth() - offSets[0], y + images[1].getHeight() - offSets[0], width + offSets[1] - images[3].getWidth() - images[5].getWidth(), height + offSets[1] - images[1].getHeight() - images[7].getHeight(), 0, 0, images[4].getWidth(), images[4].getHeight());
				g.drawImage(images[1], x + images[0].getWidth() - offSets[0], y, width + offSets[1] - images[0].getWidth() - images[2].getWidth(), images[1].getHeight(), 0, 0, images[1].getWidth(), images[1].getHeight());
				g.drawImage(images[7], x + images[0].getWidth() - offSets[0], y + height - images[7].getHeight(), width + offSets[1] - images[6].getWidth() - images[8].getWidth(), images[7].getHeight(), 0, 0, images[7].getWidth(), images[7].getHeight());
				g.drawImage(images[3], x, y + images[0].getHeight() - offSets[0], images[3].getWidth(), height - images[0].getHeight() + offSets[1] - images[6].getHeight(), 0, 0, images[3].getWidth(), images[3].getHeight());
				g.drawImage(images[5], x + width - images[5].getWidth(), y + images[2].getHeight() - offSets[0], images[5].getWidth(), height + offSets[1] - images[2].getHeight() - images[8].getHeight(), 0, 0, images[5].getWidth(), images[5].getHeight());
			}
			g.drawImageAt(images[0], x, y);
			g.drawImageAt(images[2], x + width - images[2].getWidth(), y);
			g.drawImageAt(images[6], x, y + height - images[6].getHeight());
			g.drawImageAt(images[8], x + width - images[8].getWidth(), y + height - images[8].getHeight());
		}
	}

	//-----------------------------------------------------------------------------------------------------------
	static void drawImageHorizontal(juce::Graphics& g, juce::Image images[3], int x, int y, int width, bool resampleImages = false, bool skinTiled = false, int* offSets = nullptr)
	{
		int thisOffset[2] = { 0,0 };
		if (offSets == nullptr) offSets = thisOffset;
		//
		if (resampleImages)
		{
			juce::Image xg(juce::Image::PixelFormat::ARGB, width, images[0].getHeight(), true);
			juce::Graphics gg(xg);

			if (skinTiled)
			{
				gg.setTiledImageFill(images[1], 0, images[0].getHeight(), 1.0f);
				gg.fillRect(images[0].getWidth() - offSets[0], 0, width - images[0].getWidth() - images[2].getWidth() + offSets[1], images[1].getHeight());
			}
			else gg.drawImage(images[1], images[0].getWidth() - offSets[0], 0, width - images[0].getWidth() - images[2].getWidth() + offSets[1], images[1].getHeight(), 0, 0, images[1].getWidth(), images[1].getHeight());
			gg.drawImageAt(images[0], 0, 0);
			gg.drawImageAt(images[2], width - images[2].getWidth(), 0);

			g.drawImageAt(xg, x, y);
		}
		else
		{
			if (skinTiled)
			{
				g.setTiledImageFill(images[1], x, y + images[0].getHeight(), 1.0f);
				g.fillRect(x + images[0].getWidth() - offSets[0], y, width - images[0].getWidth() - images[2].getWidth() + offSets[1], images[1].getHeight());
			}
			else g.drawImage(images[1], x + images[0].getWidth() - offSets[0], y, width - images[0].getWidth() - images[2].getWidth() + offSets[1], images[1].getHeight(), 0, 0, images[1].getWidth(), images[1].getHeight());
			g.drawImageAt(images[0], x, y);
			g.drawImageAt(images[2], x + width - images[2].getWidth(), y);
		}
	}

	//-----------------------------------------------------------------------------------------------------------
	static void drawImageVertical(juce::Graphics& g, juce::Image images[3], int x, int y, int height, bool resampleImages = false, bool skinTiled = false)
	{
		if (resampleImages)
		{
			juce::Image xg(juce::Image::PixelFormat::ARGB, images[0].getWidth(), height, true);
			juce::Graphics gg(xg);

			if (skinTiled)
			{
				gg.setTiledImageFill(images[1], 0, images[0].getHeight(), 1.0f);
				gg.fillRect(0, images[0].getHeight(), images[1].getWidth(), height - images[0].getHeight() - images[2].getHeight());
			}
			else gg.drawImage(images[1], 0, images[0].getHeight(), images[1].getWidth(), height - images[0].getHeight() - images[2].getHeight(), 0, 0, images[1].getWidth(), images[1].getHeight());
			gg.drawImageAt(images[0], 0, 0);
			gg.drawImageAt(images[2], 0, height - images[2].getHeight());

			g.drawImageAt(xg, x, y);
		}
		else
		{
			if (skinTiled)
			{
				g.setTiledImageFill(images[1], x, y + images[0].getHeight(), 1.0f);
				g.fillRect(x, y + images[0].getHeight(), images[1].getWidth(), height - images[0].getHeight() - images[2].getHeight());
			}
			else g.drawImage(images[1], x, y + images[0].getHeight(), images[1].getWidth(), height - images[0].getHeight() - images[2].getHeight(), 0, 0, images[1].getWidth(), images[1].getHeight());
			g.drawImageAt(images[0], x, y);
			g.drawImageAt(images[2], x, y + height - images[2].getHeight());
		}
	}
};
//
#endif