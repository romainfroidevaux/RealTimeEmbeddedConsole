EXEC=app
SRCS=$(wildcard *.c) 
ASRC=$(wildcard *.s)

#EXTRA_ALL= img_shell.xpm img_equality.xpm img_winner.xpm img_loser.xpm img_ship1.xpm img_ship2.xpm img_ship3.xpm img_stopwatch.xpm img_thermometer.xpm logo_eia.xpm logo_megaphone.xpm btn_battleship.xpm btn_play.xpm btn_replay.xpm btn_reset.xpm btn_returnToHome.xpm btn_rotate.xpm btn_scores.xpm btn_shell.xpm btn_ticTacToe.xpm btn_validate.xpm ttt_circleBlue.xpm ttt_circleOrange.xpm ttt_crossBlue.xpm ttt_crossOrange.xpm
EXTRA_CLEAN=clean_extra

.PHONY: clean_extra2
# Includee the standard APF27 makefile for the microprocessor labs 
include $(LMIBASE)/apf27/make/apf27.mk
CFLAGS+=-O1
#CFLAGS+=-D__QSORT__

logo_megaphone.xpm: images/logo_megaphone.jpg
	convert images/logo_megaphone.jpg images/logo_megaphone.xpm

logo_eia.xpm: images/logo_eia.jpg
	convert -resize 40% images/logo_eia.jpg images/logo_eia.xpm

btn_battleship.xpm: images/btn_battleship.jpg
	convert -resize 90% images/btn_battleship.jpg images/btn_battleship.xpm
	
btn_play.xpm: images/btn_play.jpg
	convert -resize 50% images/btn_play.jpg images/btn_play.xpm
	
btn_replay.xpm: images/btn_replay.jpg
	convert -resize 50% images/btn_replay.jpg images/btn_replay.xpm
	
btn_reset.xpm: images/btn_reset.jpg
	convert -resize 50% images/btn_reset.jpg images/btn_reset.xpm

btn_returnToHome.xpm: images/btn_returnToHome.jpg
	convert -resize 50% images/btn_returnToHome.jpg images/btn_returnToHome.xpm
	
btn_rotate.xpm: images/btn_rotate.jpg
	convert -resize 50% images/btn_rotate.jpg images/btn_rotate.xpm
	
btn_scores.xpm: images/btn_scores.jpg
	convert -resize 90% images/btn_scores.jpg images/btn_scores.xpm
	
btn_shell.xpm: images/btn_shell.jpg
	convert -resize 90% images/btn_shell.jpg images/btn_shell.xpm

btn_ticTacToe.xpm: images/btn_ticTacToe.jpg
	convert -resize 90% images/btn_ticTacToe.jpg images/btn_ticTacToe.xpm

btn_validate.xpm: images/btn_validate.jpg
	convert -resize 40% images/btn_validate.jpg images/btn_validate.xpm

ttt_circleBlue.xpm: images/ttt_circleBlue.jpg
	convert -resize 40% images/ttt_circleBlue.jpg images/ttt_circleBlue.xpm

ttt_circleOrange.xpm: images/ttt_circleOrange.jpg
	convert -resize 40% images/ttt_circleOrange.jpg images/ttt_circleOrange.xpm

ttt_crossBlue.xpm: images/ttt_crossBlue.jpg
	convert -resize 40% images/ttt_crossBlue.jpg images/ttt_crossBlue.xpm

ttt_crossOrange.xpm: images/ttt_crossOrange.jpg
	convert -resize 40% images/ttt_crossOrange.jpg images/ttt_crossOrange.xpm

img_thermometer.xpm: images/img_thermometer.jpg
	convert images/img_thermometer.jpg images/img_thermometer.xpm

img_stopwatch.xpm: images/img_stopwatch.jpg
	convert images/img_stopwatch.jpg images/img_stopwatch.xpm

img_ship1.xpm: images/img_ship1.jpg
	convert images/img_ship1.jpg images/img_ship1.xpm

img_ship2.xpm: images/img_ship2.jpg
	convert images/img_ship2.jpg images/img_ship2.xpm

img_ship3.xpm: images/img_ship3.jpg
	convert images/img_ship3.jpg images/img_ship3.xpm

img_winner.xpm: images/img_winner.jpg
	convert images/img_winner.jpg images/img_winner.xpm

img_loser.xpm: images/img_loser.jpg
	convert images/img_loser.jpg images/img_loser.xpm

img_equality.xpm: images/img_equality.jpg
	convert images/img_equality.jpg images/img_equality.xpm

img_shell.xpm: images/img_shell.jpg
	convert images/img_shell.jpg images/img_shell.xpm

clean_extra: 
	#rm -f images/*.xpm
	rm -f *.o