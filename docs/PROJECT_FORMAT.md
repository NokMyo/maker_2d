# LPRJ Project Format v1

Leella Prelude currently uses a compact binary format named `project.lprj`.

All coordinates are signed 32-bit integers in world pixels.

## Header — 24 bytes

| Offset | Size | Meaning |
| ---: | ---: | --- |
| 0 | 8 | ASCII magic `LPRJ0001` |
| 8 | 4 | Format version, currently 1 |
| 12 | 4 | Platform count |
| 16 | 4 | Entity count |
| 20 | 4 | Reserved |

The header is followed immediately by the platform array and then the entity array.

## Platform record — 16 bytes

| Offset | Size | Meaning |
| ---: | ---: | --- |
| 0 | 4 | x1 |
| 4 | 4 | y1 |
| 8 | 4 | x2 |
| 12 | 4 | y2 |

The current editor creates horizontal one-way platforms, so y1 and y2 are normally equal.

Maximum platform count in v1: **256**.

## Entity record — 16 bytes

| Offset | Size | Meaning |
| ---: | ---: | --- |
| 0 | 4 | Type |
| 4 | 4 | X |
| 8 | 4 | Y |
| 12 | 4 | Parameter / reserved |

Entity types:

- 0 — none/dead runtime slot
- 1 — player start
- 2 — monster
- 3 — NPC
- 4 — portal

Maximum entity count in v1: **128**.

The last dword is deliberately reserved so the format can gain entity-specific data without immediately changing the base record size.
