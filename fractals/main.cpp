#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <vector>
#include <string>
#include <fstream>
#include <map>
using namespace std;

typedef long double ldbl;
struct coord
{
	ldbl x, y;
};

SDL_Window *window;
SDL_Renderer *render;
SDL_Texture *field, *panel, *zoomer;
SDL_Event ev;
bool go = true;
int selected = -1;
int sizee = 4;
int frcnt = 0;

int width = 750,
height = 550;

vector < string > frcts;

void Read();
void Init();
SDL_Texture* LoadTex(char *path);
void DrawBackPlane();
void Proc();
void Draw();
void PickFractal(int x, int y);
void DrawFractal();
void DrawInterface();
void Zoom(int x, int y);

int main(int, char**)
{
	SDL_SetMainReady();
	SDL_Init(SDL_INIT_VIDEO);
	Init();

	while (true)
	{
		while (!SDL_PollEvent(&ev));
		if (ev.type == SDL_QUIT)
			break;
		else
			Proc();

		if (go)
			Draw();
	}

	SDL_Quit();
	return 0;
}

void Init()
{
	window = SDL_CreateWindow("Fractals", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(render, 240, 240, 240, 255);
	char f[] = "field.bmp";
	char p[] = "panel.bmp";
	char z[] = "zoomer.bmp";
	field = LoadTex(f);
	panel = LoadTex(p);
	zoomer = LoadTex(z);
	Read();
}

SDL_Texture* LoadTex(char *path)
{
	SDL_Surface *surf = SDL_LoadBMP(path);
	return SDL_CreateTextureFromSurface(render, surf);
}

void Proc()
{
	if (ev.type == SDL_WINDOWEVENT)
	{
		if (ev.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			width = ev.window.data1;
			height = ev.window.data2;
			go = true;
		}
	}
	else if (ev.type == SDL_MOUSEBUTTONDOWN)
	{
		if (ev.button.x < 200)
			PickFractal(ev.button.x, ev.button.y);
		else
			Zoom(ev.button.x, ev.button.y);
	}
}

void DrawBackPlane()
{
	SDL_Rect r1 = { 0, 0, 500, 500 }, r2 = { 0, 0, width, height };
	SDL_RenderCopy(render, field, &r1, &r2);
}

void Draw()
{
	go = false;
	DrawBackPlane();
	DrawInterface();
	if (selected > -1 && selected < frcnt && width > 320 && height > 120)
		DrawFractal();
	SDL_RenderPresent(render);
}

void PickFractal(int x, int y)
{
	int pick = y / 50 * 4 + x / 50;
	if (pick != selected)
	{
		selected = pick;
		sizee = 4;
		go = true;
	}
}

void Zoom(int x, int y)
{
	if (y > 10 && y < 50)
	{
		if (x > 210 && x < 250)
		{
			sizee++;
			go = true;
		}
		else if (x > 250 && x < 290 && sizee > 0)
		{
			sizee--;
			go = true;
		}
	}
}

void Read()
{
	string s;
	ifstream fin("fractals.txt");
	fin >> frcnt;
	for (int i = 0; i < frcnt; ++i)
	{
		while (s.empty())
			getline(fin, s);
		frcts.push_back(s);
		s.clear();
	}
	fin.close();
}

void DrawInterface()
{
	SDL_Rect r1 = { 0, 0, 200, height }, r2 = { 0, 0, 200, height }, r3 = { 0, 0, 80, 40 }, r4 = { 210, 10, 80, 40 };
	SDL_RenderCopy(render, panel, &r1, &r2);
	SDL_RenderCopy(render, zoomer, &r3, &r4);
}

void DrawFractal()
{
	string fractal; int it = 0; char cur;
	map < char, string > dict;
	for (; it < frcts[selected].size(); ++it)
	{
		if (frcts[selected][it] == ';')
			break;
		if ((frcts[selected][it] >= 'A' && frcts[selected][it] <= 'Z') || (frcts[selected][it] >= 'a' && frcts[selected][it] <= 'z') || (frcts[selected][it] >= '0' && frcts[selected][it] <= '9') || frcts[selected][it] == '[' || frcts[selected][it] == ']' || frcts[selected][it] == '.' || frcts[selected][it] == '-' || frcts[selected][it] == '\'')
			fractal.push_back(frcts[selected][it]);
	}
	for (; it < frcts[selected].size(); ++it)
	{
		if ((frcts[selected][it] >= 'A' && frcts[selected][it] <= 'Z') || (frcts[selected][it] >= 'a' && frcts[selected][it] <= 'z'))
		{
			cur = frcts[selected][it]; it++;
			for (; it < frcts[selected].size(); ++it)
			{
				if (frcts[selected][it] == ';')
					break;
				if ((frcts[selected][it] >= 'A' && frcts[selected][it] <= 'Z') || (frcts[selected][it] >= 'a' && frcts[selected][it] <= 'z') || (frcts[selected][it] >= '0' && frcts[selected][it] <= '9') || frcts[selected][it] == '[' || frcts[selected][it] == ']' || frcts[selected][it] == '.' || frcts[selected][it] == '-' || frcts[selected][it] == '\'')
					dict[cur].push_back(frcts[selected][it]);
			}
		}
	}
	for (int i = 0; i < sizee; ++i)
	{
		string s;
		for (int j = 0; j < fractal.size(); ++j)
		{
			if ((fractal[j] >= 'A' && fractal[j] <= 'Z') || (fractal[j] >= 'a' && fractal[j] <= 'z'))
				s += dict[fractal[j]];
			else
				s.push_back(fractal[j]);
		}
		fractal = s;
	}
	vector < coord > vertex;
	vector < coord > memvert;
	vector < ldbl > memangle;
	ldbl minx = 0, maxx = 0, miny = 0, maxy = 0, angle = 0;
	coord t1 = { 0, 0 }, t2;
	for (int i = 0; i < fractal.size(); ++i)
	{
		if (fractal[i] >= 'A' && fractal[i] <= 'L')
		{
			t2.x = cos(angle) + t1.x;
			t2.y = sin(angle) + t1.y;
			vertex.push_back(t1);
			vertex.push_back(t2);
			t1 = t2;
		}
		else if (fractal[i] >= 'a' && fractal[i] <= 'z')
		{
			t2.x = cos(angle) + t1.x;
			t2.y = sin(angle) + t1.y;
			t1 = t2;
		}
		else if (fractal[i] == '\'')
		{
			i++;
			bool mns = fractal[i] == '-'; if (mns) i++;
			ldbl q = 0; ldbl w = 0.1;
			while (fractal[i] >= '0' && fractal[i] <= '9')
				q = q * 10 + int(fractal[i++] - '0');
			if (fractal[i] == '.')
			{
				i++;
				while (fractal[i] >= '0' && fractal[i] <= '9')
				{
					if (w > 0.000000001)
					{
						q += w * ldbl(fractal[i] - '0');
						w *= 0.1;
					}
					i++;
				}
			}
			i--;
			if (mns)
				q *= -1;
			q = q * M_PI / 180.0;
			angle += q;
		}
		else if (fractal[i] == '[')
		{
			memvert.push_back(t1);
			memangle.push_back(angle);
		}
		else if (fractal[i] == ']')
		{
			t1 = memvert.back();
			angle = memangle.back();
			memvert.pop_back();
			memangle.pop_back();
		}
	}
	for (int i = 0; i < vertex.size(); ++i)
	{
		if (vertex[i].x < minx) minx = vertex[i].x;
		if (vertex[i].x > maxx) maxx = vertex[i].x;
		vertex[i].y *= -1;
		if (vertex[i].y < miny) miny = vertex[i].y;
		if (vertex[i].y > maxy) maxy = vertex[i].y;
	}
	/*ldbl wd = maxx - minx; if (maxy - miny > wd) wd = maxy - miny;
	int xs = 260, ys = 60, xd = width - 320, yd = height - 120, dd = xd; if (yd < dd) dd = yd;
	ldbl zd = ldbl(dd) / wd;
	for (int i = 0; i < vertex.size(); i += 2)
	{
		int x1 = int((vertex[i].x - minx) * zd) + xs;
		int x2 = int((vertex[i + 1].x - minx) * zd) + xs;
		int y1 = int((vertex[i].y - miny) * zd) + ys;
		int y2 = int((vertex[i + 1].y - miny) * zd) + ys;
		SDL_RenderDrawLine(render, x1, y1, x2, y2);
	}*/
	ldbl xr = ldbl(width - 320) / (maxx - minx), yr = ldbl(height - 120) / (maxy - miny), dd;
	int xs = 260, ys = 60;
	if (xr < yr)
	{
		dd = xr;
		ys += int(((height - 120) - (maxy - miny) * dd) / 2.0);
	}
	else
	{
		dd = yr;
		xs += int(((width - 320) - (maxx - minx) * dd) / 2.0);
	}
	for (int i = 0; i < vertex.size(); i += 2)
	{
		int x1 = int((vertex[i].x - minx) * dd) + xs;
		int x2 = int((vertex[i + 1].x - minx) * dd) + xs;
		int y1 = int((vertex[i].y - miny) * dd) + ys;
		int y2 = int((vertex[i + 1].y - miny) * dd) + ys;
		SDL_RenderDrawLine(render, x1, y1, x2, y2);
	}
}