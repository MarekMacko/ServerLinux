#include "conf_reader.h"
#include "reactor.h"
#include "acceptor_eh.h"

int main(int argc, char **argv)
{
	serv_sett ss;

	read_settings("config.cfg", &ss);
	
	reactor *r = 0;
	event_handler* serv_eh = 0; 

	r = create_reactor(ss.max_clients);
	if (r == 0) {
		return -1;
	}

	serv_eh = construct_acceptor(r, &ss); 
	if (serv_eh == 0) {
		destroy_reactor(r);
		return -1;
	}
	
	r->add_eh(r, serv_eh);
	r->event_loop(r);
	
	free(r);
	free(serv_eh);

	return 0;
}
