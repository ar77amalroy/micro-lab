#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Function to get symbol address from symtab
int get_sym_address(FILE *symtab, const char *symbol) {
    char label[10], addr[10];
    rewind(symtab);
    while (fscanf(symtab, "%s\t%s", label, addr) != EOF) {
        if (strcmp(label, symbol) == 0) {
            return (int)strtol(addr, NULL, 16);
        }
    }
    return 0;
}
// Function to get object code for an instruction
void get_objcode(FILE *optab, FILE *symtab, const char *opcode, const char *operand, char *objcode) {
    char code[10], code_val[10];
    int found = 0;
    rewind(optab);
    while (fscanf(optab, "%s\t%s", code, code_val) != EOF) {
        if (strcmp(opcode, code) == 0) {
            found = 1;
            int addr;
            if (strcmp(operand, "-") != 0) {
                addr = get_sym_address(symtab, operand);
            } else {
                addr = 0;
            }
            sprintf(objcode, "%s%04X", code_val, addr);
            break;
        }
    }
    if (!found) {
        if (strcmp(opcode, "WORD") == 0) {
            int val = atoi(operand);
            sprintf(objcode, "%06X", val);
        } else if (strcmp(opcode, "BYTE") == 0) {
            objcode[0] = '\0';
            if (operand[0] == 'C') {
                int len = strlen(operand) - 3;
                for (int i = 2; i < 2 + len; i++) {
                    char temp[3];
                    sprintf(temp, "%02X", operand[i]);
                    strcat(objcode, temp);
                }
            } else if (operand[0] == 'X') {
                int len = strlen(operand) - 3;
                strncpy(objcode, operand + 2, len);
                objcode[len] = '\0';
            }
        } else {
            objcode[0] = '\0';
        }
    }
}
int main() {
    char label[10], opcode[10], operand[10], objcode[100];
    int locctr = 0, start = 0, text_len = 0, text_start = 0;
    char text[1000] = "";
    FILE *input = fopen("inter.txt", "r");
    FILE *optab = fopen("optab.txt", "r");
    FILE *symtab = fopen("symtab.txt", "r");
    FILE *output = fopen("output_pass2.txt", "w");
    FILE *objprog = fopen("object_program.txt", "w");
    if (!input || !optab || !symtab || !output || !objprog) {
        printf("Error opening files\n");
        return 1;
    }
    if (fscanf(input, "%s\t%s\t%s", label, opcode, operand) == EOF) {
        printf("Input file is empty\n");
        return 1;
    }
    if (strcmp(opcode, "START") == 0) {
        start = locctr = (int)strtol(operand, NULL, 16);
        fprintf(output, "\t%s\t%s\t%s\n", label, opcode, operand);
        fprintf(objprog, "H^%s^%06X^000000\n", label, start);
        fscanf(input, "%s\t%s\t%s", label, opcode, operand);
    }
    while (strcmp(opcode, "END") != 0) {
        if (opcode[0] != '.') {
            get_objcode(optab, symtab, opcode, operand, objcode);
            int obj_len = strlen(objcode)/2;
            if (text_len + obj_len > 30) {
                fprintf(objprog, "T^%06X^%02X%s\n", text_start, text_len, text);
                text[0] = '\0';
                text_len = 0;
                text_start = locctr;
            }
            if (text_len == 0) text_start = locctr;
            if (strlen(objcode) > 0) {
                strcat(text, "^");
                strcat(text, objcode);
                text_len += obj_len;
            }
            fprintf(output, "%s\t%s\t%s\t%s\n", label, opcode, operand, objcode);
        } else {
            fprintf(output, "%s\t%s\t%s\n", label, opcode, operand);
        }
        if (strcmp(opcode, "WORD") == 0) {
            locctr += 3;
        } else if (strcmp(opcode, "RESW") == 0) {
            locctr += 3 * atoi(operand);
        } else if (strcmp(opcode, "RESB") == 0) {
            locctr += atoi(operand);
        } else if (strcmp(opcode, "BYTE") == 0) {
            if (operand[0] == 'C') locctr += strlen(operand) - 3;
            else if (operand[0] == 'X') locctr += (strlen(operand) - 3)/2;
        } else {
            locctr += 3;
        }
        if (fscanf(input, "%s\t%s\t%s", label, opcode, operand) == EOF) break;
    }
    if (text_len > 0) fprintf(objprog, "T^%06X^%02X%s\n", text_start, text_len, text);
    fprintf(objprog, "E^%06X\n", start);
    fprintf(output, "%s\t%s\t%s\n", label, opcode, operand);
    fclose(input); fclose(optab); fclose(symtab); fclose(output); fclose(objprog);
    return 0;
}
