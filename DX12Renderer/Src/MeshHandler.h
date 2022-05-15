#pragma once
#include <string>
#include <vector>

namespace rdr
{
	class MeshHandler
	{
	public:
		MeshHandler();
		~MeshHandler();

	public:
		//将导入的.FBX .OBJ文件转化为.wasset文件，加快模型读取速度
		static void ConvertMesh(const std::string& filePath, const std::string& newFilePath);
	};
}
