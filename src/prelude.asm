; Leella Prelude Editor
; 100% x86-64 assembly / NASM / Win32 + GDI
;
; Current editor prototype:
; - map canvas with world-space platforms
; - tools: select/platform/player/monster/NPC/portal
; - click or keyboard tool switching
; - platform selection + Delete
; - project save/load (.lprj)
; - default starter map
; - F5 launches LeellaRuntime.exe after saving
; - no C/C++ runtime

bits 64
default rel

%include "project.inc"

global mainCRTStartup

extern GetModuleHandleA
extern LoadCursorA
extern RegisterClassExA
extern CreateWindowExA
extern DefWindowProcA
extern ShowWindow
extern UpdateWindow
extern GetMessageA
extern TranslateMessage
extern DispatchMessageA
extern PostQuitMessage
extern ExitProcess
extern BeginPaint
extern EndPaint
extern GetClientRect
extern CreateSolidBrush
extern FillRect
extern DeleteObject
extern CreatePen
extern SelectObject
extern MoveToEx
extern LineTo
extern SetTextColor
extern SetBkMode
extern TextOutA
extern InvalidateRect
extern SetCapture
extern ReleaseCapture
extern GetKeyState
extern CreateFileA
extern WriteFile
extern ReadFile
extern CloseHandle
extern CreateProcessA
extern lstrcpyA

%define CS_HREDRAW          0x0002
%define CS_VREDRAW          0x0001
%define WS_OVERLAPPEDWINDOW 0x00CF0000
%define WS_VISIBLE          0x10000000
%define CW_USEDEFAULT       0x80000000
%define SW_SHOW             5

%define WM_DESTROY          0x0002
%define WM_PAINT            0x000F
%define WM_ERASEBKGND       0x0014
%define WM_KEYDOWN          0x0100
%define WM_MOUSEMOVE        0x0200
%define WM_LBUTTONDOWN      0x0201
%define WM_LBUTTONUP        0x0202
%define WM_MOUSEWHEEL       0x020A

%define PS_SOLID            0
%define TRANSPARENT         1
%define IDC_ARROW           32512

%define GENERIC_READ        0x80000000
%define GENERIC_WRITE       0x40000000
%define FILE_SHARE_READ     0x00000001
%define CREATE_ALWAYS       2
%define OPEN_EXISTING       3
%define FILE_ATTRIBUTE_NORMAL 0x80
%define INVALID_HANDLE_VALUE -1

%define VK_CONTROL          0x11
%define VK_DELETE           0x2E
%define VK_LEFT             0x25
%define VK_UP               0x26
%define VK_RIGHT            0x27
%define VK_DOWN             0x28
%define VK_F5               0x74
%define VK_Q                0x51
%define VK_P                0x50
%define VK_S                0x53
%define VK_O                0x4F
%define VK_1                0x31
%define VK_2                0x32
%define VK_3                0x33
%define VK_4                0x34

%define TOOL_SELECT         0
%define TOOL_PLATFORM       1
%define TOOL_PLAYER         2
%define TOOL_MONSTER        3
%define TOOL_NPC            4
%define TOOL_PORTAL         5

%define LEFT_PANEL          220
%define RIGHT_PANEL         270
%define TOP_BAR             56
%define BOTTOM_BAR          26
%define GRID_SIZE           32
%define SNAP_SIZE           16

section .data
    class_name      db "LeellaPreludeEditor",0
    window_title    db "Leella Prelude - Assembly Side-scrolling RPG Maker",0

    project_path    db "project.lprj",0
    runtime_cmd_template db "LeellaRuntime.exe",0

    txt_brand       db "LEELLA PRELUDE",0
    txt_brand_len   equ $-txt_brand-1
    txt_maps        db "MAPS",0
    txt_maps_len    equ $-txt_maps-1
    txt_map1        db "Map_001",0
    txt_map1_len    equ $-txt_map1-1
    txt_tools       db "TOOLS",0
    txt_tools_len   equ $-txt_tools-1

    txt_select      db "[Q] Select",0
    txt_select_len  equ $-txt_select-1
    txt_platform    db "[P] Platform",0
    txt_platform_len equ $-txt_platform-1
    txt_player      db "[1] Player Start",0
    txt_player_len  equ $-txt_player-1
    txt_monster     db "[2] Monster",0
    txt_monster_len equ $-txt_monster-1
    txt_npc         db "[3] NPC",0
    txt_npc_len     equ $-txt_npc-1
    txt_portal      db "[4] Portal",0
    txt_portal_len  equ $-txt_portal-1

    txt_props       db "PROPERTIES",0
    txt_props_len   equ $-txt_props-1
    txt_none        db "Selection: none",0
    txt_none_len    equ $-txt_none-1
    txt_platform_selected db "Selection: platform",0
    txt_platform_selected_len equ $-txt_platform_selected-1
    txt_entity_selected db "Selection: entity",0
    txt_entity_selected_len equ $-txt_entity_selected-1

    txt_top         db "Ctrl+S Save   Ctrl+O Load",0
    txt_top_len     equ $-txt_top-1
    txt_test        db "F5  TEST GAME",0
    txt_test_len    equ $-txt_test-1
    txt_status      db "Pure x86-64 assembly editor | 16 px snap | project.lprj",0
    txt_status_len  equ $-txt_status-1

    txt_active_select db "Active tool: Select",0
    txt_active_select_len equ $-txt_active_select-1
    txt_active_platform db "Active tool: Platform",0
    txt_active_platform_len equ $-txt_active_platform-1
    txt_active_player db "Active tool: Player Start",0
    txt_active_player_len equ $-txt_active_player-1
    txt_active_monster db "Active tool: Monster",0
    txt_active_monster_len equ $-txt_active_monster-1
    txt_active_npc db "Active tool: NPC",0
    txt_active_npc_len equ $-txt_active_npc-1
    txt_active_portal db "Active tool: Portal",0
    txt_active_portal_len equ $-txt_active_portal-1

    project_header:
        db "LPRJ0001"
        dd PROJECT_VERSION
        dd 0
        dd 0
        dd 0

    col_bg          dd 0x00171311
    col_panel       dd 0x00211D1B
    col_top         dd 0x00282320
    col_canvas      dd 0x001D1A18
    col_grid        dd 0x00352F2B
    col_platform    dd 0x00E8C56F
    col_selected    dd 0x0069B5F2
    col_preview     dd 0x007FD9A1
    col_text        dd 0x00F1EEE9
    col_muted       dd 0x00A9A39A
    col_player      dd 0x00E8D9B5
    col_monster     dd 0x006B6BE0
    col_npc         dd 0x0076C48A
    col_portal      dd 0x00D67FD4
    col_tool_active dd 0x004D463F

section .bss
    hinstance       resq 1
    hwnd_main       resq 1
    msg_buf         resb 64
    wc_buf          resb 80
    paint_buf       resb 80
    client_rect     resd 4
    temp_rect       resd 4
    io_bytes        resd 1

    startup_info    resb 104
    process_info    resb 24
    runtime_cmd     resb 128

    tool_mode       resd 1
    selected_platform resd 1
    selected_entity resd 1
    editor_camera_x resd 1

    dragging        resd 1
    drag_start_x    resd 1
    drag_start_y    resd 1
    drag_cur_x      resd 1
    drag_cur_y      resd 1

    platform_count  resd 1
    entity_count    resd 1
    platforms       resd MAX_PLATFORMS*4
    entities        resd MAX_ENTITIES*4

section .text

mainCRTStartup:
    and rsp, -16
    sub rsp, 192

    mov dword [selected_platform], -1
    mov dword [selected_entity], -1
    mov dword [editor_camera_x], 0
    mov dword [tool_mode], TOOL_PLATFORM

    call load_project
    test eax, eax
    jnz .project_ready
    call init_default_project

.project_ready:
    xor ecx, ecx
    call GetModuleHandleA
    mov [hinstance], rax

    lea rdi, [wc_buf]
    xor eax, eax
    mov ecx, 10
    rep stosq

    mov dword [wc_buf+0], 80
    mov dword [wc_buf+4], CS_HREDRAW | CS_VREDRAW
    lea rax, [WndProc]
    mov [wc_buf+8], rax
    mov rax, [hinstance]
    mov [wc_buf+24], rax

    xor ecx, ecx
    mov edx, IDC_ARROW
    call LoadCursorA
    mov [wc_buf+40], rax

    lea rax, [class_name]
    mov [wc_buf+64], rax

    lea rcx, [wc_buf]
    call RegisterClassExA
    test ax, ax
    jz .fail

    xor ecx, ecx
    lea rdx, [class_name]
    lea r8, [window_title]
    mov r9d, WS_OVERLAPPEDWINDOW | WS_VISIBLE
    mov qword [rsp+32], CW_USEDEFAULT
    mov qword [rsp+40], CW_USEDEFAULT
    mov qword [rsp+48], 1280
    mov qword [rsp+56], 800
    mov qword [rsp+64], 0
    mov qword [rsp+72], 0
    mov rax, [hinstance]
    mov [rsp+80], rax
    mov qword [rsp+88], 0
    call CreateWindowExA
    test rax, rax
    jz .fail

    mov [hwnd_main], rax

    mov rcx, rax
    mov edx, SW_SHOW
    call ShowWindow
    mov rcx, [hwnd_main]
    call UpdateWindow

.loop:
    lea rcx, [msg_buf]
    xor edx, edx
    xor r8d, r8d
    xor r9d, r9d
    call GetMessageA
    cmp eax, 0
    jle .quit

    lea rcx, [msg_buf]
    call TranslateMessage
    lea rcx, [msg_buf]
    call DispatchMessageA
    jmp .loop

.quit:
    mov ecx, dword [msg_buf+16]
    call ExitProcess

.fail:
    mov ecx, 1
    call ExitProcess


; ------------------------------------------------------------
; WndProc
; ------------------------------------------------------------
WndProc:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-64], r12
    mov [rbp-72], r13

    mov [rbp-8], rcx
    mov [rbp-12], edx
    mov [rbp-24], r8
    mov [rbp-32], r9

    cmp edx, WM_PAINT
    je .paint
    cmp edx, WM_ERASEBKGND
    je .erase
    cmp edx, WM_KEYDOWN
    je .keydown
    cmp edx, WM_LBUTTONDOWN
    je .mouse_down
    cmp edx, WM_MOUSEMOVE
    je .mouse_move
    cmp edx, WM_LBUTTONUP
    je .mouse_up
    cmp edx, WM_MOUSEWHEEL
    je .mouse_wheel
    cmp edx, WM_DESTROY
    je .destroy

.default:
    mov rcx, [rbp-8]
    mov edx, [rbp-12]
    mov r8, [rbp-24]
    mov r9, [rbp-32]
    call DefWindowProcA
    jmp .return

.erase:
    mov eax, 1
    jmp .return

.destroy:
    call save_project
    xor ecx, ecx
    call PostQuitMessage
    xor eax, eax
    jmp .return

.keydown:
    mov eax, dword [rbp-24]

    cmp eax, VK_F5
    je .do_test
    cmp eax, VK_DELETE
    je .do_delete
    cmp eax, VK_LEFT
    je .nudge_left
    cmp eax, VK_RIGHT
    je .nudge_right
    cmp eax, VK_UP
    je .nudge_up
    cmp eax, VK_DOWN
    je .nudge_down
    cmp eax, VK_Q
    je .tool_select
    cmp eax, VK_P
    je .tool_platform
    cmp eax, VK_1
    je .tool_player
    cmp eax, VK_2
    je .tool_monster
    cmp eax, VK_3
    je .tool_npc
    cmp eax, VK_4
    je .tool_portal

    cmp eax, VK_S
    je .maybe_save
    cmp eax, VK_O
    je .maybe_load
    jmp .handled

.maybe_save:
    mov ecx, VK_CONTROL
    call GetKeyState
    test ax, 8000h
    jz .handled
    call save_project
    jmp .invalidate

.maybe_load:
    mov ecx, VK_CONTROL
    call GetKeyState
    test ax, 8000h
    jz .handled
    call load_project
    mov dword [selected_platform], -1
    jmp .invalidate

.do_test:
    call launch_runtime
    jmp .invalidate

.do_delete:
    call delete_selection
    jmp .invalidate

.nudge_left:
    mov ecx, -SNAP_SIZE
    xor edx, edx
    call nudge_selection
    jmp .invalidate
.nudge_right:
    mov ecx, SNAP_SIZE
    xor edx, edx
    call nudge_selection
    jmp .invalidate
.nudge_up:
    xor ecx, ecx
    mov edx, -SNAP_SIZE
    call nudge_selection
    jmp .invalidate
.nudge_down:
    xor ecx, ecx
    mov edx, SNAP_SIZE
    call nudge_selection
    jmp .invalidate

.tool_select:
    mov dword [tool_mode], TOOL_SELECT
    jmp .invalidate
.tool_platform:
    mov dword [tool_mode], TOOL_PLATFORM
    jmp .invalidate
.tool_player:
    mov dword [tool_mode], TOOL_PLAYER
    jmp .invalidate
.tool_monster:
    mov dword [tool_mode], TOOL_MONSTER
    jmp .invalidate
.tool_npc:
    mov dword [tool_mode], TOOL_NPC
    jmp .invalidate
.tool_portal:
    mov dword [tool_mode], TOOL_PORTAL
    jmp .invalidate

.mouse_down:
    mov rcx, [rbp-8]
    lea rdx, [client_rect]
    call GetClientRect

    mov r10, [rbp-32]
    mov eax, r10d
    and eax, 0FFFFh
    mov [rbp-80], eax                ; client x
    mov eax, r10d
    shr eax, 16
    and eax, 0FFFFh
    mov [rbp-84], eax                ; client y

    ; left tool panel is clickable
    cmp dword [rbp-80], LEFT_PANEL
    jge .check_top
    mov eax, [rbp-84]
    cmp eax, 190
    jl .handled
    cmp eax, 220
    jl .mouse_tool_select
    cmp eax, 248
    jl .mouse_tool_platform
    cmp eax, 276
    jl .mouse_tool_player
    cmp eax, 304
    jl .mouse_tool_monster
    cmp eax, 332
    jl .mouse_tool_npc
    cmp eax, 360
    jl .mouse_tool_portal
    jmp .handled

.mouse_tool_select:
    mov dword [tool_mode], TOOL_SELECT
    jmp .invalidate
.mouse_tool_platform:
    mov dword [tool_mode], TOOL_PLATFORM
    jmp .invalidate
.mouse_tool_player:
    mov dword [tool_mode], TOOL_PLAYER
    jmp .invalidate
.mouse_tool_monster:
    mov dword [tool_mode], TOOL_MONSTER
    jmp .invalidate
.mouse_tool_npc:
    mov dword [tool_mode], TOOL_NPC
    jmp .invalidate
.mouse_tool_portal:
    mov dword [tool_mode], TOOL_PORTAL
    jmp .invalidate

.check_top:
    cmp dword [rbp-84], TOP_BAR
    jge .check_canvas

    ; F5 test hot area at right edge of top bar
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL+150
    cmp dword [rbp-80], eax
    jl .handled
    call launch_runtime
    jmp .invalidate

.check_canvas:
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    cmp dword [rbp-80], eax
    jge .handled

    mov eax, [client_rect+12]
    sub eax, BOTTOM_BAR
    cmp dword [rbp-84], eax
    jge .handled

    ; convert client -> world
    mov ecx, [rbp-80]
    sub ecx, LEFT_PANEL
    add ecx, [editor_camera_x]
    mov edx, [rbp-84]
    sub edx, TOP_BAR

    mov eax, [tool_mode]
    cmp eax, TOOL_SELECT
    je .canvas_select
    cmp eax, TOOL_PLATFORM
    je .canvas_platform

    ; entity tools 2..5 map to entity types 1..4
    dec eax
    mov r8d, eax
    call place_entity
    mov dword [selected_platform], -1
    mov dword [selected_entity], -1
    jmp .invalidate

.canvas_select:
    ; Entities get selection priority over platforms.
    push rcx
    push rdx
    call find_entity_at
    pop rdx
    pop rcx
    cmp eax, -1
    je .select_platform_only
    mov [selected_entity], eax
    mov dword [selected_platform], -1
    jmp .invalidate

.select_platform_only:
    call find_platform_at
    mov [selected_platform], eax
    mov dword [selected_entity], -1
    jmp .invalidate

.canvas_platform:
    ; Snap world coordinates
    add ecx, SNAP_SIZE/2
    and ecx, -SNAP_SIZE
    add edx, SNAP_SIZE/2
    and edx, -SNAP_SIZE

    mov [drag_start_x], ecx
    mov [drag_cur_x], ecx
    mov [drag_start_y], edx
    mov [drag_cur_y], edx
    mov dword [dragging], 1
    mov dword [selected_platform], -1
    mov dword [selected_entity], -1

    mov rcx, [rbp-8]
    call SetCapture
    jmp .invalidate

.mouse_move:
    cmp dword [dragging], 0
    je .handled

    mov r10, [rbp-32]
    mov eax, r10d
    and eax, 0FFFFh
    sub eax, LEFT_PANEL
    add eax, SNAP_SIZE/2
    and eax, -SNAP_SIZE
    mov [drag_cur_x], eax

    mov eax, r10d
    shr eax, 16
    and eax, 0FFFFh
    sub eax, TOP_BAR
    add eax, SNAP_SIZE/2
    and eax, -SNAP_SIZE
    mov [drag_cur_y], eax
    jmp .invalidate

.mouse_up:
    cmp dword [dragging], 0
    je .handled
    mov dword [dragging], 0
    call ReleaseCapture

    mov eax, [platform_count]
    cmp eax, MAX_PLATFORMS
    jae .invalidate

    mov ecx, [drag_start_x]
    mov edx, [drag_cur_x]
    mov r8d, ecx
    sub r8d, edx
    jns .distance_ready
    neg r8d
.distance_ready:
    cmp r8d, SNAP_SIZE
    jl .invalidate

    cmp ecx, edx
    jle .ordered
    xchg ecx, edx
.ordered:
    mov eax, [platform_count]
    imul eax, PLATFORM_SIZE
    lea r10, [platforms]
    add r10, rax

    mov [r10+0], ecx
    mov eax, [drag_start_y]
    mov [r10+4], eax
    mov [r10+8], edx
    mov [r10+12], eax

    mov eax, [platform_count]
    mov [selected_platform], eax
    mov dword [selected_entity], -1
    inc dword [platform_count]
    jmp .invalidate

.mouse_wheel:
    ; High word of wParam is signed wheel delta.
    mov rax, [rbp-24]
    shr rax, 16
    movsx eax, ax
    test eax, eax
    jg .wheel_left

    add dword [editor_camera_x], 128
    jmp .invalidate

.wheel_left:
    sub dword [editor_camera_x], 128
    cmp dword [editor_camera_x], 0
    jge .invalidate
    mov dword [editor_camera_x], 0

.invalidate:
    mov rcx, [rbp-8]
    xor edx, edx
    xor r8d, r8d
    call InvalidateRect

.handled:
    xor eax, eax
    jmp .return

.paint:
    mov rcx, [rbp-8]
    lea rdx, [paint_buf]
    call BeginPaint
    mov [rbp-40], rax

    mov rcx, [rbp-8]
    lea rdx, [client_rect]
    call GetClientRect

    ; whole window
    mov ecx, [col_bg]
    call CreateSolidBrush
    mov [rbp-48], rax
    mov rcx, [rbp-40]
    lea rdx, [client_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-48]
    call DeleteObject

    ; left panel
    mov dword [temp_rect+0], 0
    mov dword [temp_rect+4], 0
    mov dword [temp_rect+8], LEFT_PANEL
    mov eax, [client_rect+12]
    mov [temp_rect+12], eax
    mov ecx, [col_panel]
    call CreateSolidBrush
    mov [rbp-48], rax
    mov rcx, [rbp-40]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-48]
    call DeleteObject

    ; right panel
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    mov [temp_rect+0], eax
    mov dword [temp_rect+4], 0
    mov eax, [client_rect+8]
    mov [temp_rect+8], eax
    mov eax, [client_rect+12]
    mov [temp_rect+12], eax
    mov ecx, [col_panel]
    call CreateSolidBrush
    mov [rbp-48], rax
    mov rcx, [rbp-40]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-48]
    call DeleteObject

    ; top bar
    mov dword [temp_rect+0], LEFT_PANEL
    mov dword [temp_rect+4], 0
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    mov [temp_rect+8], eax
    mov dword [temp_rect+12], TOP_BAR
    mov ecx, [col_top]
    call CreateSolidBrush
    mov [rbp-48], rax
    mov rcx, [rbp-40]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-48]
    call DeleteObject

    ; canvas
    mov dword [temp_rect+0], LEFT_PANEL
    mov dword [temp_rect+4], TOP_BAR
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    mov [temp_rect+8], eax
    mov eax, [client_rect+12]
    sub eax, BOTTOM_BAR
    mov [temp_rect+12], eax
    mov ecx, [col_canvas]
    call CreateSolidBrush
    mov [rbp-48], rax
    mov rcx, [rbp-40]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-48]
    call DeleteObject

    call draw_grid
    call draw_platforms
    call draw_entities
    call draw_drag_preview
    call draw_editor_text

    mov rcx, [rbp-8]
    lea rdx, [paint_buf]
    call EndPaint
    xor eax, eax

.return:
    mov r12, [rbp-64]
    mov r13, [rbp-72]
    leave
    ret


; ------------------------------------------------------------
; draw_grid: uses WndProc paint HDC at [rbp-40]
; ------------------------------------------------------------
draw_grid:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    mov [rbp-40], r12

    ; Parent's rbp is at [rbp]
    mov rax, [rbp]
    mov r12, [rax-40]

    mov ecx, PS_SOLID
    mov edx, 1
    mov r8d, [col_grid]
    call CreatePen
    mov [rbp-8], rax
    mov rcx, r12
    mov rdx, rax
    call SelectObject
    mov [rbp-16], rax

    mov r9d, LEFT_PANEL+GRID_SIZE
.vloop:
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    cmp r9d, eax
    jge .hstart

    mov rcx, r12
    mov edx, r9d
    mov r8d, TOP_BAR
    xor r9d, r9d
    ; preserve x around call
    mov eax, edx
    mov [rbp-20], eax
    call MoveToEx

    mov rcx, r12
    mov edx, [rbp-20]
    mov r8d, [client_rect+12]
    sub r8d, BOTTOM_BAR
    call LineTo

    mov r9d, [rbp-20]
    add r9d, GRID_SIZE
    jmp .vloop

.hstart:
    mov r9d, TOP_BAR+GRID_SIZE
.hloop:
    mov eax, [client_rect+12]
    sub eax, BOTTOM_BAR
    cmp r9d, eax
    jge .done

    mov rcx, r12
    mov edx, LEFT_PANEL
    mov r8d, r9d
    xor r9d, r9d
    mov eax, r8d
    mov [rbp-20], eax
    call MoveToEx

    mov rcx, r12
    mov edx, [client_rect+8]
    sub edx, RIGHT_PANEL
    mov r8d, [rbp-20]
    call LineTo

    mov r9d, [rbp-20]
    add r9d, GRID_SIZE
    jmp .hloop

.done:
    mov rcx, r12
    mov rdx, [rbp-16]
    call SelectObject
    mov rcx, [rbp-8]
    call DeleteObject

    mov r12, [rbp-40]
    leave
    ret


draw_platforms:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    mov [rbp-48], r12
    mov [rbp-56], r13

    mov rax, [rbp]
    mov r12, [rax-40]                ; HDC

    xor r13d, r13d
.loop:
    cmp r13d, [platform_count]
    jae .done

    mov ecx, PS_SOLID
    mov edx, 5
    mov r8d, [col_platform]
    cmp r13d, [selected_platform]
    jne .make_pen
    mov r8d, [col_selected]
.make_pen:
    call CreatePen
    mov [rbp-8], rax
    mov rcx, r12
    mov rdx, rax
    call SelectObject
    mov [rbp-16], rax

    mov eax, r13d
    imul eax, PLATFORM_SIZE
    lea r10, [platforms]
    add r10, rax

    mov edx, [r10+0]
    sub edx, [editor_camera_x]
    add edx, LEFT_PANEL
    mov r8d, [r10+4]
    add r8d, TOP_BAR
    mov rcx, r12
    xor r9d, r9d
    call MoveToEx

    mov eax, r13d
    imul eax, PLATFORM_SIZE
    lea r10, [platforms]
    add r10, rax
    mov edx, [r10+8]
    sub edx, [editor_camera_x]
    add edx, LEFT_PANEL
    mov r8d, [r10+12]
    add r8d, TOP_BAR
    mov rcx, r12
    call LineTo

    mov rcx, r12
    mov rdx, [rbp-16]
    call SelectObject
    mov rcx, [rbp-8]
    call DeleteObject

    inc r13d
    jmp .loop

.done:
    mov r12, [rbp-48]
    mov r13, [rbp-56]
    leave
    ret


draw_entities:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    mov [rbp-48], r12
    mov [rbp-56], r13

    mov rax, [rbp]
    mov r12, [rax-40]

    xor r13d, r13d
.loop:
    cmp r13d, [entity_count]
    jae .done

    mov eax, r13d
    imul eax, ENTITY_SIZE
    lea r10, [entities]
    add r10, rax

    mov eax, [r10+0]
    test eax, eax
    jz .next

    mov edx, [r10+4]
    sub edx, [editor_camera_x]
    add edx, LEFT_PANEL
    mov [temp_rect+0], edx
    add edx, 26
    mov [temp_rect+8], edx

    mov edx, [r10+8]
    add edx, TOP_BAR
    mov [temp_rect+4], edx
    add edx, 36
    mov [temp_rect+12], edx

    ; Draw a slightly larger selection marker behind a selected entity.
    cmp r13d, [selected_entity]
    jne .entity_color

    mov eax, [temp_rect+0]
    sub eax, 3
    mov [temp_rect+0], eax
    mov eax, [temp_rect+4]
    sub eax, 3
    mov [temp_rect+4], eax
    mov eax, [temp_rect+8]
    add eax, 3
    mov [temp_rect+8], eax
    mov eax, [temp_rect+12]
    add eax, 3
    mov [temp_rect+12], eax

    mov ecx, [col_selected]
    call CreateSolidBrush
    mov [rbp-8], rax
    mov rcx, r12
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-8]
    call DeleteObject

    mov eax, [temp_rect+0]
    add eax, 3
    mov [temp_rect+0], eax
    mov eax, [temp_rect+4]
    add eax, 3
    mov [temp_rect+4], eax
    mov eax, [temp_rect+8]
    sub eax, 3
    mov [temp_rect+8], eax
    mov eax, [temp_rect+12]
    sub eax, 3
    mov [temp_rect+12], eax

    mov eax, r13d
    imul eax, ENTITY_SIZE
    lea r10, [entities]
    add r10, rax
    mov eax, [r10+0]

.entity_color:
    cmp eax, ENTITY_PLAYER
    jne .monster
    mov ecx, [col_player]
    jmp .fill
.monster:
    cmp eax, ENTITY_MONSTER
    jne .npc
    mov ecx, [col_monster]
    jmp .fill
.npc:
    cmp eax, ENTITY_NPC
    jne .portal
    mov ecx, [col_npc]
    jmp .fill
.portal:
    mov ecx, [col_portal]

.fill:
    call CreateSolidBrush
    mov [rbp-8], rax
    mov rcx, r12
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-8]
    call DeleteObject

.next:
    inc r13d
    jmp .loop

.done:
    mov r12, [rbp-48]
    mov r13, [rbp-56]
    leave
    ret


draw_drag_preview:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    mov [rbp-40], r12

    cmp dword [dragging], 0
    je .done

    mov rax, [rbp]
    mov r12, [rax-40]

    mov ecx, PS_SOLID
    mov edx, 4
    mov r8d, [col_preview]
    call CreatePen
    mov [rbp-8], rax
    mov rcx, r12
    mov rdx, rax
    call SelectObject
    mov [rbp-16], rax

    mov rcx, r12
    mov edx, [drag_start_x]
    sub edx, [editor_camera_x]
    add edx, LEFT_PANEL
    mov r8d, [drag_start_y]
    add r8d, TOP_BAR
    xor r9d, r9d
    call MoveToEx

    mov rcx, r12
    mov edx, [drag_cur_x]
    sub edx, [editor_camera_x]
    add edx, LEFT_PANEL
    mov r8d, [drag_start_y]
    add r8d, TOP_BAR
    call LineTo

    mov rcx, r12
    mov rdx, [rbp-16]
    call SelectObject
    mov rcx, [rbp-8]
    call DeleteObject

.done:
    mov r12, [rbp-40]
    leave
    ret


draw_editor_text:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    mov [rbp-40], r12

    mov rax, [rbp]
    mov r12, [rax-40]

    mov rcx, r12
    mov edx, [col_text]
    call SetTextColor
    mov rcx, r12
    mov edx, TRANSPARENT
    call SetBkMode

    ; brand
    mov rcx, r12
    mov edx, 18
    mov r8d, 18
    lea r9, [txt_brand]
    mov qword [rsp+32], txt_brand_len
    call TextOutA

    mov rcx, r12
    mov edx, 18
    mov r8d, 82
    lea r9, [txt_maps]
    mov qword [rsp+32], txt_maps_len
    call TextOutA

    mov rcx, r12
    mov edx, 28
    mov r8d, 112
    lea r9, [txt_map1]
    mov qword [rsp+32], txt_map1_len
    call TextOutA

    mov rcx, r12
    mov edx, 18
    mov r8d, 160
    lea r9, [txt_tools]
    mov qword [rsp+32], txt_tools_len
    call TextOutA

    mov rcx, r12
    mov edx, 28
    mov r8d, 196
    lea r9, [txt_select]
    mov qword [rsp+32], txt_select_len
    call TextOutA

    mov rcx, r12
    mov edx, 28
    mov r8d, 224
    lea r9, [txt_platform]
    mov qword [rsp+32], txt_platform_len
    call TextOutA

    mov rcx, r12
    mov edx, 28
    mov r8d, 252
    lea r9, [txt_player]
    mov qword [rsp+32], txt_player_len
    call TextOutA

    mov rcx, r12
    mov edx, 28
    mov r8d, 280
    lea r9, [txt_monster]
    mov qword [rsp+32], txt_monster_len
    call TextOutA

    mov rcx, r12
    mov edx, 28
    mov r8d, 308
    lea r9, [txt_npc]
    mov qword [rsp+32], txt_npc_len
    call TextOutA

    mov rcx, r12
    mov edx, 28
    mov r8d, 336
    lea r9, [txt_portal]
    mov qword [rsp+32], txt_portal_len
    call TextOutA

    ; top controls
    mov rcx, r12
    mov edx, LEFT_PANEL+18
    mov r8d, 19
    lea r9, [txt_top]
    mov qword [rsp+32], txt_top_len
    call TextOutA

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL+132
    mov edx, eax
    mov rcx, r12
    mov r8d, 19
    lea r9, [txt_test]
    mov qword [rsp+32], txt_test_len
    call TextOutA

    ; properties
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 82
    lea r9, [txt_props]
    mov qword [rsp+32], txt_props_len
    call TextOutA

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 116
    cmp dword [selected_entity], -1
    jne .entity_selection
    cmp dword [selected_platform], -1
    je .none
    lea r9, [txt_platform_selected]
    mov qword [rsp+32], txt_platform_selected_len
    jmp .selection_text
.entity_selection:
    lea r9, [txt_entity_selected]
    mov qword [rsp+32], txt_entity_selected_len
    jmp .selection_text
.none:
    lea r9, [txt_none]
    mov qword [rsp+32], txt_none_len
.selection_text:
    call TextOutA

    ; active tool
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 154

    mov eax, [tool_mode]
    cmp eax, TOOL_SELECT
    je .a_select
    cmp eax, TOOL_PLATFORM
    je .a_platform
    cmp eax, TOOL_PLAYER
    je .a_player
    cmp eax, TOOL_MONSTER
    je .a_monster
    cmp eax, TOOL_NPC
    je .a_npc
    lea r9, [txt_active_portal]
    mov qword [rsp+32], txt_active_portal_len
    jmp .a_draw
.a_select:
    lea r9, [txt_active_select]
    mov qword [rsp+32], txt_active_select_len
    jmp .a_draw
.a_platform:
    lea r9, [txt_active_platform]
    mov qword [rsp+32], txt_active_platform_len
    jmp .a_draw
.a_player:
    lea r9, [txt_active_player]
    mov qword [rsp+32], txt_active_player_len
    jmp .a_draw
.a_monster:
    lea r9, [txt_active_monster]
    mov qword [rsp+32], txt_active_monster_len
    jmp .a_draw
.a_npc:
    lea r9, [txt_active_npc]
    mov qword [rsp+32], txt_active_npc_len
.a_draw:
    call TextOutA

    ; bottom status
    mov rcx, r12
    mov edx, LEFT_PANEL+12
    mov eax, [client_rect+12]
    sub eax, 20
    mov r8d, eax
    lea r9, [txt_status]
    mov qword [rsp+32], txt_status_len
    call TextOutA

    mov r12, [rbp-40]
    leave
    ret


; ------------------------------------------------------------
; find_platform_at(ecx=x, edx=y) -> eax=index/-1
; tolerance 8 px around the horizontal segment.
; ------------------------------------------------------------
find_platform_at:
    mov r8d, ecx
    mov r9d, edx
    xor r10d, r10d
.loop:
    cmp r10d, [platform_count]
    jae .not_found

    mov eax, r10d
    imul eax, PLATFORM_SIZE
    lea r11, [platforms]
    add r11, rax

    cmp r8d, [r11+0]
    jl .next
    cmp r8d, [r11+8]
    jg .next

    mov eax, r9d
    sub eax, [r11+4]
    jns .abs_ok
    neg eax
.abs_ok:
    cmp eax, 8
    jle .found
.next:
    inc r10d
    jmp .loop

.found:
    mov eax, r10d
    ret
.not_found:
    mov eax, -1
    ret


; ------------------------------------------------------------
; find_entity_at(ecx=x, edx=y) -> eax=index/-1
; ------------------------------------------------------------
find_entity_at:
    mov r8d, ecx
    mov r9d, edx
    xor r10d, r10d
.loop:
    cmp r10d, [entity_count]
    jae .not_found

    mov eax, r10d
    imul eax, ENTITY_SIZE
    lea r11, [entities]
    add r11, rax

    cmp dword [r11+0], ENTITY_NONE
    je .next

    mov eax, [r11+4]
    cmp r8d, eax
    jl .next
    add eax, 26
    cmp r8d, eax
    jg .next

    mov eax, [r11+8]
    cmp r9d, eax
    jl .next
    add eax, 36
    cmp r9d, eax
    jg .next

    mov eax, r10d
    ret
.next:
    inc r10d
    jmp .loop
.not_found:
    mov eax, -1
    ret


; ------------------------------------------------------------
; delete_selection
; ------------------------------------------------------------
delete_selection:
    push rbp
    mov rbp, rsp
    sub rsp, 48

    cmp dword [selected_entity], -1
    je .platform

    mov eax, [selected_entity]
    cmp eax, 0
    jl .clear
    cmp eax, [entity_count]
    jge .clear

    mov r8d, eax
.entity_shift:
    mov r9d, [entity_count]
    dec r9d
    cmp r8d, r9d
    jge .entity_decrement

    mov eax, r8d
    inc eax
    imul eax, ENTITY_SIZE
    lea r10, [entities]
    add r10, rax

    mov eax, r8d
    imul eax, ENTITY_SIZE
    lea r11, [entities]
    add r11, rax

    mov eax, [r10+0]
    mov [r11+0], eax
    mov eax, [r10+4]
    mov [r11+4], eax
    mov eax, [r10+8]
    mov [r11+8], eax
    mov eax, [r10+12]
    mov [r11+12], eax
    inc r8d
    jmp .entity_shift

.entity_decrement:
    dec dword [entity_count]
    jmp .clear

.platform:
    call delete_selected_platform

.clear:
    mov dword [selected_entity], -1
    mov dword [selected_platform], -1
    leave
    ret


; ------------------------------------------------------------
; nudge_selection(ecx=dx, edx=dy)
; ------------------------------------------------------------
nudge_selection:
    cmp dword [selected_entity], -1
    je .platform

    mov eax, [selected_entity]
    cmp eax, 0
    jl .done
    cmp eax, [entity_count]
    jge .done
    imul eax, ENTITY_SIZE
    lea r8, [entities]
    add r8, rax
    add [r8+4], ecx
    add [r8+8], edx
    cmp dword [r8+4], 0
    jge .done
    mov dword [r8+4], 0
    ret

.platform:
    mov eax, [selected_platform]
    cmp eax, 0
    jl .done
    cmp eax, [platform_count]
    jge .done
    imul eax, PLATFORM_SIZE
    lea r8, [platforms]
    add r8, rax
    add [r8+0], ecx
    add [r8+8], ecx
    add [r8+4], edx
    add [r8+12], edx
    cmp dword [r8+0], 0
    jge .done
    ; keep segment width while clamping to world x=0
    mov eax, [r8+8]
    sub eax, [r8+0]
    mov dword [r8+0], 0
    mov [r8+8], eax
.done:
    ret


; ------------------------------------------------------------
; delete_selected_platform
; ------------------------------------------------------------
delete_selected_platform:
    push rbp
    mov rbp, rsp
    sub rsp, 48

    mov eax, [selected_platform]
    cmp eax, 0
    jl .done
    cmp eax, [platform_count]
    jge .done

    mov r8d, eax
.shift:
    mov r9d, [platform_count]
    dec r9d
    cmp r8d, r9d
    jge .decrement

    mov eax, r8d
    inc eax
    imul eax, PLATFORM_SIZE
    lea r10, [platforms]
    add r10, rax

    mov eax, r8d
    imul eax, PLATFORM_SIZE
    lea r11, [platforms]
    add r11, rax

    mov eax, [r10+0]
    mov [r11+0], eax
    mov eax, [r10+4]
    mov [r11+4], eax
    mov eax, [r10+8]
    mov [r11+8], eax
    mov eax, [r10+12]
    mov [r11+12], eax

    inc r8d
    jmp .shift

.decrement:
    dec dword [platform_count]
    mov dword [selected_platform], -1
.done:
    leave
    ret


; ------------------------------------------------------------
; place_entity(ecx=x, edx=y, r8d=type)
; Player start is unique; placing it moves the existing marker.
; ------------------------------------------------------------
place_entity:
    push rbp
    mov rbp, rsp
    sub rsp, 48

    mov [rbp-4], ecx
    mov [rbp-8], edx
    mov [rbp-12], r8d

    cmp r8d, ENTITY_PLAYER
    jne .append

    xor r9d, r9d
.find_player:
    cmp r9d, [entity_count]
    jae .append
    mov eax, r9d
    imul eax, ENTITY_SIZE
    lea r10, [entities]
    add r10, rax
    cmp dword [r10+0], ENTITY_PLAYER
    je .move_player
    inc r9d
    jmp .find_player

.move_player:
    mov eax, [rbp-4]
    mov [r10+4], eax
    mov eax, [rbp-8]
    mov [r10+8], eax
    leave
    ret

.append:
    mov eax, [entity_count]
    cmp eax, MAX_ENTITIES
    jae .done

    imul eax, ENTITY_SIZE
    lea r10, [entities]
    add r10, rax

    mov eax, [rbp-12]
    mov [r10+0], eax
    mov eax, [rbp-4]
    mov [r10+4], eax
    mov eax, [rbp-8]
    mov [r10+8], eax
    mov dword [r10+12], 0
    inc dword [entity_count]
.done:
    leave
    ret


; ------------------------------------------------------------
; init_default_project
; ------------------------------------------------------------
init_default_project:
    mov dword [platform_count], 2
    mov dword [entity_count], 2
    mov dword [selected_platform], -1
    mov dword [selected_entity], -1
    mov dword [editor_camera_x], 0

    mov dword [platforms+0], 32
    mov dword [platforms+4], 500
    mov dword [platforms+8], 900
    mov dword [platforms+12], 500

    mov dword [platforms+16], 340
    mov dword [platforms+20], 380
    mov dword [platforms+24], 620
    mov dword [platforms+28], 380

    mov dword [entities+0], ENTITY_PLAYER
    mov dword [entities+4], 100
    mov dword [entities+8], 430
    mov dword [entities+12], 0

    mov dword [entities+16], ENTITY_MONSTER
    mov dword [entities+20], 520
    mov dword [entities+24], 444
    mov dword [entities+28], 0
    ret


; ------------------------------------------------------------
; save_project -> eax=1/0
; ------------------------------------------------------------
save_project:
    push rbp
    mov rbp, rsp
    sub rsp, 80

    mov eax, [platform_count]
    mov [project_header+12], eax
    mov eax, [entity_count]
    mov [project_header+16], eax

    lea rcx, [project_path]
    mov edx, GENERIC_WRITE
    xor r8d, r8d
    xor r9d, r9d
    mov qword [rsp+32], CREATE_ALWAYS
    mov qword [rsp+40], FILE_ATTRIBUTE_NORMAL
    mov qword [rsp+48], 0
    call CreateFileA

    cmp rax, INVALID_HANDLE_VALUE
    je .fail
    mov [rbp-8], rax

    mov rcx, rax
    lea rdx, [project_header]
    mov r8d, PROJECT_HEADER_SIZE
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call WriteFile
    test eax, eax
    jz .close_fail

    mov eax, [platform_count]
    imul eax, PLATFORM_SIZE
    test eax, eax
    jz .entities

    mov rcx, [rbp-8]
    lea rdx, [platforms]
    mov r8d, eax
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call WriteFile
    test eax, eax
    jz .close_fail

.entities:
    mov eax, [entity_count]
    imul eax, ENTITY_SIZE
    test eax, eax
    jz .success

    mov rcx, [rbp-8]
    lea rdx, [entities]
    mov r8d, eax
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call WriteFile
    test eax, eax
    jz .close_fail

.success:
    mov rcx, [rbp-8]
    call CloseHandle
    mov eax, 1
    leave
    ret

.close_fail:
    mov rcx, [rbp-8]
    call CloseHandle
.fail:
    xor eax, eax
    leave
    ret


; ------------------------------------------------------------
; load_project -> eax=1/0
; ------------------------------------------------------------
load_project:
    push rbp
    mov rbp, rsp
    sub rsp, 80

    lea rcx, [project_path]
    mov edx, GENERIC_READ
    mov r8d, FILE_SHARE_READ
    xor r9d, r9d
    mov qword [rsp+32], OPEN_EXISTING
    mov qword [rsp+40], FILE_ATTRIBUTE_NORMAL
    mov qword [rsp+48], 0
    call CreateFileA

    cmp rax, INVALID_HANDLE_VALUE
    je .fail
    mov [rbp-8], rax

    mov rcx, rax
    lea rdx, [project_header]
    mov r8d, PROJECT_HEADER_SIZE
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call ReadFile
    test eax, eax
    jz .close_fail

    mov rax, PROJECT_MAGIC_QWORD
    cmp qword [project_header], rax
    jne .close_fail
    cmp dword [project_header+8], PROJECT_VERSION
    jne .close_fail

    mov eax, [project_header+12]
    cmp eax, MAX_PLATFORMS
    ja .close_fail
    mov [platform_count], eax

    mov eax, [project_header+16]
    cmp eax, MAX_ENTITIES
    ja .close_fail
    mov [entity_count], eax

    mov eax, [platform_count]
    imul eax, PLATFORM_SIZE
    test eax, eax
    jz .read_entities

    mov rcx, [rbp-8]
    lea rdx, [platforms]
    mov r8d, eax
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call ReadFile
    test eax, eax
    jz .close_fail

.read_entities:
    mov eax, [entity_count]
    imul eax, ENTITY_SIZE
    test eax, eax
    jz .success

    mov rcx, [rbp-8]
    lea rdx, [entities]
    mov r8d, eax
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call ReadFile
    test eax, eax
    jz .close_fail

.success:
    mov rcx, [rbp-8]
    call CloseHandle
    mov dword [selected_platform], -1
    mov dword [selected_entity], -1
    mov eax, 1
    leave
    ret

.close_fail:
    mov rcx, [rbp-8]
    call CloseHandle
.fail:
    xor eax, eax
    leave
    ret


; ------------------------------------------------------------
; launch_runtime
; save, then CreateProcessA("LeellaRuntime.exe")
; ------------------------------------------------------------
launch_runtime:
    push rbp
    mov rbp, rsp
    sub rsp, 112

    call save_project
    test eax, eax
    jz .done

    ; writable command line copy
    lea rcx, [runtime_cmd]
    lea rdx, [runtime_cmd_template]
    call lstrcpyA

    ; zero STARTUPINFOA + PROCESS_INFORMATION
    lea rdi, [startup_info]
    xor eax, eax
    mov ecx, 13
    rep stosq
    mov dword [startup_info], 104

    lea rdi, [process_info]
    xor eax, eax
    mov ecx, 3
    rep stosq

    xor ecx, ecx
    lea rdx, [runtime_cmd]
    xor r8d, r8d
    xor r9d, r9d
    mov qword [rsp+32], 0
    mov qword [rsp+40], 0
    mov qword [rsp+48], 0
    mov qword [rsp+56], 0
    lea rax, [startup_info]
    mov [rsp+64], rax
    lea rax, [process_info]
    mov [rsp+72], rax
    call CreateProcessA
    test eax, eax
    jz .done

    mov rcx, [process_info+0]
    call CloseHandle
    mov rcx, [process_info+8]
    call CloseHandle

.done:
    leave
    ret
