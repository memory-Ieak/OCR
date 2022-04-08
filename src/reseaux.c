#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <gtk/gtk.h>
#include <err.h>
#include <SDL.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "Display.h"
#include "pixel_operations.h"
#include "detection_character.h"

void add(long double m1[], long double m2[], int rows, int cols, long double r[])
{
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            r[i*cols+j] = m1[i*cols+j] + m2[i*cols+j];
        }
    }
}

void sous(long double m1[], long double m2[], int rows, int cols, long double r[])
{
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            r[i*cols+j] = m1[i*cols+j] - m2[i*cols+j];
        }
    }
}

void add_solo(long double m1[], long double m2[], int rows, int cols)
{
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            m1[i*cols+j] += m2[i*cols+j];
        }
    }
}

void transpose(long double m[], int rows, int cols, long double r[])
{
    for(int i = 0; i < cols; i++){
        for(int j = 0; j < rows; j++){
            r[i * rows + j] = m[j * cols + i];
        }
    }
}

void mul(long double m1[], long double m2[], int r1, int c1, int c2, long double r[])
{
	long double res = 0;
    for(int i = 0; i < r1; i++){
        for(int j = 0; j < c2; j++){
            res = 0;
            for (int k = 0; k < c1; k++){
                res += m1[i*c1+k]*m2[k*c2+j];
            }
            r[i*c2+j] = res;
        }
    }
}

long double sigmoid(long double x){
    return 1 / (1 + exp(-x));
}

void hidden_layer_calcul(long double hidden_layer[], long double input_layer[], long double hidden_layer_weights[], int size){
    long double matrice[size];
    mul(input_layer, hidden_layer_weights, 1, 784, size, matrice);
    for(int i = 0; i < size; i++){
        hidden_layer[i] = sigmoid(matrice[i]);
    }
}

void output_layer_calcul(long double output_layer[], long double hidden_layer[], long double output_layer_weights[], int size){
    long double matrice[5];
    mul(hidden_layer, output_layer_weights, 1, size, 5, matrice);
    for(int i = 0; i < 5; i++){
        output_layer[i] = sigmoid(matrice[i]); 
    }
}

void image_to_array(SDL_Surface* image_surface, long double input_layer[])
{
	int comp = 0;
	for(int i = 0; i < 28; i++){
		for(int j = 0; j < 28; j++){
			Uint32 pixel = get_pixel(image_surface, i, j);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			long double average = 1;
			if(r + g + b > 127*3)
			{
				average = 0;
			}
			input_layer[comp] = average;
			comp++;
		}
	}
}

void ImportArray(char string[], long double w[])
{
	FILE* fichier = NULL;
    char chaine[30] = "";
    long value = 0;
 
    fichier = fopen(string, "r");
    
    if (fichier != NULL)
    {
		while (fgets(chaine, 30, fichier) != NULL) // On lit le fichier tant qu'on ne reçoit pas d'erreur (NULL)
        {
			w[value] = (long double)atof(chaine);
            value++;
        }
 
        fclose(fichier);
	}
}

int only_w(long double input[]){
    int res = 1;
    for(int i = 0; i < 784; i++){
        if(input[i] == 1)
            res = 0;
    }
    return res;
}

int only_b(long double input[]){
    int res = 1;
    for(int i = 0; i < 784; i++){
        if(input[i] == 0){
            res = 0;
        }
    }
    return res;
}

char ocr(SDL_Surface* image_surface)
{
	
    long double input_layer[784];
    long double hidden_layer0[26];
    long double output_layer0[5];
    
    //image sous forme de liste contenu dans input_layer

    long double hidden_layer_weights0[784*26];
    long double output_layer_weights0[26*5];
    
    ImportArray("poids/output_layer_weights_choix_groupe.txt", output_layer_weights0);
    ImportArray("poids/hidden_layer_weights_choix_groupe.txt", hidden_layer_weights0);



	image_to_array(image_surface, input_layer);
	char lettre = ' ';
	
	if(only_w(input_layer) == 1)
	{
		lettre = ' ';
	}
	else if(only_b(input_layer) == 1)
	{
		lettre = '\n';
	}
	else
	{
		hidden_layer_calcul(hidden_layer0, input_layer, hidden_layer_weights0, 26);
		output_layer_calcul(output_layer0, hidden_layer0, output_layer_weights0, 26);
		
		long double max = 0;
		int groupe = 0;
		for (int i = 0; i < 5; i++){
			if(output_layer0[i] > max){
				max = output_layer0[i];
				groupe = i+1;
			}
		}
		
		printf("[%Lf, %Lf, %Lf, %Lf, %Lf]", output_layer0[0], output_layer0[1], output_layer0[2], output_layer0[3], output_layer0[4]);
		
		long double hidden_layer[5];
		long double output_layer[5];
		
		//image sous forme de liste contenu dans input_layer

		long double hidden_layer_weights[784*5];
		long double output_layer_weights[5*5];
		
		if(groupe == 1){
			//besoin des poids hidden_layer_groupe1 dans hidden_layer_weights
			//besoin des poids output_layer_groupe1 dans hidden_layer_weights

			ImportArray("/home/epita/Desktop/Projet/DEEP_LEARNING/poids/output_layer_groupe1.txt", output_layer_weights);
			ImportArray("/home/epita/Desktop/Projet/DEEP_LEARNING/poids/hidden_layer_groupe1.txt", hidden_layer_weights);

			hidden_layer_calcul(hidden_layer, input_layer, hidden_layer_weights,5);
			output_layer_calcul(output_layer, hidden_layer, output_layer_weights,5);

			// [a, c, e, i, o]
			max = 0;
			for (int i = 0; i < 5; i++){
				if(max < output_layer[i]){
					max = output_layer[i];
					if(i == 0)
						lettre = 'a';
					else if(i == 1)
						lettre = 'c';
					else if(i == 2)
						lettre = 'e';
					else if(i == 3)
						lettre = 'i';
					else
						lettre = 'o';
				}
			}
		}
		
		else if(groupe == 2){
			//besoin des poids hidden_layer_groupe2 dans hidden_layer_weights
			//besoin des poids output_layer_groupe2 dans hidden_layer_weights

			ImportArray("/home/epita/Desktop/Projet/DEEP_LEARNING/poids/output_layer_groupe2.txt", output_layer_weights);
			ImportArray("/home/epita/Desktop/Projet/DEEP_LEARNING/poids/hidden_layer_groupe2.txt", hidden_layer_weights);

			hidden_layer_calcul(hidden_layer, input_layer, hidden_layer_weights,5);
			output_layer_calcul(output_layer, hidden_layer, output_layer_weights,5);

			//[b, d, h, k, l]

			max = 0;
			for (int i = 0; i < 5; i++){
				if(max < output_layer[i]){
					max = output_layer[i];
					if(i == 0)
						lettre = 'b';
					else if(i == 1)
						lettre = 'd';
					else if(i == 2)
						lettre = 'h';
					else if(i == 3)
						lettre = 'k';
					else
						lettre = 'l';
				}
			}
			printf("[%Lf, %Lf, %Lf, %Lf, %Lf]", output_layer[0], output_layer[1], output_layer[2], output_layer[3], output_layer[4]);
		}
		
		else if(groupe == 4){
			//besoin des poids hidden_layer_groupe4 dans hidden_layer_weights
			//besoin des poids output_layer_groupe4 dans hidden_layer_weights

			ImportArray("/home/epita/Desktop/Projet/DEEP_LEARNING/poids/output_layer_groupe4.txt", output_layer_weights);
			ImportArray("/home/epita/Desktop/Projet/DEEP_LEARNING/poids/hidden_layer_groupe4.txt", hidden_layer_weights);

			hidden_layer_calcul(hidden_layer, input_layer, hidden_layer_weights,5);
			output_layer_calcul(output_layer, hidden_layer, output_layer_weights,5);

			//[u, v, w, x, z]

			max = 0;
			for (int i = 0; i < 5; i++){
				if(max < output_layer[i]){
					max = output_layer[i];
					if(i == 0)
						lettre = 'u';
					else if(i == 1)
						lettre = 'v';
					else if(i == 2)
						lettre = 'w';
					else if(i == 3)
						lettre = 'x';
					else
						lettre = 'z';
				}
			}
		}
		
		else{
			//besoin des poids hidden_layer_groupe5 dans hidden_layer_weights
			//besoin des poids output_layer_groupe5 dans hidden_layer_weights
			
			ImportArray("/home/epita/Desktop/Projet/DEEP_LEARNING/poids/output_layer_groupe5.txt", output_layer_weights);
			ImportArray("/home/epita/Desktop/Projet/DEEP_LEARNING/poids/hidden_layer_groupe5.txt", hidden_layer_weights);

			hidden_layer_calcul(hidden_layer, input_layer, hidden_layer_weights,5);
			output_layer_calcul(output_layer, hidden_layer, output_layer_weights,5);

			//[m, n, r, s, t]

			max = 0;
			for (int i = 0; i < 5; i++){
				if(max < output_layer[i]){
					max = output_layer[i];
					if(i == 0)
						lettre = 'm';
					else if(i == 1)
						lettre = 'n';
					else if(i == 2)
						lettre = 'r';
					else if(i == 3)
						lettre = 's';
					else
						lettre = 't';
				}
			}
		}
	}
    
    return lettre;
}
