#include "PhoneBook.h"
#include <conio.h>
#include <ctype.h>
bool recFound = false;
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   if (argc){
	   recFound = true;
   }
   else{
	   recFound = false;
   }
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

PhoneBook::PhoneBook(void)
{
	printf("\nInitializing PhoneBook.....\n\n");
	db			= NULL;
	zErrMsg		= 0;
	rc			= 0;
	sql			= 0;
	createDatabase();
}

PhoneBook::~PhoneBook(void)
{
   sqlite3_close(db);
}

int PhoneBook::menuBanner()
{
	printf("\n**************************************");
	printf("\n************MENU**********************");
	printf("\nAdd To Phonebook:	enter [A/a]");
	printf("\nDisplay Entries:	enter [D/d]");
	printf("\nUpdate Phonebook:	enter [U/u]");
	printf("\nSort Phonebook:		enter [S/s]");
	printf("\nFind in Phonebook:	enter [F/f]");
	printf("\nRemove Record from:	enter [R/r]");
	printf("\nExit:			enter [E/e]");
	printf("\n**************************************\n\n");
	return 0;
}

int PhoneBook::menu()
{
	while(1){
		menuBanner();
		currentMenuChoice = getch();
		addNewEntry();
		displayEntry();
		updateEntry();
		sortDatabase();
		searchDatabase();
		removeEntry();
		if ( currentMenuChoice == 'E' || currentMenuChoice == 'e'){
			exit(0);
		}
	}
	return 0;
}

int PhoneBook::createDatabase()
{
   /* Open database */
   rc = sqlite3_open("Phonebook.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stdout, "\nOpened database successfully\n");
   }

   /* Create SQL statement */
   sql = "CREATE TABLE PHONEBOOK("  \
         "PHONE INT PRIMARY KEY     NOT NULL," \
         "NAME           TEXT    NOT NULL );";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
   //fprintf(stderr, "\nSQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "\nTable created successfully\n");
   }
   return 0;

}

int PhoneBook::addNewEntry()
{
	if ( !(currentMenuChoice == 'A' || currentMenuChoice == 'a') ){
		return 1;
	}

	printf("\n\nAdd New Entries:");
	printf("\n*****************\n");
	printf("\nAdd Record, press 'Y'/'y' for [YES], 'N'/'n' for [NO]");
	char response = getch();
	long phoneNumber = 0;
	char contactName[BUFFER];
	/* Create SQL statement */
	while(response == 'Y' || response == 'y' ){
		printf("\nEnter Phone: ");
		scanf("%d", &phoneNumber);
		printf("\nEnter Name: ");
		scanf("%s", &contactName);
		sprintf(sqlStatement, "INSERT INTO PHONEBOOK (PHONE,NAME) VALUES (%ld, '%s');", phoneNumber,contactName);
	   /* Execute SQL statement */
	   rc = sqlite3_exec(db, sqlStatement, callback, 0, &zErrMsg);
	   if( rc != SQLITE_OK ){
		  fprintf(stderr, "SQL error: %s\n", zErrMsg);
		  sqlite3_free(zErrMsg);
	   }else{
		  fprintf(stdout, "\nRecords created successfully...\n");
	   }
       printf("\nAdd Record, press 'Y'/'y' for [YES], 'N'/'n' for [NO]");
	   response = getch();
	}
   return 0;
}

int PhoneBook::displayEntry()
{
	if ( !(currentMenuChoice == 'D' || currentMenuChoice == 'd') ){
		return 1;
	}

	printf("\n\nDisplay Results:");
	printf("\n*****************\n");

   /* Create SQL statement */
   sql = "SELECT * from PHONEBOOK";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Displayed results successfully\n");
   }
   return 0;
}

int PhoneBook::updateEntry()
{
	if ( !(currentMenuChoice == 'U' || currentMenuChoice == 'u') ){
		return 1;
	}

	long phoneNumber, newPhoneNumber;
	char newName[BUFFER];
	char choice;
	bool newPhoneFlag = false;
	bool newNameFlag  = false;
	printf("\n\nUpdate Record:\n");
	while(1){
		printf("\nEnter existing phone number in Phonebook:");
		scanf("%d", &phoneNumber);
		newNameFlag = newPhoneFlag = recFound = false;
		search(true, false, phoneNumber, "");
		if (recFound){
			printf("\nDo you want to update this phone number: Yes[y/y] | No[N/n]:");
			choice = getch();
			if ( choice == 'Y' || choice == 'y' ){
				newPhoneFlag = true;
				printf("\nEnter New Phone Number:");
				scanf("%d", &newPhoneNumber);
			}
			printf("\nDo you want to update existing name: Yes[Y/y] | No[N/n]:");
			choice = getch();
			if ( choice == 'Y' || choice == 'y' ){
				newNameFlag = true;
				printf("\nEnter New Name:");
				scanf("%s", &newName);
			}
			break;
		}
	}

    /* Create merged SQL statement */
	if (newPhoneFlag && newNameFlag){
		sprintf(sqlStatement, "UPDATE PHONEBOOK set PHONE = %ld, NAME = '%s' where PHONE=%ld;" , newPhoneNumber,newName, phoneNumber);
	}
	else if(newPhoneFlag){
		sprintf(sqlStatement, "UPDATE PHONEBOOK set PHONE = %ld where PHONE=%ld;" , newPhoneNumber, phoneNumber);
	}
	else if(newNameFlag){
		sprintf(sqlStatement, "UPDATE PHONEBOOK set NAME = '%s' where PHONE=%ld;" , newName, phoneNumber);
	}

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sqlStatement, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Updated Records successfully\n");
   }
   return 0;

}   

int PhoneBook::removeEntry()
{
	if ( !(currentMenuChoice == 'R' || currentMenuChoice == 'r') ){
		return 1;
	}

	long phoneNumber			= 0;
	char contactName[BUFFER];
	char choice					= 'Y';
	bool phoneFlag				= true;

	while(1){
		printf("\nWould you like to Remove record by Phone number? Yes[Y/y] | No[N/n] ");
		choice = getch();

		if ( tolower(choice) == 'y' ){
			phoneFlag = true;
			break;
		}

		else if( tolower(choice) == 'n'){
			phoneFlag = false;
			break;
		}
		
		else{
			printf("\nEnter correct choice.....");
		}
	}

	if ( phoneFlag ){
		printf("\nEnter Phone Number:");
		scanf("%ld", &phoneNumber);
		sprintf(sqlStatement, "DELETE from PHONEBOOK where PHONE=%ld", phoneNumber);
	}
	else{
		printf("\nEnter Name:");
		scanf("%s", &contactName);
		sprintf(sqlStatement, "DELETE from PHONEBOOK where NAME='%s'", contactName);
	}

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sqlStatement, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Remove Record Operation done successfully\n");
   }
   return 0;
}

int PhoneBook::sortDatabase()
{
	if ( !(currentMenuChoice == 'S' || currentMenuChoice == 's') ){
		return 1;
	}

   /* Create SQL statement */
   sql = "SELECT * from PHONEBOOK ORDER BY NAME";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Sort Operation done successfully\n");
   }
   return 0;
}

int PhoneBook::search( bool phone, bool name, long phoneNo, char* newName )
{
	printf("\n\nSearing Results:");
	printf("\n*****************\n");

   /* Create SQL statement */
	if ( phone ){
		sprintf(sqlStatement, "SELECT * from PHONEBOOK WHERE PHONE LIKE %s%ld%s", "'%", phoneNo, "%'");
	}
	else{
		sprintf(sqlStatement, "SELECT * from PHONEBOOK WHERE NAME LIKE %s%s%s", "'%", newName, "%'");
	}

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sqlStatement, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Search Operation done successfully\n");
   }
   return 0;

}

int PhoneBook::searchDatabase()
{
	if ( !(currentMenuChoice == 'F' || currentMenuChoice == 'f') ){
		return 1;
	}

	long phoneNumber			= 0;
	char contactName[BUFFER];
	char choice					= 'Y';
	bool phoneFlag				= true;

	while(1){
		printf("\nWould you like to search by Phone number? Yes[Y/y] | No[N/n] ");
		choice = getch();
		if ( tolower(choice) == 'y'){
			phoneFlag = true;
			break;
		}
		else if( tolower(choice) == 'n'){
			phoneFlag = false;
			break;
		}
		else{
			printf("\nEnter correct choice.....");
		}
	}

	if ( phoneFlag ){
		printf("\nEnter Phone Number:");
		scanf("%ld", &phoneNumber);
		search(true, false, phoneNumber, "");
	}
	else{
		printf("\nEnter Name:");
		scanf("%s", &contactName);
		search(false, true, 0, contactName);
	}

  return 0;
}