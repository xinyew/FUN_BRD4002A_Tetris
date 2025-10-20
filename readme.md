# MemLCD-Tetris

Behold! A classic Tetris clone, painstakingly crafted to run on a Silicon Labs BRD4002A dev kit along with any Silabs Wireless board (tested with BRD4194A and BRD4310A) Why? This should be a good alternative example in the Gecko/Simplicity SDK as joystick and lcdmem display examples.

## Features

* **Pause/Resume:** Press `BTN1` during gameplay to pause or resume the action.
* **Save/Load System:**
  * Press `BTN0` during gameplay (or while paused) to save your progress to one of 5 available slots.
  * The system uses a FIFO (First-In, First-Out) strategy, overwriting the oldest save when all slots are full.
  * Access the "Load Game" menu to view, load, or delete saved games.
* **Persistent Scoreboard:**
  * View the top 5 high scores from the main menu.
  * Scores are automatically saved and updated after each game.
* **Hard Drop:** Press the center of the joystick to instantly drop a piece.
* **T-Spins:** The game recognizes T-Spins and awards bonus points.

## Gameplay & Controls

It's Tetris, really classic one.

**Controls:**

| Action                     | Joystick     | Buttons              |
| -------------------------- | ------------ | -------------------- |
| **Move Piece**       | Left / Right | -                    |
| **Rotate Piece**     | Up           | -                    |
| **Soft Drop**        | Down         | -                    |
| **Hard Drop**        | Center Click | -                    |
| **Pause/Resume**     | -            | `BTN1`             |
| **Save Game**        | -            | `BTN0` (In-Game)   |
| **Menu Navigate**    | Up / Down    | -                    |
| **Menu Adjust**      | Left / Right | -                    |
| **Menu Select**      | Center Click | -                    |
| **Return to Menu**   | -            | `BTN1` (Sub-menus) |
| **Delete Save Slot** | -            | `BTN0` (Slot Menu) |

## Build It

Just import the project into Simplicity Studio and hit the debug button. If you're reading this, you probably know how it works.

## Next-Level Hacks

A project is never done. Here's the roadmap:

- [X] Persistent High Scores (NVM3!)
- [X] Hard Drop & T-Spins
- [X] A much-needed Pause Button
- [ ] Using BLE device to control the game remotely
- [ ] Using Wifi t to contrtol the game remotely

## Development Journey

This section outlines the requirements and implementation steps taken during the development of this project.

### Requirement 1: Pause and Resume Functionality

* **User Request:** "can you please use button 1 for pause and resume game? a text should be shown at pause that press BTN1 to resume"
* **Implementation:**
  1. A new `GAME_STATE_PAUSED` was added to the game's state machine.
  2. `tetris_pause_game()` and `tetris_resume_game()` functions were implemented to handle the game's logic when paused and resumed. This involved stopping and restarting the main game timer.
  3. The drawing logic was modified to display a "PAUSED" message and a "Press BTN1 to resume" hint when the game is in the paused state.
  4. The main button handler was updated to trigger the pause/resume functions when `BTN1` is pressed during gameplay.

### Requirement 2: Pause Screen UI Update

* **User Request:** "i don't want you to hide the game interface when paused, but overlay the texts on the game interface"
* **Implementation:**
  1. The `tetris_draw_board()` function was refactored. Instead of clearing the screen for the pause message, it was modified to draw the game board and pieces first, and then render the pause text on top, creating an overlay effect.
  2. To ensure the overlay was consistently displayed, a call to `tetris_draw_board()` was added to the main application loop whenever the game is in the `GAME_STATE_PAUSED` state.

### Requirement 3: Persistent Save/Load Functionality

* **User Request:** "i created an example project of silabs on how to use nvm3. Can you please use that as a reference and implement store and load function using BTN0? ... when you reset or reboot the board, there will be a third option ... load game where the previously stored game ... will be recovered"
* **Implementation:**
  1. The `nvm3_baremetal` project was analyzed to understand the usage of the NVM3 library for non-volatile storage.
  2. NVM3 functionality was integrated into the Tetris project.
  3. `tetris_save_game()` and `tetris_load_game()` functions were implemented to write and read the entire game state to/from NVM.
  4. A "Load Game" option was added to the main menu, which would only appear if a saved game was detected in NVM on startup.
  5. A "Game Saved" message was implemented to appear for 2 seconds after saving.
  6. An issue where saving would fail was troubleshooted and resolved. The root cause was that the size of the game state was larger than the maximum object size supported by the NVM3 configuration. The save/load logic was refactored to split the game state into multiple smaller chunks.

### Requirement 4: Game Save Slots

* **User Request:** "...implement a saved game slot function ... 5 game slots ... separate interface ... use BTN0 to remove the slot ... BTN1 will be used to go back ... game saving will be available during game playing, and BTN0 will save/overwrite the current game state to the next available slot with FIFO strategy ... The slot name will be the date+time when the game was saved."
* **Implementation:**
  1. A system to manage 5 distinct save slots was designed and implemented.
  2. A new UI screen for slot selection was created, which is displayed when "Load Game" is selected. This screen shows the status of each slot ("Empty" or the save name).
  3. Input handling for the slot menu was implemented, including navigation, loading a game, deleting a slot with `BTN0`, and returning to the main menu with `BTN1`.
  4. A FIFO (First-In, First-Out) strategy for saving was implemented. When all slots are full, the oldest save is automatically overwritten.
  5. An initial attempt to use the RTCC (Real-Time Clock) for timestamps caused a system freeze. After troubleshooting, and at the user's request, the RTCC dependency was removed and replaced with a more robust NVM-based counter to ensure both correct save ordering and system stability.

### Requirement 5: Scoreboard and Slot Naming Update

* **User Request:** "can you please add another option and interface on the main menu: scoreboard where the top 5 highest scores are stored? also please updated the slot naming sheme as the slot# plus the current score when saved"
* **Implementation:**
  1. The slot naming scheme was updated to the format "Slot X: [Score]".
  2. A new "Scoreboard" screen and a corresponding game state were created.
  3. Logic to store and manage a persistent list of the top 5 high scores using NVM3 was implemented.
  4. At the end of each game, the final score is now automatically checked against the high score list and added if it qualifies.
  5. A "Scoreboard" option was added to the main menu to allow viewing the high scores.

### Requirement 6: UI and Gameplay Adjustments

* **User Requests:**
  * "can you please adjust the right part of the game playing interface, where the line# will be replaced with "BTN1:PAUSE BTN0:SAVE". rearrange the information to make it more intuitive"
  * "i mean the texts DEL insetad of DELETE is better but it's not currently shown on the slot interface"
  * "now when i click BTN0 when paused, the game saved texts are not showing up and it was indeed not saved. can you please fix this"
* **Implementation:**
  1. The in-game side panel was redesigned, removing the "Lines" count and adding button hints for "PAUSE" and "SAVE".
  2. The missing hint text on the slot menu was troubleshooted and its position was adjusted to ensure it was visible.
  3. A bug was fixed where saving while paused was not working by adding the necessary logic to the button handler for the `GAME_STATE_PAUSED` state.

### Requirement 7: Documentation & Advanced Gameplay

* **User Request:** "what is Hard Drop & T-Spins? can you implement it for me and update the readme accordingly?"
* **Implementation:**
  1. **Hard Drop:** Implemented a hard drop feature, assigned to the center click of the joystick. This instantly drops the current piece to the bottom of the playfield and awards points based on the distance dropped.
  2. **T-Spins:** Implemented T-Spin detection logic. The game now recognizes when a T-tetromino is rotated into a tight spot and awards significant bonus points, especially when clearing lines.
  3. The `readme.md` file was updated to include these new mechanics in the "Features" and "Controls" sections, and the corresponding items were checked off in the "Next-Level Hacks" list.
