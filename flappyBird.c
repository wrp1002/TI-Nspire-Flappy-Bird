#include <os.h>
#include <SDL/SDL.h>

SDL_bool done = SDL_FALSE;

struct Player
{
	int y, x, w, h, score, highScore;
	float velY;
	int frame, frameDelayStart, frameDelay, frameDir;
	int spriteW, spriteH;
	float rot, rotVel;
	SDL_bool flapped, alive, high;
	SDL_Surface *img;
	SDL_Rect hitBox;
}player;

struct Pipe
{
	SDL_Rect pos;
	SDL_bool point;
};

void Init(SDL_Surface **screen) {
	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    if(screen == NULL) {
        printf("Couldn't initialize display: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    SDL_ShowCursor(SDL_DISABLE);
}


void InitPlayer() {
	player.y = 120;
	player.x = 20;
	player.w = 17;
	player.h = 12;
	player.hitBox.x = player.x + 5;
	player.hitBox.y = player.y + 5;
	player.hitBox.w = 10;
	player.hitBox.h = 10;
	player.velY = 0;
	player.frame = 0;
	player.frameDelayStart = 5;
	player.frameDelay = player.frameDelayStart;
	player.frameDir = 1;
	player.flapped = SDL_FALSE;
	player.alive = SDL_TRUE;
	player.spriteW = 17;
	player.spriteH = 12;
	player.rot = 0;
	player.rotVel = 0;
	player.score = 0;
	player.highScore = 0;
	player.high = SDL_FALSE;
	player.img = SDL_LoadBMP("Resources\\bird.bmp.tns");
	SDL_SetColorKey(player.img, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(player.img->format, 255, 0, 255));
};

void ResetPipes(struct Pipe pipes[]) {
	int i;
	for (i = 0; i < 4; i++) {
		pipes[i].pos.x = 325 + (100 * i);
		pipes[i].pos.y = rand() % 100 + 20;
		pipes[i].pos.w = 26;
		pipes[i].pos.h = 60;
		pipes[i].point = SDL_TRUE;
	}
}
void UpdatePipes(struct Pipe pipes[], int gameState) {
	if (gameState == 1) {
		int i;
		for (i = 0; i < 4; i++) {
			pipes[i].pos.x--;
			if (pipes[i].pos.x <= -25) {
				pipes[i].pos.x = 325 + (50);
				pipes[i].pos.y = rand() % 100 + 20;
				pipes[i].point = SDL_TRUE;
			}
		}
	}
}
void DrawPipes(struct Pipe pipes[], SDL_Surface *pipe1, SDL_Surface *pipe2, SDL_Surface *screen) {
	int i;
	for (i = 0; i < 4; i++) {
		SDL_Rect temp;
		temp.x = pipes[i].pos.x;
		temp.y = pipes[i].pos.y - 135;
		SDL_BlitSurface(pipe1, NULL, screen, &temp);
		
		temp.x = pipes[i].pos.x;
		temp.y = pipes[i].pos.y + 60;
		SDL_BlitSurface(pipe2, NULL, screen, &temp);
		
		temp.x = pipes[i].pos.x;
		temp.y = pipes[i].pos.y;
		temp.h = 60;
		//SDL_FillRect(screen, &temp, SDL_MapRGB(screen->format, 255, 0, 0));
	}
};
void CollidePipes(struct Pipe pipes[]) {
	int i;
	for (i = 0; i < 4; i++) {
		if ((player.hitBox.x + player.hitBox.w > pipes[i].pos.x && player.hitBox.x + player.hitBox.w < pipes[i].pos.x + pipes[i].pos.w) ||
			(player.hitBox.x > pipes[i].pos.x && player.hitBox.x < pipes[i].pos.x + pipes[i].pos.w)) {
				if (player.hitBox.y + player.hitBox.h > pipes[i].pos.y + pipes[i].pos.h || player.hitBox.y < pipes[i].pos.y) {
					player.alive = SDL_FALSE;
				}
			}
			if (player.hitBox.x + player.hitBox.w / 2 > pipes[i].pos.x + pipes[i].pos.w / 2 && pipes[i].point) {
				pipes[i].point = SDL_FALSE;
				player.score++;
		}
	}
}


void UpdatePlayer(int gameState) {
	if (player.alive) {
		player.frameDelay--;
		if (player.frameDelay <= 0) {
			player.frameDelay = player.frameDelayStart;
			player.frame += player.frameDir;
			if (player.frame >= 2 || player.frame <= 0)
				player.frameDir *= -1;
		}
	}
	
	if (gameState != 0 && gameState != 3) {
		player.y += player.velY;
		
		if (player.velY < 5)
			player.velY += .25;
		
		if (player.velY < 0) {
			player.rot = 20;
			player.rotVel = 0;
		}
		else {
			player.rotVel += .25;
			player.rot -= player.rotVel;
			if (player.rot < -90)
				player.rot = -90;
		}
		
		if (player.y + player.h >= 190) {
			player.y = 190 - player.h;
			player.alive = SDL_FALSE;
		}
		
		if (!player.alive) {
			player.rot = -90;
		}
	}
	
	player.hitBox.y = player.y + 3;
};
void DrawPlayer(SDL_Surface *screen) {
	SDL_Rect dest;
	dest.x = player.x;
	dest.y = player.y;
	
	SDL_Rect size;
	size.x = 0;
	size.y = player.spriteH * player.frame;
	size.w = player.spriteW;
	size.h = player.spriteH;
	
	SDL_Surface *tmp1 = SDL_CreateRGBSurface(0, player.spriteW, player.spriteH, 32, 0, 0, 0, 0);
	SDL_BlitSurface(player.img, &size, tmp1, NULL);
	//SDL_FillRect(screen, &player.hitBox, SDL_MapRGB(screen->format, 255, 0, 0));
	
	SDL_Surface *tmp2 = rotozoomSurface(tmp1, player.rot, 1.0, 0);
	SDL_BlitSurface(tmp2, NULL, screen, &dest);
	SDL_FreeSurface(tmp1);
	SDL_FreeSurface(tmp2);
}

int Controls(int gameState) {
	if (isKeyPressed(KEY_NSPIRE_ESC))
		done = SDL_TRUE;
	if (any_key_pressed()) {
		if (player.flapped == SDL_FALSE && (player.alive || gameState == 2)) {
			player.velY = -3.5;
			player.flapped = SDL_TRUE;
			if (gameState == 0)
				return 1;
			else if (gameState == 2)
				return 3;
		}
	}
	else
		player.flapped = SDL_FALSE;
	return gameState;
}

int UpdateGround(int groundPos) {
	groundPos--;
	if (groundPos == -100)
		groundPos = 0;
	return groundPos;
}

void DrawGround(int groundPos, SDL_Surface *ground, SDL_Surface *screen) {
	SDL_Rect dest;
	dest.x = groundPos;
	dest.y = 190;
	
	SDL_BlitSurface(ground, NULL, screen, &dest);
}

void LoadScore() {
	FILE* file;
	file = NULL;
    file = fopen("Resources/FlappyBird_HighScore.tns", "r+");
	
	if (file != NULL) {
		player.highScore = fgetc(file);
		fclose(file);
	}
	else 
		fclose(file);
}

void SaveScore() {
	FILE* file;
	file = fopen("Resources/FlappyBird_HighScore.tns", "w+");
    fputc(player.highScore, file);	
	fclose(file);
}

void DrawScore(SDL_Surface *numbers, SDL_Surface *screen) {
	int first = GetDigit(player.score, 1);
	int second = GetDigit(player.score, 0);
	
	SDL_Rect dest;
	dest.x = 135;
	dest.y = 50;
	SDL_Rect size;
	size.y = 0;
	size.x = 14 * first;
	size.w = 14;
	size.h = 20;
	if (first > 0)
	SDL_BlitSurface(numbers, &size, screen, &dest);
	
	
	dest.x = 149;
	dest.y = 50;
	size.y = 0;
	size.x = 14 * second;
	size.w = 14;
	size.h = 20;
	//if (second > 0)
	SDL_BlitSurface(numbers, &size, screen, &dest);
}

int GetDigit(int x, int n) {
    while (n--) {
        x /= 10;
    }
    return (x % 10);
}

void DrawMenu(int gameState, SDL_Surface *screen, SDL_Surface *get_ready, SDL_Surface *game_over, SDL_Surface *score, SDL_Surface *smallNumbers, SDL_Surface *high, SDL_Surface *medals, nSDL_Font *font) {
	if (gameState == 0) {
		SDL_Rect dest;
		dest.x = 73;
		dest.y = 80;
		SDL_BlitSurface(get_ready, NULL, screen, &dest);
		nSDL_DrawString(screen, font, 150 - nSDL_GetStringWidth(font, "Press Any Key To Start") / 2, 125, "Press Any Key To Start");
	}
	else if (gameState >= 2) {
		SDL_Rect dest;
		SDL_Rect size;
		dest.x = 66;
		dest.y = 20;
		SDL_BlitSurface(game_over, NULL, screen, &dest);
		
		dest.x = 47;
		dest.y = 70;
		SDL_BlitSurface(score, NULL, screen, &dest);
		
		int first = GetDigit(player.score, 0);
		int second = GetDigit(player.score, 1);
		
		if (player.score > 9) {
			dest.x = 223;
			dest.y = 105;
			size.y = 0;
			size.x = 12 * second;
			size.w = 12;
			size.h = 14;
			SDL_BlitSurface(smallNumbers, &size, screen, &dest);
			
			dest.x = 235;
			dest.y = 105;
			size.y = 0;
			size.x = 12 * first;
			size.w = 12;
			size.h = 14;
			SDL_BlitSurface(smallNumbers, &size, screen, &dest);
		}
		else {
			dest.x = 223;
			dest.y = 105;
			size.y = 0;
			size.x = 12 * first;
			size.w = 12;
			size.h = 14;
			SDL_BlitSurface(smallNumbers, &size, screen, &dest);
		}
		
		first = GetDigit(player.highScore, 0);
		second = GetDigit(player.highScore, 1);
		
		if (player.highScore > 9) {
			dest.x = 223;
			dest.y = 145;
			SDL_Rect size;
			size.y = 0;
			size.x = 12 * second;
			size.w = 12;
			size.h = 14;
			SDL_BlitSurface(smallNumbers, &size, screen, &dest);
			
			dest.x = 235;
			dest.y = 145;
			size.y = 0;
			size.x = 12 * first;
			size.w = 12;
			size.h = 14;
			SDL_BlitSurface(smallNumbers, &size, screen, &dest);
		}
		else {
			dest.x = 223;
			dest.y = 145;
			SDL_Rect size;
			size.y = 0;
			size.x = 12 * first;
			size.w = 12;
			size.h = 14;
			SDL_BlitSurface(smallNumbers, &size, screen, &dest);
		}

		if (player.high) {
			dest.x = 200;
			SDL_BlitSurface(high, NULL, screen, &dest);
		}
		
		int medal = -1;
		
		if (player.score >= 10)
			medal = 0;
		if (player.score >= 20)
			medal = 1;
		if (player.score >= 30)
			medal = 2;
		if (player.score >= 40)
			medal = 3;
		
		if (medal >= 0) {
			dest.x = 74;
			dest.y = 112;
			size.x = 44 * medal;
			size.y = 0 ;
			size.w = 44;
			size.h = 44;
			
			SDL_BlitSurface(medals, &size, screen, &dest);
		}
			
		nSDL_DrawString(screen, font, 85, 200, "Press Any Key To Restart");
	}
}

int main(int arc, char **argv) {
	if (!enable_relative_paths(argv))
		exit();
	SDL_Surface *screen = SDL_SetVideoMode(320, 240, is_cx ? 16 : 8, SDL_SWSURFACE);
	SDL_Surface *background = SDL_LoadBMP("Resources\\background.bmp.tns");
	SDL_Surface *ground = SDL_LoadBMP("Resources\\ground.bmp.tns");
	SDL_Surface *pipe1 = SDL_LoadBMP("Resources\\pipe1.bmp.tns");
	SDL_Surface *pipe2 = SDL_LoadBMP("Resources\\pipe2.bmp.tns");
	SDL_Surface *numbers = SDL_LoadBMP("Resources\\numbers.bmp.tns");
	SDL_Surface *smallNumbers = SDL_LoadBMP("Resources\\small_numbers.bmp.tns");
	SDL_Surface *get_ready = SDL_LoadBMP("Resources\\get_ready.bmp.tns");
	SDL_Surface *game_over = SDL_LoadBMP("Resources\\game_over.bmp.tns");
	SDL_Surface *score = SDL_LoadBMP("Resources\\score.bmp.tns");
	SDL_Surface *high = SDL_LoadBMP("Resources\\new.bmp.tns");
	SDL_Surface *medals = SDL_LoadBMP("Resources\\medals.bmp.tns");
	SDL_SetColorKey(pipe1, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(pipe1->format, 255, 0, 255));
	SDL_SetColorKey(pipe2, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(pipe2->format, 255, 0, 255));
	SDL_SetColorKey(numbers, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(numbers->format, 255, 0, 255));
	SDL_SetColorKey(get_ready, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(get_ready->format, 255, 0, 255));
	SDL_SetColorKey(game_over, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(game_over->format, 255, 0, 255));
	SDL_SetColorKey(smallNumbers, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(smallNumbers->format, 255, 0, 255));
	SDL_SetColorKey(medals, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(medals->format, 255, 0, 255));
	
	nSDL_Font *font = nSDL_LoadFont(NSDL_FONT_TINYTYPE,29, 43, 61);
	
	float frameTime = 0;
	int prevTime = 0;
	int curTime = 0;
	float deltaTime = 0;
	int flashed = 0;
	
	int gameState = 0;
	struct Pipe pipes[4];
	
	ResetPipes(pipes);
	
	int groundPos = 0;
	
	Init(&screen);
	InitPlayer();
	LoadScore();
	
	while(!done) {
		prevTime = curTime;
		curTime = SDL_GetTicks();
		deltaTime = (curTime - prevTime) / 1000.0f;
		
		frameTime += deltaTime;
		
		if (frameTime >= 1.0/60.0) {
			frameTime = 0;
			gameState = Controls(gameState);
			if (gameState == 3 && !any_key_pressed()) {
				gameState = 0;
				flashed = 0;
				InitPlayer();
				ResetPipes(pipes);
				LoadScore();
			}
			
			UpdatePlayer(gameState);
			
			if (gameState != 2 && player.alive)
				groundPos = UpdateGround(groundPos);
			if (gameState == 1 && player.alive)
				UpdatePipes(pipes, gameState);
			
			CollidePipes(pipes);
			if (!player.alive) {
				if (flashed == 0)
					flashed = 1;
				else if (flashed <= 5)
					flashed++;
			}
			
			if (gameState == 1 && !player.alive && player.y + player.h >= 190) {
				gameState = 2;
				if (player.score > player.highScore) {
					player.highScore = player.score;
					player.high = SDL_TRUE;
					SaveScore();
				}
			}
			
			
			SDL_BlitSurface(background, NULL, screen, NULL);
			DrawPipes(pipes, pipe1, pipe2, screen);
			DrawGround(groundPos, ground, screen);
			DrawPlayer(screen);
			
			if (gameState != 2 && gameState != 3)
				DrawScore(numbers, screen);
			
			DrawMenu(gameState, screen, get_ready, game_over, score, smallNumbers, high, medals, font);
			if (flashed > 0 && flashed < 5) {
				SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));
			}
			SDL_Flip(screen);
		}
    }
	SDL_FreeSurface(player.img);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(background);
	SDL_FreeSurface(ground);
	SDL_FreeSurface(pipe1);
	SDL_FreeSurface(pipe2);
	SDL_FreeSurface(numbers);
	SDL_FreeSurface(smallNumbers);
	SDL_FreeSurface(get_ready);
	SDL_FreeSurface(game_over);
	SDL_FreeSurface(score);
	SDL_FreeSurface(high);
	SDL_FreeSurface(medals);
	return 0;
}
