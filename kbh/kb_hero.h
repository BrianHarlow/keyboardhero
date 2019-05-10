/**
 * @defgroup final_game Keyboard Hero Module
 *
 *  Created on: May 10, 2019
 *      @author Brian Harlow
 *
 * @brief This module implements a trap for a thief simulation game.
 *
 * Keyboard Hero is a terminal based game that simulates the popular
 * game "Guitar Hero" in the terminal. Notes are randomly generated
 * and moved down the display at a predetermined speed toward a press
 * area. When the note is in the press area, the user must use the
 * 1-5 keys on the keyboard to get a hit. The more notes the user hits
 * the better their score.
 *
 * The Task_Remove functions must be edited to fix an error present in
 * this module. When a key is pressed, instead of removing just the note
 * that was pressed, every note in the system is cleared.
 *
 * In order to use this module KB_HERO_Init() must be called.
 *
 * @{
 */

#ifndef KB_HERO_H_
#define KB_HERO_H_

/** @brief Initializing function for Keyboard Hero Game.
 *
 * Registers the game and initializes the game with an id.
 *
 *  KB_HERO_Init must be called before the game can be played.
 *  type "$game kbh play" to play the game.
 *
 */
void KB_HERO_Init(void);

/** @} */

#endif /* KB_HERO_H_ */


