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
    ts_name_sprite       db "Sprite",0
    ts_name_class        db "Class",0
    ts_import_title      db "Import PNG/BMP Sprite Sheet",0
    ts_bmp_filter        db "Images (*.png;*.bmp)",0,"*.png;*.bmp",0,"All Files",0,"*.*",0,0
    ts_audio_filter      db "Wave Audio (*.wav)",0,"*.wav",0,"All Files",0,"*.*",0,0
    ts_audio_title_bgm   db "Import BGM WAV",0
    ts_audio_title_se    db "Import SE WAV",0
    ts_bgm_asset        db "Assets\BGM.wav",0
    ts_se_asset         db "Assets\SE.wav",0
    ts_build_bgm        db "Build\Assets\BGM.wav",0
    ts_build_se         db "Build\Assets\SE.wav",0
    ts_assets_dir        db "Assets",0
    ts_build_assets_dir  db "Build\Assets",0
    ts_sprite_path_fmt   db "Assets\Sprite%03d.asset",0
    ts_build_asset_fmt   db "Build\%s",0

    ts_default_project   db "My Tsuramechoki Game",0
    ts_default_exe       db "Game.exe",0
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
    ts_runtime_filename  db "TsuramechokiRuntime.exe",0
    ts_game_dst          db "Build\Game.exe",0
    ts_project_dst       db "Build\project.tsrp",0
    ts_build_exe_fmt     db "Build\%s",0
    ts_icon_filter       db "Windows Icon (*.ico)",0,"*.ico",0,"All Files",0,"*.*",0,0
    ts_icon_title        db "Import Game Icon",0
    ts_icon_asset        db "Assets\Game.ico",0
    ts_build_icon        db "Build\Assets\Game.ico",0
    ts_build_cwd         db "Build",0

    ts_fmt_map           db "Map %d / %d",0
    ts_fmt_mode          db "Editor mode %d   field %d",0
    ts_fmt_value         db "Value: %d",0
    ts_anim_fmt          db "Frames %d  Active %d-%d  Sprite %d",0
    ts_event_fmt         db "Cmd %d/%d  OP %d  A %d  B %d  C %d",0
    ts_ui_fmt            db "HUD (%d,%d)  Minimap %d",0
    ts_build_fmt         db "Build status %d  (0 idle / 1 built / 2 running)",0
    ts_project_fmt       db "Project: %s",0
    ts_rename_hint       db "F8 project name / Shift+F8 EXE name / Ctrl+F7 icon",0
    ts_rename_hint_len   equ $-ts_rename_hint-1
    ts_help1             db "F1 Project  F2 Player  F3 Map  F4 UI  F6 DB",0
    ts_help1_len         equ $-ts_help1-1
    ts_help2             db "TAB field  +/- edit  INS add  F7 image  F11 keys",0
    ts_help2_len         equ $-ts_help2-1
    ts_help3             db "PgUp/PgDn map  Ctrl+M new  F9 build  F10 build+run",0
    ts_help3_len         equ $-ts_help3-1

section .bss
    project_header_v2    resb PROJECT_HEADER_SIZE
    project_meta         resb PROJECT_META_SIZE
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
    sprites              resb MAX_SPRITES*SPRITE_SIZE
    class_defs           resb MAX_CLASSES*CLASS_SIZE

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
    start_map            resd 1
    save_slots           resd 1

    animation_count      resd 1
    skill_count          resd 1
    monster_count        resd 1
    item_count           resd 1
    quest_count          resd 1
    variable_count       resd 1
    event_count          resd 1
    sprite_count         resd 1
    class_count          resd 1
    ts_ofn               resb 160
    ts_file_buffer       resb 264
    ts_asset_path        resb 160
    ts_build_asset_path  resb 192
    ts_build_exe_path    resb 128
    ts_runtime_full_path resb 520

    editor_panel_mode    resd 1
    property_index       resd 1
    database_index       resd 1
    rename_mode          resd 1
    event_command_index  resd 1
    build_status         resd 1
    undo_valid           resd 1
    redo_valid           resd 1
    undo_platform_count  resd 1
    undo_entity_count    resd 1
    redo_platform_count  resd 1
    redo_entity_count    resd 1
    undo_platforms       resb MAX_PLATFORMS*PLATFORM_SIZE
    undo_entities        resb MAX_ENTITIES*ENTITY_SIZE
    undo_platform_maps   resd MAX_PLATFORMS
    undo_entity_maps     resd MAX_ENTITIES
    redo_platforms       resb MAX_PLATFORMS*PLATFORM_SIZE
    redo_entities        resb MAX_ENTITIES*ENTITY_SIZE
    redo_platform_maps   resd MAX_PLATFORMS
    redo_entity_maps     resd MAX_ENTITIES
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
    mov dword [start_map], 0
    mov dword [save_slots], 3
    lea rcx, [project_meta]
    lea rdx, [ts_default_project]
    call lstrcpyA
    mov dword [project_meta+32], 0
    mov dword [project_meta+36], 3
    mov dword [project_meta+40], 0
    mov dword [project_meta+44], 0
    mov dword [project_meta+48], 0
    mov dword [project_meta+52], 0
    mov dword [project_meta+56], -1
    mov dword [project_meta+60], -1
    mov dword [project_meta+64], 16
    mov dword [project_meta+68], 66
    mov dword [project_meta+72], 1
    mov dword [project_meta+76], 058h
    mov dword [project_meta+80], 045h
    mov dword [project_meta+84], 043h
    mov dword [project_meta+88], 020h
    mov dword [project_meta+92], 05Ah
    mov dword [project_meta+96], 052h
    mov dword [project_meta+100], 046h
    mov dword [project_meta+104], 0
    mov dword [project_meta+108], 1
    lea rcx, [project_meta+112]
    lea rdx, [ts_default_exe]
    call lstrcpyA
    mov byte [project_meta+160], 0
    mov dword [editor_panel_mode], 0
    mov dword [property_index], 0
    mov dword [database_index], 0
    mov dword [rename_mode], 0
    mov dword [event_command_index], 0
    mov dword [build_status], 0
    mov dword [undo_valid], 0
    mov dword [redo_valid], 0

    lea rcx, [maps]
    lea rdx, [ts_default_map]
    call lstrcpyA
    mov dword [maps+32], 001D1A18h
    mov dword [maps+36], 100
    mov dword [maps+40], 430
    mov dword [maps+44], -1
    mov dword [maps+48], -1
    mov dword [maps+52], 0
    mov dword [maps+56], -1
    mov dword [maps+60], -1

    mov dword [animation_count], 3
    lea rcx, [animations]
    lea rdx, [ts_default_idle]
    call lstrcpyA
    mov dword [animations+32], 4
    mov dword [animations+36], 120
    mov dword [animations+64], 1
    mov dword [animations+68], -1

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
    mov dword [animations+ANIMATION_SIZE+68], -1

    lea rcx, [animations+ANIMATION_SIZE*2]
    lea rdx, [ts_default_monster_anim]
    call lstrcpyA
    mov dword [animations+ANIMATION_SIZE*2+32], 4
    mov dword [animations+ANIMATION_SIZE*2+36], 140
    mov dword [animations+ANIMATION_SIZE*2+64], 1
    mov dword [animations+ANIMATION_SIZE*2+68], -1

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
    mov dword [monster_defs+64], 50
    mov dword [monster_defs+68], 1
    mov dword [monster_defs+72], 5
    mov dword [monster_defs+76], -1

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

    mov dword [class_count], 1
    lea rcx, [class_defs]
    lea rdx, [ts_name_class]
    call lstrcpyA
    mov dword [class_defs+32], 100
    mov dword [class_defs+36], 100
    mov dword [class_defs+40], 10
    mov dword [class_defs+44], 5
    mov dword [class_defs+48], -14
    mov dword [class_defs+52], 0
    mov dword [class_defs+56], 1
    mov dword [class_defs+60], 2
    mov dword [class_defs+64], 3
    mov dword [class_defs+68], 0

    mov dword [sprite_count], 0
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

    mov eax, [start_map]
    mov [project_meta+32], eax
    mov eax, [save_slots]
    mov [project_meta+36], eax

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
    mov eax, [sprite_count]
    mov [database_header+28], eax
    mov eax, [class_count]
    mov [database_header+32], eax
    mov dword [database_header+36], 0

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

    mov rcx, [rbp-16]
    lea rdx, [project_meta]
    mov r8d, PROJECT_META_SIZE
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

    mov eax, [sprite_count]
    imul eax, SPRITE_SIZE
    mov rcx, [rbp-16]
    lea rdx, [sprites]
    mov r8d, eax
    call ts_write_block
    test eax, eax
    jz .close_fail

    mov eax, [class_count]
    imul eax, CLASS_SIZE
    mov rcx, [rbp-16]
    lea rdx, [class_defs]
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

    mov rcx, [rbp-16]
    lea rdx, [project_meta]
    mov r8d, PROJECT_META_SIZE
    call ts_read_block
    test eax, eax
    jz .close_fail

    mov eax, [project_meta+32]
    cmp eax, [map_count]
    jae .close_fail
    mov [start_map], eax
    mov eax, [project_meta+36]
    cmp eax, 1
    jl .close_fail
    cmp eax, 3
    jg .close_fail
    mov [save_slots], eax

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

    mov eax, [sprite_count]
    imul eax, SPRITE_SIZE
    mov rcx, [rbp-16]
    lea rdx, [sprites]
    mov r8d, eax
    call ts_read_block
    test eax, eax
    jz .close_fail

    mov eax, [class_count]
    imul eax, CLASS_SIZE
    mov rcx, [rbp-16]
    lea rdx, [class_defs]
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
    mov dword [r10+44], -1
    mov dword [r10+48], -1
    mov dword [r10+52], 0
    mov dword [r10+56], -1
    mov dword [r10+60], -1
    mov dword [r10+44], -1
    mov dword [r10+48], -1
    mov dword [r10+52], 0

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
    cmp ecx, 072h                 ; F3
    je .map_settings
    cmp ecx, 073h                 ; F4
    je .ui_settings
    cmp ecx, 075h                 ; F6
    je .database
    cmp ecx, 076h                 ; F7 sprite import
    je .import_sprite
    cmp ecx, 077h                 ; F8 rename project
    je .rename
    cmp ecx, 078h                 ; F9
    je .build
    cmp ecx, 079h                 ; F10
    je .build_run
    cmp ecx, 07Ah                 ; F11
    je .control_settings
    cmp ecx, 07Bh                 ; F12
    je .import_audio
    cmp ecx, 021h                 ; PgUp
    je .prev_map
    cmp ecx, 022h                 ; PgDn
    je .next_map
    cmp ecx, 026h                 ; Up
    je .event_prev_command
    cmp ecx, 028h                 ; Down
    je .event_next_command
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
.map_settings:
    mov dword [editor_panel_mode], 10
    mov dword [property_index], 0
    jmp .handled
.ui_settings:
    mov dword [editor_panel_mode], 11
    mov dword [property_index], 0
    jmp .handled
.control_settings:
    mov dword [editor_panel_mode], 12
    mov dword [property_index], 0
    jmp .handled
.import_audio:
    call ts_import_audio
    jmp .handled
.database:
    mov eax, [editor_panel_mode]
    cmp eax, 13
    je .db_first
    cmp eax, 9
    je .db_class
    cmp eax, 3
    jb .db_first
    cmp eax, 9
    ja .db_first
    inc eax
    mov [editor_panel_mode], eax
    mov dword [property_index], 0
    mov dword [database_index], 0
    mov dword [event_command_index], 0
    jmp .handled
.db_class:
    mov dword [editor_panel_mode], 13
    mov dword [property_index], 0
    mov dword [database_index], 0
    jmp .handled
.db_first:
    mov dword [editor_panel_mode], 3
    mov dword [property_index], 0
    mov dword [database_index], 0
    mov dword [event_command_index], 0
    jmp .handled
.import_sprite:
    mov ecx, 011h
    call GetKeyState
    test ax, 8000h
    jz .sprite_normal
    call ts_import_icon
    jmp .handled
.sprite_normal:
    call ts_import_sprite
    jmp .handled
.rename:
    mov ecx, 010h
    call GetKeyState
    test ax, 8000h
    jz .rename_project
    mov dword [rename_mode], 2
    jmp .handled
.rename_project:
    mov dword [rename_mode], 1
    jmp .handled
.build:
    call export_game
    jmp .handled
.build_run:
    call export_game
    test eax, eax
    jz .handled
    call launch_built_game
    jmp .handled
.prev_map:
    call ts_prev_map
    jmp .handled
.next_map:
    call ts_next_map
    jmp .handled
.event_prev_command:
    cmp dword [editor_panel_mode], 9
    jne .not_handled
    cmp dword [event_command_index], 0
    jle .handled
    dec dword [event_command_index]
    jmp .handled
.event_next_command:
    cmp dword [editor_panel_mode], 9
    jne .not_handled
    mov eax, [database_index]
    cmp eax, [event_count]
    jae .handled
    imul eax, EVENT_SIZE
    mov ecx, [events+rax+32]
    test ecx, ecx
    jle .handled
    dec ecx
    cmp [event_command_index], ecx
    jge .handled
    inc dword [event_command_index]
    jmp .handled
.next_field:
    cmp dword [editor_panel_mode], 0
    je .not_handled
    inc dword [property_index]
    mov eax, 7
    cmp dword [editor_panel_mode], 3
    jne .nf_event
    mov eax, 9
    jmp .nf_check
.nf_event:
    cmp dword [editor_panel_mode], 13
    jne .nf_monster
    mov eax, 9
    jmp .nf_check
.nf_monster:
    cmp dword [editor_panel_mode], 5
    jne .nf_event_real
    mov eax, 11
    jmp .nf_check
.nf_event_real:
    cmp dword [editor_panel_mode], 9
    jne .nf_ui
    mov eax, 4
    jmp .nf_check
.nf_ui:
    cmp dword [editor_panel_mode], 11
    jne .nf_controls
    mov eax, 2
    jmp .nf_check
.nf_controls:
    cmp dword [editor_panel_mode], 12
    jne .nf_map
    mov eax, 6
    jmp .nf_check
.nf_map:
    cmp dword [editor_panel_mode], 10
    jne .nf_check
    mov eax, 6
.nf_check:
    cmp [property_index], eax
    jle .handled
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
; system_char_input(ecx = WM_CHAR character)
; eax=1 handled, 0 ignored
; ------------------------------------------------------------
system_char_input:
    cmp dword [rename_mode], 0
    je .ignored

    cmp dword [rename_mode], 2
    je .exe_target
    lea r10, [project_meta]
    mov dword [ts_text_buffer+120], 31
    jmp .target_ready
.exe_target:
    lea r10, [project_meta+112]
    mov dword [ts_text_buffer+120], 47
.target_ready:
    cmp ecx, 13
    je .finish
    cmp ecx, 8
    je .backspace
    cmp ecx, 32
    jl .handled
    cmp ecx, 126
    jg .handled

    mov [ts_text_buffer], cl
    mov byte [ts_text_buffer+1], 0
    mov rcx, r10
    call lstrlenA
    cmp eax, [ts_text_buffer+120]
    jae .handled
    lea rdx, [r10+rax]
    mov cl, [ts_text_buffer]
    mov [rdx], cl
    mov byte [rdx+1], 0
    jmp .handled

.backspace:
    mov rcx, r10
    call lstrlenA
    test eax, eax
    jz .handled
    dec eax
    mov byte [r10+rax], 0
    jmp .handled

.finish:
    cmp byte [r10], 0
    jne .finish_ok
    cmp dword [rename_mode], 2
    je .restore_exe
    lea rcx, [project_meta]
    lea rdx, [ts_default_project]
    call lstrcpyA
    jmp .finish_ok
.restore_exe:
    lea rcx, [project_meta+112]
    lea rdx, [ts_default_exe]
    call lstrcpyA
.finish_ok:
    mov dword [rename_mode], 0
.handled:
    mov eax, 1
    ret
.ignored:
    xor eax, eax
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
    cmp edx, 10
    je .map
    cmp edx, 11
    je .ui
    cmp edx, 12
    je .controls
    cmp edx, 13
    je .class
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

.map:
    mov eax, [active_map]
    imul eax, MAP_SIZE
    lea r8, [maps+rax]
    mov eax, [property_index]
    cmp eax, 0
    je .map_color
    cmp eax, 1
    je .map_spawn_x
    cmp eax, 2
    je .map_spawn_y
    cmp eax, 3
    je .map_music
    cmp eax, 4
    je .map_bg_sprite
    cmp eax, 5
    je .map_fg_sprite
    cmp eax, 6
    je .map_tile_sprite
    ret
.map_color:
    imul eax, ecx, 010101h
    add [r8+32], eax
    ret
.map_spawn_x:
    imul eax, ecx, 16
    add [r8+36], eax
    ret
.map_spawn_y:
    imul eax, ecx, 16
    add [r8+40], eax
    ret
.map_music:
    add [r8+44], ecx
    ret
.map_bg_sprite:
    add [r8+48], ecx
    cmp dword [r8+48], -1
    jge .map_bg_upper
    mov dword [r8+48], -1
.map_bg_upper:
    mov eax, [sprite_count]
    dec eax
    cmp [r8+48], eax
    jle .done
    mov [r8+48], eax
    ret
.map_fg_sprite:
    lea r9, [r8+56]
    jmp .map_sprite_common
.map_tile_sprite:
    lea r9, [r8+60]
.map_sprite_common:
    add dword [r9], ecx
    cmp dword [r9], -1
    jge .map_sprite_upper
    mov dword [r9], -1
.map_sprite_upper:
    mov eax, [sprite_count]
    dec eax
    cmp [r9], eax
    jle .done
    mov [r9], eax
    ret

.ui:
    mov eax, [property_index]
    cmp eax, 0
    je .ui_x
    cmp eax, 1
    je .ui_y
    cmp eax, 2
    je .ui_minimap
    ret
.ui_x:
    imul eax, ecx, 4
    add [project_meta+64], eax
    ret
.ui_y:
    imul eax, ecx, 4
    add [project_meta+68], eax
    ret
.ui_minimap:
    xor dword [project_meta+72], 1
    ret

.class:
    mov eax, [database_index]
    cmp eax, [class_count]
    jae .done
    imul eax, CLASS_SIZE
    lea r8, [class_defs+rax]
    mov eax, [property_index]
    imul eax, 4
    add eax, 32
    add dword [r8+rax], ecx
    ret

.controls:
    mov eax, [property_index]
    cmp eax, 0
    je .ctl_skill1
    cmp eax, 1
    je .ctl_interact
    cmp eax, 2
    je .ctl_item
    cmp eax, 3
    je .ctl_jump
    cmp eax, 4
    je .ctl_skill2
    cmp eax, 5
    je .ctl_skill3
    cmp eax, 6
    je .ctl_skill4
    ret
.ctl_skill1: lea r8, [project_meta+76]
    jmp .ctl_common
.ctl_interact: lea r8, [project_meta+80]
    jmp .ctl_common
.ctl_item: lea r8, [project_meta+84]
    jmp .ctl_common
.ctl_jump: lea r8, [project_meta+88]
    jmp .ctl_common
.ctl_skill2: lea r8, [project_meta+92]
    jmp .ctl_common
.ctl_skill3: lea r8, [project_meta+96]
    jmp .ctl_common
.ctl_skill4: lea r8, [project_meta+100]
.ctl_common:
    add dword [r8], ecx
    cmp dword [r8], 1
    jge .ctl_upper
    mov dword [r8], 1
.ctl_upper:
    cmp dword [r8], 0FEh
    jle .done
    mov dword [r8], 0FEh
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
    cmp dword [property_index], 1
    jne .project_start
    imul eax, ecx, 16
    add [screen_height], eax
    cmp dword [screen_height], 240
    jge .done
    mov dword [screen_height], 240
    ret
.project_start:
    cmp dword [property_index], 2
    jne .project_slots
    add [start_map], ecx
    cmp dword [start_map], 0
    jge .start_upper
    mov dword [start_map], 0
.start_upper:
    mov eax, [map_count]
    dec eax
    cmp [start_map], eax
    jle .done
    mov [start_map], eax
    ret
.project_slots:
    cmp dword [property_index], 3
    jne .project_window
    add [save_slots], ecx
    cmp dword [save_slots], 1
    jge .slots_upper
    mov dword [save_slots], 1
.slots_upper:
    cmp dword [save_slots], 3
    jle .done
    mov dword [save_slots], 3
    ret
.project_window:
    cmp dword [property_index], 4
    jne .project_audio
    xor dword [project_meta+104], 1
    ret
.project_audio:
    cmp dword [property_index], 5
    jne .project_class
    xor dword [project_meta+108], 1
    ret
.project_class:
    cmp dword [property_index], 6
    jne .done
    add [project_flags], ecx
    cmp dword [project_flags], 0
    jge .project_class_upper
    mov dword [project_flags], 0
.project_class_upper:
    mov eax, [class_count]
    test eax, eax
    jz .project_class_zero
    dec eax
    cmp [project_flags], eax
    jle .done
    mov [project_flags], eax
    ret
.project_class_zero:
    mov dword [project_flags], 0
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
    cmp eax, 5
    je .p_idle_anim
    cmp eax, 6
    je .p_walk_anim
    cmp eax, 7
    je .p_air_anim
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
.p_idle_anim:
    add [project_meta+44], ecx
    jmp .clamp_player_anim
.p_walk_anim:
    add [project_meta+48], ecx
    jmp .clamp_player_walk
.p_air_anim:
    add [project_meta+52], ecx
    jmp .clamp_player_air
.clamp_player_anim:
    lea r8, [project_meta+44]
    jmp .anim_clamp_common
.clamp_player_walk:
    lea r8, [project_meta+48]
    jmp .anim_clamp_common
.clamp_player_air:
    lea r8, [project_meta+52]
.anim_clamp_common:
    cmp dword [r8], 0
    jge .anim_upper
    mov dword [r8], 0
.anim_upper:
    mov eax, [animation_count]
    test eax, eax
    jz .anim_zero
    dec eax
    cmp [r8], eax
    jle .done
    mov [r8], eax
    ret
.anim_zero:
    mov dword [r8], 0
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
    ; fields 1..4 edit selected command opcode/a/b/c
    dec eax
    imul eax, 4
    mov edx, [event_command_index]
    imul edx, EVENT_COMMAND_SIZE
    add eax, edx
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
    cmp edx, 10
    je .map
    cmp edx, 11
    je .ui
    cmp edx, 12
    je .controls
    cmp edx, 13
    je .class
    xor eax, eax
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
.map:
    mov eax, [active_map]
    imul eax, MAP_SIZE
    lea r8, [maps+rax]
    mov eax, [property_index]
    cmp eax, 0
    je .map_g_color
    cmp eax, 1
    je .map_g_x
    cmp eax, 2
    je .map_g_y
    cmp eax, 3
    je .map_g_music
    cmp eax, 4
    je .map_g_bg
    cmp eax, 5
    je .map_g_fg
    cmp eax, 6
    je .map_g_tile
    xor eax, eax
    ret
.map_g_color:
    mov eax, [r8+32]
    ret
.map_g_x:
    mov eax, [r8+36]
    ret
.map_g_y:
    mov eax, [r8+40]
    ret
.map_g_music:
    mov eax, [r8+44]
    ret
.map_g_bg:
    mov eax, [r8+48]
    ret
.map_g_fg:
    mov eax, [r8+56]
    ret
.map_g_tile:
    mov eax, [r8+60]
    ret
.ui:
    mov eax, [property_index]
    cmp eax, 0
    je .ugx
    cmp eax, 1
    je .ugy
    cmp eax, 2
    je .ugm
    xor eax, eax
    ret
.ugx: mov eax, [project_meta+64]
    ret
.ugy: mov eax, [project_meta+68]
    ret
.ugm: mov eax, [project_meta+72]
    ret
.controls:
    mov eax, [property_index]
    cmp eax, 0
    je .cg1
    cmp eax, 1
    je .cgi
    cmp eax, 2
    je .cgit
    cmp eax, 3
    je .cgj
    cmp eax, 4
    je .cg2
    cmp eax, 5
    je .cg3
    cmp eax, 6
    je .cg4
    xor eax, eax
    ret
.cg1: mov eax, [project_meta+76]
    ret
.cgi: mov eax, [project_meta+80]
    ret
.cgit: mov eax, [project_meta+84]
    ret
.cgj: mov eax, [project_meta+88]
    ret
.cg2: mov eax, [project_meta+92]
    ret
.cg3: mov eax, [project_meta+96]
    ret
.cg4: mov eax, [project_meta+100]
    ret

.project:
    cmp dword [property_index], 0
    jne .ph
    mov eax, [screen_width]
    ret
.ph:
    cmp dword [property_index], 1
    jne .pstart
    mov eax, [screen_height]
    ret
.pstart:
    cmp dword [property_index], 2
    jne .pslots
    mov eax, [start_map]
    ret
.pslots:
    cmp dword [property_index], 3
    jne .pwindow
    mov eax, [save_slots]
    ret
.pwindow:
    cmp dword [property_index], 4
    jne .paudio
    mov eax, [project_meta+104]
    ret
.paudio:
    cmp dword [property_index], 5
    jne .pclass
    mov eax, [project_meta+108]
    ret
.pclass:
    cmp dword [property_index], 6
    jne .pzero
    mov eax, [project_flags]
    ret
.pzero:
    xor eax, eax
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
    cmp eax, 5
    je .pidle
    cmp eax, 6
    je .pwalk
    cmp eax, 7
    je .pair
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
.pidle: mov eax, [project_meta+44]
    ret
.pwalk: mov eax, [project_meta+48]
    ret
.pair: mov eax, [project_meta+52]
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
    mov edx, [event_command_index]
    imul edx, EVENT_COMMAND_SIZE
    add eax, edx
    add eax, 48
    mov eax, [r8+rax]
    ret
.class:
    mov eax, [database_index]
    cmp eax, [class_count]
    jae .zero
    imul eax, CLASS_SIZE
    lea r8, [class_defs+rax]
    mov eax, [property_index]
    imul eax, 4
    add eax, 32
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
    cmp eax, 13
    je .class
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
    mov dword [animations+rax+68], -1
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
    mov dword [monster_defs+rax+64], 50
    mov dword [monster_defs+rax+68], 1
    mov dword [monster_defs+rax+72], 5
    mov dword [monster_defs+rax+76], -1
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
.class:
    mov eax, [class_count]
    cmp eax, MAX_CLASSES
    jae .done
    mov [database_index], eax
    imul eax, CLASS_SIZE
    lea rcx, [class_defs+rax]
    lea rdx, [ts_name_class]
    call lstrcpyA
    mov eax, [database_index]
    imul eax, CLASS_SIZE
    mov dword [class_defs+rax+32], 100
    mov dword [class_defs+rax+36], 100
    mov dword [class_defs+rax+40], 10
    mov dword [class_defs+rax+44], 5
    mov dword [class_defs+rax+48], -14
    mov dword [class_defs+rax+52], 0
    mov dword [class_defs+rax+56], 1
    mov dword [class_defs+rax+60], 2
    mov dword [class_defs+rax+64], 3
    inc dword [class_count]
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
    lea rdx, [ts_project_fmt]
    lea r8, [project_meta]
    call wsprintfA
    mov [rbp-8], eax

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 180
    lea r9, [ts_text_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

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
    mov r8d, 210
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
    mov r8d, 235
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
    mov r8d, 260
    lea r9, [ts_text_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 305
    lea r9, [ts_help1]
    mov qword [rsp+32], ts_help1_len
    call TextOutA

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 330
    lea r9, [ts_help2]
    mov qword [rsp+32], ts_help2_len
    call TextOutA

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 355
    lea r9, [ts_help3]
    mov qword [rsp+32], ts_help3_len
    call TextOutA

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 380
    lea r9, [ts_rename_hint]
    mov qword [rsp+32], ts_rename_hint_len
    call TextOutA

    lea rcx, [ts_text_buffer]
    lea rdx, [ts_build_fmt]
    mov r8d, [build_status]
    call wsprintfA
    mov [rbp-8], eax
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 405
    lea r9, [ts_text_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    mov rcx, r12
    call draw_mode_preview

    mov r12, [rbp-40]
    leave
    ret

; ------------------------------------------------------------
; Editor GDI+ map layer preview.
; ------------------------------------------------------------
editor_gdip_open_sprite:
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
    lea rax, [editor_gdip_wide_path]
    mov [rsp+32], rax
    mov qword [rsp+40], 260
    call MultiByteToWideChar
    test eax, eax
    jz .fail

    mov qword [editor_gdip_image], 0
    lea rcx, [editor_gdip_wide_path]
    lea rdx, [editor_gdip_image]
    call GdipLoadImageFromFile
    test eax, eax
    jnz .fail
    mov rax, [editor_gdip_image]
    leave
    ret
.fail:
    xor eax, eax
    leave
    ret

editor_draw_full_canvas:
    push rbp
    mov rbp, rsp
    sub rsp, 144
    mov [rbp-40], r12
    mov [rbp-48], r13
    mov r12, rcx
    mov ecx, edx
    call editor_gdip_open_sprite
    test rax, rax
    jz .fail
    mov r13, rax

    mov rcx, r13
    lea rdx, [editor_gdip_width]
    call GdipGetImageWidth
    test eax, eax
    jnz .dispose
    mov rcx, r13
    lea rdx, [editor_gdip_height]
    call GdipGetImageHeight
    test eax, eax
    jnz .dispose

    mov qword [editor_gdip_graphics], 0
    mov rcx, r12
    lea rdx, [editor_gdip_graphics]
    call GdipCreateFromHDC
    test eax, eax
    jnz .dispose

    mov rcx, [editor_gdip_graphics]
    mov rdx, r13
    mov r8d, LEFT_PANEL
    mov r9d, TOP_BAR
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    sub eax, LEFT_PANEL
    mov [rsp+32], rax
    mov eax, [client_rect+12]
    sub eax, BOTTOM_BAR
    sub eax, TOP_BAR
    mov [rsp+40], rax
    mov qword [rsp+48], 0
    mov qword [rsp+56], 0
    mov eax, [editor_gdip_width]
    mov [rsp+64], rax
    mov eax, [editor_gdip_height]
    mov [rsp+72], rax
    mov qword [rsp+80], 2
    mov qword [rsp+88], 0
    mov qword [rsp+96], 0
    mov qword [rsp+104], 0
    call GdipDrawImageRectRectI

    mov rcx, [editor_gdip_graphics]
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

editor_draw_tiled_canvas:
    push rbp
    mov rbp, rsp
    sub rsp, 160
    mov [rbp-40], r12
    mov [rbp-48], r13
    mov [rbp-56], r14
    mov [rbp-64], r15
    mov r12, rcx
    mov ecx, edx
    call editor_gdip_open_sprite
    test rax, rax
    jz .fail
    mov r13, rax

    mov rcx, r13
    lea rdx, [editor_gdip_width]
    call GdipGetImageWidth
    test eax, eax
    jnz .dispose
    mov rcx, r13
    lea rdx, [editor_gdip_height]
    call GdipGetImageHeight
    test eax, eax
    jnz .dispose
    cmp dword [editor_gdip_width], 0
    jle .dispose
    cmp dword [editor_gdip_height], 0
    jle .dispose

    mov qword [editor_gdip_graphics], 0
    mov rcx, r12
    lea rdx, [editor_gdip_graphics]
    call GdipCreateFromHDC
    test eax, eax
    jnz .dispose

    mov r15d, TOP_BAR
.yloop:
    mov eax, [client_rect+12]
    sub eax, BOTTOM_BAR
    cmp r15d, eax
    jge .draw_done
    mov r14d, LEFT_PANEL
.xloop:
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    cmp r14d, eax
    jge .ynext

    mov rcx, [editor_gdip_graphics]
    mov rdx, r13
    mov r8d, r14d
    mov r9d, r15d
    mov eax, [editor_gdip_width]
    mov [rsp+32], rax
    mov eax, [editor_gdip_height]
    mov [rsp+40], rax
    mov qword [rsp+48], 0
    mov qword [rsp+56], 0
    mov eax, [editor_gdip_width]
    mov [rsp+64], rax
    mov eax, [editor_gdip_height]
    mov [rsp+72], rax
    mov qword [rsp+80], 2
    mov qword [rsp+88], 0
    mov qword [rsp+96], 0
    mov qword [rsp+104], 0
    call GdipDrawImageRectRectI
    add r14d, [editor_gdip_width]
    jmp .xloop
.ynext:
    add r15d, [editor_gdip_height]
    jmp .yloop

.draw_done:
    mov rcx, [editor_gdip_graphics]
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

editor_draw_background_layers:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp-8], rcx
    mov eax, [active_map]
    imul eax, MAP_SIZE
    mov edx, [maps+rax+48]
    cmp edx, 0
    jl .tiles
    mov rcx, [rbp-8]
    call editor_draw_full_canvas
.tiles:
    mov eax, [active_map]
    imul eax, MAP_SIZE
    mov edx, [maps+rax+60]
    cmp edx, 0
    jl .done
    mov rcx, [rbp-8]
    call editor_draw_tiled_canvas
.done:
    leave
    ret

editor_draw_foreground_layer:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    mov [rbp-8], rcx
    mov eax, [active_map]
    imul eax, MAP_SIZE
    mov edx, [maps+rax+56]
    cmp edx, 0
    jl .done
    mov rcx, [rbp-8]
    call editor_draw_full_canvas
.done:
    leave
    ret

; ------------------------------------------------------------
; Visual previews for animation/event/UI editor modes.
; ------------------------------------------------------------
draw_mode_preview:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    mov [rbp-40], r12
    mov [rbp-48], r13
    mov [rbp-56], r14
    mov r12, rcx

    mov eax, [editor_panel_mode]
    cmp eax, 3
    je .animation
    cmp eax, 9
    je .event
    cmp eax, 11
    je .ui
    jmp .done

.animation:
    mov eax, [database_index]
    cmp eax, [animation_count]
    jae .done
    imul eax, ANIMATION_SIZE
    lea r13, [animations+rax]

    lea rcx, [ts_text_buffer]
    lea rdx, [ts_anim_fmt]
    mov r8d, [r13+32]
    mov r9d, [r13+40]
    mov eax, [r13+44]
    mov [rsp+32], rax
    mov eax, [r13+68]
    mov [rsp+40], rax
    call wsprintfA
    mov [rbp-8], eax

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 420
    lea r9, [ts_text_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    xor r14d, r14d
.anim_loop:
    cmp r14d, [r13+32]
    jae .hitbox
    cmp r14d, 24
    jae .hitbox
    mov ecx, [col_grid]
    cmp r14d, [r13+40]
    jl .anim_color
    cmp r14d, [r13+44]
    jg .anim_color
    mov ecx, [col_preview]
.anim_color:
    call CreateSolidBrush
    mov [rbp-16], rax

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov ecx, r14d
    imul ecx, 9
    add eax, ecx
    mov [temp_rect+0], eax
    add eax, 7
    mov [temp_rect+8], eax
    mov dword [temp_rect+4], 446
    mov dword [temp_rect+12], 462
    mov rcx, r12
    lea rdx, [temp_rect]
    mov r8, [rbp-16]
    call FillRect
    mov rcx, [rbp-16]
    call DeleteObject
    inc r14d
    jmp .anim_loop

.hitbox:
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 80
    mov ecx, [r13+48]
    sar ecx, 1
    add eax, ecx
    mov [temp_rect+0], eax
    mov ecx, [r13+56]
    sar ecx, 1
    add eax, ecx
    cmp eax, [temp_rect+0]
    jg .hb_w_ok
    mov eax, [temp_rect+0]
    inc eax
.hb_w_ok:
    mov [temp_rect+8], eax

    mov eax, 490
    mov ecx, [r13+52]
    sar ecx, 1
    add eax, ecx
    mov [temp_rect+4], eax
    mov ecx, [r13+60]
    sar ecx, 1
    add eax, ecx
    cmp eax, [temp_rect+4]
    jg .hb_h_ok
    mov eax, [temp_rect+4]
    inc eax
.hb_h_ok:
    mov [temp_rect+12], eax

    mov ecx, [col_preview]
    call CreateSolidBrush
    mov [rbp-16], rax
    mov rcx, r12
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-16]
    call DeleteObject
    jmp .done

.event:
    mov eax, [database_index]
    cmp eax, [event_count]
    jae .done
    imul eax, EVENT_SIZE
    lea r13, [events+rax]
    xor r14d, r14d
.event_loop:
    cmp r14d, [r13+32]
    jae .done
    cmp r14d, EVENT_COMMANDS
    jae .done

    mov eax, r14d
    imul eax, EVENT_COMMAND_SIZE
    lea r10, [r13+rax+48]

    lea rcx, [ts_text_buffer]
    lea rdx, [ts_event_fmt]
    mov r8d, r14d
    inc r8d
    mov r9d, [r13+32]
    mov eax, [r10+0]
    mov [rsp+32], rax
    mov eax, [r10+4]
    mov [rsp+40], rax
    mov eax, [r10+8]
    mov [rsp+48], rax
    mov eax, [r10+12]
    mov [rsp+56], rax
    call wsprintfA
    mov [rbp-8], eax

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, r14d
    imul r8d, 20
    add r8d, 420
    lea r9, [ts_text_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    inc r14d
    jmp .event_loop

.ui:
    lea rcx, [ts_text_buffer]
    lea rdx, [ts_ui_fmt]
    mov r8d, [project_meta+64]
    mov r9d, [project_meta+68]
    mov eax, [project_meta+72]
    mov [rsp+32], rax
    call wsprintfA
    mov [rbp-8], eax

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, r12
    mov r8d, 420
    lea r9, [ts_text_buffer]
    mov eax, [rbp-8]
    mov [rsp+32], rax
    call TextOutA

    mov eax, [project_meta+64]
    add eax, LEFT_PANEL
    mov [temp_rect+0], eax
    add eax, 200
    mov [temp_rect+8], eax
    mov eax, [project_meta+68]
    add eax, TOP_BAR
    mov [temp_rect+4], eax
    add eax, 16
    mov [temp_rect+12], eax
    mov ecx, [col_preview]
    call CreateSolidBrush
    mov [rbp-16], rax
    mov rcx, r12
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-16]
    call DeleteObject

.done:
    mov r12, [rbp-40]
    mov r13, [rbp-48]
    mov r14, [rbp-56]
    leave
    ret


; ------------------------------------------------------------
; Right-panel click editor.
; ecx=client x, edx=client y, eax=1 when consumed.
; ------------------------------------------------------------
advanced_panel_click:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov [rbp-4], ecx
    mov [rbp-8], edx

    cmp dword [editor_panel_mode], 3
    je .animation
    cmp dword [editor_panel_mode], 9
    je .event
    xor eax, eax
    leave
    ret

.animation:
    cmp dword [rbp-8], 446
    jl .not_handled
    cmp dword [rbp-8], 462
    jg .not_handled

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov ecx, [rbp-4]
    sub ecx, eax
    cmp ecx, 0
    jl .not_handled
    mov eax, ecx
    cdq
    mov ecx, 9
    idiv ecx
    mov [rbp-12], eax

    mov ecx, [database_index]
    cmp ecx, [animation_count]
    jae .not_handled
    imul ecx, ANIMATION_SIZE
    lea r8, [animations+rcx]
    mov eax, [rbp-12]
    cmp eax, [r8+32]
    jae .not_handled

    mov ecx, 010h
    call GetKeyState
    test ax, 8000h
    jnz .set_end
    mov eax, [rbp-12]
    mov [r8+40], eax
    cmp eax, [r8+44]
    jle .handled
    mov [r8+44], eax
    jmp .handled
.set_end:
    mov eax, [rbp-12]
    mov [r8+44], eax
    cmp eax, [r8+40]
    jge .handled
    mov [r8+40], eax
    jmp .handled

.event:
    cmp dword [rbp-8], 420
    jl .not_handled
    mov eax, [rbp-8]
    sub eax, 420
    cdq
    mov ecx, 20
    idiv ecx
    cmp eax, 0
    jl .not_handled
    cmp eax, EVENT_COMMANDS
    jae .not_handled
    mov ecx, [database_index]
    cmp ecx, [event_count]
    jae .not_handled
    imul ecx, EVENT_SIZE
    cmp eax, [events+rcx+32]
    jae .not_handled
    mov [event_command_index], eax
    jmp .handled

.handled:
    mov eax, 1
    leave
    ret
.not_handled:
    xor eax, eax
    leave
    ret

; ------------------------------------------------------------
; Canvas click UI editor. ecx=x, edx=y.
; ------------------------------------------------------------
advanced_canvas_click:
    cmp dword [editor_panel_mode], 11
    jne .no
    sub ecx, LEFT_PANEL
    sub edx, TOP_BAR
    cmp ecx, 0
    jge .xok
    xor ecx, ecx
.xok:
    cmp edx, 0
    jge .yok
    xor edx, edx
.yok:
    mov [project_meta+64], ecx
    mov [project_meta+68], edx
    mov eax, 1
    ret
.no:
    xor eax, eax
    ret

; ------------------------------------------------------------
; One-level scene Undo / Redo (platforms + entities + map ownership).
; ------------------------------------------------------------
ts_copy_qwords:
    push rsi
    push rdi
    mov rsi, rcx
    mov rdi, rdx
    mov ecx, r8d
    rep movsq
    pop rdi
    pop rsi
    ret

history_capture:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov eax, [platform_count]
    mov [undo_platform_count], eax
    mov eax, [entity_count]
    mov [undo_entity_count], eax

    lea rcx, [platforms]
    lea rdx, [undo_platforms]
    mov r8d, (MAX_PLATFORMS*PLATFORM_SIZE)/8
    call ts_copy_qwords
    lea rcx, [entities]
    lea rdx, [undo_entities]
    mov r8d, (MAX_ENTITIES*ENTITY_SIZE)/8
    call ts_copy_qwords
    lea rcx, [platform_map_ids]
    lea rdx, [undo_platform_maps]
    mov r8d, MAX_PLATFORMS/2
    call ts_copy_qwords
    lea rcx, [entity_map_ids]
    lea rdx, [undo_entity_maps]
    mov r8d, MAX_ENTITIES/2
    call ts_copy_qwords

    mov dword [undo_valid], 1
    mov dword [redo_valid], 0
    leave
    ret

history_save_redo:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov eax, [platform_count]
    mov [redo_platform_count], eax
    mov eax, [entity_count]
    mov [redo_entity_count], eax
    lea rcx, [platforms]
    lea rdx, [redo_platforms]
    mov r8d, (MAX_PLATFORMS*PLATFORM_SIZE)/8
    call ts_copy_qwords
    lea rcx, [entities]
    lea rdx, [redo_entities]
    mov r8d, (MAX_ENTITIES*ENTITY_SIZE)/8
    call ts_copy_qwords
    lea rcx, [platform_map_ids]
    lea rdx, [redo_platform_maps]
    mov r8d, MAX_PLATFORMS/2
    call ts_copy_qwords
    lea rcx, [entity_map_ids]
    lea rdx, [redo_entity_maps]
    mov r8d, MAX_ENTITIES/2
    call ts_copy_qwords
    mov dword [redo_valid], 1
    leave
    ret

history_undo:
    cmp dword [undo_valid], 0
    je .done
    push rbp
    mov rbp, rsp
    sub rsp, 32
    call history_save_redo
    mov eax, [undo_platform_count]
    mov [platform_count], eax
    mov eax, [undo_entity_count]
    mov [entity_count], eax
    lea rcx, [undo_platforms]
    lea rdx, [platforms]
    mov r8d, (MAX_PLATFORMS*PLATFORM_SIZE)/8
    call ts_copy_qwords
    lea rcx, [undo_entities]
    lea rdx, [entities]
    mov r8d, (MAX_ENTITIES*ENTITY_SIZE)/8
    call ts_copy_qwords
    lea rcx, [undo_platform_maps]
    lea rdx, [platform_map_ids]
    mov r8d, MAX_PLATFORMS/2
    call ts_copy_qwords
    lea rcx, [undo_entity_maps]
    lea rdx, [entity_map_ids]
    mov r8d, MAX_ENTITIES/2
    call ts_copy_qwords
    mov dword [undo_valid], 0
    mov dword [selected_platform], -1
    mov dword [selected_entity], -1
    leave
.done:
    ret

history_redo:
    cmp dword [redo_valid], 0
    je .done
    push rbp
    mov rbp, rsp
    sub rsp, 32

    ; current becomes the next undo state
    mov eax, [platform_count]
    mov [undo_platform_count], eax
    mov eax, [entity_count]
    mov [undo_entity_count], eax
    lea rcx, [platforms]
    lea rdx, [undo_platforms]
    mov r8d, (MAX_PLATFORMS*PLATFORM_SIZE)/8
    call ts_copy_qwords
    lea rcx, [entities]
    lea rdx, [undo_entities]
    mov r8d, (MAX_ENTITIES*ENTITY_SIZE)/8
    call ts_copy_qwords
    lea rcx, [platform_map_ids]
    lea rdx, [undo_platform_maps]
    mov r8d, MAX_PLATFORMS/2
    call ts_copy_qwords
    lea rcx, [entity_map_ids]
    lea rdx, [undo_entity_maps]
    mov r8d, MAX_ENTITIES/2
    call ts_copy_qwords
    mov dword [undo_valid], 1

    mov eax, [redo_platform_count]
    mov [platform_count], eax
    mov eax, [redo_entity_count]
    mov [entity_count], eax
    lea rcx, [redo_platforms]
    lea rdx, [platforms]
    mov r8d, (MAX_PLATFORMS*PLATFORM_SIZE)/8
    call ts_copy_qwords
    lea rcx, [redo_entities]
    lea rdx, [entities]
    mov r8d, (MAX_ENTITIES*ENTITY_SIZE)/8
    call ts_copy_qwords
    lea rcx, [redo_platform_maps]
    lea rdx, [platform_map_ids]
    mov r8d, MAX_PLATFORMS/2
    call ts_copy_qwords
    lea rcx, [redo_entity_maps]
    lea rdx, [entity_map_ids]
    mov r8d, MAX_ENTITIES/2
    call ts_copy_qwords
    mov dword [redo_valid], 0
    mov dword [selected_platform], -1
    mov dword [selected_entity], -1
    leave
.done:
    ret

; ------------------------------------------------------------
; Import BMP sprite sheet and attach it to selected animation.
; ------------------------------------------------------------
ts_import_sprite:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    mov [rbp-40], rdi

    cmp dword [sprite_count], MAX_SPRITES
    jae .done

    lea rdi, [ts_ofn]
    xor eax, eax
    mov ecx, 20
    rep stosq
    lea rdi, [ts_file_buffer]
    mov ecx, 33
    rep stosq

    mov dword [ts_ofn+0], 152
    mov rax, [hwnd_main]
    mov [ts_ofn+8], rax
    lea rax, [ts_bmp_filter]
    mov [ts_ofn+24], rax
    lea rax, [ts_file_buffer]
    mov [ts_ofn+48], rax
    mov dword [ts_ofn+56], 260
    lea rax, [ts_import_title]
    mov [ts_ofn+88], rax
    mov dword [ts_ofn+96], 01800h

    lea rcx, [ts_ofn]
    call GetOpenFileNameA
    test eax, eax
    jz .done

    mov eax, [sprite_count]
    mov [rbp-8], eax
    imul eax, SPRITE_SIZE
    lea rcx, [sprites+rax]
    lea rdx, [ts_name_sprite]
    call lstrcpyA

    lea rcx, [ts_assets_dir]
    xor edx, edx
    call CreateDirectoryA

    lea rcx, [ts_asset_path]
    lea rdx, [ts_sprite_path_fmt]
    mov r8d, [rbp-8]
    inc r8d
    call wsprintfA

    lea rcx, [ts_file_buffer]
    lea rdx, [ts_asset_path]
    xor r8d, r8d
    call CopyFileA

    mov eax, [rbp-8]
    imul eax, SPRITE_SIZE
    lea rcx, [sprites+rax+32]
    lea rdx, [ts_asset_path]
    mov r8d, 128
    call lstrcpynA

    cmp dword [editor_panel_mode], 3
    jne .count
    mov eax, [database_index]
    cmp eax, [animation_count]
    jae .count
    imul eax, ANIMATION_SIZE
    mov edx, [rbp-8]
    mov [animations+rax+68], edx

.count:
    inc dword [sprite_count]

.done:
    mov rdi, [rbp-40]
    leave
    ret


; ------------------------------------------------------------
; Ctrl+F7 imports Assets\Game.ico and stores project icon path.
; ------------------------------------------------------------
ts_import_icon:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    mov [rbp-40], rdi
    lea rdi, [ts_ofn]
    xor eax, eax
    mov ecx, 20
    rep stosq
    lea rdi, [ts_file_buffer]
    mov ecx, 33
    rep stosq
    mov dword [ts_ofn+0], 152
    mov rax, [hwnd_main]
    mov [ts_ofn+8], rax
    lea rax, [ts_icon_filter]
    mov [ts_ofn+24], rax
    lea rax, [ts_file_buffer]
    mov [ts_ofn+48], rax
    mov dword [ts_ofn+56], 260
    lea rax, [ts_icon_title]
    mov [ts_ofn+88], rax
    mov dword [ts_ofn+96], 01800h
    lea rcx, [ts_ofn]
    call GetOpenFileNameA
    test eax, eax
    jz .done
    lea rcx, [ts_assets_dir]
    xor edx, edx
    call CreateDirectoryA
    lea rcx, [ts_file_buffer]
    lea rdx, [ts_icon_asset]
    xor r8d, r8d
    call CopyFileA
    lea rcx, [project_meta+160]
    lea rdx, [ts_icon_asset]
    call lstrcpyA
.done:
    mov rdi, [rbp-40]
    leave
    ret

; ------------------------------------------------------------
; F12 imports BGM.wav. Ctrl+F12 imports SE.wav.
; ------------------------------------------------------------
ts_import_audio:
    push rbp
    mov rbp, rsp
    sub rsp, 112
    mov [rbp-40], rdi

    lea rdi, [ts_ofn]
    xor eax, eax
    mov ecx, 20
    rep stosq
    lea rdi, [ts_file_buffer]
    mov ecx, 33
    rep stosq

    mov dword [ts_ofn+0], 152
    mov rax, [hwnd_main]
    mov [ts_ofn+8], rax
    lea rax, [ts_audio_filter]
    mov [ts_ofn+24], rax
    lea rax, [ts_file_buffer]
    mov [ts_ofn+48], rax
    mov dword [ts_ofn+56], 260

    mov ecx, 011h
    call GetKeyState
    test ax, 8000h
    jz .bgm_title
    lea rax, [ts_audio_title_se]
    mov [ts_ofn+88], rax
    mov dword [rbp-8], 1
    jmp .dialog
.bgm_title:
    lea rax, [ts_audio_title_bgm]
    mov [ts_ofn+88], rax
    mov dword [rbp-8], 0
.dialog:
    mov dword [ts_ofn+96], 01800h
    lea rcx, [ts_ofn]
    call GetOpenFileNameA
    test eax, eax
    jz .done

    lea rcx, [ts_assets_dir]
    xor edx, edx
    call CreateDirectoryA

    cmp dword [rbp-8], 0
    jne .copy_se
    lea rcx, [ts_file_buffer]
    lea rdx, [ts_bgm_asset]
    xor r8d, r8d
    call CopyFileA
    mov dword [project_meta+108], 1
    jmp .done
.copy_se:
    lea rcx, [ts_file_buffer]
    lea rdx, [ts_se_asset]
    xor r8d, r8d
    call CopyFileA
    mov dword [project_meta+108], 1
.done:
    mov rdi, [rbp-40]
    leave
    ret

; ------------------------------------------------------------
; Find TsuramechokiRuntime.exe next to the editor executable.
; rax = full path, or 0 on failure.
; ------------------------------------------------------------
resolve_runtime_source:
    push rbp
    mov rbp, rsp
    sub rsp, 48

    xor ecx, ecx
    lea rdx, [ts_runtime_full_path]
    mov r8d, 520
    call GetModuleFileNameA
    test eax, eax
    jz .fail
    mov ecx, eax
.scan:
    test ecx, ecx
    jz .no_dir
    dec ecx
    mov al, [ts_runtime_full_path+rcx]
    cmp al, 05Ch
    je .cut
    cmp al, 02Fh
    je .cut
    jmp .scan
.cut:
    inc ecx
    mov byte [ts_runtime_full_path+rcx], 0
    lea rcx, [ts_runtime_full_path]
    lea rdx, [ts_runtime_filename]
    call lstrcatA
    lea rax, [ts_runtime_full_path]
    leave
    ret
.no_dir:
    lea rcx, [ts_runtime_full_path]
    lea rdx, [ts_runtime_filename]
    call lstrcpyA
    lea rax, [ts_runtime_full_path]
    leave
    ret
.fail:
    xor eax, eax
    leave
    ret

; ------------------------------------------------------------
; export_game
; Build\Game.exe + Build\project.tsrp
; ------------------------------------------------------------
export_game:
    push rbp
    mov rbp, rsp
    sub rsp, 96
    mov [rbp-40], r12
    mov [rbp-48], r13
    mov dword [build_status], -1

    call save_project
    test eax, eax
    jz .export_fail

    lea rcx, [ts_build_dir]
    xor edx, edx
    call CreateDirectoryA

    lea rcx, [ts_build_exe_path]
    lea rdx, [ts_build_exe_fmt]
    lea r8, [project_meta+112]
    call wsprintfA

    call resolve_runtime_source
    test rax, rax
    jz .export_fail
    mov rcx, rax
    lea rdx, [ts_build_exe_path]
    xor r8d, r8d
    call CopyFileA
    test eax, eax
    jz .export_fail

    lea rcx, [project_path]
    lea rdx, [ts_project_dst]
    xor r8d, r8d
    call CopyFileA
    test eax, eax
    jz .export_fail

    lea rcx, [ts_build_assets_dir]
    xor edx, edx
    call CreateDirectoryA

    xor r12d, r12d
.asset_loop:
    cmp r12d, [sprite_count]
    jae .export_done

    mov eax, r12d
    imul eax, SPRITE_SIZE
    lea r13, [sprites+rax]
    cmp byte [r13+32], 0
    je .asset_next

    lea rcx, [ts_build_asset_path]
    lea rdx, [ts_build_asset_fmt]
    lea r8, [r13+32]
    call wsprintfA

    ; r12/r13 are nonvolatile across Win64 API calls.
    lea rcx, [r13+32]
    lea rdx, [ts_build_asset_path]
    xor r8d, r8d
    call CopyFileA
    test eax, eax
    jz .export_fail

.asset_next:
    inc r12d
    jmp .asset_loop

.export_done:
    lea rcx, [ts_bgm_asset]
    lea rdx, [ts_build_bgm]
    xor r8d, r8d
    call CopyFileA
    lea rcx, [ts_se_asset]
    lea rdx, [ts_build_se]
    xor r8d, r8d
    call CopyFileA
    lea rcx, [ts_icon_asset]
    lea rdx, [ts_build_icon]
    xor r8d, r8d
    call CopyFileA
    mov dword [build_status], 1
    mov eax, 1
    jmp .export_return
.export_fail:
    mov dword [build_status], -1
    xor eax, eax
.export_return:
    mov r12, [rbp-40]
    mov r13, [rbp-48]
    leave
    ret

; ------------------------------------------------------------
; Launch Build\Game.exe with Build as the working directory.
; ------------------------------------------------------------
launch_built_game:
    push rbp
    mov rbp, rsp
    sub rsp, 64

    mov rcx, [hwnd_main]
    xor edx, edx
    lea r8, [project_meta+112]
    xor r9d, r9d
    lea rax, [ts_build_cwd]
    mov [rsp+32], rax
    mov qword [rsp+40], 5
    call ShellExecuteA
    cmp rax, 32
    jle .fail
    mov dword [build_status], 2
    mov eax, 1
    leave
    ret
.fail:
    mov dword [build_status], -2
    xor eax, eax
    leave
    ret