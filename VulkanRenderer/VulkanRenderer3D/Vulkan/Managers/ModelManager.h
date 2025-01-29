// ModelManager.h
// This class will hold, manage and render all the models created by the user

#ifndef ModelManagerIncluded
#define ModelManagerIncluded

// File includes
#include "DataTypes/RenderClasses/Model.h"

// Standard library includes
#include <vector>
#include <memory>

namespace DDM3
{
	class ModelManager final
	{
	public:
		// Default constructor
		ModelManager();

		// Destructor
		~ModelManager();
		
		void Update();

		void AddModel(std::unique_ptr<Model> pModel);

		std::vector<std::unique_ptr<Model>>& GetModels();
	private:
		std::vector<std::unique_ptr<Model>> m_pModels{};

	};

}
#endif // !ModelManagerIncluded
