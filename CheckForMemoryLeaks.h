// Include this file in .cpp files (AFTER all includes) where you want the CRT to check for memory 
// leaks and put the line,
//
// _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );  
//
// at the start of main() - uncomment; run in Debug and uncomment the code below

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
