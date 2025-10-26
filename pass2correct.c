#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char label[10], opcode[10], operand[10], code[10], mnemonic[10], objcode[100];
    int locctr = 0, start = 0, operand_address;
    FILE *input = fopen("inter.txt","r");
    FILE *optab = fopen("optab.txt","r");
    FILE *symtab = fopen("symtab.txt","r");
    FILE *output = fopen("output_pass2.txt","w");
    FILE *objprog = fopen("object_program.txt","w");

    fscanf(input,"%s%s%s", label, opcode, operand);
    if (!strcmp(opcode,"START")) {
        start = locctr = (int)strtol(operand,NULL,16);
        fprintf(output,"\t%s\t%s\t%s\n", label, opcode, operand);
        fprintf(objprog,"H^%s^%06X^000000\n", label, start);
        fscanf(input,"%s%s%s", label, opcode, operand);
    }

    char text[1000]=""; int text_len=0, text_start=locctr;

    while (strcmp(opcode,"END")) {
        memset(objcode,0,sizeof(objcode));
        rewind(optab); int found=0;
        while (fscanf(optab,"%s%s",code,mnemonic)!=EOF) {
            if (!strcmp(opcode,code)) {
                found=1; operand_address=0; rewind(symtab);
                if (strcmp(operand,"-")) {
                    char sym[10], addr[10];
                    while (fscanf(symtab,"%s%s",sym,addr)!=EOF)
                        if (!strcmp(sym,operand)) {
                          operand_address=(int)strtol(addr,NULL,16); break;}
                }
                sprintf(objcode,"%s%04X",mnemonic,operand_address); break;
            }
        }
        if (!found) {
            if (!strcmp(opcode,"WORD"))
              sprintf(objcode,"%06X",atoi(operand));
            else if (!strcmp(opcode,"BYTE")) {
                if (operand[0]=='C') 
                  for(int i=2;i<strlen(operand)-1;i++) 
                    sprintf(objcode+strlen(objcode),"%02X",operand[i]);
                else if (operand[0]=='X') strncpy(objcode,operand+2,strlen(operand)-3), objcode[strlen(operand)-3]='\0';
            }
        }

        int objlen=strlen(objcode)/2;
        if (text_len+objlen>30) { fprintf(objprog,"T^%06X^%02X%s\n",text_start,text_len,text); 
                                 strcpy(text,""); 
                                 text_len=0; text_start=locctr;}
        if (strlen(objcode)) { strcat(text,"^"); 
                              strcat(text,objcode);
                              text_len+=objlen; }

        fprintf(output,"%s\t%s\t%s\t%s\n",label,opcode,operand,objcode);

        if (!strcmp(opcode,"WORD")) locctr+=3;
        else if (!strcmp(opcode,"RESW")) locctr+=3*atoi(operand);
        else if (!strcmp(opcode,"RESB")) locctr+=atoi(operand);
        else if (!strcmp(opcode,"BYTE")) locctr+=(operand[0]=='C')?strlen(operand)-3:(strlen(operand)-3)/2;
        else locctr+=3;

        if (fscanf(input,"%s%s%s",label,opcode,operand)==EOF) break;
    }

    if (text_len) fprintf(objprog,"T^%06X^%02X%s\n",text_start,text_len,text);
    fprintf(objprog,"E^%06X\n",start);
    fprintf(output,"%s\t%s\t%s\n",label,opcode,operand);

    fclose(input); fclose(optab); fclose(symtab); fclose(output); fclose(objprog);
    return 0;
}
