#pragma once

#include <Tether/Common/Defs.hpp>
#include <string>

namespace Tether
{
	class TETHER_EXPORT Library
	{
	public:
		explicit Library(std::string_view path);
		Library(Library&& other) noexcept;
		~Library();

		void* LoadFunction(std::string_view functionName) const;
		[[nodiscard]] void* GetHandle() const;
	private:
		void* m_LibraryHandle = nullptr;
	};
}
