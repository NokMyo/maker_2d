; Tsuramechoki runtime RPG systems
; Included by runtime.asm. 100% x86-64 NASM.

section .data
    ts_save_path          db "save0.tsav",0
    ts_hud_fmt1           db "HP %d/%d   MP %d/%d",0
    ts_hud_fmt2           db "LV %d   EXP %d   Money %d",0
    ts_hud_fmt3           db "Quest %d/%d   Potion %d",0
    ts_save_msg           db "F2 Save   F3 Load   C Potion",0
    ts_save_msg_len       equ $-ts_save_msg-1

section .bss
    project_header_v2     resb PROJECT_HEADER_SIZE
    database_header       resb DATABASE_HEADER_SIZE
    save_header           resb SAVE_HEADER_SIZE

    platform_map_ids      resd MAX_PLATFORMS
    entity_map_ids        resd MAX_ENTITIES

    maps                  resb MAX_MAPS*MAP_SIZE
    animations            resb MAX_ANIMATIONS*ANIMATION_SIZE
    skills                resb MAX_SKILLS*SKILL_SIZE
    monster_defs          resb MAX_MONSTERS*MONSTER_SIZE
    items                 resb MAX_ITEMS*ITEM_SIZE
    quests                resb MAX_QUESTS*QUEST_SIZE
    variables             resb MAX_VARIABLES*VARIABLE_SIZE

    map_count             resd 1
    active_map            resd 1
    screen_width          resd 1
    screen_height         resd 1
    player_move_speed     resd 1
    player_jump_speed     resd 1
    player_max_hp         resd 1
    player_max_mp         resd 1
    player_base_attack    resd 1
    project_flags         resd 1

    animation_count       resd 1
    skill_count           resd 1
    monster_count         resd 1
    item_count            resd 1
    quest_count           resd 1
    variable_count        resd 1

    player_mp             resd 1
    player_exp            resd 1
    player_level          resd 1
    player_money          resd 1
    quest_state           resd MAX_QUESTS
    quest_progress        resd MAX_QUESTS
    variable_values       resd MAX_VARIABLES
    inventory_counts      resd MAX_ITEMS
    system_key_cooldown   resd 1
    ts_hud_buffer         resb 160

section .text

ts_rt_read_block:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call ReadFile
    leave
    ret

ts_rt_write_block:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call WriteFile
    leave
    ret

; ------------------------------------------------------------
; TSRP v2 project loader
; ------------------------------------------------------------
load_project:
    push rbp
    mov rbp, rsp
    sub rsp, 112

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
    mov [rbp-16], rax

    mov rcx, rax
    lea rdx, [project_header_v2]
    mov r8d, PROJECT_HEADER_SIZE
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov rax, PROJECT_MAGIC_QWORD
    cmp qword [project_header_v2], rax
    jne .close_fail
    cmp dword [project_header_v2+8], PROJECT_VERSION
    jne .close_fail
    cmp dword [project_header_v2+12], PROJECT_HEADER_SIZE
    jne .close_fail

    mov eax, [project_header_v2+16]
    cmp eax, MAX_PLATFORMS
    ja .close_fail
    mov [platform_count], eax

    mov eax, [project_header_v2+20]
    cmp eax, MAX_ENTITIES
    ja .close_fail
    mov [entity_count], eax

    mov eax, [project_header_v2+24]
    test eax, eax
    jz .close_fail
    cmp eax, MAX_MAPS
    ja .close_fail
    mov [map_count], eax

    mov eax, [project_header_v2+28]
    cmp eax, [map_count]
    jae .close_fail
    mov [active_map], eax

    mov eax, [project_header_v2+32]
    mov [screen_width], eax
    mov eax, [project_header_v2+36]
    mov [screen_height], eax
    mov eax, [project_header_v2+40]
    mov [player_move_speed], eax
    mov eax, [project_header_v2+44]
    mov [player_jump_speed], eax
    mov eax, [project_header_v2+48]
    mov [player_max_hp], eax
    mov eax, [project_header_v2+52]
    mov [player_max_mp], eax
    mov eax, [project_header_v2+56]
    mov [player_base_attack], eax
    mov eax, [project_header_v2+60]
    mov [project_flags], eax

    mov eax, [platform_count]
    imul eax, PLATFORM_SIZE
    test eax, eax
    jz .entities
    mov rcx, [rbp-16]
    lea rdx, [platforms]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

.entities:
    mov eax, [entity_count]
    imul eax, ENTITY_SIZE
    test eax, eax
    jz .platform_maps
    mov rcx, [rbp-16]
    lea rdx, [entities]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

.platform_maps:
    mov eax, [platform_count]
    shl eax, 2
    test eax, eax
    jz .entity_maps
    mov rcx, [rbp-16]
    lea rdx, [platform_map_ids]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

.entity_maps:
    mov eax, [entity_count]
    shl eax, 2
    test eax, eax
    jz .maps
    mov rcx, [rbp-16]
    lea rdx, [entity_map_ids]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

.maps:
    mov eax, [map_count]
    imul eax, MAP_SIZE
    mov rcx, [rbp-16]
    lea rdx, [maps]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov rcx, [rbp-16]
    lea rdx, [database_header]
    mov r8d, DATABASE_HEADER_SIZE
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov eax, [database_header+0]
    cmp eax, MAX_ANIMATIONS
    ja .close_fail
    mov [animation_count], eax
    mov eax, [database_header+4]
    cmp eax, MAX_SKILLS
    ja .close_fail
    mov [skill_count], eax
    mov eax, [database_header+8]
    cmp eax, MAX_MONSTERS
    ja .close_fail
    mov [monster_count], eax
    mov eax, [database_header+12]
    cmp eax, MAX_ITEMS
    ja .close_fail
    mov [item_count], eax
    mov eax, [database_header+16]
    cmp eax, MAX_QUESTS
    ja .close_fail
    mov [quest_count], eax
    mov eax, [database_header+20]
    cmp eax, MAX_VARIABLES
    ja .close_fail
    mov [variable_count], eax

    mov eax, [animation_count]
    imul eax, ANIMATION_SIZE
    mov rcx, [rbp-16]
    lea rdx, [animations]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov eax, [skill_count]
    imul eax, SKILL_SIZE
    mov rcx, [rbp-16]
    lea rdx, [skills]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov eax, [monster_count]
    imul eax, MONSTER_SIZE
    mov rcx, [rbp-16]
    lea rdx, [monster_defs]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov eax, [item_count]
    imul eax, ITEM_SIZE
    mov rcx, [rbp-16]
    lea rdx, [items]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov eax, [quest_count]
    imul eax, QUEST_SIZE
    mov rcx, [rbp-16]
    lea rdx, [quests]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov eax, [variable_count]
    imul eax, VARIABLE_SIZE
    mov rcx, [rbp-16]
    lea rdx, [variables]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov rcx, [rbp-16]
    call CloseHandle
    mov eax, 1
    leave
    ret

.close_fail:
    mov rcx, [rbp-16]
    call CloseHandle
.fail:
    xor eax, eax
    leave
    ret

; ------------------------------------------------------------
; Initialize RPG state for a new play session.
; ------------------------------------------------------------
runtime_init_state:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov [rbp-40], r12

    mov eax, [player_max_hp]
    mov [player_hp], eax
    mov eax, [player_max_mp]
    mov [player_mp], eax
    mov dword [player_exp], 0
    mov dword [player_level], 1
    mov dword [player_money], 0
    mov dword [system_key_cooldown], 0

    ; give a few starter potions when item 0 exists
    cmp dword [item_count], 0
    jle .vars
    mov dword [inventory_counts], 3

.vars:
    xor r12d, r12d
.var_loop:
    cmp r12d, [variable_count]
    jae .done
    mov eax, r12d
    imul eax, VARIABLE_SIZE
    mov eax, [variables+rax+32]
    mov [variable_values+r12*4], eax
    inc r12d
    jmp .var_loop
.done:
    mov r12, [rbp-40]
    leave
    ret

; ------------------------------------------------------------
; Per-frame RPG systems: save/load + consumable.
; ------------------------------------------------------------
runtime_update_systems:
    push rbp
    mov rbp, rsp
    sub rsp, 48

    cmp dword [system_key_cooldown], 0
    jle .keys
    dec dword [system_key_cooldown]
.keys:
    cmp dword [system_key_cooldown], 0
    jne .done

    mov ecx, 071h                 ; F2 save
    call GetAsyncKeyState
    test ax, 8000h
    jz .load
    call save_game
    mov dword [system_key_cooldown], 20
    jmp .done
.load:
    mov ecx, 072h                 ; F3 load
    call GetAsyncKeyState
    test ax, 8000h
    jz .potion
    call load_game
    mov dword [system_key_cooldown], 20
    jmp .done
.potion:
    mov ecx, 043h                 ; C
    call GetAsyncKeyState
    test ax, 8000h
    jz .done
    call use_item0
    mov dword [system_key_cooldown], 15
.done:
    leave
    ret

use_item0:
    cmp dword [item_count], 0
    jle .done
    cmp dword [inventory_counts], 0
    jle .done
    mov eax, [player_hp]
    cmp eax, [player_max_hp]
    jge .done
    dec dword [inventory_counts]
    mov eax, [items+36]
    add [player_hp], eax
    mov eax, [player_max_hp]
    cmp [player_hp], eax
    jle .done
    mov [player_hp], eax
.done:
    ret

; ------------------------------------------------------------
; Save slot 0.
; ------------------------------------------------------------
save_game:
    push rbp
    mov rbp, rsp
    sub rsp, 96

    mov rax, SAVE_MAGIC_QWORD
    mov [save_header+0], rax
    mov dword [save_header+8], SAVE_VERSION
    mov eax, [active_map]
    mov [save_header+12], eax
    mov eax, [player_x]
    mov [save_header+16], eax
    mov eax, [player_y]
    mov [save_header+20], eax
    mov eax, [player_hp]
    mov [save_header+24], eax
    mov eax, [player_mp]
    mov [save_header+28], eax
    mov eax, [player_exp]
    mov [save_header+32], eax
    mov eax, [player_level]
    mov [save_header+36], eax
    mov eax, [player_money]
    mov [save_header+40], eax
    mov eax, [quest_count]
    mov [save_header+44], eax
    mov eax, [variable_count]
    mov [save_header+48], eax
    mov eax, [item_count]
    mov [save_header+52], eax
    mov qword [save_header+56], 0

    lea rcx, [ts_save_path]
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
    lea rdx, [save_header]
    mov r8d, SAVE_HEADER_SIZE
    call ts_rt_write_block
    test eax, eax
    jz .close_fail

    mov rcx, [rbp-8]
    lea rdx, [quest_state]
    mov r8d, MAX_QUESTS*4
    call ts_rt_write_block
    test eax, eax
    jz .close_fail

    mov rcx, [rbp-8]
    lea rdx, [quest_progress]
    mov r8d, MAX_QUESTS*4
    call ts_rt_write_block
    test eax, eax
    jz .close_fail

    mov rcx, [rbp-8]
    lea rdx, [variable_values]
    mov r8d, MAX_VARIABLES*4
    call ts_rt_write_block
    test eax, eax
    jz .close_fail

    mov rcx, [rbp-8]
    lea rdx, [inventory_counts]
    mov r8d, MAX_ITEMS*4
    call ts_rt_write_block
    test eax, eax
    jz .close_fail

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

load_game:
    push rbp
    mov rbp, rsp
    sub rsp, 96

    lea rcx, [ts_save_path]
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
    lea rdx, [save_header]
    mov r8d, SAVE_HEADER_SIZE
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov rax, SAVE_MAGIC_QWORD
    cmp qword [save_header], rax
    jne .close_fail
    cmp dword [save_header+8], SAVE_VERSION
    jne .close_fail

    mov rcx, [rbp-8]
    lea rdx, [quest_state]
    mov r8d, MAX_QUESTS*4
    call ts_rt_read_block
    test eax, eax
    jz .close_fail
    mov rcx, [rbp-8]
    lea rdx, [quest_progress]
    mov r8d, MAX_QUESTS*4
    call ts_rt_read_block
    test eax, eax
    jz .close_fail
    mov rcx, [rbp-8]
    lea rdx, [variable_values]
    mov r8d, MAX_VARIABLES*4
    call ts_rt_read_block
    test eax, eax
    jz .close_fail
    mov rcx, [rbp-8]
    lea rdx, [inventory_counts]
    mov r8d, MAX_ITEMS*4
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov rcx, [rbp-8]
    call CloseHandle

    mov eax, [save_header+12]
    mov [active_map], eax
    mov eax, [save_header+16]
    mov [player_x], eax
    mov eax, [save_header+20]
    mov [player_y], eax
    mov eax, [save_header+24]
    mov [player_hp], eax
    mov eax, [save_header+28]
    mov [player_mp], eax
    mov eax, [save_header+32]
    mov [player_exp], eax
    mov eax, [save_header+36]
    mov [player_level], eax
    mov eax, [save_header+40]
    mov [player_money], eax
    mov dword [player_vx], 0
    mov dword [player_vy], 0
    mov dword [camera_x], 0
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
; Map change.
; ecx = target map id
; ------------------------------------------------------------
change_map:
    cmp ecx, 0
    jl .done
    cmp ecx, [map_count]
    jae .done
    mov [active_map], ecx
    mov dword [camera_x], 0
    call initialize_player
.done:
    ret

; ------------------------------------------------------------
; Quest/NPC
; ecx = quest id
; ------------------------------------------------------------
npc_quest_interact:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp-4], ecx

    cmp ecx, 0
    jl .done
    cmp ecx, [quest_count]
    jae .done

    lea r8, [quest_state]
    mov eax, [rbp-4]
    cmp dword [r8+rax*4], QUEST_LOCKED
    jne .check_complete
    mov dword [r8+rax*4], QUEST_ACTIVE
    mov dword [dialog_ticks], 180
    jmp .done

.check_complete:
    cmp dword [r8+rax*4], QUEST_ACTIVE
    jne .done
    mov ecx, [rbp-4]
    imul ecx, QUEST_SIZE
    lea r9, [quests+rcx]
    mov edx, [r9+40]
    mov eax, [rbp-4]
    cmp [quest_progress+rax*4], edx
    jl .done

    mov dword [r8+rax*4], QUEST_REWARDED
    mov ecx, [r9+44]
    add [player_exp], ecx
    mov ecx, [r9+48]
    add [player_money], ecx
    mov ecx, [r9+52]
    cmp ecx, 0
    jl .dialog
    cmp ecx, [item_count]
    jae .dialog
    mov edx, [r9+56]
    add [inventory_counts+rcx*4], edx
.dialog:
    mov dword [dialog_ticks], 180
.done:
    leave
    ret

; ------------------------------------------------------------
; On monster death.
; ecx = monster database id
; ------------------------------------------------------------
monster_killed:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov [rbp-4], ecx

    cmp ecx, 0
    jl .quests
    cmp ecx, [monster_count]
    jae .quests
    mov eax, ecx
    imul eax, MONSTER_SIZE
    lea r8, [monster_defs+rax]
    mov eax, [r8+44]
    add [player_exp], eax
    mov eax, [r8+48]
    cmp eax, 0
    jl .quests
    cmp eax, [item_count]
    jae .quests
    mov edx, [r8+52]
    add [inventory_counts+rax*4], edx

.quests:
    xor eax, eax
.qloop:
    cmp eax, [quest_count]
    jae .done
    cmp dword [quest_state+rax*4], QUEST_ACTIVE
    jne .qnext
    mov edx, eax
    imul edx, QUEST_SIZE
    lea r8, [quests+rdx]
    cmp dword [r8+32], QUEST_KILL
    jne .qnext
    mov edx, [r8+36]
    cmp edx, [rbp-4]
    jne .qnext
    inc dword [quest_progress+rax*4]
.qnext:
    inc eax
    jmp .qloop
.done:
    leave
    ret

; ------------------------------------------------------------
; Runtime HUD extension.
; rcx = HDC
; ------------------------------------------------------------
runtime_draw_status:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    mov [rbp-40], r12
    mov r12, rcx

    lea rcx, [ts_hud_buffer]
    lea rdx, [ts_hud_fmt1]
    mov r8d, [player_hp]
    mov r9d, [player_max_hp]
    mov eax, [player_mp]
    mov [rsp+32], rax
    mov eax, [player_max_mp]
    mov [rsp+40], rax
    call wsprintfA
    mov [rbp-8], eax
    mov rcx, r12
    mov edx, 16
    mov r8d, 90
    lea r9, [ts_hud_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    lea rcx, [ts_hud_buffer]
    lea rdx, [ts_hud_fmt2]
    mov r8d, [player_level]
    mov r9d, [player_exp]
    mov eax, [player_money]
    mov [rsp+32], rax
    call wsprintfA
    mov [rbp-8], eax
    mov rcx, r12
    mov edx, 16
    mov r8d, 112
    lea r9, [ts_hud_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    mov eax, 0
    cmp dword [quest_count], 0
    jle .quest_ready
    mov eax, [quest_progress]
.quest_ready:
    lea rcx, [ts_hud_buffer]
    lea rdx, [ts_hud_fmt3]
    mov r8d, eax
    xor r9d, r9d
    cmp dword [quest_count], 0
    jle .qtarget_ready
    mov r9d, [quests+40]
.qtarget_ready:
    mov eax, [inventory_counts]
    mov [rsp+32], rax
    call wsprintfA
    mov [rbp-8], eax
    mov rcx, r12
    mov edx, 16
    mov r8d, 134
    lea r9, [ts_hud_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    mov rcx, r12
    mov edx, 16
    mov r8d, 156
    lea r9, [ts_save_msg]
    mov qword [rsp+32], ts_save_msg_len
    call TextOutA

    mov r12, [rbp-40]
    leave
    ret
