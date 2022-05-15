#pragma once
#include <d2d1.h>
#include <functional>

namespace rdr
{
	using Color = D2D1::ColorF;
	class UIRenderer;

	class BaseUI
	{
	public:
		BaseUI(D2D1_RECT_F InRect, const UIRenderer& InUIRenderer);
		virtual ~BaseUI();

	public:
		void SetRect(const D2D1_RECT_F& InRect) { Rect = InRect; }
		void SetRect(float InTop, float InBottom, float InLeft, float InRight) { Rect.top = InTop; Rect.bottom = InBottom; Rect.left = InLeft; Rect.right = InRight; }

	public:
		virtual bool IsClicked(float x, float y) = 0;
		virtual void Draw() = 0;

	public:
		std::function<void()> OnClicked;
		std::function<void()> OnUpdate;

	protected:
		const UIRenderer& uiRenderer;
		D2D1_RECT_F Rect;
	};
}
