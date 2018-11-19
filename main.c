
#include <stdio.h>
#include <string.h>

/* 获取 表达式长度 */
int get_not_opr_len(char *in, int ilen, char **out)
{
    int olen = 0;
    char *pstart = in;

    if (in == NULL || ilen <= 0) {
        return 0;
    }

    /* move to start pos */
    while (pstart < in + ilen) {
        if (*(pstart + olen) == ' ') {
            pstart++;
            continue;
        }

        break;
    }

    *out = pstart;
    olen = 0;

    /* calculate length */
    while ( pstart < in + ilen) {
        if (*(pstart + olen) == '&' 
                || *(in + olen) == '|' 
                || *(in + olen) == '!') {
            break;
        }

        if (*(pstart + olen) == ' ') {
            break;
        }

        olen++;
    }

    return olen;
}


int process_not_opr(char *data, int dlen)
{
    int rule_id;
    char *opr;
    int i;
    char mmb[BUFSIZ] = {0};
    int mmb_len = 0;

    char *pos = data;
    char *start = NULL;
    char *out = NULL;

    if (data == NULL || dlen == 0) {
        return -1;
    }

    fprintf(stderr, "data:[%s] dlen:[%d]\n", data, dlen);

    /* calculate ! */
    for (i = 0; i < dlen; i++) {
        /* 先计算 ! 的表达式 */
        if (*(pos + i) != '!') {
            continue;
        }

        if (*(pos + i) == ' ') {
            continue;
        }

        /* i是最后一个字符 */
        if (i == dlen - 1) {
            continue;
        }

        start = pos + i;
        out = NULL;
        mmb_len = get_not_opr_len(start + 1, dlen - i, &out);

        /* TODO: trim */
        if (mmb_len > 0) {
            memset(mmb, 0, sizeof(mmb));
            memcpy(mmb, out, mmb_len);
            fprintf(stderr, "mmb_len:[%d] mmb:[%s] pos:[%s]\n", mmb_len, mmb, start);
        }
    }

    return 0;
}


int main()
{
    char *data = "!30001 | 30004 & 30005 | ! 30006 & 30003 | ! 30002";

    /* process not opr */
    process_not_opr(data, strlen(data));

    /* process 'and opr' && 'or opt' */

    return 0;
}

