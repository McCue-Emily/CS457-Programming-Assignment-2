/*
    Author: Emily McCue
    Title: Programming Assignment 2
    Due Date: March 29, 2022
            ---
    Class: CS 457, Section 1001
    Professor: Prof. Zhao
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

bool tokenize(char userInput[200]);
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
bool printSelect(string dataArray[200], int dataCounter, string dbName, string totalPath, string dataName, string operand, string data);
void update(char* useLoopTokens, string useDBName);
void insert(char* useLoopTokens, string dbName);
void deleteData(char* useLoopTokens, string dbName);
bool modifyTable(string dbName, string totalPath, string dataName, string operand, string data);

int main() {

    bool exit = false;
    char userInput[200];
    
    cout << "-- CS457 PA2" << endl << endl;

    while(exit == false) {
        cin.getline(userInput, 200);
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
bool tokenize(char userInput[200]) {
    
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
    char userInputUseLoop[200];

    while (exitUseLoop == false) {

        cin.getline(userInputUseLoop, 200);
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
        } else if (token1 == "SELECT" || token1 == "select") {
            select(useLoopTokens, useDBName);
        } else if (token1 == "insert") {
            insert(useLoopTokens, useDBName);
        } else if (token1 == "update") {
            update(useLoopTokens, useDBName);
        } else if (token1 == "delete") {
            deleteData(useLoopTokens, useDBName);
        } else if (token1 == ".EXIT" || token1 == ".exit") {
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
    } else if (functionName == ".EXIT" || functionName == ".exit") {
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

    if (selectCheck == "*") {
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

        bool exists;
        exists = tableExists(totalPath);

        if (exists) {
            string tempVar;
            ifstream printFile (totalPath.c_str());
            if (printFile.is_open()) {
                while (!printFile.eof()) {
                    getline(printFile, tempVar);
                    cout << tempVar << endl;
                }
            }
            printFile.close();
        } else {
            cout << "-- !Failed to query table " << tbName << " because it does not exist." << endl;
        }
    } else {

        int dataCounter = 0;
        string dataArray[200];
        size_t found;

        while (selectCheck.back() == ',') {
            while ((found = selectCheck.find(",")) != string::npos) {
                selectCheck.replace(found, 1, "");
            }
            dataArray[dataCounter] = selectCheck;
            useLoopTokens = strtok(NULL, " ");
            selectToken = useLoopTokens;
            selectCheck = selectToken;
            dataCounter++;
        }
        
        dataArray[dataCounter] = selectCheck;
        dataCounter++;

        useLoopTokens = strtok(NULL, " ");
        char* fromToken = useLoopTokens;
        string strFromToken = fromToken;

        useLoopTokens = strtok(NULL, " ");
        char* tbToken = useLoopTokens;
        string tbName = tbToken;

        string totalPath = dbName + "/" + tbName + ".txt";

        useLoopTokens = strtok(NULL, " ");
        char* whereToken = useLoopTokens;
        string strWhereToken = whereToken;

        if (strWhereToken == "where") {

            useLoopTokens = strtok(NULL, " ");
            char* dataNameToken = useLoopTokens;
            string dataName = dataNameToken;

            useLoopTokens = strtok(NULL, " ");
            char* operandToken = useLoopTokens;
            string operand = operandToken;

            useLoopTokens = strtok(NULL, " ");
            char* dataToken = useLoopTokens;
            string data = dataToken;

            size_t pos;
            while ((pos = data.find(";")) != string::npos) {
                data.replace(pos, 1, "");
            }

            bool exists = tableExists(totalPath);

            if (exists) {

                bool success;
                success = printSelect(dataArray, dataCounter, dbName, totalPath, dataName, operand, data);

                if (!success) {
                    cout << "-- Error querying file, try again." << endl;
                }
            } else {
                cout << "-- !Failed to query table " << tbName << " because it does not exist." << endl;

            }

        } else {
        cout << "-- Invalid input." << endl;
        }

    }

}

bool printSelect(string dataArray[200], int dataCounter, string dbName, string totalPath, string dataName, string operand, string data) {

    int dataNamePosition;
    int dataNameCounter = 0;
    int indexCounter = 0;
    string firstLine;
    string tempToken;
    string line;

    ifstream tableInUse;
    tableInUse.open(totalPath);

    for (int i = 0; i < 1; i++) {
        getline(tableInUse, firstLine);
    }

    if (firstLine.find(dataName) != string::npos) {

        istringstream firstStream(firstLine);
        while (firstStream) {
            string tempWord1;
            firstStream >> tempWord1;
            if (tempWord1.find(dataName) != string::npos) {
                dataNamePosition = dataNameCounter;
            } else if (tempWord1 == "|") {
                dataNameCounter++;
            }
        }
        dataNameCounter += dataNameCounter;

        istringstream secondStream(firstLine);
        int barCount = 0;
        int index[200];
        while (secondStream) {
            string tempWord2;
            secondStream >> tempWord2;
            if (tempWord2 == "|") {
                barCount++;
                continue;
            }
            for (int i = 0; i < dataCounter; i++) {
                if (tempWord2 == dataArray[i]) {
                    index[indexCounter] = barCount;
                    indexCounter++;
                }
            }
        }

        istringstream fourthStream(firstLine);
        string tempWord4;
        string newFirstLine;
        while (fourthStream) {
            if (tempWord4 == dataName) {
                fourthStream >> tempWord4;
                while (tempWord4 != "|") {
                    fourthStream >> tempWord4;
                }
                fourthStream >> tempWord4;
            }
            newFirstLine = newFirstLine + " " + tempWord4;
            fourthStream >> tempWord4;
        }

        cout << newFirstLine << endl;

        while (getline(tableInUse, line)) {
            if (line.find(data) == string::npos) {
                int columnCounter = 0;
                istringstream thirdStream(line);
                while (thirdStream) {
                    string tempWord3;
                    thirdStream >> tempWord3;
                    if (tempWord3 == "|") {
                        columnCounter++;
                        continue;
                    }
                    for (int j = 0; j < (indexCounter); j++) {
                        if (columnCounter == index[j]) {
                            cout << tempWord3 << " | ";
                        } else if ((columnCounter - 1) == index[j]){
                            cout << tempWord3;
                            break;
                        }
                    }
                }
                cout << endl;
            }
        }

        tableInUse.close();

    } else {
        cout << "-- Error modifying records." << endl;
        tableInUse.close();
        return false;
    }
    
    tableInUse.close();
    return true;

}

void update(char* useLoopTokens, string useDBName) {

    int dataNameToBeChangedPosition;
    int dataNameFindingPosition;
    int dataNameFindingCounter = 0;
    int dataNameToBeChangedCounter = 0;
    int dataNameFinding = 0;
    int counter = 0;
    string firstLine;
    string tempToken;
    string line;

    useLoopTokens = strtok(NULL, " ");
    char* charTBName = useLoopTokens;
    string tbName = charTBName;

    string totalPath = useDBName + "/" + tbName + ".txt";

    bool exists = tableExists(totalPath);
    if (exists) {

        useLoopTokens = strtok(NULL, " ");
        char* charSetToken = useLoopTokens;
        string setToken = charSetToken;

        useLoopTokens = strtok(NULL, " ");
        char* charDataNameToBeChanged = useLoopTokens;
        string dataNameToBeChanged = charDataNameToBeChanged;

        useLoopTokens = strtok(NULL, " ");
        char* charFirstOperand = useLoopTokens;
        string firstOperand = charFirstOperand;

        useLoopTokens = strtok(NULL, " ");
        char* charDataToBeSet = useLoopTokens;
        string dataToBeSet = charDataToBeSet;

        useLoopTokens = strtok(NULL, " ");
        char* charWhere = useLoopTokens;
        string whereToken = charWhere;

        useLoopTokens = strtok(NULL, " ");
        char* charDataNameFinding = useLoopTokens;
        string dataNameFinding = charDataNameFinding;

        useLoopTokens = strtok(NULL, " ");
        char* charSecondOperand = useLoopTokens;
        string secondOperand = charSecondOperand;

        useLoopTokens = strtok(NULL, " ");
        char* charDataToBeChanged = useLoopTokens;
        string dataToBeChanged = charDataToBeChanged;

        size_t pos;
        while ((pos = dataToBeChanged.find(";")) != string::npos) {
            dataToBeChanged.replace(pos, 1, "");
        }

        ifstream tableInUse;
        tableInUse.open(totalPath);

        string tempPath = useDBName + "/Writing.txt";
        ofstream tempFile;
        tempFile.open(tempPath);

        for (int i = 0; i < 1; i++) {
            getline(tableInUse, firstLine);
        }

        if (firstLine.find(dataNameToBeChanged) != string::npos && firstLine.find(dataNameFinding) != string::npos) {

            istringstream firstStream(firstLine);

            cout << "dataNameFinding: " << dataNameFinding << endl;
            cout << "dataNameToBeChanged: " << dataNameToBeChanged << endl;

            // update Product set price = 14.99 where name = 'Gizmo';

            while (firstStream) {
                string tempWord1;
                firstStream >> tempWord1;

                if (tempWord1.find(dataNameToBeChanged) != string::npos) {
                    dataNameToBeChangedPosition = dataNameToBeChangedCounter;
                    istringstream secondStream(firstLine);

                    while (secondStream) {
                        string tempWord2;
                        secondStream >> tempWord2;
                        if (tempWord2.find(dataNameFinding) != string::npos) {
                            dataNameFindingPosition = dataNameFindingCounter;
                            cout << "found data name to be found at pos: " << dataNameFindingPosition << endl;
                        } else if (tempWord2.find(dataNameToBeChanged) != string::npos) {
                            dataNameToBeChangedPosition = dataNameToBeChangedCounter;
                            cout << "found data name to be changed at pos: " << dataNameToBeChangedPosition << endl;
                        } else if (tempWord2 == "|") {
                            dataNameFindingCounter++;
                            dataNameToBeChangedCounter++;
                        }

                    }

                }

            }

            if (dataNameFinding == dataNameToBeChanged) {
                dataNameToBeChangedPosition = dataNameFindingPosition;
            }

            dataNameFindingPosition += dataNameFindingPosition;
            dataNameToBeChangedPosition += dataNameToBeChangedPosition;

            tempFile << firstLine;

            // update Product set price = 14.99 where name = 'Gizmo';
            // update Product set name = 'Gizmo' where name = 'SuperGizmo';

            while (getline(tableInUse, line)) {
                tempFile << endl;
                istringstream thirdStream(line);
                bool keepGoing = true;

                while (thirdStream && keepGoing) {
                    string tempWord3;

                    // cout << endl <<  "word we're searching for: " << dataToBeChanged << " at position: " << dataNameFindingPosition << endl;
                    // cout << "word we're replacing search word for: " << dataToBeSet << " at position: " << dataNameToBeChangedPosition << endl;

                    for (int j = 0; j < (dataNameFindingPosition + 1); j++) {
                        thirdStream >> tempWord3;
                    }
                    cout << endl << "final tempWord3 after the loop: " << tempWord3 << endl;
                    if (tempWord3 == dataToBeChanged) {

                        cout << "line that we're editing: " << line << endl;
                        istringstream fourthStream(line);
                        string tempWord4;

                        istringstream fifthStream(line);
                        string tempWord5;

                        for (int l = 0; l < (dataNameToBeChangedPosition); l++) {
                            fourthStream >> tempWord4;
                            tempFile << tempWord4 + " ";
                        }

                        while (fifthStream) {
                            for (int k = 0; k < (dataNameToBeChangedPosition + 1); k++) {
                                fifthStream >> tempWord5;
                            }
                            cout << "final tempWord5 after the loop: " << tempWord5 << endl;
                            cout << "data to be inserted: " << dataToBeSet << endl;
                            cout << "tempWord5 will replace the data name to be set: " << dataToBeSet << " -> " << tempWord5 << endl;
                            tempFile << dataToBeSet << " ";
                            counter++;
                            fifthStream >> tempWord5;
                            while(fifthStream) {
                                //fifthStream >> tempWord5;
                                tempFile << tempWord5 + " ";
                                cout << "appended " << tempWord5 << " to the end of line being changed" << endl;
                                fifthStream >> tempWord5;
                            }
                            break;
                        }
                        break;
                        
                    } else {
                        tempFile << line;
                        cout << "line that does not contain tempWord3: " << line << endl;
                    }
                    break;
                }
            }
        }

            tableInUse.close();
            tempFile.close();
            remove(totalPath.c_str());
            rename(tempPath.c_str(), totalPath.c_str());
            if (counter == 1) {
                cout << "-- " << counter << " record modified" << endl;
            } else {
                cout << "-- " << counter << " records modified" << endl;
            }

    } else {
        cout << "-- !Failed to update table " << tbName << " because it does not exist." << endl;
    }


}

void insert(char* useLoopTokens, string dbName) {

    useLoopTokens = strtok(NULL, " ");
    char* charPointName = useLoopTokens;
    string pointToTB = charPointName;

    if (pointToTB == "into") {

        useLoopTokens = strtok(NULL, " ");
        char* charTBName = useLoopTokens;
        string tbName = charTBName;

        string totalPath = dbName + "/" + tbName + ".txt";

        useLoopTokens = strtok(NULL, "(");
        char* howToAlter = useLoopTokens;
        string strHowToAlter = howToAlter;

        if (strHowToAlter == "values") {

            useLoopTokens = strtok(NULL, " ");
            char* charRestOfTokens = useLoopTokens;
            string restOfTokens = charRestOfTokens;

            char* tempVar;

            while ((useLoopTokens = strtok(NULL, " ")) != NULL) {
                tempVar = useLoopTokens;
                restOfTokens = restOfTokens + " " + tempVar;
            }

            size_t pos;
            while ((pos = restOfTokens.find(";")) != string::npos) {
                restOfTokens.replace(pos, 1, "");
            }

            while ((pos = restOfTokens.find(")")) != string::npos) {
                restOfTokens.replace(pos, 1, "");
            }

            if (!restOfTokens.empty()) {
                string original = ",";
                string replacement = " |";
                while ((pos = restOfTokens.find(original)) != string::npos) {
                    restOfTokens.replace(pos, 1, replacement);
                }
            }
            
            bool exists = tableExists(totalPath);
    
            if (exists) {
                fstream modifyTB;
                modifyTB.open(totalPath.c_str(), fstream::app);
                modifyTB << "\n" << restOfTokens;
                modifyTB.close();
                cout << "-- 1 new record inserted." << endl;
            } else {
                cout << "-- !Failed to insert record into table " << tbName << " because it does not exist." << endl;
            }
        }

    } else {
        cout << "-- Invalid input." << endl;
    }
}

void deleteData(char* useLoopTokens, string dbName) {

    useLoopTokens = strtok(NULL, " ");
    char* fromToken = useLoopTokens;
    string pointToTB = fromToken;

    if (pointToTB == "from") {
        useLoopTokens = strtok(NULL, " ");
        char* charTBName = useLoopTokens;
        string tbName = charTBName;

        string totalPath = dbName + "/" + tbName + ".txt";

        useLoopTokens = strtok(NULL, " ");
        char* whereToken = useLoopTokens;
        string pointToValue = whereToken;

        if (pointToValue == "where") {

            size_t pos;
            while ((pos = tbName.find(";")) != string::npos) {
                tbName.replace(pos, 1, "");
            }

            useLoopTokens = strtok(NULL, " ");
            char* dataNameToken = useLoopTokens;
            string dataName = dataNameToken;

            useLoopTokens = strtok(NULL, " ");
            char* operatorToken = useLoopTokens;
            string operand = operatorToken;

            useLoopTokens = strtok(NULL, " ");
            char* dataToken = useLoopTokens;
            string data = dataToken;
            while ((pos = data.find(";")) != string::npos) {
                data.replace(pos, 1, "");
            }

            bool exists = tableExists(totalPath);

            if (exists) {

                bool success;
                success = modifyTable(dbName, totalPath, dataName, operand, data);

                if (!success) {
                    cout << "-- Error modifying file, try again." << endl;
                }
            } else {
                cout << "-- !Failed to modify table " << tbName << " because it does not exist." << endl;

            }

        }

    } else {
        cout << "-- Invalid input." << endl;
    }

}

bool modifyTable(string dbName, string totalPath, string dataName, string operand, string data) {
    
    int dataNamePosition;
    int dataNameCounter = 0;
    int counter = 0;
    string firstLine;
    string tempToken;
    string line;

    ifstream tableInUse;
    tableInUse.open(totalPath);

    string tempPath = dbName + "/Writing.txt";
    ofstream tempFile;
    tempFile.open(tempPath);

    for (int i = 0; i < 1; i++) {
        getline(tableInUse, firstLine);
    }

    char charOperand = operand[0];
    switch(charOperand) {
        case '>':
            if (firstLine.find(dataName) != string::npos) {
                istringstream firstStream(firstLine);

                while (firstStream) {
                    string tempWord1;
                    firstStream >> tempWord1;
                    if (tempWord1.find(dataName) != string::npos) {
                        dataNamePosition = dataNameCounter;
                    } else if (tempWord1 == "|") {
                        dataNameCounter++;
                    }
                }
                dataNameCounter += dataNameCounter;

                tempFile << firstLine;
                float numData = stof(data);

                while (getline(tableInUse, line)) {

                    istringstream secondStream(line);
                    bool keepGoing = true;

                    while (secondStream && keepGoing) {
                        string tempWord2;

                        for (int i = 0; i < dataNameCounter; i++) {
                            secondStream >> tempWord2;
                        }

                        if (tempWord2.find('.') != string::npos ) {

                            float tempFloat = stof(tempWord2);
                            if (tempFloat > numData) {
                                counter++;
                                keepGoing = false;
                            } else {
                                tempFile << endl << line;
                            }

                        }

                    }
                }

                tableInUse.close();
                tempFile.close();
                remove(totalPath.c_str());
                rename(tempPath.c_str(), totalPath.c_str());
                if (counter == 1) {
                    cout << "-- " << counter << " record modified" << endl;
                } else {
                    cout << "-- " << counter << " records modified" << endl;
                }
            } else {
                cout << "-- Error modifying records." << endl;
                tableInUse.close();
                tempFile.close();
                remove(tempPath.c_str());
                return false;
            }
            break;
        case '<':
            if (firstLine.find(dataName) != string::npos) {
                istringstream firstStream(firstLine);

                while (firstStream) {
                    string tempWord1;
                    firstStream >> tempWord1;
                    if (tempWord1.find(dataName) != string::npos) {
                        dataNamePosition = dataNameCounter;
                    } else if (tempWord1 == "|") {
                        dataNameCounter++;
                    }
                }
                dataNameCounter += dataNameCounter;

                tempFile << firstLine;
                float numData = stof(data);

                while (getline(tableInUse, line)) {

                    istringstream secondStream(line);
                    bool keepGoing = true;

                    while (secondStream && keepGoing) {
                        string tempWord2;

                        for (int i = 0; i < dataNameCounter; i++) {
                            secondStream >> tempWord2;
                        }

                        if (tempWord2.find('.') != string::npos ) {

                            float tempFloat = stof(tempWord2);
                            if (tempFloat < numData) {
                                counter++;
                                keepGoing = false;
                            } else {
                                tempFile << endl << line;
                            }
                        }
                    }
                }

                tableInUse.close();
                tempFile.close();
                remove(totalPath.c_str());
                rename(tempPath.c_str(), totalPath.c_str());
                if (counter == 1) {
                    cout << "-- " << counter << " record modified" << endl;
                } else {
                    cout << "-- " << counter << " records modified" << endl;
                }
            } else {
                cout << "-- Error modifying records." << endl;
                tableInUse.close();
                tempFile.close();
                remove(tempPath.c_str());
                return false;
            }
            break;
        case '=':
            if (firstLine.find(dataName) != string::npos) {

                tempFile << firstLine;

                while (getline(tableInUse, line)) {

                    if (line.find(data) != string::npos) {
                        counter++;
                    } else {
                        tempFile << endl << line;
                    }

                }

                tableInUse.close();
                tempFile.close();
                remove(totalPath.c_str());
                rename(tempPath.c_str(), totalPath.c_str());
                if (counter == 1) {
                    cout << "-- " << counter << " record modified" << endl;
                } else {
                    cout << "-- " << counter << " records modified" << endl;
                }
            } else {
                cout << "-- Error modifying records." << endl;
                tableInUse.close();
                tempFile.close();
                remove(tempPath.c_str());
                return false;
            }
            break;
    }

    tableInUse.close();
    tempFile.close();
    remove(tempPath.c_str());
    return true;

}