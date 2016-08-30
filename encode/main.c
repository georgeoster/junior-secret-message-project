#include <stdio.h>

#define EXIT_SUCCESS 0            //all good
#define EXIT_USAGE_ERROR 1        //wrong number of inputs
#define EXIT_FILE_OPEN_FAILURE 2  //file doesn.t exist
#define EXIT_MESSAGE_ERROR 3      //something wrong with the .txt which contains the message

void usage(char*);
int commentOrNot(char);

int main(int argc, char **argv) {
    FILE *in;
    FILE *out;
    FILE *message;
    //argv[0] is filename
    //argv[1] is .txt containing message to be encoded
    //argv[2] is .ppm which we will encode the picture in
    //argv[4] is .ppm which we will create which will contain the encoded message

    if (argc != 4) {
        usage(argv[0]);
        return EXIT_USAGE_ERROR;
    }
    
    if ((message=fopen(argv[1], "r"))==NULL) {
        printf("error opening %s containing message to be encoded.  Quitting ...\n", argv[1]);
        return EXIT_MESSAGE_ERROR;
    }
    
    if ((in=fopen(argv[2], "r"))==NULL) {
        printf("error opening %s containing picture to encode message in.  Quitting ...\n", argv[1]);
        return EXIT_FILE_OPEN_FAILURE;
    }
    
    if ((out=fopen(argv[3], "w"))==NULL) {
        printf("error opening %s which would be picture containing encoded message.  Quitting ...\n", argv[2]);
        return EXIT_FILE_OPEN_FAILURE;
    }
    
    char nextChar = getc(message);
    int howManyCharacters = 0;
    while (nextChar != EOF) {
        howManyCharacters++;
        nextChar = getc(message);
    }
    
    if (howManyCharacters > 255){
        printf("message to be encoded must be less than 255 characters.\n");
        return EXIT_MESSAGE_ERROR;
    }
    
    
    FILE *fp;
    if((fp=fopen(argv[2],"rb"))==NULL)
    {
        printf("Can not open the file.\n");
        return EXIT_FILE_OPEN_FAILURE;
    }
    
    //begin check if file is a P6
    int k = fgetc(fp);
    fputc(k,out);
    int kcount = 0;
    if ( k == 'P')
        kcount++;
    k = fgetc(fp);
    fputc(k,out);
    if ( k == '6')
        kcount++;
    if (kcount != 2){
        printf("file format is invalid.\n");
        return EXIT_MESSAGE_ERROR;
    }
    while ( k != '\n') {
        k = fgetc(fp);
        fputc(k,out);
    }
    //end check if file is a P6
    
    
    
  
    
    
    
    //begin check if this line is a comment
    
    k = fgetc(fp);
    fputc(k,out);
    if (commentOrNot(k) == 1){
        while ( k != '\n'){
            k = fgetc(fp);
            fputc(k,out);
        }
    }
    
    //end check if this line is a comment
    
    
    
    
    
    //begin width height and maxColor
    int width;
    int widtharray[3];
    int height;
    int heightarray[3];
    int maxColor;
    int maxColorarray[3];
    int counter = 0;
    
    k = fgetc(fp);
    fputc(k,out);
    while (k != ' '){
        widtharray[counter] = k - '0';
        k = fgetc(fp);
        fputc(k,out);
        counter++;
    }
    width = (widtharray[0]*100) + (widtharray[1]*10) + widtharray[2];
    counter = 0;
    k = fgetc(fp);
    fputc(k,out);
    while (k != '\n'){
        heightarray[counter] = k - '0';
        k = fgetc(fp);
        fputc(k,out);
        counter++;
    }
    height = (heightarray[0]*100) + (heightarray[1]*10) + heightarray[2];
    counter = 0;
    k = fgetc(fp);
    fputc(k,out);
    while (k != '\n'){
        maxColorarray[counter] = k - '0';
        k = fgetc(fp);
        fputc(k,out);
        counter++;
    }
    maxColor = (maxColorarray[0]*100) + (maxColorarray[1]*10) + maxColorarray[2];
    
    //end width height and maxColor
    
  //begin encoding of secret message
    
    FILE *secretmessage;
    secretmessage = fopen(argv[1], "r");
    unsigned char toencode = fgetc(secretmessage);  //toencode is a char from secret message
    unsigned char temptoencode;
   

    while ((char)toencode != EOF) {//begin WHILE loop
        
        int eightbytes[8];  //this will hold 8 bytes, the amount of bytes needed to encode one char of secretmessage
        int loadeightbytes;
        for (loadeightbytes=0; loadeightbytes<8; loadeightbytes++) {
            k=fgetc(fp);
            eightbytes[loadeightbytes] = k;
        }
        
        //SET THE LEAST SIGNIFICANT BIT OF THE 8 BYTES TO BE THE 8 BITS OF THE CHAR OF SECRETMESSAGE
        temptoencode = toencode;
        int i;
        int escape;
        for (i=0; i < 8; i++) {
            escape = 0;
            
            if ((eightbytes[i] & 00000001) == 1){//this is when the least significant digit of eightbytes[i] is a 1
        
                toencode <<= i;
                toencode >>= 7;

                if (toencode == 0){
                    toencode = (unsigned char)toencode + 254;
                }
                if (toencode == 1){
                    toencode = 255;
                }
                
                eightbytes[i] = (eightbytes[i] & toencode);
                toencode = temptoencode;
                escape++;
            }
            
            if (escape == 0){
                if ((eightbytes[i] & 00000001) == 0){//this is when the least significant digit of eightbytes[i] is a 0
                   
                    toencode <<= i;
                    toencode >>= 7;
               
                    eightbytes[i] = (eightbytes[i] ^ toencode);
                    toencode = temptoencode;
                }
            }
        }
        
        //write the altered 8 bytes to the output file
        int outcount;
        for (outcount=0; outcount < 8; outcount++) {
            k = eightbytes[outcount];
            fputc(k,out);
        }
        
     
        toencode = fgetc(secretmessage);//get the next char to be encoded
    }//end WHILE loop
  
    //at this point the message has been encoded, but the new picture in which it has been encoded has not been fully copied
    //we need to copy the rest of the picture.
    //but first we encode \0 to signify the end of the message
   
    int eightbytes[8];  //this will hold 8 bytes, the amount of bytes needed to encode one char of secretmessage
    int loadeightbytes;
    for (loadeightbytes=0; loadeightbytes<8; loadeightbytes++) {
        k=fgetc(fp);
        eightbytes[loadeightbytes] = k;
       // printf("eightbytes[%d] is %d\n",loadeightbytes, eightbytes[loadeightbytes]);
    }
    
    //SET THE LEAST SIGNIFICANT BIT OF THE 8 BYTES TO BE THE 8 BITS OF THE CHAR OF SECRETMESSAGE
    toencode = '\0';
    temptoencode = toencode;
    int i;
    int escape;
    for (i=0; i < 8; i++) {
        escape = 0;
        
        if ((eightbytes[i] & 00000001) == 1){//this is when the least significant digit of eightbytes[i] is a 1
            
            toencode <<= i;
            toencode >>= 7;
            
            if (toencode == 0){
                toencode = (unsigned char)toencode + 254;
            }
            if (toencode == 1){
                toencode = 255;
            }
            
            eightbytes[i] = (eightbytes[i] & toencode);
            toencode = temptoencode;
            escape++;
        }
        
        if (escape == 0){
            if ((eightbytes[i] & 00000001) == 0){//this is when the least significant digit of eightbytes[i] is a 0
                
                toencode <<= i;
                toencode >>= 7;
                
                eightbytes[i] = (eightbytes[i] ^ toencode);
                toencode = temptoencode;
            }
        }
    }
    
    //write the altered 8 bytes to the output file
    int outcount;
    for (outcount=0; outcount < 8; outcount++) {
        k = eightbytes[outcount];
        fputc(k,out);
    }
 
  
    
//now \0 is encoded. now we copy the rest of the picture.
    
    k = fgetc(fp);
    while (k != EOF) {
        fputc(k,out);
        k=fgetc(fp);
    }

    fclose(in);
    fclose(out);
    fclose(message);
    fclose(secretmessage);
    fclose(fp);
    return EXIT_SUCCESS;
}

void usage(char *pn) {
    printf("usage: %s secretmessage.txt input_file output_file\n", pn);
}

int commentOrNot(char k){
    int toreturn = 0;
    if (k == '#'){
        toreturn++;
    }
    return toreturn;
}

