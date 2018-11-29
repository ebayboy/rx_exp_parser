#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 获取 表达式长度 */
/* 表达式可能是: [30002 ] 或 [      0 ] 或 [ 30004 ] 或 [      1] */
static int get_and_or_opr_len(char *in, int ilen, char **out, int *olen)
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
static int get_not_opr_len(char *in, int ilen, char **out)
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

/* 将源字符串替换成结果字符串 */
static int replace_result(char *in, int ilen, char *start, int is_not, int offset)
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

    rule_result = get_result_by_rule(mmb, ilen);

    if (is_not) {
        /* start = !30002 */
        /* ilen + ! + offset */
        snprintf(mmb_id, sizeof(mmb_id) - 1, "%*d", ilen + offset, !rule_result);
    } else {
        /* start =[ 30001 ]*/
        /* ilen + offset */
        snprintf(mmb_id, sizeof(mmb_id) - 1, "%*d", ilen + offset, rule_result);
    }

    memcpy(start, mmb_id, ilen + offset);

    free(mmb);

    return 0;
}

/* @data: "30002 &      0 | 30004 & 30005 |      1" */
static  int process_and_or_opr(char *data, int dlen)
{
    int i, olen = 0;
    char *pos = data;
    char *start = data;
    char *out = NULL;

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


            fprintf(stderr, "%s:%d before repalce:[%s]\n", __func__, __LINE__,  data);
            fprintf(stderr, "out [%s] olen:[%d]\n", out, olen);
            /* olen == 1 : [      1] */
            if (olen > 1) {
                replace_result(out, olen, start, 0, out - start);
            }

            fprintf(stderr, "%s:%d after repalce:[%s] len:[%d]\n\n", __func__, __LINE__, data, strlen(data));

            start = pos + i + 1;
        }
    }

    return 0;
}

static int process_not_opr(char *data, int dlen)
{
    int rule_id;
    char *opr;
    int i;
    int olen = 0;

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
        olen = get_not_opr_len(start, dlen - i, &out);
        if (olen > 0) {
            printf("before data:[%s]\n", data);
            replace_result(out, olen, start, 1, out - start);
            printf("after  data:[%s]\n", data);
        }
    }

    return 0;
}

static get_exps_result(int exp1, int exp2, char opt)
{
    if (opt == '&') {
        return exp1 & exp2;
    } else if (opt == '|') {
        return exp1 | exp2;
    } else {
        return -1;
    }

    return 0;
}

/* 计算与 和 或表达式结果
 * in-out @data: [    0 &       0 |     0 &     1 |      1] or  [  1 ]
 * @dlen:  data length
 * RETURN: [ 0 ]
 * */
static int process_and_or_opt_result(char *data, int dlen)
{
    int i, j, k;
    int sum = 1;
    int exist_opr = 0;

    char *pos = data;
    char *opr = NULL;
    char *exp1, *exp2;
    int opts_count = 0;
    int exp1_num, exp2_num;

    for (i = 0 ; i < dlen; i++) {
        if (*(pos + i) == '&' || *(pos + i) == '|') {
            exist_opr = 1;
            break;
        }
    }

    if (!exist_opr) {
        /* [  1  ] */
        /* 不存在 & | 操作符， 直接返回表达式 */
        return 0;
    }

    /* [    0 &       0 |     0 &     1 |      1] */
    for (i = 0 ; i < dlen; i++) {
        if (*(pos + i) == '&' || *(pos + i) == '|') {
            opts_count++;

            /* find opr */
            opr = pos + i;

            exp1 = exp2 = NULL;
            exp1_num = exp2_num = 0;
            if (opts_count == 1) {
                /* find exp1 */
                for (j = i; j >= 0; j--) {
                    if (*(pos + j) != ' ' && *(pos + j) != '&' && *(pos + j) != '|') {
                        exp1 = pos + j;
                        exp1_num = *exp1 - '0';
                    }
                }
            } else {
                exp1_num = sum;
            }

            /* find exp2 */
            for (k = i; k < dlen; k++) {
                if (*(pos + k) != ' ' && *(pos + k) != '&' && *(pos + k) != '|') {
                    exp2 = pos + k;
                    exp2_num = *exp2 - '0';
                    break;
                }
            }

            if (exp2 == NULL) {
                continue;
            }

            sum = get_exps_result(exp1_num, exp2_num, *opr);

            fprintf(stderr, "opr:[%c] exp1_num:[%d] exp2_num:[%d] sum:[%d]\n", 
                    *opr, exp1_num, exp2_num, sum);
        }
    }

    /* 生成新的表达式 */
    /* data:[1                                       ] */
    memset(data, ' ', dlen);
    if (sum == 0) {
        data[0]  = '0';
    } else {
        data[0]  = '1';
    }

    return 0;
}

int exp_parser_parse(char *data, int dlen)
{
    /* process not opr */
    process_not_opr(data, strlen(data));

    /* process 'and opr' && 'or opt' */
    process_and_or_opr(data, strlen(data));

    process_and_or_opt_result(data, dlen);

    return 0;
}



