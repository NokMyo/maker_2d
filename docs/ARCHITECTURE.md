# Leella Prelude Architecture

## Target

- Platform: Windows x64 first
- Assembler: NASM
- ABI: Microsoft x64
- GUI: direct Win32 API
- Rendering: GDI prototype, planned Direct2D/Direct3D path later
- Runtime: separate editor/runtime executables, both written in assembly

## Repository rule

Executable program logic must remain assembly-only. Documentation and non-executable metadata may use their native text formats.

## Core modules planned

- editor/window.asm
- editor/canvas.asm
- editor/tools.asm
- editor/properties.asm
- core/project.asm
- core/map.asm
- core/serializer.asm
- runtime/player.asm
- runtime/physics.asm
- runtime/combat.asm
- runtime/events.asm

The prototype intentionally starts as one assembly translation unit. It will be split after the first editor interactions stabilize.
