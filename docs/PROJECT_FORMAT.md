# TSRP Project Format v2

All coordinates and numeric database fields are little-endian signed 32-bit integers unless stated otherwise.

## File order

1. 64-byte project header
2. 64-byte project metadata
3. platforms
4. entities
5. platform map IDs
6. entity map IDs
7. map records
8. 32-byte database header
9. animations
10. skills
11. monsters
12. items
13. quests
14. variables
15. events
16. sprite assets

## Project header

| Offset | Meaning |
|---:|---|
| 0 | 8-byte magic `TSRP0002` |
| 8 | version = 2 |
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
| 44 | reserved |

## Map — 64 bytes

Name[32], background color, spawn X/Y, music ID, reserved.

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

Sprite records contain a name and relative BMP path. Imported assets are copied to `Assets`.

## Save format

Runtime saves use magic `TSAV0001` and persist active map, player position/stats, money, quest states/progress, variables and inventory.
