#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Seyyid Hikmet CELIK
// 181201047

#define SIZE 100

struct tags{
    char title[500];
    char description[500];
    char link[500];
    char pubDate[500];
};

void removeSpaces(char* s){
    char* d = s;

    do {
        while(*d == ' '){
            ++d;
        }
    } while(*s++ = *d++);
}


char *trimWhiteSpaces(char *str){
    char *end;

    while(isspace((char)*str)) str++;  
    if(*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((char)*end)) end--;

    end[1] = '\0';  
    return str;
}


int startsWith(const char *str, const char *pre){
    char cp;
    char cs;

    if (!*pre)
        return 1;

    while ((cp = *pre++) && (cs = *str++)){
        if (cp != cs)
            return 0;
    }

    if (!cs)
        return 0;

    return 1;
}

char *strRemove(char *str, const char *sub) {
    char *p, *q, *r;

    if (*sub && (q = r = strstr(str, sub)) != NULL) {
        int len = strlen(sub);
        while ((r = strstr(p = r + len, sub)) != NULL) {
            while (p < r)
                *q++ = *p++;
        }
        while ((*q++ = *p++) != '\0')
            continue;
    }
    return str;
}

int main(int argc, char **argv){
    char inFileName[SIZE];
    char outFileName[SIZE];

    int i = 0;
    int isoGiven = 0;

    for (i = 1; i < argc; i++){
        if(argv[i][0] == '-'){
            switch(argv[i][1]){
                case 'i':
                    strcpy(inFileName, argv[i+1]);
                    ++i;
                break;

                case 'o':
                    strcpy(outFileName, argv[i+1]);
                    ++i;
                    isoGiven = 1;
                break;

                default:
                    printf("Unsupported option!\n");
                    exit(1);
            }
        }
        else{
            printf("Unsupported option!\n");
            exit(1);
        }
    }
    
    FILE * inFile;
    inFile = fopen(inFileName, "r");
    
    if(inFile == NULL){
        printf("%s file not found!\n", inFileName);
        exit(1);
    }
    
    if(!isoGiven) strcpy(outFileName, "defaultoutput.csv");
    
    char outLines[31][500];
    
    int k = 0;

    FILE * outFile;

    outFile = fopen(outFileName, "r+");
    if(outFile == NULL){
        fclose(outFile);
        outFile = fopen(outFileName, "w+");
        fprintf(outFile, "title,description,link,pubDate\n");
    }
    else{
        char firstLine[500];
        char title[20], description[20], link[20], pubDate[20];

        fscanf(outFile, " %[^\n]s", firstLine);

        if(firstLine != '\0'){
            char *colTok = strtok(firstLine, ",");

            strcpy(title, colTok);
            colTok = strtok(NULL, ",");
            strcpy(description, colTok);
            colTok = strtok(NULL, ",");
            strcpy(link, colTok);
            colTok = strtok(NULL, ",");
            strcpy(pubDate, colTok);

            removeSpaces(title);
            removeSpaces(description);
            removeSpaces(link);
            removeSpaces(pubDate);

            if(strcmp(title, "title") || strcmp(description, "description") || strcmp(link, "link") || strcmp(pubDate, "pubDate")){
                printf("Not suitable csv format. Exiting program.\n");
                exit(1);
            }
            else{
                char buf[500];
                while(fscanf(outFile, " %[^\n] ", buf)){
                    strtok(buf, "\n");
                    strcpy(outLines[k], buf);
                    k++;

                    if (feof(outFile)) break;
                }
            }
        }
        else{
            fprintf(outFile, "title,description,link,pubDate\n");
        }

    }

    fseek(outFile, 0, SEEK_END);

    struct tags csvCols[30];

    char buffer[500];

    int afterTtl = 0;

    int tagCount = 0;
    int itemCount = 0;

    char quotedStr[502] = "\"";

    while(fgets(buffer, 500, inFile)) {
        if (feof(inFile)) break;

        strtok(buffer, "\n");

        strcpy(buffer, trimWhiteSpaces(buffer));        
        if(afterTtl && !startsWith(buffer, "<guid")){

            char *tok = strtok(buffer, ">");
            while (tok != NULL) {
                strcpy(tok, strRemove(strRemove(strRemove(strRemove(tok, "<![CDATA["), "]]"), "</link"), "</pubDate"));
                if(!startsWith(tok, "<")){
                    strcat(quotedStr, tok);
                    strcat(quotedStr, "\"");
                    if(tagCount % 4 == 0){
                        strcpy(csvCols[itemCount].title, quotedStr);
                    }
                    else if(tagCount % 4 == 1){
                        strcpy(csvCols[itemCount].description, quotedStr);
                    }
                    else if(tagCount % 4 == 2){
                        strcpy(csvCols[itemCount].link, quotedStr);
                    }
                    else if(tagCount % 4 == 3){
                        strcpy(csvCols[itemCount].pubDate, quotedStr);
                        itemCount++;
                    }
                    tagCount++;

                    strcpy(quotedStr, "\"");
                }
                tok = strtok(NULL, ">");
            }
        }

        if(startsWith(buffer, "<ttl>"))
            afterTtl = 1;
    }

    char eachLine[500];
    int isExist = 0;

    for(i=0; i<itemCount; i++){
        strcpy(eachLine, csvCols[i].title);
        strcat(eachLine, ",");
        strcat(eachLine, csvCols[i].description);
        strcat(eachLine, ",");
        strcat(eachLine, csvCols[i].link);
        strcat(eachLine, ",");
        strcat(eachLine, csvCols[i].pubDate);
        
        for(k=0; k<30; k++){
            if(!strcmp(outLines[k], eachLine))
                isExist = 1;
        }

        if(!isExist)
            fprintf(outFile, "%s\n", eachLine);
        
        isExist = 0;
    }


    fclose(inFile);
    fclose(outFile);

    return 0;
}