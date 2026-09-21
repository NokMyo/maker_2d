; Tsuramechoki project format v2
; Shared by editor and runtime. 100% NASM source.

%ifndef TSURAMECHOKI_PROJECT_INC
%define TSURAMECHOKI_PROJECT_INC 1

%include "data.inc"

%define PROJECT_MAGIC_QWORD 0x3230303050525354 ; "TSRP0002" little-endian
%define PROJECT_VERSION     2
%define PROJECT_HEADER_SIZE 64

%define MAX_PLATFORMS       1024
%define PLATFORM_SIZE       16
%define MAX_ENTITIES        512
%define ENTITY_SIZE         16

%define ENTITY_NONE         0
%define ENTITY_PLAYER       1
%define ENTITY_MONSTER      2
%define ENTITY_NPC          3
%define ENTITY_PORTAL       4
%define ENTITY_LADDER       5
%define ENTITY_ROPE         6
%define ENTITY_CHECKPOINT   7
%define ENTITY_DECOR        8
%define ENTITY_EVENT        9

; ------------------------------------------------------------
; Project header v2 — 64 bytes
; ------------------------------------------------------------
; +00 qword magic "TSRP0002"
; +08 dword version
; +12 dword header_size
; +16 dword platform_count
; +20 dword entity_count
; +24 dword map_count
; +28 dword active_map
; +32 dword screen_width
; +36 dword screen_height
; +40 dword player_move_speed
; +44 dword player_jump_speed (signed, negative = upward)
; +48 dword player_max_hp
; +52 dword player_max_mp
; +56 dword player_base_attack
; +60 dword project_flags
;
; ------------------------------------------------------------
; Serialized data order
; ------------------------------------------------------------
; header
; platform[platform_count]
; entity[entity_count]
; platform_map_id[platform_count] dword
; entity_map_id[entity_count] dword
; map[map_count]
; database_header
; animation[animation_count]
; skill[skill_count]
; monster[monster_count]
; item[item_count]
; quest[quest_count]
; variable[variable_count]
; event[event_count]
;
; ------------------------------------------------------------
; Platform record — 16 bytes
; ------------------------------------------------------------
; +00 dword x1
; +04 dword y1
; +08 dword x2
; +12 dword y2
;
; Map ownership is stored in the parallel platform_map_id array.
;
; ------------------------------------------------------------
; Entity record — 16 bytes
; ------------------------------------------------------------
; +00 dword type
; +04 dword x
; +08 dword y
; +12 dword param
;
; param:
; monster -> monster database id
; NPC     -> quest id
; portal  -> target map id
; ladder/rope -> optional length
; event trigger -> event database id
;
; Map ownership is stored in the parallel entity_map_id array.

%endif
