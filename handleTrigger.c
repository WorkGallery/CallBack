#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>

		MYSQL *con;
		MYSQL_RES *result;
		MYSQL_ROW row;
		const char *trigger;
		const char *column_name;
		const char *new_value;
		int set_bit = 0;
		pid_t childpid;

void callback()
{
	
	printf("In callback Trigger name: %s\n", trigger);
	if (strcmp(trigger, "port_trigger") == 0)
	{
		printf("In port trigger\n");
		mysql_free_result(result);
		if (mysql_query(con, "SELECT column_name, new_value FROM port_log WHERE updated_at = (SELECT MAX(updated_at) FROM port_log)"))
		{
			fprintf(stderr, "Error querying database: %s\n", mysql_error(con));
			mysql_close(con);
			exit(0);
		}//end of if

		result = mysql_use_result(con);

		while ((row = mysql_fetch_row(result)) != NULL){
			printf("Column name: %s\n", row[0]);
			printf("Updated value: %s\n", row[1]);
		}
	
	}

	else if (strcmp(trigger, "user_trigger") == 0)
	{

		printf("In user trigger\n");
		
		if (mysql_query(con, "SELECT column_name, new_value FROM user_log WHERE updated_at = (SELECT MAX(updated_at) FROM user_log)"))
		{
			fprintf(stderr, "Error querying database: %s\n", mysql_error(con));
			mysql_close(con);
			exit(0);
		}	//end of if

		result = mysql_use_result(con);

		while ((row = mysql_fetch_row(result)) != NULL)
		{
				printf("Column name: %s\n", row[0]);
				printf("Updated value: %s\n", row[1]);

		}	// end of while

	}	// End of else if

	else if (strcmp(trigger, "kvm_trigger") == 0)
	{

		printf("Kvm trigger\n");
		if (mysql_query(con, "SELECT column_name, new_value FROM kvm_log WHERE updated_at = (SELECT MAX(updated_at) FROM kvm_log)"))
		{
			fprintf(stderr, "Error querying database: %s\n", mysql_error(con));
			printf("hello\n");
			mysql_close(con);
			exit(0);
		}	//end of if

		result = mysql_use_result(con);

		while ((row = mysql_fetch_row(result)) != NULL)
		{
			printf("Column name: %s\n", row[0]);
			printf("Updated value: %s\n", row[1]);

		}	//end of while loop
		
	}	//end of else if 

	else
		printf("Trigger is null\n");
		set_bit = 0;
}	//End of callback function

char* checkTrigger(void (*fn)(void) )
{
		set_bit =1;
		if (mysql_query(con, "SELECT trigger_name FROM trigger_log WHERE event_time = (SELECT MAX(event_time) FROM trigger_log);"))
		{
			fprintf(stderr, "Error querying database: %s\n", mysql_error(con));
			mysql_close(con);
			return 1;
		}

		result = mysql_store_result(con);

		while ((row = mysql_fetch_row(result)) != NULL)
		{
			trigger = row[0];
			printf("Trigger name: %s\n", trigger);
		}	//end of while
	return *trigger;

} //End of checktrigger function

int main()
{
	
	con = mysql_init(NULL);

	if (mysql_real_connect(con, "localhost", "root", "Nikita@2207", "kvm", 0, NULL, 0) == NULL)
		printf("mysql_real_connect() failed\n");
	
	else
		printf("database connected!\n");

	//registering callback with function pointer	
	void (*fn_ptr)( void ) = &callback;

	//calling callback with function pointer

		while(1){

		if(checkTrigger(fn_ptr)){
			callback();
			sleep(10);
		}
		else
			sleep(10);
		mysql_free_result(result);
}
	
	mysql_close(con);
	return 0;
} //End of main function