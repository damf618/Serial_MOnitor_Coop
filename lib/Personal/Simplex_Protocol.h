/*=====[TP4_Testing]==========================================================
 * Copyright 2020 Author Marquez Daniel <damf618@gmail.com>
 * All rights reserved.
 * License: license text or at least name and link
         (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 1.0.0
 * Creation Date: 2021/01/27
 */

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef SIMPLEX_PROTOCOL_H_
#define SIMPLEX_PROTOCOL_H_

/*=====[Inclusions of public function dependencies]==========================*/

#include"stdint.h"
#include"stdbool.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/
#define BUFFER_SIZE    100
#define DELIMETER      32    //ASCII for 
#define EOL            13  //ASCII for 
#define TRUE           1
#define FALSE          0
#define NO_SPACE       0
#define FIRST_SPACE    1
#define SECOND_SPACE   2
#define THIRD_SPACE    3
#define SEPARATOR_LEN  3
#define POINT_NAME_LEN 10
#define NAME_LEN       50
#define TYPE_LEN	   25
#define STATUS_LEN     25
#define FAIL_MESSAGE   "FALLA"
#define ALARM_MESSAGE  "ALARMA"
#define SIMPLEX_NAME_MAX_LEN 39
#define BEGIN          0

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

typedef enum{ FINAL, SPACE, DATA} message_event_t;

typedef struct message_parser_s{
	char point_name[POINT_NAME_LEN];
	char name[NAME_LEN];
	char type[TYPE_LEN];
	char status[STATUS_LEN];
	bool text_extract;
	char space_counter;
	char sep_counter;
	int index;
	bool data_flag;
}message_parser_t;


/*=====[Prototypes (declarations) of public functions]=======================*/

//message_parser_t Separator_Search(char* data);
message_parser_t Separator_Search(char *data);

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* SIMPLEX_PROTOCOL_H_ */
