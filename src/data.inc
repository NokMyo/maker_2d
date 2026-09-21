; Tsuramechoki game/project data model
; 100% NASM source. Shared by editor and runtime.

%ifndef TSURAMECHOKI_DATA_INC
%define TSURAMECHOKI_DATA_INC 1

; ------------------------------------------------------------
; Project / map limits
; ------------------------------------------------------------
%define MAX_MAPS            16
%define MAP_SIZE            64

; map record
; +00  char name[32]
; +32  dword background_color
; +36  dword spawn_x
; +40  dword spawn_y
; +44  dword music_id
; +48..63 reserved

; ------------------------------------------------------------
; Sprite asset database (BMP in the current native GDI renderer)
; ------------------------------------------------------------
%define MAX_SPRITES         64
%define SPRITE_SIZE         160
; +00 char name[32]
; +32 char path[128]

; ------------------------------------------------------------
; Animation database
; ------------------------------------------------------------
%define MAX_ANIMATIONS      64
%define ANIMATION_SIZE      72

; +00 char name[32]
; +32 frame_count
; +36 frame_ms
; +40 hit_start_frame
; +44 hit_end_frame
; +48 hit_offset_x
; +52 hit_offset_y
; +56 hit_width
; +60 hit_height
; +64 loop
; +68 sprite_id (-1 = placeholder/no image)

; ------------------------------------------------------------
; Skill database
; ------------------------------------------------------------
%define MAX_SKILLS          64
%define SKILL_SIZE          64

; +00 char name[32]
; +32 damage
; +36 mp_cost
; +40 cooldown_ticks
; +44 range
; +48 hit_count
; +52 knockback
; +56 animation_id
; +60 flags

; ------------------------------------------------------------
; Monster database
; ------------------------------------------------------------
%define MAX_MONSTERS        64
%define MONSTER_SIZE        64

%define AI_IDLE             0
%define AI_CHASE            1
%define AI_RANGED           2
%define AI_BOSS             3

; +00 char name[32]
; +32 max_hp
; +36 attack
; +40 move_speed
; +44 exp_reward
; +48 drop_item_id
; +52 drop_count
; +56 ai_type
; +60 animation_id

; ------------------------------------------------------------
; Item database
; ------------------------------------------------------------
%define MAX_ITEMS           128
%define ITEM_SIZE           64

%define ITEM_CONSUMABLE     0
%define ITEM_WEAPON         1
%define ITEM_ARMOR          2
%define ITEM_MATERIAL       3
%define ITEM_QUEST          4

; +00 char name[32]
; +32 type
; +36 value
; +40 buy_price
; +44 sell_price
; +48 attack_bonus
; +52 defense_bonus
; +56 max_hp_bonus
; +60 flags

; ------------------------------------------------------------
; Quest database
; ------------------------------------------------------------
%define MAX_QUESTS          64
%define QUEST_SIZE          64

%define QUEST_KILL          0
%define QUEST_TALK          1
%define QUEST_COLLECT       2
%define QUEST_REACH         3

%define QUEST_LOCKED        0
%define QUEST_ACTIVE        1
%define QUEST_COMPLETE      2
%define QUEST_REWARDED      3

; +00 char name[32]
; +32 objective_type
; +36 target_id
; +40 target_count
; +44 reward_exp
; +48 reward_money
; +52 reward_item
; +56 reward_count
; +60 flags

; ------------------------------------------------------------
; Game variables / flags
; ------------------------------------------------------------
%define MAX_VARIABLES       128
%define VARIABLE_SIZE       40

; +00 char name[32]
; +32 value
; +36 flags

; ------------------------------------------------------------
; Event database / command interpreter
; ------------------------------------------------------------
%define MAX_EVENTS          64
%define EVENT_COMMANDS      8
%define EVENT_COMMAND_SIZE  16
%define EVENT_SIZE          176

%define EVENT_DIALOGUE      1
%define EVENT_SET_VAR       2
%define EVENT_ADD_ITEM      3
%define EVENT_ADD_MONEY     4
%define EVENT_CHANGE_MAP    5
%define EVENT_START_QUEST   6
%define EVENT_SET_FLAG      7

; event:
; +00 char name[32]
; +32 command_count
; +36..47 reserved
; +48 commands[8]
;
; command:
; +00 opcode
; +04 a
; +08 b
; +12 c
;
; ------------------------------------------------------------
; Database header appended to TSRP project data
; ------------------------------------------------------------
%define DATABASE_HEADER_SIZE 32
; +00 animation_count
; +04 skill_count
; +08 monster_count
; +12 item_count
; +16 quest_count
; +20 variable_count
; +24 event_count
; +28 sprite_count

; ------------------------------------------------------------
; Runtime save data
; ------------------------------------------------------------
%define SAVE_MAGIC_QWORD    0x3130303056415354 ; "TSAV0001"
%define SAVE_VERSION        1
%define SAVE_HEADER_SIZE    64

; save header:
; +00 magic
; +08 version
; +12 active_map
; +16 player_x
; +20 player_y
; +24 hp
; +28 mp
; +32 exp
; +36 level
; +40 money
; +44 quest_count
; +48 variable_count
; +52 item_count
; +56 reserved
; +60 reserved
; followed by:
; quest_state[MAX_QUESTS] dwords
; quest_progress[MAX_QUESTS] dwords
; variable_values[MAX_VARIABLES] dwords
; inventory_counts[MAX_ITEMS] dwords

%endif
