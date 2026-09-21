; Tsuramechoki project format
; This file is NASM source and is shared by the editor and runtime.

%ifndef TSURAMECHOKI_PROJECT_INC
%define TSURAMECHOKI_PROJECT_INC 1

%define PROJECT_MAGIC_QWORD 0x3130303050525354 ; "TSRP0001" little-endian
%define PROJECT_VERSION     1
%define PROJECT_HEADER_SIZE 24

%define MAX_PLATFORMS       256
%define PLATFORM_SIZE       16
%define MAX_ENTITIES        128
%define ENTITY_SIZE         16

%define ENTITY_NONE         0
%define ENTITY_PLAYER       1
%define ENTITY_MONSTER      2
%define ENTITY_NPC          3
%define ENTITY_PORTAL       4

; Header layout:
; +00 qword magic "TSRP0001"
; +08 dword version
; +12 dword platform_count
; +16 dword entity_count
; +20 dword reserved
;
; Platform layout:
; +00 dword x1
; +04 dword y1
; +08 dword x2
; +12 dword y2
;
; Entity layout:
; +00 dword type
; +04 dword x
; +08 dword y
; +12 dword param

%endif
