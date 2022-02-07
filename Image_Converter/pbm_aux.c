#include <stddef.h>
#include <stdlib.h>
#include "pbm.h"

PPMImage* new_ppmimage(unsigned int w, unsigned int h, unsigned int m)
{
    PPMImage* ppm;
    size_t i;
    size_t j;
    ppm = (PPMImage*)malloc(sizeof(unsigned int**) * 3 + sizeof(unsigned int) * 3);
    if (!ppm) return NULL;
    ppm->height = h;
    ppm->width = w;
    ppm->max = m;
    for (i = 0; i < 3; ++i) {
        ppm->pixmap[i] = (unsigned int**)malloc(sizeof(unsigned int*) * h);
        for (j = 0; j < h; ++j) {
            ppm->pixmap[i][j] = (unsigned int*)malloc(sizeof(unsigned int) * w);
        }
    }
    return ppm;
}

PBMImage* new_pbmimage(unsigned int w, unsigned int h)
{
    PBMImage* pbm;
    size_t i;
    pbm = (PBMImage*)malloc(sizeof(unsigned int**) * h + sizeof(unsigned int) * 2);
    if (!pbm) return NULL;
    pbm->height = h;
    pbm->width = w;
    pbm->pixmap = (unsigned int**)malloc(sizeof(unsigned int*) * h);
    for (i = 0; i < h; ++i) {
        pbm->pixmap[i] = (unsigned int*)malloc(sizeof(unsigned int) * w);
    }
    return pbm;
}

PGMImage* new_pgmimage(unsigned int w, unsigned int h, unsigned int m)
{
    PGMImage* pgm;
    size_t i;
    pgm = (PGMImage*)malloc(sizeof(unsigned int**) * h + sizeof(unsigned int) * 3);
    if (!pgm) return NULL;
    pgm->height = h;
    pgm->width = w;
    pgm->max = m;
    pgm->pixmap = (unsigned int**)malloc(sizeof(unsigned int*) * h);
    for (i = 0; i < h; ++i) {
        pgm->pixmap[i] = (unsigned int*)malloc(sizeof(unsigned int) * w);
    }
    return pgm;
}

void del_ppmimage(PPMImage* p)
{
    size_t i;
    size_t j;
    unsigned int h = p->height;
    for (i = 0; i < 3; ++i) {
        for (j = 0; j < h; ++j) {
            free(p->pixmap[i][j]);
            p->pixmap[i][j] = NULL;
        }
        free(p->pixmap[i]);
        p->pixmap[i] = NULL;
    }
    free(p);
    p = NULL;
}

void del_pgmimage(PGMImage* p)
{
    size_t i;
    unsigned int h;
    h = p->height;
    for (i = 0; i < h; ++i) {
        free(p->pixmap[i]);
        p->pixmap[i] = NULL;
    }
    free(p->pixmap);
    p->pixmap = NULL;
    free(p);
    p = NULL;
}

void del_pbmimage(PBMImage* p)
{
    size_t i;
    unsigned int h;
    h = p->height;
    for (i = 0; i < h; ++i) {
        free(p->pixmap[i]);
        p->pixmap[i] = NULL;
    }
    free(p->pixmap);
    p->pixmap = NULL;
    free(p);
    p = NULL;
}

