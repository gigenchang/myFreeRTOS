#include "game.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Player1
int16_t player1X = 10;
int16_t player1Y = 10;
uint16_t player1W = 60;
uint16_t player1H = 10;
uint8_t player1IsReversed = 1;

//Player2
int16_t player2X = LCD_PIXEL_WIDTH - 20;
int16_t player2Y = LCD_PIXEL_HEIGHT - 20;
uint16_t player2W = 60;
uint16_t player2H = 10;
uint8_t player2IsReversed = 1;

//Ball
uint16_t ballSize = 5;
int16_t ballX = ( LCD_PIXEL_WIDTH - 5 ) / 2;
int16_t ballY = ( LCD_PIXEL_HEIGHT - 5 ) / 2;
int16_t ballVX = 5;
int16_t ballVY = 5;
uint8_t ballIsRun = 0;

//Balls
typedef struct Ball {
	int16_t X;
	int16_t Y;
	int16_t VX;
	int16_t VY;
	int8_t visible;
	uint8_t isRun;
} Ball;


#define MAX_BALL_NUM 5
Ball balls[MAX_BALL_NUM];
uint8_t numOfBalls = 0;
uint8_t hasInitBalls = 0;

uint8_t hitTimes = 0;
uint16_t player2Color = LCD_COLOR_WHITE;


void
initBalls()
{
	uint8_t i = 0;
	for(; i<MAX_BALL_NUM; i++){
		balls[i].visible = 0;
		balls[i].isRun = 0;
	}
	hasInitBalls = 1;
}


//Mode
uint8_t demoMode = 0;


void
GAME_EventHandler1()
{
	if( STM_EVAL_PBGetState( BUTTON_USER ) ){

		player1IsReversed = 0;

		while( STM_EVAL_PBGetState( BUTTON_USER ) );

		player1IsReversed = 1;
	}
}

void
GAME_EventHandler2()
{
	if ( !hasInitBalls ) {
		initBalls();
	}
	if( IOE_TP_GetState()->TouchDetected ){
		uint8_t i = 0;
		for (;i<MAX_BALL_NUM; i++) {
			if ( !balls[i].visible ) {
				balls[i].X = player1X + player1W/2;
				balls[i].Y = player1Y;
				balls[i].VX = 0;
				balls[i].VY = 5;
				balls[i].visible = 1;
				balls[i].isRun = 1;
				break;
			}
		}
	}
/*
		player2IsReversed = 1;

		while( IOE_TP_GetState()->TouchDetected );

		player2IsReversed = 0;
	}
*/
}


void
GAME_EventHandler3()
{
	/*
	if( ballIsRun == 0 ){
		BallReset();
	}*/
}


void
GAME_Update()
{
	//Player1
	LCD_SetTextColor( LCD_COLOR_BLACK );
	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	LCD_DrawFullRect( player2X, player2Y, player2W, player2H );

	if( demoMode == 0 ){

		if( player1IsReversed )
			player1X -= 5;
		else
			player1X += 5;

		if( player1X <= 0 )
			player1X = 0;
		else if( player1X + player1W >= LCD_PIXEL_WIDTH )
			player1X = LCD_PIXEL_WIDTH - player1W;

		//Player2
		if( player2IsReversed )
			player2X -= 5;
		else
			player2X += 5;

		if( player2X <= 0 ) {
			player2X = 0;
			player2IsReversed = 0;
		}
		else if( player2X + player2W >= LCD_PIXEL_WIDTH ) {
			player2X = LCD_PIXEL_WIDTH - player2W;
			player2IsReversed = 1;
		}


		//修改所有的球的位置
		uint8_t i=0;
		for (; i<MAX_BALL_NUM; i++) {
			if (balls[i].visible && balls[i].isRun) {
				LCD_SetTextColor( LCD_COLOR_BLACK );
				LCD_DrawFullRect( balls[i].X, balls[i].Y, ballSize, ballSize );
				balls[i].X += balls[i].VX;
				balls[i].Y += balls[i].VY;
				if (balls[i].X >= player2X && balls[i].X <= player2X + player2W &&
					balls[i].Y >= player2Y && balls[i].Y <= player2Y + player2H) {
					if (hitTimes == 4) {
						hitTimes = 0;
					} else {
						hitTimes++;
					}
					player2Color -= 0x007F;
					if(player2Color < 0) {
						player2Color = LCD_COLOR_WHITE;
					}
					balls[i].visible = 0;
					balls[i].isRun = 0;
				}
				if (balls[i].Y + ballSize >= LCD_PIXEL_HEIGHT) {
					balls[i].visible = 0;
					balls[i].isRun = 0;
					break;
				}
			}
		}
	}
}

void drawAllBalls()
{
	uint8_t i = 0;
	for (; i<MAX_BALL_NUM; i++){
		if (balls[i].visible) {
			LCD_DrawFullRect(balls[i].X, balls[i].Y, ballSize, ballSize);
		}
	}
}



void
GAME_Render()
{
	LCD_SetTextColor( LCD_COLOR_WHITE );
	LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
	LCD_SetTextColor( player2Color );
	LCD_DrawFullRect( player2X, player2Y, player2W, player2H );
	LCD_SetTextColor( LCD_COLOR_WHITE );
	drawAllBalls();
	LCD_DrawLine( 10, LCD_PIXEL_HEIGHT / 2, LCD_PIXEL_WIDTH - 20, LCD_DIR_HORIZONTAL );
}
