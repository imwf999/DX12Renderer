#include "BaseUI.h"

namespace rdr
{
	BaseUI::BaseUI(D2D1_RECT_F InRect, const UIRenderer& InUIRenderer)
		: uiRenderer(InUIRenderer), Rect(InRect)
	{
	}

	BaseUI::~BaseUI()
	{
	}
}
