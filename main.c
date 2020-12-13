#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mpi.h"

#define ILOSC_ITERACJI_DOM 64
#define WIELKOSC 256
#define BOK 16 // dlugosc boku siatki

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

int main(int argc, char **argv)
{

    int siatka[256] =
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


    int Ilosc_proc;
    int ID, j;
    int iteracja = 0;
    int ilosc_iteracji;

    // przypisanie danych z linii polecen
    if (argc == 1)
    {
        ilosc_iteracji = ILOSC_ITERACJI_DOM;
    }
    else if (argc == 2)
    {
        ilosc_iteracji = atoi(argv[1]);
    }
    else
    {
        printf("Blad");
        exit(1);
    }

    MPI_Status stat;

    if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
    {
        printf("MPI_Init error\n");
    }

    MPI_Comm_size(MPI_COMM_WORLD, &Ilosc_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &ID);

    assert(BOK % Ilosc_proc == 0);

    // tablica pomocnicza
    int *tab = (int *)malloc(BOK * ((BOK / Ilosc_proc) + 2) * sizeof(int));
    for (iteracja = 0; iteracja < ilosc_iteracji; iteracja++)
    {
        //printf("%d %d\n",ID, BOK * ((BOK / Ilosc_proc) + 2));
        j = BOK;
        for (int i = ID * (WIELKOSC / Ilosc_proc); i < (ID + 1) * (WIELKOSC / Ilosc_proc); i++)
        {
            tab[j] = siatka[i];
            // if(ID==1)
            //     printf(" %d %d \n",j,i);
            j++;
        }

        if (Ilosc_proc != 1)
        {
            int pobrane_1[BOK];
            int pobrane_2[BOK];
            int wyslij_1[BOK];
            int wyslij_2[BOK];
            if (ID % 2 == 0)
            {

                //pierwszy wiersz
                for (int i = 0; i < BOK; i++)
                {
                    wyslij_1[i] = tab[i + BOK];
                    // printf(" - %d - ",wyslij_1[i]);
                    //printf(" %d %d\n ",i,i+BOK);
                }
                //srodkowe wiersze
                MPI_Ssend(&wyslij_1, BOK, MPI_INT, mod(ID - 1, Ilosc_proc), 1, MPI_COMM_WORLD);

                //ostatni wiersz
                for (int i = 0; i < BOK; i++)
                {
                    wyslij_2[i] = tab[(BOK * (BOK / Ilosc_proc)) + i];
                    // printf(" %d %d\n ",i,(BOK * (BOK / Ilosc_proc)) + i);
                }
                //last row to ID+1
                MPI_Ssend(&wyslij_2, BOK, MPI_INT, mod(ID + 1, Ilosc_proc), 1, MPI_COMM_WORLD);
            }
            else
            {
                MPI_Recv(&pobrane_2, BOK, MPI_INT, mod(ID + 1, Ilosc_proc), 1, MPI_COMM_WORLD, &stat);
                MPI_Recv(&pobrane_1, BOK, MPI_INT, mod(ID - 1, Ilosc_proc), 1, MPI_COMM_WORLD, &stat);
            }
            if (ID % 2 == 0)
            {
                MPI_Recv(&pobrane_2, BOK, MPI_INT, mod(ID + 1, Ilosc_proc), 1, MPI_COMM_WORLD, &stat);
                MPI_Recv(&pobrane_1, BOK, MPI_INT, mod(ID - 1, Ilosc_proc), 1, MPI_COMM_WORLD, &stat);
            }
            else
            {
                //pierwszy wiersz
                for (int i = 0; i < BOK; i++)
                {
                    wyslij_1[i] = tab[i + BOK];
                }
                MPI_Ssend(&wyslij_1, BOK, MPI_INT, mod(ID - 1, Ilosc_proc), 1, MPI_COMM_WORLD);

                //ostatni wiersz
                for (int i = 0; i < BOK; i++)
                {
                    wyslij_2[i] = tab[(BOK * (BOK / Ilosc_proc)) + i];
                }
                MPI_Ssend(&wyslij_2, BOK, MPI_INT, mod(ID + 1, Ilosc_proc), 1, MPI_COMM_WORLD);
            }
            for (int i = 0; i < BOK; i++)
            {
                tab[i] = pobrane_1[i];
                tab[(BOK * ((BOK / Ilosc_proc) + 1)) + i] = pobrane_2[i];
            }
        }
        else
        {
            for (int i = 0; i < BOK; i++)
            {
                tab[i + WIELKOSC + BOK] = siatka[i];
                //printf(" %d %d \n",i + WIELKOSC+BOK,i);
            }
            for (int i = WIELKOSC; i < WIELKOSC + BOK; i++)
            {
                tab[i - WIELKOSC] = siatka[i - BOK];
                //printf(" %d %d \n",i - WIELKOSC,i-BOK);
            }
        }
        //sprawdzanie sasiadow
        int * final = (int *)malloc(BOK * ((BOK / Ilosc_proc)) * sizeof(int));

        for (int k = BOK; k < BOK * ((BOK / Ilosc_proc) + 1); k++)
        {
            int wiersze = BOK * (BOK / Ilosc_proc) + 2;
            int r = k / BOK;
            int c = k % BOK;
            int pop_wier = mod(r - 1, wiersze);
            int pop_kom = mod(c - 1, BOK);
            int nast_wier = mod(r + 1, wiersze);
            int nast_kom = mod(c + 1, BOK);

            int sasiedzi = tab[pop_wier * BOK + pop_kom] + tab[pop_wier * BOK + c] + tab[pop_wier * BOK + nast_kom] + tab[r * BOK + pop_kom] + tab[r * BOK + nast_kom] + tab[nast_wier * BOK + pop_kom] + tab[nast_wier * BOK + c] + tab[nast_wier * BOK + nast_kom];
            if (tab[k] == 1)
            {
                if (sasiedzi < 2)
                    final[k - BOK] = 0;
                else if (sasiedzi > 3)
                    final[k - BOK] = 0;
                else
                    final[k - BOK] = 1;
            }
            else
            {
                if (sasiedzi == 3)
                    final[k - BOK] = 1;
                else
                    final[k - BOK] = 0;
            }
        }

        j = 0;
        for (int i = ID * (WIELKOSC / Ilosc_proc); i < (ID + 1) * (WIELKOSC / Ilosc_proc); i++)
        {
            siatka[i] = final[j];
            j++;
        }
        MPI_Gather(final, BOK * (BOK / Ilosc_proc), MPI_INT, &siatka, BOK * (BOK / Ilosc_proc), MPI_INT, 0, MPI_COMM_WORLD);

        // Wypisanie odswiezonej tablicy
        if (ID == 0)
        {
            printf("\nIteracja nr %d:\n", iteracja);
            for (j = 0; j < WIELKOSC; j++)
            {
                if (j % BOK == 0)
                {
                    printf("\n");
                }
                printf("%d  ", siatka[j]);
            }
            printf("\n");
        }
    }

    free(tab);
    MPI_Finalize();
}
