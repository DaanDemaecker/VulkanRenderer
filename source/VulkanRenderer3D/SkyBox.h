// SkyBox.h
// This class will hold all nesesarry object to create and render a skybox

#ifndef SkyBoxIncluded
#define SkyBoxIncluded

// File includes
#include "Structs.h"

// Standard libraryincludes
#include <memory>
#include <string>
#include <initializer_list>

namespace D3D
{
	class VulkanRenderer3D;
	class Model;

	class SkyBox
	{
	public:
		SkyBox() = delete;

		SkyBox(std::initializer_list<const std::string>&& filePaths);

		void Render();


	private:
		std::unique_ptr<Model> m_pModel{};
	};
}


#endif // !SkyBoxIncluded