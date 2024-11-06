#include "bits/stdc++.h" 
// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <vector>
// #include <string>
using namespace std;

// SymbolInfo class
// name -> name of the variable
// type -> type of the variable
// next -> pointer to the next SymbolInfo object in the same bucket
// pos_in_scope_table -> position of the SymbolInfo object in the ScopeTable
// pos_in_bucket -> position of the SymbolInfo object in the bucket at a particular index
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

// hash function
unsigned int sdbm(string name){
    unsigned int hash = 0;
    for(int i=0;name[i];i++){
        hash = name[i] + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

// ScopeTable class
// arr -> array of SymbolInfo pointers
// totalBuckets -> total number of buckets in the ScopeTable
// parentScope -> pointer to the parent ScopeTable
// no_children_scope_tables -> number of children ScopeTables
// id -> id of the ScopeTable
class ScopeTable{
    vector<SymbolInfo*> arr;
    int totalBuckets;
public:
    ScopeTable* parentScope;
    int no_children_scope_tables;
    string id;

    ScopeTable(int n, string parent_id, int id){
        arr = vector<SymbolInfo*>(n, nullptr);
        totalBuckets = n;
        parentScope = nullptr;

        if(id!=0) this->id = parent_id + "." + to_string(id);
        else this->id = parent_id;

        no_children_scope_tables = 0;

        cout<<endl<<"New ScopeTable with id "<<this->id<<" created"<<endl;
    }

    // SymbolInfo* -> found SymbolInfo object
    // nullptr -> not found
    SymbolInfo* lookUp(string name){
        int index = sdbm(name) % totalBuckets;
        SymbolInfo* temp = arr[index];

        while(temp != nullptr){
            if(temp->getName() == name) {
                cout<<endl<<"Found in ScopeTable# "<<id<<" at position "<<index<<", "<<temp->pos_in_bucket<<endl;
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }

    // true -> inserted successfully
    // false -> already exists or not inserted
    bool insert(string name, string type){
        int index = sdbm(name) % totalBuckets;
        SymbolInfo* lookUpResult = lookUp(name);
        
        if(lookUpResult != nullptr){
            cout<<endl<<"<"<<name<<","<<type<<"> already exists in current ScopeTable"<<endl;
            return false;
        }

        SymbolInfo* newSymbolInfo = new SymbolInfo(name, type);
        SymbolInfo* temp = arr[index];
        newSymbolInfo->pos_in_scope_table = index;
        if(temp == nullptr){
            arr[index] = newSymbolInfo;
            newSymbolInfo->pos_in_bucket = 0;
        }else{
            int pos=1;
            while(temp->next != nullptr){
                temp = temp->next;
                pos++;
            }
            newSymbolInfo->pos_in_bucket = pos;
            temp->next = newSymbolInfo;
        }
        cout<<endl<<"Inserted in ScopeTable# "<<id<<" at position "<<index<<", "<<newSymbolInfo->pos_in_bucket<<endl;
        return true;
    }

    // true -> deleted successfully
    // false -> not found or not deleted
    bool del(string name){
        int index = sdbm(name)%totalBuckets;
        bool found = false;

        SymbolInfo* temp = arr[index];
        SymbolInfo* prev = nullptr;

        while(temp != nullptr){
            if(temp->getName() == name){
                found = true;
                break;
            }
            prev = temp;
            temp = temp->next;
        }

        if(!found){
            cout<<"Not found"<<endl;
            return false;
        }

        cout<<endl<<"Found in ScopeTable# "<<id<<" at position "<<index<<", "<<temp->pos_in_bucket<<endl;
        if(prev == nullptr){
            arr[index] = arr[index]->next;
        }else{
            prev->next = temp->next;
        }
        temp->next = nullptr;

        cout<<endl<<"Deleted Entry at "<<index<<", "<<temp->pos_in_bucket<<" from current ScopeTable"<<endl;

        delete temp;
        temp = nullptr;

        return true;
    }

    void printScopeTable(){
        cout<<endl<<"ScopeTable #"<<id<<endl;
        for(int i=0;i<totalBuckets;i++){
            cout<<i<<" --> ";
            SymbolInfo* temp = arr[i];
            while(temp != nullptr){
                string name = temp->getName();
                string type = temp->getType();
                cout<<"< "<<name<<" : "<<type<<"> ";
                temp = temp->next;
            }
            cout<<endl;
        }
    }

    ~ScopeTable(){
        // cout<<"ScopeTable #"<<id<<" deleted"<<endl;
        for (int i = 0; i < totalBuckets; i++) {
            SymbolInfo* temp = arr[i];
            while (temp != nullptr) {
                SymbolInfo* toDelete = temp;
                temp = temp->next;
                delete toDelete;
                toDelete = nullptr;
        }
        arr[i] = nullptr;
        }
    }
};

// SymbolTable class
// currentScopeTable -> pointer to the current ScopeTable
// totalBuckets -> total number of buckets in the ScoepTables of the SymbolTable
// starting_id -> starting id of the ScopeTables
class SymbolTable{
public:
    ScopeTable* currentScopeTable;
    int totalBuckets;
    int starting_id;

    SymbolTable(){
        totalBuckets = 0;
        starting_id = 0;
        currentScopeTable = nullptr;
    }

    void enterScope(){
        ScopeTable* newScopeTable = new ScopeTable(totalBuckets, currentScopeTable->id, ++(currentScopeTable->no_children_scope_tables));
        newScopeTable->parentScope = currentScopeTable;
        currentScopeTable = newScopeTable;
    }

    void exitScope(){
        if(currentScopeTable == nullptr){
            cout<<"No scope to exit"<<endl;
            return;
        }
        cout<<endl<<"ScopeTable with id "<<currentScopeTable->id<<" removed"<<endl;
        ScopeTable* temp = currentScopeTable;
        currentScopeTable = currentScopeTable->parentScope;
        delete temp;
    }

    // true -> inserted successfully
    // false -> already exists or not inserted
    // only inserts in the current ScopeTable
    bool insert(string name, string type){
        if(currentScopeTable == nullptr){
            cout<<"No scope to insert"<<endl;
            return false;
        }
        return currentScopeTable->insert(name, type);
    }

    // true -> deleted successfully
    // false -> not found or not deleted
    bool remove(string name){
        if(currentScopeTable == nullptr){
            cout<<"No scope to remove"<<endl;
            return false;
        }
        return currentScopeTable->del(name);
    }

    SymbolInfo* lookUp(string name){
        if(currentScopeTable == nullptr){
            cout<<"No scope to look up"<<endl;
            return nullptr;
        }
        ScopeTable* temp = currentScopeTable;
        while(temp != nullptr){
            SymbolInfo* result = temp->lookUp(name);
            if(result != nullptr) return result;
            temp = temp->parentScope;
        }
        return nullptr;
    }

    void printCurrentScopeTable(){
        if(currentScopeTable == nullptr){
            cout<<"No scope to print"<<endl;
            return;
        }
        currentScopeTable->printScopeTable();
    }

    void printAllScopeTables(){
        if(currentScopeTable == nullptr){
            cout<<"No scope to print"<<endl;
            return;
        }
        ScopeTable* temp = currentScopeTable;
        while(temp != nullptr){
            temp->printScopeTable();
            temp = temp->parentScope;
        }
    }

    void handleInput(){
        ifstream file("input.txt");
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
                    SymbolInfo* result = lookUp(name);
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

int main(){
    SymbolTable st;
    st.handleInput();
    return 0;
}