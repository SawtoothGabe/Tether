#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Ref.hpp>
#include <Tether/Common/Types.hpp>

#include <Tether/Devices/Monitor.hpp>

#include <vector>
#include <optional>
#include <atomic>

namespace Tether
{
	namespace Storage
	{
		struct AppVarStorage;
	}

	class TETHER_EXPORT Application final
	{
	public:
		struct Impl;

		static constexpr const size_t KEYCODES_LENGTH = 512;
		static constexpr const size_t SCANCODES_LENGTH = Keycodes::KEY_LAST + 1;

		Application();
		~Application();

		const int16_t* GetKeycodes() const;
		const int16_t* GetScancodes() const;

		void Run();
		void PollEvents() const;

		bool IsRunning();
		void Stop();
		
		size_t GetMonitorCount();
        std::vector<Devices::Monitor> GetMonitors();


		/**
		 *
		 * @return The HINSTANCE on Windows, and a pointer to the Xorg Display on Linux.
		 */
		void* GetHandle() const;
		Impl* GetImpl() const;
		
		static Application& Get();
	private:
		Scope<Impl> m_impl;

		std::atomic_bool m_IsRunning = true;
		void CreateKeyLUTs(int16_t* keycodes, int16_t* scancodes);

		int16_t m_Keycodes[KEYCODES_LENGTH]{};
		int16_t m_Scancodes[SCANCODES_LENGTH]{};
	};
}
