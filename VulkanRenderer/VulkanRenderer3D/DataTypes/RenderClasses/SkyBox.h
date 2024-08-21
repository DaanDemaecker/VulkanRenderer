// SkyBox.h
// This class will hold all nesesarry object to create and render a skybox

#ifndef SkyBoxIncluded
#define SkyBoxIncluded

// File includes

// Standard libraryincludes
#include <memory>
#include <string>
#include <initializer_list>

namespace D3D
{
	// Class forward declarations
	class Model;

	class SkyBox
	{
	public:
		// Delete the standard destructor
		SkyBox() = delete;
		// Constructor
		// Parameters:
		//     filePaths: a list of the names to the textures for the faces in the order:
		//                Right, left, up, down, front, back...
		SkyBox(std::initializer_list<const std::string>&& filePaths);
		
		// Destructor
		~SkyBox();

		// Delete copy and move functions
		SkyBox(SkyBox& other) = delete;
		SkyBox(SkyBox&& other) = delete;
		SkyBox& operator=(SkyBox& other) = delete;
		SkyBox& operator=(SkyBox&& other) = delete;

		// Render the skybox
		void Render();


	private:
		// A unique pointer for the model of the skybox
		std::unique_ptr<Model> m_pModel{};
	};
}


#endif // !SkyBoxIncluded