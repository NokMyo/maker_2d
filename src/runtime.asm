; Leella Prelude Runtime
; 100% x86-64 assembly / NASM / Win32 + GDI
;
; Prototype runtime features:
; - loads project.lprj
; - A/D or arrows move
; - Space jumps
; - X attacks
; - gravity and one-way platform landing
; - horizontal camera follow
; - simple monster hit/death state
; - NPC/portal/monster placeholder rendering

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
extern SetTimer
extern KillTimer
extern GetAsyncKeyState
extern CreateFileA
extern ReadFile
extern CloseHandle
extern MessageBoxA

%define CS_HREDRAW          0x0002
%define CS_VREDRAW          0x0001
%define WS_OVERLAPPEDWINDOW 0x00CF0000
%define WS_VISIBLE          0x10000000
%define CW_USEDEFAULT       0x80000000
%define SW_SHOW             5

%define WM_DESTROY          0x0002
%define WM_PAINT            0x000F
%define WM_ERASEBKGND       0x0014
%define WM_TIMER            0x0113

%define PS_SOLID            0
%define TRANSPARENT         1
%define IDC_ARROW           32512

%define GENERIC_READ        0x80000000
%define FILE_SHARE_READ     0x00000001
%define OPEN_EXISTING       3
%define FILE_ATTRIBUTE_NORMAL 0x80
%define INVALID_HANDLE_VALUE -1

%define VK_LEFT             0x25
%define VK_RIGHT            0x27
%define VK_SPACE            0x20
%define VK_A                0x41
%define VK_D                0x44
%define VK_X                0x58
%define VK_ESCAPE           0x1B

%define PLAYER_W            30
%define PLAYER_H            46
%define MOVE_SPEED          5
%define JUMP_SPEED          -14
%define MAX_FALL_SPEED      14

section .data
    class_name       db "LeellaPreludeRuntime",0
    window_title     db "Leella Prelude - Test Game",0
    project_path     db "project.lprj",0

    load_error_title db "Leella Prelude Runtime",0
    load_error_text  db "project.lprj could not be loaded.",0

    txt_help         db "A/D or arrows: move   Space: jump   X: attack   Esc: quit",0
    txt_help_len     equ $-txt_help-1
    txt_runtime      db "LEELLA PRELUDE TEST RUNTIME",0
    txt_runtime_len  equ $-txt_runtime-1

    project_header:
        db "LPRJ0001"
        dd PROJECT_VERSION
        dd 0
        dd 0
        dd 0

    col_bg           dd 0x00191512
    col_platform     dd 0x00D6B664
    col_player       dd 0x00E8D9B5
    col_monster      dd 0x006B6BE0
    col_npc          dd 0x0076C48A
    col_portal       dd 0x00D67FD4
    col_attack       dd 0x006FB5F2
    col_text         dd 0x00F2EEE8

section .bss
    hinstance        resq 1
    hwnd_main        resq 1
    msg_buf          resb 64
    wc_buf           resb 80
    paint_buf        resb 80
    client_rect      resd 4
    temp_rect        resd 4
    io_bytes         resd 1

    platform_count   resd 1
    entity_count     resd 1
    platforms        resd MAX_PLATFORMS*4
    entities         resd MAX_ENTITIES*4
    enemy_hp         resd MAX_ENTITIES

    player_x         resd 1
    player_y         resd 1
    spawn_x          resd 1
    spawn_y          resd 1
    player_vx        resd 1
    player_vy        resd 1
    player_grounded  resd 1
    facing_right     resd 1
    camera_x         resd 1

    attack_ticks     resd 1
    attack_cooldown  resd 1
    attack_hit_done  resd 1

section .text

mainCRTStartup:
    and rsp, -16
    sub rsp, 192

    call load_project
    test eax, eax
    jnz .project_ok

    xor ecx, ecx
    lea rdx, [load_error_text]
    lea r8, [load_error_title]
    xor r9d, r9d
    call MessageBoxA

.project_ok:
    call initialize_player

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
    mov qword [rsp+48], 1100
    mov qword [rsp+56], 720
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

    mov rcx, [hwnd_main]
    mov edx, 1
    mov r8d, 16
    xor r9d, r9d
    call SetTimer

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
    sub rsp, 224
    mov [rbp-64], r12
    mov [rbp-72], r13

    mov [rbp-8], rcx
    mov [rbp-16], r8
    mov [rbp-24], r9

    cmp edx, WM_PAINT
    je .paint
    cmp edx, WM_ERASEBKGND
    je .erase
    cmp edx, WM_TIMER
    je .timer
    cmp edx, WM_DESTROY
    je .destroy

.default:
    mov rcx, [rbp-8]
    mov r8, [rbp-16]
    mov r9, [rbp-24]
    call DefWindowProcA
    jmp .return

.erase:
    mov eax, 1
    jmp .return

.timer:
    call update_game
    mov rcx, [rbp-8]
    xor edx, edx
    xor r8d, r8d
    call InvalidateRect
    xor eax, eax
    jmp .return

.destroy:
    mov rcx, [rbp-8]
    mov edx, 1
    call KillTimer
    xor ecx, ecx
    call PostQuitMessage
    xor eax, eax
    jmp .return

.paint:
    mov rcx, [rbp-8]
    lea rdx, [paint_buf]
    call BeginPaint
    mov [rbp-32], rax

    mov rcx, [rbp-8]
    lea rdx, [client_rect]
    call GetClientRect

    mov ecx, [col_bg]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [client_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

    ; platforms
    mov ecx, PS_SOLID
    mov edx, 5
    mov r8d, [col_platform]
    call CreatePen
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    mov rdx, rax
    call SelectObject
    mov [rbp-48], rax

    xor r12d, r12d
.platform_loop:
    cmp r12d, [platform_count]
    jae .platform_done

    mov eax, r12d
    imul eax, PLATFORM_SIZE
    lea r13, [platforms]
    add r13, rax

    mov edx, [r13+0]
    sub edx, [camera_x]
    mov r8d, [r13+4]
    mov rcx, [rbp-32]
    xor r9d, r9d
    call MoveToEx

    mov edx, [r13+8]
    sub edx, [camera_x]
    mov r8d, [r13+12]
    mov rcx, [rbp-32]
    call LineTo

    inc r12d
    jmp .platform_loop

.platform_done:
    mov rcx, [rbp-32]
    mov rdx, [rbp-48]
    call SelectObject
    mov rcx, [rbp-40]
    call DeleteObject

    ; entities
    xor r12d, r12d
.entity_loop:
    cmp r12d, [entity_count]
    jae .player_draw

    mov eax, r12d
    imul eax, ENTITY_SIZE
    lea r13, [entities]
    add r13, rax

    mov eax, [r13+0]
    test eax, eax
    jz .entity_next
    cmp eax, ENTITY_PLAYER
    je .entity_next

    mov edx, [r13+4]
    sub edx, [camera_x]
    mov [temp_rect+0], edx
    add edx, 28
    mov [temp_rect+8], edx

    mov edx, [r13+8]
    mov [temp_rect+4], edx
    add edx, 36
    mov [temp_rect+12], edx

    cmp eax, ENTITY_MONSTER
    jne .check_npc
    mov ecx, [col_monster]
    jmp .fill_entity
.check_npc:
    cmp eax, ENTITY_NPC
    jne .check_portal
    mov ecx, [col_npc]
    jmp .fill_entity
.check_portal:
    mov ecx, [col_portal]

.fill_entity:
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

.entity_next:
    inc r12d
    jmp .entity_loop

.player_draw:
    mov eax, [player_x]
    sub eax, [camera_x]
    mov [temp_rect+0], eax
    add eax, PLAYER_W
    mov [temp_rect+8], eax

    mov eax, [player_y]
    mov [temp_rect+4], eax
    add eax, PLAYER_H
    mov [temp_rect+12], eax

    mov ecx, [col_player]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

    ; attack hitbox preview
    cmp dword [attack_ticks], 0
    je .hud

    mov eax, [player_x]
    sub eax, [camera_x]
    cmp dword [facing_right], 0
    je .attack_left

    add eax, PLAYER_W
    mov [temp_rect+0], eax
    add eax, 64
    mov [temp_rect+8], eax
    jmp .attack_y

.attack_left:
    sub eax, 64
    mov [temp_rect+0], eax
    add eax, 64
    mov [temp_rect+8], eax

.attack_y:
    mov eax, [player_y]
    add eax, 6
    mov [temp_rect+4], eax
    add eax, 34
    mov [temp_rect+12], eax

    mov ecx, [col_attack]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

.hud:
    mov rcx, [rbp-32]
    mov edx, [col_text]
    call SetTextColor
    mov rcx, [rbp-32]
    mov edx, TRANSPARENT
    call SetBkMode

    mov rcx, [rbp-32]
    mov edx, 16
    mov r8d, 14
    lea r9, [txt_runtime]
    mov qword [rsp+32], txt_runtime_len
    call TextOutA

    mov rcx, [rbp-32]
    mov edx, 16
    mov r8d, 38
    lea r9, [txt_help]
    mov qword [rsp+32], txt_help_len
    call TextOutA

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
; update_game
; ------------------------------------------------------------
update_game:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    mov [rbp-40], r12
    mov [rbp-48], r13

    ; Escape closes the window by posting quit directly.
    mov ecx, VK_ESCAPE
    call GetAsyncKeyState
    test ax, 8000h
    jz .movement
    xor ecx, ecx
    call PostQuitMessage
    jmp .done

.movement:
    mov dword [player_vx], 0

    mov ecx, VK_LEFT
    call GetAsyncKeyState
    test ax, 8000h
    jnz .go_left
    mov ecx, VK_A
    call GetAsyncKeyState
    test ax, 8000h
    jnz .go_left
    jmp .check_right

.go_left:
    mov dword [player_vx], -MOVE_SPEED
    mov dword [facing_right], 0

.check_right:
    mov ecx, VK_RIGHT
    call GetAsyncKeyState
    test ax, 8000h
    jnz .go_right
    mov ecx, VK_D
    call GetAsyncKeyState
    test ax, 8000h
    jz .jump

.go_right:
    mov dword [player_vx], MOVE_SPEED
    mov dword [facing_right], 1

.jump:
    cmp dword [player_grounded], 0
    je .attack_input
    mov ecx, VK_SPACE
    call GetAsyncKeyState
    test ax, 8000h
    jz .attack_input
    mov dword [player_vy], JUMP_SPEED
    mov dword [player_grounded], 0

.attack_input:
    cmp dword [attack_cooldown], 0
    jle .can_attack
    dec dword [attack_cooldown]

.can_attack:
    mov ecx, VK_X
    call GetAsyncKeyState
    test ax, 8000h
    jz .physics
    cmp dword [attack_cooldown], 0
    jne .physics

    mov dword [attack_ticks], 8
    mov dword [attack_cooldown], 18
    mov dword [attack_hit_done], 0

.physics:
    mov eax, [player_vx]
    add [player_x], eax
    cmp dword [player_x], 0
    jge .gravity
    mov dword [player_x], 0

.gravity:
    ; previous bottom
    mov eax, [player_y]
    add eax, PLAYER_H
    mov [rbp-8], eax

    mov eax, [player_vy]
    inc eax
    cmp eax, MAX_FALL_SPEED
    jle .vy_ok
    mov eax, MAX_FALL_SPEED
.vy_ok:
    mov [player_vy], eax
    add [player_y], eax
    mov dword [player_grounded], 0

    cmp dword [player_vy], 0
    jl .after_collision

    mov eax, [player_y]
    add eax, PLAYER_H
    mov [rbp-12], eax                  ; new bottom

    mov eax, [player_x]
    add eax, PLAYER_W/2
    mov [rbp-16], eax                  ; center x

    xor r12d, r12d
.collision_loop:
    cmp r12d, [platform_count]
    jae .after_collision

    mov eax, r12d
    imul eax, PLATFORM_SIZE
    lea r13, [platforms]
    add r13, rax

    mov eax, [rbp-16]
    cmp eax, [r13+0]
    jl .collision_next
    cmp eax, [r13+8]
    jg .collision_next

    mov eax, [rbp-8]
    cmp eax, [r13+4]
    jg .collision_next
    mov eax, [rbp-12]
    cmp eax, [r13+4]
    jl .collision_next

    mov eax, [r13+4]
    sub eax, PLAYER_H
    mov [player_y], eax
    mov dword [player_vy], 0
    mov dword [player_grounded], 1
    jmp .after_collision

.collision_next:
    inc r12d
    jmp .collision_loop

.after_collision:
    ; fall reset
    mov eax, [player_y]
    cmp eax, 1100
    jl .attack_update
    mov eax, [spawn_x]
    mov [player_x], eax
    mov eax, [spawn_y]
    mov [player_y], eax
    mov dword [player_vy], 0

.attack_update:
    cmp dword [attack_ticks], 0
    jle .camera
    dec dword [attack_ticks]
    cmp dword [attack_hit_done], 0
    jne .camera
    call attack_monsters
    mov dword [attack_hit_done], 1

.camera:
    mov eax, [player_x]
    sub eax, 420
    jge .camera_nonnegative
    xor eax, eax
.camera_nonnegative:
    mov [camera_x], eax

.done:
    mov r12, [rbp-40]
    mov r13, [rbp-48]
    leave
    ret


; ------------------------------------------------------------
; attack_monsters
; Very small prototype combat check.
; ------------------------------------------------------------
attack_monsters:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov [rbp-40], r12
    mov [rbp-48], r13

    xor r12d, r12d
.loop:
    cmp r12d, [entity_count]
    jae .done

    mov eax, r12d
    imul eax, ENTITY_SIZE
    lea r13, [entities]
    add r13, rax

    cmp dword [r13+0], ENTITY_MONSTER
    jne .next

    mov eax, [r13+4]
    sub eax, [player_x]

    cmp dword [facing_right], 0
    je .left_facing

    cmp eax, 0
    jl .next
    cmp eax, 92
    jg .next
    jmp .vertical

.left_facing:
    cmp eax, 0
    jg .next
    cmp eax, -92
    jl .next

.vertical:
    mov eax, [r13+8]
    sub eax, [player_y]
    cmp eax, -50
    jl .next
    cmp eax, 70
    jg .next

    lea rax, [enemy_hp]
    mov ecx, r12d
    shl rcx, 2
    add rax, rcx
    dec dword [rax]
    cmp dword [rax], 0
    jg .next

    ; Dead monster is removed only in runtime memory.
    mov dword [r13+0], ENTITY_NONE

.next:
    inc r12d
    jmp .loop

.done:
    mov r12, [rbp-40]
    mov r13, [rbp-48]
    leave
    ret


; ------------------------------------------------------------
; initialize_player
; ------------------------------------------------------------
initialize_player:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov [rbp-40], r12
    mov [rbp-48], r13

    mov dword [spawn_x], 64
    mov dword [spawn_y], 64
    mov dword [facing_right], 1

    xor r12d, r12d
.scan:
    cmp r12d, [entity_count]
    jae .set_player

    mov eax, r12d
    imul eax, ENTITY_SIZE
    lea r13, [entities]
    add r13, rax

    cmp dword [r13+0], ENTITY_PLAYER
    jne .maybe_monster
    mov eax, [r13+4]
    mov [spawn_x], eax
    mov eax, [r13+8]
    mov [spawn_y], eax
    jmp .next

.maybe_monster:
    cmp dword [r13+0], ENTITY_MONSTER
    jne .next
    lea rax, [enemy_hp]
    mov ecx, r12d
    shl rcx, 2
    add rax, rcx
    mov dword [rax], 3

.next:
    inc r12d
    jmp .scan

.set_player:
    mov eax, [spawn_x]
    mov [player_x], eax
    mov eax, [spawn_y]
    mov [player_y], eax

    mov r12, [rbp-40]
    mov r13, [rbp-48]
    leave
    ret


; ------------------------------------------------------------
; load_project -> eax = 1 success, 0 fail
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
    mov [rbp-16], eax
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
