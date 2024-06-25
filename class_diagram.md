```mermaid
classDiagram
	_MISP <|-- multi_inherit_this

	multi_inherit_this <|-- entity
    entity <|-- object
    object <|-- rigidbody
    rigidbody <|-- character
    rigidbody <|-- projectile
    
	multi_inherit_this <|-- stateController
	stateController <|-- CharacterController
	CharacterController <|-- character

    character <|-- CannonCharacter
    character <|-- MissileCharacter
	character <|-- SecwindCharacter

    projectile <|-- CannonProjectile
	projectile <|-- PrecisionCannonProjectile
    projectile <|-- GuidedMissileProjectile
    projectile <|-- MissileProjectile
	projectile <|-- EnergyBallProjectile
    projectile <|-- MultiEnergyBallProjectile
	projectile <|-- NutShellProjectile
    
	rigidbody <|-- Ground
	Ground <|-- Stairway

    entity <|-- Layer
    entity <|-- scene
    entity <|-- Resource
    entity <|-- Texture

	Camera

	CameraManager
	ResourceManager
	ObjectManager
	SoundManager
    Resource <|-- ImageWrapper
    ImageWrapper <|-- GifWrapper
	Resource <|-- Sound

	Timer <|-- entity
	Timer <|-- NextPlayerTime
	Timer <|-- ProjectileTimer

	entity <|-- item
	item <|-- DoubleShotItem
	item <|-- TeleportItem
	item <|-- RepairItem

    scene <|-- TitleScene
    scene <|-- BattleScene
    scene <|-- BulletinBoardScene
    scene <|-- LoadingScene
    scene <|-- LobbyScene
    scene <|-- RoomScene
	scene <|-- SummaryScene

	BattleScene <|-- SkyValleyMap
	BattleScene <|-- DesertMap

	Round
	Radar
	Vector2
	Debug
	Application
	Texture
	SoundPack

	class multi_inherit_this {
		std::shared_ptr< T > shared_from_this()
		std::shared_ptr< U > downcast_from_this()
	}

	class Vector2 {
		Vector2 top() const noexcept
		Vector2 bottom() const noexcept
		Vector2 left() const noexcept
		Vector2 right() const noexcept

		float magnitude() const
		float inner_product(const Vector2& other) const
		float global_inner_angle(const Vector2& other) const
		float local_inner_angle(const Vector2& other) const
		float dot_product(const Vector2& other) const
		float global_angle() const noexcept
		float get_x() const noexcept
		Vector2 rotate(float radian) const noexcept
		float get_y() const noexcept
		Vector2 reflect_x() const noexcept
		Vector2 reflect_y() const noexcept
		Vector2 abs() const noexcept
		Vector2 normalized() const noexcept

		-float m_x
		-float m_y
	}

	class Debug {
		+static void initialize(HDC hdc)
		+static void Log(const std::wstring& str)
		+static void set_debug_flag()
		+static void push(std::function< void() > func)

		+static void draw_line(const Math::Vector2 left, const Math::Vector2 right)
		+static void draw_dot(const Math::Vector2 point)
		+static void draw_circle(Math::Vector2 point, float radius)
		+static void draw_rect(Math::Vector2 point, Math::Vector2 size)
		+static void render()

		-inline static bool m_bDebug = false
		-static constexpr int y_movement = 15
		-static constexpr int y_initial = 30
		-inline static int x = 100
		-inline static int y = y_initial
		-inline static HDC m_hdc
		-inline static std::queue< std::function< void() > > m_render_queue
	}

    class entity {
        +const std::wstring& get_name() const noexcept
        -std::wstring m_name
    }

    class scene {
        +virtual void initialize()
		+virtual void update()
		+virtual void render()
		+virtual void deactivate()
		+virtual void activate()

		+std::vector< std::weak_ptr< T > > is_in_range(const Math::Vector2& mid_point, float radius)

		+void add_game_object(LayerType layer_type, const std::weak_ptr< object >& obj)
		+void remove_game_object(LayerType layer_type, const std::weak_ptr< object >& obj)

		+std::vector< std::weak_ptr< T > > get_objects()
		+std::weak_ptr< Camera > get_camera()

		+std::vector< std::weak_ptr< object > > get_objects()
		-std::weak_ptr< Camera > m_camera
		-std::vector< std::weak_ptr< object > > m_objects
		-std::vector< Layer > m_layers
    }

    class Layer {
        +void initialize()
		+void update() const
		+void render() const
		+void deactivate() const
		+void activate() const
		+void add_game_object(const std::weak_ptr< object >& object)
		+void remove_game_object(const std::weak_ptr< object >& obj)
        -std::vector< std::weak_ptr< object > > m_objects
    }

    class Resource {
        +virtual bool load() = 0
		+const std::filesystem::path& get_path() const
		+void set_path(const std::filesystem::path& path)
		-std::filesystem::path m_path
    }

	class Sound {
		+virtual bool load() override
		+void play(bool loop)
		+void stop(bool reset)
		+void set_position(float position, bool loop)
		+void set_volume(float volume)
		+int get_decibel(float volume) const
		+bool is_playing() const
		-bool load_wav_file(const std::filesystem::path& path)

		-LPDIRECTSOUNDBUFFER	m_sound_buffer
		-DSBUFFERDESC m_buffer_desc
		-int m_volume
		-bool m_bPlaying
	}

	class SoundPack {
		+std::weak_ptr< Resource::Sound > get_sound(const std::wstring& category)
		-std::map< std::wstring, std::weak_ptr< Resource::Sound > > m_sounds
	}

    class Texture {
        +std::weak_ptrT get_imageT(const std::wstring& category, const std::wstring& orientation)
	    -std::map< std::wstring, std::weak_ptr< T >> m_images
    }

    class object {
        +virtual void render()
		+virtual void update()

		+virtual void move_left()
		+virtual void move_right()
		+virtual void move_down()
		+virtual void move_up()
		+float get_x() const
		+float get_y() const
		+Math::Vector2 get_position() const
		+virtual Math::Vector2 operator+(const Math::Vector2& vector) const
		+virtual object& operator+=(const Math::Vector2& vector)

		+bool is_active() const
		+virtual void set_disabled()
		+virtual void set_enabled()

		+Math::Vector2 get_top() const
		+Math::Vector2 get_bottom() const
		+Math::Vector2 get_left() const
		+Math::Vector2 get_right() const
		+Math::Vector2 get_top_left() const
		+Math::Vector2 get_top_right() const
		+Math::Vector2 get_bottom_left() const
		+Math::Vector2 get_bottom_right() const
		+Math::Vector2 get_center() const

		Math::Vector2 get_hit_point(const eHitVector& e_vector) const
		Math::Vector2 to_top_left_local_position(const Math::Vector2& other) const
		Math::Vector2 to_top_right_local_position(const Math::Vector2& other) const
		Math::Vector2 to_nearest_local_position(const Math::Vector2& other) const
		Math::Vector2 get_nearest_point(const Math::Vector2& other) const

		+float get_mass() const

        -float m_mass
		-bool m_bActive
    }

    class rigidbody {
        +void initialize() override
		+void update() override
		+virtual void prerender()
		+static void block_window_frame(const std::weak_ptr< rigidbody >& target)

		+virtual void on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr< rigidbody >& other)
		+virtual void on_nocollison()
		+static CollisionCode is_collision(const std::weak_ptr< object >& left, const std::weak_ptr< object >& right) noexcept

		+void set_speed(const Math::Vector2& speed)

		+void move_down() override
		+void move_left() override
		+void move_right() override
		+void move_up() override
		+virtual void stop()

		+void set_movement_pitch_radian(const float pitch)
		+void set_user_pitch_radian(const float pitch)
		+float get_movement_pitch_radian() const
		+float get_user_pitch_radian() const

		+const Math::Vector2& get_offset() const
		+Math::Vector2 get_mixed_offset() const
		+Math::Vector2 get_velocity_offset() const
		+Math::Vector2 get_offset_forward_position() const
		+Math::Vector2 get_offset_bottom_forward_position() const
		+Math::Vector2 get_velocity_forward_position() const

		+void set_offset(const Math::Vector2& offset)

		-float m_movement_pitch_radian
		-float m_user_pitch_radian
		-Math::Vector2 m_speed
		-Math::Vector2 m_curr_speed
		-bool m_bGravity

		-Math::Vector2 m_offset

		-void apply_gravity()

		-virtual void move()

		-void reset_current_speed()
		-void reset_current_gravity_speed()
		-void disable_gravity()
		-void enable_gravity()


		-float m_gravity_speed
		-float m_gravity_acceleration
    }

	class stateController {
		+StateEnum get_state()
		+virtual void initialize()
		+virtual void update() = 0
		+virtual void prerender()
		-void set_state(const StateEnum&)

		-virtual void set_current_sprite(const StateEnum&) = 0
		-AnimFlag is_anim_finished() const
		-void reset_anim_counter()

		-Texture< GifWrapper > m_texture
		-SpritePointer m_current_sprite
		-SoundPack m_sound_pack

		-StateEnum m_state
		-AnimElapsedFloat m_anim_elapsed
	}

	class CharacterController {
		+void initialize() override
		+void update() override
		+void prerender() override

		+void set_unmovable()
		+void set_movable()

		+bool is_projectile_fire_counted() const
		+bool is_projectile_active() const

		+float get_charged_power() const
		+float get_hp_percentage() const
		+float get_mp_percentage() const
		+float get_hp_raw() const

		+const std::vector< std::weak_ptr< ObjectBase::projectile > >& get_projectiles() const
		+const eProjectileType& get_projectile_type() const
		+const std::weak_ptr< ObjectBase::projectile >& get_one_active_projectile()

		-friend class Object::item

		-void set_sprite_offset(const std::wstring& name, const std::wstring& orientation, const Math::Vector2& offset)
		-const std::wstring& get_current_sprite_name() const

		-virtual void fire()
		-virtual void move_left()
		-virtual void move_right()
		-virtual void stop()
		-virtual void move()

		-void apply_damage(float damage)
		-void set_hp(const float hp)

		-void add_active_projectile(const std::weak_ptr< ObjectBase::projectile >& prj)

		-void equip_nutshell()
		-void unequip_nutshell()

		-virtual std::weak_ptr< ObjectBase::projectile > get_main_projectile() = 0
		-virtual std::weak_ptr< ObjectBase::projectile > get_sub_projectile() = 0

		static std::wstring anim_name_getter(const eCharacterState& state)
		void set_current_sprite(const eCharacterState&) override

		-void default_state()
		-void idle_state()
		-void move_state()
		-void firing_state()
		-void fire_state()
		-void fired_state()
		-void hit_state()
		-void turn_end_state()
		-void preitem_state()
		-void item_state()
		-void death_state()
		-void dead_state()
		-void set_item_active(int n)

		-void change_projectile()
		-void reset_mp()

		-float m_hp
		-float m_mp
		-float m_power
		-bool m_bMovable

		-const Math::Vector2& m_ref_velocity
		-const Math::Vector2& m_ref_offset

		-eProjectileType m_projectile_type
		-eProjectileType m_tmp_projectile_type

		-std::map< int, std::shared_ptr< Object::item > > m_available_items
		-std::weak_ptr< Object::item > m_active_item

		-std::vector< std::weak_ptr< ObjectBase::projectile > > m_active_projectiles
	}

    class character {
        +void initialize() override
		+void update() override
		+void render() override
		+void prerender() override

		+void hit(const std::weak_ptr< projectile >& p, const Math::Vector2& hit_point)

		+const std::wstring& get_short_name() const

		+void on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr< Abstract::rigidBody >& other) override
		+void on_nocollison() override

		+float get_armor() const

		-bool m_bGrounded

		-const std::wstring m_short_name
		-float m_armor

		-float get_damage_pen_dist(const std::weak_ptr< projectile >& p, const Math::Vector2& hit_point) const

		-void move() override
		-void fire() override
		-void move_left() override
		-void move_right() override
		-void stop() override

		-void render_hp_bar(const Math::Vector2& position)

		-void ground_walk(const CollisionCode& collision, const std::weak_ptr< Object::Ground >& ptr_ground)
		-void ground_gravity(const std::weak_ptr< Object::Ground >& ptr_ground)
		-void ground_pitching(const std::weak_ptr< Object::Ground >& ptr_ground)

		-Math::Vector2 get_next_velocity(const Math::Vector2& local_position_bottom, const std::weak_ptr< Object::Ground >& ground_ptr) const

		-std::weak_ptr< projectile > initialize_projectile(const Math::Vector2& angle, const float charged)

		-ProjectileTimer m_multi_projectile_timer
    }

    class projectile {
		+static Math::Vector2 projectile_speed_getter(const std::wstring& short_name, const std::wstring& type)

		+void initialize() override
		+void fire(const Math::Vector2& position, const Math::Vector2& velocity, const float charged)
		+void update() override
		+void on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr< rigidbody >& other) override
		+void render() override
		+void prerender() override
		+const character* get_origin() const

		+void explosion_near_ground(const Math::Vector2& hit_vector) const

		+void reset_cooldown()
		+bool is_cooldown() const
		+bool is_exploded() const
		+void up_hit_count()
		+int get_hit_count() const
		+int get_fire_count() const
		+int get_max_hit_count() const
		+virtual void play_fire_sound() = 0
		+virtual void play_hit_sound() = 0

		+const std::weak_ptr< GifWrapper >& get_current_sprite() const
		+int get_radius() const
		+float get_damage() const
		+float get_penetration_rate() const
		+const Math::Vector2& get_fired_position() const

		-float m_damage
		-int m_radius
		-const int m_max_hit_count
		-int m_curr_hit_count
		-int m_fire_count

		-float m_hit_cooldown
		-float m_armor_penetration

		-bool m_bExploded

		-Math::Vector2 m_wind_acceleration

		-const character* m_shooter
		-Texture< GifWrapper > m_texture
		-std::weak_ptr< GifWrapper > m_current_sprite
		-Math::Vector2 m_fired_position
		-Math::Vector2 m_previous_position

		-virtual void post_hit()

		-SoundPack m_sound_pack
    }

    class MissileCharacter {
        +void initialize() override
		-std::weak_ptr< ObjectBase::projectile > get_main_projectile() override
		-std::weak_ptr< ObjectBase::projectile > get_sub_projectile() override
    }

    class CannonCharacter {
        +void initialize() override
		-std::weak_ptr< ObjectBase::projectile > get_main_projectile() override
		-std::weak_ptr< ObjectBase::projectile > get_sub_projectile() override
    }

	class MissileCharacter {
        +void initialize() override
		-std::weak_ptr< ObjectBase::projectile > get_main_projectile() override
		-std::weak_ptr< ObjectBase::projectile > get_sub_projectile() override
    }

	class SecwindCharacter {
        +void initialize() override
		-std::weak_ptr< ObjectBase::projectile > get_main_projectile() override
		-std::weak_ptr< ObjectBase::projectile > get_sub_projectile() override
    }

    class MissileProjectile {
		+void initialize() override
		+virtual void play_hit_sound() override
		+virtual void play_fire_sound() override
    }

	class GuidedMissileProjectile {
		+void initialize() override
		+void update() override
		+virtual void play_hit_sound() override
		+virtual void play_fire_sound() override
    }

	class CannonProjectile {
        +void initialize() override
        +virtual void play_hit_sound() override
		+virtual void play_fire_sound() override
    }

    class PrecisionCannonProjectile {
        +void initialize() override
        +virtual void play_hit_sound() override
		+virtual void play_fire_sound() override
    }

	class EnergyBallProjectile {
        +void initialize() override
        +virtual void play_hit_sound() override
		+virtual void play_fire_sound() override
    }

    class MultiEnergyBallProjectile {
        +void initialize() override
        +virtual void play_hit_sound() override
		+virtual void play_fire_sound() override
    }

	class NutShellProjectile {
		+void initialize() override
		+virtual void play_hit_sound() override
		+virtual void play_fire_sound() override
	}

    class Ground {
        +void initialize() override
		+void render() override

		+GroundState safe_is_destroyed(const Math::Vector2& local_position) const
		+void safe_set_destroyed_global(const Math::Vector2& hit_position, const float radius)
		+bool safe_is_object_stuck_global(const Math::Vector2& position) const
		+bool safe_is_object_stuck_local(const Math::Vector2& position) const
		+Math::Vector2 safe_nearest_surface(const Math::Vector2& global_position) const
		+Math::Vector2 safe_orthogonal_surface_global(const Math::Vector2& global_position, const int depth = -1, const int start_y = -1) const

		-HDC get_ground_hdc() const
		-void unsafe_set_destroyed(const int x, const int y)
		-void unsafe_set_destroyed_visual(int x, int y)
		-void safe_set_circle_destroyed(const Math::Vector2& center_position, const int radius)
		-Math::Vector2 safe_orthogonal_surface_local(const Math::Vector2& local_position, const int depth) const
		-void unsafe_set_line_destroyed(const Math::Vector2& line, const int n)
		-void unsafe_set_line_destroyed_reverse(const Math::Vector2& line, int n)
		-bool safe_is_projectile_hit(const Math::Vector2& hit_position, const std::weak_ptr< ObjectBase::projectile >& projectile_ptr) const
		-void _debug_draw_destroyed_table() const

		-friend Radar
		-std::vector< std::vector< GroundState > > m_destroyed_table
		-HDC m_ground_hdc
		-HBITMAP m_ground_bitmap
    }

	class Stairway {
		+void initialize() override;
		-Math::Vector2 m_ramp_offset;
	}

    class ObjectManager {
        -static std::vector< std::shared_ptr< object > > m_objects
        +static std::weak_ptr< T > create_object(Args ... args)
		+static void remove_object(const std::weak_ptr< Abstract::object >& obj)
		+static void cleanup()
    }

    class ResourceManager {
		+static std::weak_ptr< T > load(const std::wstring& name, const std::filesystem::path& path)
		+static std::weak_ptr< T > find(const std::wstring& name) noexcept
		+static void cleanup()
        -static std::shared_ptr< T > find_internally(const std::wstring& name) noexcept
		-inline static std::map< std::wstring, std::shared_ptr< Abstract::Resource > > m_resources
    }

    class CameraManager {
        +static std::weak_ptr create_camera(Args... args)
		+static void cleanup()
        -static std::vector< std::shared_ptr< Camera > > m_cameras
    }

    class Camera {
        +void update()
		+void initialize()
		+void set_object(const std::weak_ptr< Abstract::object >& obj)
		+Math::Vector2 get_relative_position(const std::weak_ptr< Abstract::object >& obj) const
		+Math::Vector2 get_offset() const
		+Math::Vector2 get_offset(const Math::Vector2& hitbox) const
		std::weak_ptr< Abstract::object > get_locked_object() const
		Math::Vector2 m_target_center
		Math::Vector2 m_window_size
		Math::Vector2 m_center_position
		std::weak_ptr< Abstract::object > m_lock_target
    }

    class DeltaTime {
		+static void initialize()
		+static void update()
		+static void render()
		+static float get_deltaTime()
        -static LARGE_INTEGER m_cpu_frequency
		-static LARGE_INTEGER m_prev_frequency
		-static LARGE_INTEGER m_curr_frequency
		-static float m_deltaTime
    }

    class Input {
        struct Key
        +static void initialize()
		+static void update()
		+static bool getKeyDown(eKeyCode)
		+static bool getKeyUp(eKeyCode)
		+static bool getKey(eKeyCode)
        -static std::vector< Key > m_keys
		-static void checkKeyState()
    }

    class WinAPIHandles {
        +static void cleanup()
		+static void initialize(HWND hwnd, HDC hdc)
		+static int get_window_width()
		+static int get_window_height()
		+static HDC get_buffer_dc()
		+static HDC get_main_dc()
		+static HWND get_hwnd()
		+static int get_actual_max_y()
		+static RECT& get_window_size()

		-inline static GdiplusStartupInput input
		-inline static ULONG_PTR token
		-inline static RECT m_window_size
		-inline static RECT m_native_size
		-inline static HWND m_hwnd = nullptr
		-inline static HDC m_hdc = nullptr
		-inline static HDC m_buffer_hdc = nullptr
		-inline static HBITMAP m_buffer_bitmap = nullptr
    }

    class Application {
        +void initialize(HWND, HDC)
		+void update()
		+void render()
		+static void cleanup()
		-HWND m_hwnd
		-HDC m_hdc
		-HDC m_buffer_hdc
    }

    class GifWrapper {
        +virtual bool load() override
		+virtual void initialize()
		+static void cleanup()

		+virtual bool load() override
		+virtual void initialize()

		+void play(const std::function< void() >& on_end)
		+void stop() const
		+void OnTimer()
		+virtual void flip() override
		+virtual void rotate(const float angle)
		+void reset_transform()
		+unsigned int get_total_play_time() const

		+inline static std::map< UINT_PTR, GifWrapper* > registered_gifs
        
        -UINT_PTR m_timer_id
		-UINT m_dimension_count
		-UINT m_frame_count
		-UINT m_total_buffer
		-UINT m_current_frame
		-WCHAR m_str_guid[39]
		-std::vector< unsigned int > m_frame_delays
		-std::function< void() > m_reserved_function
		-inline static UINT used_timer_id = gif_timer_id
    }

    class ImageWrapper {
        +void cleanup()
		+virtual void render(const Math::Vector2& position, const Math::Vector2& hitbox, const Math::Vector2& scaling, const float rotate_degree)
		+const Math::Vector2& get_hitbox() const
		+virtual void flip()
		+void set_offset(const Math::Vector2& offset)

        -virtual bool load() override
		-std::shared_ptr< Image > m_image
		-std::shared_ptr< Graphics > m_gdi_handle
		-Math::Vector2 m_size
		-Math::Vector2 m_offset
    }

	class DesertMap {
		+void pre_initialize() override
		+void set_bgm() override
		+void set_background_img() override
		+void set_grounds() override
		+void set_characters() override
		+void set_client_character() override
	}

	class SkyValleyMap {
		+void pre_initialize() override
		+void set_bgm() override
		+void set_background_img() override
		+void set_grounds() override
		+void set_characters() override
		+void set_client_character() override
	}

	class item {
		+virtual void initialize() = 0
		+virtual void update(const std::weak_ptr< ObjectBase::character >& owner) = 0
		+virtual bool is_effect_ended()
		+bool is_instant() const
		+virtual void reset()
		+void set_ended()

		-void set_hp(const std::weak_ptr< ObjectBase::character >& owner, float hp) const
		-void equip_nutshell(const std::weak_ptr< ObjectBase::character >& owner)
		-void unequip_nutshell(const std::weak_ptr< ObjectBase::character >& owner)
		-void fire(const std::weak_ptr< ObjectBase::character >& owner) const

		-bool m_instant
		-bool m_is_used
		-bool m_is_ended
	}

	class DoubleShotItem {
		+void initialize() override
		+void update(const std::weak_ptr< ObjectBase::character >& owner) override
		+void reset() override
		-int m_fire_count
	}

	class RepairItem {
		+void initialize() override
		+void update(const std::weak_ptr< ObjectBase::character >& owner) override
	}

	class TeleportItem {
		+void initialize() override
		+void update(const std::weak_ptr< ObjectBase::character >& owner) override
	}

	class Radar {
		+void initialize()
		+void update() const
		+void render() const

		+HDC get_radar_hdc() const

		-Math::Vector2 m_center
		-BLENDFUNCTION m_bf
		-HDC m_radar_hdc
		-HBITMAP m_radar_bitmap
	}

	class Round {
		+void initialize(const std::vector< std::weak_ptr< ObjectBase::character > >& players)
		+void update()
		+float get_current_time() const
		+float get_wind_acceleration() const
		+const std::weak_ptr< ObjectBase::character >& get_current_player() const
		+const eRoundState& get_current_status() const

		-void check_countdown()
		-void check_fired()
		-void pre_next_player()
		-void next_player()
		-void check_winning_condition()
		-void winner()
		
		-float m_curr_timeout = 0.0f
		-bool m_bfired = false

		-NextPlayerTimer m_timer_next_player

		-float m_wind_affect = 0.0f
		-inline static std::default_random_engine e
		-inline static std::uniform_real_distribution< float > dis

		-eRoundState m_state
		-std::vector< std::weak_ptr< ObjectBase::character > > m_known_players
		-std::vector< std::weak_ptr< ObjectBase::character > > m_all_players
		-std::weak_ptr< ObjectBase::character > m_current_player
		-std::weak_ptr< ObjectBase::character > m_winner
		-struct safe_weak_comparer
	}

	class BattleScene {
		+void initialize() override
		+void pre_initialize() = 0
		+void update() override
		+void render() override
		+void deactivate() override
		+void activate() override
		+std::weak_ptr< Round > get_round_status()
		+const Math::Vector2& get_map_size() const

		+virtual void set_bgm() = 0
		+virtual void set_background_img() = 0
		+virtual void set_grounds() = 0
		+virtual void set_characters() = 0
		+virtual void set_client_character() = 0

		-Math::Vector2 m_map_size
		-std::weak_ptr< ObjectBase::character > m_self
		-std::vector< std::weak_ptr< ObjectBase::character > > m_characters
		-std::vector< std::weak_ptr< Object::Ground > > m_grounds
		-std::weak_ptr< Resource::Sound > m_bgm
		-std::weak_ptr< ImageWrapper > m_hud
		-std::weak_ptr< ImageWrapper > m_background
		-std::shared_ptr< Round > m_round
		-Radar m_radar
	}

	class BulletinBoardScene {
		+void initialize() override
		+void update() override
		+void render() override
		+void deactivate() override
		+void activate() override

		+std::weak_ptr< ImageWrapper > m_imBackground
		+std::weak_ptr< Resource::Sound > m_sound_bgm
	}

	class LoadingScene {
		+void initialize() override
		+void update() override
		+void render() override
		+void deactivate() override
		+void activate() override

		+std::weak_ptr< ImageWrapper > m_imBackground
	}

	class LobbyScene {
		+void initialize() override
		+void update() override
		+void render() override
		+void deactivate() override
		+void activate() override

		-std::weak_ptr< ImageWrapper > m_imBackground
		-std::weak_ptr< Resource::Sound > m_bgm
	}

	class RoomScene {
		+void initialize() override
		+void update() override
		+void render() override
		+void deactivate() override
		+void activate() override

		-std::weak_ptr< ImageWrapper > m_imBackground
		-std::weak_ptr< Resource::Sound > m_bgm
	}

	class SummaryScene {
		+void initialize() override
		+void update() override
		+void render() override
		+void deactivate() override
		+void activate() override

		-std::weak_ptr< ImageWrapper > m_imBackground
		-std::weak_ptr< Resource::Sound > m_bgm
		-std::shared_ptr< Round > m_round
	}

	class TitleScene {
		+void initialize() override
		+void update() override
		+void render() override
		+void deactivate() override
		+void activate() override

		+std::weak_ptr< ImageWrapper > m_imBackground
		+std::shared_ptr< ObjectBase::character > m_object
	}

	class Timer {
		+void initialize()
		+void start(const std::function< void() >& on_end)
		+bool is_started() const
		+virtual void on_timer()
		+virtual void reset()

		+inline static std::map< UINT_PTR, Timer* > registered_timer

		-UINT_PTR m_timer_id
		-UINT m_duration
		-inline static int used_timer_id = timer_id
		-bool m_bStarted
	}

	class ProjectileTime {
		+void on_timer() override
		+void set_count(int count)
		+void reset() override
		-int m_count
		-int m_curr_count
	}

	class NextPlayerTimer {
		+inline void on_timer() override
	}
```
