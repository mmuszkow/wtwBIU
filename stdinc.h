#ifndef _MSC_VER
#error This code can be only compiled using Visual Studio
#endif

#pragma once

#ifdef _DEBUG
# define CRTDBG_MAP_ALLOC
# include <stdlib.h>
# include <crtdbg.h>
#endif

#define _CRT_SECURE_NO_DEPRECATE 1
#define _ATL_SECURE_NO_DEPRECATE 1
#define _CRT_NON_CONFORMING_SWPRINTFS 1

#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <stack>
using namespace std;

#include <windows.h>
#include <Commctrl.h>
#ifdef WIN32
#pragma comment (lib,"Comctl32.lib")
#else
#pragma comment (lib,"Comctl64.lib")
#endif
#include <Richedit.h>
#include "resource.h"

#include "plInterface.h"
