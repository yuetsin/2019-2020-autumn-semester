#include "jsl_log.h"
// #include "lock_server.h"
#include "lock_server_cache.h"
#include "rpc.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Main loop of lock_server

int main( int argc, char* argv[] ) {
    int count = 0;

    setvbuf( stdout, NULL, _IONBF, 0 );
    setvbuf( stderr, NULL, _IONBF, 0 );

    srandom( getpid() );

    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s port\n", argv[ 0 ] );
        exit( 1 );
    }

    char* count_env = getenv( "RPC_COUNT" );
    if ( count_env != NULL ) {
        count = atoi( count_env );
    }

#ifndef RSM
    lock_server_cache ls;
    rpcs              server( atoi( argv[ 1 ] ), count );
    server.reg( lock_protocol::stat, &ls, &lock_server_cache::stat );
    server.reg( lock_protocol::acquire, &ls, &lock_server_cache::acquire );
    server.reg( lock_protocol::release, &ls, &lock_server_cache::release );
#endif

    while ( 1 )
        sleep( 1000 );
}