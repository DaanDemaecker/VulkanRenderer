// Singleton.h
// This class will serve as the base for any singleton

#ifndef _DDM_SINGLETON_
#define _DDM_SINGLETON_

namespace DDM
{
	// Make singletion templated class
	template <typename T>
	class Singleton
	{
	public:
		/// <summary>
		/// Get a reference to the instance of the singleton
		/// </summary>
		/// <returns></returns>
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

#endif // !_DDM_SINGLETON_