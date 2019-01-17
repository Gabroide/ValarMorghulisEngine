#ifndef __Globals_h__
#define __Globals_h__

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

#define MAX(x,y) ((x>y)?x:y)
#define MIN(x,y) ((x<y)?x:y)

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1300
#define SCREEN_HEIGHT 850
#define FULLSCREEN false
#define RESIZEABLE true
#define BORDERLESS false
#define FULLSCREEN_DESKTOP false
#define VSYNC true
#define DEFAULT_GO_NAME "GameObject"
#define DEFAULT_CAMERA_NAME "Camera"
#define TITLE "Valar Morghulis"
#define AUTHOR "Gabriel Cambronero"
#define DESCRIPTION "C++/C engine for game development"
#define REPOSITORY "https://github.com/Gabroide/ValarMorghulis"
#define LICENSE "https://github.com/Gabroide/ValarMorghulis/blob/master/LICENSE"

#endif // __Globals_h__