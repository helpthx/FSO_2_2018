	//gcc nome_arquivo.c -o sudoku -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>

/* Alunos/Matricula:  Brian Lui                 15/0006802
                      Joao Vitor Rodrigues      15/0013329
                      Lucas Machado Martins     15/0015917
*/

char matrix[9][9];
int verf_linha[9] = {0};
int verf_coluna[9] = {0};
int verf_grid[9] = {0};


void *verf_line(void *param);
void *verf_colu(void *param);
void *verf_grid_0(void *param);
void *verf_grid_1(void *param);
void *verf_grid_2(void *param);
void *verf_grid_3(void *param);
void *verf_grid_4(void *param);
void *verf_grid_5(void *param);
void *verf_grid_6(void *param);
void *verf_grid_7(void *param);
void *verf_grid_8(void *param);

int main(int argc, char *argv[])
{

    int a = 0, i = 0, j = 0, k = 0;
    int s_l = 0, s_c = 0, s_g = 0;
    char texto[81] = {0}; //caracteres unicos.
    char fileName[50];

    FILE *file;
    file = fopen(argv[1], "r");

    if (file == NULL)
    {
        printf("Arquivo nao pode ser aberto\n");
        return 0;
    }

    a=0;
    char ch;
    while( (ch=fgetc(file))!= EOF ){
     if(ch != ' '){ // Tirando os espaços entres os numeros.
            if (ch != '\r' && ch != '\n') // Tirando as quebras de linha
            {
            texto[a]=ch; //Aqui cada caractere é colocado no array
            a++;}
         }
     }
    texto[a]='\0';

    fclose(file);

    //Transformar o arquivo em uma matrix vista por todas as threads
    a = 0;
    for (i = 0; i < 9; i++) //linha
    {
        for (j = 0; j < 9; j++) //Coluna
        {
            matrix[i][j] = texto[a];
            a++;
        }
    }

    //Printar a matrix
    /*printf("Matrix de entrada\n");
    for (i = 0; i < 9; i++) //linha
    {
        for (j = 0; j < 9; j++) //Coluna
        {
            printf("%c" , matrix[i][j]);

        }
        printf("\n");
    }
    printf("\n");*/

    //Iniciar as threads de varredura
    pthread_t tid[12];

    pthread_create(&tid[2],NULL,verf_grid_8,NULL);

    pthread_create(&tid[3],NULL,verf_grid_5,NULL);

    pthread_create(&tid[4],NULL,verf_grid_3,NULL);

    pthread_create(&tid[5],NULL,verf_grid_0,NULL);

    pthread_create(&tid[6],NULL,verf_grid_4,NULL);

    pthread_create(&tid[7],NULL,verf_grid_6,NULL);

    pthread_create(&tid[8],NULL,verf_grid_7,NULL);

    pthread_create(&tid[9],NULL,verf_grid_1,NULL);

    pthread_create(&tid[10],NULL,verf_grid_2,NULL);
    sleep(1);

    pthread_create(&tid[0],NULL,verf_line,NULL);
    sleep(1);

    pthread_create(&tid[1],NULL,verf_colu,NULL);
    //sleep(1);


    //Terminar as threads
    for (i = 0; i < 12; i++)
    {
    pthread_join(tid[i],NULL);
    wait(NULL);

    }
    printf("Retornando a thread pai\n");
   // sleep(1);




    //Printar os resultados
    //printf("linhas: ");
    for (i = 0; i < 9; i++)
    {
        //printf("%d", verf_linha[i]);
        s_l = s_l + verf_linha[i];
    }

    //printf("\nColunas: ");
    for (j = 0; j < 9; j++)
    {
       // printf("%d", verf_coluna[j]);
        s_c = s_c + verf_coluna[j];
    }

    //printf("\nGrids: ");
    for (k = 0; k < 9; k++)
    {
      //  printf("%d", verf_grid[k]);
        s_g = s_g + verf_grid[k];
    }

    printf("\n");

    if (s_l == 9 && s_c == 9 && s_g == 9)
    {
        printf("===============\n");
        printf("Grid válido !!!\n");
        printf("===============\n");
    }
    else{
        printf("=================\n");
        printf("Grid inválido !!!\n");
        printf("=================\n");
    }






    return 0;
    }


//Verificação das linhas

void *verf_line(void *param){
//printf("To aqui na linha \n");
    int i, j;
    int linha[9] = {0};
    for (j = 0; j < 9; j++) //Linha
    {
        for (i = 0; i < 9; i++)
        {
       linha[j] = linha[j] + matrix[j][i];

       }
        //Verificação
        if( linha[j] == 477){
            verf_linha[j] = 1;
        }
        else{
            verf_linha[j] = 0;
        }
    }
 //sleep(1);
}
//Verificação das colunas
void *verf_colu(void *param){
//printf("To aqui na coluna \n");
    int i, j;
    int coluna[9] = {0};
    for (i = 0; i < 9; i++) //coluna
    {
        for (j = 0; j < 9; j++)
        {
       coluna[i] = coluna[i] + matrix[j][i];

       }
        //Verificação
        if(coluna[i] == 477){
            verf_coluna[i] = 1;
        }
        else{
            verf_coluna[i] = 0;
        }
    }
//sleep(1);
}
//Verificação do grid 0
void *verf_grid_0(void *param){
//printf("To aqui grid 0\n");
    int i, j;
    int grid=0;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
          {
             grid = grid + matrix[i][j];
          }
    }
    if(grid == 477){
        verf_grid[0] = 1;
    }
    else{
        verf_grid[0] = 0;
    }

}
//Verificação do grid 1
void *verf_grid_1(void *param){
//printf("To aqui grid 1\n");
    int i, j;
    int grid=0;
    for (i = 0; i < 3; i++)
    {
        for (j = 3; j < 6; j++)
          {
             grid = grid + matrix[i][j];
          }
    }
    if(grid == 477){
        verf_grid[1] = 1;
    }
    else{
        verf_grid[1] = 0;
    }

}

//Verificação do grid 2
void *verf_grid_2(void *param){

//printf("To aqui grid 2\n");
    int i, j;
    int grid=0;
    for (i = 0; i < 3; i++)
    {
        for (j = 6; j < 9; j++)
          {
             grid = grid + matrix[i][j];
          }
    }
    if(grid == 477){
        verf_grid[2] = 1;
    }
    else{
        verf_grid[2] = 0;
    }

}

//Verificação do grid 3
void *verf_grid_3(void *param){
//printf("To aqui grid 3\n");
    int i, j;
    int grid=0;
    for (i = 3; i < 6; i++)
    {
        for (j = 0; j < 3; j++)
          {
             grid = grid + matrix[i][j];
          }
    }
    if(grid == 477){
        verf_grid[3] = 1;
    }
    else{
        verf_grid[3] = 0;
    }
}

//Verificação do grid 4
void *verf_grid_4(void *param){
//printf("To aqui grid 4\n");
    int i, j;
    int grid=0;
    for (i = 3; i < 6; i++)
    {
        for (j = 3; j < 6; j++)
          {
             grid = grid + matrix[i][j];
          }
    }
    if(grid == 477){
        verf_grid[4] = 1;
    }
    else{
        verf_grid[4] = 0;
    }
}

//Verificação do grid 5
void *verf_grid_5(void *param){
//printf("To aqui grid 5\n");
    int i, j;
    int grid=0;
    for (i = 3; i < 6; i++)
    {
        for (j = 6; j < 9; j++)
          {
             grid = grid + matrix[i][j];
          }
    }
    if(grid == 477){
        verf_grid[5] = 1;
    }
    else{
        verf_grid[5] = 0;
    }
//wait(NULL);
}

//Verificação do grid 6
void *verf_grid_6(void *param){
//printf("To aqui grid 6\n");
    int i, j;
    int grid=0;
    for (i = 6; i < 9; i++)
    {
        for (j = 0; j < 3; j++)
          {
             grid = grid + matrix[i][j];
          }
    }
    if(grid == 477){
        verf_grid[6] = 1;
    }
    else{
        verf_grid[6] = 0;
    }
//wait(NULL);
}

//Verificação do grid 7
void *verf_grid_7(void *param){
//printf("To aqui grid 7\n");
    int i, j;
    int grid=0;
    for (i = 6; i < 9; i++)
    {
        for (j = 3; j < 6; j++)
          {
             grid = grid + matrix[i][j];
          }
    }
    if(grid == 477){
        verf_grid[7] = 1;
    }
    else{
        verf_grid[7] = 0;
    }

}

//Verificação do grid 8
void *verf_grid_8(void *param){
//printf("To aqui grid 8\n");
    int i, j;
    int grid=0;
    for (i = 6; i < 9; i++)
    {
        for (j = 6; j < 9; j++)
          {
             grid = grid + matrix[i][j];
          }
    }
    if(grid == 477){
        verf_grid[8] = 1;
    }
    else{
        verf_grid[8] = 0;
    }

}
