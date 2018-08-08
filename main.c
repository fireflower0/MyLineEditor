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
int  GetStart(char *command);
int  GetEnd(char *command);
void Insert(int start);
int  Interact();
void List(int start, int end);
int  LoadFile(char *fileName);
int  Quit();
int  SaveFile(char *fileName);

int main(int argc, char *argv[]){
    int iCount;
    int iResult = SUCCESS;

    // コマンドライン引数のチェック
    if(argc != 2){
        iResult = ERROR_ARGUMENT_NUMBER;
    }else{
        // 与えられた引数の１文字目が「-」の場合、
        if(argv[1][0] == '-'){
            // 次の１文字に応じた処理を行う
            switch(argv[1][1]){
            case 'v':   // バージョン情報の表示
                printf("edline Version 0.01 2018(c)Fireflower0\n");
                return 0;
            case 'h':   // 起動方法とコマンドの情報を表示
            case '?':
                printf("ラインエディタMyLineEditor\n\n"
                       "起動方法 : MyLineEditor ファイル名\n"
                       "\nコマンド : \n%s", gcpHelpMessage);
                return 0;
            }
        }

        // 引数の内容を取得
        gcpFileName = argv[1];
        iResult = LoadFile(gcpFileName);
        printf("%s:", gcpFileName);

        if(iResult == ERROR_CANNOT_OPEN_FILE){
            printf("新しいファイルです\n");
        }else{
            printf("%d行読み込みました\n", giLineCount);
        }

        // ユーザーとの受け答え
        iResult = Interact();
    }

    // １行ごとのメモリ解放
    for(iCount = 0; iCount < giLineCount; iCount++){
        if(gcppLine[iCount]) free(gcppLine[iCount]);
    }
    // 全行へのポインタが格納された領域を開放
    if(gcppLine) free(gcppLine);

    // エラーだった場合は、エラーメッセージを表示
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
    char carrBuffer[MAX_CHAR];
    char cCurChar = '\0';
    FILE *fp;
    int  iCount = 0;
    char **cppLineInitialValue = NULL;
    int  iResult = SUCCESS;

    // ファイルを開く
    fp = fopen(fileName, "r");

    // ファイルが開けたかチェック
    if(fp == NULL){
        iResult = ERROR_CANNOT_OPEN_FILE;
    }else{
        // ファイルを最後まで読み込んで、行数を取得
        while(NULL != fgets(carrBuffer, MAX_CHAR, fp)){
            giLineCount++;
        }
        
        // ファイルの最初に戻る
        rewind(fp);

        // 文字へのポインタを行数分格納できるメモリ空間を確保
        cppLineInitialValue = (char**)calloc(giLineCount, sizeof(char*));
        // gcppLineにも値を入れる
        gcppLine = cppLineInitialValue;

        while(cCurChar != EOF){
            cCurChar = getc(fp);
            if((cCurChar == '\n') || ((cCurChar == EOF) && (iCount != 0))){
                carrBuffer[iCount] = '\n';
                carrBuffer[iCount + 1] = '\0';
                *gcppLine = (char*)malloc(iCount + 2);   // １行分のメモリ空間を確保
                strcpy(*gcppLine, carrBuffer);
                gcppLine++;                              // 次の行へ進む
                iCount = 0;
            }else{
                carrBuffer[iCount] = cCurChar;
                iCount++;
            }
        }
        if(0 != fclose(fp)){
            iResult = ERROR_CANNOT_CLOSE_FILE;
        }
        // gcppLineの値を最初に戻す
        gcppLine = cppLineInitialValue;
    }
    return iResult;
}

int Quit(){
    return 0;
}

int SaveFile(char *fileName){
    return 0;
}
