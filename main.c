#include "SDL.h"
#include "SDL_ttf.h"
#include "stdio.h"
#include "SDL_image.h"

#define true 1
#define false 0

#ifdef _WIN32
#include <Windows.h>
#elif __APPLE__
#include <stdlib>
#endif

void HANDLE_ERROR() {
	const char* error = SDL_GetError();
	if (*error) {
#ifdef _WIN32
		MessageBoxA(NULL, error, "SDL ERROR", MB_OK | MB_ICONERROR);
#elif _APPLE__
		char command[256];
		snprintf(command, sizeof(command), "osascript -e 'display dialog \"%s\" buttons {\"OK\"}'", error);
		system(command);
#endif
	}
}


typedef enum {
	ODIN_NORMAL_TEXT,
	ODIN_CODEBLOCK_TEXT,
	ODIN_PLOTBLOCK,
	ODIN_EQUATION_TEXT
}ODIN_CONTENT_TYPE;

typedef struct {
	ODIN_CONTENT_TYPE type;
	int columns;
	int rows;
	char* content;
}Content;

typedef struct {
	char* path;
	char* name;
	char* content;
	int columns;
	int rows;
	char* mirmir;
}Rune;

typedef struct {
	int type; // we will use int since theres only 2 states either it is some overview or an open rune
	char* name;
	Rune* mirmir;
	Rune document;
}Odin_Tab;

typedef struct {
	int screenHeight;
	int screenWidth;
	SDL_Window* window;
	SDL_Renderer* renderer;
	Rune currentRune;
	int hasDocumentOpen;
	Rune* runesOpened;
	int isRunning;
	Odin_Tab* tabs;
	int hasFocus;
	TTF_Font* font;
	Uint32 blinkTimer;
	Uint32 cursorBlinkTimer;
	int cursorVisible;
}Runtime;

typedef struct {
	SDL_Texture* icon;
	void (*onClick)(Runtime* runtime);
	int x;
	int y;
	int w;
	int h;
	int isVisible;
}Button;




//because Visual Studio is not my fortei i'm gonna do this onorthodox way
// this will be the base UI, this consists of the sidebar 
// BASE_UI
////////////////////////////////////

SDL_Texture* createTexture(Runtime* runtime, char* imagePath) {
	SDL_Surface* iconSurface = IMG_Load(imagePath);
	if (!iconSurface) {
		printf("there was an error while trying to create the image surface: %s", SDL_GetError());
	}
	SDL_Texture* iconTexture = SDL_CreateTextureFromSurface(runtime->renderer, iconSurface);
	if (!iconSurface) {
		printf("there was an error while trying to create the image texture: %s", SDL_GetError());
	}
	SDL_FreeSurface(iconSurface);
	return iconTexture;
}

int mouseX, mouseY;
int detailBarWidth = 0;
Button buttons[3];


void search(Runtime* runtime) {
	printf("this is search\n");
}

void explorer(Runtime* runtime) {
	printf("this is explorer\n");
}

void timeline(Runtime* runtime) {
	printf("this is timeline\n");
}

void setUp(Runtime* runtime) {
	char* strings[3] = { "C:\\Users\\X\\Desktop\\eksamen prog\\Odin\\ODIN\\Res\\search_icon.png","C:\\Users\\X\\Desktop\\eksamen prog\\Odin\\ODIN\\Res\\files_icon.png","C:\\Users\\X\\Desktop\\eksamen prog\\Odin\\ODIN\\Res\\branch_icon.png" };
	for (int i = 0;i < 3;i++) {
		buttons[i] = (Button){ createTexture(runtime,strings[i]),i == 0 ? &search : i == 1 ? &explorer : &timeline,(60 - 40) / 2,10 + 50 * i,40,40,true };
	}
	runtime->font = TTF_OpenFont("C:\\WINDOWS\\FONTS\\TIMES.TTF", 24); // Replace with valid font path
	if (!runtime->font) {
		printf("Failed to load font: %s\n", TTF_GetError());
	}

	runtime->hasFocus = 0;
	runtime->cursorBlinkTimer = 0;
	runtime->cursorVisible = 0;
}


void drawSidebar(Runtime* runtime) {
	SDL_Rect sidePanel = { 0,0,60,runtime->screenHeight };
	SDL_SetRenderDrawColor(runtime->renderer, 58, 85, 180, 255);
	SDL_RenderFillRect(runtime->renderer, &sidePanel);

	// Here we are handling the buttons  
	for (int i = 0;i < 3;i++) {
		Button b = buttons[i];
		SDL_Rect rect = { b.x,b.y,b.w,b.h };
		SDL_RenderCopy(runtime->renderer, b.icon, NULL, &rect);
	}

	//
	SDL_Rect extendbar = { 60 + detailBarWidth,0,5,runtime->screenHeight };
	if ((mouseX >= 60 + detailBarWidth && mouseX <= 60 + detailBarWidth + 5 + 10) && mouseY >= 0 && mouseY <= runtime->screenHeight) {
		SDL_SetRenderDrawColor(runtime->renderer, 255, 85, 255, 255);
	}
	SDL_RenderFillRect(runtime->renderer, &extendbar);

	//
}


//Dashboard
///////////////////////////////////

void drawDashBoard(Runtime* runtime) {
	int line = (int)((runtime->screenWidth - 60 + 5 + detailBarWidth) * 0.95);
	SDL_Rect lineRect = { ((runtime->screenWidth - line - 5) / 2) + 60 + 5 + detailBarWidth,250,line,2 };
	SDL_SetRenderDrawColor(runtime->renderer, 255, 44, 44, 255);
	SDL_RenderFillRect(runtime->renderer, &lineRect);


	for (int i = 0;i < 11;i++) {
		int y = 60;
		int width = 150;
		int height = 170;
		int x = ((runtime->screenWidth - line - 5) / 2) + 60 + 5 + detailBarWidth + 20 + 155 * i;
		SDL_Rect rect = { x,y,width,height };
		if ((mouseX >= x && mouseX <= x + width) && (mouseY >= y && mouseY <= y + height)) {
			SDL_SetRenderDrawColor(runtime->renderer, 50, 50, 50, 255);
		}
		else SDL_SetRenderDrawColor(runtime->renderer, 30, 30, 30, 255);
		SDL_RenderFillRect(runtime->renderer, &rect);
	}
}


void drawDocument(Runtime* runtime) {
	SDL_Renderer* renderer = runtime->renderer;
	// Document area dimensions
	int docX = 60 + detailBarWidth + 20;
	int docY = 20;
	int docWidth = runtime->screenWidth - (60 + detailBarWidth + 40);
	int docHeight = runtime->screenHeight - 40;

	// Draw white background
	SDL_Rect documentRect = { docX, docY, docWidth, docHeight };
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &documentRect);

}






///////////////////////////////////





//The render function

///////////////////////////////////
void render(Runtime* runtime) {
	SDL_SetRenderDrawColor(runtime->renderer, 30, 30, 30, 255);
	SDL_RenderClear(runtime->renderer);
	drawSidebar(runtime);
	if (runtime->hasDocumentOpen) {
		drawDocument(runtime);
	}else drawDashBoard(runtime);
	SDL_RenderPresent(runtime->renderer);
}

///////////////////////////////////


//handling events 

void handleEvent(Runtime* runtime, SDL_Event event) {
	if (event.type == SDL_WINDOWEVENT) {
		if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
			SDL_GetWindowSize(runtime->window, &runtime->screenWidth, &runtime->screenHeight);
			printf("width is %d and height is %d", runtime->screenWidth, runtime->screenHeight);
		}
	}
	if (event.type == SDL_MOUSEMOTION) {
		mouseX = event.button.x;
		mouseY = event.button.y;
	}

	if (event.type == SDL_MOUSEBUTTONDOWN) {
		for (int i = 0;i < (sizeof(buttons) / sizeof(Button));i++) {
			Button b = buttons[i];
			if (b.isVisible && (event.button.x >= b.x && event.button.x <= b.x + b.w) && event.button.y >= b.y && event.button.y <= b.y + b.h) {
				b.onClick(runtime);
			}

		}
	}
	SDL_Renderer* renderer = runtime->renderer;
}




int main(int argv, char** argc) {
	//start up the SDL library so that we get access to all the threads and functions 
	//check if there is an error and fetch it from the queue and print to console 
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0 || TTF_Init() < 0 || IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) < 0) {
		printf("%s", SDL_GetError());
		return -1;
	}



	//init runtime in heap 
	Runtime runtime = { 0 };
	//initalize our window 
	runtime.window = SDL_CreateWindow("ODIN", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
	//check if there was an error initializing 
	if (!runtime.window) {
		printf("there was an error while trying to create the window: %s", SDL_GetError());
		return -1;
	}
	//initalize our renderer 
	runtime.renderer = SDL_CreateRenderer(runtime.window, -1, SDL_RENDERER_ACCELERATED);
	runtime.isRunning = true;
	runtime.hasDocumentOpen = true;
	setUp(&runtime);



	SDL_Event event;
	//this is our running loop
	while (runtime.isRunning) {
		//get the latest event from the event thread by passing a reference 
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				runtime.isRunning = false;
			}
			handleEvent(&runtime, event);
		}
		render(&runtime);
		SDL_Delay(10);
	}

	SDL_DestroyRenderer(runtime.renderer);
	SDL_DestroyWindow(runtime.window);


	return 0;
}
