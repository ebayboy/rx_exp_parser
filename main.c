
#include <stdio.h>
#include <string.h>

/* 获取 表达式长度 */
int get_not_opr_len(char *in, int ilen, char **out)
{
    int olen = 0;
    char *pstart = NULL;

    if (in == NULL || ilen <= 0) {
        return 0;
    }

    if (*in != '!') {
        return -1;
    }

    /* continue ! */
    pstart = in + 1;

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
    while ( (pstart + olen) < in + ilen) {
        if (*(pstart + olen) == '&' 
                || *(pstart + olen) == '|' 
                || *(pstart + olen) == '!') {
            break;
        }

        if (*(pstart + olen) == ' ') {
            break;
        }

        olen++;
    }

    return olen;
}

static int get_result_by_rule(const char *rule_id, int len)
{
    char rule_str[BUFSIZ] = {0};
    int id = 0;

    memcpy(rule_str, rule_id, len);

    id = atoi(rule_str) % 2;

    return id;
}

int process_and_or_opr(char *data, int dlen)
{
    int rule_id = 0, i = 0, mmb_len = 0;
    char *opr;
    char *pos = data;
    char mmb[BUFSIZ] = {0};
    char *start = NULL;
    char *out = NULL;
    int rule_result = 0;

    if (data == NULL || dlen == 0) {
        return -1;
    }

    fprintf(stderr, "%s:%d data:[%s] dlen:[%d]\n", __func__, __LINE__, data, dlen);

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
        mmb_len = get_not_opr_len(start, dlen - i, &out);

        if (mmb_len > 0) {
            memset(mmb, 0, sizeof(mmb));
            memcpy(mmb, out, mmb_len);

            rule_result = get_result_by_rule(mmb, mmb_len);
            fprintf(stderr, "mmb_len:[%d] mmb:[%s] pos:[%s] rc:[%d]\n", mmb_len, mmb, start, rule_result);
        }


    }

    return 0;
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
    int rule_result = 0;

    if (data == NULL || dlen == 0) {
        return -1;
    }

    fprintf(stderr, "%s:%d data:[%s] dlen:[%d]\n", __func__, __LINE__, data, dlen);

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
        mmb_len = get_not_opr_len(start, dlen - i, &out);

        if (mmb_len > 0) {
            memset(mmb, 0, sizeof(mmb));
            memcpy(mmb, out, mmb_len);

            /* 将源字符串替换成结果字符串 */
            char mmb_id[512] = {0};
            rule_result = get_result_by_rule(mmb, mmb_len);

            snprintf(mmb_id, sizeof(mmb_id) - 1, "%*d", mmb_len + 1, !rule_result);

#if 0
            printf("mmb:[%s] mmb_id:[%s] mmb_id_len:[%d] start:[%s] mmb_len:[%d]\n",
                    mmb, mmb_id, strlen(mmb_id), start, mmb_len);
#endif

            printf("mmb:[%s]\n", mmb);
            printf("before data:[%s]\n", data);

            memcpy(start, mmb_id, mmb_len + (out - start));

            printf("after  data:[%s]\n", data);
        }
    }

    return 0;
}

int main()
{
    char data[1024] = "30002 & !30003 | 30004 & 30005 | !30006";
   // "!30001 | 30004 & 30005 | ! 30006 & 30003 | ! 30002";

    /* process not opr */
    process_not_opr(data, strlen(data));

    /* process 'and opr' && 'or opt' */
    process_and_or_opr(data, strlen(data));

    return 0;
}

