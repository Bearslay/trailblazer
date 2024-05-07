#ifndef PERLIN
#define PERLIN

#include <cmath>
#include <vector>
#include <utility>

double interpolate(double a0, double a1, double w) {
    // Add to clamp values
    // if (0.0 > w) return a0;
    // if (1.0 < w) return a1;

    // Linear interpolation
    // return (a1 - a0) * w + a0;
    
    // Cubic interpolation (smoother than linear)
    // return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;

    // Smoothest interpolation (2nd derivative is zero on boundaries)
    return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}

std::pair<double, double> randomGradient(int ix, int iy) {
    const unsigned w = 8 * sizeof(unsigned), s = w / 2;
    unsigned a = ix, b = iy;

    a *= 3284157443;
    
    b ^= a << s | a >> (w - s);
    b *= 1911520717;
    
    a ^= b << s | b >> (w - s);
    a *= 2048419325;
    
    double random = a * (M_PI / ~(~0u >> 1));
    return {std::cos(random), std::sin(random)};
}

double dotGridGradient(int ix, int iy, double x, double y) {
    std::pair<double, double> gradient = randomGradient(ix, iy);
    return ((x - (double)ix) * gradient.first + (y - (double)iy) * gradient.second);
}

double perlin(double x, double y) {
    int x0 = (int)x, x1 = x0 + 1, y0 = (int)y, y1 = y0 + 1;
    double sx = x - (double)x0;

    // Range from [-1, 1]
    return interpolate(interpolate(dotGridGradient(x0, y0, x, y), dotGridGradient(x1, y0, x, y), sx), interpolate(dotGridGradient(x0, y1, x, y), dotGridGradient(x1, y1, x, y), sx), y - (double)y0);
    // Range from [0, 1]
    // return interpolate(interpolate(dotGridGradient(x0, y0, x, y), dotGridGradient(x1, y0, x, y), sx), interpolate(dotGridGradient(x0, y1, x, y), dotGridGradient(x1, y1, x, y), sx), y - (double)y0) * 0.5 + 0.5;
}

std::vector<std::vector<double>> getNoiseGrid(int w, int h, int o = 8, double bias = 2.0, double scale = 350.0) {
    std::vector<std::vector<double>> output;

    for (int i = 0; i < h; i++) {
        output.emplace_back();
        for (int j = 0; j < w; j++) {
            double val = 0.0, freq = 1.0, amp = 1.0;

            for (int k = 0; k < o; k++) {
                val += perlin(i * freq / scale, j * freq / scale) * amp;

                freq *= bias;
                amp /= bias;
            }

            if (val > 1.0) {val = 1.0;}
            else if (val < -1.0) {val = -1.0;}

            output[i].emplace_back(val * 0.5 + 0.5);
        }
    }

    return output;
}

#endif /* PERLIN */
