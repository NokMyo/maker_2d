# Tsuramechoki Architecture

## Current target

- Platform: Windows x64
- Assembler: NASM
- ABI: Microsoft x64
- Editor UI: direct Win32 API
- Prototype rendering: GDI
- Runtime: separate native executable
- Program logic: assembly only
- C/C++ runtime: not used

## Current executable split

### Tsuramechoki.exe

Built from `src/editor.asm`.

Responsibilities:

- native editor window
- platform and entity authoring
- world-space editing camera
- selection/manipulation
- TSRP serialization
- test-run process launch

### TsuramechokiRuntime.exe

Built from `src/runtime.asm`.

Responsibilities:

- TSRP loading
- input
- player physics
- platform collision
- camera
- prototype combat
- monster behavior
- NPC/portal interaction
- runtime rendering

### Shared format

`src/project.inc` defines constants and record layouts shared by both executables.

## Data flow

Editor memory
→ `project.tsrp`
→ Runtime loader
→ playable test map

The runtime does not depend on the editor process after launch.

## Repository rule

Executable program logic must remain x86-64 assembly. Documentation and non-executable metadata may use their native text formats.

## Why GDI first

The current GDI renderer is deliberately temporary. It lets the editor, serialization, physics and test-play loop be proven before the project adds a larger rendering/asset layer.

The intended next rendering step is a native Direct2D or Direct3D assembly layer while retaining the same project data model.

## Planned module split

The prototype is intentionally still concentrated in two translation units. Once the feature boundaries settle, it can be split into assembly include/modules such as:

- editor/window.asm
- editor/canvas.asm
- editor/tools.asm
- editor/selection.asm
- core/project.asm
- core/serializer.asm
- runtime/player.asm
- runtime/physics.asm
- runtime/combat.asm
- runtime/events.asm
- render/gdi.asm
- render/assets.asm

This split must not introduce a second implementation language.
