#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include "font.xbf"

; // ???

int width = 320;
int height = 240;
long int screensize = width * height;
short *fbp = 0;

void setPixel(int x, int y, int r, int g, int b)
{
    if (x >= width || y >= height) return;

    fbp[x + y * width] = r << 11 | g << 6 | b << 0;
}

void drawString(int px, int py, int scale, const char str[], int r, int g, int b)
{
    for (int i = 0; i < strlen(str); i++)
    {
        char c = str[i];

        for (char x = 0; x < font_width; x++)
        {
            char xb = 1 << x;

            for (char y = 0; y < font_height; y++)
            {
                if (font[c][y] & xb)
                {
                    for (int j = 0; j < scale * scale; j++)
                    {
                        setPixel(
                                px + x * scale + j % scale + scale * (font_width + 1) * i,
                                py + y * scale + j / scale,
                                r, g, b);
                    }
                }
            }
        }
    }
}

void drawBackground()
{
    for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
        setPixel(x, y, x * 0x10 / width, 0, y * 0x10 / height);
}

int main(int argc, char* argv[])
{
    int fbfd = 0;

    // Open the file for reading and writing
    fbfd = open("/dev/fb1", O_RDWR);
    if (!fbfd)
    {
        printf("Error: cannot open framebuffer device.\n");
        return(1);
    }

    fbp = (short*) mmap(0, 
                        screensize * 2, 
                        PROT_READ | PROT_WRITE, 
                        MAP_SHARED, 
                        fbfd, 0);

    time_t rawtime;
    struct tm * timeinfo;
    char timestr[10];

    std::string input;
    while (getline(std::cin, input))
    {
        drawBackground();

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        // strftime(timestr, sizeof(timestr), "%H%M", timeinfo);

        sprintf(timestr, "%.2i:%.2i", (rawtime / 60 / 60 + 2) % 24, rawtime / 60 % 60);

        drawString(20, 20, 9, timestr, 0x1f, 0x1f, 0x1f);

        char s = 3;
        int y = 100;
        char *line = std::strtok(const_cast<char*>(input.c_str()), "\x01");
        while (line != NULL)
        {
            drawString(20, y, s, line, 0x1f, 0x10 + s * 3, 0x00);
            y += (font_height) * s + 8;
            if (s > 1) s--;
            line = std::strtok(NULL, "\x01");
        }
    }

    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
