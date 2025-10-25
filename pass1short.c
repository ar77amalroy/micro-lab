#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char label[10], opcode[10], operand[10], code[10], mnemonic[10], line[100];
    int locctr = 0, start = 0, length;
    FILE *input = fopen("input.txt", "r");
    FILE *optab = fopen("optab.txt", "r");
    FILE *symtab = fopen("symtab.txt", "w");
    FILE *output = fopen("output.txt", "w");

    if (!input || !optab || !symtab || !output) {
        printf("Error opening files.\n");
        return 1;
    }

    if (fscanf(input, "%s%s%s", label, opcode, operand) == EOF) {
        printf("Error: input.txt is empty.\n");
        return 1;
    }

    if (!strcmp(opcode, "START")) {
        start = (int)strtol(operand, NULL, 16);
        locctr = start;
        fprintf(output, "\t%s\t%s\t%s\n", label, opcode, operand);
        fscanf(input, "%s%s%s", label, opcode, operand);
    }

    while (strcmp(opcode, "END")) {
        fprintf(output, "%04X\t", locctr);
        if (strcmp(label, "-")) fprintf(symtab, "%s\t%04X\n", label, locctr);

        int found = 0;
        rewind(optab);
        while (fscanf(optab, "%s%s", code, mnemonic) != EOF) {
            if (!strcmp(opcode, code)) {
                locctr += 3;
                found = 1;
                break;
            }
        }

        if (!found) {
            if (!strcmp(opcode, "WORD")) locctr += 3;
            else if (!strcmp(opcode, "RESW")) locctr += 3 * atoi(operand);
            else if (!strcmp(opcode, "RESB")) locctr += atoi(operand);
            else if (!strcmp(opcode, "BYTE"))
                locctr += (operand[0] == 'C') ? strlen(operand) - 3 : (strlen(operand) - 3) / 2;
        }

        fprintf(output, "%s\t%s\t%s\n", label, opcode, operand);
        if (fscanf(input, "%s%s%s", label, opcode, operand) == EOF) break;
    }

    fprintf(output, "\t%s\t%s\t%s\n", label, opcode, operand);
    length = locctr - start;
    printf("The length of code: %d\n", length);

    fclose(input); fclose(optab); fclose(symtab); fclose(output);

    // Display output file
    if ((output = fopen("output.txt", "r"))) {
        printf("\n\nContents of output.txt:\n");
        while (fgets(line, sizeof(line), output)) printf("%s", line);
        fclose(output);
    }

    // Display symtab file
    FILE *fsymtab = fopen("symtab.txt", "r");
    if (fsymtab) {
        printf("\n\nContents of symtab.txt:\n");
        while (fgets(line, sizeof(line), fsymtab)) printf("%s", line);
        fclose(fsymtab);
    }

    return 0;
}