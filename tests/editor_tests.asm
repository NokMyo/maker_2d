; Win64 regression tests invoke the real editor routines. No second language.
%define mainCRTStartup editor_entry
%include "editor.asm"
%undef mainCRTStartup
extern SetEndOfFile
extern DestroyWindow
extern GetDC
extern ReleaseDC
extern CreateCompatibleDC
extern DeleteDC

%macro ASSERT_EQ 3
    mov ecx, %3
    cmp %1, %2
    jne test_fail
%endmacro

section .data
    test_window_class db "STATIC",0
section .bss
    test_hwnd resq 1
    test_dc resq 1
    test_handle resq 1
section .text
global mainCRTStartup
mainCRTStartup:
    and rsp, -16
    sub rsp, 192
    call init_default_project
    mov dword [items+48], 777
    mov dword [events+64], 999
    call init_default_project
    ASSERT_EQ dword [items+48], 0, 10
    ASSERT_EQ dword [events+64], 0, 11

    ; A standard hidden window supplies real client coordinates to WndProc.
    xor ecx, ecx
    lea rdx, [test_window_class]
    xor r8d, r8d
    xor r9d, r9d
    mov qword [rsp+32], 0
    mov qword [rsp+40], 0
    mov qword [rsp+48], 1280
    mov qword [rsp+56], 800
    mov qword [rsp+64], 0
    mov qword [rsp+72], 0
    mov qword [rsp+80], 0
    mov qword [rsp+88], 0
    call CreateWindowExA
    test rax, rax
    mov ecx, 12
    jz test_fail
    mov [test_hwnd], rax
    mov dword [tool_mode], TOOL_MONSTER
    mov rcx, rax
    mov edx, WM_LBUTTONDOWN
    xor r8d, r8d
    mov r9d, (200 << 16) | 500
    call WndProc
    ASSERT_EQ dword [entity_count], 3, 13
    ASSERT_EQ dword [entities+ENTITY_SIZE*2+4], 280, 14
    ASSERT_EQ dword [entities+ENTITY_SIZE*2+8], 144, 15
    call history_undo
    ASSERT_EQ dword [entity_count], 2, 16
    call history_redo
    ASSERT_EQ dword [entity_count], 3, 17

    mov dword [rename_mode], 1
    mov ecx, VK_P
    call system_keydown
    ASSERT_EQ eax, 1, 18
    ASSERT_EQ dword [tool_mode], TOOL_MONSTER, 19
    mov dword [rename_mode], 0

    ; Exact round trip; truncated read must restore current unsaved state.
    call save_project
    ASSERT_EQ eax, 1, 20
    mov dword [player_max_hp], 321
    call load_project
    ASSERT_EQ eax, 1, 21
    ASSERT_EQ dword [player_max_hp], 100, 22
    ASSERT_EQ dword [undo_valid], 0, 23
    mov dword [player_max_hp], 321
    mov dword [entities+4], 765
    lea rcx, [project_path]
    mov edx, GENERIC_WRITE
    xor r8d, r8d
    xor r9d, r9d
    mov qword [rsp+32], CREATE_ALWAYS
    mov qword [rsp+40], FILE_ATTRIBUTE_NORMAL
    mov qword [rsp+48], 0
    call CreateFileA
    mov [test_handle], rax
    mov rcx, rax
    lea rdx, [project_header_v2]
    mov r8d, PROJECT_HEADER_SIZE
    call ts_write_block
    mov rcx, [test_handle]
    call CloseHandle
    call load_project
    ASSERT_EQ eax, 0, 24
    ASSERT_EQ dword [player_max_hp], 321, 25
    ASSERT_EQ dword [entities+4], 765, 26

    ; A locked destination makes replacement fail without truncating the old file.
    call init_default_project
    call save_project
    ASSERT_EQ eax, 1, 30
    lea rcx, [project_path]
    mov edx, GENERIC_READ
    xor r8d, r8d
    xor r9d, r9d
    mov qword [rsp+32], OPEN_EXISTING
    mov qword [rsp+40], FILE_ATTRIBUTE_NORMAL
    mov qword [rsp+48], 0
    call CreateFileA
    mov [test_handle], rax
    mov dword [player_max_hp], 500
    call save_project
    ASSERT_EQ eax, 0, 31
    mov rcx, [test_handle]
    call CloseHandle
    call load_project
    ASSERT_EQ eax, 1, 32
    ASSERT_EQ dword [player_max_hp], 100, 33

    ; Metadata boundary checks reject unsafe names.
    lea rcx, [project_meta+112]
    mov dword [rcx], 02E2E5C2Eh
    call validate_exe_name
    ASSERT_EQ eax, 0, 27
    call init_default_project
    call save_project
    ASSERT_EQ eax, 1, 28

    ; Event preview calls wsprintf with four stack arguments. Preserve r14.
    lea rcx, [editor_gdip_token]
    lea rdx, [editor_gdip_startup]
    xor r8d, r8d
    call GdiplusStartup
    mov rcx, [test_hwnd]
    call GetDC
    mov [test_dc], rax
    mov dword [client_rect+8], 1280
    mov dword [client_rect+12], 800
    mov dword [editor_panel_mode], 9
    mov r14, 12345678h
    mov rcx, rax
    call draw_mode_preview
    ASSERT_EQ r14, 12345678h, 29
    mov rcx, [test_hwnd]
    mov rdx, [test_dc]
    call ReleaseDC
    mov rcx, [test_hwnd]
    call DestroyWindow
    mov rcx, [editor_gdip_token]
    call GdiplusShutdown
    xor ecx, ecx
    call ExitProcess

test_fail:
    call ExitProcess
