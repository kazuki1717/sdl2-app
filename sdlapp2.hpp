#define SDLAPP_VERSION "c++ 2.0.0 2026-03-18"

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

#pragma comment(lib, "sdl2.lib")
#pragma comment(lib, "sdl2_ttf.lib")
#pragma comment(lib, "sdl2_mixer.lib")
#pragma comment(lib, "sdl2_image.lib")

#undef main

#include <string>
#include <iostream>
#include <exception>
#include <vector>



#define MIN(A, B) ((A) < (B) ? (A) : (B))




// == pre-processer ==

#if defined(_MSC_VER) && !defined(_CODE_IN_MSVC)

#define _CODE_IN_MSVC(code) code
#define _CODE_NOT_MSVC(code)

#else

#define _CODE_IN_MSVC(code)
#define _CODE_NOT_MSVC(code) code

#endif



// == sdl_exeception_t ==

class sdl_exception_t : public std::exception {
    char message[512];

public:
    sdl_exception_t(const char* fmt, ...) {
        char* p = 
            _CODE_IN_MSVC(message + 17; strcpy_s(message, 512, "sdl_exception_t: "))
            _CODE_NOT_MSVC(strcpy(message, "sdl_exception_t: "))
        ;

        va_list args;
        va_start(args, fmt);
        vsnprintf(p, 512, fmt, args);
        va_end(args);
    }

    const char* what() const noexcept override {
        return message;
    }
};




// == type ==

using sdl_tick_t = uint32_t;


// == colors ==

constexpr SDL_Color SDLAPP_COLOR_WHITE{255, 255, 255, 255};
constexpr SDL_Color SDLAPP_COLOR_GRAY{128, 128, 128, 255};
constexpr SDL_Color SDLAPP_COLOR_BLACK{0, 0, 0, 255};

constexpr SDL_Color SDLAPP_COLOR_RED{255, 0, 0, 255};
constexpr SDL_Color SDLAPP_COLOR_GREEN{0, 255, 0, 255};
constexpr SDL_Color SDLAPP_COLOR_BLUE{0, 0, 255, 255};

constexpr SDL_Color SDLAPP_COLOR_YELLOW{255, 255, 0, 255};
constexpr SDL_Color SDLAPP_COLOR_CYAN{0, 255, 255, 255};
constexpr SDL_Color SDLAPP_COLOR_MAGENTA{255, 0, 255, 255};

constexpr SDL_Color SDLAPP_COLOR_PURPLE{128, 0, 128, 255};






// == ostream ==

std::ostream& operator<<(std::ostream& os, const SDL_Rect rect) {
    char buf[129];
    snprintf(buf, 128, "SDL_Rect{%d, %d, %d, %d}", rect.x, rect.y, rect.w, rect.h);
    return os << buf;
}

std::ostream& operator<<(std::ostream& os, const SDL_Point point) {
    char buf[65];
    snprintf(buf, 64, "SDL_Point{%d, %d}", point.x, point.y);
    return os << buf;
}



std::ostream& operator<<(std::ostream& os, const SDL_FRect rect) {
    char buf[129];
    snprintf(buf, 128, "SDL_FRect{%g, %g, %g, %g}", rect.x, rect.y, rect.w, rect.h);
    return os << buf;
}

std::ostream& operator<<(std::ostream& os, const SDL_FPoint point) {
    char buf[65];
    snprintf(buf, 64, "SDL_FPoint{%g, %g}", point.x, point.y);
    return os << buf;
}



std::ostream& operator<<(std::ostream& os, const SDL_Color color) {
    char buf[65];
    snprintf(buf, 64, "SDL_Color{%d, %d, %d, %d}", color.r, color.g, color.b, color.a);
    return os << buf;
}




// == basic ==

class sdl_basic_t {
protected:
    class render_info_t {
    public:
        SDL_Renderer* renderer;
        int wnd_width, wnd_height;
        sdl_tick_t tick;
    };


    // == render ==

    inline static int render_clear(SDL_Renderer* renderer) {
        return SDL_RenderClear(renderer);
    }

    inline static int render_copy(SDL_Renderer* renderer, SDL_Texture* texture,
                const SDL_Rect* src_rect = nullptr, const SDL_FRect* dest_rect = nullptr,
                double angle = 0.0, const SDL_FPoint* center = nullptr,
                SDL_RendererFlip flip = SDL_FLIP_NONE
    ) {
        return SDL_RenderCopyExF(renderer, texture, src_rect, dest_rect, angle, center, flip);
    }


    inline static int render_fill_rect(SDL_Renderer* renderer, SDL_FRect* rect = nullptr) {
        return SDL_RenderFillRectF(renderer, rect);
    }

    inline static int render_fill_rect(SDL_Renderer* renderer, SDL_FRect& rect) {
        return SDL_RenderFillRectF(renderer, &rect);
    }

    inline static int render_fill_rect(SDL_Renderer* renderer, float x, float y, float w, float h) {
        SDL_FRect rect{x, y, w, h};
        return SDL_RenderFillRectF(renderer, &rect);
    }



    // == set ==

    inline static int set_render_draw_color(SDL_Renderer* renderer, SDL_Color color) {
        return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    }

    inline static int set_render_draw_color(SDL_Renderer* renderer, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) {
        return SDL_SetRenderDrawColor(renderer, r, g, b, a);
    }

    inline static int set_render_target(SDL_Renderer* renderer, SDL_Texture* target) {
        return SDL_SetRenderTarget(renderer, target);
    }



    inline static void set_window_title(SDL_Window* window, const std::string& title) {
        SDL_SetWindowTitle(window, title.c_str());
    }

    inline static void set_window_size(SDL_Window* window, int width, int height) {
        SDL_SetWindowSize(window, width, height);
    }

    

    // == get ==

    inline static void get_current_display_mode(int display_index, SDL_DisplayMode* dm) {
        if (SDL_GetCurrentDisplayMode(display_index, dm) != 0) {
			throw sdl_exception_t("failed to get display mode since %s!", SDL_GetError());
		}
    }

    inline static sdl_tick_t get_ticks() {
        return SDL_GetTicks();
    }



    // == check ==

    inline static bool point_in_rect(const SDL_FPoint& point, const SDL_FRect& rect) {
        return rect.x <= point.x && rect.y <= point.y &&
                point.x <= rect.x + rect.w && point.y <= rect.y + rect.h
        ;
    }

    inline static bool point_in_rect(const SDL_Point& point, const SDL_Rect& rect) {
        return rect.x <= point.x && rect.y <= point.y &&
                point.x <= rect.x + rect.w && point.y <= rect.y + rect.h
        ;
    }
};


// == sdl_window_t ==


class sdl_window_t : public sdl_basic_t {
    friend class sdl_texture_t;
    friend class sdl_font_t;


protected:
    // == running variables ==

    bool running = true;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    int window_width = 0;
    int window_height = 0;


    SDL_Event event;

    uint32_t update_delay = 50;
    uint32_t next_update_time = 0;

    uint32_t render_delay = 15;
    uint32_t next_render_time = 0;
    bool render_lazy_draw = false;

    std::vector<SDL_Thread*> threads{16};
    int thread_count = 0;




    virtual void on_setup() {
        init_info_t info;
        init_sdl(info);
        init_window(info);
    }

    virtual void on_event(SDL_Event& e) {
        switch (e.type) {
            case SDL_QUIT:
                post_stop();
                return;
            case SDL_WINDOWEVENT:
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        window_width = e.window.data1;
                        window_height = e.window.data2;
                        post_redraw();
                        return;
                }
            case SDL_MOUSEWHEEL:
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEMOTION:
                post_redraw();
                return;
        }
    }

    virtual void on_update(sdl_tick_t tick) {
        
    }

    virtual void on_render(sdl_tick_t tick) {
        
    }





    // == init ==

    class init_info_t {
    public:
        uint32_t sdl_flags = SDL_INIT_EVERYTHING;
        SDL_LogPriority log_priority = SDL_LOG_PRIORITY_INFO;

        bool ttf_flags = true;

        int image_flags = 0x3f;
        int mixer_flags = 0xff;

        int      audio_frequency = MIX_DEFAULT_FREQUENCY;
        uint16_t audio_format = MIX_DEFAULT_FORMAT;
        int      audio_channels = MIX_DEFAULT_CHANNELS;
        int      audio_chunk_size = 1024;



        const char*     wnd_title = "basic window";
        SDL_Point       wnd_position = {SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED};
        SDL_Point       wnd_size = {SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED};
        SDL_WindowFlags wnd_flags = SDL_WINDOW_RESIZABLE;
        const char*     wnd_icon = nullptr;

        int rnd_index = -1;
        int rnd_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    };

    void init_sdl(init_info_t& info) {
        if (SDL_Init(info.sdl_flags) != 0) {
            throw sdl_exception_t("failed to init sdl");
        }
        if (info.log_priority) {
            SDL_LogSetAllPriority(info.log_priority);
        }
        if (info.ttf_flags) {
            if (TTF_Init() != 0) {
                throw sdl_exception_t("failed to init ttf");
            }
        }
        if (info.image_flags) {
            IMG_Init(info.image_flags);
        }
        if (info.mixer_flags) {
            Mix_Init(info.mixer_flags);

            int ret = Mix_OpenAudio(info.audio_frequency, info.audio_format, info.audio_channels, info.audio_chunk_size);
            if (ret != 0) {
                throw sdl_exception_t("failed to open audio");
            }
        }
    }


    void init_window(init_info_t& info) {
        if (window) {
            return;
        }

        window = SDL_CreateWindow(info.wnd_title,
            info.wnd_position.x, info.wnd_position.y,
            info.wnd_size.x, info.wnd_size.y,
            info.wnd_flags
        );
        if (window == nullptr) {
            throw sdl_exception_t("failed to create window since %s!", SDL_GetError());
        }

        SDL_GetWindowSize(window, &window_width, &window_height);
        
        if (info.wnd_icon) {
            SDL_Surface* surface = IMG_Load(info.wnd_icon);

            if (surface == nullptr) {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "failed to set window icon, %s", IMG_GetError());
            }
            else {
                SDL_SetWindowIcon(window, surface);
                SDL_FreeSurface(surface);
            }
        }

        if (info.rnd_flags) {
            renderer = SDL_CreateRenderer(window, info.rnd_index, info.rnd_flags);

            if (renderer == nullptr) {
                throw sdl_exception_t("failed to create renderer since %s!", SDL_GetError());
            }
        }
    }




    // == post ==

    inline void post_redraw() {
        if (render_lazy_draw == true) {
            next_render_time = 1;
        }
    }

    inline void post_stop() {
        running = false;
    }



    // == create ==

    SDL_Thread* create_thread(const char* name, SDL_ThreadFunction func, void* data) {
        if (thread_count >= threads.max_size()) {
            threads.resize(thread_count * 2);
        }

        SDL_Thread* thread = SDL_CreateThread(func, name, data);
        if (thread == nullptr) {
            throw sdl_exception_t("failed to create thread since %s", SDL_GetError());
        }

        return threads[thread_count++] = thread;
    }


    // == set ==

    inline void set_update_delay(uint32_t delay) {
        update_delay = delay;
    }

    inline void set_render_delay(uint32_t delay) {
        render_delay = delay;
    }



    inline void disable_update() {
        update_delay = -1;
        next_update_time = -1;
    }

    inline void enable_lazy_draw() {
        render_lazy_draw = true;
    }

    

public:

    ~sdl_window_t() {
        if (renderer != nullptr) {
            SDL_DestroyRenderer(renderer);
        }
        if (window != nullptr) {
            SDL_DestroyWindow(window);
        }

        Mix_Quit();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
    }


    void run() {
        try {
            on_setup();

            uint32_t now, next;

            while (running) {
                next = MIN(next_update_time, next_render_time);

                while (SDL_WaitEventTimeout(&event, next > (now = get_ticks()) ? next - now : 0)) {
                    on_event(event);
                    
                    if (next_render_time != -1 || running == false) {
                        break;
                    }
                }

                now = get_ticks();

                // -- handle tick --
                if (now >= next_update_time) {
                    on_update(now);
                    next_update_time = now + update_delay;
                }

                // -- render --
                if (render_lazy_draw == true) {
                    if (next_render_time != -1) {
                        on_render(now);
                        SDL_RenderPresent(renderer);
                        next_render_time = -1;
                    }
                    continue;
                }

                if (now >= next_render_time) {
                    on_render(now);
                    SDL_RenderPresent(renderer);
                    next_render_time = now + render_delay;
                }
            }

            SDL_Log("app exit normally");
        }
        catch (sdl_exception_t& e) {
            std::cerr << e.what() << "\n";
        }
        catch (std::exception& e) {
            SDL_Log("std::exception: %s", e.what());
        }

        // == wait threads ==

        for (int i = 0; i < thread_count; i++) {
            SDL_WaitThread(threads[i], nullptr);
        }
    }
};





// == resources ==

// base class for resources, such as texture, font, music, etc.
// this class designed for private use only
class sdl_resource_t {
protected:
    class basic_info_t {
    public:
        int load_method = 0;
        std::string file;

        void* resource = nullptr;
        int use_count = 1;

        virtual ~basic_info_t() {}

        basic_info_t(void* resource = nullptr, int load_method = 0) : resource(resource), load_method(load_method) {}
        basic_info_t(const std::string& file, int load_method = 0) : file(file), load_method(load_method) {}
    };

    using release_t = void (*)(void*);

    basic_info_t* ptr;

    

    // == delete / move ==

    void _delete(release_t release) noexcept {
        ptr->use_count--;
        if (ptr->use_count == 0) {
            if (has_loaded()) {
                release(ptr->resource);
            }
            delete ptr;
        }
    }

    void _move2(release_t release, const sdl_resource_t& other) noexcept {
        _delete(release);
        ptr = other.ptr;
        ptr->use_count++;
    }

    void _release(release_t release) const noexcept {
        if (has_loaded()) {
            release(ptr->resource);
            ptr->resource = nullptr;
        }
    }


    // == init ==

    sdl_resource_t(basic_info_t* ptr)
    : ptr(ptr) {}

    // constexpr sdl_resource_t() {}



public:
    // == copy1 / move1 ==

    sdl_resource_t(const sdl_resource_t& other) noexcept : ptr(other.ptr) {
        ptr->use_count++;
    }

    sdl_resource_t(sdl_resource_t&& other) noexcept : ptr(other.ptr) {
        ptr->use_count++;
    }


    // == get ==

    const std::string& get_file_name() const {
        return ptr->file;
    }

    // == has ==

    bool has_loaded() const {
        return ptr->resource != nullptr;
    }

    // == load ==

    virtual void load() const = 0;
};






class sdl_font_t : public sdl_resource_t {
    class info_t : public basic_info_t {
    public:
        int ptsize = 0;

        info_t(const std::string& file, int ptsize) : basic_info_t(file), ptsize(ptsize) {}
        info_t(TTF_Font* font) : basic_info_t((void*) font) {}
    };

public:
    // == delete ==

    ~sdl_font_t() {
        _delete((release_t) TTF_CloseFont);
    }

    // == init ==

    sdl_font_t(const std::string& file, int ptsize) : sdl_resource_t(new info_t(file, ptsize)) {}

    sdl_font_t(TTF_Font* font = nullptr) : sdl_resource_t(new info_t(font)) {}


    // == copy / move ==

    sdl_font_t(const sdl_font_t&) noexcept = default;

    sdl_font_t& operator=(const sdl_font_t& other) noexcept {
        _move2((release_t) TTF_CloseFont, other);
        return *this;
    }


    sdl_font_t(sdl_font_t&&) noexcept = default;

    sdl_font_t& operator=(sdl_font_t&& other) noexcept {
        _move2((release_t) TTF_CloseFont, other);
        return *this;
    }
    


    // == get ==

    operator TTF_Font*() const {
        load();
        return (TTF_Font*) ptr->resource;
    }

    int get_size() const {
        load();
        return *(int*)(ptr + 1);
    }

    // == load / release ==

    void load() const {
        int* ptsize = (int*)(ptr + 1);

        if (has_loaded()) {
            if (ptsize[0] == 0) {
                ptsize[0] = TTF_FontHeight((TTF_Font*)ptr->resource);       // some sdl2 version couldn't get font height by TTF_Font
            }
            return;
        }

        ptr->resource = TTF_OpenFont(ptr->file.c_str(), ptsize[0]);
        if (ptr->resource == nullptr) {
            throw sdl_exception_t("failed to open font '%s' since %s!", ptr->file.c_str(), TTF_GetError());
        }
    }

    void release() const {
        _release((release_t) TTF_CloseFont);
    }
};



enum sdl_render_text_mode_t {
    SDLAPP_TEXT_SOLID = 3,
    SDLAPP_TEXT_BLENDED,
    SDLAPP_TEXT_SHADED,
};


class sdl_surface_t : public sdl_resource_t {
    class info_t : public basic_info_t {
    public:
        sdl_font_t font;
        SDL_Color fg;
        SDL_Color bg;
        uint32_t warp_length;

        info_t(const sdl_font_t& font, const std::string& text, sdl_render_text_mode_t mode, SDL_Color fg, SDL_Color bg, uint32_t warp_length
        )
        : basic_info_t(text, mode), font(font), fg(fg), bg(bg), warp_length(warp_length) {}
    };

public:
    // == delete ==

    ~sdl_surface_t() {
        _delete((release_t) SDL_FreeSurface);
    }


    // == init ==

    sdl_surface_t(const std::string& file) : sdl_resource_t(new basic_info_t(file)) {}
    sdl_surface_t(SDL_Surface* surface = nullptr) : sdl_resource_t(new basic_info_t(surface)) {}

    sdl_surface_t(
                const sdl_font_t& font, const std::string& text, sdl_render_text_mode_t mode = SDLAPP_TEXT_SOLID,
                SDL_Color fg = SDLAPP_COLOR_WHITE, SDL_Color bg = SDLAPP_COLOR_BLACK,
                uint32_t warp_length = 0
    ):
    sdl_resource_t(new info_t(font, text, mode, fg, bg, warp_length)) {}


    // == copy / move ==

    sdl_surface_t(const sdl_surface_t& other) noexcept = default;

    sdl_surface_t& operator=(const sdl_surface_t& other) noexcept {
        _move2((release_t) SDL_DestroyTexture, other);
        return *this;
    }
    

    sdl_surface_t(sdl_surface_t&& other) noexcept = default;

    sdl_surface_t& operator=(sdl_surface_t&& other) noexcept {
        _move2((release_t) SDL_DestroyTexture, other);
        return *this;
    }
    


    // == get ==

    operator SDL_Surface*() const {
        load();
        return (SDL_Surface*) ptr->resource;
    }

    int get_width() const {
        load();
        return ((SDL_Surface*) ptr->resource)->w;
    }

    int get_height() const {
        load();
        return ((SDL_Surface*) ptr->resource)->h;
    }


    // == load / release ==

    void load() const {
        if (has_loaded()) {
            return;
        }

        if (ptr->load_method == 0) {
            ptr->resource = IMG_Load(ptr->file.c_str());

            if (ptr->resource == nullptr) {
                throw sdl_exception_t("failed to load texture '%s', maybe the file not exist!", ptr->file.c_str());
            }

            return;
        }

        info_t* info = (info_t*) ptr;

        switch (info->load_method) {
            case SDLAPP_TEXT_SOLID:
                info->resource = TTF_RenderUTF8_Solid_Wrapped(info->font, info->file.c_str(), info->fg, info->warp_length);
                break;
            case SDLAPP_TEXT_BLENDED:
                info->resource = TTF_RenderUTF8_Blended_Wrapped(info->font, info->file.c_str(), info->fg, info->warp_length);
                break;
            case SDLAPP_TEXT_SHADED:
                info->resource = TTF_RenderUTF8_Shaded_Wrapped(info->font, info->file.c_str(), info->fg, info->bg, info->warp_length);
                break;

            default:
                throw sdl_exception_t("invalid text render mode %d!", info->load_method);
        }
        if (info->resource == nullptr) {
            throw sdl_exception_t("failed to render text '%s'!", info->file.c_str());
        }
    }

    void release() const {
        _release((release_t) SDL_FreeSurface);
    }
};





class sdl_texture_t : public sdl_resource_t {
    class texture_info_t : public basic_info_t {
    public:
        sdl_window_t* owner;
        int width = 0;
        int height = 0;

        texture_info_t(void* texture, int load_method, sdl_window_t* owner, int width, int height
        ):
        basic_info_t(texture, load_method), owner(owner), width(width), height(height) {}


        texture_info_t(const std::string& file, int load_method, sdl_window_t* owner
        ):
        basic_info_t(file, load_method), owner(owner) {}
    };


    class surface_info_t : public texture_info_t {
    public:
        sdl_surface_t surface;

        surface_info_t(
                    sdl_window_t* owner, const sdl_surface_t& surface
        ):
        texture_info_t(nullptr, 1, owner, surface.get_width(), surface.get_height()), surface(surface) {}
    };


    class empty_info_t : public texture_info_t {
    public:
        SDL_PixelFormatEnum format;
        SDL_TextureAccess access;
        
        empty_info_t(
                sdl_window_t* owner, SDL_PixelFormatEnum format, SDL_TextureAccess access,
                int width, int height
        ):
        texture_info_t(nullptr, 2, owner, width, height), format(format), access(access) {}
    };

    
    class render_info_t : public texture_info_t {
    public:
        sdl_font_t font;
        SDL_Color fg;
        SDL_Color bg;
        uint32_t warp_length;

        render_info_t(sdl_window_t* owner, sdl_font_t font, const std::string& text, sdl_render_text_mode_t mode,
                    SDL_Color fg, SDL_Color bg, uint32_t warp_length
        ):
        texture_info_t(text, mode, owner), font(font), fg(fg), bg(bg), warp_length(warp_length) {}
    };

public:
    // == delete ==

    ~sdl_texture_t() {
        _delete((release_t) SDL_DestroyTexture);
    }


    // == init ==

    sdl_texture_t(sdl_window_t* owner, const std::string& file) : sdl_resource_t(new texture_info_t(file, 0, owner)) {}
    sdl_texture_t(sdl_window_t* owner, SDL_Texture* texture = nullptr) : sdl_resource_t(new texture_info_t(texture, 0, owner, 0, 0)) {}


    sdl_texture_t(sdl_window_t* owner, const sdl_surface_t& surface) : sdl_resource_t(new surface_info_t(owner, surface)) {}


    sdl_texture_t(
                sdl_window_t* owner, SDL_PixelFormatEnum format, SDL_TextureAccess access,
                int width, int height
    ):
    sdl_resource_t(new empty_info_t(owner, format, access, width, height)) {}


    sdl_texture_t(
                sdl_window_t* owner,
                sdl_font_t font, const std::string& text, sdl_render_text_mode_t mode,
                SDL_Color fg = SDLAPP_COLOR_WHITE, SDL_Color bg = SDLAPP_COLOR_BLACK, uint32_t warp_length = 0
    ):
    sdl_resource_t(new render_info_t(owner, font, text, mode, fg, bg, warp_length)) {}


    // == copy ==

    sdl_texture_t(const sdl_texture_t& other) noexcept = default;

    sdl_texture_t& operator=(const sdl_texture_t& other) {
        if (((texture_info_t*) ptr)->owner != ((texture_info_t*) other.ptr)->owner) {
            throw std::invalid_argument("couldn't convert texture owned other window!");
        }

        _move2((release_t) SDL_DestroyTexture, other);
        return *this;
    }
    

    sdl_texture_t(sdl_texture_t&& other) noexcept = default;

    sdl_texture_t& operator=(sdl_texture_t&& other) {
        if (((texture_info_t*) ptr)->owner != ((texture_info_t*) other.ptr)->owner) {
            throw std::invalid_argument("couldn't convert texture owned other window!");
        }

        _move2((release_t) SDL_DestroyTexture, other);
        return *this;
    }
    


    // == get ==

    operator SDL_Texture*() const {
        load();
        return (SDL_Texture*) ptr->resource;
    }

    int get_width() const {
        load();
        return ((texture_info_t*) ptr)->width;
    }

    int get_height() const {
        load();
        return ((texture_info_t*) ptr)->height;
    }


    // == load / release ==

    void load() const {
        if (has_loaded()) {
            return;
        }

        texture_info_t* basic = (texture_info_t*) ptr;

        if (basic->owner->renderer == nullptr) {
            throw sdl_exception_t("failed to load texture '%s' since renderer still not created!", ptr->file.c_str());
        }


        if (basic->load_method == 0) {
            basic->resource = IMG_LoadTexture(basic->owner->renderer, basic->file.c_str());

            if (basic->resource == nullptr) {
                throw sdl_exception_t("failed to load texture '%s', maybe the file not exist!", basic->file.c_str());
            }

            SDL_QueryTexture((SDL_Texture*)basic->resource, nullptr, nullptr, &basic->width, &basic->height);
            return;
        }
        if (ptr->load_method == 1) {
            surface_info_t* info = (surface_info_t*) ptr;
            info->resource = SDL_CreateTextureFromSurface(basic->owner->renderer, info->surface);

            if (info->resource == nullptr) {
                throw sdl_exception_t("failed to create texture from surface, %s", SDL_GetError());
            }
            info->width = info->surface.get_width();
            info->height = info->surface.get_height();
            return;
        }
        if (ptr->load_method == 2) {
            empty_info_t* info = (empty_info_t*) ptr;
            info->resource = SDL_CreateTexture(basic->owner->renderer, info->format, info->access, info->width, info->height);

            if (info->resource == nullptr) {
                throw sdl_exception_t("failed to create texture, %s", SDL_GetError());
            }
            return;
        }

        render_info_t* info = (render_info_t*) ptr;
        sdl_surface_t surface(info->font, info->file, (sdl_render_text_mode_t)info->load_method, info->fg, info->bg, info->warp_length);
        
        ptr->resource = SDL_CreateTextureFromSurface(basic->owner->renderer, surface);
        info->width = surface.get_width();
        info->height = surface.get_height();
    }

    void release() const {
        _release((release_t) SDL_DestroyTexture);
    }
};




class sdl_music_t : public sdl_resource_t {

public:
    // == delete ==

    ~sdl_music_t() {
        _delete((release_t)Mix_FreeMusic);
    }


    // == init ==

    sdl_music_t(const std::string& file) : sdl_resource_t(new basic_info_t(file)) {}
    sdl_music_t(Mix_Music* music = nullptr) : sdl_resource_t(new basic_info_t((void*) music)) {}


    // == copy / move ==

    sdl_music_t(const sdl_music_t& other) noexcept = default;

    sdl_music_t& operator=(const sdl_music_t& other) noexcept {
        _move2((release_t) Mix_FreeMusic, other);
        return *this;
    }
    

    sdl_music_t(sdl_music_t&& other) noexcept = default;

    sdl_music_t& operator=(sdl_music_t&& other) noexcept {
        _move2((release_t) Mix_FreeMusic, other);
        return *this;
    }
    


    // == get ==

    operator Mix_Music*() const {
        load();
        return (Mix_Music*) ptr->resource;
    }
    

    // == play ==

    void play(int loops = 0) const {
        load();
        if (Mix_PlayMusic((Mix_Music*) ptr->resource, loops) != 0) {
            throw sdl_exception_t("failed to play music '%s'!", ptr->file.c_str());
        }
    }


    // == load / release ==

    void load() const {
        if (has_loaded()) {
            return;
        }

        ptr->resource = Mix_LoadMUS(ptr->file.c_str());
        if (ptr->resource == nullptr) {
            throw sdl_exception_t("failed to load music '%s', maybe the file not exist!", ptr->file.c_str());
        }
    }

    void release() const {
        _release((release_t) Mix_FreeMusic);
    }
};







// == entity ==

class sdl_entity_t {
protected:
    class render_info_t {
    public:
        SDL_Renderer* renderer;
        int w_width, w_height;
        sdl_tick_t tick;
    };

public:
    float x, y;
    bool show;

    constexpr sdl_entity_t(float x = 0, float y = 0, bool show = true) : x(x), y(y), show(show) {}


    virtual void render(render_info_t& info) const = 0;
    
    virtual bool in_point(SDL_FPoint& point) const {
        return false;
    }

    virtual bool on_rect(SDL_FRect& rect) const {
        return false;
    }
};



class sdl_text_entity_t : public sdl_entity_t {
    sdl_surface_t surface;
    sdl_texture_t texture;

public:
    sdl_font_t font;
    SDL_FRect rect;
    sdl_render_text_mode_t mode;

    SDL_Color fg, bg;
    uint32_t warp_length;



    sdl_text_entity_t(
                sdl_window_t* owner,
                sdl_font_t font, const std::string& text,
                SDL_FRect rect, sdl_render_text_mode_t mode = SDLAPP_TEXT_BLENDED,
                SDL_Color fg = SDLAPP_COLOR_WHITE, SDL_Color bg = SDLAPP_COLOR_WHITE,
                uint32_t warp_length = 0
    ):
    surface(font, text, mode, fg, bg, warp_length), texture(owner),
    font(font), rect(rect), mode(mode) {}


    void render(render_info_t& info) const {
        
    }


    void set_text(const std::string& text) {
        this->surface = sdl_surface_t(font, text, mode, fg, bg, warp_length);
    }
};


