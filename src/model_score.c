/*******************************************************************************
 * Model for Score Module - Header file
 *
 * This module provide services for the Score module.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include "model_score.h"
#include "model.h"
// --- Global variables --- //
static int scoreP1 = 0;
static int scoreP2 = 0;
//--------------------------------------------------------------------------//
// ------------------------ Global Funtions ------------------------------- //
//--------------------------------------------------------------------------//
int model_score_get_score(enum model_player player){
	if (player == model_player1){
		return scoreP1;
	}else{
		return scoreP2;
	}
}

void model_score_set_winner(enum model_player player){
	if(player == model_player1){
		scoreP1++;
	}else if(player == model_player2){
		scoreP2++;
	}
}

void model_score_reset(){
	scoreP1 = 0;
	scoreP2 = 0;
}
