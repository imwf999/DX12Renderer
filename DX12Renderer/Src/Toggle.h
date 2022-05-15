#pragma once
#include <d2d1.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <functional>
#include "BaseUI.h"

namespace rdr
{
	using Microsoft::WRL::ComPtr;
	class Text;
	class UIRenderer;

	class Toggle : public BaseUI
	{
	public:
		Toggle(const wchar_t* InCharArr, const D2D1_RECT_F& InRect, const UIRenderer& InUiRenderer, bool InIsChecked = false);
		~Toggle() override;

	public:
		void Draw() override;
		bool IsClicked(float x, float y) override { return x >= CheckBoxRect.left && x <= CheckBoxRect.right && y >= CheckBoxRect.top && y < CheckBoxRect.bottom; }

	public:
		bool GetIsChecked() const { return  isChecked; }
		void SetCheckState();
		void SetColor(const Color& color);
		void SetCheckChangeEvent(const std::function<void(bool)>& changeEvent) { OnCheckChanged = changeEvent; }

	private:
		std::function<void(bool)> OnCheckChanged;

	private:
		D2D1_RECT_F CheckBoxRect;
		std::unique_ptr<Text> ptrText;
		ComPtr<ID2D1SolidColorBrush> pCheckBoxBrush;
		bool isChecked;
	};
}
