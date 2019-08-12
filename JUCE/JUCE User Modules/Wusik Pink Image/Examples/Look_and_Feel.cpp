// -------------------------------------------------------------------------------------------------------------------------------
void LookAndFeelEx::drawScrollbar (Graphics& g,
                                 ScrollBar& scrollbar,
                                 int x, int y,
                                 int width, int height,
                                 bool isScrollbarVertical,
                                 int thumbStartPosition,
                                 int thumbSize,
                                 bool /*isMouseOver*/,
                                 bool /*isMouseDown*/)
{
	if (isScrollbarVertical)
	{
		WusikPinkImage::drawImageVertical(g, scrollBarImages[1][0], x, y, height, resampleImages, skinTiled);
		if (thumbSize != 0)	WusikPinkImage::drawImageVertical(g, scrollBarImages[1][1], x, thumbStartPosition, thumbSize, resampleImages, skinTiled);
	}
	else
	{
		WusikPinkImage::drawImageHorizontal(g, scrollBarImages[0][0], x, y, width, resampleImages, skinTiled, offSets);
		if (thumbSize != 0)	WusikPinkImage::drawImageHorizontal(g, scrollBarImages[0][1], thumbStartPosition, y, thumbSize, resampleImages, skinTiled, offSets);
	}
	
// -------------------------------------------------------------------------------------------------------------------------------
void LookAndFeelEx::drawAlertBox(Graphics& g, AlertWindow& alert, const Rectangle<int>& textArea, TextLayout& textLayout)
{
	WusikPinkImage::drawImageSquare(g, alertBackground, 0, 0, alert.getWidth(), alert.getHeight(), resampleImages, skinTiled, offSets);
	LookAndFeel_V2::drawAlertBox(g, alert, textArea, textLayout);
}
