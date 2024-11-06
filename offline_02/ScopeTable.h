#ifndef SCOPETABLE_H
#define SCOPETABLE_H

#include "SymbolInfo.h"
using namespace std;

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

        // cout<<endl<<"New ScopeTable with id "<<this->id<<" created"<<endl;
    }

    // hash function
    unsigned int sdbm(string name){
        unsigned int hash = 0;
        for(int i=0;name[i];i++){
            hash = name[i] + (hash << 6) + (hash << 16) - hash;
        }
        return hash;
    }

    // SymbolInfo* -> found SymbolInfo object
    // nullptr -> not found
    SymbolInfo* lookUp(string name, string type){
        int index = sdbm(name) % totalBuckets;
        SymbolInfo* temp = arr[index];

        while(temp != nullptr){
            if(temp->getName() == name && (temp->getType() == type || temp->getType() == "no_type")) {
                // cout<<endl<<"Found in ScopeTable# "<<id<<" at position "<<index<<", "<<temp->pos_in_bucket<<endl;
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
        SymbolInfo* lookUpResult = lookUp(name,type);
        
        if(lookUpResult != nullptr){
            // cout<<endl<<"<"<<name<<","<<type<<"> already exists in current ScopeTable"<<endl;
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
        // cout<<endl<<"Inserted in ScopeTable# "<<id<<" at position "<<index<<", "<<newSymbolInfo->pos_in_bucket<<endl;
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
            // cout<<"Not found"<<endl;
            return false;
        }

        // cout<<endl<<"Found in ScopeTable# "<<id<<" at position "<<index<<", "<<temp->pos_in_bucket<<endl;
        if(prev == nullptr){
            arr[index] = arr[index]->next;
        }else{
            prev->next = temp->next;
        }
        temp->next = nullptr;

        // cout<<endl<<"Deleted Entry at "<<index<<", "<<temp->pos_in_bucket<<" from current ScopeTable"<<endl;

        delete temp;
        temp = nullptr;

        return true;
    }

    string printScopeTable(){
        // cout<<endl<<"ScopeTable #"<<id<<endl;
        // for(int i=0;i<totalBuckets;i++){
        //     cout<<i<<" --> ";
        //     SymbolInfo* temp = arr[i];
        //     while(temp != nullptr){
        //         string name = temp->getName();
        //         string type = temp->getType();
        //         cout<<"< "<<name<<" : "<<type<<"> ";
        //         temp = temp->next;
        //     }
        //     cout<<endl;
        // }

        string out;
        out += "\nScopeTable #" + id + "\n";
        for(int i=0;i<totalBuckets;i++){
            SymbolInfo* temp = arr[i];
            bool firstElement = true;
            while(temp != nullptr){
                string name = temp->getName();
                string type = temp->getType();
                if(firstElement){
                    out += to_string(i) + " --> ";
                    firstElement = false;
                }
                out += "< " + name + " : " + type + "> ";
                temp = temp->next;
                if(temp == nullptr){
                    out += "\n";
                }
            }
        }
        return out;
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

#endif