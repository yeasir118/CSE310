#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "ScopeTable.h"

class SymbolTable{
public:
    ScopeTable* currentScopeTable;
    int totalBuckets;
    int starting_id;

    SymbolTable(int tB){
        totalBuckets = tB;
        starting_id = 0;
        ScopeTable* newScopeTable = new ScopeTable(totalBuckets, "1", 0);
        currentScopeTable = newScopeTable;
    }

    void enterScope(){
        ScopeTable* newScopeTable = new ScopeTable(totalBuckets, currentScopeTable->id, ++(currentScopeTable->no_children_scope_tables));
        newScopeTable->parentScope = currentScopeTable;
        currentScopeTable = newScopeTable;
    }

    void exitScope(){
        if(currentScopeTable == nullptr){
            // cout<<"No scope to exit"<<endl;
            return;
        }
        // cout<<endl<<"ScopeTable with id "<<currentScopeTable->id<<" removed"<<endl;
        ScopeTable* temp = currentScopeTable;
        currentScopeTable = currentScopeTable->parentScope;
        delete temp;
    }

    // true -> inserted successfully
    // false -> already exists or not inserted
    // only inserts in the current ScopeTable
    bool insert(string name, string type){
        if(currentScopeTable == nullptr){
            // cout<<"No scope to insert"<<endl;
            return false;
        }
        return currentScopeTable->insert(name, type);
    }

    // true -> deleted successfully
    // false -> not found or not deleted
    bool remove(string name){
        if(currentScopeTable == nullptr){
            // cout<<"No scope to remove"<<endl;
            return false;
        }
        return currentScopeTable->del(name);
    }

    SymbolInfo* lookUp(string name, string type){
        if(currentScopeTable == nullptr){
            // cout<<"No scope to look up"<<endl;
            return nullptr;
        }
        ScopeTable* temp = currentScopeTable;
        while(temp != nullptr){
            SymbolInfo* result = temp->lookUp(name, type);
            if(result != nullptr) return result;
            temp = temp->parentScope;
        }
        return nullptr;
    }

    string printCurrentScopeTable(){
        // if(currentScopeTable == nullptr){
        //     cout<<"No scope to print"<<endl;
        //     return;
        // }
        // currentScopeTable->printScopeTable();

        string out;
        if(currentScopeTable == nullptr){
            out += "No scope to print\n";
        }
        else out += currentScopeTable->printScopeTable();
        return out;
    }

    string printAllScopeTables(){
        // if(currentScopeTable == nullptr){
        //     cout<<"No scope to print"<<endl;
        //     return;
        // }
        // ScopeTable* temp = currentScopeTable;
        // while(temp != nullptr){
        //     temp->printScopeTable();
        //     temp = temp->parentScope;
        // }

        string out;
        if(currentScopeTable == nullptr){
            out += "No scope to print\n";
        }else{
            ScopeTable* temp = currentScopeTable;
            while(temp != nullptr){
                out += temp->printScopeTable();
                temp = temp->parentScope;
            }
        }
        return out;
    }

    void handleInput(){
        ifstream file("input_table.txt");
        string input;
        bool firstLine = true;
        while(getline(file, input)){
            cout<<endl<<input<<endl;
            stringstream ss_input(input);
            if(firstLine){
                ss_input>>(this->totalBuckets);
                ScopeTable* newScopeTable = new ScopeTable(totalBuckets, "1", 0);
                currentScopeTable = newScopeTable;

                firstLine = false;
            }else{
                vector<string> inputTokens;
                string temp;
                while(getline(ss_input, temp, ' ')){
                    inputTokens.push_back(temp);
                }

                string currentInput = inputTokens[0];

                if(currentInput[0] == 'I'){
                    string name = inputTokens[1];
                    string type = inputTokens[2];
                    insert(name, type);
                }else if(currentInput[0] == 'L'){
                    string name = inputTokens[1];
                    SymbolInfo* result = lookUp(name,"no_type");
                    if(result == nullptr){
                        cout<<endl<<"Not found"<<endl;
                    }
                }else if(currentInput[0] == 'D'){
                    string name = inputTokens[1];
                    remove(name);
                }else if(currentInput[0] == 'P'){
                    if(inputTokens[1][0] == 'A'){
                        printAllScopeTables();
                    }else{
                        printCurrentScopeTable();
                    }
                }else if(currentInput[0] == 'S'){
                    enterScope();
                }else if(currentInput[0] == 'E'){
                    exitScope();
                }
            }
        }
    }

    ~SymbolTable(){
        // cout<<endl<<"SymbolTable deleted"<<endl;
        while(currentScopeTable != nullptr){
            ScopeTable* temp = currentScopeTable;
            currentScopeTable = currentScopeTable->parentScope;
            delete temp;
        }
    }
};

#endif