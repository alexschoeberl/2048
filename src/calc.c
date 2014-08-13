/**
 * @file calc.c
 * @author Alexander Schöberl 0942320
 * @date 27.05.2014
 *
 * @brief Functions to calculate 2048 values.
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/* Functions */
void call_srand(void);
int rand_lim(int limit);
void add_one(int *values);
int is_full(int *values);
int move(int *values, int rows[][4]);

void create_pitch(int *values);
int game_over(int *values, int max);
int move_left(int *values);
int move_right(int *values);
int move_up(int *values);
int move_down(int *values);

/* Global Variables */
int srand_called = 0;

/* Implementations */
/**
 * @brief A pitch will be written into pitch using the given values. 
 * @param pitch: (WIDTH*4-3)*9+9 minimum length, must contain only '\0' chars
 * @param values: 16 ints, negative values become empty cells, WIDTH-2 maximum chars to write each value
 */
void create_pitch(int *values) {
	call_srand();
	
	int p_1 = 0;
	int p_2 = 0;
	while(p_1 == p_2) {
		p_1 = rand_lim(15);
		p_2 = rand_lim(15);
	}
	
	int p_1_v = rand_lim(3);
	if (p_1_v < 3)
		p_1_v = 2;
	else
		p_1_v = 4;
	int p_2_v = rand_lim(3);
	if (p_2_v < 3)
		p_2_v = 2;
	else
		p_2_v = 4;
	
	values[p_1] = p_1_v;
	values[p_2] = p_2_v;
}

/**
 * @return 0: game NOT over, 1: game won, -1: game lost
 */
int game_over(int *values, int max) {
	int tmpL[16] = {0};
	int tmpR[16] = {0};
	int tmpU[16] = {0};
	int tmpD[16] = {0};
	int zero = 0;
	int i;
	for (i = 0; i < 16; ++i) {
		if (values[i] >= max)
			return 1;
		if (values[i] == 0)
			zero++;
		tmpL[i] = values[i];
		tmpR[i] = values[i];
		tmpU[i] = values[i];
		tmpD[i] = values[i];
	}
	if (zero > 0)
		return 0;
	if (move_left(tmpL) == -1 && move_right(tmpR) == -1 && move_up(tmpU) == -1 && move_down(tmpD) == -1)
		return -1;
	return 0;
}

int move_left(int *values) {
	int rows[4][4] = {
		{0, 1, 2, 3},
		{4, 5, 6, 7},
		{8, 9, 10, 11},
		{12, 13, 14, 15}
	};
	if (move(values, rows) == 1) {
		add_one(values);
		return 0;
	}
	return -1;
}

int move_right(int *values) {
	int rows[4][4] = {
		{3, 2, 1, 0},
		{7, 6, 5, 4},
		{11, 10, 9, 8},
		{15, 14, 13, 12}
	};
	if (move(values, rows) == 1) {
		add_one(values);
		return 0;
	}
	return -1;
}

int move_up(int *values) {
	int rows[4][4] = {
		{0, 4, 8, 12},
		{1, 5, 9, 13},
		{2, 6, 10, 14},
		{3, 7, 11, 15}
	};
	if (move(values, rows) == 1) {
		add_one(values);
		return 0;
	}
	return -1;
}

int move_down(int *values) {
	int rows[4][4] = {
		{12, 8, 4, 0},
		{13, 9, 5, 1},
		{14, 10, 6, 2},
		{15, 11, 7, 3}
	};
	if (move(values, rows) == 1) {
		add_one(values);
		return 0;
	}
	return -1;
}

/**
 * @brief Only use internally.
 */
int move(int *values, int rows[][4]) {
	int cntr = 0;
	int r, i, u;
	for (r = 0; r < 4; ++r) {
		// for each row:
		// first: put same together
		for (i = 0; i < 4; ++i) {
			if (values[rows[r][i]] != 0) {
				for (u = i+1; u < 4; ++u) {
					if (values[rows[r][i]] == values[rows[r][u]]) {
						values[rows[r][i]] = values[rows[r][i]]*2;
						values[rows[r][u]] = 0;
						u = 4;
						cntr++;
					} else if (values[rows[r][u]] != 0) {
						u = 4;
					}
				}
			}
		}
		// second: move to the left
		for (i = 0; i < 4; ++i) {
			if (values[rows[r][i]] == 0) {
				for (u = i+1; u < 4; ++u) {
					if (values[rows[r][u]] != 0) {
						values[rows[r][i]] = values[rows[r][u]];
						values[rows[r][u]] = 0;
						u = 4;
						cntr++;
					}
				}
			}
		}
	}
	if (cntr != 0)
		return 1;
	return 0;
}

/**
 * @brief Only use internally.
 */
void add_one(int *values) {
	call_srand();
	
	int cntr = 0;
	int i;
	for (i = 0; i < 16; ++i) {
		if (values[i] == 0) {
			cntr++;
			break;
		}
	}
	if (cntr == 0)
		return;
	
	int p_1 = rand_lim(15);
	while (values[p_1] > 0) {
		p_1 = rand_lim(15);
	}
	
	int p_1_v = rand_lim(3);
	if (p_1_v < 3)
		p_1_v = 2;
	else
		p_1_v = 4;
	
	values[p_1] = p_1_v;
}

/**
 * @brief Calls srand if it has not been called before. Only use internally.
 */
void call_srand(void) {
	if (srand_called == 0) {
		srand(time(NULL));
		srand_called++;
	}
}

/**
 * @brief Returns random value between 0 and limit. Only use internally.
 */
int rand_lim(int limit) {
	int divisor = RAND_MAX/(limit+1);
	int retval;
	do { 
		retval = rand() / divisor;
	} while (retval > limit);
	return retval;
}
