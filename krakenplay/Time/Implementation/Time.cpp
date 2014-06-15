
#include "../Time.h"

namespace
{
  struct TimeInit
  {
    TimeInit()
    {
      ezTime::Initialize();
    }
  } timeInit;
};

// Include inline file
#if defined(_WIN32)
  #include "Win/Time_win.h"
#elif defined(__unix__)
  #include "OSX/Time_osx.h"
#elif defined(__APPLE__)
  #include "Posix/Time_posix.h"
#else
  #error "Time functions are not implemented on current platform"
#endif
