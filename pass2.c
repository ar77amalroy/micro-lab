#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
    char label[10], opcode[10], operand[10], code[10], mnemonic[10], objcode[100];
    int operand_address, start, locctr, length;
    FILE *input, *optab, *symtab, *output, *objprog;
    input = fopen("inter.txt", "r");
    optab = fopen("optab.txt", "r");
    symtab = fopen("symtab.txt", "r");
    output = fopen("output_pass2.txt", "w");
    objprog = fopen("object_program.txt", "w");
    if (!input || !optab || !symtab || !output || !objprog) {
        printf("Error opening files\n");
        return 1;
    }
    if (fscanf(input, "%s\t%s\t%s", label, opcode, operand) == EOF) {
        printf("Error: input file is empty\n");
        return 1;
    }
    if (strcmp(opcode, "START") == 0) {
        start = (int)strtol(operand, NULL, 16);
        locctr = start;
        fprintf(output, "\t%s\t%s\t%s\n", label, opcode, operand);
        fprintf(objprog, "H^%s^%06X^000000\n", label, start);
        if (fscanf(input, "%s\t%s\t%s", label, opcode, operand) == EOF) {
            printf("Error: no instructions after START\n");
            return 1;
        }
    } else {
        start = 0;
        locctr = 0;
    }
    int text_record_length = 0;
    char text_record[1000] = "";
    int text_record_start = locctr;
    while (strcmp(opcode, "END") != 0) {
        if (opcode[0] != '.') {
            rewind(optab);
            int found = 0;
            memset(objcode, 0, sizeof(objcode));
            while (fscanf(optab, "%s\t%s", code, mnemonic) != EOF) {
                if (strcmp(opcode, code) == 0) {
                    found = 1;
                    rewind(symtab);
                    int sym_found = 0;
                    operand_address = 0;
                    if (strcmp(operand, "-") != 0) {
                        while (fscanf(symtab, "%s\t%s", label, code) != EOF) {
                            if (strcmp(label, operand) == 0) {
                                operand_address = (int)strtol(code, NULL, 16);
                                sym_found = 1;
                                break;
                            }
                        }
                        if (!sym_found) {
                            operand_address = 0;
                        }
                    }
                    sprintf(objcode, "%s%04X", mnemonic, operand_address);
                    break;
                }
            }
            if (!found) {
                if (strcmp(opcode, "WORD") == 0) {
                    int val = atoi(operand);
                    sprintf(objcode, "%06X", val);
                } else if (strcmp(opcode, "BYTE") == 0) {
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
                    strcpy(objcode, "");
                }
            }
            int obj_len = strlen(objcode) / 2;
            if (text_record_length + obj_len > 30) {
                fprintf(objprog, "T^%06X^%02X%s\n", text_record_start, text_record_length, text_record);
                strcpy(text_record, "");
                text_record_length = 0;
                text_record_start = locctr;
            }
            if (text_record_length == 0) {
                text_record_start = locctr;
            }
            if (strlen(objcode) > 0) {
                strcat(text_record, "^");
                strcat(text_record, objcode);
                text_record_length += obj_len;
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
            if (operand[0] == 'C') {
                locctr += strlen(operand) - 3;
            } else if (operand[0] == 'X') {
                locctr += (strlen(operand) - 3) / 2;
            }
        } else {
            locctr += 3;
        }
        if (fscanf(input, "%s\t%s\t%s", label, opcode, operand) == EOF) {
            break;
        }
    }
    if (text_record_length > 0) {
        fprintf(objprog, "T^%06X^%02X%s\n", text_record_start, text_record_length, text_record);
    }
    fprintf(objprog, "E^%06X\n", start);
    if (strcmp(opcode, "END") == 0) {
        fprintf(output, "%s\t%s\t%s\n", label, opcode, operand);
    }
    fclose(input);
    fclose(optab);
    fclose(symtab);
    fclose(output);
    fclose(objprog);
    return 0;
}
