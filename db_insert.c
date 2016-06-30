//  NOTE : If the prepare operation was unsuccessful 
// (that is, mysql_stmt_prepare() returns nonzero), 
// the error message can be obtained by calling mysql_stmt_error().
/* This function insert into DB the read data from sensor*/

int db_insert (short type, short id, float reading){

	short sql_error = 0;
	MYSQL_STMT *sql_statement1;
	MYSQL_BIND sql_bind_parameters1[3];
	

	//Setup to create query
	sql_statement1= mysql_stmt_init(mysql1);
	
	if (!sql_statement1){
		sql_error = 1;
		fprintf (stderr,"Error in %d fallo mysql_stmt_init(mysql1)\n",sql_error);
	}
	
	//----- SET THE QUERY TEXT -----
	#define SQL_QUERY_TEXT_1 "INSERT INTO \
	sensores_registros(id_sensor,registro,created_at) \
	VALUES(?,?,now())"
	
	if (mysql_stmt_prepare(sql_statement1, SQL_QUERY_TEXT_1, strlen(SQL_QUERY_TEXT_1))){
		sql_error = 1;
		fprintf (stderr,"Error in %d fallo mysql_stmt_prepare(sql_statement1, SQL_QUERY_TEXT_1, strlen(SQL_QUERY_TEXT_1))\n",sql_error);
	}

	//----- SET THE QUERY PARAMETER VALUES -----
	//If you want to know how many parameters are expected
	//int param_count = mysql_stmt_param_count(sql_statement1);
	
	//Set the parameter values
	memset(sql_bind_parameters1, 0, sizeof(sql_bind_parameters1));		//Reset the parameters memory to null
	
	resolvSensorID (type,id);
	//fprintf (stderr,"Inserting into DB %s \n",str);
	//Integer
	/*char str_data[8] = "TEMP01";
	
	unsigned long str_length = strlen(str_data);
	sql_bind_parameters1[0].buffer_type = MYSQL_TYPE_STRING;
	sql_bind_parameters1[0].buffer = (char*)str_data;
	sql_bind_parameters1[0].buffer_length = sizeof(str_data);
	sql_bind_parameters1[0].is_null = 0;
	sql_bind_parameters1[0].length = &str_length;			//<Note: this is a pointer!
	*/
	
	unsigned long str_length = strlen(str);
	sql_bind_parameters1[0].buffer_type = MYSQL_TYPE_STRING;
	sql_bind_parameters1[0].buffer = (char*)str;
	sql_bind_parameters1[0].buffer_length = sizeof(str);
	sql_bind_parameters1[0].is_null = 0;
	sql_bind_parameters1[0].length = &str_length;			//<Note: this is a pointer!
	//Integer
	/*int int_data1 = 20;
	sql_bind_parameters1[1].buffer_type = MYSQL_TYPE_LONG;
	sql_bind_parameters1[1].buffer = (char*)&int_data1;		//<Note: this is a pointer!
	sql_bind_parameters1[1].is_null = 0;
	sql_bind_parameters1[1].length = 0;
	*/
	
	//Float
	//float int_data1 = 20.5;
	sql_bind_parameters1[1].buffer_type = MYSQL_TYPE_FLOAT;
	sql_bind_parameters1[1].buffer = (char*)&reading;		//<Note: this is a pointer!
	sql_bind_parameters1[1].is_null = 0;
	sql_bind_parameters1[1].length = 0;

	
	//string
	/*char str_data[50] = "Hello";
	unsigned long str_length = strlen(str_data);
	sql_bind_parameters1[1].buffer_type = MYSQL_TYPE_STRING;
	sql_bind_parameters1[1].buffer = (char*)str_data;
	sql_bind_parameters1[1].buffer_length = sizeof(str_data);
	sql_bind_parameters1[1].is_null = 0;
	sql_bind_parameters1[1].length = &str_length;			//<Note: this is a pointer!
	*/
	//smallint
	/*short small_data;
	my_bool is_null = 1;		//We'll store this as null in this example
	sql_bind_parameters1[2].buffer_type = MYSQL_TYPE_SHORT;
	sql_bind_parameters1[2].buffer = (char*)&small_data;	//<Note: this is a pointer!
	sql_bind_parameters1[2].is_null = &is_null;				//<Note: this is a pointer!
	sql_bind_parameters1[2].length = 0;
	*/
	//Pointers are used in the bind parameters so that if you are say adding multiple rows you can use the same query setup with new values for each execute of it.
	
	//Bind the buffers
	if (mysql_stmt_bind_param(sql_statement1, sql_bind_parameters1)){
		sql_error = 1;
		fprintf (stderr,"Error in %d fallo mysql_stmt_bind_param(sql_statement1, sql_bind_parameters1)\n",sql_error);
	}

	//----- EXECUTE THE QUERY ------
	if (!sql_error){
		if (mysql_stmt_execute(sql_statement1)){
			sql_error = 1;
			//fprintf (stderr, "%s\n", sql_statement1);
			fprintf (stderr,"Error in %d fallo. mysql_stmt_execute(sql_statement1)\n",sql_error);
		}
	}

	//If you want to get the number of affected rows
	//long affected_rows = mysql_stmt_affected_rows(sql_statement1);
	//if (affected_rows != 1)
	//{
	//	do something
	//}
	//fprintf (stderr,"Affected Rows %d \n", affected_rows); 
	//fprintf (stderr, "INSERT STMT: %s \n", sql_statement1);
	//IF YOU WANT TO GET THE VALUE GENERATED FOR AN AUTO_INCREMENT COLUMN IN THE PREVIOUS INSERT/UPDATE STATEMENT
	//my_ulonglong sql_insert_id = mysql_stmt_insert_id(sql_statement1);

	//If you want to do the query again then change any values you want to change and call mysql_stmt_execute(sql_statement1) again

	//Close the statement
	if (sql_statement1){
		if (mysql_stmt_close(sql_statement1)){
			sql_error = 1;
			fprintf (stderr,"Error in %d fallo.mysql_stmt_close(sql_statement1)\n",sql_error);
		}	
		//else
		//fprintf (stderr,"Commit Succefull \n");
	}
	
	return sql_error;
}

//*******************************************************************************
//****************************** END C SCRIPT ***********************************
//*******************************************************************************

