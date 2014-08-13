/**
 * @file draw.c
 * @author Alexander Schöberl 0942320
 * @date 27.05.2014
 *
 * @brief Functions to write pitch into string.
 */

#include <stdio.h>
#include <string.h>

#define WIDTH 9

/* Functions */
void draw_pitch(char *pitch, int *values);
void draw_border(char *border);
void draw_cell(char *cell, int value);

/* Implementations */
/**
 * @brief A pitch will be written into pitch using the given values. 
 * @param pitch: (WIDTH*4-3)*9+9 minimum length, must contain only '\0' chars
 * @param values: 16 ints, 0 and negative values become empty cells, WIDTH-2 maximum chars to write each value
 */
void draw_pitch(char *pitch, int *values) {
	char border[WIDTH*4-2] = {0};
	draw_border(border);
	char next_cell[WIDTH+1] = {0};
	
	strcat(pitch, border);
	strcat(pitch, "\n");
	
	draw_cell(next_cell, values[0]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[1]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[2]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[3]);
	strcat(pitch, next_cell);
	memset(next_cell, 0, WIDTH);
	strcat(pitch, "\n");
	
	strcat(pitch, border);
	strcat(pitch, "\n");
	
	draw_cell(next_cell, values[4]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[5]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[6]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[7]);
	strcat(pitch, next_cell);
	memset(next_cell, 0, WIDTH);
	strcat(pitch, "\n");
	
	strcat(pitch, border);
	strcat(pitch, "\n");
	
	draw_cell(next_cell, values[8]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[9]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[10]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[11]);
	strcat(pitch, next_cell);
	memset(next_cell, 0, WIDTH);
	strcat(pitch, "\n");
	
	strcat(pitch, border);
	strcat(pitch, "\n");
	
	draw_cell(next_cell, values[12]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[13]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[14]);
	strcat(pitch, next_cell);
	pitch[strlen(pitch)-1] = '\0';
	memset(next_cell, 0, WIDTH);
	draw_cell(next_cell, values[15]);
	strcat(pitch, next_cell);
	memset(next_cell, 0, WIDTH);
	strcat(pitch, "\n");
	strcat(pitch, border);
}

/**
 * @brief A pitch border will be written into border. 
 * @param border: WIDTH*4-2 minimum length, must contain only '\0' chars
 */
void draw_border(char *border) {
	char cell[WIDTH+1] = {0};
	cell[0] = '|';
	cell[WIDTH-1] = '|';
	int i;
	for (i = 1; i < WIDTH-1; ++i) {
		cell[i] = '-';
	}
	
	strcat(border, cell);
	border[strlen(border)-1] = '\0';
	strcat(border, cell);
	border[strlen(border)-1] = '\0';
	strcat(border, cell);
	border[strlen(border)-1] = '\0';
	strcat(border, cell);
}

/**
 * @brief A game cell will be written into cell. 
 * @param cell: WIDTH+1 minimum length, must contain only '\0' chars
 * @param value: WIDTH-2 maximum chars to write this value, 0 and negative values become empty cells
 */
void draw_cell(char *cell, int value) {
	char val_str[WIDTH-1] = {0};
	(void) sprintf(val_str, "%d", value);
	int wsps = (WIDTH-2)-strlen(val_str);
	
	if (value < 1)
		memset(val_str, ' ', strlen(val_str));
	
	int i = 0;
	cell[i] = '|';
	for (i = 1; i <= (wsps/2); ++i) {
		cell[i] = ' ';
	}
	(void) strcat(cell, val_str);
	for (i = i+strlen(val_str); i <= (WIDTH-2); ++i) {
		cell[i] = ' ';
	}
	cell[i] = '|';
}
