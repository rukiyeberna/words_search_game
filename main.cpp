#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <time.h>
#include <string>


const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 40; // Size of each cell
char  GAME_BOARD[ WINDOW_WIDTH/CELL_SIZE][WINDOW_HEIGHT/CELL_SIZE];
char* hiddenWordsArray[5] = {"code", "int", "mobile", "java", "programs"};
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;
//Globally used font
TTF_Font *gFont = NULL;

class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );

		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}

	//Return success
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}
//Rendered texture
LTexture gTextTexture;
bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Open the font
	gFont = TTF_OpenFont( "lazy.ttf", 24 );
	if( gFont == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		//Render text
		SDL_Color textColor = { 0, 0, 0 };
		if( !gTextTexture.loadFromRenderedText( "Bu harfleri oyunda kullanabilirsin", textColor ) )
		{
			printf( "Failed to render text texture!\n" );
			success = false;
		}
	}

	return success;
}
bool letterControl(char letter, int pos){
      for (int i = 0; i < 5; i++)
         if(pos < strlen(hiddenWordsArray[i]))
            if ( hiddenWordsArray[i][pos] == letter)
                return true;
    return false;

}
bool wordControl(char letter, int pos){
      for (int i = 0; i < 5; i++)
        if(pos ==  strlen(hiddenWordsArray[i]))
            if ( hiddenWordsArray[i][pos-1] == letter)
                   return true;
    return false;

}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    gWindow = SDL_CreateWindow("WordGame---Find the 5 hidden words (programs, mobile, int, java and code)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Surface* gSurface = SDL_GetWindowSurface(gWindow);

    //Create vsynced renderer for window
    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    //Initialize renderer color
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
				}
    if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
    // Clear the surface to white
    SDL_FillRect(gSurface, NULL, SDL_MapRGB(gSurface->format, 255, 255, 255));


    // vertical lines
    for (int x = CELL_SIZE; x < WINDOW_WIDTH; x += CELL_SIZE) {
        SDL_Rect lineRect = { x, 0, 1, WINDOW_HEIGHT };
        SDL_FillRect(gSurface, &lineRect, SDL_MapRGB(gSurface->format, 0, 0, 0));
    }

    // horizontal lines
    for (int y = CELL_SIZE; y < WINDOW_HEIGHT; y += CELL_SIZE) {
        SDL_Rect lineRect = { 0, y, WINDOW_WIDTH, 1 };
        SDL_FillRect(gSurface, &lineRect, SDL_MapRGB(gSurface->format, 0, 0, 0));
    }

    char  GAME_BOARD[WINDOW_WIDTH/CELL_SIZE][WINDOW_HEIGHT/CELL_SIZE];
    int x,y;
    srand(time(NULL));
    for (x=0;x<WINDOW_WIDTH/CELL_SIZE;x++){
        for (y=0;y<WINDOW_HEIGHT/CELL_SIZE;y++){
            GAME_BOARD[x][y] = 'a'+ rand()%26;
        }
    }
    int index;

    for (index = 0; index < 5; index++) {
        if (index % 2 == 0) { // horizontal
            int row = rand() % 10;
            int col = rand() % 3;

            for (int i = 0; i < strlen(hiddenWordsArray[index]); i++, col++) {
                GAME_BOARD[row][col] = hiddenWordsArray[index][i];
            }
        } else {
            int row = rand() % 3;
            int col = rand() % 10;

            for (int i = 0; i < strlen(hiddenWordsArray[index]); i++, row++) {
                GAME_BOARD[row][col] = hiddenWordsArray[index][i];
            }
        }
    }

bool isRunning = true;
bool isMouseButtonDown = false;
int selectedCellX = -1;
int selectedCellY = -1;
int previousSelectedCellX = -1;
int previousSelectedCellY = -1;
int wordSize = 0;

while (isRunning) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                isMouseButtonDown = true;
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                selectedCellX = mouseX / CELL_SIZE;
                selectedCellY = mouseY / CELL_SIZE;
            }
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                isMouseButtonDown = false;
                selectedCellX = -1;
                selectedCellY = -1;
            }
        }
    }


    for (int x=0;x<WINDOW_WIDTH/CELL_SIZE;x++){
        for (int y=0;y<WINDOW_HEIGHT/CELL_SIZE;y++){
                // Determine the cell's position and dimensions
            SDL_Rect cellRect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };

            // Render the letter in the cell
            SDL_Rect letterRect = { x * CELL_SIZE + 5, y * CELL_SIZE + 5, CELL_SIZE - 10, CELL_SIZE - 10 };
            SDL_Color textColor = { 0, 0, 0 }; // Black color for the letter
            char letterString[2] = { GAME_BOARD[x][y], '\0' }; // Convert the letter to a string
            char letter = GAME_BOARD[x][y];
            SDL_Surface* letterSurface = TTF_RenderText_Solid(gFont, letterString, textColor);
              // Highlight the selected cell
            if (isMouseButtonDown && x == selectedCellX && y == selectedCellY ){
                if (letterControl(letter,wordSize)){ // the next cell should be either to the right or down
                    if (wordSize == 0 || (wordSize > 0 && ((previousSelectedCellX==selectedCellX && selectedCellY-previousSelectedCellY == 1) || selectedCellX-previousSelectedCellX == 1 && previousSelectedCellY==selectedCellY ))){
                        SDL_FillRect(gSurface, &cellRect, SDL_MapRGB(gSurface->format, 900, 200, 200));// color for click
                        wordSize++;
                        previousSelectedCellX = selectedCellX;
                        previousSelectedCellY = selectedCellY;
                        if (wordControl(letter,wordSize)){
                            wordSize = 0;
                            previousSelectedCellX = -1;
                            previousSelectedCellY = -1;
                        }
                    }
                }
            }
            SDL_BlitSurface(letterSurface, NULL, gSurface, &letterRect);
            SDL_FreeSurface(letterSurface);
        }
    }

    SDL_UpdateWindowSurface(gWindow);
}


    SDL_DestroyWindow(gWindow);
    SDL_Quit();

    return 0;
}
