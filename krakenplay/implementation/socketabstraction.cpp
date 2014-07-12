#include "../socketabstraction.h"
#ifndef _WIN32
	#include <cstring>
#endif

namespace Krakenplay
{
	const std::string GetLastSocketErrorDescription()
	{
#ifdef _WIN32
		int errorCode = WSAGetLastError();
		/// \TODO User FormatMessage to obtain string.
		return std::to_string(errorCode);
#else
		return strerror(errno);
#endif
	}
}
