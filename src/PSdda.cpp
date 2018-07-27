#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t get_fileSize(const char* filename);
const char* GetFileBaseName(const char* szPath);
char* memfind(const char* buf, const char* tofind, size_t len);

int main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("本工具清除PSD文件中垃圾数据 by 蘭公子 sRGB.vicp.net\n\n");
        printf("Usage: psdda.exe  test.psd  [fix_psd.psd] \n");
        return -1;
    }

    const char* filename = argv[1];
    const char* savefile = GetFileBaseName(filename);

    if (argc == 3) {
        savefile = argv[2];
    }

    FILE* psdfile = fopen(filename, "rb");
    if (psdfile == NULL) {
        fputs("File error", stderr);
        exit(1);
    }

    long filesize = get_fileSize(filename);
    size_t result;


    char* buf = new char[filesize];
    result = fread(buf, 1, filesize, psdfile);

    printf("文件大小:%d\t\t文件名:%s\n",  filesize, savefile);


    const char* flag_beg = "<photoshop:DocumentAncestors>" ;
    const char* flag_end = "</photoshop:DocumentAncestors>" ;

    char* pch = NULL;
    char* pch2 = NULL;
    size_t pos = 0;
    size_t pos2 = 0;
    bool flag_found = false;

    pch = memfind(buf, flag_beg, result);
    pch2 = memfind(buf, flag_end, result);

    while ((pch != NULL) && (pch2 != NULL)) {

        pos = pch - buf;
        pos2 = pch2 - buf + strlen(flag_end);
        printf("DocumentAncestors标记起止: %d   -->  %d\n",  pos, pos2);

        memset(pch, '+', pos2 - pos);   // 清楚垃圾数据

        flag_found = true;

        pch = memfind(buf, flag_beg, result);
        pch2 = memfind(buf, flag_end, result);
    }

    if (!flag_found) {
        fputs("PSD文件没有 DocumentAncestors标记，不需要工具 ", stderr);
        exit(1);
    }

    // 默认修复文件名，前缀 Fix_
    char fix_name[256];
    if (argc == 2) {
        strcpy(fix_name, "Fix_");
        strcat(fix_name, savefile);
        savefile = fix_name;
    }

    FILE* pFile = fopen(savefile, "wb");
    fwrite(buf, sizeof(char), result, pFile);

    printf("清理垃圾后文件名:\t%s\n记得要用PS打开保存才能把文件变小!\n", savefile);

    delete [] buf;
    fclose(pFile);
    fclose(psdfile);

    return 0;
}




// 获得文件大小
size_t get_fileSize(const char* filename)
{
    FILE* pfile = fopen(filename, "rb");
    fseek(pfile, 0, SEEK_END);
    size_t size = ftell(pfile);
    fclose(pfile);
    return size;

}


// 得到全路径文件的文件名
const char* GetFileBaseName(const char* szPath)
{
    const char* ret = szPath + strlen(szPath);
    while ((*ret != '\\') && (ret != (szPath - 1))) // 得到文件名
        ret--;
    ret++;
    return ret;
}

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
