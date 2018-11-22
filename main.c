
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 获取 表达式长度 */
/* 表达式可能是: [30002 ] 或 [      0 ] 或 [ 30004 ] 或 [      1] */
int get_and_or_opr_len(char *in, int ilen, char **out, int *olen)
{
    int i;
    char *start = NULL;
    char *pos = in;

    if (in == NULL || out == NULL || ilen == 0 || olen == NULL) {
        return -1;
    }

    for (i = 0; i < ilen; i++) {

        if (start == NULL && *(pos + i) != ' ') {
           start = pos + i;
           *olen = 1;
           continue;
        }

        if (start != NULL) {
            if (*(pos + i) == ' ' || *(pos + i) == '&' || *(pos + i) == '|') {
                break;
            }
            *olen += 1;
        }
    }

    *out = start;

    return 0;
}

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

/* @data: "30002 &      0 | 30004 & 30005 |      1" */
int process_and_or_opr(char *data, int dlen)
{
    int i, olen = 0;
    char *pos = data;
    char *start = data;
    char *out = NULL;

    fprintf(stderr, "%s:%d\n", __func__, __LINE__);

    if (data == NULL  || dlen <= 0) {
        fprintf(stderr, "ERROR: %s:%d\n", __func__, __LINE__);
        return -1;
    }

    start = data;
    for (i = 0; i < dlen && (start < data + dlen); i++) {
        /* get opr */
        if (*(pos + i) == '&' || *(pos + i) == '|') {
            /* find pre exp */
            /* 表达式可能是: [30002 ] 或 [      0 ] 或 [ 30004 ] 或 [      1] */
            get_and_or_opr_len(start, dlen - i, &out, &olen);

            /* olen == 1 : [      1] */
            if (olen > 1) {
                 
            }

            fprintf(stderr, "start:[%s] ilen:[%d] out:[%.*s]\n", start , dlen - i, olen, out);
            start = pos + i + 1;
            fprintf(stderr, "start_new:[%s]\n", start);
        }
    }

    return 0;
}

int replace_result(char *in, int ilen, char *start)
{
    char *mmb = NULL;
    char mmb_id[64] = {0};
    int rule_result;

    if (in == NULL || ilen == 0 || strlen(in) == 0) {
        return -1;
    }

    if ((mmb = malloc(ilen)) == NULL) {
        return -1;
    }
    memset(mmb, 0, sizeof(mmb));
    memcpy(mmb, in, ilen);

    /* 将源字符串替换成结果字符串 */
    rule_result = get_result_by_rule(mmb, ilen);
    snprintf(mmb_id, sizeof(mmb_id) - 1, "%*d", ilen + 1, !rule_result);
    memcpy(start, mmb_id, ilen + (in - start));

    free(mmb);

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
            printf("before data:[%s]\n", data);
            replace_result(out, mmb_len, start);
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

