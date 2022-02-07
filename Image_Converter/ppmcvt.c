#include <stdio.h>
#include <stddef.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include "pbm.h"
void convert_ppm_to_pbm(const PPMImage* ppm, PBMImage* pbm);
void convert_ppm_to_pgm(const PPMImage* ppm, PGMImage* pgm);
void isolate_or_remove(const PPMImage* ppm, PPMImage* new_ppm, const char* channel, char option);
void sepia(const PPMImage* ppm, PPMImage* new_ppm);
void mirror(const PPMImage* ppm, PPMImage* new_ppm);
void make_thumbnail(const PPMImage* ppm, PPMImage* new_ppm, int thumbnail_scale);
void make_nup(const PPMImage* ppm, PPMImage* new_ppm);
int main( int argc, char *argv[] )
{
	int o;
	PPMImage* ppm;
	PBMImage* pbm;
	PGMImage* pgm;
	PPMImage* new_ppm;
	char* output;
	char* input;
	unsigned int h;
	unsigned int w;
	unsigned int m;
	unsigned int grayscale;
	int thumbnail_scale;
	char* temp;
	char* channel;
	char* argv_first;
	char option = 'o';
	int is_first_flag = 1;
	int visited = 0;	
	if (argc == 1) {
		printf("Error: No input file specified\n");
		goto out;
	}		
	while ((o = getopt(argc, argv, "bg:i:r:smt:n:o:")) != -1) {
		switch(o) {
		case 'b' :
			if (is_first_flag != 1) {
				printf("Error: Multiple transformations specified\n");
				goto out;
			}
			is_first_flag = 0;
			option = 'b';		
			break;
		case 'g' :
			if (is_first_flag != 1) {
				printf("Error: Multiple transformations specified\n");
				goto out;
			}
			is_first_flag = 0;
			option = 'g';
			if (strlen(optarg) >= 6) {
				printf("Error: Invalid max grayscale pixel value: %s; must be less than 65,536\n", optarg);
				goto out;
			}	
			grayscale = atoi(optarg);	
			if (grayscale == 0 || grayscale >= 65536) {
				printf("Error: Invalid max grayscale pixel value: %s; must be less than 65,536\n", optarg);
				goto out;
			}	
			break;
		case 'i':
			if (is_first_flag != 1) {
				printf("Error: Muliple transformations specified\n");
				goto out;
			}
			is_first_flag = 0;
			option = 'i';
			channel = optarg;
			if (strcmp(channel, "red") != 0 && strcmp(channel, "green") != 0 && strcmp(channel, "blue") != 0) {
				printf("Error: Invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", channel);
				goto out;
			}	
			break;
		case 'r':
			if (is_first_flag != 1) {
				printf("Error: Multiple transformations specified\n");
				goto out;
			}
			is_first_flag = 0;
			option = 'r';
			channel = optarg;	
			if (strcmp(channel, "red") != 0 && strcmp(channel, "green") != 0 && strcmp(channel, "blue") != 0) {
				printf("Error: Invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", channel);
				goto out;
			}	
			break;
		case 's':
			if (is_first_flag != 1) {
				printf("Error: Multiple transformations specified\n");
				goto out;
			}
			is_first_flag = 0;
			option = 's';		
			break;	
		case 'm' :
			if (is_first_flag != 1) {
				printf("Error: Multiple transformations specified\n");
				goto out;
			}
			is_first_flag = 0;
			option = 'm';	
			break;	
		case 't':
			if (is_first_flag != 1) {
				printf("Error: Multiple transformations specified\n");
				goto out;
			}
			is_first_flag = 0;
			option = 't';
			thumbnail_scale = atoi(optarg);		
			if (thumbnail_scale == 0 || thumbnail_scale > 8) {
				printf("Error: Invalid scale factor: %d; must be 1-8\n", thumbnail_scale);
				goto out;
			}	
			if (strlen(optarg) != 1) {
				printf("fopen(): No such file or directory\n");
				goto out;
			}	
			break;
		case 'n':
			if (is_first_flag != 1) {
				printf("Error: Multiple transformations specified\n");
				goto out;
			}
			is_first_flag = 0;
			option = 'n';		
			thumbnail_scale = atoi(optarg);	
			if (thumbnail_scale == 0 || thumbnail_scale > 8) {
				printf("Error: Invalid scale factor: %d; must be 1-8\n", thumbnail_scale);
				goto out;
			}
			if (strlen(optarg) != 1) {
				printf("fopen(): No such file or directory\n");
				goto out;
			}		
			break;
		case 'o':
			if (option == 'o' && strcmp(argv[1], "-o") != 0) {
				output = NULL;
				visited = 1;
				break;
			}	
			if (optind >= argc) {
				printf("Error: No input file specified\n");
				goto out;
			}	
			output = optarg;	
			input = argv[optind];
			visited = 1;
			break;	
		case '?' :
			printf("Usage: ppmcvt[-bgirsmtno] [FILE]\n");
			goto out;		
		}
		

	}
    if (!visited) {
        printf("Error: No output file sepcified\n");
        goto out;
    }	
    ppm = read_ppmfile(input);
    w = ppm->width;
    h = ppm->height;
    if (option == 'o' || option == 'b') {
            pbm = new_pbmimage(w, h);
            convert_ppm_to_pbm(ppm, pbm);
            write_pbmfile(pbm, output);
            del_pbmimage(pbm);
    } else if (option == 'g') {
            pgm = new_pgmimage(w, h, grayscale);
            convert_ppm_to_pgm(ppm, pgm);
            write_pgmfile(pgm, output);
            del_pgmimage(pgm);
    } else if (option == 'i' || option == 'r') {
            new_ppm = new_ppmimage(w, h, ppm->max);
            isolate_or_remove(ppm, new_ppm, channel, option);
            write_ppmfile(new_ppm, output);
            del_ppmimage(new_ppm);
    } else if (option == 's') {
            new_ppm = new_ppmimage(w, h, ppm->max);
            sepia(ppm, new_ppm);
            write_ppmfile(new_ppm, output);
            del_ppmimage(new_ppm);
    } else if (option == 'm') {
            new_ppm = new_ppmimage(w, h, ppm->max);
            mirror(ppm, new_ppm);
            write_ppmfile(new_ppm, output);
            del_ppmimage(new_ppm);
    } else if (option == 't') {
            unsigned int new_width = (ppm->width % thumbnail_scale == 0) ? (ppm->width / thumbnail_scale) : (ppm->width / thumbnail_scale + 1);
            unsigned int new_height = (ppm->height % thumbnail_scale == 0) ? (ppm->height / thumbnail_scale) : (ppm->height / thumbnail_scale + 1);
            new_ppm = new_ppmimage(new_width, new_height, ppm->max);
            make_thumbnail(ppm, new_ppm, thumbnail_scale);
            write_ppmfile(new_ppm, output);
            del_ppmimage(new_ppm);
    } else if (option = 'n') {
            unsigned int new_width = (ppm->width % thumbnail_scale == 0) ? (ppm->width / thumbnail_scale) : (ppm->width / thumbnail_scale + 1);
            unsigned int new_height = (ppm->height % thumbnail_scale == 0) ? (ppm->height / thumbnail_scale) : (ppm->height / thumbnail_scale + 1);
            PPMImage* temp_ppm = new_ppmimage(new_width, new_height, ppm->max);
            make_thumbnail(ppm, temp_ppm, thumbnail_scale);
            new_ppm = new_ppmimage(w, h, ppm->max);
            make_nup(temp_ppm, new_ppm);
            write_ppmfile(new_ppm, output);
            del_ppmimage(new_ppm);
            del_ppmimage(temp_ppm);
    }
    del_ppmimage(ppm);
	return 0;
out:
	return 1;	
}

void convert_ppm_to_pbm(const PPMImage* ppm, PBMImage* pbm)
{
	unsigned int i;
	unsigned int j;
	float avg;	
	for (i = 0; i < ppm->height; ++i) {
	       	for (j = 0; j < ppm->width; ++j) {	
			avg = (ppm->pixmap[0][i][j] + ppm->pixmap[1][i][j] + ppm->pixmap[2][i][j]) / 3.0;	
			pbm->pixmap[i][j] = (avg < ((ppm->max) / 2.0)) ? 1 : 0;
		}
	}
}

void convert_ppm_to_pgm(const PPMImage* ppm, PGMImage* pgm)
{
	unsigned int i;
	unsigned int j;
	float avg;
	for (i = 0; i < ppm->height; ++i) {
		for (j = 0; j < ppm->width; ++j) {
			avg = (ppm->pixmap[0][i][j] + ppm->pixmap[1][i][j] + ppm->pixmap[2][i][j]) / 3.0;
			pgm->pixmap[i][j] = (unsigned int)((avg / ppm->max) * (pgm->max));		
		}	
	}
}

void isolate_or_remove(const PPMImage* ppm, PPMImage* new_ppm, const char* channel, char option)
{
	unsigned int i;
	unsigned int j;
	if (strcmp(channel, "red") == 0) {
		for (i = 0; i < ppm->height; ++i) {
			for (j = 0; j < ppm->width; ++j) {
				new_ppm->pixmap[0][i][j] = option == 'i' ? ppm->pixmap[0][i][j] : 0;
				new_ppm->pixmap[1][i][j] = option == 'i' ? 0 : ppm->pixmap[1][i][j]; 
				new_ppm->pixmap[2][i][j] = option == 'i' ? 0 : ppm->pixmap[2][i][j];
			}
		}
		return;
	}
	if (strcmp(channel, "green") == 0) {
		for (i = 0; i < ppm->height; ++i) {
			for (j = 0; j < ppm->width; ++j) {
				new_ppm->pixmap[0][i][j] = option == 'i' ? 0 : ppm->pixmap[0][i][j]; 
				new_ppm->pixmap[1][i][j] = option == 'i' ? ppm->pixmap[1][i][j] : 0;
				new_ppm->pixmap[2][i][j] = option == 'i' ? 0 : ppm->pixmap[2][i][j]; 
			}
		}
		return;
	}	
	if (strcmp(channel, "blue") == 0) {
		for (i = 0; i < ppm->height; ++i) {
			for (j = 0; j < ppm->width; ++j) {
				new_ppm->pixmap[0][i][j] = option == 'i' ? 0 : ppm->pixmap[0][i][j]; 
				new_ppm->pixmap[1][i][j] = option == 'i' ? 0 : ppm->pixmap[1][i][j];
				new_ppm->pixmap[2][i][j] = option == 'i' ? ppm->pixmap[2][i][j] : 0;
			}
		}
		return;
	}
}
void sepia(const PPMImage* ppm, PPMImage* new_ppm)
{
	unsigned int i;
	unsigned int j;
	for (i = 0; i < ppm->height; ++i) {
		for (j = 0; j < ppm->width; ++j) {	
			unsigned int new_red;
			unsigned int new_green;
			unsigned int new_blue;
			new_red = (unsigned int)(0.393 * ppm->pixmap[0][i][j] + 0.769 * ppm->pixmap[1][i][j] + 0.189 * ppm->pixmap[2][i][j]);
			new_green = (unsigned int)(0.349 * ppm->pixmap[0][i][j] + 0.686 * ppm->pixmap[1][i][j] + 0.168 * ppm->pixmap[2][i][j]);
			new_blue = (unsigned int)(0.272 * ppm->pixmap[0][i][j] + 0.534 * ppm->pixmap[1][i][j] + 0.131 * ppm->pixmap[2][i][j]);
			new_ppm->pixmap[0][i][j] = new_red > ppm->max ? ppm->max : new_red; 
			new_ppm->pixmap[1][i][j] = new_green > ppm->max ? ppm->max : new_green; 
			new_ppm->pixmap[2][i][j] = new_blue > ppm->max ? ppm->max : new_blue;
		}
	}
}

void mirror(const PPMImage* ppm, PPMImage* new_ppm) 
{
	unsigned int i;
	unsigned int j;
	unsigned int k;	
	for (i = 0; i < ppm->height; ++i) {
		j = 0;
		k = ppm->width - 1;
		while (j < k) {
			new_ppm->pixmap[0][i][j] = ppm->pixmap[0][i][j];
			new_ppm->pixmap[1][i][j] = ppm->pixmap[1][i][j];
			new_ppm->pixmap[2][i][j] = ppm->pixmap[2][i][j];
			new_ppm->pixmap[0][i][k] = ppm->pixmap[0][i][j];
			new_ppm->pixmap[1][i][k] = ppm->pixmap[1][i][j];
			new_ppm->pixmap[2][i][k] = ppm->pixmap[2][i][j];	
			j++;
			k--;
		}
		if (j == k) {
			new_ppm->pixmap[0][i][j] = ppm->pixmap[0][i][j];
			new_ppm->pixmap[1][i][j] = ppm->pixmap[1][i][j];
			new_ppm->pixmap[2][i][j] = ppm->pixmap[2][i][j];
		}		
	}
}

void make_thumbnail(const PPMImage* ppm, PPMImage* new_ppm, int thumbnail_scale) 
{
	unsigned int i;
	unsigned int j;
	for (i = 0; i < new_ppm->height; ++i) {
		if (i * thumbnail_scale >= ppm->height) {
			continue;
		}
		for (j = 0; j < new_ppm->width; ++j) {
			if (j * thumbnail_scale >= ppm->width) {
				continue;
			}
			new_ppm->pixmap[0][i][j] = ppm->pixmap[0][i * thumbnail_scale][j * thumbnail_scale];
			new_ppm->pixmap[1][i][j] = ppm->pixmap[1][i * thumbnail_scale][j * thumbnail_scale];
			new_ppm->pixmap[2][i][j] = ppm->pixmap[2][i * thumbnail_scale][j * thumbnail_scale];
		}
	}	
}	

void make_nup(const PPMImage* ppm, PPMImage* new_ppm) {
	unsigned int i;
	unsigned int j;	
	unsigned int a;
	unsigned int b;	
	a = 0;	
	for (i = 0; i < new_ppm->height; ++i) {
		b = 0;	
		for (j = 0; j < new_ppm->width; ++j) {	
			new_ppm->pixmap[0][i][j] = ppm->pixmap[0][a][b]; 
			new_ppm->pixmap[1][i][j] = ppm->pixmap[1][a][b]; 
			new_ppm->pixmap[2][i][j] = ppm->pixmap[2][a][b];
			b++;
			if (b == ppm->width) {
				b = 0;
			}
		}
		a++;
		if (a == ppm->height) {
			a = 0;
		}
	}		
}

		
