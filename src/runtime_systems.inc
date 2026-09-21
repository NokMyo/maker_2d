; Tsuramechoki runtime RPG systems
; Included by runtime.asm. 100% x86-64 NASM.

section .data
    ts_save_path0         db "save0.tsav",0
    ts_save_path1         db "save1.tsav",0
    ts_save_path2         db "save2.tsav",0
    ts_hud_fmt1           db "HP %d/%d   MP %d/%d",0
    ts_hud_fmt2           db "LV %d   EXP %d   Money %d",0
    ts_hud_fmt3           db "Quest %d/%d   Potion %d",0
    ts_save_msg           db "F2 Save  F3 Load  F4 Slot  C Potion  I/K/J/T panels  V Equip",0
    ts_save_msg_len       equ $-ts_save_msg-1
    ts_panel_item_fmt     db "Inventory: %s x%d",0
    ts_panel_skill_fmt    db "Skill: %s  DMG %d  MP %d",0
    ts_panel_quest_fmt    db "Quest: %s  state %d  progress %d",0
    ts_panel_stats_fmt    db "Stats: ATK %d  DEF %d  Weapon %d  Armor %d",0
    ts_slot_fmt           db "Save slot: %d/%d",0

section .bss
    project_header_v2     resb PROJECT_HEADER_SIZE
    project_meta          resb PROJECT_META_SIZE
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
    events                resb MAX_EVENTS*EVENT_SIZE
    sprites               resb MAX_SPRITES*SPRITE_SIZE

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
    start_map             resd 1
    save_slots            resd 1
    current_save_slot     resd 1

    animation_count       resd 1
    skill_count           resd 1
    monster_count         resd 1
    item_count            resd 1
    quest_count           resd 1
    variable_count        resd 1
    event_count           resd 1
    sprite_count          resd 1

    player_mp             resd 1
    player_exp            resd 1
    player_level          resd 1
    player_money          resd 1
    quest_state           resd MAX_QUESTS
    quest_progress        resd MAX_QUESTS
    variable_values       resd MAX_VARIABLES
    inventory_counts      resd MAX_ITEMS
    system_key_cooldown   resd 1
    climbing              resd 1
    ui_panel              resd 1
    equipped_weapon       resd 1
    equipped_armor        resd 1
    player_defense        resd 1
    mp_regen_tick         resd 1
    game_tick             resd 1
    ts_bitmap_info        resb 32
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

    mov rcx, [rbp-16]
    lea rdx, [project_meta]
    mov r8d, PROJECT_META_SIZE
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

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

    mov eax, [project_meta+32]
    cmp eax, [map_count]
    jae .close_fail
    mov [start_map], eax
    mov [active_map], eax

    mov eax, [project_meta+36]
    cmp eax, 1
    jl .close_fail
    cmp eax, 3
    jg .close_fail
    mov [save_slots], eax
    mov dword [current_save_slot], 0

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
    mov eax, [database_header+24]
    cmp eax, MAX_EVENTS
    ja .close_fail
    mov [event_count], eax
    mov eax, [database_header+28]
    cmp eax, MAX_SPRITES
    ja .close_fail
    mov [sprite_count], eax

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

    mov eax, [event_count]
    imul eax, EVENT_SIZE
    mov rcx, [rbp-16]
    lea rdx, [events]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    mov eax, [sprite_count]
    imul eax, SPRITE_SIZE
    mov rcx, [rbp-16]
    lea rdx, [sprites]
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
    mov dword [current_save_slot], 0
    mov dword [climbing], 0
    mov dword [ui_panel], 0
    mov dword [equipped_weapon], -1
    mov dword [equipped_armor], -1
    mov dword [player_defense], 0
    mov dword [mp_regen_tick], 0
    mov dword [game_tick], 0

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

    call update_checkpoint
    inc dword [game_tick]

    inc dword [mp_regen_tick]
    cmp dword [mp_regen_tick], 60
    jl .cooldown
    mov dword [mp_regen_tick], 0
    mov eax, [player_mp]
    cmp eax, [player_max_mp]
    jge .cooldown
    inc dword [player_mp]

.cooldown:
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
    jz .slot
    call load_game
    mov dword [system_key_cooldown], 20
    jmp .done
.slot:
    mov ecx, 073h                 ; F4 cycle save slot
    call GetAsyncKeyState
    test ax, 8000h
    jz .potion
    inc dword [current_save_slot]
    mov eax, [current_save_slot]
    cmp eax, [save_slots]
    jl .slot_ready
    mov dword [current_save_slot], 0
.slot_ready:
    mov dword [system_key_cooldown], 20
    jmp .done
.potion:
    mov ecx, 043h                 ; C
    call GetAsyncKeyState
    test ax, 8000h
    jz .inventory_panel
    call use_item0
    mov dword [system_key_cooldown], 15
    jmp .done

.inventory_panel:
    mov ecx, 049h                 ; I
    call GetAsyncKeyState
    test ax, 8000h
    jz .skill_panel
    mov dword [ui_panel], 1
    mov dword [system_key_cooldown], 15
    jmp .done
.skill_panel:
    mov ecx, 04Bh                 ; K
    call GetAsyncKeyState
    test ax, 8000h
    jz .quest_panel
    mov dword [ui_panel], 2
    mov dword [system_key_cooldown], 15
    jmp .done
.quest_panel:
    mov ecx, 04Ah                 ; J
    call GetAsyncKeyState
    test ax, 8000h
    jz .stats_panel
    mov dword [ui_panel], 3
    mov dword [system_key_cooldown], 15
    jmp .done
.stats_panel:
    mov ecx, 054h                 ; T
    call GetAsyncKeyState
    test ax, 8000h
    jz .equip
    mov dword [ui_panel], 4
    mov dword [system_key_cooldown], 15
    jmp .done
.equip:
    mov ecx, 056h                 ; V
    call GetAsyncKeyState
    test ax, 8000h
    jz .done
    call equip_first_gear
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
; Active save-slot path -> rax
; ------------------------------------------------------------
get_save_path:
    mov eax, [current_save_slot]
    cmp eax, 1
    je .slot1
    cmp eax, 2
    je .slot2
    lea rax, [ts_save_path0]
    ret
.slot1:
    lea rax, [ts_save_path1]
    ret
.slot2:
    lea rax, [ts_save_path2]
    ret

; ------------------------------------------------------------
; Save active slot.
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

    call get_save_path
    mov rcx, rax
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

    call get_save_path
    mov rcx, rax
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
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp-4], ecx

    cmp ecx, 0
    jl .done
    cmp ecx, [map_count]
    jae .done

    mov [active_map], ecx
    mov dword [camera_x], 0
    call initialize_player

    mov ecx, [rbp-4]
    call quest_reach_map

.done:
    leave
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
    call add_item
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
    mov ecx, eax
    mov edx, [r8+52]
    call add_item

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
; Add item and update collect quests.
; ecx = item id, edx = count
; ------------------------------------------------------------
add_item:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov [rbp-4], ecx
    mov [rbp-8], edx

    cmp ecx, 0
    jl .done
    cmp ecx, [item_count]
    jae .done
    add [inventory_counts+rcx*4], edx

    xor eax, eax
.qloop:
    cmp eax, [quest_count]
    jae .done
    cmp dword [quest_state+rax*4], QUEST_ACTIVE
    jne .next
    mov edx, eax
    imul edx, QUEST_SIZE
    lea r8, [quests+rdx]
    cmp dword [r8+32], QUEST_COLLECT
    jne .next
    mov edx, [r8+36]
    cmp edx, [rbp-4]
    jne .next
    mov edx, [inventory_counts+rdx*4]
    mov [quest_progress+rax*4], edx
.next:
    inc eax
    jmp .qloop
.done:
    leave
    ret

; ------------------------------------------------------------
; Reach-map quest progress.
; ecx = map id
; ------------------------------------------------------------
quest_reach_map:
    xor eax, eax
.loop:
    cmp eax, [quest_count]
    jae .done
    cmp dword [quest_state+rax*4], QUEST_ACTIVE
    jne .next
    mov edx, eax
    imul edx, QUEST_SIZE
    lea r8, [quests+rdx]
    cmp dword [r8+32], QUEST_REACH
    jne .next
    cmp dword [r8+36], ecx
    jne .next
    mov dword [quest_progress+rax*4], 1
.next:
    inc eax
    jmp .loop
.done:
    ret

; ------------------------------------------------------------
; Event interpreter.
; ecx = event id
; ------------------------------------------------------------
run_event:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    mov [rbp-40], r12
    mov [rbp-48], r13

    cmp ecx, 0
    jl .done
    cmp ecx, [event_count]
    jae .done

    mov eax, ecx
    imul eax, EVENT_SIZE
    lea r13, [events+rax]
    mov r12d, [r13+32]
    cmp r12d, EVENT_COMMANDS
    jle .count_ok
    mov r12d, EVENT_COMMANDS
.count_ok:
    xor ecx, ecx
.loop:
    cmp ecx, r12d
    jae .done
    mov eax, ecx
    imul eax, EVENT_COMMAND_SIZE
    lea r8, [r13+rax+48]
    mov eax, [r8+0]

    cmp eax, EVENT_DIALOGUE
    je .dialogue
    cmp eax, EVENT_SET_VAR
    je .set_var
    cmp eax, EVENT_ADD_ITEM
    je .add_item
    cmp eax, EVENT_ADD_MONEY
    je .add_money
    cmp eax, EVENT_CHANGE_MAP
    je .map
    cmp eax, EVENT_START_QUEST
    je .quest
    cmp eax, EVENT_SET_FLAG
    je .set_var
    jmp .next

.dialogue:
    mov dword [dialog_ticks], 180
    jmp .next

.set_var:
    mov eax, [r8+4]
    cmp eax, 0
    jl .next
    cmp eax, [variable_count]
    jae .next
    mov edx, [r8+8]
    mov [variable_values+rax*4], edx
    jmp .next

.add_item:
    mov eax, [r8+4]
    mov edx, [r8+8]
    push rcx
    mov ecx, eax
    call add_item
    pop rcx
    jmp .next

.add_money:
    mov eax, [r8+4]
    add [player_money], eax
    jmp .next

.map:
    mov eax, [r8+4]
    push rcx
    mov ecx, eax
    call change_map
    pop rcx
    jmp .next

.quest:
    mov eax, [r8+4]
    cmp eax, 0
    jl .next
    cmp eax, [quest_count]
    jae .next
    mov dword [quest_state+rax*4], QUEST_ACTIVE

.next:
    inc ecx
    jmp .loop

.done:
    mov r12, [rbp-40]
    mov r13, [rbp-48]
    leave
    ret

; ------------------------------------------------------------
; Ladder/rope traversal.
; eax=1 when climbing physics owns vertical movement.
; ------------------------------------------------------------
update_traversal:
    push rbp
    mov rbp, rsp
    sub rsp, 80
    mov [rbp-40], r12
    mov [rbp-48], r13

    xor r12d, r12d
.scan:
    cmp r12d, [entity_count]
    jae .not_near
    mov eax, [active_map]
    cmp dword [entity_map_ids+r12*4], eax
    jne .next

    mov eax, r12d
    imul eax, ENTITY_SIZE
    lea r13, [entities+rax]
    mov eax, [r13+0]
    cmp eax, ENTITY_LADDER
    je .check
    cmp eax, ENTITY_ROPE
    jne .next

.check:
    mov eax, [player_x]
    add eax, PLAYER_W/2
    sub eax, [r13+4]
    test eax, eax
    jns .absx
    neg eax
.absx:
    cmp eax, 26
    jg .next

    mov eax, [player_y]
    add eax, PLAYER_H/2
    mov edx, [r13+8]
    sub edx, PLAYER_H
    cmp eax, edx
    jl .next
    mov ecx, [r13+12]
    test ecx, ecx
    jg .length_ok
    mov ecx, 160
.length_ok:
    add ecx, [r13+8]
    cmp eax, ecx
    jg .next

    mov ecx, 026h
    call GetAsyncKeyState
    test ax, 8000h
    jz .down
    sub dword [player_y], 4
    mov dword [climbing], 1
    jmp .owned
.down:
    mov ecx, 028h
    call GetAsyncKeyState
    test ax, 8000h
    jz .hold
    add dword [player_y], 4
    mov dword [climbing], 1
    jmp .owned
.hold:
    cmp dword [climbing], 0
    je .next
.owned:
    mov dword [player_vy], 0
    mov dword [player_grounded], 0
    mov eax, 1
    jmp .done

.next:
    inc r12d
    jmp .scan

.not_near:
    mov dword [climbing], 0
    xor eax, eax
.done:
    mov r12, [rbp-40]
    mov r13, [rbp-48]
    leave
    ret

; ------------------------------------------------------------
; Checkpoint touch updates respawn position.
; ------------------------------------------------------------
update_checkpoint:
    xor r10d, r10d
.loop:
    cmp r10d, [entity_count]
    jae .done
    mov eax, [active_map]
    cmp dword [entity_map_ids+r10*4], eax
    jne .next
    mov eax, r10d
    imul eax, ENTITY_SIZE
    lea r11, [entities+rax]
    cmp dword [r11+0], ENTITY_CHECKPOINT
    jne .next

    mov eax, [player_x]
    sub eax, [r11+4]
    test eax, eax
    jns .absx
    neg eax
.absx:
    cmp eax, 40
    jg .next
    mov eax, [player_y]
    sub eax, [r11+8]
    test eax, eax
    jns .absy
    neg eax
.absy:
    cmp eax, 60
    jg .next
    mov eax, [r11+4]
    mov [spawn_x], eax
    mov eax, [r11+8]
    mov [spawn_y], eax
.next:
    inc r10d
    jmp .loop
.done:
    ret

; ------------------------------------------------------------
; Equip first owned weapon/armor.
; ------------------------------------------------------------
equip_first_gear:
    xor r10d, r10d
.scan:
    cmp r10d, [item_count]
    jae .recalc
    cmp dword [inventory_counts+r10*4], 0
    jle .next
    mov eax, r10d
    imul eax, ITEM_SIZE
    lea r11, [items+rax]
    cmp dword [r11+32], ITEM_WEAPON
    jne .armor
    mov [equipped_weapon], r10d
    jmp .next
.armor:
    cmp dword [r11+32], ITEM_ARMOR
    jne .next
    mov [equipped_armor], r10d
.next:
    inc r10d
    jmp .scan

.recalc:
    mov dword [player_defense], 0
    mov eax, [equipped_armor]
    cmp eax, 0
    jl .done
    cmp eax, [item_count]
    jae .done
    imul eax, ITEM_SIZE
    mov eax, [items+rax+52]
    mov [player_defense], eax
.done:
    ret

; ------------------------------------------------------------
; Effective attack bonus from equipped weapon -> eax
; ------------------------------------------------------------
equipment_attack_bonus:
    mov eax, [equipped_weapon]
    cmp eax, 0
    jl .none
    cmp eax, [item_count]
    jae .none
    imul eax, ITEM_SIZE
    mov eax, [items+rax+48]
    ret
.none:
    xor eax, eax
    ret


; ------------------------------------------------------------
; Draw one frame from an imported BMP sprite sheet.
; rcx=HDC, edx=animation id, r8d=x, r9d=y
; eax=1 drawn, 0 fallback
; ------------------------------------------------------------
draw_animation:
    push rbp
    mov rbp, rsp
    sub rsp, 128
    mov [rbp-40], r12
    mov [rbp-48], r13
    mov [rbp-56], r14
    mov [rbp-64], r15

    mov r12, rcx
    mov [rbp-8], r8d
    mov [rbp-12], r9d

    cmp edx, 0
    jl .fail
    cmp edx, [animation_count]
    jae .fail

    mov eax, edx
    imul eax, ANIMATION_SIZE
    lea r13, [animations+rax]

    mov eax, [r13+68]
    cmp eax, 0
    jl .fail
    cmp eax, [sprite_count]
    jae .fail

    imul eax, SPRITE_SIZE
    lea r14, [sprites+rax]
    cmp byte [r14+32], 0
    je .fail

    xor ecx, ecx
    lea rdx, [r14+32]
    xor r8d, r8d
    xor r9d, r9d
    mov qword [rsp+32], 0
    mov qword [rsp+40], 02010h
    call LoadImageA
    test rax, rax
    jz .fail
    mov r15, rax

    mov rcx, rax
    mov edx, 32
    lea r8, [ts_bitmap_info]
    call GetObjectA
    test eax, eax
    jz .delete_bitmap

    mov rcx, r12
    call CreateCompatibleDC
    test rax, rax
    jz .delete_bitmap
    mov [rbp-24], rax

    mov rcx, rax
    mov rdx, r15
    call SelectObject
    mov [rbp-32], rax

    mov ecx, [r13+32]
    cmp ecx, 1
    jge .frames_ok
    mov ecx, 1
.frames_ok:
    mov eax, [ts_bitmap_info+4]
    cdq
    idiv ecx
    mov [rbp-16], eax              ; frame width

    mov eax, [r13+36]
    cdq
    mov ecx, 16
    idiv ecx
    cmp eax, 1
    jge .ticks_ok
    mov eax, 1
.ticks_ok:
    mov ecx, eax
    mov eax, [game_tick]
    xor edx, edx
    div ecx

    mov ecx, [r13+32]
    cmp ecx, 1
    jge .mod_ok
    mov ecx, 1
.mod_ok:
    xor edx, edx
    div ecx
    mov eax, edx
    imul eax, [rbp-16]
    mov [rbp-20], eax              ; source x

    mov rcx, r12
    mov edx, [rbp-8]
    mov r8d, [rbp-12]
    mov r9d, [rbp-16]
    mov eax, [ts_bitmap_info+8]
    mov [rsp+32], rax
    mov rax, [rbp-24]
    mov [rsp+40], rax
    mov eax, [rbp-20]
    mov [rsp+48], rax
    mov qword [rsp+56], 0
    mov qword [rsp+64], 00CC0020h
    call BitBlt

    mov rcx, [rbp-24]
    mov rdx, [rbp-32]
    call SelectObject
    mov rcx, [rbp-24]
    call DeleteDC
    mov rcx, r15
    call DeleteObject
    mov eax, 1
    jmp .done

.delete_bitmap:
    mov rcx, r15
    call DeleteObject
.fail:
    xor eax, eax
.done:
    mov r12, [rbp-40]
    mov r13, [rbp-48]
    mov r14, [rbp-56]
    mov r15, [rbp-64]
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

    lea rcx, [ts_hud_buffer]
    lea rdx, [ts_slot_fmt]
    mov r8d, [current_save_slot]
    inc r8d
    mov r9d, [save_slots]
    call wsprintfA
    mov [rbp-8], eax
    mov rcx, r12
    mov edx, 16
    mov r8d, 178
    lea r9, [ts_hud_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    mov eax, [ui_panel]
    cmp eax, 1
    je .panel_inventory
    cmp eax, 2
    je .panel_skill
    cmp eax, 3
    je .panel_quest
    cmp eax, 4
    je .panel_stats
    jmp .panel_done

.panel_inventory:
    cmp dword [item_count], 0
    jle .panel_done
    lea rcx, [ts_hud_buffer]
    lea rdx, [ts_panel_item_fmt]
    lea r8, [items]
    mov r9d, [inventory_counts]
    call wsprintfA
    jmp .panel_draw

.panel_skill:
    cmp dword [skill_count], 0
    jle .panel_done
    lea rcx, [ts_hud_buffer]
    lea rdx, [ts_panel_skill_fmt]
    lea r8, [skills]
    mov r9d, [skills+32]
    mov eax, [skills+36]
    mov [rsp+32], rax
    call wsprintfA
    jmp .panel_draw

.panel_quest:
    cmp dword [quest_count], 0
    jle .panel_done
    lea rcx, [ts_hud_buffer]
    lea rdx, [ts_panel_quest_fmt]
    lea r8, [quests]
    mov r9d, [quest_state]
    mov eax, [quest_progress]
    mov [rsp+32], rax
    call wsprintfA
    jmp .panel_draw

.panel_stats:
    call equipment_attack_bonus
    add eax, [player_base_attack]
    lea rcx, [ts_hud_buffer]
    lea rdx, [ts_panel_stats_fmt]
    mov r8d, eax
    mov r9d, [player_defense]
    mov eax, [equipped_weapon]
    mov [rsp+32], rax
    mov eax, [equipped_armor]
    mov [rsp+40], rax
    call wsprintfA

.panel_draw:
    mov [rbp-8], eax
    mov rcx, r12
    mov edx, 16
    mov r8d, 202
    lea r9, [ts_hud_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

.panel_done:
    mov r12, [rbp-40]
    leave
    ret
