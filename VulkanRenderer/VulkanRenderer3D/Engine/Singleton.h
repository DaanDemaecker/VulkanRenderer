// Singleton.h
// This class will serve as the base for any singleton

#ifndef SingletonIncluded
#define SingletonIncluded

namespace DDM3
{
	// Make singletion templated class
	template <typename T>
	class Singleton
	{
	public:
		// Get the single instance of the class
		static T& GetInstance()
		{
			// Create static instance, because the variable is static, this will only be done once
			static T instance{};
			// Return the instance
			return instance;
		}

		virtual ~Singleton() = default;
		Singleton(const Singleton& other) = delete;
		Singleton(Singleton&& other) = delete;
		Singleton& operator=(const Singleton& other) = delete;
		Singleton& operator=(Singleton&& other) = delete;

	protected:
		// Make constructor protected so a second instance can't be made from outside the class
		Singleton() = default;
	};
}

#endif // !SingletonIncluded