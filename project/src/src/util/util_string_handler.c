#include <string.h>
#include <stdio.h>
#include "device/device_communication.h"
#include "util/util_string_handler.h"

char **string_to_string_array(const char string[]){
    char *convert_string = malloc(sizeof(string));
    strcpy(convert_string, string);

    int i = 0;
    int count = 0;

    for(i; i<strlen(string); i++){
        if(string[i] == DEVICE_COMMUNICATION_MESSAGE_FIELDS_DELIMITER[0]) count++;
    }

    char **tokenized_string;
    tokenized_string = malloc(count * sizeof(char*));

    char *token_string;


    token_string = strtok(convert_string, DEVICE_COMMUNICATION_MESSAGE_FIELDS_DELIMITER);

    count = 0;
    while( token_string != NULL ) {
        tokenized_string[count] = malloc((DEVICE_COMMUNICATION_MESSAGE_LENGTH + 1) * sizeof(char));
        strcpy(tokenized_string[count], token_string);
        token_string = strtok(NULL, DEVICE_COMMUNICATION_MESSAGE_FIELDS_DELIMITER);
        count++;
    }

    free(convert_string);
    return tokenized_string;
}

char *string_array_to_string(const char *input[]){
    char * output_string = malloc(DEVICE_COMMUNICATION_MESSAGE_LENGTH);
    char * tmp;

    strcpy(output_string, input[0]);
    strcat(output_string, DEVICE_COMMUNICATION_MESSAGE_FIELDS_DELIMITER);
    int i = 1;
    while(input[i] != NULL){
        strcat(output_string, input[i]);
        strcat(output_string, DEVICE_COMMUNICATION_MESSAGE_FIELDS_DELIMITER);
        i++;
    }
    return output_string;
}