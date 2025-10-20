# MemLCD-Tetris

Behold! A classic Tetris clone, painstakingly crafted to run on a Silicon Labs BRD4002A dev kit along with any Silabs Wireless board (tested with BRD4194A and BRD4310A) Why? This should be a good alternative example in the Gecko/Simplicity SDK as joystick and lcdmem display examples.

## Gameplay & Controls

It's Tetris, really classic one.

**Controls:**

| Action                   | Joystick     | Buttons          |
| ------------------------ | ------------ | ---------------- |
| **Move Piece**     | Left / Right | BTN0 / BTN1      |
| **Rotate Piece**   | Up           | -                |
| **Soft Drop**      | Down         | -                |
| **Menu Navigate**  | Up / Down    | -                |
| **Menu Adjust**    | Left / Right | -                |
| **Menu Select**    | Center Click | -                |
| **Return to Menu** | -            | BTN1 (Game Over) |

## Build It

Just import the project into Simplicity Studio and hit the debug button. If you're reading this, you probably know how it works.

## Next-Level Hacks

A project is never done. Here's the roadmap:

- [ ] Persistent High Scores (NVM3!)
- [ ] Hard Drop & T-Spins
- [ ] A much-needed Pause Button
- [ ] Bleep-bloop chiptune soundtrack
