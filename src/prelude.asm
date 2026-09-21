; Leella Prelude
; 100% x86-64 assembly source
; NASM + Microsoft linker, Win32 API only

bits 64
default rel

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
extern SetCapture
extern ReleaseCapture

%define CS_HREDRAW          0x0002
%define CS_VREDRAW          0x0001
%define WS_OVERLAPPEDWINDOW 0x00CF0000
%define WS_VISIBLE          0x10000000
%define CW_USEDEFAULT       0x80000000
%define SW_SHOW             5

%define WM_DESTROY          0x0002
%define WM_PAINT            0x000F
%define WM_ERASEBKGND       0x0014
%define WM_MOUSEMOVE        0x0200
%define WM_LBUTTONDOWN      0x0201
%define WM_LBUTTONUP        0x0202

%define PS_SOLID            0
%define TRANSPARENT         1
%define IDC_ARROW           32512

%define LEFT_PANEL          220
%define RIGHT_PANEL         270
%define TOP_BAR             56
%define GRID_SIZE           32
%define SNAP_SIZE           16
%define MAX_PLATFORMS       256

section .data
    class_name      db "LeellaPreludeWindow",0
    window_title    db "Leella Prelude - 2D Side-scrolling RPG Maker",0

    txt_brand       db "LEELLA PRELUDE",0
    txt_brand_len   equ $-txt_brand-1
    txt_maps        db "MAPS",0
    txt_maps_len    equ $-txt_maps-1
    txt_map1        db "Map_001",0
    txt_map1_len    equ $-txt_map1-1
    txt_tools       db "TOOLS",0
    txt_tools_len   equ $-txt_tools-1
    txt_platform    db "[P] Platform",0
    txt_platform_len equ $-txt_platform-1
    txt_entity      db "[E] Entity",0
    txt_entity_len  equ $-txt_entity-1
    txt_props       db "PROPERTIES",0
    txt_props_len   equ $-txt_props-1
    txt_prop_name   db "Selection: Platform",0
    txt_prop_name_len equ $-txt_prop_name-1
    txt_hint        db "Drag on the canvas to draw a platform",0
    txt_hint_len    equ $-txt_hint-1
    txt_test        db "TEST GAME",0
    txt_test_len    equ $-txt_test-1
    txt_status      db "Platform tool | Snap 16 px | No auto-generation",0
    txt_status_len  equ $-txt_status-1

    ; COLORREF = 0x00BBGGRR
    col_bg          dd 0x00171311
    col_panel       dd 0x00211D1B
    col_top         dd 0x00282320
    col_canvas      dd 0x001D1A18
    col_grid        dd 0x00352F2B
    col_platform    dd 0x00E8C56F
    col_preview     dd 0x0069B5F2
    col_text        dd 0x00F1EEE9
    col_muted       dd 0x00A9A39A
    col_accent      dd 0x00B88954

section .bss
    hinstance       resq 1
    hwnd_main       resq 1
    msg_buf         resb 64
    wc_buf          resb 80
    paint_buf       resb 80
    client_rect     resd 4
    temp_rect       resd 4

    dragging        resd 1
    drag_start_x    resd 1
    drag_start_y    resd 1
    drag_cur_x      resd 1
    drag_cur_y      resd 1

    platform_count  resd 1
    ; each entry: x1,y1,x2,y2 (4 dwords)
    platforms       resd MAX_PLATFORMS*4

section .text

mainCRTStartup:
    and rsp, -16
    sub rsp, 160

    xor ecx, ecx
    call GetModuleHandleA
    mov [hinstance], rax

    ; WNDCLASSEXA
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
    jz .exit_fail

    xor ecx, ecx
    lea rdx, [class_name]
    lea r8, [window_title]
    mov r9d, WS_OVERLAPPEDWINDOW | WS_VISIBLE
    mov qword [rsp+32], CW_USEDEFAULT
    mov qword [rsp+40], CW_USEDEFAULT
    mov qword [rsp+48], 1280
    mov qword [rsp+56], 800
    mov qword [rsp+64], 0
    mov qword [rsp+72], 0
    mov rax, [hinstance]
    mov [rsp+80], rax
    mov qword [rsp+88], 0
    call CreateWindowExA
    test rax, rax
    jz .exit_fail

    mov [hwnd_main], rax

    mov rcx, rax
    mov edx, SW_SHOW
    call ShowWindow

    mov rcx, [hwnd_main]
    call UpdateWindow

.message_loop:
    lea rcx, [msg_buf]
    xor edx, edx
    xor r8d, r8d
    xor r9d, r9d
    call GetMessageA
    cmp eax, 0
    jle .exit_ok

    lea rcx, [msg_buf]
    call TranslateMessage

    lea rcx, [msg_buf]
    call DispatchMessageA
    jmp .message_loop

.exit_ok:
    mov ecx, dword [msg_buf+16]
    call ExitProcess

.exit_fail:
    mov ecx, 1
    call ExitProcess


; ------------------------------------------------------------
; LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM)
; rcx = hwnd, edx = msg, r8 = wparam, r9 = lparam
; ------------------------------------------------------------
WndProc:
    push rbp
    mov rbp, rsp
    sub rsp, 192

    ; Win64 ABI: preserve non-volatile registers used by the renderer.
    mov [rbp-64], r12
    mov [rbp-72], r13

    mov [rbp-8], rcx
    mov [rbp-16], r8
    mov [rbp-24], r9

    cmp edx, WM_PAINT
    je .paint
    cmp edx, WM_ERASEBKGND
    je .erase
    cmp edx, WM_LBUTTONDOWN
    je .mouse_down
    cmp edx, WM_MOUSEMOVE
    je .mouse_move
    cmp edx, WM_LBUTTONUP
    je .mouse_up
    cmp edx, WM_DESTROY
    je .destroy

.default:
    mov rcx, [rbp-8]
    ; edx still contains original message for paths that jump directly here.
    mov r8, [rbp-16]
    mov r9, [rbp-24]
    call DefWindowProcA
    mov r12, [rbp-64]
    mov r13, [rbp-72]
    leave
    ret

.erase:
    mov eax, 1
    mov r12, [rbp-64]
    mov r13, [rbp-72]
    leave
    ret

.destroy:
    xor ecx, ecx
    call PostQuitMessage
    xor eax, eax
    mov r12, [rbp-64]
    mov r13, [rbp-72]
    leave
    ret

.mouse_down:
    mov r10, [rbp-24]
    mov eax, r10d
    and eax, 0FFFFh
    mov ecx, eax

    mov eax, r10d
    shr eax, 16
    and eax, 0FFFFh
    mov edx, eax

    cmp ecx, LEFT_PANEL
    jl .handled_zero
    cmp edx, TOP_BAR
    jl .handled_zero

    ; Snap X to 16
    add ecx, SNAP_SIZE/2
    and ecx, -SNAP_SIZE
    ; Snap Y to 16
    add edx, SNAP_SIZE/2
    and edx, -SNAP_SIZE

    mov [drag_start_x], ecx
    mov [drag_cur_x], ecx
    mov [drag_start_y], edx
    mov [drag_cur_y], edx
    mov dword [dragging], 1

    mov rcx, [rbp-8]
    call SetCapture

    mov rcx, [rbp-8]
    xor edx, edx
    mov r8d, 0
    call InvalidateRect
    jmp .handled_zero

.mouse_move:
    cmp dword [dragging], 0
    je .handled_zero

    mov r10, [rbp-24]
    mov eax, r10d
    and eax, 0FFFFh
    mov ecx, eax
    add ecx, SNAP_SIZE/2
    and ecx, -SNAP_SIZE
    mov [drag_cur_x], ecx

    mov eax, r10d
    shr eax, 16
    and eax, 0FFFFh
    mov edx, eax
    add edx, SNAP_SIZE/2
    and edx, -SNAP_SIZE
    mov [drag_cur_y], edx

    mov rcx, [rbp-8]
    xor edx, edx
    mov r8d, 0
    call InvalidateRect
    jmp .handled_zero

.mouse_up:
    cmp dword [dragging], 0
    je .handled_zero

    mov dword [dragging], 0
    call ReleaseCapture

    mov eax, [platform_count]
    cmp eax, MAX_PLATFORMS
    jae .invalidate_after_up

    mov ecx, [drag_start_x]
    mov edx, [drag_cur_x]

    ; Require at least one snap step.
    mov r8d, ecx
    sub r8d, edx
    jns .abs_ready
    neg r8d
.abs_ready:
    cmp r8d, SNAP_SIZE
    jl .invalidate_after_up

    cmp ecx, edx
    jle .ordered
    xchg ecx, edx
.ordered:
    mov r9d, [drag_start_y]

    mov eax, [platform_count]
    imul eax, 16
    lea r10, [platforms]
    add r10, rax
    mov [r10+0], ecx
    mov [r10+4], r9d
    mov [r10+8], edx
    mov [r10+12], r9d
    inc dword [platform_count]

.invalidate_after_up:
    mov rcx, [rbp-8]
    xor edx, edx
    mov r8d, 0
    call InvalidateRect

.handled_zero:
    xor eax, eax
    mov r12, [rbp-64]
    mov r13, [rbp-72]
    leave
    ret

.paint:
    mov rcx, [rbp-8]
    lea rdx, [paint_buf]
    call BeginPaint
    mov [rbp-32], rax              ; HDC

    mov rcx, [rbp-8]
    lea rdx, [client_rect]
    call GetClientRect

    ; Entire background
    mov ecx, [col_bg]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [client_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

    ; Left panel
    mov dword [temp_rect+0], 0
    mov dword [temp_rect+4], 0
    mov dword [temp_rect+8], LEFT_PANEL
    mov eax, [client_rect+12]
    mov [temp_rect+12], eax
    mov ecx, [col_panel]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

    ; Right panel
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    mov [temp_rect+0], eax
    mov dword [temp_rect+4], 0
    mov eax, [client_rect+8]
    mov [temp_rect+8], eax
    mov eax, [client_rect+12]
    mov [temp_rect+12], eax
    mov ecx, [col_panel]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

    ; Top bar over the canvas
    mov dword [temp_rect+0], LEFT_PANEL
    mov dword [temp_rect+4], 0
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    mov [temp_rect+8], eax
    mov dword [temp_rect+12], TOP_BAR
    mov ecx, [col_top]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

    ; Canvas
    mov dword [temp_rect+0], LEFT_PANEL
    mov dword [temp_rect+4], TOP_BAR
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    mov [temp_rect+8], eax
    mov eax, [client_rect+12]
    sub eax, 26
    mov [temp_rect+12], eax
    mov ecx, [col_canvas]
    call CreateSolidBrush
    mov [rbp-40], rax
    mov rcx, [rbp-32]
    lea rdx, [temp_rect]
    mov r8, rax
    call FillRect
    mov rcx, [rbp-40]
    call DeleteObject

    ; Grid pen
    mov ecx, PS_SOLID
    mov edx, 1
    mov r8d, [col_grid]
    call CreatePen
    mov [rbp-48], rax
    mov rcx, [rbp-32]
    mov rdx, rax
    call SelectObject
    mov [rbp-56], rax

    ; Vertical grid
    mov r12d, LEFT_PANEL
    add r12d, GRID_SIZE
.vgrid:
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    cmp r12d, eax
    jge .hgrid_start

    mov rcx, [rbp-32]
    mov edx, r12d
    mov r8d, TOP_BAR
    xor r9d, r9d
    call MoveToEx

    mov rcx, [rbp-32]
    mov edx, r12d
    mov r8d, [client_rect+12]
    sub r8d, 26
    call LineTo

    add r12d, GRID_SIZE
    jmp .vgrid

.hgrid_start:
    mov r12d, TOP_BAR
    add r12d, GRID_SIZE
.hgrid:
    mov eax, [client_rect+12]
    sub eax, 26
    cmp r12d, eax
    jge .grid_done

    mov rcx, [rbp-32]
    mov edx, LEFT_PANEL
    mov r8d, r12d
    xor r9d, r9d
    call MoveToEx

    mov rcx, [rbp-32]
    mov edx, [client_rect+8]
    sub edx, RIGHT_PANEL
    mov r8d, r12d
    call LineTo

    add r12d, GRID_SIZE
    jmp .hgrid

.grid_done:
    mov rcx, [rbp-32]
    mov rdx, [rbp-56]
    call SelectObject
    mov rcx, [rbp-48]
    call DeleteObject

    ; Existing platforms
    mov ecx, PS_SOLID
    mov edx, 5
    mov r8d, [col_platform]
    call CreatePen
    mov [rbp-48], rax
    mov rcx, [rbp-32]
    mov rdx, rax
    call SelectObject
    mov [rbp-56], rax

    xor r12d, r12d
.platform_loop:
    cmp r12d, [platform_count]
    jae .platform_done

    mov eax, r12d
    imul eax, 16
    lea r13, [platforms]
    add r13, rax

    mov rcx, [rbp-32]
    mov edx, [r13+0]
    mov r8d, [r13+4]
    xor r9d, r9d
    call MoveToEx

    mov rcx, [rbp-32]
    mov edx, [r13+8]
    mov r8d, [r13+12]
    call LineTo

    inc r12d
    jmp .platform_loop

.platform_done:
    mov rcx, [rbp-32]
    mov rdx, [rbp-56]
    call SelectObject
    mov rcx, [rbp-48]
    call DeleteObject

    ; Drag preview
    cmp dword [dragging], 0
    je .text

    mov ecx, PS_SOLID
    mov edx, 4
    mov r8d, [col_preview]
    call CreatePen
    mov [rbp-48], rax
    mov rcx, [rbp-32]
    mov rdx, rax
    call SelectObject
    mov [rbp-56], rax

    mov rcx, [rbp-32]
    mov edx, [drag_start_x]
    mov r8d, [drag_start_y]
    xor r9d, r9d
    call MoveToEx

    mov rcx, [rbp-32]
    mov edx, [drag_cur_x]
    mov r8d, [drag_start_y]
    call LineTo

    mov rcx, [rbp-32]
    mov rdx, [rbp-56]
    call SelectObject
    mov rcx, [rbp-48]
    call DeleteObject

.text:
    mov rcx, [rbp-32]
    mov edx, [col_text]
    call SetTextColor
    mov rcx, [rbp-32]
    mov edx, TRANSPARENT
    call SetBkMode

    ; Brand
    mov rcx, [rbp-32]
    mov edx, 18
    mov r8d, 18
    lea r9, [txt_brand]
    mov qword [rsp+32], txt_brand_len
    call TextOutA

    ; Left panel labels
    mov rcx, [rbp-32]
    mov edx, 18
    mov r8d, 86
    lea r9, [txt_maps]
    mov qword [rsp+32], txt_maps_len
    call TextOutA

    mov rcx, [rbp-32]
    mov edx, 28
    mov r8d, 116
    lea r9, [txt_map1]
    mov qword [rsp+32], txt_map1_len
    call TextOutA

    mov rcx, [rbp-32]
    mov edx, 18
    mov r8d, 178
    lea r9, [txt_tools]
    mov qword [rsp+32], txt_tools_len
    call TextOutA

    mov rcx, [rbp-32]
    mov edx, 28
    mov r8d, 208
    lea r9, [txt_platform]
    mov qword [rsp+32], txt_platform_len
    call TextOutA

    mov rcx, [rbp-32]
    mov edx, 28
    mov r8d, 238
    lea r9, [txt_entity]
    mov qword [rsp+32], txt_entity_len
    call TextOutA

    ; Top hint
    mov rcx, [rbp-32]
    mov edx, LEFT_PANEL+18
    mov r8d, 19
    lea r9, [txt_hint]
    mov qword [rsp+32], txt_hint_len
    call TextOutA

    ; Test button label
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL+100
    mov edx, eax
    mov rcx, [rbp-32]
    mov r8d, 19
    lea r9, [txt_test]
    mov qword [rsp+32], txt_test_len
    call TextOutA

    ; Right panel
    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, [rbp-32]
    mov r8d, 86
    lea r9, [txt_props]
    mov qword [rsp+32], txt_props_len
    call TextOutA

    mov eax, [client_rect+8]
    sub eax, RIGHT_PANEL
    add eax, 18
    mov edx, eax
    mov rcx, [rbp-32]
    mov r8d, 120
    lea r9, [txt_prop_name]
    mov qword [rsp+32], txt_prop_name_len
    call TextOutA

    ; Bottom status
    mov rcx, [rbp-32]
    mov edx, LEFT_PANEL+12
    mov eax, [client_rect+12]
    sub eax, 20
    mov r8d, eax
    lea r9, [txt_status]
    mov qword [rsp+32], txt_status_len
    call TextOutA

    mov rcx, [rbp-8]
    lea rdx, [paint_buf]
    call EndPaint

    xor eax, eax
    mov r12, [rbp-64]
    mov r13, [rbp-72]
    leave
    ret
