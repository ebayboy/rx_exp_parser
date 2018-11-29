
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "exp_parser.h"

int main()
{
    char data[1024] = "30002 & ! 30003 | 30004 & 30005 | !30006";
    int dlen = strlen(data);

    exp_parser_parse(data, dlen);

    fprintf(stderr, "data:[%.*s]\n", dlen, data);

    return 0;
}

