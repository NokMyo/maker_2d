# Tsuramechoki Controls

## Editor

| Input | Action |
|---|---|
| Q | Select |
| P | Platform |
| 1 | Player start |
| 2 | Monster |
| 3 | NPC |
| 4 | Portal |
| 5 | Ladder |
| 6 | Rope |
| 7 | Checkpoint |
| 8 | Decor |
| 9 | Event trigger |
| Arrow keys | Move selection 16 px |
| Delete | Delete selection |
| Mouse wheel | Horizontal editor camera |
| Home | Camera X = 0 |
| Ctrl+N | New starter project |
| Ctrl+S | Save project |
| Ctrl+O | Load project |
| Ctrl+M | Add map |
| PgUp / PgDn | Previous / next map |
| F1 | Project settings |
| F2 | Player settings |
| F6 | Cycle database |
| F7 | Import PNG/BMP sprite sheet |
| Tab | Next property field |
| + / - | Adjust current value |
| Insert | Add database record |
| F5 | Save + test game |
| F9 | Export Build/Game.exe (or configured basename) |
| F10 | Export and launch game |
| F3 / F4 / F11 | Map / HUD / key settings |
| F8 / Shift+F8 / Ctrl+F8 | Rename context / executable / event dialogue |
| Ctrl+F7 | Import game icon |
| F12 / Ctrl+F12 | Import BGM / sound effect WAV |
| Ctrl+Z / Ctrl+Y | One-level scene undo / redo |

When canvas mode is active, +/- changes the selected entity's `param`. This links a placed entity to database data: monster→monster ID, NPC→quest ID, portal→map ID, ladder/rope→length, event trigger→event ID.

## Runtime

| Input | Action |
|---|---|
| A/D or Left/Right | Move |
| Space | Jump |
| Up/Down | Climb ladder/rope |
| X | Attack |
| E | Interact |
| C | Use item 0 |
| I | Inventory panel |
| K | Skill panel |
| J | Quest panel |
| T | Stats panel |
| V | Equip first owned weapon/armor |
| F2 | Save |
| F3 | Load |
| F4 | Cycle save slot |
| 1 / 2 | Select event choice |
| Esc | Quit |


While renaming, ordinary key presses edit text without changing tools; Enter finishes and Escape exits text entry.
