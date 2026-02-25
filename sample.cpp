// clang++ test.cpp -lsdl2 -lsdl2_ttf -lsdl2_image -lsdl2_mixer

#include "sdl2-app.hpp"

class hello_world_t : public sdl_app_t {
public:
    // == resources ==

    // a font resource, it will be loaded when first to use it.
    sdl_font_t font{"PixelMplus10-Regular.ttf", 48};

    // empty font resource, for dynamic load font after init_ttf()
    // but don't forget to set it before use, else it fail since no targeted file.
    sdl_font_t font_unloaded;


    // texture resource load from file.
    sdl_texture_t texture_titi{"titi.png"};

    // make texture by rendering text, the actually resource when you first to use it.
    // you can choice BLENDED, SOLID or SHADED, and set warp length
    sdl_texture_t texture_hello_world{font, "hello world", SDLAPP_TEXT_BLENDED, SDLAPP_COLOR_YELLOW, SDLAPP_COLOR_BLACK, 0};

    // empty texture resource.
    sdl_texture_t texture_unloaded;
    

    // music resource load from file.
    sdl_music_t music_the_world{"the-world!!!.mp3"};

    // empty music resource.
    sdl_music_t music_unloaded;



    // callback run when start, able to customize init process here.
    // sdl_app_t will init all if you didn't make this method.
    void on_setup() {
        init_sdl();
        init_ttf();

        init_window_title = "hello world";
        init_window_size = {600, 600};
        init_window_icon = "titi.png";
        init_window_renderer();

        disable_update();
        enable_lazy_draw();
    }

    // callback when getting event.
    // don't forget to call sdl_app_t::on_event(e) if you want to keep default event process.
    void on_event(SDL_Event& e) {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_BACKSPACE || e.key.keysym.sym == SDLK_ESCAPE) {
                send_stop_signal();
                return;
            }
        }
        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                if (e.window.data1 > e.window.data2) {
                    set_window_size(e.window.data1, e.window.data1);
                    return;
                }
                else if (e.window.data2 > e.window.data1) {
                    set_window_size(e.window.data2, e.window.data2);
                    return;
                }

                sdl_app_t::on_event(e);
                return;
            }
        }
        sdl_app_t::on_event(e);
    }

    // callback when update
    // if you don't have update process, just put disable_update() to on_setup().
    // you can control update frequency by set_update_delay(1000 / fps)
    void on_update(uint32_t tick) {
        
    }

    // callback when rendering window.
    // if you want to render only when event happen, put enable_lazy_draw() to on_setup() and call send_redraw_signal() when you want to redraw window, it will better performance for simply app.
    // you can control fps by set_render_delay(1000 / fps)
    void on_render(uint32_t tick) {
        SDL_FRect rect;

        set_render_draw_color(SDLAPP_COLOR_BLACK);
        render_clear();

        rect.w = (float)texture_titi.get_width() * (float)window_height / (float)texture_titi.get_height(),
        rect.h = (float)window_height;
        rect.x = ((float)window_width - rect.w) / 2.0f;
        rect.y = 0.0f;
        render_copy(texture_titi, nullptr, &rect);

        rect.w = window_width;
        rect.h = (float)texture_hello_world.get_height() * (float)window_width / (float)texture_hello_world.get_width();
        rect.x = 0.0f;
        rect.y = ((float)window_height - rect.h) / 2.0f;
        render_copy(texture_hello_world, nullptr, &rect);
    }
};


int main(int argc, char** argv) {
    hello_world_t app;
    app.run();
    return 0;
}