#include "Toggle.h"
#include "RendererHelper.h"
#include "Text.h"
#include "UIRenderer.h"

namespace rdr
{
	//TODO:可自定义一个矩形框类，对D2D1_RECT_F加一层封装

	Toggle::Toggle(const wchar_t* InCharArr, const D2D1_RECT_F& InRect, const UIRenderer& InUiRenderer, bool InIsChecked)
		: BaseUI(InRect, InUiRenderer), ptrText(std::make_unique<Text>(InCharArr, InRect, uiRenderer)), isChecked(InIsChecked)
	{
		float deltaSize = (InRect.bottom - InRect.top) * 0.02f;
		float boxSize = (InRect.bottom - InRect.top) * 0.96f;
		float top = InRect.top + deltaSize, bottom = InRect.bottom - deltaSize, left = InRect.right - deltaSize - boxSize, right = InRect.right - deltaSize;
		CheckBoxRect = { left, top, right,bottom };
		SetColor(D2D1::ColorF::LightSteelBlue);
		OnClicked = [this]()->void
		{
			isChecked = !isChecked;
			if (OnCheckChanged) OnCheckChanged(isChecked);
		};
	}

	Toggle::~Toggle()
	{
	}

	void Toggle::Draw()
	{
		ptrText->Draw();
		if (isChecked)uiRenderer.GetDeviceContext6()->FillRectangle(&CheckBoxRect, pCheckBoxBrush.Get());
		else uiRenderer.GetDeviceContext6()->DrawRectangle(&CheckBoxRect, pCheckBoxBrush.Get());
	}

	void Toggle::SetCheckState()
	{
		isChecked = !isChecked;
		if (OnCheckChanged) OnCheckChanged(isChecked);
	}

	void Toggle::SetColor(const Color& color)
	{
		if (pCheckBoxBrush) pCheckBoxBrush.Reset();
		CHECK_RESULT(uiRenderer.GetDeviceContext6()->CreateSolidColorBrush(color, &pCheckBoxBrush));
		ptrText->SetColor(color);
	}
}
