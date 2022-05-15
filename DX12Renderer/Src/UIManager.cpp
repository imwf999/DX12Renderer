#include "UIManager.h"
#include "Audio.h"
#include "Camera.h"
#include "ConstValue.h"
#include "Game.h"
#include "GameTimer.h"
#include "Input.h"
#include "MaterialPool.h"
#include "Renderer.h"
#include "RendererHelper.h"
#include "Text.h"
#include "Toggle.h"

namespace rdr
{
	UIManager::UIManager()
	{
	}

	UIManager::~UIManager()
	{
	}

	void UIManager::Initialize(const UIRenderer& uiRenderer)
	{
		D2D1_RECT_F tRect0 = { 10.0f, 10.0f, 290.0f, 28.0f };
		D2D1_RECT_F tRect1 = { 10.0f, 38.0f, 430.0f , 56.0f };
		D2D1_RECT_F tRect3 = { 10.0f, 66.0f, 110.0f , 84.0f };
		D2D1_RECT_F tRect4 = { 10.0f, 94.0f, 110.0f, 112.0f };

		std::shared_ptr<Text> text1 = std::make_shared<Text>(L"帧率: 0   每帧用时: 0毫秒", tRect0, uiRenderer);
		std::shared_ptr<Text> text2 = std::make_shared<Text>(L"摄像机位置: (0, 0, 0)", tRect1, uiRenderer);
		std::shared_ptr<Toggle> toggle1 = std::make_shared<Toggle>(L"播放音乐", tRect3, uiRenderer);
		std::shared_ptr<Toggle> toggle2 = std::make_shared<Toggle>(L"全局光照", tRect4, uiRenderer);

		text1->OnUpdate = [text1]()->void
		{
			text1->SetText(Game::Instance()->GetGameTimer()->GetFrameTimeText());
		};

		text2->OnUpdate = [text2]()->void
		{
			std::wstring tStr = L"摄像机位置:" + Float3ToWstring(Game::Instance()->GetCamera()->GetCameraPosition());
			text2->SetText(std::move(tStr));
		};

		toggle1->SetCheckChangeEvent([](bool isChecked)->void
			{
				if (isChecked == true)
					Game::Instance()->GetAudio()->Play(0);
				else
					Game::Instance()->GetAudio()->Stop();
			});

		toggle2->SetCheckChangeEvent([](bool isChecked)->void
			{
				Game::Instance()->GetRenderer()->GetMaterialPool()->SetOpenIndirectLight(isChecked);
			});

		UIData.push_back(text1);
		UIData.push_back(text2);
		UIData.push_back(toggle1);
		UIData.push_back(toggle2);
	}

	void UIManager::Update()
	{
		for(const auto& item : UIData)
		{
			if (item->OnUpdate)
				item->OnUpdate();
		}
	}

	void UIManager::CheckClick(float x, float y)
	{
		for(const auto& item : UIData)
		{
			if(item->IsClicked(x, y) == true && item->OnClicked)
			{
				item->OnClicked();
			}
		}
	}
}
