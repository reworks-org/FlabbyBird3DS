#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <sftd.h>
#include <fstream>

#include "common.h"

int scroll = 0;
std::string fps;
std::string scorestr;
std::string highscorestr = "";

sf2d_texture* background;
sftd_font* font;
sftd_font* console_font;

sf2d_texture* bird;
sf2d_texture* pipe;

int game_state = 0; // 0 is menu, one is game.
int y = 0;
int vy = 0;
int wx[2];
int wy[2];
int score = 0;
int highscore = 0;

u8* buffer;
u32 size;

bool draw_controls = false;

struct Rect {
	int x, y;
	int w, h;
};

Rect pipe_a;
Rect pipe_b;
Rect bird_r;

// thanks to lazyfoo for collision code from his sdl2 tutorials.
bool check_collision( Rect& a, Rect& b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    // if there is a collision
    return true;
}

void init()
{
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0, 0, 0, 255));
	sf2d_set_vblank_wait(0);
	
	sftd_init();	

	srand(time(NULL));

	font = sftd_load_font_file("res/game_over.ttf");
	console_font = sftd_load_font_file("res/f25.ttf");
	background = sfil_load_PNG_file("res/background.png", SF2D_PLACE_RAM);
	bird = sfil_load_PNG_file("res/bird.png", SF2D_PLACE_RAM);
	pipe = sfil_load_PNG_file("res/pipe.png", SF2D_PLACE_RAM);

	wx[0] = 400;
	wy[0] = 120;
	wx[1] = 600;
	wy[1] = 180;
	y = 120;

	pipe_a.w = 14;
	pipe_a.h = 240;
	pipe_b.w = 14;
	pipe_b.h = 240;	

	bird_r.x = 120;
	bird_r.w = 44;
	bird_r.h = 22;

	// says .bin but is really just a .txt. Can open and edit with notepad or w/e.
	std::ifstream temp;
	temp.open("res/highscore.bin");
	temp >> highscore;
	highscorestr = "Highscore: " + to_string(highscore);
	temp.close();
}

void end()
{
	std::ofstream temp;
	temp.open("res/highscore.bin");
	temp << to_string(highscore);
	temp.close();

	sftd_free_font(font);
	sftd_free_font(console_font);
	sf2d_free_texture(background);
	sf2d_free_texture(bird);
	sf2d_free_texture(pipe);

	sf2d_fini();
	sftd_fini();
}

void reset()
{
	score = 0;
	wx[0] = 400;
	wy[0] = 120;
	wx[1] = 600;
	wy[1] = 180;
	y = 120;
	vy = 0;
	game_state = 0; 
}

void update()
{
	fps = "FPS: " + to_string((int)sf2d_get_fps()); 

	--scroll;
	if (scroll < -SCREEN_WIDTH_TOP) {
		scroll = 0;
	}

	if (game_state == 1) {
		vy += 1;
		y += vy;

		for (int i = 0; i < 2; i++) {
			if (wx[i] < -14) {
				wy[i] = simple_random(60, 180);
				wx[i] = 400;
			}

			if (wx[i] == SCREEN_WIDTH_TOP) {
				score++;
				if (score > highscore) {
					highscore = score;
					highscorestr = "Highscore: " + to_string(highscore);
				}
				scorestr = "Score: " + to_string(score);
			}

			if ( y > SCREEN_HEIGHT_TOP || y < 0) {
				reset();
			}

			pipe_a.x = wx[i];
			pipe_a.y = wy[i] - (240 / 2 + 32) - 120;
		
			pipe_b.x = wx[i];
			pipe_b.y = wy[i] + (240 / 2 + 32) - 120;
			
			bird_r.y = y;

			if (check_collision(bird_r, pipe_a) || check_collision(bird_r, pipe_b)) {
				reset();
			}

			wx[i] -= 3;
		}
	}
}

void draw_top()
{
	sf2d_start_frame(GFX_TOP, GFX_LEFT);

	if (game_state == 0) {

		sf2d_draw_texture(background, scroll, 0);
		sf2d_draw_texture(background, scroll + SCREEN_WIDTH_TOP, 0);

		sftd_draw_text(font, 70, 3, RGBA8(255, 255, 0, 255), 128, "Flabby Bird");

	} else {
		//if (scroll > -1800) scroll = 0;
		sf2d_draw_texture(background, scroll, 0);
		sf2d_draw_texture(background, scroll + SCREEN_WIDTH_TOP, 0);

		for (int i = 0; i < 2; i++) {
			sf2d_draw_texture(pipe, wx[i], wy[i] - (240 / 2 + 32) - 120); // top
			sf2d_draw_texture(pipe, wx[i], wy[i] + (240 / 2 + 32) - 120); // bottom
		}

		sf2d_draw_texture(bird, 120, y);
	}

	sf2d_end_frame();
}

void draw_bottom()
{
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

	if (game_state == 0) {

		if (!draw_controls) {
			sftd_draw_text(font, 1, -50, RGBA8(255, 255, 255, 255), 72, fps.c_str());
			sftd_draw_text(font, 73, 70, RGBA8(255, 255, 255, 255), 50, "Press (A) to start!");
			sftd_draw_text(font, 73, 100, RGBA8(255, 255, 255, 255), 50, highscorestr.c_str());
		} else {
			sftd_draw_text(console_font, 0, 0, RGBA8(255, 255, 255, 255), 12, "X = Open / close instructions.");
			sftd_draw_text(console_font, 0, 10, RGBA8(255, 255, 255, 255), 12, "A = Start game.");
			sftd_draw_text(console_font, 0, 20, RGBA8(255, 255, 255, 255), 12, "B = Jump.");
			sftd_draw_text(console_font, 0, 30, RGBA8(255, 255, 255, 255), 12, "START = Quit game / back to menu.");
			sftd_draw_text(console_font, 0, 40, RGBA8(255, 255, 255, 255), 12, "SELECT = Reset high score.");
		}

	} else {

		sftd_draw_text(font, 1, -50, RGBA8(255, 255, 255, 255), 72, fps.c_str());
		sftd_draw_text(font, 1, 100, RGBA8(255, 255, 255, 255), 72, scorestr.c_str());

	}

	sf2d_end_frame();
}

void render()
{
	draw_top();
	draw_bottom();
	sf2d_swapbuffers();
}

int main()
{
	init();

	uint64_t last_time = get_nano_time();
	uint64_t timer = get_system_time();
	const double ns = 1000000000.0 / 30.0;
	double delta = 0.0;
	int frames = 0;
	int updates = 0;

	while (aptMainLoop())
	{
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (game_state == 0) {

			if (kDown & KEY_START) break; // break in order to return to hbmenu
			if (kDown & KEY_SELECT) { highscore = 0; }
			if (kDown & KEY_X) { if (!draw_controls) { draw_controls = true; } else { draw_controls = false; } }
			if (kDown & KEY_A) { game_state = 1; }

		} else {

			if (kDown & KEY_START) reset();
			if (kDown & KEY_X) { if (!draw_controls) { draw_controls = true; } else { draw_controls = false; } }
			if (kDown & KEY_B) { vy = -8; }

		}

		uint64_t now = get_nano_time();
		delta += (now - last_time) / ns;
		last_time = now;

		while (delta >= 1)
		{
			update();
			++updates;
			--delta;
		}
		render();
		++frames;

		if ((get_system_time() - timer) > 1000) {
			timer += 1000;
			updates = 0;
			frames = 0;
		}

	}

	end();

	return EXIT_SUCCESS;
}