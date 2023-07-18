```mermaid
classDiagram
    entity <|-- object
    object <|-- rigidbody
    rigidbody <|-- character
    rigidbody <|-- projectile
    
    character <|-- CannonCharacter
    character <|-- MissileCharacter

    projectile <|-- CannonProjectile
    projectile <|-- MissileProjectile

    rigidbody <|-- Ground

    entity <|-- Layer
    entity <|-- Scene
    entity <|-- Resource
    entity <|-- Texture

    Resource <|-- ImageWrapper
    ImageWrapper <|-- GifWrapper

    Scene <|-- TitleScene
    Scene <|-- BattleScene
    Scene <|-- BulletinBoardScene
    Scene <|-- LoadingScene
    Scene <|-- LobbyScene
    Scene <|-- RoomScene

    class entity {
        +const std::wstring& get_name() const noexcept
        -std::wstring m_name
    }

    class Scene {
        +virtual void initialize();
		+virtual void update();
		+virtual void render();
		+virtual void deactivate();
		+virtual void activate();

		+std::vector<std::weak_ptr<T>> is_in_range(const Math::Vector2& top_left, const Math::Vector2& hit_box, float radius)

		+void add_game_object(LayerType layer_type, const std::weak_ptr<object>& obj);
		+void remove_game_object(LayerType layer_type, const std::weak_ptr<object>& obj);
		+std::weak_ptr<Camera> get_camera();

		+std::vector<std::weak_ptr<object>> get_objects()

        -std::weak_ptr<Camera> m_camera
		-std::vector<std::weak_ptr<object>> m_objects
		-std::vector<Layer> m_layers
    }

    class Layer {
        +void initialize()
		+void update() const
		+void render() const
		+void deactivate() const
		+void activate() const
		+void add_game_object(const std::weak_ptr<object>& object)
		+void remove_game_object(const std::weak_ptr<object>& obj)
        -std::vector<std::weak_ptr<object>> m_objects
    }

    class Resource {
        +virtual bool load()
		+const std::filesystem::path& get_path() const
		+void set_path(const std::filesystem::path& path)
		-std::filesystem::path m_path;
    }

    class Texture {
        +std::weak_ptrT get_imageT(const std::wstring& category, const std::wstring& orientation)
	    -std::map<std::wstring, std::weak_ptrT> m_images
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
		+float get_mass() const;

        -float m_mass;
		-bool m_bActive;
    }

    class rigidbody {
        +void initialize() override
		+void update() override
		+static void block_window_frame(const std::weak_ptr<rigidBody>& target)

		+virtual void on_collision(const CollisionCode& collison, const std::shared_ptr<rigidBody>& other)
		+virtual void on_nocollison()
		+static CollisionCode is_collision(const std::weak_ptr<object>& left, const std::weak_ptr<object>& right) noexcept

		+void set_speed(const float speed)

		+void move_down() override
		+void move_left() override
		+void move_right() override
		+void move_up() override
		+virtual void stop()

		-void apply_gravity();
		-bool update_reflection(CollisionCode code) noexcept;
        -virtual void move();

		-void reset_current_speed();
		-void reset_current_gravity_speed();
		-void disable_gravity();
		-void enable_gravity();

        -float m_speed
		-float m_curr_speed
		-bool m_bGravity
		-bool m_bClipping
		-float m_gravity_speed
		-float m_gravity_acceleration
    }

    class character {
        +virtual void initialize() override
		+void hit(const std::weak_ptr<projectile>& p)
		+void update() override
		+void render() override
		+virtual void firing()
		+virtual void shoot()
		+float get_charged_power() const
		+virtual void move() override

		+virtual void on_collision(const CollisionCode& collision, const std::shared_ptr<rigidBody>& other) override
		+virtual void on_nocollison() override;

		+void set_current_sprite(const std::wstring& name, const std::wstring& orientation)
		+void set_sprite_offset(const std::wstring& name, const std::wstring& orientation, const Math::Vector2& offset)
		+const std::wstring& get_current_sprite_name() const
		+Math::Vector2 get_offset() const

		+virtual void move_left() override
		+virtual void move_right() override
		+virtual void stop() override

		+float get_hp_percentage() const
		+float get_mp_percentage() const

        -float m_hp
		-float m_mp
		-float m_power
		-bool m_bGrounded

		-Math::Vector2 m_offset
		-Texture<GifWrapper> m_texture
		-std::weak_ptr<GifWrapper> m_current_sprite
    }

    class projectile {
		+void initialize() override
		+virtual void fire(const Math::Vector2& position, const Math::Vector2& velocity, const float charged)
		+virtual void update() override
		+virtual void on_collision(const CollisionCode& collison, const std::shared_ptr<rigidBody>& other) override
		+virtual void focus_this()
		+virtual void unfocus_this()
		+virtual void render() override
		+int get_radius() const
		+float get_damage() const
        
        -float m_damage
		-int m_radius
		-Texture<GifWrapper> m_texture
		-std::weak_ptr<GifWrapper> m_current_sprite
		-Math::Vector2 m_fired_position
    }

    class MissileCharacter {
        +void initialize() override
		+void shoot() override
		-std::shared_ptr<MissileProjectile> m_base_projectile
    }

    class CannonCharacter {
        +void initialize() override
		+void shoot() override
		-std::shared_ptr<MissileProjectile> m_base_projectile
    }

    class MissileProjectile {
        +void initialize() override;
        -Math::Vector2 m_fired_position;
    }

    class CannonProjectile {
        +void initialize() override;
        -Math::Vector2 m_fired_position;
    }

    class Ground {
        +`virtual void on_collision(const CollisionCode& collison, const std::shared_ptr<rigidBody>& other) override
		+void render() override
		+bool is_destroyed(const int x, const int y) const
		+void get_explosion_effect(const Math::Vector2& bottom, const float radius)
		+void set_destroyed(const int x, const int y)
		+void set_line_destroyed(const int mid_x, const int mid_y, const int n)
        -std::vector<std::vector<bool>> m_destroyed_table;
    }

    class ObjectManager {
        -static std::vector<std::shared_ptr<object>> m_objects
        +create_objectT(Args... args)
    }
    class ResourceManager{
        +static std::weak_ptr<T> loadT(const std::wstring name, const std::filesystem::path path)
        +static std::weak_ptr<T> findT(const std::wstring name)
        +static cleanup();
        -static std::shared_ptr<T> find_internally(const std::wstring name)
        -static std::map<std::wstring, std::shared_ptr<Resource>> m_resources
    }
    class CameraManager {
        +static std::weak_ptr create_camera(Args... args)
        -static std::vector<std::shared_ptr<Camera>> m_cameras
    }
    class Camera {
        +void update()
		+void initialize()
		+void set_object(const std::weak_ptr& obj)
		+void restore_object()
        +Vector2 get_relative_position(const std::weak_ptr& obj) const
		+Vector2 get_offset() const
		+std::weak_ptr get_locked_object() const
        -Math::Vector2 m_window_size
		-Math::Vector2 m_center_position
		-std::weak_ptr m_lock_target
		-std::weak_ptr m_backup_target
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
        +static void initialize();
		+static void update();
		+static bool getKeyDown(eKeyCode);
		+static bool getKeyUp(eKeyCode);
		+static bool getKey(eKeyCode);
        -static std::vector<Key> m_keys;
		-static void checkKeyState();
    }
    class WinAPIHandles {
        +static void initialize(HWND hwnd, HDC hdc)
		+static int get_window_width()
		+static int get_window_height()
		+static HDC get_buffer_dc()
		+static HDC get_main_dc()
		+static HWND get_hwnd()
		+static int get_actual_max_y()
		+static RECT& get_window_size()
        -GdiplusStartupInput input
		-ULONG_PTR token
		-static RECT m_window_size
		-static RECT m_native_size
		-static HWND m_hwnd
		-static HDC m_hdc
		-static HDC m_buffer_hdc
		-static HBITMAP m_buffer_bitmap
    }
    class Application {
        +void initialize(HWND, HDC)
		+void update()
		+void render()
        -ResourceManager m_resource_manager;
		-SceneManager m_scene_manager;
		-WinAPIHandles m_win_handles;
		-HWND m_hwnd;
		-HDC m_hdc;
		-HDC m_buffer_hdc;
    }

    class GifWrapper {
        +virtual bool load() override
		+virtual void initialize()
		+static void cleanup()

		+void play(const std::function<void()>& on_end)
		+void stop() const
		+void OnTimer()
		+virtual void flip() override
		+virtual void rotate(const float angle)
		+void reset_transfrom()

		+inline static std::map<UINT_PTR, GifWrapper*> registered_gifs
        
        -UINT_PTR m_timer_id
		-UINT m_dimension_count
		-UINT m_frame_count
		-UINT m_total_buffer
		-UINT m_current_frame
		-WCHAR m_str_guid[39]
		-std::vector<unsigned int> m_frame_delays
		-std::function<void()> m_reserved_function
		-inline static UINT used_timer_id = gif_timer_id
    }

    class ImageWrapper {
        +void cleanup()
		+virtual void render(const Math::Vector2& position, const Math::Vector2& hitbox, const Math::Vector2& scaling, const float rotate_degree)
		+const Math::Vector2& get_hitbox() const
		+virtual void flip()
		+void set_offset(const Math::Vector2& offset)

        -virtual bool load() override;
		-std::shared_ptr<Image> m_image;
		-std::shared_ptr<Graphics> m_gdi_handle;
		-Math::Vector2 m_size;
		-Math::Vector2 m_offset;
    }
```