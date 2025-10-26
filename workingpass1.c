#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char label[10], opcode[10], operand[10], code[10], mnemonic[10];
    int locctr, start = 0, length, size = 0;
    FILE *fp1, *fp2, *fp3, *fp4;

    fp1 = fopen("input.txt", "r");
    fp2 = fopen("optab.txt", "r");
    fp3 = fopen("symtab.txt", "w");
    fp4 = fopen("inter.txt", "w");

    if (fp1 == NULL || fp2 == NULL || fp3 == NULL || fp4 == NULL) {
        printf("Error opening files.\n");
        return 1;
    }

    fscanf(fp1, "%s\t%s\t%s", label, opcode, operand);

    if (strcmp(opcode, "START") == 0) {
        start = (int)strtol(operand, NULL, 16);
        locctr = start;
        fprintf(fp4, "**\t%s\t%s\t%s\n", label, opcode, operand);
        fscanf(fp1, "%s\t%s\t%s", label, opcode, operand);
    } else {
        locctr = 0;
    }

    while (strcmp(opcode, "END") != 0) {
        fprintf(fp4, "%04X\t", locctr);

        if (strcmp(label, "**") != 0 && strcmp(opcode, "RESW") != 0 && strcmp(opcode, "RESB") != 0) {
            fprintf(fp3, "%s\t%04X\n", label, locctr);
        }

        int found = 0;
        rewind(fp2);

        while (fscanf(fp2, "%s\t%s", code, mnemonic) != EOF) {
            if (strcmp(code, opcode) == 0) {
                found = 1;
                locctr += 3;
                size += 3;
                break;
            }
        }

        if (!found) {
            if (strcmp(opcode, "WORD") == 0) {
                locctr += 3;
                size += 3;
            } else if (strcmp(opcode, "RESW") == 0) {
                locctr += 3 * atoi(operand);
            } else if (strcmp(opcode, "RESB") == 0) {
                locctr += atoi(operand);
            } else if (strcmp(opcode, "BYTE") == 0) {
                if (operand[0] == 'C') {
                    locctr += strlen(operand) - 3;
                    size += strlen(operand) - 3;
                } else if (operand[0] == 'X') {
                    locctr += (strlen(operand) - 3) / 2;
                    size += (strlen(operand) - 3) / 2;
                }
            }
        }

        fprintf(fp4, "%s\t%s\t%s\n", label, opcode, operand);
        fscanf(fp1, "%s\t%s\t%s", label, opcode, operand);
    }

    fprintf(fp4, "%04X\t%s\t%s\t**", locctr, label, opcode);
    length = locctr - start;
    printf("Length of the code: %04X\n", length);

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);

    FILE *flen = fopen("length.txt", "w");
    fprintf(flen, "%02X\n%02X\n", length,size);
    fclose(flen);

    fp3 = fopen("symtab.txt", "a");
    fprintf(fp3, "gamma\t%04X\n", locctr);
    fclose(fp3);

    printf("\nContents of inter.txt:\n");
    FILE *finter = fopen("inter.txt", "r");
    char ch;
    while ((ch = fgetc(finter)) != EOF) {
        printf("%c", ch);
    }
    fclose(finter);

    printf("\n\nContents of symtab.txt:\n");
    FILE *fsymtab = fopen("symtab.txt", "r");
    while ((ch = fgetc(fsymtab)) != EOF) {
        printf("%c", ch);
    }
    fclose(fsymtab);

    return 0;
}
