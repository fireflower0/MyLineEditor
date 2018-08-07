#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR 65536
#define FORMAT "%5d:%s"
#define NOT_SET -1
#define SUCCESS 0
#define ERROR_ARGUMENT_NUMBER 1
#define ERROR_CANNOT_OPEN_FILE 2
#define ERROR_CANNOT_CLOSE_FILE 3

char *gcpErrorMessage[4] = {
    "成功",
    "引数を指定して下さい(-?でヘルプ)",
    "ファイルを開けませんでした",
    "ファイルを閉じれませんでした"
};

char *gcpHelpMessage =
    "a : 追加(行頭のCtrl+zで終了)\n"
    "d : 削除(「1d」「1-2d」のように行指定)\n"
    "i : 挿入(「1i」のように行指定、行頭のCtrl+zで終了)\n"
    "l : 表示(「2l」「2-5l」のように行指定)\n"
    "q : 終了\n"
    "(行番号) : 編集\n";

char *gcpFileName;
char **gcppLine = NULL;
int  giLineCount = 0;

void Append();
void Delete(int start, int end);
void Edit(int start);
char GetCommandChar(char *command);
int GetStart(char *command);
int GetEnd(char *command);
void Insert(int start);
int Interact();
void List(int start, int end);
int LoadFile(char *fileName);
int Quit();
int SaveFile(char *fileName);

int main(int argc, char *argv[]){
    int iCount;
    int iResult = SUCCESS;

    if(argc != 2){
        iResult = ERROR_ARGUMENT_NUMBER;
    }else{
        if(argv[1][0] == '-'){
            switch(argv[1][1]){
            case 'v':
                printf("edline Version 0.01 2018(c)Fireflower0\n");
                return 0;
            case 'h':
            case '?':
                printf("ラインエディタMyLineEditor\n\n"
                       "起動方法 : MyLineEditor ファイル名\n"
                       "\nコマンド : \n%s", gcpHelpMessage);
                return 0;
            }
        }
            
        gcpFileName = argv[1];
        iResult = LoadFile(gcpFileName);
        printf("%s\n", gcpFileName);

        if(iResult == ERROR_CANNOT_OPEN_FILE){
            printf("新しいファイルです\n");
        }else{
            printf("%d行読み込みました\n", giLineCount);
        }
            
        iResult = Interact();
    }

    for(iCount = 0; iCount < giLineCount; iCount++){
        if(gcppLine[iCount]) free(gcppLine[iCount]);
    }
    if(gcppLine) free(gcppLine);

    if(iResult != SUCCESS){
        printf("%s\n", gcpErrorMessage[iResult]);
    }
    
    return 0;
}

void Append(){
}

void Delete(int start, int end){
}

void Edit(int start){
}

char GetCommandChar(char *command){
    return 0;
}

int GetStart(char *command){
    return 0;
}

int GetEnd(char *command){
    return 0;
}

void Insert(int start){
}

int Interact(){
    return 0;
}

void List(int start, int end){
}

int LoadFile(char *fileName){
    return 0;
}

int Quit(){
    return 0;
}

int SaveFile(char *fileName){
    return 0;
}
