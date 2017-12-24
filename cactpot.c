#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint8_t bool;
#define true 1
#define false 0

const uint32_t payouts[] = {10000, 36, 720, 360, 80, 252, 108, 72, 54, 180, 72, 180, 119, 36, 306, 1080, 144, 1800, 3600};

struct payout_totals {
	uint32_t UD;
	uint32_t DU;
	uint32_t R1;
	uint32_t R2;
	uint32_t R3;
	uint32_t C1;
	uint32_t C2;
	uint32_t C3;
};

void gen_combinations(uint8_t *cur_board, bool *used, struct payout_totals *totals, uint8_t num_used) {
	if (num_used == 9) {
		totals->UD += payouts[cur_board[0] + cur_board[4] + cur_board[8] - 6];
		totals->DU += payouts[cur_board[2] + cur_board[4] + cur_board[6] - 6];
		totals->R1 += payouts[cur_board[0] + cur_board[1] + cur_board[2] - 6];
		totals->R2 += payouts[cur_board[3] + cur_board[4] + cur_board[5] - 6];
		totals->R3 += payouts[cur_board[6] + cur_board[7] + cur_board[8] - 6];
		totals->C1 += payouts[cur_board[0] + cur_board[3] + cur_board[6] - 6];
		totals->C2 += payouts[cur_board[1] + cur_board[4] + cur_board[7] - 6];
		totals->C3 += payouts[cur_board[2] + cur_board[5] + cur_board[8] - 6];
		return;
	}

	int first_open;
	for (int i = 0; i < 9; i++) {
		if (cur_board[i] == 0) {
			first_open = i;
			break;
		}
	}
	for (int i = 1; i <= 9; i++) {
		if (used[i - 1] == false) {
			used[i - 1] = true;
			cur_board[first_open] = i;
			gen_combinations(cur_board, used, totals, num_used + 1);
			used[i - 1] = false;
			cur_board[first_open] = 0;
		}
	}
}

uint32_t factorial(uint32_t n) {
	if (n <= 1) {
		return n;
	}
	return n * factorial(n - 1);
}

void calculate_lines(uint8_t *cur_board, struct payout_totals *totals) {
	bool *used = malloc(9 * sizeof(bool));
	uint8_t num_used = 0;
	for (int i = 0; i < 9; i++) {
		if (cur_board[i] != 0) {
			used[cur_board[i] - 1] = true;
			num_used++;
		}
	}
	gen_combinations(cur_board, used, totals, num_used);
	uint32_t num_combinations = factorial(9 - num_used);

	totals->UD /= num_combinations;
	totals->DU /= num_combinations;
	totals->R1 /= num_combinations;
	totals->R2 /= num_combinations;
	totals->R3 /= num_combinations;
	totals->C1 /= num_combinations;
	totals->C2 /= num_combinations;
	totals->C3 /= num_combinations;
}

int main() {
	uint8_t *cur_board = malloc(9 * sizeof(uint8_t));
	struct payout_totals *totals = malloc(sizeof(struct payout_totals));

	cur_board[0] = 9;
	cur_board[1] = 0;
	cur_board[2] = 0;
	cur_board[3] = 0;
	cur_board[4] = 7;
	cur_board[5] = 0;
	cur_board[6] = 0;
	cur_board[7] = 0;
	cur_board[8] = 0;

	calculate_lines(cur_board, totals);
	
	printf("%u\t%u\t%u\t%u\t%u\n", totals->UD, totals->C1, totals->C2, totals->C3, totals->DU);
	printf("%u\t%u\t%u\t%u\n", totals->R1, cur_board[0], cur_board[1], cur_board[2]);
	printf("%u\t%u\t%u\t%u\n", totals->R2, cur_board[3], cur_board[4], cur_board[5]);
	printf("%u\t%u\t%u\t%u\n", totals->R3, cur_board[6], cur_board[7], cur_board[8]);
/*
	printf("UD: %u\n", totals->UD);
	printf("DU: %u\n", totals->DU);
	printf("R1: %u\n", totals->R1);
	printf("R2: %u\n", totals->R2);
	printf("R3: %u\n", totals->R3);
	printf("C1: %u\n", totals->C1);
	printf("C2: %u\n", totals->C2);
	printf("C3: %u\n", totals->C3);
*/
}
