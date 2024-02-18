#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <WinUser.h>
using namespace std;
typedef struct line line;

struct line{
    //line* next;
    //line* previous;
    vector<char> buffer;
};
int load_file(string filename, vector<line>& bufferVector);
int write_to_file(string filename, vector<line>& bufferVector);
int drawEverything(int current_line_index, vector<line>& bufferVector){
    printf("\033[2J");
    int i ;
    for(i = 0; i < bufferVector.size(); i++){
        line current = bufferVector[i];
        int j;
        if(i == current_line_index)
            printf("> ");
        for(j = 0; j < bufferVector.at(i).buffer.size(); j++){
            printf("%c", bufferVector.at(i).buffer.at(j));

            
        }
        printf("\n");
    }
    return 0;
}
int drawLine(line& line){
    printf("\033[2K\r> ");    
    for(int i = 0; i < line.buffer.size(); i++)
        putchar(line.buffer.at(i));
    return 0;
}
int main(int argc, char* argv[]){

    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(hInput, 0x200);
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleMode(hOutput, 0x0001 | 0x0004);
    string inputFile = "test.txt";
    if(argc != 1){
        inputFile = argv[1];
    }
    int current_line_index = 0;
    int current_col_index = 0;
    //printf("\033[2J");
    //printf("%c[%dmHELLO!\n", 0x1B, 41);
    //printf("%c[%dmMEOW!!!!!\n", 0x1B, 97);
    //system("cls");
    //system("cmd.exe /c dir C:\\");
    char a = 0;
    vector<line> bufferV;
    if (load_file(inputFile, bufferV) != 0){
        perror("error in loading file");
    }
    current_line_index = bufferV.size()-1;
    current_col_index = bufferV.at(current_line_index).buffer.size()-1;
    line current_line = bufferV.at(current_line_index);
    int i = 0;
    drawEverything(current_line_index, bufferV);

    while(a != -1){
        a = getchar();
        if(a==0){
            a = -1;
        }
        else if(a == 13){
            vector<char> newLineBuffer;
            if(current_col_index < current_line.buffer.size()){
                current_line.buffer.at(current_col_index) = a;
                for(int i = current_col_index+1; i < current_line.buffer.size(); i++)
                    newLineBuffer.push_back(current_line.buffer.at(current_col_index));
                for(int i = current_col_index+1; i < current_line.buffer.size(); i++)
                    current_line.buffer.pop_back();
            }
            else
                current_line.buffer.push_back(a);

            line newLine;     
            newLine.buffer = newLineBuffer;
            bufferV.push_back(newLine);
            if(current_line_index != bufferV.size()-2){
                for(int i = bufferV.size()-2; i > current_line_index; i--){
                    bufferV[i+1] = bufferV[i];
                }
                bufferV.at(current_line_index+1) = newLine;
            }
            bufferV.at(current_line_index) = current_line;
            current_line_index++;
            current_line = bufferV.at(current_line_index);
            
            drawEverything(current_line_index, bufferV);
            printf("\n");
        }
        else if (a == 19){
            printf("", write_to_file(inputFile, bufferV));
        }
        else if(a == 27){
            char b = getchar();
            if(b == 91){
                char c = getchar();
                if(c == 65){
                    //printf("up");
                    //printf("\033[<16>A");

                }
                else if(c == 66){
                    //printf("down");
                    //printf("\033[<16>B");
                }
                else if(c == 67){
                    //printf("left");
                    //printf("\033[<16>C");
                    }
                else if (c == 68){
                    //printf("right");
                    //printf("\033[<16>D");
                }
            }
        }
        else if(a == 127){

            if(current_line.buffer.size() == 0){
                if(current_line_index == 0)
                    continue;
                for(int i = current_line_index; i < bufferV.size()-1; i++)
                    bufferV.at(i) = bufferV.at(i-1);
                bufferV.at(bufferV.size()-1) = current_line;
                bufferV.pop_back();
                current_line_index--;
                current_line = bufferV.at(current_line_index);

                drawEverything(current_line_index, bufferV);
            }
            else{
                for(int i = current_col_index; i < current_line.buffer.size()-1; i++)
                    current_line.buffer.at(i) = current_line.buffer.at(i+1);
                current_line.buffer.pop_back();
                bufferV.at(current_line_index) = current_line;
                drawLine(current_line);
            }
        }

        else{
            if(current_col_index < current_line.buffer.size())
                current_line.buffer.at(current_col_index) = a;
            else
                current_line.buffer.push_back(a);
            drawLine(current_line);
            bufferV.at(current_line_index) = current_line;
            current_col_index++;
        }
        
    }
    return 0;
}


int load_file(string filename, vector<line>& bufferVector){
    FILE* inputFile = fopen(filename.c_str(), "r");
    if(!inputFile){
        perror("File opening failed");
        return 1;
    }
    int i = 0;
    int j = 0;
    line start;
    bufferVector.push_back(start);
    while(feof(inputFile) == 0){
        char currentChar = getc(inputFile);
       
        bufferVector.at(i).buffer.push_back(currentChar);
        j++;
        if (currentChar == '\n'){
            line a;
            //bufferVector.at(bufferVector.size()-1).next = &a;
            //a.previous = &(bufferVector.at(bufferVector.size()-1));
            bufferVector.push_back(a);
            i++;
            j = 0;
        }
    }
    fclose(inputFile);
    return 0;
}
int write_to_file(string filename, vector<line>& vector_buffer){
    FILE* outputFile = fopen(filename.c_str(), "w");
    if(!outputFile){
        perror("Error in opening outputFile");
        return 1;
    }
    for(int i = 0; i < vector_buffer.size(); i++){
        for(int j = 0; j < vector_buffer.at(i).buffer.size(); j++){
            fputc(vector_buffer.at(i).buffer.at(j), outputFile);
            //printf("%c", vector_buffer.at(i).buffer.at(j));
        }
    }
    fclose(outputFile);
    return 0;
}