
#include <stdio.h>
#include <SDL2/SDL.h>
#include <complex.h>


#define WIDTH 800
#define HEIGHT 600
#define MAX_ITER 100

int mandelbrot(double complex c) {
    double complex z = 0;
    int n = 0;
    while (cabs(z) <= 2.0 && n < MAX_ITER) {
        z = z * z + c;
        n++;
    }
    return n;
}

void render_mandelbrot(SDL_Surface *surface, double xmin, double xmax, double ymin, double ymax) {
    for (int py = 0; py < HEIGHT; py++) {
        for (int px = 0; px < WIDTH; px++) {
            double x0 = xmin + (xmax - xmin) * px / (WIDTH - 1);
            double y0 = ymin + (ymax - ymin) * py / (HEIGHT - 1);
            double complex c = x0 + y0 * I;
            int n = mandelbrot(c);
            Uint8 shade = (n == MAX_ITER) ? 0 : 255; // 0 = black (in set), 255 = white (out)
            Uint32 color = SDL_MapRGB(surface->format, shade, shade, shade);
            Uint32 *pixels = (Uint32 *)surface->pixels;
            pixels[py * (surface->pitch / 4) + px] = color;
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Mandelbrot Set", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Surface *surface = SDL_GetWindowSurface(window);

    // Initial region in the complex plane
    double xmin = -2.0, xmax = 1.0;
    double ymin = -1.2, ymax = 1.2;

    render_mandelbrot(surface, xmin, xmax, ymin, ymax);
    SDL_UpdateWindowSurface(window);

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;
                
                // Convert pixel coordinates to complex plane coordinates
                double cx = xmin + (xmax - xmin) * mouse_x / (WIDTH - 1);
                double cy = ymin + (ymax - ymin) * mouse_y / (HEIGHT - 1);
                
                double zoom_factor;
                if (event.button.button == SDL_BUTTON_LEFT) {
                    zoom_factor = 0.5;
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    zoom_factor = 2.0;
                } else {
                    continue;
                }
                
                // Calculate new bounds
                double width = (xmax - xmin) * zoom_factor;
                double height = (ymax - ymin) * zoom_factor;
                
                xmin = cx - width / 2.0;
                xmax = cx + width / 2.0;
                ymin = cy - height / 2.0;
                ymax = cy + height / 2.0;
                
                // Re-render
                render_mandelbrot(surface, xmin, xmax, ymin, ymax);
                SDL_UpdateWindowSurface(window);
            }
        }
        SDL_Delay(10);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}