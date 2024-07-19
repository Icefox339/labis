#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <string.h>
#include <WinUser.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h> 

#include "fonts.h"
//ëàáèêè
#include "stack.h"
#include "vector.h"

#define MAIN_MENU 0
#define START 1
#define USAGE 2
#define ABOUT 3
#define QUIT 4 
#define LABIRINT 5 
#define LOAD_FILE 6 
#define DRAW 7 
#define SAVE 8 
#define SAVED_SUCCESS 9


#define OFF 0 
#define BINARY_TREE 1 
#define HUNT_AND_KILL 2
#define WYLSON 3 
#define LOADED_LABI 4

#define LABI_START_WIDTH 10
#define LABI_START_HEIGHT LABI_START_WIDTH
#define LABI_START_SPEED 3

#define STATIC 0
#define DYNAMIC 1


#define MAX_FILE_NAME_LEN 256
#define MAX_VISIBLE_NAME_LEN 27
#pragma comment(lib, "opengl32.lib")

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

short drawing_method = DYNAMIC;

time_t start_time;
time_t end_time;

int width, height;
int name_count = 0;
int cells_counter = 0;
int location = 0; // 0 - main menu, 1 - start, 2 - usage, 3 - about, 4 - quit, 5 - labirint, 6 - load file, 7 - draw, 8 - save, 9 - saved_success
int labi = 0; // 0 - off,  1 - binary_tree, 2 - hunt_and_kill, 3 - wylson
short click_delay = 1; // äèëåé êëèêà äëÿ òîãî, ÷òîáû íå ðåãàëî ñðàçó íåñêîëüêî êíîïîê íà âñåõ ëîêàöèÿõ
int labi_width = LABI_START_WIDTH;
int labi_height = LABI_START_HEIGHT;
char file_name[MAX_FILE_NAME_LEN];
int file_name_counter = 0;
int speed = LABI_START_SPEED;
int labi_created = 0;
char file_buffer_name[1] ="\n";
int labi_force_reload = 0;
// ëàáèêè
enum CellState {
	Close,
	Open
};

enum VisitState {
	notVisited,
	inCurBranch,
	inDonePath,
};

struct Cell {
	int x;
	int y;
	enum CellState Left;
	enum CellState Right;
	enum CellState Top;
	enum CellState Bottom;
	enum CellState Visited;
} typedef Cell;
Cell** main_maze;


//áèáàáèêè
typedef struct {
	char name[20];
	float vert[8];
	BOOL hover; 
} TButton;
typedef struct {
	char name[500];
	float vert[8];
	BOOL hover;
} TBoard;
// main menu slots on
TButton btn[] = { 
	{"start", {300,200 , 500,200 , 500,260 , 300,260}, FALSE},
	{"usage", {300,320, 500,320 , 500,380 , 300,380}, FALSE}, 
	{"about", {300,440 , 500,440 , 500,500, 300,500}, FALSE},
	{"quit", {300,560 , 500,560 , 500,620 , 300,620}, FALSE},
};
int btnCnt = 4;

TBoard brds[] = {
	{"Welcome to very mine labirynthm generator!", {50,0, 200,0, 200,60 , 50,60}, FALSE}, // header board
	{"Powered by lansololabs", {0,700 , 150,700, 150,800 , 0,800}, FALSE},
};
int brdCnt = 2;
// main menu slots off

// about slots on 
TButton about_btn[] = {
	{"back", {50,20 , 200,20 , 200,80, 50,80}, FALSE},
};
int about_btnCnt = 1;
TBoard about_brd[] = {
	{"- SuperCreator: Pankov Ilya Dmitrievich\nCreators:\n- Savinov Igor (Application Designer) >.< &\nRomanov Leonid (generators rrAAwr)\n- Group 4851003/20002\n- Institute for Cyber Security and \nInformation Protection\n- Department of Information\nSecurity of Computer Systems\n- Saint Petersburg State \nTechnical University\n- 2023 year", {50,200 , 700,200 , 700,500 , 50,500}, FALSE},
};
int about_brdCnt = 1;
// about slots off

// usage slots on
TButton usage_btn[] = {
	{"back", {50,20 , 200,20 , 200,80, 50,80}, FALSE},
};
int usage_btnCnt = 1;
TBoard usage_brd[] = {
	{"You can easily use this application\nby your mouse input.\nPress 'start' to choose\nalgorythm to use for your\ngeneration process and then generate\nyour own labirynthm UWU!\nAlso use keyboard\nTo type name in LOAD or SAVE", {50,200 , 700,200 , 700,500 , 50,500}, FALSE},
};
int usage_brdCnt = 1;
// usage slots off

// start slots on
TButton start_btn[] = {
	
	{"hunt and kill", {250, 320, 600,320 , 600,380 , 250,380}, FALSE},
	{"binary tree", {250,200 , 600,200 , 600,260 , 250,260}, FALSE},
	{"wylson", {250,440 , 600,440 , 600,500, 250,500}, FALSE},
	{"load file", {250,560 , 600,560 , 600,620 , 250,620}, FALSE},
	{"back", {250,650, 600,650, 600,720, 250,720}, FALSE},
	
};
int start_btnCnt = 5;
TBoard start_brd[] = {
	{"Choose your algorythm to generate\n or load existing file", {50,0, 200,0, 200, 60, 50,60}, FALSE},
};
int start_brdCnt = 1;
// start slots off

// labirint slots on
TButton labi_btn[] = {
	{"next", {250, 320, 600,320 , 600,380 , 250,380}, FALSE},
	{"back", {250,650, 600,650, 600,720, 250,720}, FALSE},
	{"+S", {250, 170, 330,170 , 330,230 , 250,230}, FALSE},
	{"-S", {500, 170, 580,170 , 580,230 , 500,230}, FALSE},
	{"+R", {500, 250, 580,250 , 580,310 , 500,310}, FALSE},
	{"-R", {250, 250, 330,250 , 330,310 , 250,310}, FALSE},
};
int labi_btnCnt = 6;
TBoard labi_brd[] = {
	{"Choose labirythm's size in first string and\n rate in second string (higher->slower)", {50,0, 200,0, 200, 60, 50,60}, FALSE},
	{"5", {360,170 , 450,170 , 450,230 , 360,230}, FALSE},
	{"10", {360,250, 450, 250, 450,310, 360, 310}, FALSE},
};
int labi_brdCnt = 3;

// labitint slots off

// draw slots on
TButton draw_btn[] = {
	{"back", {10,120 , 150,120 , 150,180, 10,180}, FALSE},
	{"save", {10,200 , 150,200 , 150,260 , 10,260}, FALSE},
};
int draw_btnCnt = 2;
TBoard draw_brd[] = {
	{" ", {10,20 , 100, 20, 100,80, 10,80}, FALSE},
};
int draw_brdCnt = 1;
// draw slots off

// load file slots on 
TButton load_btn[] = {
	/*{"I", {250, 170, 330,170 , 330,230 , 250,230}, FALSE},
	{"D", {500, 170, 580,170 , 580,230 , 500,230}, FALSE},
	{"P", {500, 250, 580,250 , 580,310 , 500,310}, FALSE},*/
	{"next", {250, 320, 600,320 , 600,380 , 250,380}, FALSE},
	{"back", {10,120 , 150,120 , 150,180, 10,180}, FALSE},
};
int load_btnCnt = 2;


TBoard load_brd[] = {
	{"Type yours file name consists\n of less than 27 ZAGLAVNIE chars", {50,0, 200,0, 200, 60, 50,60}, FALSE},
	{" ",  {160,250, 250, 250, 150,210, 160, 310}, FALSE},
};
int load_brdCnt = 2;
// load slots off 

// save slots on 
TButton save_btn[] = {
	{"next", {250, 320, 600,320 , 600,380 , 250,380}, FALSE},
	//{"back", {10,120 , 150,120 , 150,180, 10,180}, FALSE},
};
int save_btnCnt = 1;


TBoard save_brd[] = {
	{"Type your file name consist\n of not more than 27 chars", {50,0, 200,0, 200, 60, 50,60}, FALSE},
	{"",  {160,250, 250, 250, 150,210, 160, 310}, FALSE},
};
int save_brdCnt = 2;
// save slots off

// saved slots on 
TButton saved_btn[] = {
	{"menu", {50,120 , 200,120 , 200,180, 50,180}, FALSE},
};
int saved_btnCnt = 1;
TBoard saved_brd[] = {
	{"Your labirintik was saved in file", {50,0, 200,0, 200, 60, 50,60}, FALSE},
	{"", {160,250, 250, 250, 150,210, 160, 310}, FALSE},
};
int saved_brdCnt = 2;
// saved slots off

char printCell(Cell cell) {
	if (cell.Left == Open && cell.Right == Open && cell.Top == Open && cell.Bottom == Open) {
		return '0';
	}
	else if (cell.Left == Open && cell.Right == Open && cell.Top == Open && cell.Bottom == Close) {
		return '1';
	}
	else if (cell.Left == Open && cell.Right == Open && cell.Top == Close && cell.Bottom == Open) {
		return '2';
	}
	else if (cell.Left == Open && cell.Right == Open && cell.Top == Close && cell.Bottom == Close) {
		return '3';
	}
	else if (cell.Left == Open && cell.Right == Close && cell.Top == Open && cell.Bottom == Open) {
		return '4';
	}
	else if (cell.Left == Open && cell.Right == Close && cell.Top == Open && cell.Bottom == Close) {
		return '5';
	}
	else if (cell.Left == Open && cell.Right == Close && cell.Top == Close && cell.Bottom == Open) {
		return '6';
	}
	else if (cell.Left == Open && cell.Right == Close && cell.Top == Close && cell.Bottom == Close) {
		return '7';
	}
	else if (cell.Left == Close && cell.Right == Open && cell.Top == Open && cell.Bottom == Open) {
		return '8';
	}
	else if (cell.Left == Close && cell.Right == Open && cell.Top == Open && cell.Bottom == Close) {
		return '9';
	}
	else if (cell.Left == Close && cell.Right == Open && cell.Top == Close && cell.Bottom == Open) {
		return 'A';
	}
	else if (cell.Left == Close && cell.Right == Open && cell.Top == Close && cell.Bottom == Close) {
		return 'B';
	}
	else if (cell.Left == Close && cell.Right == Close && cell.Top == Open && cell.Bottom == Open) {
		return 'C';
	}
	else if (cell.Left == Close && cell.Right == Close && cell.Top == Open && cell.Bottom == Close) {
		return 'D';
	}
	else if (cell.Left == Close && cell.Right == Close && cell.Top == Close && cell.Bottom == Open) {
		return 'E';
	}
	else if (cell.Left == Close && cell.Right == Close && cell.Top == Close && cell.Bottom == Close) {
		return 'F';
	}
}

void print_string(float x, float y, char* text, float r, float g, float b)
{
	static char buffer[99999]; // ~500 chars
	int num_quads;

	num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));

	glColor3f(r, g, b);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 16, buffer);
	glDrawArrays(GL_QUADS, 0, num_quads * 4);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void TButton_Show(TButton btn)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	if (btn.hover) glColor3f(1.0f, 0.5f, 0.0f); // öâåò êíîïî÷åê 
	else glColor3f(0.1f, 0.1f, 0.5f);
	glVertexPointer(2, GL_FLOAT, 0, btn.vert);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPushMatrix();
	glTranslatef(btn.vert[0], btn.vert[1], 0); // ïåðåäâèãàåì áóêâû ê êíîïêå
	glScalef(5, 5, 5); // óâåëè÷èâàåì òåêñò
	print_string(3, 3, btn.name, 1, 1, 1);
	glPopMatrix();
}
void TBoard_Show(TBoard brd)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glColor3f(0, 0, 0);
	glVertexPointer(2, GL_FLOAT, 0, brd.vert);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glPushMatrix();
	glTranslatef(brd.vert[0], brd.vert[1], 0); // ïåðåäâèãàåì áóêâû ê äîñêå
	glScalef(3, 3, 3); // óâåëè÷èâàåì òåêñò
	print_string(3, 3, brd.name, 1, 1, 1);
	glPopMatrix();
}
void Percentage_View(TBoard brd, float percentage)
{

	glEnableClientState(GL_VERTEX_ARRAY);
	glColor3f(0, 0, 0);
	glVertexPointer(2, GL_FLOAT, 0, brd.vert);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	int int_percentage = (int)percentage;
	char ujas[] = "    of 100%";
	char percent_string[4];
	_itoa_s(int_percentage, percent_string, 4, 10);
	int len = strlen(percent_string);
	ujas[0] = percent_string[0];
	if (len == 2)
	ujas[1] = percent_string[1];
	if (len == 3)
	ujas[2] = percent_string[2];
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glTranslatef(brd.vert[0], brd.vert[1], 0); // ïåðåäâèãàåì áóêâû ê äîñêå
	glScalef(3, 3, 3); // óâåëè÷èâàåì òåêñò
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0, 0, 0); glVertex2f(0, 15);
	glColor3f(0, 0, 0); glVertex2f(20, 15); glVertex2f(0, 0);
	glColor3f(0, 0, 0); glVertex2f(20, 0);
	glEnd();
	print_string(3, 3, ujas, 1, 1, 1);
	glPopMatrix();
}
void ShowCell(char index)
{
	//glPushMatrix();
	/*glLineWidth(5);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0.5, 0.5, 0.5); glVertex2f(0, 1);
	glColor3f(0.5, 0.5, 0.5); glVertex2f(1, 1); glVertex2f(0, 0);
	glColor3f(0.5, 0.5, 0.5); glVertex2f(1, 0);
	glEnd();*/
	if (index == '0') { // ïóñòàÿ êëåòêà
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glEnd();
	}
	if (index == '1') { // closed bot
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();

		
	}
	if (index == '2') { // closed top
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glEnd();
	}
	if (index == '3') { // closed bot top
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glEnd();
	}
	if (index == '4') { // closed right
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glEnd();
	}
	if (index == '5') { // closed right bot
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
		
	}
	if (index == '6') { // closed top right
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glEnd();
	}
	if (index == '7') { // closed top right bot 
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glEnd();
	}
	if (index == '8') { // closed left
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glEnd();
	}
	if (index == '9') { // closed left bot 
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
	}
	if (index == 'A') { // closed left top
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glEnd();
	}
	if (index == 'B') { // closed left top bot
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
	}
	if (index == 'C') { // closed right left
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
	}
	if (index == 'D') { // closed right bot left
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 1);
		glColor3f(0, 0, 0); glVertex2f(1, 1);
		glEnd();
	}
	if (index == 'E') { // closed right top left
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glColor3f(1,1, 1); glVertex2f(1, 1);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(1, 0);
		glEnd();
	}
	if (index == 'F') { // all closed
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glColor3f(1, 1, 1); glVertex2f(1, 1);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(1, 0);
		glEnd();
	}
	if (index == 'G') { // invisible left closed
		glBegin(GL_LINES);
		glColor3f(1, 1, 1); glVertex2f(0, 0);
		glColor3f(1, 1, 1); glVertex2f(0, 1);
		glEnd();
	}
	//glPopMatrix();
}
void Print_labi_file(Cell** maze,const int height_w,const int width_w)
{
	FILE* outputFile = fopen("maze.txt", "w");
	fprintf(outputFile, "%d\n", labi_height);
	fprintf(outputFile, "%d\n", labi_width);
	for (int v = 0; v < labi_height; v++) {
		for (int g = 0; g < labi_width; g++) {
			fprintf(outputFile, "%d ", printCell(maze[v][g]));
		}
		fprintf(outputFile, "\n");
	}
	fclose(outputFile);
}
bool mazePrepare(int height, int width, Cell** maze)
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (maze[y][x].Visited == false) {
				return true;
			}
		}
	}
	return false;
}

void addBranch(struct Stack* path, Cell** maze, HDC hDC) 
{
	Cell current = *(Cell*)pop(path);
	Cell next;
	while (!isEmptyStack(path)) {
		next = *(Cell*)pop(path);
		if (current.x == next.x) {//ãîðèçîíòàëüíî

			if (next.y > current.y) {
				maze[next.y][next.x].Top = Open;
				maze[current.y][current.x].Bottom = Open;
			}
			else {
				maze[current.y][current.x].Top = Open;
				maze[next.y][next.x].Bottom = Open;
			}
		}
		else {
			if (next.x > current.x) {//Òóò âðîäå î÷åâèäíî
				maze[current.y][current.x].Right = Open;
				maze[next.y][next.x].Left = Open;
			}
			else {//Òóò âðîäå î÷åâèäíî
				maze[current.y][current.x].Left = Open;
				maze[next.y][next.x].Right = Open;
			}
		}
		current = next;
		maze[current.y][current.x].Visited = inDonePath;
		//SwapBuffers(hDC);
		glPushMatrix();
		glTranslatef((int)current.x, labi_height -  (int)current.y, 0);
		
		ShowCell(printCell(maze[current.y][current.x]));
		
		cells_counter++;
		glPopMatrix();
		Percentage_View(draw_brd[0], (float)cells_counter * 100 / (float)(labi_width * labi_height));
		
		if (drawing_method == DYNAMIC) {
			SwapBuffers(hDC);
			Sleep(speed);
			SwapBuffers(hDC);
		}
		
		//if (cells_counter+1 == labi_width * labi_height) Sleep(200);
		
		

	}
}
void Show_labi_wylson(HDC hDC)
{
	glLoadIdentity();
	glScalef(1.3 / labi_width, 1.3 / labi_height, 1);
	glTranslatef(-labi_width * 0.30, -labi_height * 0.5, 0);
	srand(time(NULL));
	int width_work = labi_width;
	int height_work = labi_height;

	Cell** maze = (Cell**)malloc(sizeof(Cell*) * (height_work + 1));
	for (int row = 0; row < height_work + 1; row++) {
		maze[row] = (Cell*)malloc(sizeof(Cell) * width_work);
	}
	start_time = clock();
	if (!labi_created) {
		if (maze != NULL) {
			for (int y = 0; y < height_work; y++) {//Èíèöèàëèçèðóåì ìàòðèöó äëÿ ëàáèðèíòà. Íà êëåòî÷êó âñåãî 6 àðãóìåíòîâ- 4 ãðàíè 2-ìåðíîé ñòåíêè è êîîðäèíàòû êëåòêè ëàáèðèíòà
				for (int x = 0; x < width_work; x++) {
					maze[y][x].x = x;
					maze[y][x].y = y;
					maze[y][x].Bottom = Close;
					maze[y][x].Left = Close;
					maze[y][x].Right = Close;
					maze[y][x].Top = Close;
					maze[y][x].Visited = notVisited;
				}
			}

			maze[0][0].Visited = inDonePath;
			maze[0][0].Top = Open;
			maze[height_work - 1][width_work - 1].Bottom = Open;


			int count = 0;

			struct Stack* path = createStack();
			while (mazePrepare(height_work, width_work, maze)) {
				Cell branchStart;
				do {
					int x = rand() % width_work;
					int y = rand() % height_work;
					/*int x = 0;
					int y = 2;*/
					branchStart = maze[y][x];
				} while (branchStart.Visited == inDonePath);
				maze[branchStart.y][branchStart.x].Visited = inCurBranch;
				push(path, &maze[branchStart.y][branchStart.x]);

				while (maze[branchStart.y][branchStart.x].Visited != inDonePath) {

					int dir = 0;
					Cell nextStep[5] = { 0 };
					if ((branchStart.x >= 0) && (branchStart.x < width_work - 1)) {//Âïðàâî
						nextStep[dir] = maze[branchStart.y][branchStart.x + 1];
						dir++;
					}
					if ((branchStart.y > 0) && (branchStart.y <= height_work - 1)) {//Ââåðõ
						nextStep[dir] = maze[branchStart.y - 1][branchStart.x];
						dir++;
					}
					if ((branchStart.x > 0) && (branchStart.x <= width_work - 1)) {
						nextStep[dir] = maze[branchStart.y][branchStart.x - 1];//Âëåâî
						dir++;
					}
					if ((branchStart.y >= 0) && (branchStart.y < height_work - 1)) {//Âíèç
						nextStep[dir] = maze[branchStart.y + 1][branchStart.x];
						dir++;
					}

					Cell next_step;// Çäåñü áàã ñ íåïîíÿòíûì çíà÷åíèåì
					next_step = nextStep[rand() % dir];
					dir = 0;


					if (branchStart.x == next_step.x) {

						if (next_step.y > branchStart.y) {
							branchStart.y++;

						}
						else {
							branchStart.y--;

						}
					}
					else {
						if (next_step.x > branchStart.x) {
							branchStart.x++;

						}
						else {
							branchStart.x--;

						}
					}
					Cell removeCell;
					switch (maze[branchStart.y][branchStart.x].Visited) {
					case 0:
						push(path, &maze[branchStart.y][branchStart.x]);
						maze[branchStart.y][branchStart.x].Visited = inCurBranch;
						break;

					case 1: //Çàíÿòà, ñòèðàåì çàöèêëèâàíèå
						removeCell = *(Cell*)pop(path);
						maze[removeCell.y][removeCell.x].Visited = notVisited;
						while (!isEmptyStack(path) && (removeCell.x != branchStart.x || removeCell.y != branchStart.y)) {
							maze[removeCell.y][removeCell.x].Visited = notVisited;
							removeCell = *(Cell*)pop(path);
						}
						push(path, &maze[branchStart.y][branchStart.x]);

						break;
					case 2: {//Ãîòîâûé ïðîõîä
						push(path, &maze[branchStart.y][branchStart.x]);
						addBranch(path, maze, hDC);
						
						break;
					}
					}
				}





			}

		}

		end_time = clock();
		double time_total = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		FILE* time_file = fopen("time_labi.txt", "a+");
		fprintf(time_file, "%f - time; wylson       ; %d - size;\n", time_total, labi_width);
		fclose(time_file);
		Print_labi_file(maze, height_work, width_work);

	}



	labi_created = 1;
	SwapBuffers(hDC);
	
};
void Show_labi_hunt_and_kill(HDC hDC)
{
	glLoadIdentity();
	glScalef(1.3 / labi_width, 1.3 / labi_height, 1);
	glTranslatef(-labi_width * 0.30, -labi_height * 0.5, 0);
	srand(time(NULL));
	int width_work = labi_width;
	int height_work = labi_height;
	start_time = clock();
	Cell** maze = (Cell**)malloc(sizeof(Cell*) * (height_work + 1));
	for (int row = 0; row < height_work + 1; row++) {
		maze[row] = (Cell*)malloc(sizeof(Cell) * width_work + 1);
	}
	if (!labi_created) {
		for (int y = 0; y < height_work; y++) {//Èíèöèàëèçèðóåì ìàòðèöó äëÿ ëàáèðèíòà. Íà êëåòî÷êó âñåãî 6 àðãóìåíòîâ- 4 ãðàíè 2-ìåðíîé ñòåíêè è êîîðäèíàòû êëåòêè ëàáèðèíòà
			for (int x = 0; x < width_work; x++) {
				maze[y][x].x = x;
				maze[y][x].y = y;
				maze[y][x].Bottom = Close;
				maze[y][x].Left = Close;
				maze[y][x].Right = Close;
				maze[y][x].Top = Close;
				maze[y][x].Visited = notVisited;
			}
		}
		Cell branchStart;
		int x = rand() % width_work;
		int y = rand() % height_work;
		branchStart = maze[y][x];


		while (mazePrepare(height_work, width_work, maze)) {



			while (maze[branchStart.y][branchStart.x].Visited != inDonePath) {

				int dir = 0;
				Cell nextStep[5] = { 0 };
				if ((branchStart.x >= 0) && (branchStart.x < width_work - 1)) {//Âïðàâî
					nextStep[dir] = maze[branchStart.y][branchStart.x + 1];
					dir++;
				}
				if ((branchStart.y > 0) && (branchStart.y <= height_work - 1)) {//Ââåðõ
					nextStep[dir] = maze[branchStart.y - 1][branchStart.x];
					dir++;
				}
				if ((branchStart.x > 0) && (branchStart.x <= width_work - 1)) {
					nextStep[dir] = maze[branchStart.y][branchStart.x - 1];//Âëåâî
					dir++;
				}
				if ((branchStart.y >= 0) && (branchStart.y < height_work - 1)) {//Âíèç
					nextStep[dir] = maze[branchStart.y + 1][branchStart.x];
					dir++;
				}

				Cell next_step;
				next_step = nextStep[rand() % dir];
				if (branchStart.Visited != inDonePath) {
					maze[branchStart.y][branchStart.x].Visited = inDonePath;
					dir = 0;

					if (branchStart.x == next_step.x) {

						if (next_step.y > branchStart.y) {
							maze[next_step.y][next_step.x].Top = Open;
							maze[branchStart.y][branchStart.x].Bottom = Open;
							branchStart.y++;

						}
						else {
							maze[branchStart.y][branchStart.x].Top = Open;
							maze[next_step.y][next_step.x].Bottom = Open;
							branchStart.y--;

						}
					}
					else {
						if (next_step.x > branchStart.x) {
							maze[branchStart.y][branchStart.x].Right = Open;
							maze[next_step.y][next_step.x].Left = Open;
							branchStart.x++;

						}
						else {
							maze[branchStart.y][branchStart.x].Left = Open;
							maze[next_step.y][next_step.x].Right = Open;
							branchStart.x--;

						}
					}
				}
				glPushMatrix();
				glTranslatef((int)maze[branchStart.y][branchStart.x].x, labi_height - (int)maze[branchStart.y][branchStart.x].y, 0);
				ShowCell(printCell(maze[branchStart.y][branchStart.x]));
				

				glPopMatrix();
				
				if (drawing_method == DYNAMIC) {
					Percentage_View(draw_brd[0], (float)cells_counter * 100 / (float)(labi_width * labi_height));
					SwapBuffers(hDC);
					Sleep(speed);
					SwapBuffers(hDC);
				}

				switch (maze[branchStart.y][branchStart.x].Visited) {
				case notVisited:
					break;
				case inDonePath: {//Ãîòîâûé ïðîõîä
					Cell nextStepHunt[5] = { 0 };
					int dir = 0;
					if (mazePrepare(height_work, width_work, maze) == false) {
						break;
					}

					for (int y = 0; y < height_work; y++) {
						for (int x = 0; x < width_work; x++) {
							if (maze[y][x].Visited == false) {
								if (y < height_work - 1) {
									if (maze[y + 1][x].Visited == inDonePath) {//Âíèç
										nextStepHunt[dir] = maze[y + 1][x];
										dir++;
									}
								}
								if (y > 0) {
									if (maze[y - 1][x].Visited == inDonePath) {//Ââåðõ
										nextStepHunt[dir] = (maze[y - 1][x]);
										dir++;
									}
								}
								if (x < width_work - 1) {
									if (maze[y][x + 1].Visited == inDonePath) {//Âïðàâî
										nextStepHunt[dir] = (maze[y][x + 1]);
										dir++;
									}
								}
								if (x > 0) {
									if (maze[y][x - 1].Visited == inDonePath) {//Âëåâî
										nextStepHunt[dir] = (maze[y][x - 1]);
										dir++;
									}
								}
								if (dir > 0) {
									Cell next = nextStepHunt[rand() % dir];
									if (maze[y][x].x == next.x) {//ãîðèçîíòàëüíî

										if (next.y > maze[y][x].y) {
											maze[next.y][next.x].Top = Open;
											maze[y][x].Bottom = Open;
										}
										else {
											maze[y][x].Top = Open;
											maze[next.y][next.x].Bottom = Open;
										}
									}
									else {
										if (next.x > maze[y][x].x) {//Òóò âðîäå î÷åâèäíî
											maze[maze[y][x].y][maze[y][x].x].Right = Open;
											maze[next.y][next.x].Left = Open;
										}
										else {//Òóò âðîäå î÷åâèäíî
											maze[maze[y][x].y][maze[y][x].x].Left = Open;
											maze[next.y][next.x].Right = Open;
										}
									}

									glPushMatrix();
									glTranslatef((int)maze[y][x].x, labi_height - (int)maze[y][x].y, 0);
									ShowCell(printCell(maze[y][x]));
									cells_counter++;
									cells_counter++;
									glPopMatrix();
									
									if (drawing_method == DYNAMIC) {
										Percentage_View(draw_brd[0], (float)cells_counter * 100 / (float)(labi_width * labi_height));
										SwapBuffers(hDC);
										Sleep(speed);
										SwapBuffers(hDC);
									}
									branchStart = maze[y][x];

									y = height_work;
									x = width_work;
									break;
								}
								else
									continue;


							}
						}
					}
					break;
				}
				}
			}
			

		}
		end_time = clock();
		double time_total = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		FILE* time_file = fopen("time_labi.txt", "a+");
		fprintf(time_file, "%f - time; hunt and kill; %d - size;\n", time_total, labi_width);
		fclose(time_file);
		Print_labi_file(maze, height_work, width_work);
	}
	labi_created = 1;

}
void Show_labi_binary(HDC hDC) 
{
	glLoadIdentity();
	glScalef(1.3 / labi_width, 1.3 / labi_height, 1);
	glTranslatef(-labi_width*0.30, -labi_height*0.5, 0);
	srand(time(NULL));
	int width_work = labi_width;
	int height_work = labi_height;
	

	Cell** maze = (Cell**)malloc(sizeof(Cell*) * (height_work + 1));
	for (int row = 0; row < height_work + 1; row++) {
		maze[row] = (Cell*)malloc(sizeof(Cell) * width_work);
	}
	start_time = clock(); 
	if (!labi_created) {
		if (maze != NULL) {
			for (int x = 0; x < width_work; x++) {//Èíèöèàëèçèðóåì ìàòðèöó äëÿ ëàáèðèíòà. Íà êëåòî÷êó âñåãî 6 àðãóìåíòîâ- 4 ãðàíè 2-ìåðíîé ñòåíêè è êîîðäèíàòû êëåòêè ëàáèðèíòà
				for (int y = 0; y < height_work; y++) {
					maze[y][x].x = x;
					maze[y][x].y = y;
					maze[y][x].Bottom = Close;
					maze[y][x].Left = Close;
					maze[y][x].Right = Close;
					maze[y][x].Top = Close;
					maze[y][x].Visited = 0;
				}
			}

			int startX = 0;
			int startY = 0;

			maze[0][0].Top = Open; 
			maze[height_work - 1][width_work - 1].Bottom = Open;

			struct Stack* path = createStack();
			push(path, &maze[startY][startX]);

			while (!isEmptyStack(path)) {//Êîãäà çàêîí÷àòñÿ êëåòî÷êè)

				char newStringFlag = 0;
				Cell _cur_cell = *(Cell*)pop(path);

				Vector* nextStep = createVector();
				if ((_cur_cell.x >= 0) && (_cur_cell.x < width_work - 1)) {//Âïðàâî
					push_back(nextStep, &maze[_cur_cell.y][_cur_cell.x + 1]);
				}
				if ((_cur_cell.y > 0) && (_cur_cell.y < height_work)) {//Ââåðõ
					push_back(nextStep, &maze[_cur_cell.y - 1][_cur_cell.x]);
				}
				if ((_cur_cell.x == width_work - 1) && _cur_cell.y < height_work - 1) {//Ïåðåõîä íà íåêñò ñòðîêó
					newStringFlag = 1;
				}


				if (!isEmptyVector(nextStep) || newStringFlag != 0) {//Ïîêà åñòü õîòÿ áû îäíî ðåøåíèå
					Cell next_step;
					if (!newStringFlag) {//Íå ïåðåõîä ëè íà íîâóþ ñòðîêó
						next_step = *(Cell*)vGet(nextStep, rand() % vectorSize(nextStep));
						if (next_step.x != _cur_cell.x) {//Òóò âðîäå î÷åâèäíî
							maze[_cur_cell.y][_cur_cell.x].Right = Open;
							maze[next_step.y][next_step.x].Left = Open;
						}
						else if (next_step.y != _cur_cell.y) {
							maze[_cur_cell.y][_cur_cell.x].Top = Open;
							maze[next_step.y][next_step.x].Bottom = Open;
						}

						if (_cur_cell.y == height_work - 1 && _cur_cell.x == width_work - 1) {
							break;
						}
						next_step = maze[_cur_cell.y][_cur_cell.x + 1];//Øàæî÷åê âïðàâî

					}
					else {
						if (_cur_cell.y > 0) {
							maze[_cur_cell.y][_cur_cell.x].Top = Open;
							maze[_cur_cell.y - 1][_cur_cell.x].Bottom = Open;
						}
						next_step = maze[_cur_cell.y + 1][startX];
						newStringFlag = 0;
					}
					pop(path);//Âûêèäûâàåì îòðàáîòàííóþ êëåòî÷êó è ðåøåíèÿ ñ íåé âìåñòå
					push(path, &next_step);//Âïèñûâàåì íîâóþ êëåòî÷êó


					glPushMatrix();
					glTranslatef((int)_cur_cell.x, labi_height-(int)_cur_cell.y, 0);
					ShowCell(printCell(maze[_cur_cell.y][_cur_cell.x]));
					cells_counter++;

					glPopMatrix();
					
				
					//
					if (drawing_method == DYNAMIC) {
						Percentage_View(draw_brd[0], (float)cells_counter * 100 / (float)(labi_width * labi_height));
						SwapBuffers(hDC);
						Sleep(speed);
						SwapBuffers(hDC);
					}
					//
					

					/*glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
					glClear(GL_COLOR_BUFFER_BIT);
					SwapBuffers(hDC);*/
					
					/*Sleep(100);*/
					glTranslatef(-(int)_cur_cell.x, -(labi_height-(int)_cur_cell.y), 0);

					glTranslatef((int)_cur_cell.x, -(int)_cur_cell.y, 0);
					ShowCell(printCell(maze[_cur_cell.y][_cur_cell.x]));
					cells_counter++;
					glTranslatef(-(int)_cur_cell.x, (int)_cur_cell.y, 0);
				}
				else {
					pop(path);//Èçáàâëÿåìñÿ îò âàðèàíòîâ áåç ðåøåíèé
				}

			}
		}
		end_time = clock();
		double time_total = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		FILE* time_file = fopen("time_labi.txt", "a+");
		fprintf(time_file, "%f - time; binary tree  ; %d - size;\n", time_total, labi_width);
		fclose(time_file);
		Print_labi_file(maze, height_work, width_work);
	} 
	//SwapBuffers(hDC); ìóäðîñòü äëÿ áîäðîñòè)))) 
	
	labi_created = 1;

	
	/*for (int j = 0; j < labi_height; j++) {
		for (int i = 0; i < labi_width; i++) {
			glPushMatrix();
			glTranslatef(i, j, 0);
			ShowCell(1);
			glPopMatrix();
		}
	}*/
	
}
//void Print_labi_file(Cell** maze, int height_work, int width_work)
//{
//	FILE* outputFile = fopen("MAZE.txt", "w");
//	fprintf(outputFile, "%d\n", width_work);
//	fprintf(outputFile, "%d\n", height_work);
//	for (int y = 0; y < height_work; y++) {
//		for (int x = 0; x < width_work; x++) {
//			fprintf(outputFile, "%d ", printCell(maze[y][x]));
//		}
//		fprintf(outputFile, "\n");
//	}
//}
void SizeNRate_view(TBoard brd, TBoard brd2)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glColor3f(0, 0, 0);
	glVertexPointer(2, GL_FLOAT, 0, brd.vert);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	char mazafaka[] = "000000";
	_itoa_s(labi_width, mazafaka, 6, 10);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glTranslatef(brd.vert[0], brd.vert[1], 0); // ïåðåäâèãàåì áóêâû ê äîñêå
	glScalef(3, 3, 3); // óâåëè÷èâàåì òåêñò
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0, 0, 0); glVertex2f(0, 15);
	glColor3f(0, 0, 0); glVertex2f(20, 15); glVertex2f(0, 0);
	glColor3f(0, 0, 0); glVertex2f(20, 0);
	glEnd();
	print_string(3, 3, mazafaka, 1, 1, 1);
	if (drawing_method == DYNAMIC) {
		glPopMatrix();
		glEnableClientState(GL_VERTEX_ARRAY);
		glColor3f(0, 0, 0);
		glVertexPointer(2, GL_FLOAT, 0, brd2.vert);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
		_itoa_s(speed, mazafaka, 4, 10);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, width, height, 0, -1, 1);
		glTranslatef(brd2.vert[0], brd2.vert[1], 0); // ïåðåäâèãàåì áóêâû ê äîñêå
		glScalef(3, 3, 3); // óâåëè÷èâàåì òåêñò
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0, 0, 0); glVertex2f(0, 15);
		glColor3f(0, 0, 0); glVertex2f(20, 15); glVertex2f(0, 0);
		glColor3f(0, 0, 0); glVertex2f(20, 0);
		glEnd();
		print_string(3, 3, mazafaka, 1, 1, 1);

	
	}
	glPopMatrix();
}
//void Rate_view(TBoard brd)
//{
//	
//}
void Name_view(TBoard brd)
{
	char string_to_print[MAX_FILE_NAME_LEN];
	memset(string_to_print, ' ', MAX_FILE_NAME_LEN);
	for (int j = 0; j < file_name_counter; j++) {
		string_to_print[j] = file_name[j];
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glColor3f(0, 0, 0);
	glVertexPointer(2, GL_FLOAT, 0, brd.vert);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glTranslatef(brd.vert[0], brd.vert[1], 0); // ïåðåäâèãàåì áóêâû ê äîñêå
	glScalef(3, 3, 3); // óâåëè÷èâàåì òåêñò
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0, 0, 0); glVertex2f(0, 15);
	glColor3f(0, 0, 0); glVertex2f(20, 15); glVertex2f(0, 0);
	glColor3f(0, 0, 0); glVertex2f(20, 0);
	glEnd();
	print_string(3, 3, string_to_print, 1, 1, 1);
	glPopMatrix();
}
void Saved_view(TBoard brd)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glColor3f(0, 0, 0);
	glVertexPointer(2, GL_FLOAT, 0, brd.vert);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glTranslatef(brd.vert[0], brd.vert[1], 0); // ïåðåäâèãàåì áóêâû ê äîñêå
	glScalef(3, 3, 3); // óâåëè÷èâàåì òåêñò
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0, 0, 0); glVertex2f(0, 15);
	glColor3f(0, 0, 0); glVertex2f(20, 15); glVertex2f(0, 0);
	glColor3f(0, 0, 0); glVertex2f(20, 0);
	glEnd();
	print_string(3, 3, file_name, 1, 1, 1);
	glPopMatrix();
} 

BOOL PointInButton(int x, int y, TButton button)
{
	return ((x > button.vert[0]) && (x < button.vert[4]) && (y > button.vert[1]) && (y < button.vert[5]));
}

void ShowMenu(TButton buttons[], TBoard boards[], int button_count, int board_count)
{
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1); 
	for (int i = 0; i < button_count; i++) TButton_Show(buttons[i]);
	for (int i = 0; i < board_count; i++) TBoard_Show(boards[i]);
	glPopMatrix();
}
char* Parse_name(char* string_name) {
	char negr[MAX_FILE_NAME_LEN];
	memset(negr, ' ', MAX_FILE_NAME_LEN);
	for (int i = 0; i < file_name_counter; i++) {
		negr[i] = file_name[i];
	}
	negr[file_name_counter] = '.';
	negr[file_name_counter + 1] = 't';
	negr[file_name_counter + 2] = 'x';
	negr[file_name_counter + 3] = 't';
	negr[file_name_counter + 4] = '\0';
	return negr;
}
void Save_labi_file()
{
	rename("maze.txt", Parse_name(file_name));
}
void Draw_file(HDC hDC) {
	//if (labi_created) {
	FILE* output = NULL;
	
	if (labi == LOADED_LABI && labi_force_reload == 0) {
		output = fopen(Parse_name(file_name), "r");
	}
	else { output = fopen("maze.txt", "r"); }
	int lab_w;
	int	lab_h;
	fscanf(output, "%d\n", &lab_w);
	fscanf(output, "%d\n", &lab_h);
	glLoadIdentity();
	glScalef(1.3 / lab_w, 1.3 / lab_h, 1);
	glTranslatef(-lab_w * 0.30, -lab_h * 0.5, 0);
	
		int cellstate = 0;

		for (int i = 0; i < lab_h; i++) {

			for (int j = 0; j < lab_w; j++) {
				glPushMatrix();
				glTranslatef(j, lab_h - i, 0);
				fscanf(output, "%d", &cellstate);
				//SwapBuffers(hDC);
				ShowCell(cellstate);
				glPopMatrix();

				
			}
		}
		for (int i = 0; i < lab_h; i++) {
			glPushMatrix();
			glTranslatef(0, lab_h - i, 0);
			ShowCell('G');
			glPopMatrix();
		}
		fclose(output);
	//}
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hwnd;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL bQuit = FALSE;
	float theta = 0.0f;

	/* register window class */
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "GLSample";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


	if (!RegisterClassEx(&wcex))
		return 0;
	//resolution_width = GetSystemMetrics(SM_CXSCREEN);
	//resolution_height = GetSystemMetrics(SM_CYSCREEN);
	int wid = 800;
	int hei = 800;
	/*if (labi) {
		wid = resolution_width;
		hei = resolution_height;
	}*/
	/* create main window */
	hwnd = CreateWindowEx(0,
		"GLSample",
		"OpenGL Sample",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wid,
		hei,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, nCmdShow);

	/* enable OpenGL for the window */
	EnableOpenGL(hwnd, &hDC, &hRC);

	RECT rct;
	GetClientRect(hwnd, &rct);
	glOrtho(0, rct.right, rct.bottom, 0, 1, -1); // ïîâîðîò ìàòðèöû äëÿ áóêâ

	/* program main loop */
	while (!bQuit)
	{
		/* check for messages */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* handle or dispatch messages */
			if (msg.message == WM_QUIT)
			{
				bQuit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			/* OpenGL animation code goes here */

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);


			click_delay = 0;
			
			if (location == MAIN_MENU) ShowMenu(btn, brds, btnCnt, brdCnt);
			if (location == LABIRINT) {


				ShowMenu(labi_btn, labi_brd, labi_btnCnt+(drawing_method-1)*2, labi_brdCnt+drawing_method-1);
				SizeNRate_view(labi_brd[1], labi_brd[2]);
				//Rate_view();
			}
			if (location == ABOUT) ShowMenu(about_btn, about_brd, about_btnCnt, about_brdCnt);
			if (location == USAGE) ShowMenu(usage_btn, usage_brd, usage_btnCnt, usage_brdCnt);
			if (location == START) ShowMenu(start_btn, start_brd, start_btnCnt, start_brdCnt);
			if (location == SAVE){

				ShowMenu(save_btn, save_brd, save_btnCnt, save_brdCnt);
				Name_view(save_brd[1]);
				
			}
			if (location == SAVED_SUCCESS) {
				ShowMenu(saved_btn, saved_brd, saved_btnCnt, saved_brdCnt);
				Saved_view(saved_brd[1]);
			}
			if (location == LOAD_FILE) {
				ShowMenu(load_btn, load_brd, load_btnCnt, load_brdCnt);
				Name_view(load_brd[1]);
			}
			if (location == DRAW && labi == BINARY_TREE) {

				ShowMenu(draw_btn, draw_brd, draw_btnCnt, draw_brdCnt);

				if (!labi_created) Show_labi_binary(hDC);
				else {
					labi_force_reload = 1;
					labi = LOADED_LABI;
				}
				Percentage_View(draw_brd[0], 100);
			}
			if (location == DRAW && labi == WYLSON) {
				ShowMenu(draw_btn, draw_brd, draw_btnCnt, draw_brdCnt);

				if (!labi_created) Show_labi_wylson(hDC);
				else { 
					labi_force_reload = 1;
					labi = LOADED_LABI;
				}
				Percentage_View(draw_brd[0], 100); 
			}
			if (location == DRAW && labi == HUNT_AND_KILL) {
				ShowMenu(draw_btn, draw_brd, draw_btnCnt, draw_brdCnt);
				if (!labi_created) {
					Show_labi_hunt_and_kill(hDC);
				}
				else { 
					labi_force_reload = 1;
					labi = LOADED_LABI;
				}
				Percentage_View(draw_brd[0], 100);
			}
			if (location == DRAW && labi == LOADED_LABI) {
				ShowMenu(draw_btn, draw_brd, draw_btnCnt, draw_brdCnt);
				labi_created = 1; 
				Draw_file(hDC);
			}

				
				
			}
			SwapBuffers(hDC);

			
		}
	

	/* shutdown OpenGL */
	DisableOpenGL(hwnd, hDC, hRC);

	/* destroy the window explicitly */
	DestroyWindow(hwnd);

	return msg.wParam;
}
void Handle_key(char symbol)
{
	if (symbol != VK_BACK) {
		if (file_name_counter < MAX_VISIBLE_NAME_LEN) {
			file_name[file_name_counter] = symbol;
			file_name_counter++;
		}
	}
	if (symbol == VK_BACK) {
		if (file_name_counter > 0)file_name_counter--;
	}
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN: 
		if (location == MAIN_MENU) {
			file_name_counter = 0;
			labi_force_reload = 0;
			for (int i = 0; i < btnCnt; i++) {
				if (PointInButton(LOWORD(lParam), HIWORD(lParam), btn[i])) // ïðîâåðêà íà ïîïàäàíèå â êíîïêó
				{
					if (strcmp(btn[i].name, "quit") == 0) PostQuitMessage(0); // ïîïàäàíèå íà êíîïêó qèiò
					if (strcmp(btn[i].name, "about") == 0) location = ABOUT;
					if (strcmp(btn[i].name, "usage") == 0) location = USAGE;
					if (strcmp(btn[i].name, "start") == 0) {
						location = START;
						click_delay = 1;
						file_name_counter = 0;
					}
				}
			}
		}
		if (location == START && !click_delay) {
			for (int i = 0; i < start_btnCnt; i++) {
				if (PointInButton(LOWORD(lParam), HIWORD(lParam), start_btn[i])) // ïðîâåðêà íà ïîïàäàíèå â êíîïêó
				{
					if (strcmp(start_btn[i].name, "back") == 0) location = MAIN_MENU;
					
					if (strcmp(start_btn[i].name, "hunt and kill") == 0) {
						location = LABIRINT;
						labi = HUNT_AND_KILL;
						click_delay = 1;
					}
					if (strcmp(start_btn[i].name, "binary tree") == 0) {
						location = LABIRINT;
						labi = BINARY_TREE;
						click_delay = 1;
					}
					if (strcmp(start_btn[i].name, "wylson") == 0) {
						location = LABIRINT;
						labi = WYLSON;
						click_delay = 1;
					}

					if (strcmp(start_btn[i].name, "load file") == 0) {
						location = LOAD_FILE;
						name_count = 0;
						click_delay = 1;
						//memset(file_buffer_name, '\0', 4);
						//memset(file_name, '-', 4);

					}

				}
			}
		}
		if (location == ABOUT) {
			for (int i = 0; i < about_btnCnt; i++) {
				if (PointInButton(LOWORD(lParam), HIWORD(lParam), about_btn[i])) // ïðîâåðêà íà ïîïàäàíèå â êíîïêó
				{
					if (strcmp(about_btn[i].name, "back") == 0) location = MAIN_MENU;
				}
			}
		}
		if (location == USAGE) {
			for (int i = 0; i < usage_btnCnt; i++) {
				if (PointInButton(LOWORD(lParam), HIWORD(lParam), usage_btn[i])) // ïðîâåðêà íà ïîïàäàíèå â êíîïêó
				{
					if (strcmp(usage_btn[i].name, "back") == 0) location = MAIN_MENU;
				}
			}
		}
		if (location == LABIRINT && !click_delay) {
			file_name_counter = 0;
			labi_force_reload = 0;
			for (int i = 0; i < labi_btnCnt; i++) {
				if (PointInButton(LOWORD(lParam), HIWORD(lParam), labi_btn[i]))
				{
					if (strcmp(labi_btn[i].name, "back") == 0) {
						location = START;
						labi = 0;
						labi_created = 0;
						click_delay = 1;
					}
					if (strcmp(labi_btn[i].name, "next") == 0) {
						location = DRAW;
						click_delay = 1;
						labi_created = 0;
						memset(file_name, '\0', MAX_FILE_NAME_LEN);
					}
					if (strcmp(labi_btn[i].name, "+S") == 0) {
						if (labi_width && labi_height > 0) {
							labi_width += 1;
							labi_height += 1;
						}
					}
					if (strcmp(labi_btn[i].name, "-S") == 0) {
						if (labi_width && labi_height > 1) {
							labi_width -= 1;
							labi_height -= 1;
						}
					}
					if (strcmp(labi_btn[i].name, "-R") == 0) {
						if (speed > 10) speed -= 10;
						
					}
					if (strcmp(labi_btn[i].name, "+R") == 0) {
						if (speed > 1) speed += 10;

					}
					
				}
			}
		}
		if (location == LOAD_FILE) {
			for (int i = 0; i < load_btnCnt; i++) {
				if (PointInButton(LOWORD(lParam), HIWORD(lParam), load_btn[i]))
				{
					if (strcmp(load_btn[i].name, "back") == 0) {
						location = START;
						labi = OFF;
						labi_created = 0;
						click_delay = 1;
						name_count = 0; 
						file_name_counter = 0;
						//memset(file_buffer_name, '\0', 4);
						//memset(file_name, '-', 4);

					}
					if (strcmp(load_btn[i].name, "next") == 0) {
						FILE* input = fopen(Parse_name(file_name), "r");
							if (input) {
								location = DRAW;
								labi = LOADED_LABI;
								labi_created = 1;
								click_delay = 1;
								cells_counter = 0;
							}
							else {
								location = START;
								file_name_counter = 0;
							}
					}
					
				}
			}
		}
		if (location == DRAW) {
			for (int i = 0; i < draw_btnCnt; i++) {
				if (PointInButton(LOWORD(lParam), HIWORD(lParam), draw_btn[i]))
				{
					if (strcmp(draw_btn[i].name, "back") == 0) {
						location = START;
						labi = OFF;
						labi_created = 0;
						click_delay = 1;
						cells_counter = 0;
						file_name_counter = 0;
					}
					if (strcmp(draw_btn[i].name, "save") == 0) {
						location = SAVE;
						click_delay = 1;
						labi_created = 0;
						name_count = 0;
						file_name_counter = 0;
						//memset(file_buffer_name, '\0', 4);
						//memset(file_name, '-', 4);
					}
					

				}
			}
		}
		if (location == SAVED_SUCCESS) {
			for (int i = 0; i < saved_btnCnt; i++) {
				if (strcmp(saved_btn[i].name, "menu") == 0) {
					location = MAIN_MENU;
					labi = OFF;
					labi_created = 0;
					file_name_counter = 0;
					name_count = 0;
					click_delay = 1;
					//memset(file_buffer_name, '\0', 4);
					//memset(file_name, '-', 4);

				}
			}
		}
		if (location == SAVE) {
			for (int i = 0; i < save_btnCnt; i++) {
				if (PointInButton(LOWORD(lParam), HIWORD(lParam), save_btn[i]))
				{
					
					if (strcmp(save_btn[i].name, "next") == 0) {
						location = SAVED_SUCCESS;
						labi = 4;
						labi_created = 1;
						cells_counter = 0;
						click_delay = 1;
						Save_labi_file(); 
						

					}


				}
			}
		}
		break;
	case WM_MOUSEMOVE: 
		if (location == MAIN_MENU) for (int i = 0; i < btnCnt; i++) btn[i].hover = PointInButton(LOWORD(lParam), HIWORD(lParam), btn[i]); // ïîäñâåòêà êíîïîê
		if (location == ABOUT) for (int i = 0; i < about_btnCnt; i++) about_btn[i].hover = PointInButton(LOWORD(lParam), HIWORD(lParam), about_btn[i]);
		if (location == USAGE) for (int i = 0; i < usage_btnCnt; i++) usage_btn[i].hover = PointInButton(LOWORD(lParam), HIWORD(lParam), usage_btn[i]);
		if (location == START) for (int i = 0; i < start_btnCnt; i++) start_btn[i].hover = PointInButton(LOWORD(lParam), HIWORD(lParam), start_btn[i]);
		if (location == LABIRINT) for (int i = 0; i < labi_btnCnt; i++) labi_btn[i].hover = PointInButton(LOWORD(lParam), HIWORD(lParam), labi_btn[i]);
		if (location == DRAW) for (int i = 0; i < draw_btnCnt; i++) draw_btn[i].hover = PointInButton(LOWORD(lParam), HIWORD(lParam), draw_btn[i]);
		if (location == LOAD_FILE) for (int i = 0; i < load_btnCnt; i++) load_btn[i].hover = PointInButton(LOWORD(lParam), HIWORD(lParam), load_btn[i]);
		if (location == SAVE) for (int i = 0; i < save_btnCnt; i++) save_btn[i].hover = PointInButton(LOWORD(lParam), HIWORD(lParam), save_btn[i]);
		if (location == SAVED_SUCCESS) for (int i = 0; i < saved_btnCnt; i++) saved_btn[i].hover = PointInButton(LOWORD(lParam), HIWORD(lParam), saved_btn[i]);

		break;
	case WM_SIZE: 
		width = LOWORD(lParam);
		height = HIWORD(lParam); 
		glViewport(0, 0, width, height);
		glLoadIdentity();
		float k = 0;
		if (height != 0) {
			k = (float)width / (float)height;
		}
		else k = 1;
		glOrtho(-k, k, -1, 1, -1, 1);
		break;
	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
	{		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case 'A':
			Handle_key(wParam);
			break;
		case 'B':
			Handle_key(wParam);
			break;
		case 'C':
			Handle_key(wParam);
			break;
		case 'D':
			Handle_key(wParam);
			break;
		case 'E':
			Handle_key(wParam);
			break;
		case 'F':
			Handle_key(wParam);
			break;
		case 'G':
			Handle_key(wParam);
			break;
		case 'H':
			Handle_key(wParam);
			break;
		case 'I':
			Handle_key(wParam);
			break;
		case 'J':
			Handle_key(wParam);
			break;
		case 'K':
			Handle_key(wParam);
			break;
		case 'L':
			Handle_key(wParam);
			break;
		case 'M':
			Handle_key(wParam);
			break;
		case 'N':
			Handle_key(wParam);
			break;
		case 'O':
			Handle_key(wParam);
			break;
		case 'P':
			Handle_key(wParam);
			break;
		case 'Q':
			Handle_key(wParam);
			break;
		case 'R':
			Handle_key(wParam);
			break;
		case 'S':
			Handle_key(wParam);
			break;
		case 'T':
			Handle_key(wParam);
			break;
		case 'U':
			Handle_key(wParam);
			break;
		case 'V':
			Handle_key(wParam);
			break;
		case 'W':
			Handle_key(wParam);
			break;
		case 'X':
			Handle_key(wParam);
			break;
		case 'Y':
			Handle_key(wParam);
			break;
		case 'Z':
			Handle_key(wParam);
			break;
		case VK_BACK:
			Handle_key(wParam);
			break;
		}

	
	}
	break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC(hwnd);

	/* set the pixel format for the DC */
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}

