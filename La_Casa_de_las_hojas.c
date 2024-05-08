#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <semaphore.h>

#define rows 10
#define columns 120
#define words 12
#define words_in_crucigram 6
#define max_word_size 8
#define vertical_words 3
#define horizontal_words 3

sem_t semaphore_id;

char matrix[rows][columns];

char all_words[words][max_word_size] = {{'A','N','A','K','I','N',' ',' '}, //VERTICAL
                                        {'B','A','T','M','A','N',' ',' '}, //HORIZONTAL
                                        {'V','E','N','U','S',' ',' ',' '}, //VERTICAL
                                        {'E','I','N','S','T','E','I','N'}, //HORIZONTAL
                                        {'M','A','D','R','I','D',' ',' '}, //VERTICAL
                                        {'P','I','A','N','O',' ',' ',' '}, //HORIZONTAL
                                        {'A','G','U','A',' ',' ',' ',' '}, //VERTICAL - 2
                                        {'L','A','N','A',' ',' ',' ',' '}, //HORIZONTAL - 2
                                        {'P','E','R','R','O',' ',' ',' '}, //VERTICAL - 2
                                        {'E','L','F','O',' ',' ',' ',' '}, //HORIZONTAL - 2
                                        {'M','A','Q','U','I','N','A',' '}, //VERTICAL - 2
                                        {'F','I','S','I','C','A',' ',' '} //HORIZONTAL - 2
                                      }; 
int revealedvertical[3] = {0,0,0};
int revealedhorizontal[3] = {0,0,0};
int current_v_words[3];
int current_h_words[3];

int question;
char guess[max_word_size];

int won = 0;
int lost = 0;
int correct = 1;
int timeout = 0;
int first_horizontal_word_row = 0;
int first_horizontal_word_column = 29;
int second_horizontal_word_row = 1;
int second_horizontal_word_column = 60;
int third_horizontal_word_row = 4;
int third_horizontal_word_column = 89;


void print_matrix()
{
    printf("\n");
    for(int i = 0;i < rows; i++)
    {
        for(int j = 0; j < columns;j++)
        {
            printf("%c",matrix[i][j]);
        }
        printf("\n");
    }

}

void fill_empty_matrix()
{
    for(int i = 0;i < rows; i++)
    {
        for(int j = 0; j < columns;j++)
        {
            matrix[i][j] = '-';
        }
    }
}

void reveal_word(int VorH, int which_word, int index)
{
    int matrix_index = (which_word*(columns/4)) + (columns/4);
    switch(VorH)
    {
        case 0:
            for(int i = 0; i < columns; i++)
            {
                if(all_words[index][i] == ' ') break;
                matrix[i][matrix_index] = all_words[index][i];
            }
            break;
        case 1:
            if(current_h_words[which_word] == 1)
            {
                for(int i = 0; i < max_word_size; i++)
                {
                    if(all_words[index][i] == ' ') break;
                    matrix[first_horizontal_word_row][first_horizontal_word_column+i] = all_words[index][i];
                }
            }
            else if(current_h_words[which_word] == 7)
            {
                for(int i = 0; i < max_word_size; i++)
                {
                    if(all_words[index][i] == ' ') break;
                    matrix[first_horizontal_word_row][first_horizontal_word_column+i] = all_words[index][i];
                }
            }else if(current_h_words[which_word] == 3)
            {
                for(int i = 0; i < max_word_size; i++)
                {
                    if(all_words[index][i] == ' ') break;
                    matrix[second_horizontal_word_row][second_horizontal_word_column+i] = all_words[index][i];
                }
            }else if(current_h_words[which_word] == 9)
            {
                for(int i = 0; i < max_word_size; i++)
                {
                    if(all_words[index][i] == ' ') break;
                    matrix[second_horizontal_word_row][second_horizontal_word_column+i] = all_words[index][i];
                }
            }
            else if(current_h_words[which_word] == 5)
            {
                for(int i = 0; i < max_word_size; i++)
                {
                    if(all_words[index][i] == ' ') break;
                    matrix[third_horizontal_word_row][third_horizontal_word_column+i] = all_words[index][i];
                }
            }else if(current_h_words[which_word] == 11)
            {
                for(int i = 0; i < max_word_size; i++)
                {
                    if(all_words[index][i] == ' ') break;
                    matrix[third_horizontal_word_row][third_horizontal_word_column+i] = all_words[index][i];
                }
            }
            
            break;
    }
}

void change_word(int VorH, int which_word, int index)
{
    int matrix_index = (which_word*(columns/4)) + (columns/4);
    int new_index = index + (6);
    switch(VorH)
    {
        case 0:
            for(int i = 0; i < max_word_size; i++)
            {
                if(all_words[new_index][i] == ' ') matrix[i][matrix_index] = '-';
                else matrix[i][matrix_index] = '*';
            }
            current_v_words[which_word] = new_index;
            break;
        case 1:
            if(current_h_words[which_word] == 1)
            {
                for(int i = 0; i < max_word_size; i++)
                {
                    if(all_words[new_index][i] == ' ') matrix[first_horizontal_word_row][first_horizontal_word_column+i] = '-';
                    else matrix[first_horizontal_word_row][first_horizontal_word_column+i] = '*';
                }
                current_h_words[which_word] = 7;
            }else if(current_h_words[which_word] == 3)
            {
                for(int i = 0; i < max_word_size; i++)
                {
                    if(all_words[new_index][i] == ' ') matrix[second_horizontal_word_row][second_horizontal_word_column+i] = '-';
                    else matrix[second_horizontal_word_row][second_horizontal_word_column+i] = '*';
                }
                current_h_words[which_word] = 9;
            }else if(current_h_words[which_word] == 5)
            {
                for(int i = 0; i < max_word_size; i++)
                {
                    if(all_words[new_index][i] == ' ') matrix[third_horizontal_word_row][third_horizontal_word_column+i] = '-';
                    else matrix[third_horizontal_word_row][third_horizontal_word_column+i] = '*';
                }
                current_h_words[which_word] = 11;
            }
            break;
    }
}

void* put_v_words_in_matrix(void* args)
{
    int k = 0;
    int z = 0;
    for(int i = columns/4; i < columns; i = i + columns/4)
    {
        for(int j = 0;j < rows;j++)
        {
            if(all_words[k][j] == ' ') break;
            sem_wait(&semaphore_id);
            matrix[j][i] = '*';
            current_v_words[z] = k;
            sem_post(&semaphore_id);
        }
        z++;
        k += 2;    
    }   
    pthread_exit(NULL);
}

void* put_h_words_in_matrix(void* args)
{
    for(int i = 0; i < max_word_size; i++)
    {
        if(all_words[1][i] == ' ') break;
        sem_wait(&semaphore_id);
        matrix[first_horizontal_word_row][first_horizontal_word_column + i] = '*';
        sem_post(&semaphore_id);
    }
        current_h_words[0] = 1;
    for(int i = 0; i < max_word_size; i++)
    {
        if(all_words[3][i] == ' ') break;
        sem_wait(&semaphore_id);
        matrix[second_horizontal_word_row][second_horizontal_word_column + i] = '*';
        sem_post(&semaphore_id);
    }
        current_h_words[1] = 3;
    for(int i = 0; i < max_word_size; i++)
    {
        if(all_words[5][i] == ' ') break;
        sem_wait(&semaphore_id);
        matrix[third_horizontal_word_row][third_horizontal_word_column + i] = '*';
        sem_post(&semaphore_id);
    }
    current_h_words[2] = 5;
    pthread_exit(NULL);
}

void* check_v_word(void* args)
{
    int index = current_v_words[question - 1];

    for(int i = 0; i<max_word_size;i++)
    {
        if(all_words[index][i] == ' ') break;
        if(guess[i] != all_words[index][i]) 
        {
            correct = 0;
            break;
        }
    }
    if(correct == 1) 
    {
        printf("\n correcto!");
        sem_wait(&semaphore_id);
        revealedvertical[question - 1] = 1;
        sem_post(&semaphore_id);
        reveal_word(0,question - 1, index);
    }
    else 
    {
        if(index >= 6) 
        {
            printf("\n agotaste tus intentos, intenta volver a jugar");
            lost = 1;
            kill(getpid(),SIGKILL);
        }
        else
        {
            printf("\n incorrecto! cambiando palabra");
            change_word(0,question - 1, index);
        }
    }
    pthread_exit(NULL);
}

void* check_h_word(void* args)
{
    int index = current_h_words[question - 1];

    for(int i = 0; i<max_word_size;i++)
    {
        if(all_words[index][i] == ' ') break;
        if(guess[i] != all_words[index][i]) 
        {
            correct = 0;
            break;
        }
    }
    if(correct == 1) 
    {
        printf("\n correcto!");
        sem_wait(&semaphore_id);
        revealedhorizontal[question - 1] = 1;
        sem_post(&semaphore_id);
        reveal_word(1,question - 1, index);
    }
    else 
    {
        if(index >= 6) 
        {
            printf("\n agotaste tus intentos, intenta volver a jugar");
            lost = 1;
            kill(getpid(),SIGKILL);
        }
        else
        {
            printf("\n incorrecto! cambiando palabra");
            change_word(1,question - 1, index);
        }
    }
    pthread_exit(NULL);
}

void print_questions()
{
    printf("RESPONDE EN MAYUSCULAS\n");
    printf("PALABRAS VERTICALES (0)\n");
    for(int i = 0; i < vertical_words; i++)
    {
        switch(current_v_words[i])
        {
            case 0:
                printf("%d. Primer nombre de la verdadera identidad de Darth Vader",i+1);
                break;
            case 2:
                printf("%d. Segundo planeta del sitema solar màs cercano al sol",i+1);
                break;
            case 4:
                printf("%d. Capital de España",i+1);
                break;
            case 6:
                printf("%d  Liquido vital para los seres vivos en la tierra", i+1);
                break;
            case 8:
                printf("%d  Mascota que desciende del lobo", i+1);
                break;
            case 10:
                printf("%d  Artificio para aprovechar, dirigir o regular la acción de una fuerza.", i+1);
                break;
        }
        printf("\n");
    }
    printf("PALABRAS HORIZONTALES (1)\n");
    for(int i = 0; i < horizontal_words; i++)
    {
        switch(current_h_words[i])
        {
            case 1:
                printf("%d. Superheroe millonario que tiene de enemigo a un bufòn.",i+1);
                break;
            case 3:
                printf("%d. Apellido del cientifico que creo la teorìa de la relatividad.",i+1);
                break;
            case 5:
                printf("%d. Instrumento màs famoso de cuerda percutida",i+1);
                break;
            case 7:
                printf("%d. Se extrae de las ovejas",i+1);
                break;
            case 9:
                printf("%d. Especie de Legolas",i+1);
                break;
            case 11:
                printf("%d. Ciencia exacta que estudia la materia y el movimiento",i+1);
                break;
        }
        printf("\n");
    }

}

void check_win()
{
    printf("\n");
    int tempwon = 1;
    for(int i =0; i < 3; i++)
    {
        if(revealedhorizontal[i] == 0) 
        {
            tempwon = 0;
        }
        if(revealedvertical[i] == 0) 
        {
            tempwon = 0;
        }
    }
    if(tempwon == 1) 
    {
        printf("\nFelicidades Ganaste!\n");
        won = tempwon;
        kill(getpid(),SIGKILL);
    }
}

void alarm_handler(int signum)
{
    timeout = 1;
}

void caps()
{   
    for(int i = 0; i<10; i++)
    {
        guess[i] = toupper(guess[i]);
    }
}

int main()
{
    pid_t pid_B, pid_C;
    int status, VoH;
    fill_empty_matrix();
    pid_B = fork();
    if(pid_B == 0)
    {
        pthread_t thread_C, thread_D, thread_A, thread_B;
        sem_init(&semaphore_id,0,1);
        pthread_create( &thread_C, NULL, put_v_words_in_matrix, NULL );
        pthread_create( &thread_A, NULL, put_h_words_in_matrix, NULL );
        pthread_join( thread_C, NULL);
        pthread_join( thread_A, NULL);
        while(won == 0 && lost == 0)
        {
            system("clear");
            check_win();
            timeout = 0;
            correct = 1;
            print_matrix();
            print_questions();
            printf("¿Palabra vertical (0) u horizontal (1)?: ");
            scanf("%d", &VoH);
            printf("Escoja que palabra: ");
            scanf("%d", &question);
            if(question >= 1 && question <= 3)
            {
            if(VoH == 0)
            {
                if(revealedvertical[question-1] == 0)
                {
                    printf("Tienes 10 segundos, si ya te gastaste tu intento tendras que volver a jugar\n");
                    alarm(10);
                    signal(SIGALRM,alarm_handler);
                    printf("Escriba su suposición:\n");
                    scanf("%s", guess);
                    if (timeout == 0) 
                    {
                        //caps(guess);
                        alarm(0); 
                        pthread_create(&thread_D, NULL, check_v_word, NULL);
                        pthread_join(thread_D, NULL);
                    }
                    if (timeout == 1) 
                    {
                        int index = current_v_words[question - 1];
                        if(index >= 6)
                        {
                            printf("\nagotaste tus intentos, intenta volver a jugar");
                            lost = 1;
                            return 0;
                        }
                        else
                        {
                            printf("Tardaste mucho, cambiando palabra\n");
                            change_word(0,question - 1, index);
                        } 
                    }
                }else
                {
                    printf("La palabra ya fue resuelta");
                }
            }else if(VoH == 1)
            {
                if(revealedhorizontal[question-1] == 0)
                {
                    printf("Tienes 10 segundos, si ya te gastaste tu intento tendras que volver a jugar\n");
                    alarm(10);
                    signal(SIGALRM,alarm_handler);
                    printf("Escriba su suposición:\n");
                    scanf("%s", guess);
                    if (timeout == 0) 
                    {
                        //caps(guess);
                        alarm(0); 
                        pthread_create(&thread_B, NULL, check_h_word, NULL);
                        pthread_join(thread_B, NULL);
                    }
                    if (timeout == 1) 
                    {
                        int index = current_h_words[question - 1];
                        if(index >= 6)
                        {
                            printf("\nagotaste tus intentos, intenta volver a jugar");
                            lost = 1;
                            return 0;
                        }
                        else
                        {
                            printf("Tardaste mucho, cambiando palabra\n");
                            change_word(1,question - 1, index);
                        } 
                    }
                }else
                {
                    printf("La palabra ya fue resuelta");
                }
            }
            else
            {
                printf("ingresa un numero valido");
            }
        }else
            {
                printf("ingresa un numero valido");
            }
        }
        return 0;   
    }
    waitpid(pid_B, &status,0); 

    sem_destroy(&semaphore_id);
    return 0;

}