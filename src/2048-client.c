/**
 * @file 2048-client.c
 * @author Alexander Schöberl 0942320
 * @date 27.05.2014
 *
 * @brief Client entry point.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "interface.h"

/* Functions */
void usage_client(void);
void bail_out_client(char *message);

void close_con(void);
int create_game(void);
int load_game(int game_id);
int send_cmd(int game_id, int command);
int end_game(int game_id);

void draw_pitch(char *pitch, int *values);

/* Global Variables */
int game_id = 0;
int state = 0;
int values[16] = {0};

/* Implementations */
void set_game_id(int tmp_game_id) {
	game_id = tmp_game_id;
}

void set_state(int tmp_state) {
	state = tmp_state;
}

/**
 * @brief A game cell will be written into cell. 
 * @param cell: WIDTH+1 minimum length, must contain only '\0' chars
 * @param value: WIDTH-2 maximum chars to write this value, 0 and negative values become empty cells
 */
void set_values(int *tmp_values) {
	int i;
	for (i = 0; i < 16; i++) {
		if (tmp_values[i] < 0 || tmp_values[i] > 9999999)
			bail_out_client("Server Error");
		values[i] = tmp_values[i];
	}
}

void print_pitch(void) {
	char pitch[(9*4-3)*9+9] = {0};
	draw_pitch(pitch, values);
	printf("\033[H\033[J");
	(void) fprintf(stdout, "%s: %d\n", "Spielnummer: ", game_id);
	(void) fprintf(stdout, "%s\n", pitch);
	(void) fprintf(stdout, "%s\n", "a/d/w/s -> links/rechts/oben/unten");
	(void) fprintf(stdout, "%s\n", "q -> beenden, l -> aufgeben");
	fflush(stdout);
}

/**
 * @brief A pitch will be written into pitch using the given values. 
 * @param pitch: (WIDTH*4-3)*9+9 minimum length, must contain only '\0' chars
 * @param values: 16 ints, 0 and negative values become empty cells, WIDTH-2 maximum chars to write each value
 */
void run_game(void) {
	char c = '\0';
	while(1) {
		int r;
		print_pitch();
		if (state == ST_WON) {
			(void) fprintf(stdout, "Game %d WON!!!\n", game_id);
			break;
		} else if (state == ST_LOST) {
			(void) fprintf(stdout, "Game %d LOST!!!\n", game_id);
			break;
		} 
		c = fgetc(stdin);
		if (c == EOF)
			break;
		switch (c) {
			case 'a':
				r = send_cmd(game_id, CMD_LEFT);
				(void) fprintf(stdout, "Game %d left: %d\n", game_id, r);
				if (r < 0)
					bail_out_client("Server return Error");
				break;
			case 'd':
				r = send_cmd(game_id, CMD_RIGHT);
				(void) fprintf(stdout, "Game %d right: %d\n", game_id, r);
				if (r < 0)
					bail_out_client("Server return Error");
				break;
			case 'w':
				r = send_cmd(game_id, CMD_UP);
				(void) fprintf(stdout, "Game %d up: %d\n", game_id, r);
				if (r < 0)
					bail_out_client("Server return Error");
				break;
			case 's':
				r = send_cmd(game_id, CMD_DOWN);
				(void) fprintf(stdout, "Game %d down: %d\n", game_id, r);
				if (r < 0)
					bail_out_client("Server return Error");
				break;
			case 'l':
				r = end_game(game_id);
				(void) fprintf(stdout, "Game %d lost: %d\n", game_id, r);
				if (r < 0)
					bail_out_client("Server return Error");
				exit(EXIT_SUCCESS);
				break;
			case 'q':
				close_con();
				(void) fprintf(stdout, "Game %d stored\n", game_id);
				exit(EXIT_SUCCESS);
				break;
			default:
				break;
		}
	}
}

int main(int argc, char **argv) {
	int n_cntr = 0;
	int u_cntr = 0;
	char *id_str;
	char c;
	while ((c = getopt(argc, argv, "nu:")) != EOF) {
		switch (c) {
			case 'n':
				if (n_cntr != 0 || u_cntr != 0)
					usage_client();
				n_cntr += 1;
				break;
			case 'u':
				if (n_cntr != 0 || u_cntr != 0)
					usage_client();
				u_cntr += 1;
				id_str = optarg;
				break;
			case '?':
				usage_client();
				break;
			default:
				usage_client();
				break;
		}
	}
	if (u_cntr+n_cntr == 0)
		usage_client();
	
	if (n_cntr == 1) {
		game_id = create_game();
		if (game_id <= 0)
			game_id = create_game();
		if (game_id <= 0)
			bail_out_client("create Game Error");
		(void) fprintf(stdout, "Game %d created\n", game_id);
		run_game();
	}
	
	if (u_cntr == 1) {
		game_id = (int) strtol(id_str, NULL, 10);
		if (game_id <= 0)
			bail_out_client("load Game Error");
		game_id = load_game(game_id);
		if (game_id <= 0)
			bail_out_client("load Game Error");
		(void) fprintf(stdout, "Game %d loaded\n", game_id);
		run_game();
	}
}
