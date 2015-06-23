/*******************************************************************************
 * Controller for Battleship Module - Header file
 *
 * This module provide services for the controller module.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdbool.h>
#include "view_tictactoe.h"
#include "model_score.h"
#include "model_tictactoe.h"
#include "utils.h"
#include "model.h"
#define TICTACTOE_SIZE 3

struct tictactoe_grid{
	enum view_tictactoe_case grid[TICTACTOE_SIZE][TICTACTOE_SIZE];
};
static struct tictactoe_grid currentGrid;
static int tictactoe_selectedCases = 0;

bool model_tictactoe_is_end(){
	//check draw
	if(tictactoe_selectedCases == (TICTACTOE_SIZE*TICTACTOE_SIZE)){
		return 1;
	}
	return 0;
}
int model_tictactoe_valid(struct utils_position position){
	return position.x >= 0 && position.y >= 0 && position.x < 3 && position.y <3;
}

int model_tictactoe_get_case(struct utils_position position){
	if(model_tictactoe_valid(position)){
		return currentGrid.grid[position.x][position.y];
	}
	else{
		return tictactoe_empty;
	}
}

int model_tictactoe_add_case(struct utils_position position, enum view_tictactoe_case case_type){
	if(model_tictactoe_valid(position)){
		if(currentGrid.grid[position.x][position.y] == tictactoe_empty){
			tictactoe_selectedCases++;
			currentGrid.grid[position.x][position.y] = case_type;
			return 1;
		}
	}
	return 0;
}

int model_tictactoe_winner(struct utils_position p, int s){
	//check col
	for(int i = 0; i < TICTACTOE_SIZE; i++){
		if(currentGrid.grid[p.x][i] != s)
			break;
		if(i == TICTACTOE_SIZE-1){
			return 1;
		}
	}

	//check row
	for(int i = 0; i < TICTACTOE_SIZE; i++){
		if(currentGrid.grid[i][p.y] != s)
			break;
		if(i == TICTACTOE_SIZE-1){
			return 1;
		}
	}

	//check diag
	if(p.x == p.y){
		for(int i = 0; i < TICTACTOE_SIZE; i++){
			if(currentGrid.grid[i][i] != s)
				break;
			if(i == TICTACTOE_SIZE-1){
				return 1;
			}
		}
	}

	//check anti diag
	for(int i = 0;i<TICTACTOE_SIZE;i++){
		if(currentGrid.grid[i][(TICTACTOE_SIZE-1)-i] != s)
			break;
		if(i == TICTACTOE_SIZE-1){
			return 1;
		}
	}
	return 0;

}

void model_tictactoe_reset_grid(){
	tictactoe_selectedCases = 0;
	for (int i = 0; i<TICTACTOE_SIZE;i++){
		for( int j = 0; j< TICTACTOE_SIZE;j++){
			currentGrid.grid[i][j] = tictactoe_empty;
		}
	}
}
 
