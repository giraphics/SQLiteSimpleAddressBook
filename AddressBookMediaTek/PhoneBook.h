#pragma once
#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#define BUFFER 1000
class PhoneBook;
class PhoneBook
{
private:
	sqlite3*	db;
	char*		zErrMsg;
	int			rc;
	char*		sql;
	char		sqlStatement[BUFFER];
	char		currentMenuChoice;

	PhoneBook(void); //Private Constructor

public:
	~PhoneBook(void);
	int menu();
	int menuBanner();
	int createDatabase();
	int addNewEntry();
	int displayEntry();
	int updateEntry();
	int removeEntry();
	int sortDatabase();
	int searchDatabase();
	int search( bool phone, bool name, long phoneNo, char* newName );
	static PhoneBook& Instance()
    {
        static PhoneBook pbOject;
        return pbOject;
    }
};
