# TSRP Project Format v3

All coordinates and numeric database fields are little-endian signed 32-bit integers unless stated otherwise.

## File order

1. 64-byte project header
2. 192-byte project metadata
3. platforms
4. entities
5. platform map IDs
6. entity map IDs
7. map records
8. 40-byte database header
9. animations
10. skills
11. monsters
12. items
13. quests
14. variables
15. events
16. sprite assets
17. player classes

## Project header

| Offset | Meaning |
|---:|---|
| 0 | 8-byte magic `TSRP0003` |
| 8 | version = 3 |
| 12 | header size = 64 |
| 16 | platform count |
| 20 | entity count |
| 24 | map count |
| 28 | editor active map |
| 32 | screen width |
| 36 | screen height |
| 40 | player move speed |
| 44 | player jump speed |
| 48 | player max HP |
| 52 | player max MP |
| 56 | player base attack |
| 60 | project flags |

## Project metadata

| Offset | Meaning |
|---:|---|
| 0 | project name, 32-byte C string |
| 32 | game start map |
| 36 | save slot count, 1..3 |
| 40 | UI flags |
| 44–60 | idle/walk/air/hit/dead animation IDs (5 dwords) |
| 64 / 68 / 72 | HUD X / HUD Y / minimap enabled |
| 76–100 | skill 1, interact, item, jump, skill 2/3/4 key codes |
| 104 / 108 | window flags / audio enabled |
| 112 | executable basename, 48-byte C string ending in `.exe` |
| 160 | optional icon path, 32-byte C string |

## Map — 64 bytes

Name[32], background color, spawn X/Y, music ID, background sprite ID, layer flags, foreground sprite ID, tile sprite ID.

## Platform — 16 bytes

x1, y1, x2, y2. Ownership is stored in a parallel map-ID array.

## Entity — 16 bytes

type, x, y, param. Entity types are player, monster, NPC, portal, ladder, rope, checkpoint, decor and event trigger.

`param` links placed objects to data:
- monster → monster database ID
- NPC → quest ID
- portal → target map ID
- ladder/rope → length
- event trigger → event ID

## Databases

Animation records include frame timing and attack hitbox data. Skill records include damage, MP cost, cooldown, range, hit count, knockback and animation ID. Monster records include stats, rewards, drops, AI and animation. Item records include item type/prices/stats. Quest records cover kill/talk/collect/reach objectives. Variables are named integers.

Event records contain up to 8 commands. Commands are 16 bytes: opcode + three integer arguments. Current opcodes are dialogue, set variable/flag, add item, add money, change map, start quest, IF variable equals, and two-way choice.

Sprite records contain a name and relative PNG/BMP path. Imported assets are copied to `Assets`.

## Save format

Runtime saves use magic `TSAV0001` and persist active map, player position/stats, money, quest states/progress, variables and inventory.


## Record sizes and loader rules

The database header stores animation, skill, monster, item, quest, variable, event, sprite and class counts followed by one reserved dword. Record sizes are respectively 72, 64, 80, 64, 64, 40, 304, 160 and 72 bytes. `src/data.inc` defines each field offset.

Every read must return the full requested byte count. Counts are bounded before any array read. Names and dialogue must contain a NUL within their fixed field. Asset paths remain directly below `Assets\`; absolute paths and parent traversal are rejected. Map ownership IDs must reference an existing map. Invalid editor loads restore the previous in-memory project.

TSAV0001 remains a 64-byte header plus 64 quest states, 64 quest progress values, 128 variable values and 128 inventory counts (all dwords): 1,600 bytes total. Equipment IDs occupy header offsets 56 and 60. Saved database counts must match the current project. Level must be 1–10,000; HP growth and base attack growth are reconstructed from the project's starting class and saved level. A failed load never partially replaces live RPG state.

This save version has no project identity or content hash. Matching database counts do not prove that a save belongs to the same project revision. Checkpoint coordinates and defeated-monster state are not serialized.
