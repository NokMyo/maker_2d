; Tsuramechoki Runtime
; 100% x86-64 assembly / NASM / Win32 + GDI
;
; Prototype runtime features:
; - loads project.tsrp
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
extern LoadImageA
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
extern CreateCompatibleDC
extern DeleteDC
extern GetObjectA
extern BitBlt
extern StretchBlt
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
extern WriteFile
extern CloseHandle
extern MessageBoxA
extern wsprintfA
extern lstrlenA
extern MultiByteToWideChar
extern GdiplusStartup
extern GdiplusShutdown
extern GdipLoadImageFromFile
extern GdipDisposeImage
extern GdipGetImageWidth
extern GdipGetImageHeight
extern GdipCreateFromHDC
extern GdipDeleteGraphics
extern GdipDrawImageRectRectI

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
%define GENERIC_WRITE       0x40000000
%define FILE_SHARE_READ     0x00000001
%define CREATE_ALWAYS       2
%define OPEN_EXISTING       3
%define FILE_ATTRIBUTE_NORMAL 0x80
%define INVALID_HANDLE_VALUE -1

%define VK_LEFT             0x25
%define VK_RIGHT            0x27
%define VK_DOWN             0x28
%define VK_SPACE            0x20
%define VK_A                0x41
%define VK_D                0x44
%define VK_X                0x58
%define VK_E                0x45
%define VK_ESCAPE           0x1B

%define PLAYER_W            30
%define PLAYER_H            46
%define MOVE_SPEED          5
%define JUMP_SPEED          -14
%define MAX_FALL_SPEED      14

section .data
    class_name       db "TsuramechokiRuntime",0
    window_title     db "Tsuramechoki - Test Game",0
    project_path     db "project.tsrp",0

    load_error_title db "Tsuramechoki Runtime",0
    load_error_text  db "project.tsrp could not be loaded.",0

    txt_help         db "A/D or arrows: move   Space: jump   X: attack   E: interact   Esc: quit",0
    txt_help_len     equ $-txt_help-1
    txt_runtime      db "TSURAMECHOKI TEST RUNTIME",0
    txt_runtime_len  equ $-txt_runtime-1
    txt_dialog       db "NPC: This is a live test map made in Tsuramechoki.",0
    txt_dialog_len   equ $-txt_dialog-1

    project_header:
        db "TSRP0003"
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
    col_ladder       dd 0x0076A6C4
    col_checkpoint   dd 0x006FD6C0
    col_decor        dd 0x00847A72
    col_event        dd 0x00E09762
    col_attack       dd 0x006FB5F2
    col_text         dd 0x00F2EEE8
    col_hp           dd 0x006DCC75
    col_hp_bg        dd 0x003A3531
    col_dialog       dd 0x00282320

    gdip_startup_input:
        dd 1
        dd 0
        dq 0
        dd 0
        dd 0

section .bss
    hinstance        resq 1
    hwnd_main        resq 1
    msg_buf          resb 64
    wc_buf           resb 80
    paint_buf        resb 80
    client_rect      resd 4
    temp_rect        resd 4
    io_bytes         resd 1
    gdip_token       resq 1
    gdip_wide_path   resw 260
    gdip_image       resq 1
    gdip_graphics    resq 1
    gdip_width       resd 1
    gdip_height      resd 1

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
    drop_through_ticks resd 1
    facing_right     resd 1
    camera_x         resd 1

    attack_ticks     resd 1
    attack_cooldown  resd 1
    attack_hit_done  resd 1
    attack_total_ticks resd 1
    attack_frame_ticks resd 1
    attack_anim_id   resd 1
    active_skill_id  resd 1
    attack_hits_done resd 1
    attack_last_frame resd 1

    player_hp        resd 1
    invuln_ticks     resd 1
    interact_cooldown resd 1
    dialog_ticks     resd 1

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
    lea rcx, [gdip_token]
    lea rdx, [gdip_startup_input]
    xor r8d, r8d
    call GdiplusStartup

    call runtime_init_state
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
    lea r8, [project_meta]
    mov r9d, WS_OVERLAPPEDWINDOW | WS_VISIBLE
    mov qword [rsp+32], CW_USEDEFAULT
    mov qword [rsp+40], CW_USEDEFAULT
    mov eax, [screen_width]
    mov [rsp+48], rax
    mov eax, [screen_height]
    mov [rsp+56], rax
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
    mov rcx, [gdip_token]
    test rcx, rcx
    jz .quit_exit
    call GdiplusShutdown
.quit_exit:
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

    mov eax, [active_map]
    imul eax, MAP_SIZE
    lea r10, [maps+rax]
    mov ecx, [r10+32]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [client_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

    mov rcx, [rbp-32]
    call runtime_draw_background

    mov rcx, [rbp-32]
    call runtime_draw_tiles

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

    mov eax, [active_map]
    cmp dword [platform_map_ids+r12*4], eax
    jne .platform_next

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

.platform_next:
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

    mov eax, [active_map]
    cmp dword [entity_map_ids+r12*4], eax
    jne .entity_next

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
    jne .entity_color_dispatch

    mov eax, [r13+12]
    cmp eax, 0
    jl .entity_color_dispatch
    cmp eax, [monster_count]
    jae .entity_color_dispatch
    imul eax, MONSTER_SIZE
    mov edx, [monster_defs+rax+60]
    mov rcx, [rbp-32]
    mov r8d, [temp_rect+0]
    mov r9d, [temp_rect+4]
    call draw_animation
    test eax, eax
    jnz .entity_next

.entity_color_dispatch:
    mov eax, [r13+0]
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
    cmp eax, ENTITY_PORTAL
    jne .check_ladder
    mov ecx, [col_portal]
    jmp .fill_entity
.check_ladder:
    cmp eax, ENTITY_LADDER
    je .ladder_color
    cmp eax, ENTITY_ROPE
    je .ladder_color
    cmp eax, ENTITY_CHECKPOINT
    jne .check_decor
    mov ecx, [col_checkpoint]
    jmp .fill_entity
.ladder_color:
    mov ecx, [col_ladder]
    jmp .fill_entity
.check_decor:
    cmp eax, ENTITY_DECOR
    jne .event_color
    mov ecx, [col_decor]
    jmp .fill_entity
.event_color:
    mov ecx, [col_event]

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

    mov edx, [project_meta+44]     ; idle
    cmp dword [attack_ticks], 0
    jg .player_attack_anim
    cmp dword [player_grounded], 0
    je .player_air_anim
    cmp dword [player_vx], 0
    je .player_anim_ready
    mov edx, [project_meta+48]     ; walk
    jmp .player_anim_ready

.player_air_anim:
    mov edx, [project_meta+52]     ; air
    jmp .player_anim_ready

.player_attack_anim:
    mov eax, [active_skill_id]
    cmp eax, 0
    jl .player_anim_ready
    cmp eax, [skill_count]
    jae .player_anim_ready
    imul eax, SKILL_SIZE
    mov edx, [skills+rax+56]

.player_anim_ready:
    mov rcx, [rbp-32]
    mov r8d, [temp_rect+0]
    mov r9d, [temp_rect+4]
    call draw_animation
    test eax, eax
    jnz .attack_preview

    mov ecx, [col_player]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

.attack_preview:
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
    call runtime_draw_foreground

    ; HP background at configured HUD anchor.
    mov eax, [project_meta+64]
    mov [temp_rect+0], eax
    add eax, 200
    mov [temp_rect+8], eax
    mov eax, [project_meta+68]
    mov [temp_rect+4], eax
    add eax, 16
    mov [temp_rect+12], eax
    mov ecx, [col_hp_bg]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

    ; HP fill scaled to configured maximum.
    mov eax, [project_meta+64]
    mov [temp_rect+0], eax
    mov eax, [project_meta+68]
    mov [temp_rect+4], eax
    mov eax, [player_hp]
    imul eax, 200
    mov [rbp-84], eax
    call get_effective_max_hp
    mov ecx, eax
    mov eax, [rbp-84]
    cdq
    test ecx, ecx
    jg .hp_div
    mov ecx, 1
.hp_div:
    idiv ecx
    add eax, [project_meta+64]
    mov [temp_rect+8], eax
    mov eax, [project_meta+68]
    add eax, 16
    mov [temp_rect+12], eax
    mov ecx, [col_hp]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

    cmp dword [dialog_ticks], 0
    jle .hud_text

    ; NPC dialogue panel
    mov dword [temp_rect+0], 80
    mov eax, [client_rect+12]
    sub eax, 150
    mov [temp_rect+4], eax
    mov eax, [client_rect+8]
    sub eax, 80
    mov [temp_rect+8], eax
    mov eax, [client_rect+12]
    sub eax, 80
    mov [temp_rect+12], eax
    mov ecx, [col_dialog]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

.hud_text:
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

    mov rcx, [rbp-32]
    call runtime_draw_status
    mov rcx, [rbp-32]
    call runtime_draw_minimap

    cmp dword [dialog_ticks], 0
    jle .end_text

    mov rax, [dialog_text_ptr]
    test rax, rax
    jz .fallback_dialog

    mov rcx, rax
    call lstrlenA
    mov [rbp-80], eax

    mov rcx, [rbp-32]
    mov edx, 104
    mov eax, [client_rect+12]
    sub eax, 122
    mov r8d, eax
    mov r9, [dialog_text_ptr]
    mov eax, [rbp-80]
    mov [rsp+32], rax
    call TextOutA
    jmp .end_text

.fallback_dialog:
    mov rcx, [rbp-32]
    mov edx, 104
    mov eax, [client_rect+12]
    sub eax, 122
    mov r8d, eax
    lea r9, [txt_dialog]
    mov qword [rsp+32], txt_dialog_len
    call TextOutA

.end_text:
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
    cmp dword [drop_through_ticks], 0
    jle .movement_ready
    dec dword [drop_through_ticks]
.movement_ready:
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
    mov eax, [player_move_speed]
    neg eax
    mov [player_vx], eax
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
    mov eax, [player_move_speed]
    mov [player_vx], eax
    mov dword [facing_right], 1

.jump:
    cmp dword [player_grounded], 0
    je .attack_input
    mov ecx, [project_meta+88]
    call GetAsyncKeyState
    test ax, 8000h
    jz .attack_input

    mov ecx, VK_DOWN
    call GetAsyncKeyState
    test ax, 8000h
    jz .normal_jump

    mov dword [drop_through_ticks], 12
    mov dword [player_grounded], 0
    mov dword [player_vy], 2
    add dword [player_y], 4
    jmp .attack_input

.normal_jump:
    mov eax, [player_jump_speed]
    mov [player_vy], eax
    mov dword [player_grounded], 0

.attack_input:
    cmp dword [attack_cooldown], 0
    jle .can_attack
    dec dword [attack_cooldown]

.can_attack:
    cmp dword [attack_cooldown], 0
    jne .physics

    mov dword [active_skill_id], -1

    mov ecx, [project_meta+76]
    call GetAsyncKeyState
    test ax, 8000h
    jz .skill2_input
    mov dword [active_skill_id], 0
    jmp .skill_selected
.skill2_input:
    mov ecx, [project_meta+92]
    call GetAsyncKeyState
    test ax, 8000h
    jz .skill3_input
    mov dword [active_skill_id], 1
    jmp .skill_selected
.skill3_input:
    mov ecx, [project_meta+96]
    call GetAsyncKeyState
    test ax, 8000h
    jz .skill4_input
    mov dword [active_skill_id], 2
    jmp .skill_selected
.skill4_input:
    mov ecx, [project_meta+100]
    call GetAsyncKeyState
    test ax, 8000h
    jz .physics
    mov dword [active_skill_id], 3

.skill_selected:
    mov eax, [active_skill_id]
    cmp eax, 0
    jl .physics
    cmp eax, [skill_count]
    jae .physics
    imul eax, SKILL_SIZE
    lea r11, [skills+rax]

    mov dword [attack_ticks], 8
    mov dword [attack_total_ticks], 8
    mov dword [attack_frame_ticks], 1
    mov dword [attack_anim_id], -1
    mov dword [attack_cooldown], 18
    mov dword [attack_hits_done], 0
    mov dword [attack_last_frame], -1

    mov eax, [r11+36]
    cmp [player_mp], eax
    jl .physics
    sub [player_mp], eax

    mov eax, [r11+40]
    cmp eax, 1
    jge .cooldown_ready
    mov eax, 1
.cooldown_ready:
    mov [attack_cooldown], eax

    mov eax, [r11+56]
    mov [attack_anim_id], eax
    cmp eax, 0
    jl .attack_ready
    cmp eax, [animation_count]
    jae .attack_ready

    imul eax, ANIMATION_SIZE
    lea r10, [animations+rax]

    mov eax, [r10+36]
    add eax, 15
    cdq
    mov ecx, 16
    idiv ecx
    cmp eax, 1
    jge .frame_ticks_ok
    mov eax, 1
.frame_ticks_ok:
    mov [attack_frame_ticks], eax

    mov ecx, [r10+32]
    cmp ecx, 1
    jge .frame_count_ok
    mov ecx, 1
.frame_count_ok:
    imul eax, ecx
    mov [attack_ticks], eax
    mov [attack_total_ticks], eax

.attack_ready:
    mov dword [attack_hit_done], 0

.physics:
    mov eax, [player_vx]
    add [player_x], eax
    cmp dword [player_x], 0
    jge .traversal
    mov dword [player_x], 0

.traversal:
    call update_traversal
    test eax, eax
    jnz .after_collision

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
    cmp dword [drop_through_ticks], 0
    jg .after_collision

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

    mov eax, [active_map]
    cmp dword [platform_map_ids+r12*4], eax
    jne .collision_next

    mov eax, r12d
    imul eax, PLATFORM_SIZE
    lea r13, [platforms]
    add r13, rax

    mov eax, [rbp-16]
    cmp eax, [r13+0]
    jl .collision_next
    cmp eax, [r13+8]
    jg .collision_next

    ; foothold_y = y1 + (centerX-x1)*(y2-y1)/(x2-x1)
    mov ecx, [r13+8]
    sub ecx, [r13+0]
    test ecx, ecx
    jz .collision_next

    mov eax, [rbp-16]
    sub eax, [r13+0]
    mov edx, [r13+12]
    sub edx, [r13+4]
    imul eax, edx
    cdq
    idiv ecx
    add eax, [r13+4]
    mov [rbp-24], eax                  ; interpolated surface y

    mov edx, [rbp-8]
    cmp edx, eax
    jg .collision_next
    mov edx, [rbp-12]
    cmp edx, eax
    jl .collision_next

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
    jle .after_attack
    dec dword [attack_ticks]

    mov eax, [attack_total_ticks]
    sub eax, [attack_ticks]         ; elapsed ticks
    cdq
    mov ecx, [attack_frame_ticks]
    cmp ecx, 1
    jge .attack_div_ok
    mov ecx, 1
.attack_div_ok:
    idiv ecx                        ; eax = current animation frame
    mov [rbp-20], eax

    cmp eax, [attack_last_frame]
    je .after_attack

    mov edx, [attack_anim_id]
    cmp edx, 0
    jl .legacy_hit_window
    cmp edx, [animation_count]
    jae .legacy_hit_window
    imul edx, ANIMATION_SIZE
    lea r10, [animations+rdx]

    mov eax, [rbp-20]
    cmp eax, [r10+40]
    jl .after_attack
    cmp eax, [r10+44]
    jg .after_attack

    mov eax, 1
    mov edx, [active_skill_id]
    cmp edx, 0
    jl .hit_count_ready
    cmp edx, [skill_count]
    jae .hit_count_ready
    imul edx, SKILL_SIZE
    mov eax, [skills+rdx+48]
    cmp eax, 1
    jge .hit_count_ready
    mov eax, 1
.hit_count_ready:
    cmp [attack_hits_done], eax
    jae .after_attack

    call attack_monsters
    inc dword [attack_hits_done]
    mov eax, [rbp-20]
    mov [attack_last_frame], eax
    jmp .after_attack

.legacy_hit_window:
    cmp dword [attack_hit_done], 0
    jne .after_attack
    call attack_monsters
    mov dword [attack_hit_done], 1

.after_attack:
    call update_monsters_and_damage
    call update_interaction
    call runtime_update_systems

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

    mov eax, [active_map]
    cmp dword [entity_map_ids+r12*4], eax
    jne .next

    mov eax, r12d
    imul eax, ENTITY_SIZE
    lea r13, [entities]
    add r13, rax

    cmp dword [r13+0], ENTITY_MONSTER
    jne .next

    ; Resolve current attack box in world coordinates.
    mov dword [rbp-8], 0           ; offset x
    mov dword [rbp-12], -8         ; offset y
    mov dword [rbp-16], 92         ; width
    mov dword [rbp-20], 70         ; height

    mov eax, [attack_anim_id]
    cmp eax, 0
    jl .box_ready
    cmp eax, [animation_count]
    jae .box_ready
    imul eax, ANIMATION_SIZE
    lea r10, [animations+rax]
    mov eax, [r10+48]
    mov [rbp-8], eax
    mov eax, [r10+52]
    mov [rbp-12], eax
    mov eax, [r10+56]
    cmp eax, 1
    jl .keep_width
    mov [rbp-16], eax
.keep_width:
    mov eax, [r10+60]
    cmp eax, 1
    jl .box_ready
    mov [rbp-20], eax

.box_ready:
    ; Monster center point.
    mov eax, [r13+4]
    add eax, 14
    mov [rbp-24], eax
    mov eax, [r13+8]
    add eax, 18
    mov [rbp-28], eax

    mov eax, [player_y]
    add eax, [rbp-12]
    mov edx, eax                    ; top
    add eax, [rbp-20]              ; bottom
    cmp dword [rbp-28], edx
    jl .next
    cmp dword [rbp-28], eax
    jg .next

    cmp dword [facing_right], 0
    je .box_left

    mov eax, [player_x]
    add eax, PLAYER_W
    add eax, [rbp-8]               ; left
    mov edx, eax
    add eax, [rbp-16]              ; right
    cmp dword [rbp-24], edx
    jl .next
    cmp dword [rbp-24], eax
    jg .next
    jmp .apply_damage

.box_left:
    mov eax, [player_x]
    sub eax, [rbp-8]               ; right anchor
    mov edx, eax
    sub edx, [rbp-16]              ; left
    cmp dword [rbp-24], edx
    jl .next
    cmp dword [rbp-24], eax
    jg .next

.apply_damage:

    lea rax, [enemy_hp]
    mov ecx, r12d
    shl rcx, 2
    add rax, rcx
    push rax
    call equipment_attack_bonus
    mov edx, eax
    pop rax
    add edx, [player_base_attack]
    mov ecx, [active_skill_id]
    cmp ecx, 0
    jl .damage_ready
    cmp ecx, [skill_count]
    jae .damage_ready
    imul ecx, SKILL_SIZE
    add edx, [skills+rcx+32]
.damage_ready:
    sub dword [rax], edx

    ; Skill knockback on every successful hit.
    mov eax, [active_skill_id]
    cmp eax, 0
    jl .after_knockback
    cmp eax, [skill_count]
    jae .after_knockback
    imul eax, SKILL_SIZE
    mov edx, [skills+rax+52]
    cmp dword [facing_right], 0
    je .knock_monster_left
    add [r13+4], edx
    jmp .after_knockback
.knock_monster_left:
    sub [r13+4], edx
.after_knockback:
    cmp dword [rax], 0
    jg .next

    ; Reward/drop/quest progress before removing runtime monster.
    mov ecx, [r13+12]
    call monster_killed
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
; update_monsters_and_damage
; Basic chase AI + player contact damage.
; ------------------------------------------------------------
update_monsters_and_damage:
    push rbp
    mov rbp, rsp
    sub rsp, 80
    mov [rbp-40], r12
    mov [rbp-48], r13

    cmp dword [invuln_ticks], 0
    jle .dialog_tick
    dec dword [invuln_ticks]

.dialog_tick:
    cmp dword [dialog_ticks], 0
    jle .scan_start
    dec dword [dialog_ticks]

.scan_start:
    xor r12d, r12d

.scan:
    cmp r12d, [entity_count]
    jae .done

    mov eax, [active_map]
    cmp dword [entity_map_ids+r12*4], eax
    jne .next

    mov eax, r12d
    imul eax, ENTITY_SIZE
    lea r13, [entities]
    add r13, rax
    cmp dword [r13+0], ENTITY_MONSTER
    jne .next

    ; Resolve AI and movement speed from monster database.
    mov dword [rbp-12], 1
    mov dword [rbp-16], AI_CHASE
    mov eax, [r13+12]
    cmp eax, 0
    jl .ai_resolved
    cmp eax, [monster_count]
    jae .ai_resolved
    imul eax, MONSTER_SIZE
    mov edx, [monster_defs+rax+40]
    cmp edx, 1
    jge .speed_ok
    mov edx, 1
.speed_ok:
    mov [rbp-12], edx
    mov edx, [monster_defs+rax+56]
    mov [rbp-16], edx

.ai_resolved:
    ; dx = player_x - monster_x
    mov eax, [player_x]
    sub eax, [r13+4]
    mov [rbp-8], eax

    mov ecx, eax
    test ecx, ecx
    jns .abs_dx
    neg ecx
.abs_dx:
    cmp dword [rbp-16], AI_IDLE
    je .contact
    cmp dword [rbp-16], AI_RANGED
    je .ranged_ai
    cmp dword [rbp-16], AI_BOSS
    je .boss_ai

    ; Chase AI: pursue inside 300 px, stop at melee distance.
    cmp ecx, 300
    jg .contact
    cmp ecx, 36
    jl .contact
    jmp .move_toward

.boss_ai:
    cmp ecx, 520
    jg .contact
    cmp ecx, 42
    jl .contact
    mov eax, [rbp-12]
    shl eax, 1
    mov [rbp-12], eax
    jmp .move_toward

.ranged_ai:
    ; Keep approximately 120..220 px distance.
    cmp ecx, 120
    jl .move_away
    cmp ecx, 220
    jg .move_toward
    jmp .contact

.move_toward:
    cmp dword [rbp-8], 0
    jl .move_left
    mov eax, [rbp-12]
    add [r13+4], eax
    jmp .contact
.move_left:
    mov eax, [rbp-12]
    sub [r13+4], eax
    jmp .contact

.move_away:
    cmp dword [rbp-8], 0
    jl .away_right
    mov eax, [rbp-12]
    sub [r13+4], eax
    jmp .contact
.away_right:
    mov eax, [rbp-12]
    add [r13+4], eax

.contact:
    cmp dword [invuln_ticks], 0
    jne .next

    mov eax, [player_x]
    sub eax, [r13+4]
    test eax, eax
    jns .contact_abs_x
    neg eax
.contact_abs_x:
    cmp eax, 28
    jg .next

    mov eax, [player_y]
    sub eax, [r13+8]
    test eax, eax
    jns .contact_abs_y
    neg eax
.contact_abs_y:
    cmp eax, 42
    jg .next

    mov eax, [r13+12]
    cmp eax, 0
    jl .contact_damage_default
    cmp eax, [monster_count]
    jae .contact_damage_default
    imul eax, MONSTER_SIZE
    mov eax, [monster_defs+rax+36]
    jmp .contact_damage_ready
.contact_damage_default:
    mov eax, 1
.contact_damage_ready:
    sub eax, [player_defense]
    cmp eax, 1
    jge .contact_after_def
    mov eax, 1
.contact_after_def:
    sub [player_hp], eax
    mov dword [invuln_ticks], 60
    mov dword [player_vy], -8

    mov eax, [player_x]
    cmp eax, [r13+4]
    jl .knock_left
    add dword [player_x], 28
    jmp .health_check
.knock_left:
    sub dword [player_x], 28
    cmp dword [player_x], 0
    jge .health_check
    mov dword [player_x], 0

.health_check:
    cmp dword [player_hp], 0
    jg .next
    call respawn_player

.next:
    inc r12d
    jmp .scan

.done:
    mov r12, [rbp-40]
    mov r13, [rbp-48]
    leave
    ret


; ------------------------------------------------------------
; update_interaction
; E near NPC opens prototype dialogue.
; E near a portal returns to spawn (param reserved for future target data).
; ------------------------------------------------------------
update_interaction:
    push rbp
    mov rbp, rsp
    sub rsp, 80
    mov [rbp-40], r12
    mov [rbp-48], r13

    cmp dword [interact_cooldown], 0
    jle .read_key
    dec dword [interact_cooldown]

.read_key:
    mov ecx, [project_meta+80]
    call GetAsyncKeyState
    test ax, 8000h
    jz .done
    cmp dword [interact_cooldown], 0
    jne .done

    mov dword [interact_cooldown], 18
    xor r12d, r12d

.scan:
    cmp r12d, [entity_count]
    jae .done

    mov eax, [active_map]
    cmp dword [entity_map_ids+r12*4], eax
    jne .next

    mov eax, r12d
    imul eax, ENTITY_SIZE
    lea r13, [entities]
    add r13, rax

    mov eax, [r13+0]
    cmp eax, ENTITY_NPC
    je .range
    cmp eax, ENTITY_PORTAL
    je .range
    cmp eax, ENTITY_EVENT
    jne .next

.range:
    mov eax, [player_x]
    sub eax, [r13+4]
    test eax, eax
    jns .abs_x
    neg eax
.abs_x:
    cmp eax, 70
    jg .next

    mov eax, [player_y]
    sub eax, [r13+8]
    test eax, eax
    jns .abs_y
    neg eax
.abs_y:
    cmp eax, 70
    jg .next

    cmp dword [r13+0], ENTITY_NPC
    jne .check_event
    mov ecx, [r13+12]
    call npc_quest_interact
    jmp .done

.check_event:
    cmp dword [r13+0], ENTITY_EVENT
    jne .portal
    mov ecx, [r13+12]
    call run_event
    jmp .done

.portal:
    mov ecx, [r13+12]
    call change_map
    jmp .done

.next:
    inc r12d
    jmp .scan

.done:
    mov r12, [rbp-40]
    mov r13, [rbp-48]
    leave
    ret


; ------------------------------------------------------------
; respawn_player
; ------------------------------------------------------------
respawn_player:
    mov eax, [spawn_x]
    mov [player_x], eax
    mov eax, [spawn_y]
    mov [player_y], eax
    mov dword [player_vx], 0
    mov dword [player_vy], 0
    call get_effective_max_hp
    mov [player_hp], eax
    mov eax, [player_max_mp]
    mov [player_mp], eax
    mov dword [invuln_ticks], 90
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

    mov eax, [active_map]
    imul eax, MAP_SIZE
    lea r13, [maps+rax]
    mov eax, [r13+36]
    mov [spawn_x], eax
    mov eax, [r13+40]
    mov [spawn_y], eax
    mov dword [facing_right], 1
    mov dword [invuln_ticks], 0
    mov dword [interact_cooldown], 0
    mov dword [dialog_ticks], 0

    xor r12d, r12d
.scan:
    cmp r12d, [entity_count]
    jae .set_player

    mov eax, [active_map]
    cmp dword [entity_map_ids+r12*4], eax
    jne .next

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

    mov edx, 3
    mov eax, [r13+12]
    cmp eax, 0
    jl .set_enemy_hp
    cmp eax, [monster_count]
    jae .set_enemy_hp
    imul eax, MONSTER_SIZE
    mov edx, [monster_defs+rax+32]
.set_enemy_hp:
    mov eax, r12d
    mov [enemy_hp+rax*4], edx

.next:
    inc r12d
    jmp .scan

.set_player:
    mov eax, [spawn_x]
    mov [player_x], eax
    mov eax, [spawn_y]
    mov [player_y], eax
    mov dword [player_vx], 0
    mov dword [player_vy], 0
    mov dword [drop_through_ticks], 0

    mov r12, [rbp-40]
    mov r13, [rbp-48]
    leave
    ret


; ------------------------------------------------------------
; load_project -> eax = 1 success, 0 fail
; ------------------------------------------------------------
load_project_legacy:
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


%include "runtime_systems.inc"
