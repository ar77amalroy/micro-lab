#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
    char opcode[10], operand[10], label[10], mnemonic[10], code[10];
    int locctr, start, length;
    char ch;
    FILE *input, *optab, *symbol, *output;
    input = fopen("input.txt", "r");
    optab = fopen("optab.txt", "r");
    symbol = fopen("symtab.txt", "w");
    output = fopen("output.txt", "w");
    if (!input) {
        printf("Error: Could not open input.txt\n");
    }
    if (!optab) {
        printf("Error: Could not open optab.txt\n");
    }
    if (!symbol) {
        printf("Error: Could not open symtab.txt\n");
    }
    if (!output) {
        printf("Error: Could not open output.txt\n");
    }
    if (!input || !optab || !symbol || !output) {
        return 1;
    }
    if (fscanf(input, "%s\t%s\t%s", label, opcode, operand) == EOF) {
        printf("Error: input.txt is empty.\n");
        return 1;
    }
    if (strcmp(opcode, "START") == 0) {
        start = (int)strtol(operand, NULL, 16);
        locctr = start;
        fprintf(output, "\t%s\t%s\t%s\n", label, opcode, operand);
        if (fscanf(input, "%s\t%s\t%s", label, opcode, operand) == EOF) {
            printf("Error: No instructions after START.\n");
            return 1;
        }
    } else {
        locctr = 0;
    }
    while (strcmp(opcode, "END") != 0) {
        fprintf(output, "%04X\t", locctr);
        if (strcmp(label, "-") != 0) {
            fprintf(symbol, "%s\t%04X\n", label, locctr);
        }
        rewind(optab);
        int found = 0;
        while (fscanf(optab, "%s\t%s", code, mnemonic) != EOF) {
            if (strcmp(opcode, code) == 0) {
                locctr += 3;
                found = 1;
                break;
            }
        }
        if (!found) {
            if (strcmp(opcode, "WORD") == 0) {
                locctr += 3;
            } else if (strcmp(opcode, "RESW") == 0) {
                locctr += 3 * atoi(operand);
            } else if (strcmp(opcode, "RESB") == 0) {
                locctr += atoi(operand);
            } else if (strcmp(opcode, "BYTE") == 0) {
                if (operand[0] == 'C') {
                    locctr += strlen(operand) - 3;
                } else if (operand[0] == 'X') {
                    locctr += (strlen(operand) - 3) / 2;
                }
            }
        }
        fprintf(output, "%s\t%s\t%s\n", label, opcode, operand);
        if (fscanf(input, "%s\t%s\t%s", label, opcode, operand) == EOF) {
            break;
        }
    }
    fprintf(output, "\t%s\t%s\t%s\n", label, opcode, operand);
    length = locctr - start;
    printf("The length of code: %d\n", length);
    fclose(input);
    fclose(optab);
    fclose(symbol);
    fclose(output);
    output = fopen("output.txt", "r");
    if (!output) {
        printf("Error: Could not reopen output.txt for reading.\n");
        return 1;
    }
    printf("\n\nContents of output.txt:\n");
    char line[100];
    while (fgets(line, sizeof(line), output)) {
        printf("%s", line);
    }
    fclose(output);
    FILE *fsymtab = fopen("symtab.txt", "r");
    if (!fsymtab) {
        printf("Error: Could not open symtab.txt\n");
        return 1;
    }
    printf("\n\nContents of symtab.txt:\n");
    while ((ch = fgetc(fsymtab)) != EOF) {
        printf("%c", ch);
    }
    fclose(fsymtab);
    return 0;
}
