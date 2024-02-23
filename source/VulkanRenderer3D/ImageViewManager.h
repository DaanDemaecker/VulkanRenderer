// ImageViewManager.h
// This class will hold and manage the color- and depthImaveViews

namespace D3D
{
	class ImageViewManager
	{
	public:
		ImageViewManager() = default;
		~ImageViewManager() = default;

		ImageViewManager(ImageViewManager& other) = delete;
		ImageViewManager(ImageViewManager&& other) = delete;

		ImageViewManager& operator=(ImageViewManager& other) = delete;
		ImageViewManager& operator=(ImageViewManager&& other) = delete;

	private:
	};
}