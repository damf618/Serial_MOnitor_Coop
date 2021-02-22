/*=====[TP4_Testing]==========================================================
 * Copyright 2020 Author Marquez Daniel <damf618@gmail.com>
 * All rights reserved.
 * License: license text or at least name and link
         (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 1.0.0
 * Creation Date: 2020/07/25
 */

/*=====[Inclusions of function dependencies]=================================*/

#include "Simplex_Protocol.h"
#include <stdio.h>
#include <string.h>

/*=====[Definition macros of public constants]===============================*/

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public functions]=======================*/

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

void string_cleaner(message_parser_t *rtn)
{
	memset(rtn->point_name, 0, POINT_NAME_LEN);
	memset(rtn->name, 0, NAME_LEN);
	memset(rtn->type, 0, TYPE_LEN);
	memset(rtn->status, 0, STATUS_LEN);
}

void Message_Setup(message_parser_t *rtn)
{
	rtn->text_extract = TRUE;
	rtn->space_counter = 0;
	rtn->sep_counter = 0;
	rtn->index = 0;
	string_cleaner(rtn);
}

void Separator_Extract(message_parser_t *rtn, char msg_char, int index)
{
	rtn->space_counter = 0;
	rtn->data_flag = TRUE;

	switch (rtn->sep_counter)
	{
	case NO_SPACE:
		rtn->point_name[rtn->index] = msg_char;
		rtn->index++;
		break;

	case FIRST_SPACE:
		rtn->name[rtn->index] = msg_char;
		rtn->index++;
		break;

	case SECOND_SPACE:
		rtn->type[rtn->index] = msg_char;
		rtn->index++;
		break;

	case THIRD_SPACE:
		rtn->status[rtn->index] = msg_char;
		rtn->index++;
		break;
	
	default:
		//printf("Error");
		#ifdef TEST
			Serial.println("Error in Segmentation");
		#endif
		break;
	}
}

void Separator_Eval(message_parser_t *rtn, int i, char msg_char)
{
	rtn->space_counter++;
	if (2 == rtn->space_counter)
	{
		rtn->sep_counter++;
		rtn->index = 0;
	}
	else
	{
		if (rtn->data_flag)
		{
			Separator_Extract(rtn, msg_char, i);
			rtn->space_counter++;
			rtn->data_flag = FALSE;
		}
	}
}

void Message_Final_Touch(message_parser_t *rtn)
{
	int name_real_length=0;
	char new_name[NAME_LEN];

	rtn->point_name[strlen(rtn->point_name) - 1] = 0;
	
	for(int i=SIMPLEX_NAME_MAX_LEN;i>=BEGIN;i--)
	{
		if(DELIMETER!=rtn->name[i])
		{
			name_real_length=i;
			break;
		}
	}
	memcpy(new_name,rtn->name,(name_real_length+1)*sizeof(char));
	memset(rtn->name, 0, NAME_LEN);
	memcpy(rtn->name,new_name,(name_real_length+1)*sizeof(char));
	
	if (0 == rtn->status[0])
	{
		memcpy(rtn->status, FAIL_MESSAGE, TYPE_LEN * sizeof(char));
		memset(rtn->type, 0, STATUS_LEN);
	}
	else
	{
		rtn->type[strlen(rtn->type) - 1] = 0;
		memcpy(rtn->status, FAIL_MESSAGE, TYPE_LEN * sizeof(char));
	}
}

message_parser_t Separator_Search(char *data)
{
	message_parser_t rtn;
	message_event_t element;

	Message_Setup(&rtn);
	//bool space_flag;
	//bool separator_flag;
	int name_counter=-1;
	bool name_flag=FALSE;

	for (int i = 0; i <= BUFFER_SIZE; i++)
	{
		if (DELIMETER == data[i])
		{
			element = SPACE;
		}
		else if (EOL == data[i])
		{
			element = FINAL;
		}
		else
		{
			element = DATA;
		}  

		if ((FIRST_SPACE == rtn.sep_counter)&&(DATA == element)&&(!name_flag))
		{
			name_counter=BEGIN;
			name_flag=TRUE;
		}	

		if((name_counter>=BEGIN)&&(name_counter<=SIMPLEX_NAME_MAX_LEN))
		{
			name_counter++;
			Separator_Extract(&rtn, data[i], i);
		}
		else
		{
			switch (element)
			{
			case DATA:
				Separator_Extract(&rtn, data[i], i);
				break;

			case SPACE:
				Separator_Eval(&rtn, i, data[i]);
				break;

			case FINAL:
				i = BUFFER_SIZE + 1;
				break;

			default:
				//printf("Undefined char");
				#ifdef TEST
					Serial.println("Error unexpected char");
				#endif
				break;
			}
		}
	}
	Message_Final_Touch(&rtn);
	return rtn;
}
