
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
#elif defined(__unix__)
  #include "time_osx.h"
#elif defined(__APPLE__)
  #include "time_posix.h"
#else
  #error "Time functions are not implemented on current platform"
#endif
