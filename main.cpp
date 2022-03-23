/*
    Author: Emily McCue
    Title: Programming Assignment 1
    Due Date: March 21, 2022
            ---
    Class: CS 457, Section 1001
    Professor: Prof. Zhao
*/

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

bool tokenize(char userInput[50]);
void nowUsing(string useDBName);
void createTB(char* useLoopTokens, string dbName);
void dropTB(char* useLoopTokens, string useDBName);
bool notUsing(char* tokens);
void create(char* tokens);
bool databaseExists(const string &s);
bool tableExists(string totalPath);
void drop(char* tokens);
void alter(char* useLoopTokens, string dbName);
void select(char* useLoopTokens, string dbName);

int main() {

    bool exit = false;
    char userInput[50];
    
    cout << "-- CS457 PA1" << endl << endl;

    while(exit == false) {
        cin.getline(userInput, 50);
        exit = tokenize(userInput);
    }

    return 0;
}

/**
    Tokenize the first word in userInput.

    This token decides whether to operate within a database or whether
    to continue operating outside of a database. Depending on the 
    token, it calls different functions nowUsing() or notUsing().

    @param userInput which is the user inputted command
    @return boolean value T/F which decides if we continue or exit
*/
bool tokenize(char userInput[50]) {
    
    char* tokens = strtok(userInput, " ");
    string token1 = tokens;

    if (token1 == "USE") {

        tokens = strtok(NULL, " ");
        char* temp = tokens;
        string useDBName = temp;

        size_t pos;
        while ((pos = useDBName.find(";")) != string::npos) {
            useDBName.replace(pos, 1, "");
        }

        bool exists = databaseExists(useDBName);

        if(exists) {
            cout << "-- Using database " << useDBName << "." << endl;
            nowUsing(useDBName);
            return true;
        } else {
            cout << "-- !Failed to use database " << useDBName << " because it does not exist." << endl;
        }
        return false;

    } else {
        bool notUsingLoop = notUsing(tokens);
        return notUsingLoop;
    }

}

/**
    Using a database.

    User has entered USE "database name" and now operating within the use
    database loop which allows user to create and edit tables within
    entered database.

    @param useDBName which is the name of the database currently in use
*/
void nowUsing(string useDBName) {

    bool exitUseLoop = false;
    char userInputUseLoop[50];

    while (exitUseLoop == false) {

        cin.getline(userInputUseLoop, 50);
        char* useLoopTokens = strtok(userInputUseLoop, " ");
        char* charToken1 = useLoopTokens;
        string token1 = charToken1;

        size_t pos;
        while ((pos = token1.find(";")) != string::npos) {
            token1.replace(pos, 1, "");
        }

        if (token1 == "USE") {

            useLoopTokens = strtok(NULL, " ");
            string useDBName = useLoopTokens;
            while ((pos = useDBName.find(";")) != string::npos) {
                useDBName.replace(pos, 1, "");
            }

            bool exists = databaseExists(useDBName);

            if(exists) {
                cout << "-- Using database " << useDBName << endl;
                nowUsing(useDBName);
            } else {
                cout << "-- !Failed to use database " << useDBName << " because it does not exist." << endl;
            }

        } else if (token1 == "CREATE") {
            createTB(useLoopTokens, useDBName);
        } else if (token1 == "DROP") {
            dropTB(useLoopTokens, useDBName);
        } else if (token1 == "ALTER") {
            alter(useLoopTokens, useDBName);
        } else if (token1 == "SELECT") {
            select(useLoopTokens, useDBName);
        } else if (token1 == ".EXIT") {
            cout << "-- All done." << endl;
            exitUseLoop = true;
        } else {
            cout << "Invalid Input." << endl;
        }

    }

}

/**
    Create tables within the database in use.

    Using the command tokens and the name of the database in use, creates a
    table with inputted table name if the table name is not already in use.

    @param useLoopTokens which is the user inputted command as tokens
    @param dbName which is the database name currently in use
*/
void createTB(char* useLoopTokens, string dbName) {

    useLoopTokens = strtok(NULL, " ");
    char* tableToken = useLoopTokens;
    string tbCheck = tableToken;

    if (tbCheck == "TABLE") {
        useLoopTokens = strtok(NULL, " ");
        char* charTBName = useLoopTokens;
        string tbName = charTBName;

        size_t pos;
        while ((pos = tbName.find(";")) != string::npos) {
            tbName.replace(pos, 1, "");
        }

        string totalPath = dbName + "/" + tbName + ".txt";

        string restOfTokens;
        char* tempVar;

        while ((useLoopTokens = strtok(NULL, " ")) != NULL) {
            tempVar = useLoopTokens;
            restOfTokens = restOfTokens + " " + tempVar;
        }

        if (!restOfTokens.empty()) {
            string original = ",";
            string replacement = " |";
            while ((pos = restOfTokens.find(original)) != string::npos) {
                restOfTokens.replace(pos, 1, replacement);
            }
            while ((pos = restOfTokens.find(";")) != string::npos) {
                restOfTokens.replace(pos, 1, "");
            }
            restOfTokens.erase(0,2);
            restOfTokens.erase(prev(restOfTokens.end()));

        }
        
        bool exists = tableExists(totalPath);
 
        if (!exists) {
            ofstream newTB(totalPath.c_str());
            newTB << restOfTokens;
            newTB.close();
            cout << "-- Table " << tbName << " created." << endl;
        } else {
            cout << "-- !Failed to create table " << tbName << " because it already exists." << endl;
        }
    } else {
        cout << "-- Invalid input." << endl;
    }

}

/**
    Drop tables within the database in use.

    Using the command tokens and the name of the database in use, drops the
    table with inputted table name if the table exists.

    @param useLoopTokens which is the user inputted command as tokens
    @param dbName which is the database name currently in use
*/
void dropTB(char* useLoopTokens, string useDBName) {

    useLoopTokens = strtok(NULL, " ");
    char* tableToken = useLoopTokens;
    string tbCheck = tableToken;

    if (tbCheck == "TABLE") {
        useLoopTokens = strtok(NULL, " ");
        char* charTBName = useLoopTokens;
        string tbName = charTBName;

        size_t pos;
        while ((pos = tbName.find(";")) != string::npos) {
            tbName.replace(pos, 1, "");
        }

        string totalPath = useDBName + "/" + tbName + ".txt";
        
        bool exists = tableExists(totalPath);

        if (exists) {
            if (remove(totalPath.c_str()) == 0) {
                cout << "-- Table " << tbName << " deleted." << endl;
            } else {
                cout << "-- Error deleting file, try again." << endl;
            }
        } else {
            cout << "-- !Failed to delete table " << tbName << " because it does not exist." << endl;

        }
    } else {
        cout << "-- Invalid input." << endl;
    }

}

/**
    Limits functions available until using a database.

    This function is used before the user inputs the "USE" command and
    is then working within a database. This limits the capabilities
    of the user until enters the nowUsing() loop.

    @param tokens which is the user inputted command as tokens
    @return boolean value T/F which decides if we continue or exit
*/
bool notUsing(char* tokens) {

    char* token1 = tokens;
    string functionName = token1;

    if (functionName == "CREATE") {
        create(tokens);
    } else if (functionName == "DROP") {
        drop(tokens);
    } else if (functionName == ".EXIT") {
        cout << "-- All done." << endl;
        return true;
    } else {
        cout << "-- Invalid Input." << endl;
    }

    return false;

}

/**
    Create database.

    This function creates a database with the user inputted token as
    the name as long as the name is not already in use (the database
    already exists).

    @param tokens which is the user inputted command as tokens
*/
void create(char* tokens) {

    char* createToken = tokens;

    tokens = strtok(NULL, " ");
    char* token2 = tokens;
    string strToken2 = token2;

    if (strToken2 == "DATABASE") {
        tokens = strtok(NULL, " ");
        char* charDBName = tokens;
        charDBName[strlen(charDBName)-1] = '\0';
        string dbName = charDBName;

        bool exists = databaseExists(dbName);

        if (!exists) {
            mkdir(charDBName,0777);
            cout << "-- Database " << dbName << " created." << endl;
        } else {
            cout << "-- !Failed to create database " << dbName << " because it already exists." << endl;
        }

    } else {
        cout << "-- Invalid input." << endl;

    }
    
}

/**
    Check if database exists already.

    The inputted database name is passed into this function and it returns true
    or false depending on if the database name correlates to a pre-existing
    database (if the database already exists).

    @param s which is the user inputted database name
    @return boolean value T/F which correlates to if the database exists or not
*/
bool databaseExists(const string &s) {
    struct stat buffer;
    return(stat(s.c_str(), &buffer) == 0);

}

/**
    Check if table exists already.

    The inputted table path is passed into this function and it returns true
    or false depending on if the table path correlates to a pre-existing
    table already in the database in use (if the table already exists in
    said database).

    @param totalPath which is the user inputted table name and database in use 
                    name formatted to a total path to the table
    @return boolean value T/F which decides if we continue or exit
*/
bool tableExists(string totalPath) {
    ifstream tbCheck;
    tbCheck.open(totalPath);
    if(tbCheck) {
        tbCheck.close();
        return true;
    } else {
        return false;
    }

}

/**
    Drop database.

    Using the command tokens, it finds the name of the database to be dropped
    and drops the database with the inputted database name if it exists.

    @param tokens which is the user inputted command.
*/
void drop(char* tokens) {

    char* token1 = tokens;

    tokens = strtok(NULL, " ");
    char* token2 = tokens;
    string strToken2 = token2;

    if (strToken2 == "DATABASE") {
        tokens = strtok(NULL, " ");
        char* charDBName = tokens;
        charDBName[strlen(charDBName)-1] = '\0';
        string dbName = charDBName;

        bool exists = databaseExists(dbName);
        if (exists) {
            rmdir(charDBName);
            cout << "-- Database " << dbName << " deleted." << endl;
        } else {
            cout << "-- !Failed to delete " << dbName << " because it does not exist." << endl;
        }

    } else {
        cout << "-- Invalid input." << endl;
    }

}

/**
    Alter tables.

    This function alters tables withing the database in use if given
    the table name, and information on how it will be altered.
    If the table exists within the database in use, it then alters it
    by processing the next tokens and stores the new information in the
    table provided.

    @param useLoopTokens which is the user inputted command.
    @param dbName which is the database name in use.
*/
void alter(char* useLoopTokens, string dbName) {

    useLoopTokens = strtok(NULL, " ");
    char* tableToken = useLoopTokens;
    string tbCheck = tableToken;

    if (tbCheck == "TABLE") {
        useLoopTokens = strtok(NULL, " ");
        char* charTBName = useLoopTokens;
        string tbName = charTBName;

        string totalPath = dbName + "/" + tbName + ".txt";

        useLoopTokens = strtok(NULL, " ");
        char* howToAlter = useLoopTokens;
        string strHowToAlter = howToAlter;

        if (strHowToAlter == "ADD") {

            string restOfTokens;
            char* tempVar;

            while ((useLoopTokens = strtok(NULL, " ")) != NULL) {
                tempVar = useLoopTokens;
                restOfTokens = restOfTokens + " " + tempVar;
            }

            size_t pos;
            while ((pos = restOfTokens.find(";")) != string::npos) {
                restOfTokens.replace(pos, 1, "");
            }

            if (!restOfTokens.empty()) {
                string original = ",";
                string replacement = " |";
                size_t pos;
                while ((pos = restOfTokens.find(original)) != string::npos) {
                    restOfTokens.replace(pos, 1, replacement);
                }
            }
            
            bool exists = tableExists(totalPath);
    
            if (exists) {
                fstream modifyTB;
                modifyTB.open(totalPath.c_str(), fstream::app);
                modifyTB << " |" << restOfTokens;
                modifyTB.close();
                cout << "-- Table " << tbName << " modified." << endl;
            } else {
                cout << "-- !Failed to modify table " << tbName << " because it does not exist." << endl;
            }
        }

    } else {
        cout << "-- Invalid input." << endl;
    }
        
}

/**
    Select tables.

    This function selects tables withing the database in use if given
    the table name, and information on how much to be selected.
    If the table exists within the database in use, it then selects the
    amount by processing the next tokens and displays the information to
    the screen.

    @param useLoopTokens which is the user inputted command.
    @param dbName which is the database name in use.
*/
void select(char* useLoopTokens, string dbName) {

    useLoopTokens = strtok(NULL, " ");
    char* selectToken = useLoopTokens;
    string selectCheck = selectToken;

    useLoopTokens = strtok(NULL, " ");
    char* fromToken = useLoopTokens;
    string strFromToken = fromToken;

    useLoopTokens = strtok(NULL, " ");
    char* tbNameToken = useLoopTokens;
    string tbName = tbNameToken;
    
    size_t pos;
    while ((pos = tbName.find(";")) != string::npos) {
        tbName.replace(pos, 1, "");
    }

    string totalPath = dbName + "/" + tbName + ".txt";


    if (selectCheck == "*" && strFromToken == "FROM") {
        bool exists;
        exists = tableExists(totalPath);

        if (exists) {
            string tempVar;
            ifstream printFile (totalPath.c_str());
            if (printFile.is_open()) {
                while (! printFile.eof()) {
                    getline(printFile, tempVar);
                    cout << tempVar << endl;
                }
            }
            printFile.close();
        } else {
            cout << "-- !Failed to query table " << tbName << " because it does not exist." << endl;
        }
    } else {
        cout << " -- Invalid input." << endl;
    }

}