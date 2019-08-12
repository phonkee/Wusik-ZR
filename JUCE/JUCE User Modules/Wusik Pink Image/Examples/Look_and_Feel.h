// -------------------------------------------------------------------------------------------------------------------------------
class JUCE_API LookAndFeelEx : public LookAndFeel_V2
{
public:
	~LookAndFeelEx();
	//
	void drawScrollbar(Graphics& g, ScrollBar&, int x, int y, int width, int height, bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown);
	void drawAlertBox(Graphics& g, AlertWindow& alert, const Rectangle<int>& textArea, TextLayout& textLayout);
};