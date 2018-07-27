#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 内存查找字符 memfind
char* memfind(const char* buf, const char* tofind, size_t len)
{
    size_t findlen = strlen(tofind);
    if (findlen > len) {
        return ((char*)NULL);
    }
    if (len < 1) {
        return ((char*)buf);
    }

    {
        const char* bufend = &buf[len - findlen + 1];
        const char* c = buf;
        for (; c < bufend; c++) {
            if (*c == *tofind) { // first letter matches
                if (!memcmp(c + 1, tofind + 1, findlen - 1)) { // found
                    return ((char*)c);
                }
            }
        }
    }

    return ((char*)NULL);
}



// #define BUF_32M  1024

#define BUF_32M  64 * 1024 * 1024


struct DocumentAncestors {
    size_t begin = 0;
    size_t end = 0;
    bool ret = false;
};

int main(int argc, char* argv[])
{
    FILE* psdfile;
    long filesize;
    size_t result;

    if (argc == 1) {

        printf("本工具 删除 photoshop:DocumentAncestors 使用帮助:\npsdda  test.psd\n");
        return -1;
    }

    char* filename = argv[1];

    psdfile = fopen(filename, "rb");
    if (psdfile == NULL) {
        fputs("File error", stderr);
        exit(1);
    }

    // 文件大小:
    fseek(psdfile, 0, SEEK_END);
    filesize = ftell(psdfile);
    rewind(psdfile);

    char* buf = new char[BUF_32M];

    const char* flag = "<photoshop:DocumentAncestors>";
    const char* end_flag = "</photoshop:DocumentAncestors>";

    char* pos = NULL;
    char* end_pos = NULL;

    DocumentAncestors  psdda[128];
    DocumentAncestors* pps_begin = psdda;
    DocumentAncestors* pps = psdda;
    int cnt = 0;



    size_t loop = 0;

    result = fread(buf, 1, BUF_32M, psdfile);

    while (result > 0) {

        pos = memfind(buf, flag, result);

        if (pos != NULL) {
            pps_begin->begin = pos - buf + BUF_32M * loop;
            //  printf("begin位置%d\n",  pps_begin->begin);
            pps_begin++;
        }


        end_pos = memfind(buf, end_flag, result);

        if (end_pos != NULL) {
            pps->end = end_pos - buf + BUF_32M * loop  + strlen(end_flag) ;
            //   printf("end位置%d\n",  pps->end);

            // 保存结果
            pps->ret = true;
            pps++;
            cnt++;
        }

        loop++;  // 再次读文件
        result = fread(buf, 1, BUF_32M, psdfile);
    }

    // 验证文件
    for (int i = 0 ; i != cnt ; i++) {
        if (psdda[i].ret) {
            printf("DocumentAncestors起止位置: %d\t%d \n", psdda[i].begin, psdda[i].end);

            if (psdda[i].begin > psdda[i].end) // 有几率不能正确位置搜索标记，所以稍微验证一下
                return -1;
        }

    }

    rewind(psdfile);
    printf("新文件大小%d\n",  filesize);

    delete[] buf;
    buf = new char[filesize];

    // 另存新文件
    FILE* newfile;
    newfile = fopen("fix_psd.psd", "wb");

    result = fread(buf, 1, psdda[0].begin, psdfile);
    fwrite(buf, sizeof(char), result, newfile);

    for (int i = 0 ; i != cnt ; i++) {
        int jump = (psdda[i].end - psdda[i].begin);
        fseek(psdfile, jump, SEEK_CUR);

        // 填充空格
        char* sp_buf = new char[jump];
        memset(sp_buf, ' ', jump);
        fwrite(sp_buf, sizeof(char), jump, newfile);

        delete[] sp_buf;

        if (psdda[i + 1].begin > 0) {
            result = fread(buf, 1, (psdda[i + 1].begin - psdda[i].end), psdfile);
            fwrite(buf, sizeof(char), result, newfile);
        }
    }

    result = fread(buf, 1, filesize, psdfile);
    fwrite(buf, sizeof(char), result, newfile);

    delete[] buf;
    fclose(psdfile);
    fclose(newfile);
    return 0;
}


