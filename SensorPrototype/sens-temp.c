#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "net/rpl/rpl.h"
#include "rest-engine.h"
//#include "project-conf.h"


#define STARTING_TEMPERATURE 0


// First order variable
static int u_k_1 = (STARTING_TEMPERATURE+1);
static int u_k = (STARTING_TEMPERATURE+1);
static int temp_k = STARTING_TEMPERATURE;	// temperature at current time
static int temp_k_1 = STARTING_TEMPERATURE;	// temperature last sample

int room_id = 0;
 
void id_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
void id_post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
void temp_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
void temp_post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void temp_periodic_handler();

PERIODIC_RESOURCE(temp_sens,"title=\"TempR\";rt=\"S\";obs", temp_get_handler,temp_post_handler,NULL,NULL, TIME_SAMPLING,temp_periodic_handler);

/*
*	Resource used only for simulations
*/
//RESOURCE(set_temp_environment, "title=\"Set_Temp\";rt=\"P\"", NULL, temp_post_handler, NULL, NULL);

RESOURCE(Id, "title=\"RoomId\";rt=\"Id\"", id_get_handler, id_post_handler, NULL, NULL);

void id_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	/* Populat the buffer with the response payload*/
	char message[30];
	int length = 30;

	sprintf(message, "{'type':'room', 'id':'%d'}", room_id);
	length = strlen(message);
	memcpy(buffer, message, length);

	REST.set_header_content_type(response, REST.type.TEXT_PLAIN); 
	REST.set_header_etag(response, (uint8_t *) &length, 1);
	REST.set_response_payload(response, buffer, length);
}

void id_post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int new_id, len;
  const char *val = NULL;
     
  len=REST.get_post_variable(request, "e", &val);
     
  if( len > 0 ){
     new_id = atoi(val);	
     room_id = new_id;
     REST.set_response_status(response, REST.status.CREATED);
  } else {
     REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}


void temp_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	/* Populat the buffer with the response payload*/
	char message[23];
	int length = 23;

	sprintf(message, "{'e':'%03d','u':'C'}", ((int) temp_k));
	length = strlen(message);
	memcpy(buffer, message, length);

	REST.set_header_content_type(response, REST.type.TEXT_PLAIN); 
	REST.set_header_etag(response, (uint8_t *) &length, 1);
	REST.set_response_payload(response, buffer, length);
}

void temp_post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int new_value, len;
  const char *val = NULL;
     
  len=REST.get_post_variable(request, "e", &val);
     
  if( len > 0 ){
     new_value = atoi(val);	
     u_k = new_value + 1; 
     /* 
     * because it doesn't reach the final value. It's done only for the coherence of the
     * simulation
     */
     REST.set_response_status(response, REST.status.CREATED);
  } else {
     REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}

static void temp_periodic_handler()
{
	temp_k_1 = temp_k;
	temp_k = (9 * temp_k_1) + (u_k_1);
	temp_k = temp_k / 10;
	if (((int) temp_k) != ((int) temp_k_1))
		REST.notify_subscribers(&temp_sens);
	u_k_1 = u_k;
}

PROCESS(temperature_process, "Temperature sensor");

AUTOSTART_PROCESSES(&temperature_process);

PROCESS_THREAD(temperature_process, ev, data)
{
	PROCESS_BEGIN();



	/* Initialize Rest engine */
	rest_init_engine();

	/* Activate the application-specific resources */
	rest_activate_resource(&temp_sens, "tempr");
		
	//rest_activate_resource(&set_temp_environment, "set_t");
	rest_activate_resource(&Id, "id");


	while(1) {
		PROCESS_WAIT_EVENT();
	}

	PROCESS_END();
}