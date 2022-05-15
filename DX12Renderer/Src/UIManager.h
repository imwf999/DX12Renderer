#pragma once
#include <memory>
#include <vector>

namespace rdr
{
	class BaseUI;
	class UIRenderer;

	class UIManager
	{
	public:
		UIManager();
		~UIManager();

	public:
		void Initialize(const UIRenderer& uiRenderer);
		void Update();
		void CheckClick(float x, float y);

	public:
		const std::vector<std::shared_ptr<BaseUI>>& GetUIData() const { return UIData; }

	private:
		std::vector<std::shared_ptr<BaseUI>> UIData;
	};
}
