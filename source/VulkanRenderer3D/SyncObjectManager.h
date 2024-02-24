// SyncObjectManager.h
// This class wil hold and manage the vulkan sync objects

#ifndef SyncObjectManagerIncluded
#define SyncObjectManagerIncluded

namespace D3D
{
	class SyncObjectManager
	{
	public:
		SyncObjectManager() = default;
		~SyncObjectManager() = default;

		SyncObjectManager(SyncObjectManager& other) = delete;
		SyncObjectManager(SyncObjectManager&& other) = delete;

		SyncObjectManager& operator=(SyncObjectManager& other) = delete;
		SyncObjectManager& operator=(SyncObjectManager&& other) = delete;

	private:

	};
}
#endif // !SyncObjectManagerIncluded