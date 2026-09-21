; Regression tests use real file, combat, event and progression routines.
%define mainCRTStartup runtime_entry
%include "runtime.asm"
%undef mainCRTStartup
%macro ASSERT_EQ 3
    mov ecx, %3
    cmp %1, %2
    jne test_fail
%endmacro
section .bss
    test_handle resq 1
section .text
global mainCRTStartup
mainCRTStartup:
    and rsp, -16
    sub rsp, 192
    call load_project
    ASSERT_EQ eax, 1, 40
    call apply_player_class
    call runtime_init_state
    call initialize_player
    mov dword [project_meta+108], 0

    ; Both surviving and lethal hits must read the indexed monster HP.
    mov dword [player_x], 450
    mov dword [player_y], 430
    mov dword [active_skill_id], 0
    mov dword [attack_anim_id], -1
    mov dword [skills+52], 0
    call attack_monsters
    ASSERT_EQ dword [enemy_hp+4], 10, 41
    ASSERT_EQ dword [entities+ENTITY_SIZE], ENTITY_MONSTER, 42
    call attack_monsters
    ASSERT_EQ dword [entities+ENTITY_SIZE], ENTITY_NONE, 43
    ASSERT_EQ dword [player_exp], 10, 44
    ASSERT_EQ dword [inventory_counts], 4, 45

    ; Item-grant followed by another command survives nested calls.
    mov dword [events+32], 2
    mov dword [events+48], EVENT_ADD_ITEM
    mov dword [events+52], 0
    mov dword [events+56], 2
    mov dword [events+64], EVENT_ADD_MONEY
    mov dword [events+68], 7
    xor ecx, ecx
    call run_event
    ASSERT_EQ dword [inventory_counts], 6, 46
    ASSERT_EQ dword [player_money], 7, 47
    ; Negative conditional skips must not loop backwards forever.
    mov dword [events+48], EVENT_IF_VAR_EQ
    mov dword [events+52], 0
    mov dword [events+56], 999
    mov dword [events+60], -1
    xor ecx, ecx
    call run_event
    ASSERT_EQ dword [player_money], 7, 48

    mov dword [player_exp], 100
    call check_level_up
    ASSERT_EQ dword [player_level], 2, 49
    ASSERT_EQ dword [player_max_hp], 110, 50
    call save_game
    ASSERT_EQ eax, 1, 51
    ; Simulate restart: loading must reconstruct, not stack, level bonuses.
    call runtime_init_state
    mov dword [initial_max_hp], 100
    mov dword [initial_base_attack], 10
    mov dword [player_max_hp], 100
    mov dword [player_base_attack], 10
    call load_game
    ASSERT_EQ eax, 1, 52
    ASSERT_EQ dword [player_max_hp], 110, 53
    ASSERT_EQ dword [player_base_attack], 11, 54
    call load_game
    ASSERT_EQ eax, 1, 55
    ASSERT_EQ dword [player_max_hp], 110, 56

    call get_save_path
    mov rcx, rax
    mov edx, GENERIC_READ
    xor r8d, r8d
    xor r9d, r9d
    mov qword [rsp+32], OPEN_EXISTING
    mov qword [rsp+40], FILE_ATTRIBUTE_NORMAL
    mov qword [rsp+48], 0
    call CreateFileA
    mov [test_handle], rax
    mov dword [player_money], 999
    call save_game
    ASSERT_EQ eax, 0, 63
    mov rcx, [test_handle]
    call CloseHandle
    call load_game
    ASSERT_EQ eax, 1, 64
    ASSERT_EQ dword [player_money], 7, 65

    ; Invalid map or truncated save must leave all live state untouched.
    mov dword [save_staging+12], MAX_MAPS
    call write_staged_save
    mov dword [inventory_counts], 123
    call load_game
    ASSERT_EQ eax, 0, 57
    ASSERT_EQ dword [inventory_counts], 123, 58
    ASSERT_EQ dword [active_map], 0, 59
    call get_save_path
    mov rcx, rax
    mov edx, GENERIC_WRITE
    xor r8d, r8d
    xor r9d, r9d
    mov qword [rsp+32], CREATE_ALWAYS
    mov qword [rsp+40], FILE_ATTRIBUTE_NORMAL
    mov qword [rsp+48], 0
    call CreateFileA
    mov [test_handle], rax
    mov rcx, rax
    lea rdx, [save_header]
    mov r8d, SAVE_HEADER_SIZE
    call ts_rt_write_block
    mov rcx, [test_handle]
    call CloseHandle
    call load_game
    ASSERT_EQ eax, 0, 60
    ASSERT_EQ dword [inventory_counts], 123, 61
    ASSERT_EQ dword [player_level], 2, 62
    xor ecx, ecx
    call ExitProcess

write_staged_save:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    call get_save_path
    mov rcx, rax
    mov edx, GENERIC_WRITE
    xor r8d, r8d
    xor r9d, r9d
    mov qword [rsp+32], CREATE_ALWAYS
    mov qword [rsp+40], FILE_ATTRIBUTE_NORMAL
    mov qword [rsp+48], 0
    call CreateFileA
    mov [test_handle], rax
    mov rcx, rax
    lea rdx, [save_staging]
    mov r8d, SAVE_HEADER_SIZE+(MAX_QUESTS*2+MAX_VARIABLES+MAX_ITEMS)*4
    call ts_rt_write_block
    mov rcx, [test_handle]
    call CloseHandle
    leave
    ret

test_fail:
    call ExitProcess
