#pragma once

#ifndef _WIN32
#include "sys/linux/RQSignal.h"		
#else
#include "sys/win/RQSignal.h"
#endif // !_WIN32