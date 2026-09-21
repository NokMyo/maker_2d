; Tsuramechoki editor project/database systems
; Included by editor.asm. 100% x86-64 NASM.

section .data
    ts_name_map          db "Map_%03d",0
    ts_name_animation    db "Animation",0
    ts_name_skill        db "Skill",0
    ts_name_monster      db "Monster",0
    ts_name_item         db "Item",0
    ts_name_quest        db "Quest",0
    ts_name_variable     db "Variable",0
    ts_name_event        db "Event",0

    ts_default_map       db "Map_001",0
    ts_default_idle      db "PlayerIdle",0
    ts_default_attack    db "PlayerAttack",0
    ts_default_monster_anim db "MonsterIdle",0
    ts_default_skill     db "Basic Attack",0
    ts_default_monster   db "Slime",0
    ts_default_item      db "Potion",0
    ts_default_quest     db "First Hunt",0
    ts_default_var       db "StoryFlag",0

    ts_build_dir         db "Build",0
    ts_runtime_src       db "TsuramechokiRuntime.exe",0
    ts_game_dst          db "Build\Game.exe",0
    ts_project_dst       db "Build\project.tsrp",0

    ts_fmt_map           db "Map %d / %d",0
    ts_fmt_mode          db "Editor mode %d   field %d",0
    ts_fmt_value         db "Value: %d",0
    ts_help1             db "F1 Project  F2 Player  F6 DB cycle",0
    ts_help1_len         equ $-ts_help1-1
    ts_help2             db "TAB field  +/- edit  INS add record",0
    ts_help2_len         equ $-ts_help2-1
    ts_help3             db "PgUp/PgDn map  Ctrl+M new map  F9 build",0
    ts_help3_len         equ $-ts_help3-1

section .bss
    project_header_v2    resb PROJECT_HEADER_SIZE
    database_header      resb DATABASE_HEADER_SIZE

    platform_map_ids     resd MAX_PLATFORMS
    entity_map_ids       resd MAX_ENTITIES

    maps                 resb MAX_MAPS*MAP_SIZE
    animations           resb MAX_ANIMATIONS*ANIMATION_SIZE
    skills               resb MAX_SKILLS*SKILL_SIZE
    monster_defs         resb MAX_MONSTERS*MONSTER_SIZE
    items                resb MAX_ITEMS*ITEM_SIZE
    quests               resb MAX_QUESTS*QUEST_SIZE
    variables            resb MAX_VARIABLES*VARIABLE_SIZE
    events               resb MAX_EVENTS*EVENT_SIZE

    map_count            resd 1
    active_map           resd 1
    screen_width         resd 1
    screen_height        resd 1
    player_move_speed    resd 1
    player_jump_speed    resd 1
    player_max_hp        resd 1
    player_max_mp        resd 1
    player_base_attack   resd 1
    project_flags        resd 1

    animation_count      resd 1
    skill_count          resd 1
    monster_count        resd 1
    item_count           resd 1
    quest_count          resd 1
    variable_count       resd 1
    event_count          resd 1

    editor_panel_mode    resd 1
    property_index       resd 1
    database_index       resd 1
    ts_text_buffer       resb 128

section .text

; ------------------------------------------------------------
; init_project_systems
; ------------------------------------------------------------
init_project_systems:
    push rbp
    mov rbp, rsp
    sub rsp, 64

    mov dword [map_count], 1
    mov dword [active_map], 0
    mov dword [screen_width], 1280
    mov dword [screen_height], 720
    mov dword [player_move_speed], 5
    mov dword [player_jump_speed], -14
    mov dword [player_max_hp], 100
    mov dword [player_max_mp], 100
    mov dword [player_base_attack], 10
    mov dword [project_flags], 0
    mov dword [editor_panel_mode], 0
    mov dword [property_index], 0
    mov dword [database_index], 0

    lea rcx, [maps]
    lea rdx, [ts_default_map]
    call lstrcpyA
    mov dword [maps+32], 001D1A18h
    mov dword [maps+36], 100
    mov dword [maps+40], 430

    mov dword [animation_count], 3
    lea rcx, [animations]
    lea rdx, [ts_default_idle]
    call lstrcpyA
    mov dword [animations+32], 4
    mov dword [animations+36], 120
    mov dword [animations+64], 1

    lea rcx, [animations+ANIMATION_SIZE]
    lea rdx, [ts_default_attack]
    call lstrcpyA
    mov dword [animations+ANIMATION_SIZE+32], 6
    mov dword [animations+ANIMATION_SIZE+36], 80
    mov dword [animations+ANIMATION_SIZE+40], 2
    mov dword [animations+ANIMATION_SIZE+44], 4
    mov dword [animations+ANIMATION_SIZE+48], 26
    mov dword [animations+ANIMATION_SIZE+52], 4
    mov dword [animations+ANIMATION_SIZE+56], 70
    mov dword [animations+ANIMATION_SIZE+60], 42

    lea rcx, [animations+ANIMATION_SIZE*2]
    lea rdx, [ts_default_monster_anim]
    call lstrcpyA
    mov dword [animations+ANIMATION_SIZE*2+32], 4
    mov dword [animations+ANIMATION_SIZE*2+36], 140
    mov dword [animations+ANIMATION_SIZE*2+64], 1

    mov dword [skill_count], 1
    lea rcx, [skills]
    lea rdx, [ts_default_skill]
    call lstrcpyA
    mov dword [skills+32], 10
    mov dword [skills+36], 0
    mov dword [skills+40], 18
    mov dword [skills+44], 70
    mov dword [skills+48], 1
    mov dword [skills+52], 28
    mov dword [skills+56], 1

    mov dword [monster_count], 1
    lea rcx, [monster_defs]
    lea rdx, [ts_default_monster]
    call lstrcpyA
    mov dword [monster_defs+32], 30
    mov dword [monster_defs+36], 10
    mov dword [monster_defs+40], 1
    mov dword [monster_defs+44], 10
    mov dword [monster_defs+48], 0
    mov dword [monster_defs+52], 1
    mov dword [monster_defs+56], AI_CHASE
    mov dword [monster_defs+60], 2

    mov dword [item_count], 1
    lea rcx, [items]
    lea rdx, [ts_default_item]
    call lstrcpyA
    mov dword [items+32], ITEM_CONSUMABLE
    mov dword [items+36], 30
    mov dword [items+40], 50
    mov dword [items+44], 10

    mov dword [quest_count], 1
    lea rcx, [quests]
    lea rdx, [ts_default_quest]
    call lstrcpyA
    mov dword [quests+32], QUEST_KILL
    mov dword [quests+36], 0
    mov dword [quests+40], 3
    mov dword [quests+44], 100
    mov dword [quests+48], 50
    mov dword [quests+52], 0
    mov dword [quests+56], 2

    mov dword [variable_count], 1
    lea rcx, [variables]
    lea rdx, [ts_default_var]
    call lstrcpyA
    mov dword [variables+32], 0

    mov dword [event_count], 1
    lea rcx, [events]
    lea rdx, [ts_name_event]
    call lstrcpyA
    mov dword [events+32], 1
    mov dword [events+48], EVENT_DIALOGUE

    leave
    ret

; ------------------------------------------------------------
; write/read helpers
; rcx handle, rdx buffer, r8d bytes
; ------------------------------------------------------------
ts_write_block:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call WriteFile
    leave
    ret

ts_read_block:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    lea r9, [io_bytes]
    mov qword [rsp+32], 0
    call ReadFile
    leave
    ret

; ------------------------------------------------------------
; save_project — TSRP v2
; ------------------------------------------------------------
save_project:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    mov [rbp-8], r12

    lea r12, [project_header_v2]
    mov rax, PROJECT_MAGIC_QWORD
    mov [r12+0], rax
    mov dword [r12+8], PROJECT_VERSION
    mov dword [r12+12], PROJECT_HEADER_SIZE
    mov eax, [platform_count]
    mov [r12+16], eax
    mov eax, [entity_count]
    mov [r12+20], eax
    mov eax, [map_count]
    mov [r12+24], eax
    mov eax, [active_map]
    mov [r12+28], eax
    mov eax, [screen_width]
    mov [r12+32], eax
    mov eax, [screen_height]
    mov [r12+36], eax
    mov eax, [player_move_speed]
    mov [r12+40], eax
    mov eax, [player_jump_speed]
    mov [r12+44], eax
    mov eax, [player_max_hp]
    mov [r12+48], eax
    mov eax, [player_max_mp]
    mov [r12+52], eax
    mov eax, [player_base_attack]
    mov [r12+56], eax
    mov eax, [project_flags]
    mov [r12+60], eax

    mov eax, [animation_count]
    mov [database_header+0], eax
    mov eax, [skill_count]
    mov [database_header+4], eax
    mov eax, [monster_count]
    mov [database_header+8], eax
    mov eax, [item_count]
    mov [database_header+12], eax
    mov eax, [quest_count]
    mov [database_header+16], eax
    mov eax, [variable_count]
    mov [database_header+20], eax
    mov eax, [event_count]
    mov [database_header+24], eax
    mov dword [database_header+28], 0

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
    mov [rbp-16], rax

    mov rcx, rax
    lea rdx, [project_header_v2]
    mov r8d, PROJECT_HEADER_SIZE
    call ts_write_block
    test eax, eax
    jz .close_fail

    mov eax, [platform_count]
    imul eax, PLATFORM_SIZE
    test eax, eax
    jz .entities
    mov rcx, [rbp-16]
    lea rdx, [platforms]
    mov r8d, eax
    call ts_write_block
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
    call ts_write_block
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
    call ts_write_block
    test eax, eax
    jz .close_fail

.entity_maps:
    mov eax, [entity_count]
    shl eax, 2
    test eax, eax
    jz .map_records
    mov rcx, [rbp-16]
    lea rdx, [entity_map_ids]
    mov r8d, eax
    call ts_write_block
    test eax, eax
    jz .close_fail

.map_records:
    mov eax, [map_count]
    imul eax, MAP_SIZE
    mov rcx, [rbp-16]
    lea rdx, [maps]
    mov r8d, eax
    call ts_write_block
    test eax, eax
    jz .close_fail

    mov rcx, [rbp-16]
    lea rdx, [database_header]
    mov r8d, DATABASE_HEADER_SIZE
    call ts_write_block
    test eax, eax
    jz .close_fail

    mov eax, [animation_count]
    imul eax, ANIMATION_SIZE
    mov rcx, [rbp-16]
    lea rdx, [animations]
    mov r8d, eax
    call ts_write_block
    test eax, eax
    jz .close_fail

    mov eax, [skill_count]
    imul eax, SKILL_SIZE
    mov rcx, [rbp-16]
    lea rdx, [skills]
    mov r8d, eax
    call ts_write_block
    test eax, eax
    jz .close_fail

    mov eax, [monster_count]
    imul eax, MONSTER_SIZE
    mov rcx, [rbp-16]
    lea rdx, [monster_defs]
    mov r8d, eax
    call ts_write_block
    test eax, eax
    jz .close_fail

    mov eax, [item_count]
    imul eax, ITEM_SIZE
    mov rcx, [rbp-16]
    lea rdx, [items]
    mov r8d, eax
    call ts_write_block
    test eax, eax
    jz .close_fail

    mov eax, [quest_count]
    imul eax, QUEST_SIZE
    mov rcx, [rbp-16]
    lea rdx, [quests]
    mov r8d, eax
    call ts_write_block
    test eax, eax
    jz .close_fail

    mov eax, [variable_count]
    imul eax, VARIABLE_SIZE
    mov rcx, [rbp-16]
    lea rdx, [variables]
    mov r8d, eax
    call ts_write_block
    test eax, eax
    jz .close_fail

    mov eax, [event_count]
    imul eax, EVENT_SIZE
    mov rcx, [rbp-16]
    lea rdx, [events]
    mov r8d, eax
    call ts_write_block
    test eax, eax
    jz .close_fail

.success:
    mov rcx, [rbp-16]
    call CloseHandle
    mov eax, 1
    jmp .done

.close_fail:
    mov rcx, [rbp-16]
    call CloseHandle
.fail:
    xor eax, eax
.done:
    mov r12, [rbp-8]
    leave
    ret

; ------------------------------------------------------------
; load_project — TSRP v2
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
    call ts_read_block
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
    cmp eax, MAX_MAPS
    ja .close_fail
    test eax, eax
    jz .close_fail
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
    jz .read_entities
    mov rcx, [rbp-16]
    lea rdx, [platforms]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

.read_entities:
    mov eax, [entity_count]
    imul eax, ENTITY_SIZE
    test eax, eax
    jz .read_platform_maps
    mov rcx, [rbp-16]
    lea rdx, [entities]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

.read_platform_maps:
    mov eax, [platform_count]
    shl eax, 2
    test eax, eax
    jz .read_entity_maps
    mov rcx, [rbp-16]
    lea rdx, [platform_map_ids]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

.read_entity_maps:
    mov eax, [entity_count]
    shl eax, 2
    test eax, eax
    jz .read_maps
    mov rcx, [rbp-16]
    lea rdx, [entity_map_ids]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

.read_maps:
    mov eax, [map_count]
    imul eax, MAP_SIZE
    mov rcx, [rbp-16]
    lea rdx, [maps]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

    mov rcx, [rbp-16]
    lea rdx, [database_header]
    mov r8d, DATABASE_HEADER_SIZE
    call ts_read_block
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

    mov eax, [animation_count]
    imul eax, ANIMATION_SIZE
    mov rcx, [rbp-16]
    lea rdx, [animations]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

    mov eax, [skill_count]
    imul eax, SKILL_SIZE
    mov rcx, [rbp-16]
    lea rdx, [skills]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

    mov eax, [monster_count]
    imul eax, MONSTER_SIZE
    mov rcx, [rbp-16]
    lea rdx, [monster_defs]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

    mov eax, [item_count]
    imul eax, ITEM_SIZE
    mov rcx, [rbp-16]
    lea rdx, [items]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

    mov eax, [quest_count]
    imul eax, QUEST_SIZE
    mov rcx, [rbp-16]
    lea rdx, [quests]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

    mov eax, [variable_count]
    imul eax, VARIABLE_SIZE
    mov rcx, [rbp-16]
    lea rdx, [variables]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

    mov eax, [event_count]
    imul eax, EVENT_SIZE
    mov rcx, [rbp-16]
    lea rdx, [events]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

    mov rcx, [rbp-16]
    call CloseHandle
    mov dword [selected_platform], -1
    mov dword [selected_entity], -1
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
; add/switch maps
; ------------------------------------------------------------
ts_add_map:
    push rbp
    mov rbp, rsp
    sub rsp, 64

    mov eax, [map_count]
    cmp eax, MAX_MAPS
    jae .done
    mov [active_map], eax

    mov ecx, eax
    imul ecx, MAP_SIZE
    lea rcx, [maps+rcx]
    lea rdx, [ts_name_map]
    mov r8d, eax
    inc r8d
    call wsprintfA

    mov eax, [active_map]
    imul eax, MAP_SIZE
    lea r10, [maps+rax]
    mov dword [r10+32], 001D1A18h
    mov dword [r10+36], 100
    mov dword [r10+40], 430

    inc dword [map_count]
    mov dword [editor_camera_x], 0
    mov dword [selected_platform], -1
    mov dword [selected_entity], -1
.done:
    leave
    ret

ts_next_map:
    mov eax, [active_map]
    inc eax
    cmp eax, [map_count]
    jb .set
    xor eax, eax
.set:
    mov [active_map], eax
    mov dword [editor_camera_x], 0
    mov dword [selected_platform], -1
    mov dword [selected_entity], -1
    ret

ts_prev_map:
    mov eax, [active_map]
    test eax, eax
    jnz .dec
    mov eax, [map_count]
    dec eax
    jmp .set
.dec:
    dec eax
.set:
    mov [active_map], eax
    mov dword [editor_camera_x], 0
    mov dword [selected_platform], -1
    mov dword [selected_entity], -1
    ret

; ------------------------------------------------------------
; system_keydown
; ecx = VK. eax=1 handled, 0 not handled.
; ------------------------------------------------------------
system_keydown:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov [rbp-4], ecx

    cmp ecx, 070h                 ; F1
    je .project
    cmp ecx, 071h                 ; F2
    je .player
    cmp ecx, 075h                 ; F6
    je .database
    cmp ecx, 078h                 ; F9
    je .build
    cmp ecx, 021h                 ; PgUp
    je .prev_map
    cmp ecx, 022h                 ; PgDn
    je .next_map
    cmp ecx, 009h                 ; Tab
    je .next_field
    cmp ecx, 02Dh                 ; Insert
    je .insert
    cmp ecx, 0BBh                 ; OEM+
    je .plus
    cmp ecx, 06Bh                 ; numpad+
    je .plus
    cmp ecx, 0BDh                 ; OEM-
    je .minus
    cmp ecx, 06Dh                 ; numpad-
    je .minus
    cmp ecx, 01Bh                 ; Esc -> canvas
    je .canvas
    cmp ecx, 04Dh                 ; M
    jne .not_handled

    mov ecx, 011h
    call GetKeyState
    test ax, 8000h
    jz .not_handled
    call ts_add_map
    jmp .handled

.project:
    mov dword [editor_panel_mode], 1
    mov dword [property_index], 0
    jmp .handled
.player:
    mov dword [editor_panel_mode], 2
    mov dword [property_index], 0
    jmp .handled
.database:
    mov eax, [editor_panel_mode]
    cmp eax, 3
    jb .db_first
    cmp eax, 9
    jae .db_first
    inc eax
    mov [editor_panel_mode], eax
    mov dword [property_index], 0
    mov dword [database_index], 0
    jmp .handled
.db_first:
    mov dword [editor_panel_mode], 3
    mov dword [property_index], 0
    mov dword [database_index], 0
    jmp .handled
.build:
    call export_game
    jmp .handled
.prev_map:
    call ts_prev_map
    jmp .handled
.next_map:
    call ts_next_map
    jmp .handled
.next_field:
    cmp dword [editor_panel_mode], 0
    je .not_handled
    inc dword [property_index]
    cmp dword [property_index], 7
    jbe .handled
    mov dword [property_index], 0
    jmp .handled
.insert:
    call ts_add_database_record
    jmp .handled
.plus:
    mov ecx, 1
    call ts_adjust_setting
    jmp .handled
.minus:
    mov ecx, -1
    call ts_adjust_setting
    jmp .handled
.canvas:
    mov dword [editor_panel_mode], 0
    jmp .handled
.not_handled:
    xor eax, eax
    leave
    ret
.handled:
    mov eax, 1
    leave
    ret

; ------------------------------------------------------------
; ts_adjust_setting(ecx = delta)
; ------------------------------------------------------------
ts_adjust_setting:
    mov edx, [editor_panel_mode]
    cmp edx, 0
    je .entity_param
    cmp edx, 1
    je .project
    cmp edx, 2
    je .player
    cmp edx, 3
    je .animation
    cmp edx, 4
    je .skill
    cmp edx, 5
    je .monster
    cmp edx, 6
    je .item
    cmp edx, 7
    je .quest
    cmp edx, 8
    je .variable
    cmp edx, 9
    je .event
    ret

.entity_param:
    mov eax, [selected_entity]
    cmp eax, 0
    jl .done
    cmp eax, [entity_count]
    jae .done
    imul eax, ENTITY_SIZE
    add dword [entities+rax+12], ecx
    ret

.entity_param:
    mov eax, [selected_entity]
    cmp eax, 0
    jl .zero
    cmp eax, [entity_count]
    jae .zero
    imul eax, ENTITY_SIZE
    mov eax, [entities+rax+12]
    ret
.zero:
    xor eax, eax
    ret

.project:
    cmp dword [property_index], 0
    jne .project_h
    imul eax, ecx, 16
    add [screen_width], eax
    cmp dword [screen_width], 320
    jge .done
    mov dword [screen_width], 320
    ret
.project_h:
    imul eax, ecx, 16
    add [screen_height], eax
    cmp dword [screen_height], 240
    jge .done
    mov dword [screen_height], 240
    ret

.player:
    mov eax, [property_index]
    cmp eax, 0
    je .p_speed
    cmp eax, 1
    je .p_jump
    cmp eax, 2
    je .p_hp
    cmp eax, 3
    je .p_mp
    cmp eax, 4
    je .p_attack
    ret
.p_speed:
    add [player_move_speed], ecx
    cmp dword [player_move_speed], 1
    jge .done
    mov dword [player_move_speed], 1
    ret
.p_jump:
    sub [player_jump_speed], ecx
    cmp dword [player_jump_speed], -4
    jle .done
    mov dword [player_jump_speed], -4
    ret
.p_hp:
    imul eax, ecx, 10
    add [player_max_hp], eax
    cmp dword [player_max_hp], 10
    jge .done
    mov dword [player_max_hp], 10
    ret
.p_mp:
    imul eax, ecx, 10
    add [player_max_mp], eax
    cmp dword [player_max_mp], 0
    jge .done
    mov dword [player_max_mp], 0
    ret
.p_attack:
    add [player_base_attack], ecx
    cmp dword [player_base_attack], 1
    jge .done
    mov dword [player_base_attack], 1
    ret

.animation:
    mov eax, [database_index]
    cmp eax, [animation_count]
    jae .done
    imul eax, ANIMATION_SIZE
    lea r8, [animations+rax]
    mov eax, [property_index]
    imul eax, 4
    add eax, 32
    add dword [r8+rax], ecx
    ret

.skill:
    mov eax, [database_index]
    cmp eax, [skill_count]
    jae .done
    imul eax, SKILL_SIZE
    lea r8, [skills+rax]
    jmp .record_field
.monster:
    mov eax, [database_index]
    cmp eax, [monster_count]
    jae .done
    imul eax, MONSTER_SIZE
    lea r8, [monster_defs+rax]
    jmp .record_field
.item:
    mov eax, [database_index]
    cmp eax, [item_count]
    jae .done
    imul eax, ITEM_SIZE
    lea r8, [items+rax]
    jmp .record_field
.quest:
    mov eax, [database_index]
    cmp eax, [quest_count]
    jae .done
    imul eax, QUEST_SIZE
    lea r8, [quests+rax]
.record_field:
    mov eax, [property_index]
    imul eax, 4
    add eax, 32
    add dword [r8+rax], ecx
    ret
.variable:
    mov eax, [database_index]
    cmp eax, [variable_count]
    jae .done
    imul eax, VARIABLE_SIZE
    lea r8, [variables+rax]
    add dword [r8+32], ecx
    ret
.event:
    mov eax, [database_index]
    cmp eax, [event_count]
    jae .done
    imul eax, EVENT_SIZE
    lea r8, [events+rax]
    mov eax, [property_index]
    cmp eax, 0
    jne .event_command
    add dword [r8+32], ecx
    cmp dword [r8+32], 0
    jge .event_max
    mov dword [r8+32], 0
.event_max:
    cmp dword [r8+32], EVENT_COMMANDS
    jle .done
    mov dword [r8+32], EVENT_COMMANDS
    ret
.event_command:
    ; fields 1..4 edit first command opcode/a/b/c
    dec eax
    imul eax, 4
    add eax, 48
    add dword [r8+rax], ecx
.done:
    ret

; ------------------------------------------------------------
; get selected setting value -> eax
; ------------------------------------------------------------
ts_get_setting_value:
    mov edx, [editor_panel_mode]
    cmp edx, 0
    je .entity_param
    cmp edx, 1
    je .project
    cmp edx, 2
    je .player
    cmp edx, 3
    je .animation
    cmp edx, 4
    je .skill
    cmp edx, 5
    je .monster
    cmp edx, 6
    je .item
    cmp edx, 7
    je .quest
    cmp edx, 8
    je .variable
    cmp edx, 9
    je .event
    xor eax, eax
    ret
.project:
    cmp dword [property_index], 0
    jne .ph
    mov eax, [screen_width]
    ret
.ph:
    mov eax, [screen_height]
    ret
.player:
    mov eax, [property_index]
    cmp eax, 0
    je .ps
    cmp eax, 1
    je .pj
    cmp eax, 2
    je .php
    cmp eax, 3
    je .pmp
    cmp eax, 4
    je .pa
    xor eax, eax
    ret
.ps: mov eax, [player_move_speed]
    ret
.pj: mov eax, [player_jump_speed]
    ret
.php: mov eax, [player_max_hp]
    ret
.pmp: mov eax, [player_max_mp]
    ret
.pa: mov eax, [player_base_attack]
    ret
.animation:
    mov eax, [database_index]
    imul eax, ANIMATION_SIZE
    lea r8, [animations+rax]
    jmp .field
.skill:
    mov eax, [database_index]
    imul eax, SKILL_SIZE
    lea r8, [skills+rax]
    jmp .field
.monster:
    mov eax, [database_index]
    imul eax, MONSTER_SIZE
    lea r8, [monster_defs+rax]
    jmp .field
.item:
    mov eax, [database_index]
    imul eax, ITEM_SIZE
    lea r8, [items+rax]
    jmp .field
.quest:
    mov eax, [database_index]
    imul eax, QUEST_SIZE
    lea r8, [quests+rax]
.field:
    mov eax, [property_index]
    imul eax, 4
    add eax, 32
    mov eax, [r8+rax]
    ret
.variable:
    mov eax, [database_index]
    imul eax, VARIABLE_SIZE
    mov eax, [variables+rax+32]
    ret
.event:
    mov eax, [database_index]
    imul eax, EVENT_SIZE
    lea r8, [events+rax]
    mov eax, [property_index]
    cmp eax, 0
    jne .event_field
    mov eax, [r8+32]
    ret
.event_field:
    dec eax
    imul eax, 4
    add eax, 48
    mov eax, [r8+rax]
    ret

; ------------------------------------------------------------
; add a generic DB record in current DB mode
; ------------------------------------------------------------
ts_add_database_record:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov eax, [editor_panel_mode]
    cmp eax, 3
    je .animation
    cmp eax, 4
    je .skill
    cmp eax, 5
    je .monster
    cmp eax, 6
    je .item
    cmp eax, 7
    je .quest
    cmp eax, 8
    je .variable
    cmp eax, 9
    je .event
    jmp .done
.animation:
    mov eax, [animation_count]
    cmp eax, MAX_ANIMATIONS
    jae .done
    mov [database_index], eax
    imul eax, ANIMATION_SIZE
    lea rcx, [animations+rax]
    lea rdx, [ts_name_animation]
    call lstrcpyA
    mov eax, [database_index]
    imul eax, ANIMATION_SIZE
    mov dword [animations+rax+32], 1
    mov dword [animations+rax+36], 100
    inc dword [animation_count]
    jmp .done
.skill:
    mov eax, [skill_count]
    cmp eax, MAX_SKILLS
    jae .done
    mov [database_index], eax
    imul eax, SKILL_SIZE
    lea rcx, [skills+rax]
    lea rdx, [ts_name_skill]
    call lstrcpyA
    inc dword [skill_count]
    jmp .done
.monster:
    mov eax, [monster_count]
    cmp eax, MAX_MONSTERS
    jae .done
    mov [database_index], eax
    imul eax, MONSTER_SIZE
    lea rcx, [monster_defs+rax]
    lea rdx, [ts_name_monster]
    call lstrcpyA
    mov eax, [database_index]
    imul eax, MONSTER_SIZE
    mov dword [monster_defs+rax+32], 10
    mov dword [monster_defs+rax+36], 5
    mov dword [monster_defs+rax+40], 1
    mov dword [monster_defs+rax+56], AI_CHASE
    inc dword [monster_count]
    jmp .done
.item:
    mov eax, [item_count]
    cmp eax, MAX_ITEMS
    jae .done
    mov [database_index], eax
    imul eax, ITEM_SIZE
    lea rcx, [items+rax]
    lea rdx, [ts_name_item]
    call lstrcpyA
    inc dword [item_count]
    jmp .done
.quest:
    mov eax, [quest_count]
    cmp eax, MAX_QUESTS
    jae .done
    mov [database_index], eax
    imul eax, QUEST_SIZE
    lea rcx, [quests+rax]
    lea rdx, [ts_name_quest]
    call lstrcpyA
    mov eax, [database_index]
    imul eax, QUEST_SIZE
    mov dword [quests+rax+40], 1
    inc dword [quest_count]
    jmp .done
.variable:
    mov eax, [variable_count]
    cmp eax, MAX_VARIABLES
    jae .done
    mov [database_index], eax
    imul eax, VARIABLE_SIZE
    lea rcx, [variables+rax]
    lea rdx, [ts_name_variable]
    call lstrcpyA
    inc dword [variable_count]
    jmp .done
.event:
    mov eax, [event_count]
    cmp eax, MAX_EVENTS
    jae .done
    mov [database_index], eax
    imul eax, EVENT_SIZE
    lea rcx, [events+rax]
    lea rdx, [ts_name_event]
    call lstrcpyA
    mov eax, [database_index]
    imul eax, EVENT_SIZE
    mov dword [events+rax+32], 1
    mov dword [events+rax+48], EVENT_DIALOGUE
    inc dword [event_count]
.done:
    leave
    ret

; ------------------------------------------------------------
; system_draw_panel(rcx = HDC)
; ------------------------------------------------------------
system_draw_panel:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    mov [rbp-40], r12
    mov r12, rcx

    mov rcx, r12
    mov edx, [col_text]
    call SetTextColor
    mov rcx, r12
    mov edx, TRANSPARENT
    call SetBkMode

    lea rcx, [ts_text_buffer]
    lea rdx, [ts_fmt_map]
    mov r8d, [active_map]
    inc r8d
    mov r9d, [map_count]
    call wsprintfA
    mov [rbp-8], eax

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 205
    lea r9, [ts_text_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    lea rcx, [ts_text_buffer]
    lea rdx, [ts_fmt_mode]
    mov r8d, [editor_panel_mode]
    mov r9d, [property_index]
    call wsprintfA
    mov [rbp-8], eax

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 230
    lea r9, [ts_text_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    call ts_get_setting_value
    mov r8d, eax
    lea rcx, [ts_text_buffer]
    lea rdx, [ts_fmt_value]
    call wsprintfA
    mov [rbp-8], eax

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 255
    lea r9, [ts_text_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 300
    lea r9, [ts_help1]
    mov qword [rsp+32], ts_help1_len
    call TextOutA

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 325
    lea r9, [ts_help2]
    mov qword [rsp+32], ts_help2_len
    call TextOutA

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 350
    lea r9, [ts_help3]
    mov qword [rsp+32], ts_help3_len
    call TextOutA

    mov r12, [rbp-40]
    leave
    ret

; ------------------------------------------------------------
; export_game
; Build\Game.exe + Build\project.tsrp
; ------------------------------------------------------------
export_game:
    push rbp
    mov rbp, rsp
    sub rsp, 64

    call save_project

    lea rcx, [ts_build_dir]
    xor edx, edx
    call CreateDirectoryA

    lea rcx, [ts_runtime_src]
    lea rdx, [ts_game_dst]
    xor r8d, r8d
    call CopyFileA

    lea rcx, [project_path]
    lea rdx, [ts_project_dst]
    xor r8d, r8d
    call CopyFileA

    leave
    ret
