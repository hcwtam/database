#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum
{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_COMMAND
} PrepareResult;

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;
typedef struct
{
    StatementType type;
} Statement;

typedef struct
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

InputBuffer *new_input_buffer()
{
    InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void *close_input_buffer(InputBuffer *input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}

void print_prompt()
{
    printf("db > ");
}

void read_input(InputBuffer *input_buffer)
{
    ssize_t byte_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (byte_read < 0)
    {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    // ignore trailing newline
    input_buffer->input_length = byte_read - 1;
    input_buffer->buffer[byte_read - 1] = 0;
}

MetaCommandResult do_meta_command(InputBuffer *input_buffer)
{
    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_COMMAND;
}

void execute_statement(Statement *statement)
{
    switch (statement->type)
    {
    case (STATEMENT_INSERT):
        printf("do insert here\n");
        break;
    case (STATEMENT_SELECT):
        printf("do select here\n");
        break;

    default:
        printf("Should not get here: trying to execute non-existing statement\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char const *argv[])
{
    InputBuffer *input_buffer = new_input_buffer();

    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer))
            {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecognised command '%s'\n", input_buffer->buffer);
                continue;
            default:
                printf("Should not get here: parsing meta command\n");
                exit(EXIT_FAILURE);
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement))
        {
        case (PREPARE_SUCCESS):
            break;
        case (PREPARE_UNRECOGNIZED_COMMAND):
            printf("Unrecognised keyword at start of '%s'\n", input_buffer->buffer);
            continue;

        default:
            printf("Should not get here: parsing statement\n");
            exit(EXIT_FAILURE);
        }

        execute_statement(&statement);
        printf("Executed.\n");
    }
}
