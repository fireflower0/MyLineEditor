#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR 65536
#define FORMAT "%5ld:%s"
#define NOT_SET -1UL
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
    "a : 追加(行頭のCtrl+z(Windows)または、Ctrl+d(Ubuntu)で終了)\n"
    "d : 削除(「1d」「1-2d」のように行指定)\n"
    "i : 挿入(「1i」のように行指定、行頭のCtrl+z(Windows)または、Ctrl+d(Ubuntu)で終了)\n"
    "l : 表示(「2l」「2-5l」のように行指定)\n"
    "q : 終了\n"
    "(行番号) : 編集\n";

char *gcpFileName;
char **gcppLine = NULL;
unsigned long  gulLineCount = 0;

void Append();
void Delete(unsigned long start, unsigned long end);
void Edit(unsigned long start);
char GetCommandChar(char *command);
unsigned long  GetStart(char *command);
unsigned long  GetEnd(char *command);
void Insert(unsigned long start);
int  Interact();
void List(unsigned long start, unsigned long end);
int  LoadFile(char *fileName);
int  Quit();
int  SaveFile(char *fileName);
char *my_gets(char* s);

int main(int argc, char *argv[]){
    unsigned long iCount;
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
            printf("%ld行読み込みました\n", gulLineCount);
        }

        // ユーザーとの受け答え
        iResult = Interact();
    }

    // １行ごとのメモリ解放
    for(iCount = 0; iCount < gulLineCount; iCount++){
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

unsigned long GetStart(char *command){
    char carrDigit[11];   // 開始行の数値を構成する文字を格納する変数
                          // ※ 32ビット整数の上限が10桁なので、配列の大きさを11に設定
    unsigned long ulCount1, ulResult; // commandの何文字目を処理しているかを格納するカウンタ変数
    unsigned long ulCount2 = 0;       // 変数carrDigitの何文字目を処理しているかを格納するカウンタ変数

    // 1文字目が数値かチェック
    if(!isdigit(command[0])) return NOT_SET;

    // 1文字目から数値でない文字が出てくるまでループ
    for(ulCount1 = 0; ulCount1 < strlen(command); ulCount1++){
        if(isdigit(command[ulCount1])){
            carrDigit[ulCount2] = command[ulCount1];
            ulCount2++;
        }else{
            break;
        }
    }

    // 最後にNULL文字を入れる
    carrDigit[ulCount2] = '\0';

    // 文字列を数値に変換
    ulResult = atoi(carrDigit);
    
    if(ulResult < 1) return 1;
    if(ulResult > gulLineCount) return gulLineCount;
    return ulResult;
}

unsigned long GetEnd(char *command){
    char carrDigit[11];
    unsigned long ulCount1, ulResult;
    unsigned long ulCount2 = 0;
    char *cpStart;

    // 「-」以降のアドレスを取得
    cpStart = strchr(command, '-');

    // 「-」が見つからない場合はNOT_SETを返して終了
    if(cpStart == NULL) return NOT_SET;

    for(ulCount1 = 1; ulCount1 < strlen(cpStart); ulCount1++){
        if(isdigit(cpStart[ulCount1])){
            carrDigit[ulCount2] = cpStart[ulCount1];
            ulCount2++;
        }else{
            break;
        }
    }

    if(ulCount2 == 0){
        // 「1-l」のように入力された場合もNOT_SETを返す
        return NOT_SET;
    }else{
        carrDigit[ulCount2] = '\0';
        ulResult = atoi(carrDigit);
        if(ulResult < 1) return 1;
        if(ulResult > gulLineCount) return gulLineCount;
        return ulResult;
    }
}

int Interact(){
    char carrCommand[MAX_CHAR];  // ユーザーが入力したコマンドを文字列を一時保管する変数
    char cCommandChar;           // コマンドの種別を示す変数
    unsigned long iStart, iEnd;            // ユーザーが入力したコマンド操作の開始行と終了行を格納する変数

    for(;;){
        printf("%s", "USER >> ");
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
                if(iEnd   == NOT_SET) iEnd   = gulLineCount;
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

void List(unsigned long start, unsigned long end){
    unsigned long iCount;

    if(gulLineCount > 0){
        for(iCount = start; iCount <= end; iCount++){
            printf(FORMAT, iCount, gcppLine[iCount - 1]);
        }
    }else{
        printf("表示できる行がありません\n");
    }
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
            gulLineCount++;
        }
        
        // ファイルの最初に戻る
        rewind(fp);

        // 文字へのポインタを行数分格納できるメモリ空間を確保
        cppLineInitialValue = (char**)calloc(gulLineCount, sizeof(char*));
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

/***** 編集に関する処理 *****/

// 編集処理
void Edit(unsigned long start){
    char carrLine[MAX_CHAR];   // 一時的に行のデータを格納する変数

    if(gulLineCount > 0){
        printf(FORMAT, start, gcppLine[start - 1]);   // 行番号と現在の内容を表示
        printf(FORMAT, start, "");                    // 新しい内容の入力を促す
        if(NULL != my_gets(carrLine)){
            strcat(carrLine, "\n");                   // 入力された文字列に改行文字を付加
            free(gcppLine[start - 1]);                // これまで行の内容を保持していたメモリを開放
            gcppLine[start - 1] = (char*)malloc(strlen(carrLine) + 1); // 入力された文字列の文字数に合わせて新たにメモリ確保
            strcpy(gcppLine[start - 1], carrLine);    // 新しいメモリにコピー
        }
    }else{
        printf("編集できる行がありません\n");
    }
}

// 末尾に行を追加する処理
void Append(){
    unsigned long iCount;
    char carrLine[MAX_CHAR];
    char **cppOldLine = NULL;   // 現在使っているメモリ空間を一時的に保持する変数

    // ユーザーに入力を促す
    printf(FORMAT, gulLineCount + 1, "");

    // ユーザーがEOF(Ctrl+z[Windows], Ctrl+d[Ubuntu])を入力するまでループ
    while(NULL != my_gets(carrLine)){
        strcat(carrLine, "\n");
        gulLineCount++;         // 1行追加されたら、インクリメントする

        // 現在の行の情報をチェック
        if(!gcppLine){
            // 行の情報がNULLだったらメモリを確保
            gcppLine = (char**)calloc(gulLineCount, sizeof(char*));
        }else{
            // 行の情報が何かしら入っていたら一時退避し、
            cppOldLine = gcppLine;
            // 1行分大きいメモリ領域を確保する
            gcppLine = (char**)calloc(gulLineCount, sizeof(char*));
            // 退避した既存の情報を新しいメモリへコピー
            for(iCount = 0; iCount < gulLineCount; iCount++){
                gcppLine[iCount] = cppOldLine[iCount];
            }
            // 古いメモリ領域は開放
            free(cppOldLine);
        }

        // ユーザーが今回入力した行のためのメモリを確保
        gcppLine[gulLineCount - 1] = (char*)malloc(strlen(carrLine) + 1);
        // 入力された文字列を新しいメモリへコピー
        strcpy(gcppLine[gulLineCount - 1], carrLine);

        // 次の行の入力を促す
        printf(FORMAT, gulLineCount + 1, "");
    }
    printf("\n");
}

// 挿入処理
void Insert(unsigned long start){
    unsigned long ulCount;
    char carrLine[MAX_CHAR];
    char **cppOldLine = NULL;

    if(gulLineCount > 0){
        printf(FORMAT, start, "");

        // ユーザーがEOF(Ctrl+z[Windows], Ctrl+d[Ubuntu])を入力するまでループ
        while(NULL != my_gets(carrLine)){
            strcat(carrLine, "\n");
            gulLineCount++;

            // 行の情報を退避
            cppOldLine = gcppLine;

            // 1行分大きいメモリ領域を確保
            gcppLine = (char**)calloc(gulLineCount, sizeof(char*));

            // 挿入する行より前の行を新しいメモリへコピーする
            for(ulCount = 0; ulCount < start; ulCount++){
                gcppLine[ulCount] = cppOldLine[ulCount];
            }

            // 新しい行のメモリを確保して、入力された文字列を新しいメモリへコピー
            gcppLine[start - 1] = (char*)malloc(strlen(carrLine) + 1);
            strcpy(gcppLine[start - 1], carrLine);

            // 残りの行を新しいメモリへコピー
            for(ulCount = start; ulCount < gulLineCount; ulCount++){
                gcppLine[ulCount] = cppOldLine[ulCount - 1];
            }

            // 古いメモリ領域は開放
            free(cppOldLine);

            start++;

            // 次の行の入力を促す
            printf(FORMAT, start, "");
        }
        printf("\n");
    }else{
        // 現在の行数が0だったらAppend関数を呼び出す
        Append();
    }
}

// 削除処理
void Delete(unsigned long start, unsigned long end){
    unsigned long ulCount;
    unsigned long ulHowManyLines = end - start + 1;
    char **cppOldLine = NULL;

    if(ulHowManyLines > 0){
        if(gulLineCount > 0){
            gulLineCount = gulLineCount - ulHowManyLines;
            cppOldLine = gcppLine;

            if(gulLineCount == 0){
                // 行数が0になる場合は、NULLを格納
                gcppLine = NULL;
            }else{
                // 削除された行数分小さくなったメモリ領域を確保
                gcppLine = (char**)calloc(gulLineCount, sizeof(char*));
            }

            // 削除する行の前までを新しいメモリへコピー
            for(ulCount = 0; ulCount < start - 1; ulCount++){
                gcppLine[ulCount] = cppOldLine[ulCount];
            }

            // 削除する行が入っているメモリを開放
            for(ulCount = start - 1; ulCount < end; ulCount++){
                free(cppOldLine[ulCount]);
            }

            // 残りの行を新しいメモリへコピー
            for(ulCount = start - 1; ulCount < gulLineCount; ulCount++){
                gcppLine[ulCount] = cppOldLine[ulCount + ulHowManyLines];
            }

            // 古いメモリ領域は開放
            free(cppOldLine);
        }else{
            printf("削除できる行がありません\n");
        }
    }
}

/***** ユーティリティ *****/

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
