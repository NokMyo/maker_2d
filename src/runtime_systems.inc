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
    ts_choice_hint        db "Choice: [1] option A   [2] option B",0
    ts_choice_hint_len    equ $-ts_choice_hint-1

section .bss
    project_header_v2     resb PROJECT_HEADER_SIZE
    project_meta          resb PROJECT_META_SIZE
    database_header       resb DATABASE_HEADER_SIZE
    save_header           resb SAVE_HEADER_SIZE
    save_staging          resb SAVE_HEADER_SIZE+(MAX_QUESTS*2+MAX_VARIABLES+MAX_ITEMS)*4
    initial_max_hp        resd 1
    initial_base_attack   resd 1

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
    class_defs            resb MAX_CLASSES*CLASS_SIZE

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
    class_count           resd 1
    skill_slot_ids        resd 4

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
    player_bonus_hp       resd 1
    mp_regen_tick         resd 1
    game_tick             resd 1
    dialog_text_ptr       resq 1
    choice_active         resd 1
    choice_event_a        resd 1
    choice_event_b        resd 1
    ts_bitmap_info        resb 32
    ts_hud_buffer         resb 160

section .text

ts_rt_read_block:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp-4], r8d
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call ReadFile
    test eax, eax
    jz .done
    mov eax, [rbp-4]
    cmp [io_bytes], eax
    sete al
    movzx eax, al
.done:
    leave
    ret

ts_rt_write_block:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp-4], r8d
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call WriteFile
    test eax, eax
    jz .done
    mov eax, [rbp-4]
    cmp [io_bytes], eax
    sete al
    movzx eax, al
.done:
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
    mov eax, [database_header+32]
    cmp eax, MAX_CLASSES
    ja .close_fail
    mov [class_count], eax

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

    mov eax, [class_count]
    imul eax, CLASS_SIZE
    mov rcx, [rbp-16]
    lea rdx, [class_defs]
    mov r8d, eax
    call ts_rt_read_block
    test eax, eax
    jz .close_fail

    call validate_project_data
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
; Apply the project's selected starting class/job.
; project_flags stores the class id in TSRP v3.
; ------------------------------------------------------------
apply_player_class:
    mov dword [skill_slot_ids+0], 0
    mov dword [skill_slot_ids+4], 1
    mov dword [skill_slot_ids+8], 2
    mov dword [skill_slot_ids+12], 3

    cmp dword [class_count], 0
    jle .done
    mov eax, [project_flags]
    cmp eax, 0
    jl .done
    cmp eax, [class_count]
    jae .done
    imul eax, CLASS_SIZE
    lea r10, [class_defs+rax]

    mov eax, [r10+32]
    cmp eax, 1
    jl .hp_done
    mov [player_max_hp], eax
.hp_done:
    mov eax, [r10+36]
    cmp eax, 0
    jl .mp_done
    mov [player_max_mp], eax
.mp_done:
    mov eax, [r10+40]
    cmp eax, 1
    jl .atk_done
    mov [player_base_attack], eax
.atk_done:
    mov eax, [r10+44]
    cmp eax, 1
    jl .move_done
    mov [player_move_speed], eax
.move_done:
    mov eax, [r10+48]
    cmp eax, -1
    jge .jump_done
    mov [player_jump_speed], eax
.jump_done:
    mov eax, [r10+52]
    mov [skill_slot_ids+0], eax
    mov eax, [r10+56]
    mov [skill_slot_ids+4], eax
    mov eax, [r10+60]
    mov [skill_slot_ids+8], eax
    mov eax, [r10+64]
    mov [skill_slot_ids+12], eax
.done:
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
    mov [initial_max_hp], eax
    mov [player_hp], eax
    mov eax, [player_base_attack]
    mov [initial_base_attack], eax
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
    mov dword [player_bonus_hp], 0
    mov dword [mp_regen_tick], 0
    mov dword [game_tick], 0
    mov qword [dialog_text_ptr], 0
    mov dword [choice_active], 0

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
; Audio subsystem. BGM uses MCI so short SE can use PlaySound.
; ------------------------------------------------------------
runtime_start_audio:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    cmp dword [project_meta+108], 0
    je .done
    lea rcx, [bgm_open_cmd]
    xor edx, edx
    xor r8d, r8d
    xor r9d, r9d
    call mciSendStringA
    lea rcx, [bgm_play_cmd]
    xor edx, edx
    xor r8d, r8d
    xor r9d, r9d
    call mciSendStringA
.done:
    leave
    ret

runtime_stop_audio:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    cmp dword [project_meta+108], 0
    je .done
    lea rcx, [bgm_close_cmd]
    xor edx, edx
    xor r8d, r8d
    xor r9d, r9d
    call mciSendStringA
.done:
    leave
    ret

runtime_play_se:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    cmp dword [project_meta+108], 0
    je .done
    lea rcx, [se_path]
    xor edx, edx
    mov r8d, 020003h
    call PlaySoundA
.done:
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
    cmp dword [choice_active], 0
    je .normal_keys

    mov ecx, 031h
    call GetAsyncKeyState
    test ax, 8000h
    jz .choice_two
    mov ecx, [choice_event_a]
    mov dword [choice_active], 0
    call run_event
    mov dword [system_key_cooldown], 20
    jmp .done
.choice_two:
    mov ecx, 032h
    call GetAsyncKeyState
    test ax, 8000h
    jz .done
    mov ecx, [choice_event_b]
    mov dword [choice_active], 0
    call run_event
    mov dword [system_key_cooldown], 20
    jmp .done

.normal_keys:
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
    mov ecx, [project_meta+84]    ; configured item key
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
    push rbp
    mov rbp, rsp
    sub rsp, 32
    cmp dword [item_count], 0
    jle .done
    cmp dword [items+32], ITEM_CONSUMABLE
    jne .done
    cmp dword [items+36], 0
    jle .done
    cmp dword [inventory_counts], 0
    jle .done
    call get_effective_max_hp
    mov edx, eax
    mov eax, [player_hp]
    cmp eax, edx
    jge .done
    dec dword [inventory_counts]
    mov eax, [items+36]
    add [player_hp], eax
    call get_effective_max_hp
    cmp [player_hp], eax
    jle .done
    mov [player_hp], eax
.done:
    leave
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
    mov eax, [equipped_weapon]
    mov [save_header+56], eax
    mov eax, [equipped_armor]
    mov [save_header+60], eax

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
    lea rdx, [save_staging]
    mov r8d, SAVE_HEADER_SIZE+(MAX_QUESTS*2+MAX_VARIABLES+MAX_ITEMS)*4
    call ts_rt_read_block
    mov [rbp-12], eax
    mov rcx, [rbp-8]
    call CloseHandle
    cmp dword [rbp-12], 0
    je .fail

    mov rax, SAVE_MAGIC_QWORD
    cmp [save_staging], rax
    jne .fail
    cmp dword [save_staging+8], SAVE_VERSION
    jne .fail
    mov eax, [save_staging+12]
    cmp eax, [map_count]
    jae .fail
    mov eax, [save_staging+44]
    cmp eax, [quest_count]
    jne .fail
    mov eax, [save_staging+48]
    cmp eax, [variable_count]
    jne .fail
    mov eax, [save_staging+52]
    cmp eax, [item_count]
    jne .fail
    cmp dword [save_staging+36], 1
    jl .fail
    cmp dword [save_staging+36], 10000
    jg .fail
    cmp dword [save_staging+24], 1
    jl .fail
    cmp dword [save_staging+28], 0
    jl .fail
    mov eax, [save_staging+28]
    cmp eax, [player_max_mp]
    jg .fail
    cmp dword [save_staging+32], 0
    jl .fail
    cmp dword [save_staging+40], 0
    jl .fail

    xor r10d, r10d
.quest_check:
    cmp r10d, MAX_QUESTS
    jae .inventory_start
    cmp dword [save_staging+SAVE_HEADER_SIZE+r10*4], QUEST_REWARDED
    ja .fail
    cmp dword [save_staging+SAVE_HEADER_SIZE+MAX_QUESTS*4+r10*4], 0
    jl .fail
    inc r10d
    jmp .quest_check
.inventory_start:
    xor r10d, r10d
.inventory_check:
    cmp r10d, MAX_ITEMS
    jae .gear
    cmp dword [save_staging+SAVE_HEADER_SIZE+(MAX_QUESTS*2+MAX_VARIABLES)*4+r10*4], 0
    jl .fail
    inc r10d
    jmp .inventory_check
.gear:
    mov eax, [save_staging+56]
    mov edx, ITEM_WEAPON
    call .validate_gear
    test eax, eax
    jz .fail
    mov eax, [save_staging+60]
    mov edx, ITEM_ARMOR
    call .validate_gear
    test eax, eax
    jz .fail

    ; All validation precedes mutation. Reconstruct level growth from baseline.
    mov eax, [save_staging+36]
    dec eax
    mov ecx, eax
    add ecx, [initial_base_attack]
    jo .fail
    imul eax, 10
    add eax, [initial_max_hp]
    jo .fail
    mov [player_max_hp], eax
    mov [player_base_attack], ecx
    mov eax, [save_staging+12]
    mov [active_map], eax
    call initialize_player
    xor r10d, r10d
.copy:
    mov eax, [save_staging+SAVE_HEADER_SIZE+r10*4]
    mov [quest_state+r10*4], eax
    inc r10d
    cmp r10d, MAX_QUESTS*2+MAX_VARIABLES+MAX_ITEMS
    jb .copy
    mov eax, [save_staging+16]
    mov [player_x], eax
    mov eax, [save_staging+20]
    mov [player_y], eax
    mov eax, [save_staging+24]
    mov [player_hp], eax
    mov eax, [save_staging+28]
    mov [player_mp], eax
    mov eax, [save_staging+32]
    mov [player_exp], eax
    mov eax, [save_staging+36]
    mov [player_level], eax
    mov eax, [save_staging+40]
    mov [player_money], eax
    mov eax, [save_staging+56]
    mov [equipped_weapon], eax
    mov eax, [save_staging+60]
    mov [equipped_armor], eax
    call equip_recalculate
    call get_effective_max_hp
    cmp [player_hp], eax
    jle .hp_ok
    mov [player_hp], eax
.hp_ok:
    mov dword [camera_x], 0
    mov dword [player_grounded], 0
    mov dword [attack_ticks], 0
    mov dword [attack_cooldown], 0
    mov dword [choice_active], 0
    mov dword [climbing], 0
    mov qword [dialog_text_ptr], 0
    mov eax, 1
    leave
    ret
.fail:
    xor eax, eax
    leave
    ret

; Leaf validation against staged inventory, before modifying live equipment.
.validate_gear:
    cmp eax, -1
    je .gear_valid
    cmp eax, [item_count]
    jae .gear_bad
    cmp dword [save_staging+SAVE_HEADER_SIZE+(MAX_QUESTS*2+MAX_VARIABLES)*4+rax*4], 0
    jle .gear_bad
    imul eax, ITEM_SIZE
    cmp [items+rax+32], edx
    jne .gear_bad
.gear_valid:
    mov eax, 1
    ret
.gear_bad:
    xor eax, eax
    ret

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

    mov eax, ecx
    imul eax, QUEST_SIZE
    lea rax, [quests+rax]
    mov [dialog_text_ptr], rax

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

    cmp dword [r9+32], QUEST_TALK
    jne .talk_progress_done
    mov eax, [rbp-4]
    mov edx, [r9+40]
    mov [quest_progress+rax*4], edx
.talk_progress_done:

    mov edx, [r9+40]
    mov eax, [rbp-4]
    cmp [quest_progress+rax*4], edx
    jl .done

    mov dword [r8+rax*4], QUEST_REWARDED
    mov ecx, [r9+44]
    add [player_exp], ecx
    call check_level_up
    mov eax, [rbp-4]
    imul eax, QUEST_SIZE
    lea r9, [quests+rax]
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
    call check_level_up
    mov eax, [rbp-4]
    imul eax, MONSTER_SIZE
    lea r8, [monster_defs+rax]
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
    test r12d, r12d
    jle .done
    cmp r12d, EVENT_COMMANDS
    jbe .count_ok
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
    cmp eax, EVENT_IF_VAR_EQ
    je .if_var_eq
    cmp eax, EVENT_CHOICE
    je .choice
    jmp .next

.dialogue:
    lea rax, [r13+176]
    cmp byte [rax], 0
    jne .dialogue_text_ready
    mov rax, r13
.dialogue_text_ready:
    mov [dialog_text_ptr], rax
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
    mov [rbp-8], ecx
    mov ecx, eax
    call add_item
    mov ecx, [rbp-8]
    jmp .next

.add_money:
    mov eax, [r8+4]
    add [player_money], eax
    jmp .next

.map:
    mov eax, [r8+4]
    mov [rbp-8], ecx
    mov ecx, eax
    call change_map
    mov ecx, [rbp-8]
    jmp .next

.quest:
    mov eax, [r8+4]
    cmp eax, 0
    jl .next
    cmp eax, [quest_count]
    jae .next
    mov dword [quest_state+rax*4], QUEST_ACTIVE
    jmp .next

.if_var_eq:
    mov eax, [r8+4]
    cmp eax, 0
    jl .next
    cmp eax, [variable_count]
    jae .next
    mov edx, [r8+8]
    cmp [variable_values+rax*4], edx
    je .next
    mov eax, [r8+12]
    test eax, eax
    js .done
    cmp eax, EVENT_COMMANDS
    ja .done
    add ecx, eax
    jmp .next

.choice:
    mov eax, [r8+4]
    mov [choice_event_a], eax
    mov eax, [r8+8]
    mov [choice_event_b], eax
    mov dword [choice_active], 1
    mov [dialog_text_ptr], r13
    mov dword [dialog_ticks], 600
    jmp .done

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
    push rbp
    mov rbp, rsp
    sub rsp, 32
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
    call equip_recalculate
    leave
    ret

equip_recalculate:
    mov dword [player_defense], 0
    mov dword [player_bonus_hp], 0
    mov eax, [equipped_armor]
    cmp eax, 0
    jl .done
    cmp eax, [item_count]
    jae .done
    imul eax, ITEM_SIZE
    mov edx, [items+rax+52]
    mov [player_defense], edx
    mov edx, [items+rax+56]
    mov [player_bonus_hp], edx
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
; Draw active map background sprite stretched to the client area.
; rcx = HDC, eax = 1 drawn / 0 fallback
; ------------------------------------------------------------
runtime_draw_background:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp-8], rcx
    mov eax, [active_map]
    imul eax, MAP_SIZE
    mov edx, [maps+rax+48]
    cmp edx, 0
    jl .none
    mov rcx, [rbp-8]
    call gdip_draw_fullscreen
    leave
    ret
.none:
    xor eax, eax
    leave
    ret

runtime_draw_foreground:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp-8], rcx
    mov eax, [active_map]
    imul eax, MAP_SIZE
    mov edx, [maps+rax+56]
    cmp edx, 0
    jl .none
    mov rcx, [rbp-8]
    call gdip_draw_fullscreen
    leave
    ret
.none:
    xor eax, eax
    leave
    ret

runtime_draw_tiles:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp-8], rcx
    mov eax, [active_map]
    imul eax, MAP_SIZE
    mov edx, [maps+rax+60]
    cmp edx, 0
    jl .none
    mov rcx, [rbp-8]
    call gdip_draw_tiled
    leave
    ret
.none:
    xor eax, eax
    leave
    ret

; ------------------------------------------------------------
; GDI+ sprite loader. ecx = sprite id, rax = GpImage* or 0.
; ------------------------------------------------------------
gdip_open_sprite:
    push rbp
    mov rbp, rsp
    sub rsp, 64

    cmp ecx, 0
    jl .fail
    cmp ecx, [sprite_count]
    jae .fail

    mov eax, ecx
    imul eax, SPRITE_SIZE
    lea r10, [sprites+rax]
    cmp byte [r10+32], 0
    je .fail

    xor ecx, ecx
    xor edx, edx
    lea r8, [r10+32]
    mov r9d, -1
    lea rax, [gdip_wide_path]
    mov [rsp+32], rax
    mov qword [rsp+40], 260
    call MultiByteToWideChar
    test eax, eax
    jz .fail

    mov qword [gdip_image], 0
    lea rcx, [gdip_wide_path]
    lea rdx, [gdip_image]
    call GdipLoadImageFromFile
    test eax, eax
    jnz .fail

    mov rax, [gdip_image]
    leave
    ret
.fail:
    xor eax, eax
    leave
    ret

; ------------------------------------------------------------
; rcx = HDC, edx = sprite id
; ------------------------------------------------------------
gdip_draw_fullscreen:
    push rbp
    mov rbp, rsp
    sub rsp, 160
    mov [rbp-40], r12
    mov [rbp-48], r13
    mov r12, rcx
    mov ecx, edx
    call gdip_open_sprite
    test rax, rax
    jz .fail
    mov r13, rax

    mov rcx, r13
    lea rdx, [gdip_width]
    call GdipGetImageWidth
    test eax, eax
    jnz .dispose
    mov rcx, r13
    lea rdx, [gdip_height]
    call GdipGetImageHeight
    test eax, eax
    jnz .dispose

    mov qword [gdip_graphics], 0
    mov rcx, r12
    lea rdx, [gdip_graphics]
    call GdipCreateFromHDC
    test eax, eax
    jnz .dispose

    mov rcx, [gdip_graphics]
    mov rdx, r13
    xor r8d, r8d
    xor r9d, r9d
    mov eax, [client_rect+8]
    mov [rsp+32], rax
    mov eax, [client_rect+12]
    mov [rsp+40], rax
    mov qword [rsp+48], 0
    mov qword [rsp+56], 0
    mov eax, [gdip_width]
    mov [rsp+64], rax
    mov eax, [gdip_height]
    mov [rsp+72], rax
    mov qword [rsp+80], 2
    mov qword [rsp+88], 0
    mov qword [rsp+96], 0
    mov qword [rsp+104], 0
    call GdipDrawImageRectRectI

    mov rcx, [gdip_graphics]
    call GdipDeleteGraphics
    mov rcx, r13
    call GdipDisposeImage
    mov eax, 1
    jmp .done
.dispose:
    mov rcx, r13
    call GdipDisposeImage
.fail:
    xor eax, eax
.done:
    mov r12, [rbp-40]
    mov r13, [rbp-48]
    leave
    ret

; ------------------------------------------------------------
; rcx = HDC, edx = sprite id. Repeat source image over client.
; ------------------------------------------------------------
gdip_draw_tiled:
    push rbp
    mov rbp, rsp
    sub rsp, 176
    mov [rbp-40], r12
    mov [rbp-48], r13
    mov [rbp-56], r14
    mov [rbp-64], r15
    mov r12, rcx
    mov ecx, edx
    call gdip_open_sprite
    test rax, rax
    jz .fail
    mov r13, rax

    mov rcx, r13
    lea rdx, [gdip_width]
    call GdipGetImageWidth
    test eax, eax
    jnz .dispose
    mov rcx, r13
    lea rdx, [gdip_height]
    call GdipGetImageHeight
    test eax, eax
    jnz .dispose
    cmp dword [gdip_width], 0
    jle .dispose
    cmp dword [gdip_height], 0
    jle .dispose

    mov qword [gdip_graphics], 0
    mov rcx, r12
    lea rdx, [gdip_graphics]
    call GdipCreateFromHDC
    test eax, eax
    jnz .dispose

    xor r15d, r15d
.yloop:
    cmp r15d, [client_rect+12]
    jge .draw_done
    xor r14d, r14d
.xloop:
    cmp r14d, [client_rect+8]
    jge .ynext

    mov rcx, [gdip_graphics]
    mov rdx, r13
    mov r8d, r14d
    mov r9d, r15d
    mov eax, [gdip_width]
    mov [rsp+32], rax
    mov eax, [gdip_height]
    mov [rsp+40], rax
    mov qword [rsp+48], 0
    mov qword [rsp+56], 0
    mov eax, [gdip_width]
    mov [rsp+64], rax
    mov eax, [gdip_height]
    mov [rsp+72], rax
    mov qword [rsp+80], 2
    mov qword [rsp+88], 0
    mov qword [rsp+96], 0
    mov qword [rsp+104], 0
    call GdipDrawImageRectRectI

    add r14d, [gdip_width]
    jmp .xloop
.ynext:
    add r15d, [gdip_height]
    jmp .yloop

.draw_done:
    mov rcx, [gdip_graphics]
    call GdipDeleteGraphics
    mov rcx, r13
    call GdipDisposeImage
    mov eax, 1
    jmp .done
.dispose:
    mov rcx, r13
    call GdipDisposeImage
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
; Minimal runtime minimap.
; rcx = HDC
; ------------------------------------------------------------
runtime_draw_minimap:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    mov [rbp-48], r12
    mov [rbp-56], r13
    mov r12, rcx

    cmp dword [project_meta+72], 0
    je .disabled

    ; Panel background in top-right.
    mov eax, [client_rect+8]
    sub eax, 230
    mov [temp_rect+0], eax
    mov dword [temp_rect+4], 16
    mov eax, [client_rect+8]
    sub eax, 16
    mov [temp_rect+8], eax
    mov dword [temp_rect+12], 136
    mov ecx, [col_dialog]
    call CreateSolidBrush
    mov [rbp-8], rax
    mov rcx, r12
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-8]
    call DeleteObject

    mov ecx, PS_SOLID
    mov edx, 2
    mov r8d, [col_platform]
    call CreatePen
    mov [rbp-8], rax
    mov rcx, r12
    mov rdx, rax
    call SelectObject
    mov [rbp-16], rax

    xor r13d, r13d
.platform_loop:
    cmp r13d, [platform_count]
    jae .platform_done
    mov eax, [active_map]
    cmp dword [platform_map_ids+r13*4], eax
    jne .platform_next

    mov eax, r13d
    imul eax, PLATFORM_SIZE
    lea r10, [platforms+rax]

    mov eax, [r10+0]
    sar eax, 3
    mov edx, [client_rect+8]
    sub edx, 220
    add edx, eax
    mov eax, [r10+4]
    sar eax, 3
    add eax, 20
    mov r8d, eax
    mov rcx, r12
    xor r9d, r9d
    call MoveToEx

    mov eax, r13d
    imul eax, PLATFORM_SIZE
    lea r10, [platforms+rax]
    mov eax, [r10+8]
    sar eax, 3
    mov edx, [client_rect+8]
    sub edx, 220
    add edx, eax
    mov eax, [r10+12]
    sar eax, 3
    add eax, 20
    mov r8d, eax
    mov rcx, r12
    call LineTo

.platform_next:
    inc r13d
    jmp .platform_loop

.platform_done:
    mov rcx, r12
    mov rdx, [rbp-16]
    call SelectObject
    mov rcx, [rbp-8]
    call DeleteObject

    ; Player dot.
    mov eax, [player_x]
    sar eax, 3
    mov edx, [client_rect+8]
    sub edx, 220
    add eax, edx
    mov [temp_rect+0], eax
    add eax, 6
    mov [temp_rect+8], eax
    mov eax, [player_y]
    sar eax, 3
    add eax, 20
    mov [temp_rect+4], eax
    add eax, 6
    mov [temp_rect+12], eax

    mov ecx, [col_player]
    call CreateSolidBrush
    mov [rbp-8], rax
    mov rcx, r12
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-8]
    call DeleteObject

    mov r12, [rbp-48]
    mov r13, [rbp-56]
    leave
    ret
.disabled:
    mov r12, [rbp-48]
    mov r13, [rbp-56]
    leave
    ret

; ------------------------------------------------------------
; Draw one frame from an imported BMP sprite sheet.
; rcx=HDC, edx=animation id, r8d=x, r9d=y
; eax=1 drawn, 0 fallback
; ------------------------------------------------------------
draw_animation:
    push rbp
    mov rbp, rsp
    sub rsp, 176
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

    mov ecx, [r13+68]
    call gdip_open_sprite
    test rax, rax
    jz .fail
    mov r14, rax

    mov rcx, r14
    lea rdx, [gdip_width]
    call GdipGetImageWidth
    test eax, eax
    jnz .dispose
    mov rcx, r14
    lea rdx, [gdip_height]
    call GdipGetImageHeight
    test eax, eax
    jnz .dispose

    mov ecx, [r13+32]
    cmp ecx, 1
    jge .frames_ok
    mov ecx, 1
.frames_ok:
    mov eax, [gdip_width]
    xor edx, edx
    div ecx
    cmp eax, 1
    jge .fw_ok
    mov eax, 1
.fw_ok:
    mov [rbp-16], eax

    mov eax, [r13+36]
    add eax, 15
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
    jge .frame_mod
    mov ecx, 1
.frame_mod:
    xor edx, edx
    div ecx
    mov eax, edx
    imul eax, [rbp-16]
    mov [rbp-20], eax

    mov qword [gdip_graphics], 0
    mov rcx, r12
    lea rdx, [gdip_graphics]
    call GdipCreateFromHDC
    test eax, eax
    jnz .dispose

    mov rcx, [gdip_graphics]
    mov rdx, r14
    mov r8d, [rbp-8]
    mov r9d, [rbp-12]
    mov eax, [rbp-16]
    mov [rsp+32], rax
    mov eax, [gdip_height]
    mov [rsp+40], rax
    mov eax, [rbp-20]
    mov [rsp+48], rax
    mov qword [rsp+56], 0
    mov eax, [rbp-16]
    mov [rsp+64], rax
    mov eax, [gdip_height]
    mov [rsp+72], rax
    mov qword [rsp+80], 2
    mov qword [rsp+88], 0
    mov qword [rsp+96], 0
    mov qword [rsp+104], 0
    call GdipDrawImageRectRectI

    mov rcx, [gdip_graphics]
    call GdipDeleteGraphics
    mov rcx, r14
    call GdipDisposeImage
    mov eax, 1
    jmp .done
.dispose:
    mov rcx, r14
    call GdipDisposeImage
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
; Effective maximum HP -> eax
; ------------------------------------------------------------
get_effective_max_hp:
    mov eax, [player_max_hp]
    add eax, [player_bonus_hp]
    cmp eax, 1
    jge .done
    mov eax, 1
.done:
    ret

; ------------------------------------------------------------
; Basic level curve: next level costs level*100 EXP.
; Carries excess EXP, +1 base attack, +10 base max HP per level.
; ------------------------------------------------------------
check_level_up:
    push rbp
    mov rbp, rsp
    sub rsp, 48
.loop:
    mov eax, [player_level]
    cmp eax, 1
    jl .done
    cmp eax, 10000
    jge .done
    imul eax, 100
    cmp [player_exp], eax
    jl .done

    sub [player_exp], eax
    inc dword [player_level]
    inc dword [player_base_attack]
    add dword [player_max_hp], 10
    call get_effective_max_hp
    mov [player_hp], eax
    mov eax, [player_max_mp]
    mov [player_mp], eax
    jmp .loop
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

    call get_effective_max_hp
    mov [rbp-16], eax
    lea rcx, [ts_hud_buffer]
    lea rdx, [ts_hud_fmt1]
    mov r8d, [player_hp]
    mov r9d, [rbp-16]
    mov eax, [player_mp]
    mov [rsp+32], rax
    mov eax, [player_max_mp]
    mov [rsp+40], rax
    call wsprintfA
    mov [rbp-8], eax
    mov rcx, r12
    mov edx, [project_meta+64]
    mov r8d, [project_meta+68]
    add r8d, 24
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
    mov edx, [project_meta+64]
    mov r8d, [project_meta+68]
    add r8d, 46
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
    mov edx, [project_meta+64]
    mov r8d, [project_meta+68]
    add r8d, 68
    lea r9, [ts_hud_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    mov rcx, r12
    mov edx, [project_meta+64]
    mov r8d, [project_meta+68]
    add r8d, 90
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
    mov edx, [project_meta+64]
    mov r8d, [project_meta+68]
    add r8d, 112
    lea r9, [ts_hud_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    cmp dword [choice_active], 0
    je .no_choice_hint
    mov rcx, r12
    mov edx, [project_meta+64]
    mov r8d, [project_meta+68]
    add r8d, 158
    lea r9, [ts_choice_hint]
    mov qword [rsp+32], ts_choice_hint_len
    call TextOutA
.no_choice_hint:

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
    mov edx, [project_meta+64]
    mov r8d, [project_meta+68]
    add r8d, 136
    lea r9, [ts_hud_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

.panel_done:
    mov r12, [rbp-40]
    leave
    ret


%include "validation.inc"
