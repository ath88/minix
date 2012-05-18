#ifndef _PROS_PROTO_H
#define _PROS_PROTO_H

/* Function prototypes. */

/* main.c */
_PROTOTYPE(int main, (int argc, char **argv)                    );
_PROTOTYPE(void reply, (endpoint_t whom, message *m_ptr)        );
_PROTOTYPE(void write_buffer, (message *m_ptr)                  );
_PROTOTYPE(void do_ctl, (message *m_ptr)                        );
_PROTOTYPE(void do_ctl, (message *m_ptr)                        );
_PROTOTYPE(void attach_memory, (key_t key1, key_t key2)         );


#endif /* _PROS_PROTO_H */
