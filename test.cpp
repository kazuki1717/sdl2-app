#include "sdl2-app.hpp"

class hello_world_t : public sdl_app_t {
public:
    // == resources ==
    // resources will be loaded lazily when first used
    // we can use them after on_setup(), like on_prepare()

    sdl_font_t font_rog_regular{"ROGFonts-Regular.otf", 48};

    sdl_texture_t texture_titi{"titi.png"};
    sdl_texture_t texture_hello_world{font_rog_regular, "hello world", SDLAPP_TEXT_BLENDED, SDLAPP_COLOR_YELLOW, SDLAPP_COLOR_BLACK, 0};


    // override method 
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

    void on_render(uint32_t tick) {
        SDL_FRect rect;

        render_set_draw_color();
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