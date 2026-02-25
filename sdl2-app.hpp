#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

#undef main

#include <string>
#include <iostream>
#include <exception>



#define MIN(A, B) ((A) < (B) ? (A) : (B))





// == sdl_exeception_t ==

class sdl_exception_t : public std::exception {
    char message[513];
    const char* file;
    int line;

public:
    sdl_exception_t(const char* file, int line, const char* fmt, ...) {
        this->file = file;
        this->line = line;

        va_list args;
        va_start(args, fmt);
        vsnprintf(message, 512, fmt, args);
        va_end(args);
    }

    const char* what() const noexcept override {
        return message;
    }

    void print() const {
        fprintf(stderr, "sdl_exception from %s-%d: %s\n", file, line, message);
    }
};

#define sdl_make_exception(fmt, ...) sdl_exception_t(__FILE__, __LINE__, fmt, __VA_ARGS__)





// == colors ==

constexpr SDL_Color SDLAPP_COLOR_WHITE{255, 255, 255, 255};
constexpr SDL_Color SDLAPP_COLOR_BLACK{0, 0, 0, 255};
constexpr SDL_Color SDLAPP_COLOR_RED{255, 0, 0, 255};
constexpr SDL_Color SDLAPP_COLOR_GREEN{0, 255, 0, 255};
constexpr SDL_Color SDLAPP_COLOR_BLUE{0, 0, 255, 255};

constexpr SDL_Color SDLAPP_COLOR_YELLOW{255, 255, 0, 255};
constexpr SDL_Color SDLAPP_COLOR_CYAN{0, 255, 255, 255};
constexpr SDL_Color SDLAPP_COLOR_MAGENTA{255, 0, 255, 255};




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




// == sdl_app_t ==


class sdl_app_t {
    friend class sdl_texture_t;
    friend class sdl_font_t;


protected:
    static bool running;
    static SDL_Window* window;
    int window_width = 0;
    int window_height = 0;

    static SDL_Renderer* renderer;

    SDL_Event event;

    uint32_t update_delay = 50;
    uint32_t next_update_time = 0;

    uint32_t render_delay = 15;
    uint32_t next_render_time = 0;
    bool render_lazy_draw = false;

    
    // == init variables ==

    uint32_t init_sdl_flags = SDL_INIT_EVERYTHING;
    SDL_LogPriority init_log_priority = SDL_LOG_PRIORITY_INFO;

    int init_image_flags = 0x3f;
    int init_mixer_flags = 0xff;

    int      init_audio_frequency = MIX_DEFAULT_FREQUENCY;
    uint16_t init_audio_format = MIX_DEFAULT_FORMAT;
    int      init_audio_channels = MIX_DEFAULT_CHANNELS;
    int      init_audio_chunk_size = 1024;
    
    std::string     init_window_title = "sdl window";
    SDL_Point       init_window_position = {SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED};
    SDL_Point       init_window_size = {800, 600};
    SDL_WindowFlags init_window_flags = SDL_WINDOW_RESIZABLE;
    const char*     init_window_icon = nullptr;

    int init_renderer_index = -1;
    int init_renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;







    virtual void on_setup() {
        init_all();
    }

    virtual void on_event(SDL_Event& e) {
        switch (e.type) {
            case SDL_QUIT:
                send_stop_signal();
                return;
            case SDL_WINDOWEVENT:
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        window_width = e.window.data1;
                        window_height = e.window.data2;
                        send_redraw_signal();
                        return;
                }
            case SDL_MOUSEWHEEL:
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEMOTION:
                send_redraw_signal();
                return;
        }
    }

    virtual void on_update(uint32_t tick) {
        
    }

    virtual void on_render(uint32_t tick) {
        
    }






    void init_sdl() {
        if (SDL_Init(init_sdl_flags) != 0) {
            throw sdl_make_exception("failed to init sdl");
        }
        SDL_LogSetAllPriority(init_log_priority);
    }

    void init_ttf() {
        if (TTF_Init() != 0) {
            throw sdl_make_exception("failed to init ttf");
        }
    }

    void init_image() {
        IMG_Init(init_image_flags);
    }

    void init_mixer() {
        Mix_Init(init_mixer_flags);

        if (Mix_OpenAudio(init_audio_frequency, init_audio_format, init_audio_channels, init_audio_chunk_size) != 0) {
            throw sdl_make_exception("failed to open audio");
        }
    }

    void init_window() {
        if (window != nullptr) {
            return;
        }

        window = SDL_CreateWindow(init_window_title.c_str(),
            init_window_position.x, init_window_position.y, init_window_size.x, init_window_size.y,
            init_window_flags
        );

        if (window == nullptr) {
            throw sdl_make_exception("failed to create window since %s!", SDL_GetError());
        }

        SDL_GetWindowSize(window, &window_width, &window_height);
        
        if (init_window_icon != nullptr) {
            SDL_Surface* surface = IMG_Load(init_window_icon);

            if (surface) {
                SDL_SetWindowIcon(window, surface);
                SDL_FreeSurface(surface);
            }
        }
    }

    void init_renderer() {
        if (window == nullptr || renderer != nullptr) {
            return;
        }

        renderer = SDL_CreateRenderer(window, init_renderer_index, init_renderer_flags);
        if (renderer == nullptr) {
            throw sdl_make_exception("failed to create renderer since %s!", SDL_GetError());
        }
    }



    void init_window_renderer() {
        init_window();
        init_renderer();
    }

    void init_all() {
        init_sdl();
        init_ttf();
        init_image();
        init_mixer();
        init_window();
        init_renderer();
    }






    void send_redraw_signal() {
        if (render_lazy_draw == true) {
            next_render_time = 1;
        }
    }

    void send_stop_signal() {
        running = false;
    }



    int render_set_draw_color(SDL_Color color) {
        return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    }

    int render_set_draw_color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) {
        return SDL_SetRenderDrawColor(renderer, r, g, b, a);
    }

    int render_clear() {
        return SDL_RenderClear(renderer);
    }

    int render_copy(SDL_Texture* texture,
                    const SDL_Rect* src_rect = nullptr, const SDL_FRect* dest_rect = nullptr,
                    double angle = 0.0, const SDL_FPoint* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE) {
        return SDL_RenderCopyExF(renderer, texture, src_rect, dest_rect, angle, center, flip);
    }




    void set_window_title(const std::string& title) {
        SDL_SetWindowTitle(window, title.c_str());
    }

    void set_window_size(int width, int height) {
        SDL_SetWindowSize(window, width, height);
    }

    void disable_update() {
        update_delay = -1;
        next_update_time = -1;
    }

    void enable_lazy_draw() {
        render_lazy_draw = true;
    }



    void get_current_display_mode(int display_index, SDL_DisplayMode* dm) {
        if (SDL_GetCurrentDisplayMode(display_index, dm) != 0) {
			throw sdl_make_exception("failed to get display mode since %s!", SDL_GetError());
		}
    }

    

public:

    ~sdl_app_t() {
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

                while (SDL_WaitEventTimeout(&event, next > (now = SDL_GetTicks()) ? next - now : 0)) {
                    on_event(event);
                    
                    if (next_render_time != -1 || running == false) {
                        break;
                    }
                }

                now = SDL_GetTicks();

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
            SDL_Log("app exited normally");
        }
        catch (sdl_exception_t& e) {
            e.print();
        }
        catch (std::exception& e) {
            std::cerr << "std::exception: " << e.what() << "\n";
        }
    }
};

bool sdl_app_t::running = true;
SDL_Window* sdl_app_t::window = nullptr;
SDL_Renderer* sdl_app_t::renderer = nullptr;





// == resources ==

// base class for resources, such as texture, font, music, etc.
// this class designed for private use only
class sdl_resource_t {
protected:
    class context_t {
    public:
        int load_method = 0;
        std::string file;

        void* resource = nullptr;
        int use_count = 1;

        context_t(const std::string& _file) : file(_file) {}
        context_t(void* _resource) : resource(_resource) {}
    };

    using delete_method_t = void (*)(void*);

    context_t* ptr = nullptr;

    

    // == delete ==

    void _delete(delete_method_t delete_method) noexcept {
        ptr->use_count--;
        if (ptr->use_count == 0) {
            if (ptr->resource) {
                delete_method(ptr->resource);
            }
            delete ptr;
        }
    }


    // == init ==

    sdl_resource_t(const std::string& file, size_t back_size) {
        ptr = (context_t*)malloc(sizeof(context_t) + back_size);
        new (ptr) context_t(file);
    }

    sdl_resource_t(void* resouce, size_t back_size) {
        ptr = (context_t*)malloc(sizeof(context_t) + back_size);
        new (ptr) context_t(resouce);
    }

    constexpr sdl_resource_t() {}


    // == copy ==

    void _copy(const sdl_resource_t& other) noexcept {
        ptr = other.ptr;
        ptr->use_count++;
    }

public:
    const std::string& get_file_name() const {
        return ptr->file;
    }
};






class sdl_font_t : public sdl_resource_t {
    void load() const {
        int* ptsize = (int*)(ptr + 1);

        if (ptr->resource != nullptr) {
            if (ptsize[0] == 0) {
                ptsize[0] = TTF_FontHeight((TTF_Font*)ptr->resource);
            }
            return;
        }

        ptr->resource = TTF_OpenFont(ptr->file.c_str(), ptsize[0]);
        if (ptr->resource == nullptr) {
            throw sdl_make_exception("failed to open font '%s' since %s!", ptr->file.c_str(), TTF_GetError());
        }
    }

public:
    // == delete ==

    ~sdl_font_t() {
        _delete((delete_method_t)TTF_CloseFont);
    }

    // == init ==

    sdl_font_t(const std::string& file, int ptsize) : sdl_resource_t(file, sizeof(int)) {
        *(int*)(ptr + 1) = ptsize;
    }

    sdl_font_t(TTF_Font* font) : sdl_resource_t((void*)font, sizeof(int)) {}


    // == copy ==

    sdl_font_t(const sdl_font_t& other) noexcept {
        _copy(other);
    }

    sdl_font_t& operator=(const sdl_font_t& other) {
        _delete((delete_method_t)TTF_CloseFont);
        _copy(other);
        return *this;
    }

    // == move ==

    sdl_font_t(sdl_font_t&& other) noexcept {
        _copy(other);
    }

    sdl_font_t& operator=(sdl_font_t&& other) {
        _delete((delete_method_t)TTF_CloseFont);
        _copy(other);
        return *this;
    }
    


    // == get ==

    operator TTF_Font*() const {
        load();
        return (TTF_Font*)ptr->resource;
    }

    int get_size() const {
        load();
        return *(int*)(ptr + 1);
    }
};



enum sdl_render_text_mode_t {
    SDLAPP_TEXT_SOLID = 2,
    SDLAPP_TEXT_BLENDED,
    SDLAPP_TEXT_SHADED,
};


class sdl_surface_t : public sdl_resource_t {
    class back_t {
    public:
        sdl_font_t font;
        SDL_Color fg;
        SDL_Color bg;
        uint32_t warp_length;

        back_t(const sdl_font_t& font, const SDL_Color fg, const SDL_Color bg, uint32_t warp_length)
            : font(font), fg(fg), bg(bg), warp_length(warp_length) {}
    };

    void load() const {
        if (ptr->resource != nullptr) {
            return;
        }

        back_t* back = (back_t*)(ptr + 1);

        if (ptr->load_method == 0) {
            ptr->resource = IMG_Load(ptr->file.c_str());
            if (ptr->resource == nullptr) {
                throw sdl_make_exception("failed to load texture '%s', maybe the file not exist!", ptr->file.c_str());
            }

            return;
        }


        switch (ptr->load_method) {
            case SDLAPP_TEXT_SOLID:
                ptr->resource = TTF_RenderUTF8_Solid_Wrapped(back->font, ptr->file.c_str(), back->fg, back->warp_length);
                break;
            case SDLAPP_TEXT_BLENDED:
                ptr->resource = TTF_RenderUTF8_Blended_Wrapped(back->font, ptr->file.c_str(), back->fg, back->warp_length);
                break;
            case SDLAPP_TEXT_SHADED:
                ptr->resource = TTF_RenderUTF8_Shaded_Wrapped(back->font, ptr->file.c_str(), back->fg, back->bg, back->warp_length);
                break;



            default:
                throw sdl_make_exception("invalid text render mode %d!", ptr->load_method);
        }
        if (ptr->resource == nullptr) {
            throw sdl_make_exception("failed to render text '%s'!", ptr->file.c_str());
        }
    }

public:
    // == delete ==

    ~sdl_surface_t() {
        _delete((delete_method_t)SDL_FreeSurface);
        if (ptr->load_method != 0) {
            ((back_t*)(ptr + 1))->~back_t();
        }
    }


    // == init ==

    sdl_surface_t(const std::string& file = "") : sdl_resource_t(file, sizeof(SDL_Point)) {}
    sdl_surface_t(SDL_Texture* texture) : sdl_resource_t((void*)texture, sizeof(SDL_Point)) {}

    sdl_surface_t(sdl_font_t font, const std::string& text, sdl_render_text_mode_t mode, SDL_Color fg = SDLAPP_COLOR_WHITE, SDL_Color bg = SDLAPP_COLOR_BLACK, uint32_t warp_length = 0)
        : sdl_resource_t(text, sizeof(back_t)) {
        ptr->load_method = mode;

        back_t* back = (back_t*)(ptr + 1);
        new (back) back_t(font, fg, bg, warp_length);
    }


    // == copy ==

    sdl_surface_t(const sdl_surface_t& other) noexcept {
        _copy(other);
    }

    sdl_surface_t& operator=(const sdl_surface_t& other) {
        _delete((delete_method_t)SDL_DestroyTexture);
        _copy(other);
        return *this;
    }

    // == move ==

    sdl_surface_t(sdl_surface_t&& other) noexcept {
        _copy(other);
    }

    sdl_surface_t& operator=(sdl_surface_t&& other) {
        _delete((delete_method_t)SDL_FreeSurface);
        _copy(other);
        return *this;
    }
    


    // == get ==

    operator SDL_Surface*() const {
        load();
        return (SDL_Surface*)ptr->resource;
    }

    int get_width() const {
        load();
        return ((SDL_Surface*)ptr->resource)->w;
    }

    int get_height() const {
        load();
        return ((SDL_Surface*)ptr->resource)->h;
    }
};





class sdl_texture_t : public sdl_resource_t {
    class surface_back_t {
    public:
        int width = 0;
        int height = 0;
        sdl_surface_t surface;

        surface_back_t(const sdl_surface_t& surface) : surface(surface) {}
    };
    
    class render_back_t {
    public:
        int width = 0;
        int height = 0;
        sdl_font_t font;
        SDL_Color fg;
        SDL_Color bg;
        uint32_t warp_length;

        render_back_t(const sdl_font_t& font, const SDL_Color fg, const SDL_Color bg, uint32_t warp_length)
            : font(font), fg(fg), bg(bg), warp_length(warp_length) {}
    };

    void load() const {
        if (ptr->resource != nullptr) {
            return;
        }
        if (sdl_app_t::renderer == nullptr) {
            throw sdl_make_exception("failed to load texture '%s' since renderer still not created!", ptr->file.c_str());
        }


        if (ptr->load_method == 0) {
            ptr->resource = IMG_LoadTexture(sdl_app_t::renderer, ptr->file.c_str());
            if (ptr->resource == nullptr) {
                throw sdl_make_exception("failed to load texture '%s', maybe the file not exist!", ptr->file.c_str());
            }

            SDL_Point* size = (SDL_Point*)(ptr + 1);
            SDL_QueryTexture((SDL_Texture*)ptr->resource, nullptr, nullptr, &size->x, &size->y);
            return;
        }
        if (ptr->load_method == 1) {
            surface_back_t* back = (surface_back_t*)(ptr + 1);
            ptr->resource = SDL_CreateTextureFromSurface(sdl_app_t::renderer, back->surface);
            back->width = back->surface.get_width();
            back->height = back->surface.get_height();
            return;
        }


        render_back_t* back = (render_back_t*)(ptr + 1);
        sdl_surface_t surface(back->font, ptr->file, (sdl_render_text_mode_t)ptr->load_method, back->fg, back->bg, back->warp_length);
        
        ptr->resource = SDL_CreateTextureFromSurface(sdl_app_t::renderer, surface);
        back->width = surface.get_width();
        back->height = surface.get_height();
    }

public:
    // == delete ==

    ~sdl_texture_t() {
        _delete((delete_method_t)SDL_DestroyTexture);

        if (ptr->load_method == 1) {
            ((surface_back_t*)(ptr + 1))->~surface_back_t();
        }
        else if (ptr->load_method >= 2) {
            ((render_back_t*)(ptr + 1))->~render_back_t();
        }
    }


    // == init ==

    sdl_texture_t(const std::string& file = "") : sdl_resource_t(file, sizeof(SDL_Point)) {}
    sdl_texture_t(SDL_Texture* texture) : sdl_resource_t((void*)texture, sizeof(SDL_Point)) {}

    sdl_texture_t(const sdl_surface_t& surface) : sdl_resource_t(nullptr, sizeof(surface_back_t)) {
        ptr->load_method = 1;

        surface_back_t* back = (surface_back_t*)(ptr + 1);
        new (back) surface_back_t(surface);
    }

    sdl_texture_t(const sdl_font_t& font, const std::string& text, sdl_render_text_mode_t mode, SDL_Color fg = SDLAPP_COLOR_WHITE, SDL_Color bg = SDLAPP_COLOR_BLACK, uint32_t warp_length = 0)
        : sdl_resource_t(text, sizeof(render_back_t)) {
        ptr->load_method = mode;

        render_back_t* back = (render_back_t*)(ptr + 1);
        new (back) render_back_t(font, fg, bg, warp_length);
    }


    // == copy ==

    sdl_texture_t(const sdl_texture_t& other) noexcept {
        _copy(other);
    }

    sdl_texture_t& operator=(const sdl_texture_t& other) {
        _delete((delete_method_t)SDL_DestroyTexture);
        _copy(other);
        return *this;
    }

    // == move ==

    sdl_texture_t(sdl_texture_t&& other) noexcept {
        _copy(other);
    }

    sdl_texture_t& operator=(sdl_texture_t&& other) {
        _delete((delete_method_t)SDL_DestroyTexture);
        _copy(other);
        return *this;
    }
    


    // == get ==

    operator SDL_Texture*() const {
        load();
        return (SDL_Texture*)ptr->resource;
    }

    int get_width() const {
        load();
        return ((SDL_Point*)(ptr + 1))->x;
    }

    int get_height() const {
        load();
        return ((SDL_Point*)(ptr + 1))->y;
    }
};




class sdl_music_t : public sdl_resource_t {
    void load() const {
        if (ptr->resource != nullptr) {
            return;
        }

        ptr->resource = Mix_LoadMUS(ptr->file.c_str());
        if (ptr->resource == nullptr) {
            throw sdl_make_exception("failed to load music '%s', maybe the file not exist!", ptr->file.c_str());
        }
    }

public:
    // == delete ==

    ~sdl_music_t() {
        _delete((delete_method_t)Mix_FreeMusic);
    }


    // == init ==

    sdl_music_t(const std::string& file) : sdl_resource_t(file, 0) {}
    sdl_music_t(Mix_Music* music) : sdl_resource_t((void*)music, 0) {}


    // == copy ==

    sdl_music_t(const sdl_music_t& other) noexcept {
        _copy(other);
    }

    sdl_music_t& operator=(const sdl_music_t& other) {
        _delete((delete_method_t)Mix_FreeMusic);
        _copy(other);
        return *this;
    }

    // == move ==

    sdl_music_t(sdl_music_t&& other) noexcept {
        _copy(other);
    }

    sdl_music_t& operator=(sdl_music_t&& other) {
        _delete((delete_method_t)Mix_FreeMusic);
        _copy(other);
        return *this;
    }
    


    // == get ==

    operator Mix_Music*() const {
        load();
        return (Mix_Music*)ptr->resource;
    }
    

    // == play ==

    void play(int loops = 0) const {
        load();
        if (Mix_PlayMusic((Mix_Music*)ptr->resource, loops) != 0) {
            throw sdl_make_exception("failed to play music '%s'!", ptr->file.c_str());
        }
    }
};






