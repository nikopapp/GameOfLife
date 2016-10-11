/*This is the Life Wars Game.
It takes as input the coordinates of two #Life 1.06 files
and fills a board. Then it creates 5000 iterations, prints
the accumulative score of 50 games and decides the winner  */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "neillsdl2.h"

#define RECTSIZE                     2
#define MILLISECONDDELAY            10
#define PREFIX             "#Life 1.06"
#define HEIGHT                     576/RECTSIZE
#define WIDTH                      768/RECTSIZE
#define TOTAL_GAMES                  1
/* I assign team one number 1 and team two number 10
so that the sumNeighbour switch doesn't get clashing cases*/
#define TEAM_ONE                     1
#define TEAM_TWO                    10
#define DEAD_CELL                    0
#define GENERATIONS               5000
#define MAX_FILE                   120
#define POINTS                     100
#define TEAM_ONE_2          2*TEAM_ONE
#define TEAM_ONE_3          3*TEAM_ONE
#define TEAM_TWO_2          2*TEAM_TWO
#define TEAM_TWO_3          3*TEAM_TWO

void zeroFillArray(int p[HEIGHT][WIDTH]);
int  fileFillBoard(int p[HEIGHT][WIDTH], FILE *player, int team);
void nextStep(int cell[HEIGHT][WIDTH]
            , int newCell[HEIGHT][WIDTH], int sum[HEIGHT][WIDTH]);
int  sumNeighbours(int i, int j, int p[HEIGHT][WIDTH]);
int  overspill(int coordinate, int max);
int  scoreSum(int p [HEIGHT][WIDTH], int team);
void scorePrint(int scoreP1, int scoreP2, int games);
int draw(int board[HEIGHT][WIDTH], SDL_Simplewin sw, SDL_Rect rectangle);

struct life {
   int self[HEIGHT][WIDTH], sum[HEIGHT][WIDTH];
};

int main (int argc, char **argv)
{
   int i =0, games =0;
   struct life boardA, boardB;
   int scoreP1 =0, scoreP2 =0;
   char *fName1, *fName2;
   FILE *player1, *player2;

      SDL_Simplewin sw;
      SDL_Rect rectangle;
      rectangle.w = RECTSIZE;
      rectangle.h = RECTSIZE;

   if( argc == 3 ){
      fName1 = argv[1];
      fName2 = argv[2];
   }
   else{
      printf("Unable to input your 'arguments'\n");
      return(0);
   }
   Neill_SDL_Init(&sw);
   while (games<TOTAL_GAMES){
      zeroFillArray(boardA.self);
      zeroFillArray(boardB.self);
      i = 0;
      /*File Input */
      while(i < 2){
         i = 0;
         player1 = fopen(fName1, "r");
         zeroFillArray(boardA.self);
         i += fileFillBoard(boardA.self, player1, TEAM_ONE);
         fclose(player1);
         player2 = fopen(fName2, "r");
         i += fileFillBoard(boardA.self, player2, TEAM_TWO);
         fclose(player2);
      }

      i = 0;
      draw(boardA.self , sw, rectangle);
      SDL_Delay(2000);
      /*WHERE THE WAR HAPPENS*/
      while(i < GENERATIONS && !sw.finished){
         /*Instead of copying the board to a temporary
         array it oscilates between two boards--FASTER */
         nextStep(boardA.self, boardB.self, boardB.sum);
         draw(boardA.self , sw, rectangle);
         nextStep(boardB.self, boardA.self, boardA.sum);
         draw(boardA.self , sw, rectangle);
      /* Has anyone pressed ESC or killed the SDL window ?
         Must be called frequently - it's the only way of escaping */
         Neill_SDL_Events(&sw);

       i += 2;
      }
   scoreP1+=scoreSum(boardA.self, TEAM_ONE);
   scoreP2+=scoreSum(boardA.self, TEAM_TWO);
   scorePrint( scoreP1, scoreP2, games);
   /*sw.finished=0;*/
   games++;
   }
   /* Clear up graphics subsystems */
   atexit(SDL_Quit);
   return(0);
}
void initial(void)
{

}
int draw(int board[HEIGHT][WIDTH], SDL_Simplewin sw, SDL_Rect rectangle)
{
      int j,i;

      /* Sleep for a short time */
      SDL_Delay(MILLISECONDDELAY);


      for(i=0;i<HEIGHT;i++){
         for(j=0;j<WIDTH;j++){
            if(board[i][j]==0){
      /* Choose a random colour, a mixture of red, green and blue. */
      Neill_SDL_SetDrawColour(&sw, 100, 100, 100);
      /* Unfilled Rectangle, fixed size, random position */
      rectangle.x = j*RECTSIZE;
      rectangle.y = i*RECTSIZE;
      SDL_RenderFillRect(sw.renderer, &rectangle);
            }
            else if(board[i][j]==TEAM_ONE){
              Neill_SDL_SetDrawColour(&sw, 255,255,255);

      /* Filled Rectangle, fixed size, random position */
      rectangle.x = j*RECTSIZE;
      rectangle.y = i*RECTSIZE;
      SDL_RenderFillRect(sw.renderer, &rectangle);
            }
            else if(board[i][j]==TEAM_TWO){
              Neill_SDL_SetDrawColour(&sw, 0,0,0);

      /* Filled Rectangle, fixed size, random position */
      rectangle.x = j*RECTSIZE;
      rectangle.y = i*RECTSIZE;
      SDL_RenderFillRect(sw.renderer, &rectangle);
            }
                                   }
                                 }


      /* Update window - no graphics appear on some devices until this is finished */
      SDL_RenderPresent(sw.renderer);
      SDL_UpdateWindowSurface(sw.win);




   return 0;

}
/*FUNCTIONS*/
void scorePrint(int scoreP1, int scoreP2, int games)
{
   char *winner;
   if(scoreP1 > scoreP2){
      winner="Player 1";
   }
   else if(scoreP1 < scoreP2){
      winner = "Player 2";
   }
   else{
      winner = "Tie";
   }
   printf("%2d %5d %5d %s\n", games, scoreP1, scoreP2, winner );
   if(scoreP1 == scoreP2){
      winner = "Nobody";
   }
   if(games == TOTAL_GAMES-1){
      printf("%s wins by %d cells to %d cells\n", \
winner,scoreP1,scoreP2 );
  }
}
void zeroFillArray(int p[HEIGHT][WIDTH])
{
   int i,j;
   for(i=0; i < HEIGHT; i++){
      for(j=0; j < WIDTH; j++){
         p[i][j] = 0;
      }
   }
}
int fileFillBoard(int p[HEIGHT][WIDTH],FILE *player, int team)
{
   int i,x,y,xOrigin,yOrigin,xFin,yFin;
   time_t t;
   srand((unsigned) time(&t)+rand());
   xOrigin = rand()%WIDTH;
   yOrigin = rand()%HEIGHT;
      for(i=0; i<=POINTS; i++){
         if(fscanf(player, "%d%d", &x, &y)!=2){
            return(1);
         }
         xFin = overspill(xOrigin+x,WIDTH);
         yFin = overspill(yOrigin+y,HEIGHT);
         if(p[yFin][xFin] == DEAD_CELL){
            p[yFin][xFin] = team;
         }
         else if(p[yFin][xFin] != DEAD_CELL){
           return(0);
         }
     }
   return(1);
}
void nextStep(int cell[HEIGHT][WIDTH],
              int newCell[HEIGHT][WIDTH], int sum[HEIGHT][WIDTH])
{
   int j, i;
      for(i=0; i<HEIGHT; i++){
         for(j=0; j<WIDTH; j++){
            sum[i][j] = sumNeighbours(i,j,cell);
               switch (cell[i][j]) {
                  case DEAD_CELL:
                     if (sum[i][j] == TEAM_ONE_3 ||
                         sum[i][j] == (TEAM_ONE_2 + TEAM_TWO)){
                        newCell[i][j] = TEAM_ONE;
                     }
                     else if (sum[i][j] == TEAM_TWO_3 ||
                              sum[i][j] == (TEAM_ONE + TEAM_TWO_2)){
                        newCell[i][j] = TEAM_TWO;
                     }
                     else{
                        newCell[i][j] = DEAD_CELL;
                     }
                     break;
                  case TEAM_ONE:
                     if (sum[i][j] == TEAM_ONE_2 ||
                         sum[i][j] == TEAM_ONE_3){
                        newCell[i][j] = TEAM_ONE;
                     }
                     else{
                        newCell[i][j] = DEAD_CELL;
                     }
                     break;
                  case TEAM_TWO:
                     if (sum[i][j] == TEAM_TWO_2 ||
                         sum[i][j] == TEAM_TWO_3){
                        newCell[i][j] = TEAM_TWO;
                     }
                     else{
                        newCell[i][j] = DEAD_CELL;
                     }
                     break;
              }
        }
     }
}
int sumNeighbours(int i, int j, int p[HEIGHT][WIDTH])
{
   int iUp, iLow, jLeft, jRight, sum;
   i      = overspill(i,  HEIGHT);   /*Checking if coordinates*/
   iUp    = overspill(i-1,HEIGHT);   /*fall outside the board*/
   iLow   = overspill(i+1,HEIGHT);   /*and assigns them the */
   j      = overspill(j,  WIDTH);    /*equivalent inside the board*/
   jLeft  = overspill(j-1,WIDTH);
   jRight = overspill(j+1,WIDTH);
   sum = p[i][jRight]+p[i][jLeft]+   /*summing cells right-left    */
   p[iLow][j]+p[iUp][j]+             /*summing cells up-down       */
   p[iUp][jRight]+p[iUp][jLeft]+     /*summing cells Uright-Uleft  */
   p[iLow][jRight]+p[iLow][jLeft];   /*summing cells Dright-Dleft  */
   return(sum);
}
int overspill(int coordinate, int max)
{
   int division;
   if (coordinate>(max-1)){
      division = coordinate/(max-1);
      coordinate -= (division*(max));
   }
   if (coordinate < 0){
     coordinate += (max);
   }
   return (coordinate);
}
int scoreSum(int p[HEIGHT][WIDTH], int player)
{
   int i, j, sum=0;
   for(i=0; i<HEIGHT; i++){
      for(j=0; j<WIDTH; j++){
         if(p[i][j] == player){
         sum++;
         }
      }
   }
   return(sum);
}
