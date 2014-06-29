
#include "../time.h"

namespace
{
	struct TimeInit
	{
		TimeInit()
		{
			Krakenplay::Time::Initialize();
		}
	} timeInit;
};

// Include inline file
#if defined(_WIN32)
  #include "time_win.h"
#elif defined(__unix)
  #include "time_posix.h"
#elif defined(__APPLE__)
  #include "time_osx.h"
#else
  #error "Time functions are not implemented on current platform"
#endif
