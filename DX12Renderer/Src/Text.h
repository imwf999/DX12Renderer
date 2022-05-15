#pragma once
#include <d2d1.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <functional>
#include "BaseUI.h"

#define DEFAULT_FONT L"Î¢Èí¿¬Ìå"
#define DEFAULT_WEIGHT DWRITE_FONT_WEIGHT_SEMI_LIGHT
#define DEFAULT_STYLE DWRITE_FONT_STYLE_NORMAL
#define DEFAULT_STRETCH DWRITE_FONT_STRETCH_NORMAL

namespace rdr
{
	using Microsoft::WRL::ComPtr;
	class UIRenderer;

	class Text : public BaseUI
	{
	public:
		Text(const wchar_t* InCharArr, const D2D1_RECT_F& InRect, const UIRenderer& uiRenderer);
		Text(const std::wstring& wstr, const D2D1_RECT_F& InRect, const UIRenderer& uiRenderer);
		~Text() override;

	public:
		void Draw() override;
		bool IsClicked(float x, float y) override { return x >= Rect.left && x <= Rect.right && y >= Rect.top && y < Rect.bottom; }

	public:
		void SetText(std::wstring& str) { String = str;}
		void SetText(std::wstring&& str) { String = std::move(str); }
		void SetColor(const Color& color);
		void SetFontSize(uint32_t fontSize);

	private:
		std::wstring String;
		uint32_t FontSize;
		std::wstring Font;
		ComPtr<IDWriteTextFormat> pIDWriteTextFormat;
		ComPtr<ID2D1SolidColorBrush> pID2D1SolidColorBrush;

	};
}
