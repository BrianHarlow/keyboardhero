/*
 * kb_hero.c
 *
 *  Created on: Mar 24, 2019
 *      Author: Brian Laptop
 */


#include "project_settings.h"
#include "random_int.h"
#include "stddef.h"
#include "strings.h"
#include "game.h"
#include "timing.h"
#include "task.h"
#include "terminal.h"
#include "random_int.h"

#define DISPLAY_WIDTH 50
#define DISPLAY_HEIGHT 25
#define DATA_WIDTH 10
#define DATA_HEIGHT 5
#define LIFE_WIDTH 50
#define LIFE_HEIGHT 5
#define PRESS_WIDTH (DISPLAY_WIDTH / 5)
#define PRESS_HEIGHT 5
#define MIN_NOTE_RATE 300
#define MAX_NOTE_RATE 2000
#define KBH_NOTE_SPEED 75
#define KBH_MAX_NOTES 5

static void Callback(int argc, char * argv[]);
static void Receiver(uint8_t c);

typedef struct {
    char xmin;          // Minimum x coordinate for note
    char xmax;          // Maximum x coordinate for note
    char ymin;          // Minimum y coordinate for note
    char ymax;          // Maximum y coordinate for note
    char status;        // Status of object
} note_t;

static void Play(void);
static void Help(void);
static void MoveDownNote(note_t * note);

static void SendNote(void);     // Send the note down the terminal
static void GameOver(void);     // Game Over screen
static void AddScore(void);     // Update the user's score
static void AddHit(void);       // Update the user's misses
static void AddMiss(void);      // Update the user's hits
static void SetLife(void);      // Update the user's life
static void DrawFrame(void);    // Draw the border for the game

char life_c[4] = "Life";
char score_c[5] = "Score";
char hits_c[4] = "Hits";
char misses_c[6] = "Misses";

/// game structure
struct kb_hero_t {
    int life;           // Life for the round
    int score;          // Score for the round
    int hits;      // Notes hit for the round
    int misses;   // Notes missed for the round
    uint8_t id;         // ID of game=
};

static struct kb_hero_t game;
static note_t notes[KBH_MAX_NOTES];

void KB_HERO_Init(void) {
    // Register the module with the game system and give it the name "MUH3"
    game.id = Game_Register("KBH", "press the key of the note", Play, Help);
    // Register a callback with the game system.
    // this is only needed if your game supports more than "play", "help" and "highscores"
    Game_RegisterCallback(game.id, Callback);
}

void Help(void) {
    Game_Printf("Press the number corresponding to where the note is. Use the '1' - '5' keys!\r\n");
#endif
}

void Callback(int argc, char * argv[]) {
    // "play" and "help" are called automatically so just process "reset" here
    if(argc == 0) Game_Log(game.id, "too few args");
    if(strcasecmp(argv[0],"reset") == 0) {
        // reset scores
        game.score = 0;
        Game_Log(game.id, "Scores reset");
    }else Game_Log(game.id, "command not supported");
}

void Play(void){
    volatile uint8_t i;
    game.score = 0;
    game.hits = 0;
    game.misses = 0;
    DrawFrame();
    SetLife();
    Game_RegisterPlayer1Receiver(Receiver);
    for(i = 0; i < KBH_MAX_NOTES; i++) notes[i].status = 0;
    Game_HideCursor();

    Task_Schedule(SendNote, 0, 5000, 0);

}

void DrawFrame(void){
    int i;

    // Clear the screen
    Game_ClearScreen();
    // Draw the box for displaying the user's life
    Game_DrawRect(0, 0, LIFE_WIDTH, LIFE_HEIGHT);
    // Draw the display for the game itself
    Game_DrawRect(0, (LIFE_HEIGHT), DISPLAY_WIDTH, (LIFE_HEIGHT + DISPLAY_HEIGHT));
    // Draw the five rectangles for the press area
    Game_DrawRect(0, (LIFE_HEIGHT + DISPLAY_HEIGHT), PRESS_WIDTH, (LIFE_HEIGHT + DISPLAY_HEIGHT + PRESS_HEIGHT));
    Game_DrawRect(PRESS_WIDTH, (LIFE_HEIGHT + DISPLAY_HEIGHT), (2 * PRESS_WIDTH), (LIFE_HEIGHT + DISPLAY_HEIGHT + PRESS_HEIGHT));
    Game_DrawRect((2 * PRESS_WIDTH), (LIFE_HEIGHT + DISPLAY_HEIGHT), (3 * PRESS_WIDTH), (LIFE_HEIGHT + DISPLAY_HEIGHT + PRESS_HEIGHT));
    Game_DrawRect((3 * PRESS_WIDTH), (LIFE_HEIGHT + DISPLAY_HEIGHT), (4 * PRESS_WIDTH), (LIFE_HEIGHT + DISPLAY_HEIGHT + PRESS_HEIGHT));
    Game_DrawRect((4 * PRESS_WIDTH), (LIFE_HEIGHT + DISPLAY_HEIGHT), (5 * PRESS_WIDTH), (LIFE_HEIGHT + DISPLAY_HEIGHT + PRESS_HEIGHT));
    // Draw the box for displaying the score
    Game_DrawRect(LIFE_WIDTH, 0, (LIFE_WIDTH + DATA_WIDTH), DATA_HEIGHT);
    // Draw the box for displaying the hits for the round
    Game_DrawRect(LIFE_WIDTH, DATA_HEIGHT, (LIFE_WIDTH + DATA_WIDTH), (2 * DATA_HEIGHT));
    // Draw the box for displaying the misses for the round
    Game_DrawRect(LIFE_WIDTH, (2 * DATA_HEIGHT), (LIFE_WIDTH + DATA_WIDTH), (3 * DATA_HEIGHT));

    // Label the Life box
    for(i = 0; i < 5; i++){
        Game_CharXY(life_c[i], (25 + i), 1);
    }
    // Label the Score box
    for(i = 0; i < 6; i++){
        Game_CharXY(score_c[i], (53 + i), 2);
    }
    // Label the Hits box
    for(i = 0; i < 4; i++){
        Game_CharXY(hits_c[i], (54 + i), 7);
    }
    // Label the Misses box
    for(i = 0; i < 6; i++){
        Game_CharXY(misses_c[i], (53 + i), 12);
    }
    // Add lines between columns in display
    for(i = 0; i < 24; i++){
        Game_CharXY('|', 10, (6 + i));
    }
    for(i = 0; i < 24; i++){
        Game_CharXY('|', 20, (6 + i));
    }
    for(i = 0; i < 24; i++){
        Game_CharXY('|', 30, (6 + i));
    }
    for(i = 0; i < 24; i++){
        Game_CharXY('|', 40, (6 + i));
    }
}

void SendNote(void){
    uint8_t setColumn;
    setColumn = random_int(0,4);
    char xmin;
    char xmax;
    if(setColumn == 0){
        xmin = 2;
        xmax = 9;
        Terminal_SetColor(PLAYER1_UART, BackgroundGreen);
    }
    else if(setColumn == 1){
        xmin = 12;
        xmax = 19;
        Terminal_SetColor(PLAYER1_UART, BackgroundRed);
    }
    else if(setColumn == 2){
        xmin = 22;
        xmax = 29;
        Terminal_SetColor(PLAYER1_UART, BackgroundYellow);
    }
    else if(setColumn == 3){
        xmin = 32;
        xmax = 39;
        Terminal_SetColor(PLAYER1_UART, BackgroundCyan);
    }
    else if(setColumn == 4){
        xmin = 42;
        xmax = 49;
        Terminal_SetColor(PLAYER1_UART, BackgroundMagenta);
    }
    else{
        xmin = 2;
        xmax = 9;
        Terminal_SetColor(PLAYER1_UART, BackgroundGreen);
    }
    char ymin;
    char ymax;
    volatile uint8_t i;
    note_t * note = 0;
    // Find an unused note object
    for(i = 0; i < KBH_MAX_NOTES; i++) if(notes[i].status == 0) note = &notes[i];
    if(note) {
        note->status = 1;
        note->xmin = xmin;
        note->xmax = xmax;
        note->ymin = 6;
        note->ymax = 9;
        // use the task scheduler to move the char down and right at the calculated
        // period
        Task_Schedule((task_t)MoveDownNote, note, KBH_NOTE_SPEED, KBH_NOTE_SPEED);
        Game_FillRect(' ', note->xmin, note->ymin, note->xmax, note->ymax);
        Terminal_SetColor(PLAYER1_UART, BackgroundBlack);
    }
    Task_Schedule(SendNote, 0, random_int(MIN_NOTE_RATE, MAX_NOTE_RATE), 0);
}

void MoveDownNote(note_t * note){
    // Check if the note is still in play
    if(note->ymax < (LIFE_HEIGHT + DISPLAY_HEIGHT + PRESS_HEIGHT + 5)){
        // Clear the current position
        Game_FillRect(' ', note->xmin, note->ymin, note->xmax, note->ymax);
        // Update position
        note->ymin++;
        note->ymax++;
        // Re-print the note
        if(note->xmin == 2){
            Terminal_SetColor(PLAYER1_UART, BackgroundGreen);
        }
        else if(note->xmin == 12){
            Terminal_SetColor(PLAYER1_UART, BackgroundRed);
        }
        else if(note->xmin == 22){
            Terminal_SetColor(PLAYER1_UART, BackgroundYellow);
        }
        else if(note->xmin == 32){
            Terminal_SetColor(PLAYER1_UART, BackgroundCyan);
        }
        else if(note->xmin == 42){
            Terminal_SetColor(PLAYER1_UART, BackgroundMagenta);
        }
        else {
            Terminal_SetColor(PLAYER1_UART, BackgroundGreen);
        }
        Game_FillRect(' ', note->xmin, note->ymin, note->xmax, note->ymax);
        Terminal_SetColor(PLAYER1_UART, BackgroundBlack);
    }
    // If the note is not in play and needs to be cleared
    else{
        // At this point the user missed the note, so add to the misses
        AddMiss();
        SetLife();
        Game_FillRect(' ', note->xmin, note->ymin, note->xmax, note->ymax);
        Task_Remove((task_t)MoveDownNote, note);
        note->status = 0;
    }
}

void Receiver(uint8_t c) {
    uint8_t i;
    switch (c) {
        case '1':
            // If user presses the 1 key, check the first press note.
            for(i = 0; i < KBH_MAX_NOTES; i++){
                if(notes[i].xmin == 2){
                    if((notes[i].ymin >= 29) && (notes[i].ymin <= 34)){
                        AddScore();
                        AddHit();
                        Game_FillRect(' ', notes[i].xmin, notes[i].ymin, notes[i].xmax, notes[i].ymax);
                        Task_Remove((task_t)MoveDownNote, 0);
                        notes[i].status = 0;
                    }
                }
                else{
                    //AddMiss();
                    //SetLife();
                }
            }
            break;
        case '2':
            for(i = 0; i < KBH_MAX_NOTES; i++){
                if(notes[i].xmin == 12){
                    if((notes[i].ymin >= 29) && (notes[i].ymin <= 34)){
                        AddScore();
                        AddHit();
                        Game_FillRect(' ', notes[i].xmin, notes[i].ymin, notes[i].xmax, notes[i].ymax);
                        Task_Remove((task_t)MoveDownNote, 0);
                        notes[i].status = 0;
                    }
                }
                else{
                    //AddMiss();
                    //SetLife();
                }
            }
            break;
        case '3':
            for(i = 0; i < KBH_MAX_NOTES; i++){
                if(notes[i].xmin == 22){
                    if((notes[i].ymin >= 29) && (notes[i].ymin <= 34)){
                        AddScore();
                        AddHit();
                        Game_FillRect(' ', notes[i].xmin, notes[i].ymin, notes[i].xmax, notes[i].ymax);
                        Task_Remove((task_t)MoveDownNote, 0);
                        notes[i].status = 0;
                    }
                }
                else{
                    //AddMiss();
                    //SetLife();
                }
            }
            break;
        case '4':
            for(i = 0; i < KBH_MAX_NOTES; i++){
                if(notes[i].xmin == 32){
                    if((notes[i].ymin >= 29) && (notes[i].ymin <= 34)){
                        AddScore();
                        AddHit();
                        Game_FillRect(' ', notes[i].xmin, notes[i].ymin, notes[i].xmax, notes[i].ymax);
                        Task_Remove((task_t)MoveDownNote, 0);
                        notes[i].status = 0;
                    }
                }
                else{
                    //AddMiss();
                    //SetLife();
                }
            }
            break;
        case '5':
            for(i = 0; i < KBH_MAX_NOTES; i++){
                if(notes[i].xmin == 42){
                    if((notes[i].ymin >= 29) && (notes[i].ymin <= 34)){
                        AddScore();
                        AddHit();
                        Game_FillRect(' ', notes[i].xmin, notes[i].ymin, notes[i].xmax, notes[i].ymax);
                        Task_Remove((task_t)MoveDownNote, 0);
                        notes[i].status = 0;
                    }
                }
                else{
                    //AddMiss();
                    //SetLife();
                }
            }
            break;
        case '\r':
            //GameOver();
            break;
        default:
            break;
    }
}

void SetLife(void){
    volatile uint8_t i;
    // If you still have lives update the bar
    if(game.misses <= 10){
        // Clear the old bar
        for(i = 0; i < 10; i++){
            Game_CharXY(' ', 22 + i, 3);
        }
        // Draw the new bar
        Terminal_SetColor(PLAYER1_UART, BackgroundGreen);
        for(i = 0; i < (10 - game.misses); i++){
            Game_CharXY(' ', 22 + i, 3);
        }
        Terminal_SetColor(PLAYER1_UART, BackgroundBlack);
    }
    // If you don't have lives, end the game
    else{
        GameOver();
    }
}

void AddScore(void){
    game.score++;
    Terminal_CursorXY(PLAYER1_UART, 53, 2);
    Game_Printf("Score: %i", game.score);
}

void AddHit(void){
    game.hits++;
    Terminal_CursorXY(PLAYER1_UART, 54, 7);
    Game_Printf("Hits: %i", game.hits);
}

void AddMiss(void){
    game.misses++;
    Terminal_CursorXY(PLAYER1_UART, 51, 12);
    Game_Printf("Misses: %i", game.misses);
}

void GameOver(void){
    Game_SetColor(BackgroundBlack);
    Task_Remove(SendNote, 0);
    Game_ClearScreen();
    Game_Printf("GAME OVER!\r\n");
    Game_Printf("You scored %d points!", game.score);
}


