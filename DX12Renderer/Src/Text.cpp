#include "Text.h"
#include "ConstValue.h"
#include "RendererHelper.h"
#include "UIRenderer.h"
#include "wrl.h"

namespace rdr
{
	Text::Text(const wchar_t* InCharArr, const D2D1_RECT_F& InRect, const UIRenderer& InUiRenderer)
		: BaseUI(InRect, InUiRenderer),String(InCharArr), FontSize(18), Font(DEFAULT_FONT)
	{
		SetColor(D2D1::ColorF::LightSteelBlue);
		SetFontSize(FontSize);
		CHECK_RESULT(pIDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED));
		CHECK_RESULT(pIDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
	}

	Text::Text(const std::wstring& wstr, const D2D1_RECT_F& InRect, const UIRenderer& InUiRenderer)
		: BaseUI(InRect, InUiRenderer), String(wstr), Font(DEFAULT_FONT), FontSize(18)
	{
		SetColor(D2D1::ColorF::LightSteelBlue);
		SetFontSize(FontSize);
		CHECK_RESULT(pIDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED));
		CHECK_RESULT(pIDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
	}

	Text::~Text()
	{
	}

	void Text::Draw()
	{
		uiRenderer.GetDeviceContext6()->DrawTextW(String.c_str(), String.size(), pIDWriteTextFormat.Get(), &Rect, 
			pID2D1SolidColorBrush.Get());
	}

	void Text::SetColor(const Color& color)
	{
		if (pID2D1SolidColorBrush)pID2D1SolidColorBrush.Reset();
		CHECK_RESULT(uiRenderer.GetDeviceContext6()->CreateSolidColorBrush(color, &pID2D1SolidColorBrush));
	}

	void Text::SetFontSize(uint32_t fontSize)
	{
		if (pIDWriteTextFormat) pIDWriteTextFormat.Reset();
		CHECK_RESULT(uiRenderer.GetDWriteFactory()->CreateTextFormat(Font.c_str(), nullptr, DEFAULT_WEIGHT, DEFAULT_STYLE, 
			DEFAULT_STRETCH, fontSize, L"zh-cn", &pIDWriteTextFormat));
	}
}
