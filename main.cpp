// clang++ main.cpp -lsdl2 -lsdl2_ttf -lsdl2_image -lsdl2_mixer

#include "sdlapp2.hpp"

class hello_world_t : public sdl_window_t {
public:
    // == resources ==

    sdl_font_t font{"PixelMplus10-Regular.ttf", 48};    // a font resource, it will be loaded when first to use it.
    
    sdl_font_t font_empty;      // empty font resource, for dynamic load font after init_ttf()
                                // but don't forget to set it before use, else it fail since no targeted file.



    sdl_texture_t img_titi{this, "titi.png"};     // texture resource load from file.

    // make texture by rendering text, the actually resource when you first to use it.
    // you can choice BLENDED, SOLID or SHADED, and set warp length
    sdl_texture_t img_text_hello_world{this, font, "hello world", SDLAPP_TEXT_SOLID, SDLAPP_COLOR_PURPLE};
    
    sdl_texture_t img_empty{this};        // empty texture targeted to "".
    


    sdl_music_t music_the_world{"the-world!!!.mp3"};    // music resource load from file.
    sdl_music_t music_empty;                            // empty music resource.



    // == code ==

    // callback run when start, able to customize init process here.
    // sdl_app_t will init all if you didn't make this method.
    void on_setup() {
        init_info_t info;

        info.mixer_flags = 0;
        init_sdl(info);

        info.wnd_title = "hello world";
        info.wnd_size = {818, 818};
        info.wnd_icon = "titi.png";
        init_window(info);

        disable_update();
        // enable_lazy_draw();
    }

    // callback when getting event.
    // don't forget to call sdl_app_t::on_event(e) if you want to keep default event process.
    void on_event(SDL_Event& e) {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_BACKSPACE || e.key.keysym.sym == SDLK_ESCAPE) {
                post_stop();
                return;
            }
        }
        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                if (e.window.data1 > e.window.data2) {
                    set_window_size(window, e.window.data1, e.window.data1);
                    return;
                }
                else if (e.window.data2 > e.window.data1) {
                    set_window_size(window, e.window.data2, e.window.data2);
                    return;
                }
            }
        }
        sdl_window_t::on_event(e);
    }

    // callback when update
    // if you don't have update process, just put disable_update() to on_setup().
    // you can control update frequency by set_update_delay(1000 / fps)
    void on_update(sdl_tick_t tick) {
        
    }

    // callback when rendering window.
    // if you want to render only when event happen, put enable_lazy_draw() to on_setup() and call send_redraw_signal() when you want to redraw window, it will better performance for simply app.
    // you can control fps by set_render_delay(1000 / fps)
    void on_render(sdl_tick_t tick) {
        SDL_FRect rect;

        rect = {0, 0, (float) window_width, (float) window_height};
        render_copy(renderer, img_titi, nullptr, &rect);

        rect.w = window_width * 0.9f;
        rect.h = (float) img_text_hello_world.get_height() * (float) window_width / (float) img_text_hello_world.get_width();
        rect.x = ((float) window_width - rect.w) * 0.5f;
        rect.y = ((float) window_height - rect.h) * 0.3f;
        render_copy(renderer, img_text_hello_world, nullptr, &rect);
    }
};


int main(int argc, char** argv) {
    hello_world_t hello_world;
    hello_world.run();
    return 0;
}