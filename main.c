#include <stdio.h>
#include <process.h>
#include <malloc.h>

struct human
{
    char   last_name[20];
    char  first_name[20];
    char middle_name[20];
    unsigned long long int phone_number;
};

struct list
{ //Doubly linked list
    struct human local_data;
    struct list *next;
    struct list *previous;
};

struct list *top = NULL;
struct list *bot = NULL;

void delete_all_top ()
{
    struct list *n = top;
    struct list *o;

    while(n)
    {
        o = n->next;
        free(n);
        n = o;
    }

    top = NULL;
    bot = NULL;
}

FILE * input;
FILE * output;

void error_handler(int error, int critical)
{
    if (critical == 0)
    {
        printf("Error");
    }
    else
    {
        printf("Critical error");
    }

    printf(" number %d: ", error);

    switch (error)
    {
    case 1:
    {
        printf("Missing null command line argument");
        break;
    }
    case 2:
    {
        printf("The program parameters are completely missing");
        break;
    }
    case 3:
    {
        printf("The name of the output file and the output format are missing");
        break;
    }
    case 4:
    {
        printf("Failed to open input file");
        break;
    }
    case 5:
    {
        printf("Failed to open output file");
        break;
    }
    case 6:
    {
        printf("It is not possible to swap nonexistent array elements");
        break;
    }
    case 7:
    {
        printf("Unknown comparison result");
        break;
    }
    case 8:
    {
        printf("Unable to allocate more memory");
        break;
    }
    default:
    {
        printf("Unknown error");
        critical = 1;
    }
    }

    if (critical == 0)
    {
        printf(";\n");
    }
    else
    {
        printf(".");
        fclose(input);
        fclose(output);
        delete_all_top();
        exit(critical);
    }
}

void push_bot (struct human * new_data)
{
    struct list *b;
    if ((b = (struct list *)malloc(sizeof(struct list))) == NULL)
    {
        error_handler(8, 2);
    }

    b->local_data = * new_data;
    b->next = NULL;
    b->previous = NULL;

    if (top != NULL)
    {
        bot->next = b;
        b->previous = bot;
        bot = b;
    }
    else
    {
        bot = b;
        top = b;
    }
}

void read_data_file()
{
    struct human buffer;
    while(fscanf(input, "%s %s %s %llu", buffer.last_name, buffer.first_name,
                 buffer.middle_name, &buffer.phone_number) != EOF)
    {
        push_bot(&buffer);
    }
}

void write_data_file()
{
    struct list * buffer = top;
    while (buffer != NULL)
    {
        fprintf(output, "%s %s %s %llu\n",
                buffer->local_data.last_name,
                buffer->local_data.first_name,
                buffer->local_data.middle_name,
                buffer->local_data.phone_number);
        buffer = buffer->next;
    }
}

void swap(struct list * first, struct list * second)
{
    struct human buffer;
    buffer = first->local_data;
    first->local_data = second->local_data;
    second->local_data = buffer;
}

int comparison_string(const char first[20], const char second[20])
{
    int i = 0;
    for(; first[i] != '\0' && second[i] != '\0'; i++)
    {
        if (first[i] < second[i])
        {
            return 0;
        }
        else if (first[i] > second[i])
        {
            return 1;
        }
    }
    if (first[i] == '\0' && second[i] != '\0')
    {
        return 0;
    }
    else if (second[i] == '\0' && first[i] != '\0')
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

typedef int ( * structure_function ) (  struct human const * ,
                                        struct human const * );

int structure_function_comparison_by_last_names
        (struct human const * const first_struct, struct human const * const second_struct)
{
    return comparison_string(first_struct->last_name, second_struct->last_name);
}

int structure_function_comparison_by_first_names
        (struct human const * const first_struct, struct human const * const second_struct)
{
    return comparison_string(first_struct->first_name, second_struct->first_name);
}

int structure_function_comparison_by_middle_names
        (struct human const * const first_struct, struct human const * const second_struct)
{
    return comparison_string(first_struct->middle_name, second_struct->middle_name);
}

int structure_function_comparison_by_phone_number
        (struct human const * const first_struct, struct human const * const second_struct)
{
    if(first_struct->phone_number < second_struct->phone_number)
    {
        return 0;
    }
    else if (first_struct->phone_number > second_struct->phone_number)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

////structure_function_comparison_by_last_names
////structure_function_comparison_by_first_names
////structure_function_comparison_by_middle_names
////structure_function_comparison_by_phone_number

structure_function first = structure_function_comparison_by_last_names;
structure_function second = structure_function_comparison_by_first_names;
structure_function third = structure_function_comparison_by_middle_names;
structure_function fourth = structure_function_comparison_by_phone_number;

int comparison(struct human one, struct human two)
{
    int buffer = first(&one, &two);
    if (buffer != 2)
    {
        return buffer;
    }
    buffer = second(&one, &two);
    if (buffer != 2)
    {
        return buffer;
    }
    buffer = third(&one, &two);
    if (buffer != 2)
    {
        return buffer;
    }
    buffer = fourth(&one, &two);
    if (buffer != 2)
    {
        return buffer;
    }
    return 2;
}

struct list * distribution(struct list * left, struct list * right)
{
    struct human reference = right->local_data;

    struct list *mid = left->previous;

    struct list *iter = left;
    while (iter != right)
    {
        int result = comparison(iter->local_data, reference);
        if (result == 0 || result == 2)
        {
            if (mid == NULL)
            {
                mid = left;
            }
            else
            {
                mid = mid->next;
            }
            swap(mid, iter);
        }
        iter = iter->next;
    }
    if (mid == NULL)
    {
        mid = left;
    }
    else
    {
        mid = mid->next;
    }
    swap(mid, right);
    return mid;
}

void quick_sort(struct list * left, struct list * right)
{
    if (right != NULL && left != right && left != right->next)
    {
        struct list * mid = distribution(left, right);
        quick_sort(left, mid->previous);
        quick_sort(mid->next, right);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        error_handler(1, 1);
    }

    switch (argc)
    {
    case 1:
    {
        error_handler(2, 1);
    }
    case 2:
    {
        error_handler(3, 1);
    }
    default:
    {
        break;
    }
    }

    if ((input = fopen(argv[1], "r")) == NULL)
    {
        error_handler(4, 1);
    }

    read_data_file();
    fclose(input);

    quick_sort(top, bot);

    if ((output = fopen(argv[2], "w")) == NULL)
    {
        error_handler(5, 1);
    }

    write_data_file();
    fclose(output);
    delete_all_top();

    return 0;
}