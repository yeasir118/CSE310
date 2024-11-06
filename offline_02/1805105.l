%option noyywrap

%x STRING_STATE
%x CHAR_STATE
%x SINGLE_LINE_COMMENT_STATE
%x MULTI_LINE_COMMENT_STATE

%{
#include<bits/stdc++.h>
#include "SymbolTable.h"
#define BUCKET_SIZE 7

using namespace std;

SymbolTable symbolTable(BUCKET_SIZE);

ofstream tokenout("token.txt");
ofstream logout("log.txt");

int linecount = 1;
int errorcount = 0;
int state_start_line = 0;

string string_constant;
string char_constant;
string single_line_comment_constant;
string multi_line_comment_constant;

bool multiline_ok = false;

void write_to_log(string token, string lexeme){
    logout<<"\nLine no "<<linecount<<": Token <"<<token<<"> Lexeme "<<lexeme<<" found\n";
};
%}

%%
[ \t\f\r\v]+ {}
[\n] {linecount++;}
if|else|for|while|do|break|int|char|float|double|void|return|switch|case|default|continue {
        tokenout<<"<"<<yytext<<">";
        write_to_log("KEYWORD",yytext);
    }
[0-9]*[.][0-9]*([.][0-9]*)+ {
        logout<<"\nError at line no "<<linecount<<": Too many decimal points "<<yytext<<"\n";
        errorcount++;
    }
[0-9]*[.][0-9]+[eE][+-]?[0-9]*[.][0-9]* {
        logout<<"\nError at line no "<<linecount<<": Ill formed number "<<yytext<<"\n";
        errorcount++;
    }
[0-9]*[.][0-9]+([eE][+-][0-9]+)? {
        tokenout<<"<CONST_FLOAT,"<<yytext<<">";
        write_to_log("CONST_FLOAT",yytext);
        if(symbolTable.insert(yytext, "CONST_FLOAT")){
            logout<<symbolTable.printAllScopeTables();
        }else{
            logout<<endl<<"<"<<yytext<<",CONST_FLOAT"<<"> already exists in current ScopeTable"<<endl;
        }
    }
[0-9]+[eE][+-]?[0-9]*[.][0-9]* {
        logout<<"\nError at line no "<<linecount<<": Ill formed number "<<yytext<<"\n";
        errorcount++;
    }
[0-9]+[eE][+-]?[0-9]+ {
        tokenout<<"<CONST_FLOAT,"<<yytext<<">";
        write_to_log("CONST_FLOAT",yytext);
        if(symbolTable.insert(yytext, "CONST_FLOAT")){
            logout<<symbolTable.printAllScopeTables();
        }else{
            logout<<endl<<"<"<<yytext<<",CONST_FLOAT"<<"> already exists in current ScopeTable"<<endl;
        }
    }
[0-9]+[_a-zA-Z]+ {
        logout<<"\nError at line no "<<linecount<<": Invalid prefix on ID or invalid suffix on Number "<<yytext<<"\n";
        errorcount++;  
    }
[0-9]+ {
        tokenout<<"<CONST_INT,"<<yytext<<">";
        write_to_log("CONST_INT",yytext);
        if(symbolTable.insert(yytext, "CONST_INT")){
            logout<<symbolTable.printAllScopeTables();
        }else{
            logout<<endl<<"<"<<yytext<<",CONST_INT"<<"> already exists in current ScopeTable"<<endl;
        }
    }
\' {
        char_constant = "";
        BEGIN CHAR_STATE;
    }
<CHAR_STATE>(\\n)|(\\t)|(\\\\)|(\\\')|(\\a)|(\\f)|(\\r)|(\\b)|(\\v)|(\\0) {
    char_constant += yytext;
}
<CHAR_STATE>\' {
        if(char_constant.length()==0){
            logout<<"\nError at line no "<<linecount<<": Empty character constant error '"<<char_constant<<"'\n";
            errorcount++; 
        }else if(char_constant.length()==1 || (char_constant.length()==2 && char_constant[0]=='\\')){
            tokenout<<"<CONST_CHAR,"<<char_constant<<">";
            string temp(1,'\'');
            temp += char_constant + '\'';
            write_to_log("CONST_CHAR",temp);
            if(symbolTable.insert(temp, "CONST_CHAR")){
                logout<<symbolTable.printAllScopeTables();
            }else{
                logout<<endl<<"<"<<yytext<<",CONST_CHAR"<<"> already exists in current ScopeTable"<<endl;
            }
        }else{
            logout<<"\nError at line no "<<linecount<<": Multi character constant error '"<<char_constant<<"'\n";
            errorcount++; 
        }
        BEGIN INITIAL;
    }
<CHAR_STATE>[\n] {
        logout<<"\nError at line no "<<linecount<<": Unterminated character '"<<char_constant<<"\n";
        errorcount++; 
        linecount++;
        BEGIN INITIAL;
    }
<CHAR_STATE><<EOF>> {
        logout<<"\nError at line no "<<linecount<<": Unterminated character '"<<char_constant<<"\n";
        errorcount++; 
        BEGIN INITIAL;
    }
<CHAR_STATE>. {
        char_constant += yytext;
    }
[*]|[/]|[%] {
        tokenout<<"<MULOP,"<<yytext<<">";
        write_to_log("MULOP",yytext);
    }
[+]{2}|[-]{2} {
        tokenout<<"<INCOP,"<<yytext<<">";
        write_to_log("INCOP",yytext);
    }
[<>]|(<=)|(>=)|(==)|(!=) {
        tokenout<<"<RELOP,"<<yytext<<">";
        write_to_log("RELOP",yytext);
    }
[=] {
        tokenout<<"<ASSIGNOP,"<<yytext<<">";
        write_to_log("ASSIGNOP",yytext);
    }
(&&)|(\|\|) {
        tokenout<<"<LOGICOP,"<<yytext<<">";
        write_to_log("LOGICOP",yytext);
    }
[!] {
        tokenout<<"<NOT,"<<yytext<<">";
        write_to_log("NOT",yytext);
    }
[(] {
        tokenout<<"<LPAREN,"<<yytext<<">";
        write_to_log("LPAREN",yytext);
    }
[)] {
        tokenout<<"<RPAREN,"<<yytext<<">";
        write_to_log("RPAREN",yytext);
    }
[\{] {
        tokenout<<"<LCURL,"<<yytext<<">";
        write_to_log("LCURL",yytext);
        symbolTable.enterScope();
    }
[\}] {
        tokenout<<"<RCURL,"<<yytext<<">";
        write_to_log("RCURL",yytext);
        symbolTable.exitScope();
    }
(\[) {
        tokenout<<"<LTHIRD,"<<yytext<<">";
        write_to_log("LTHIRD",yytext);
    }
(\]) {
        tokenout<<"<RTHIRD,"<<yytext<<">";
        write_to_log("RTHIRD",yytext);
    }
[,] {
        tokenout<<"<COMMA,"<<yytext<<">";
        write_to_log("COMMA",yytext);
    }
[;] {
        tokenout<<"<SEMICOLON,"<<yytext<<">";
        write_to_log("SEMICOLON",yytext);
    }
[+]{1}|[-]{1} {
        tokenout<<"<ADDOP,"<<yytext<<">";
        write_to_log("ADDOP",yytext);
    }
[a-zA-Z_]{1}[a-zA-Z_0-9]* {
        tokenout<<"<ID,"<<yytext<<">";
        write_to_log("ID",yytext);
        if(symbolTable.insert(yytext, "ID")){
            logout<<symbolTable.printAllScopeTables();
        }else{
            logout<<endl<<"<"<<yytext<<",ID"<<"> already exists in current ScopeTable"<<endl;
        }
    }
\" {
        string_constant="";
        multiline_ok=false;
        state_start_line = linecount;
        BEGIN STRING_STATE;
    }
<STRING_STATE>(\\t)|(\\\\)|(\\\')|(\\a)|(\\f)|(\\r)|(\\b)|(\\v)|(\\0)|(\\\") {
        string_constant += yytext;
    }
<STRING_STATE>[\\]/[\n] {
        multiline_ok=true;
        string_constant += yytext;
    }
<STRING_STATE>[\n] {
        if(!multiline_ok){
            logout<<"\nError at line no "<<state_start_line<<": Unterminated String \""<<string_constant<<"\n";
            errorcount++;
            BEGIN INITIAL;
        }else{
            string_constant += "\n";
            multiline_ok=false;
        }
        linecount++;
    }
<STRING_STATE>\" {
        tokenout<<"<STRING,"<<string_constant<<">";
        string temp(1,'\"');
        temp += string_constant + '\"'; 
        write_to_log("STRING",temp);
        BEGIN INITIAL;
    }
<STRING_STATE><<EOF>> {
        logout<<"\nError at line no "<<state_start_line<<": Unterminated String \""<<string_constant<<"\n";
        errorcount++;
        BEGIN INITIAL;
    }
<STRING_STATE>. {
        string_constant+=yytext;
    }
\/\/ {
    single_line_comment_constant = "";
    multiline_ok=false;
    state_start_line = linecount;
    BEGIN SINGLE_LINE_COMMENT_STATE;
}
<SINGLE_LINE_COMMENT_STATE>[\\]/[\n] {
        multiline_ok=true;
        single_line_comment_constant += yytext;
    }
<SINGLE_LINE_COMMENT_STATE>[\n] {
        if(!multiline_ok){
            write_to_log("COMMENT",single_line_comment_constant);
            BEGIN INITIAL;
        }else{
            single_line_comment_constant += "\n";
            multiline_ok=false;
        }
        linecount++;
    }
<SINGLE_LINE_COMMENT_STATE><<EOF>> {
        write_to_log("COMMENT",single_line_comment_constant);
        BEGIN INITIAL;
    }
<SINGLE_LINE_COMMENT_STATE>. {
    single_line_comment_constant += yytext;
}
\/\* {
        multi_line_comment_constant = "";
        state_start_line = linecount;
        BEGIN MULTI_LINE_COMMENT_STATE;
    }
<MULTI_LINE_COMMENT_STATE>\*\/ {
        string temp;
        temp += "/*" + multi_line_comment_constant + "*/"; 
        write_to_log("COMMENT",temp);
        BEGIN INITIAL;
    }
<MULTI_LINE_COMMENT_STATE>[\n] {
        multi_line_comment_constant += '\n';
        linecount++;
    }
<MULTI_LINE_COMMENT_STATE><<EOF>> {
        logout<<"\nError at line no "<<state_start_line<<": Unterminated Comment "<<multi_line_comment_constant<<"\n";
        errorcount++; 
        BEGIN INITIAL;
    }
<MULTI_LINE_COMMENT_STATE>. {
        multi_line_comment_constant += yytext;
    }
<<EOF>> {logout<<"\nTotal lines: "<<linecount<<"\nTotal errors: "<<errorcount;return 0;}
. {
        logout<<"\nError at line no "<<linecount<<": Unrecognized character "<<yytext<<"\n";
        errorcount++;
    }
%%
int main(int argc, char* argv[]){
    if(argc!=2){
        printf("argument error!\n");
        return 0;
    }

    yyin = fopen(argv[1], "r");

    yylex();

    fclose(yyin);
    tokenout.close();
    logout.close();
    return 0;
}