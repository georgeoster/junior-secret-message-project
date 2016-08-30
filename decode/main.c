#include <stdio.h>

#define EXIT_SUCCESS 0            //all good
#define EXIT_USAGE_ERROR 1        //wrong number of inputs
#define EXIT_FILE_OPEN_FAILURE 2  //file doesn.t exist

void usage(char*);
int commentOrNot(char);

int main(int argc, char **argv) {
    //argv[0] is filename
    //argv[1] is ppm which will contains the encoded message
    
    FILE *in;
    in=fopen(argv[1],"rb");
    
    if (argc != 2) {
        usage(argv[0]);
        return EXIT_USAGE_ERROR;
    }
    
    
    if(in == NULL)
    {
        printf("Can not open the file.\n");
        return EXIT_FILE_OPEN_FAILURE;
    }
    
    //begin check if file is a P6
    int k = fgetc(in);
    int kcount = 0;
    if ( k == 'P')
        kcount++;
    k = fgetc(in);
    if ( k == '6')
        kcount++;
    if (kcount != 2){
        printf("file format is invalid.\n");
        return EXIT_FILE_OPEN_FAILURE;
    }
    while ( k != '\n') {
        k = fgetc(in);
    }
    //end check if file is a P6
    
    //begin check if this line is a comment
    k = fgetc(in);
    if (commentOrNot(k) == 1){
        while ( k != '\n'){
            k = fgetc(in);
        }
    }
    //end check if this line is a comment
    k = fgetc(in);
    //begin width height and maxColor
    while (k != '\n'){
        k = fgetc(in);
    }
    k = fgetc(in);
    while (k != '\n'){
        k = fgetc(in);
    }
    //end width height and maxColor
    
    printf("the secret message is: ");
    
    int count;
    char toprint = 0;
    int escape = 0;
    
    
    while(escape != 1){//for (count=0; count < 14; count++) {
        
        int i;
        for (i=0; i < 8; i++) {
            k = fgetc(in);
            if ((k & 1) == 0) {
                //k is even
            }
            else{
                toprint = toprint ^ (1 << (7-i));
            }
            
        }
        if (toprint == '\0')
            escape++;//printf("got a slash0 over here");
        printf("%c",toprint);
        toprint = 0;
    }
    printf("\n");
    
    return 0;
}


void usage(char *pn) {
    printf("usage: %s input_file\n", pn);
}

int commentOrNot(char k){
    int toreturn = 0;
    if (k == '#'){
        toreturn++;
    }
    return toreturn;
}

