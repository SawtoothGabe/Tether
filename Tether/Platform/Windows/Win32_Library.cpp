#include <Tether/Common/Library.hpp>

#include <Windows.h>

namespace Tether
{
	Library::Library(const std::string_view path)
	{
		m_LibraryHandle = LoadLibrary(path.data());
	}

	Library::Library(Library&& other) noexcept
	{
		m_LibraryHandle = other.m_LibraryHandle;
		other.m_LibraryHandle = nullptr;
	}

	Library::~Library()
	{
		if (!m_LibraryHandle)
			return;

		FreeLibrary(static_cast<HMODULE>(m_LibraryHandle));
	}

	void* Library::LoadFunction(const std::string_view functionName) const
	{
		return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(m_LibraryHandle),
			functionName.data()));
	}

	void* Library::GetHandle() const
	{
		return m_LibraryHandle;
	}
}
