#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include "bits/stdc++.h"
using namespace std;

class SymbolInfo{
    string name;
    string type;
public:
    SymbolInfo *next;
    int pos_in_scope_table;
    int pos_in_bucket;
    SymbolInfo(string name, string type){
        this->name = name;
        this->type = type;
        next = nullptr;
        pos_in_scope_table = -1;
        pos_in_bucket = -1;
    }
    string getName(){
        return name;
    }
    string getType(){
        return type;
    }
    void setName(string name){
        this->name = name;
    }
    void setType(string type){
        this->type = type;
    }

    ~SymbolInfo(){
        // cout<<"Destructor for "<<name<<endl;
        next = nullptr;
    }
};

#endif