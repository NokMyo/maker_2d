; Shared bounded validation. All routines preserve Win64 nonvolatile registers.
section .text

; rcx=string, edx=capacity. eax=1 only for a NUL within capacity.
validate_string:
    xor eax, eax
.loop:
    test edx, edx
    jz .done
    cmp byte [rcx], 0
    je .valid
    inc rcx
    dec edx
    jmp .loop
.valid:
    mov eax, 1
.done:
    ret

; An executable basename, never a path or shell command. Max 47 bytes.
validate_exe_name:
    xor r8d, r8d
.loop:
    cmp r8d, 48
    jae .bad
    movzx eax, byte [rcx+r8]
    test eax, eax
    jz .extension
    cmp eax, 32
    jb .bad
    cmp eax, 127
    jae .bad
    cmp eax, 34
    je .bad
    cmp eax, 42
    je .bad
    cmp eax, 47
    je .bad
    cmp eax, 58
    je .bad
    cmp eax, 60
    je .bad
    cmp eax, 62
    je .bad
    cmp eax, 63
    je .bad
    cmp eax, 92
    je .bad
    cmp eax, 124
    je .bad
    inc r8d
    jmp .loop
.extension:
    cmp r8d, 5
    jb .bad
    cmp byte [rcx], '.'
    je .bad
    mov eax, [rcx+r8-4]
    or eax, 20202000h
    cmp eax, 06578652Eh ; .exe (case insensitive)
    jne .bad
    mov eax, 1
    ret
.bad:
    xor eax, eax
    ret

; Only relative paths below Assets are used by the project importer.
; rcx=path; edx=capacity. Empty optional paths are valid.
validate_asset_path:
    cmp byte [rcx], 0
    je .valid
    cmp edx, 8
    jb .bad
    cmp dword [rcx], 065737341h ; Asse
    jne .bad
    cmp word [rcx+4], 07374h   ; ts
    jne .bad
    cmp byte [rcx+6], 92
    jne .bad
    add rcx, 7
    sub edx, 7
.loop:
    test edx, edx
    jz .bad
    movzx eax, byte [rcx]
    test eax, eax
    jz .valid
    cmp eax, 32
    jb .bad
    cmp eax, 58
    je .bad
    cmp eax, 47
    je .bad
    cmp eax, 92
    je .bad
    cmp eax, '.'
    jne .next
    cmp edx, 2
    jb .bad
    cmp byte [rcx+1], '.'
    je .bad
.next:
    inc rcx
    dec edx
    jmp .loop
.valid:
    mov eax, 1
    ret
.bad:
    xor eax, eax
    ret

; rcx=records, edx=count, r8d=stride. All record names have capacity 32.
validate_record_names:
    mov r10, rcx
    mov r11d, edx
.record:
    test r11d, r11d
    jz .valid
    xor r9d, r9d
.name:
    cmp byte [r10+r9], 0
    je .next
    inc r9d
    cmp r9d, 32
    jb .name
    xor eax, eax
    ret
.next:
    add r10, r8
    dec r11d
    jmp .record
.valid:
    mov eax, 1
    ret

; Called only after count-bounded, exact reads of every TSRP v3 block.
validate_project_data:
    push rbp
    mov rbp, rsp
    sub rsp, 48
    cmp dword [screen_width], 320
    jl .bad
    cmp dword [screen_width], 16384
    jg .bad
    cmp dword [screen_height], 240
    jl .bad
    cmp dword [screen_height], 16384
    jg .bad
    cmp dword [player_move_speed], 1
    jl .bad
    cmp dword [player_max_hp], 1
    jl .bad
    cmp dword [player_max_mp], 0
    jl .bad
    cmp dword [player_base_attack], 1
    jl .bad

    lea rcx, [project_meta]
    mov edx, 32
    call validate_string
    test eax, eax
    jz .bad
    lea rcx, [project_meta+112]
    call validate_exe_name
    test eax, eax
    jz .bad
    lea rcx, [project_meta+160]
    mov edx, 32
    call validate_asset_path
    test eax, eax
    jz .bad

%macro CHECK_NAMES 3
    lea rcx, [%1]
    mov edx, [%2]
    mov r8d, %3
    call validate_record_names
    test eax, eax
    jz .bad
%endmacro
    CHECK_NAMES maps, map_count, MAP_SIZE
    CHECK_NAMES animations, animation_count, ANIMATION_SIZE
    CHECK_NAMES skills, skill_count, SKILL_SIZE
    CHECK_NAMES monster_defs, monster_count, MONSTER_SIZE
    CHECK_NAMES items, item_count, ITEM_SIZE
    CHECK_NAMES quests, quest_count, QUEST_SIZE
    CHECK_NAMES variables, variable_count, VARIABLE_SIZE
    CHECK_NAMES events, event_count, EVENT_SIZE
    CHECK_NAMES sprites, sprite_count, SPRITE_SIZE
    CHECK_NAMES class_defs, class_count, CLASS_SIZE
%unmacro CHECK_NAMES 3

    xor r10d, r10d
.platforms:
    cmp r10d, [platform_count]
    jae .entity_start
    mov eax, [platform_map_ids+r10*4]
    cmp eax, [map_count]
    jae .bad
    inc r10d
    jmp .platforms
.entity_start:
    xor r10d, r10d
.entities:
    cmp r10d, [entity_count]
    jae .events_start
    mov eax, [entity_map_ids+r10*4]
    cmp eax, [map_count]
    jae .bad
    inc r10d
    jmp .entities
.events_start:
    mov dword [rbp-4], 0
.events:
    mov eax, [rbp-4]
    cmp eax, [event_count]
    jae .sprites_start
    imul eax, EVENT_SIZE
    cmp dword [events+rax+32], EVENT_COMMANDS
    ja .bad
    lea rcx, [events+rax+176]
    mov edx, 128
    call validate_string
    test eax, eax
    jz .bad
    inc dword [rbp-4]
    jmp .events
.sprites_start:
    mov dword [rbp-4], 0
.sprites:
    mov eax, [rbp-4]
    cmp eax, [sprite_count]
    jae .valid
    imul eax, SPRITE_SIZE
    lea rcx, [sprites+rax+32]
    mov edx, 128
    call validate_asset_path
    test eax, eax
    jz .bad
    inc dword [rbp-4]
    jmp .sprites
.valid:
    mov eax, 1
    leave
    ret
.bad:
    xor eax, eax
    leave
    ret
