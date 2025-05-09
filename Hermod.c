#ifdef _WIN32
#include <Windows.h>
#elif __APPLE__
#include <stdlib.h>
#endif

void Handle_Error(char* error){
    #ifdef _WIN32
		MessageBoxA(NULL, error, "error", MB_OK | MB_ICONERROR);
#elif _APPLE__
		char command[256];
		snprintf(command, sizeof(command), "osascript -e 'display dialog \"%s\" buttons {\"OK\"}'", error);
		system(command);
#endif
}
