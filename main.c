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
char *my_gets(char* s);

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

// ユーザーが入力したコマンド文字列から、コマンドの種別を表す１文字を抜き出す関数
// 例：「1-3l」から「l」を抜き出す
char GetCommandChar(char *command){
    char carrCommandChar[] = "adhilqADHILQ?"; // コマンド文字と解釈すべき文字の一覧
    unsigned long ulCount;

    for(ulCount = 0; ulCount < strlen(carrCommandChar); ulCount++){
        if(NULL != strchr(command, carrCommandChar[ulCount])){
            return tolower(carrCommandChar[ulCount]);
        }
    }
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
    char carrCommand[MAX_CHAR];  // ユーザーが入力したコマンドを文字列を一時保管する変数
    char cCommandChar;           // コマンドの種別を示す変数
    int iStart, iEnd;            // ユーザーが入力したコマンド操作の開始行と終了行を格納する変数

    for(;;){
        printf("USER >> ");
        if(NULL == my_gets(carrCommand)) continue;
        if(strcmp(carrCommand, "") != -1){
            cCommandChar = GetCommandChar(carrCommand);
            iStart       = GetStart(carrCommand);
            iEnd         = GetEnd(carrCommand);

            switch(cCommandChar){
            case 'q':
                return Quit();
            case 'l':
                if(iStart == NOT_SET) iStart = 1;
                if(iEnd   == NOT_SET) iEnd   = giLineCount;
                List(iStart, iEnd);
                break;
            case 'a':
                Append();
                break;
            case 'i':
                if(iStart == NOT_SET){
                    Append();
                }else{
                    Insert(iStart);
                }
                break;
            case 'd':
                if(iStart == NOT_SET){
                    printf("行を指定して下さい\n");
                }else{
                    if(iEnd == NOT_SET) iEnd = iStart;
                    Delete(iStart, iEnd);
                }
                break;
            case 'h':
            case '?':
                printf("%s", gcpHelpMessage);
                break;
            default:
                if(iStart != NOT_SET) Edit(iStart);
            }
        }
    }
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

// gets関数の自作代替関数
char *my_gets(char* s){
    char *cpLn;
    int  iChar;

    // fgets : ストリーム (stream) から 1 行単位で文字列を読み取る
    if(fgets(s, MAX_CHAR, stdin) == NULL) return NULL;

    // 改行文字を検索
    cpLn = strchr(s, '\n');

    if(cpLn){
        // 改行文字があった場合、終端文字に置き換える
        *cpLn = '\0';
    }else{
        // 入力ストリーム上に文字が残ってる場合、改行文字が読み取られるまで空読みする
        for(;;){
            // getchar : 標準入力 (standard input) から 1 文字単位で文字を読み取る
            iChar = getchar();
            if(iChar == '\n' || iChar == EOF) break;
        }
    }
    return s;
}
