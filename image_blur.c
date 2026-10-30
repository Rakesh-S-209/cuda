#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Simple BMP Header structures
#pragma pack(push, 1)
typedef struct {
    unsigned short bfType;
    unsigned int   bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int   bfOffBits;
} BMPHeader;

typedef struct {
    unsigned int   biSize;
    int            biWidth;
    int            biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int   biCompression;
    unsigned int   biSizeImage;
    int            biXPelsPerMeter;
    int            biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biImportant;
} BMPInfoHeader;
#pragma pack(pop)

// Structure to hold pixel data
typedef struct {
    unsigned char r, g, b;
} Pixel;

// Function to apply Box Blur using OpenMP on Intel i7 CPU cores
void apply_box_blur(Pixel *input, Pixel *output, int width, int height, int radius) {
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r_sum = 0, g_sum = 0, b_sum = 0;
            int count = 0;

            // Average neighboring pixels (Convolution kernel)
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    int ny = y + dy;
                    int nx = x + dx;

                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        int idx = ny * width + nx;
                        r_sum += input[idx].r;
                        g_sum += input[idx].g;
                        b_sum += input[idx].b;
                        count++;
                    }
                }
            }

            int out_idx = y * width + x;
            output[out_idx].r = (unsigned char)(r_sum / count);
            output[out_idx].g = (unsigned char)(g_sum / count);
            output[out_idx].b = (unsigned char)(b_sum / count);
        }
    }
}

int main() {
    char input_filename[256] = "input.bmp";
    char output_filename[256] = "output_blurred.bmp";
    int blur_radius = 3; // Adjust blur intensity

    FILE *f_in = fopen(input_filename, "rb");
    if (!f_in) {
        printf("Error: Could not open input.bmp. Please place a 24-bit BMP file named 'input.bmp' in the project folder.\n");
        return 1;
    }

    BMPHeader header;
    BMPInfoHeader infoHeader;

    fread(&header, sizeof(BMPHeader), 1, f_in);
    fread(&infoHeader, sizeof(BMPInfoHeader), 1, f_in);

    if (infoHeader.biBitCount != 24) {
        printf("Error: Only 24-bit uncompressed BMP images are supported in this simple C demo.\n");
        fclose(f_in);
        return 1;
    }

    int width = infoHeader.biWidth;
    int height = abs(infoHeader.biHeight);
    int row_stride = (width * 3 + 3) & (~3); // BMP rows are padded to multiples of 4 bytes

    unsigned char *img_data = (unsigned char *)malloc(row_stride * height);
    fseek(f_in, header.bfOffBits, SEEK_SET);
    fread(img_data, row_stride * height, 1, f_in);
    fclose(f_in);

    // Convert raw BMP bytes to 2D Pixel array
    Pixel *input_pixels = (Pixel *)malloc(width * height * sizeof(Pixel));
    Pixel *output_pixels = (Pixel *)malloc(width * height * sizeof(Pixel));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int src_idx = (height - 1 - y) * row_stride + x * 3; // BMP is bottom-up
            int dest_idx = y * width + x;
            input_pixels[dest_idx].b = img_data[src_idx + 0];
            input_pixels[dest_idx].g = img_data[src_idx + 1];
            input_pixels[dest_idx].r = img_data[src_idx + 2];
        }
    }

    printf("Processing image (%d x %d) using Intel i7 multi-core CPU with OpenMP...\n", width, height);
    double start_time = omp_get_wtime();

    // Run the blur algorithm
    apply_box_blur(input_pixels, output_pixels, width, height, blur_radius);

    double end_time = omp_get_wtime();
    printf("Blur completed in %.4f seconds!\n", end_time - start_time);

    // Convert pixels back to BMP byte array
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int src_idx = y * width + x;
            int dest_idx = (height - 1 - y) * row_stride + x * 3;
            img_data[dest_idx + 0] = output_pixels[src_idx].b;
            img_data[dest_idx + 1] = output_pixels[src_idx].g;
            img_data[dest_idx + 2] = output_pixels[src_idx].r;
        }
    }

    // Save output image
    FILE *f_out = fopen(output_filename, "wb");
    f_in = fopen(input_filename, "rb"); // Copy header from original
    unsigned char *temp_header_buf = (unsigned char *)malloc(header.bfOffBits);
    fread(temp_header_buf, header.bfOffBits, 1, f_in);
    fwrite(temp_header_buf, header.bfOffBits, 1, f_out);
    fclose(f_in);

    fwrite(img_data, row_stride * height, 1, f_out);
    fclose(f_out);

    free(img_data);
    free(input_pixels);
    free(output_pixels);

    printf("Result saved to %s. Launching native viewer window...\n", output_filename);

    // Pop up the result in a native OS window automatically
    #if defined(_WIN32)
        system("start output_blurred.bmp");
    #elif defined(__APPLE__)
        system("open output_blurred.bmp");
    #else
        system("xdg-open output_blurred.bmp");
    #endif

    return 0;
}