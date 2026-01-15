/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS building
*   ---   install kernel
*         main
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒5‒3 S
*******************************************************************************/
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//==============================================================================
int
main(
  int argc
, char *argv[]
){  if( argc != 3 )
        return 1;
    char *src_file = argv[1];
    char *mount_device = argv[2];
    //syscall( 717, 0UL, 0UL, LOCK_UN );
    //syscall( 701, 0UL );
    long device_id = syscall( 700, mount_device );
    if( !~device_id )
    {   perror( "1" );
        return 1;
    }
    uint64_t directory_uid;
    if( !~syscall( 703, device_id, ~0UL, "system", &directory_uid ))
    {   if( errno != EEXIST )
        {   perror( "2" );
            goto Error_0;
        }
Retry_1:;
        uint64_t n = 0;
        uint64_t *list = 0;
        if( !~syscall( 707, device_id, ~0UL, &n, list ))
        {   perror( "3" );
            goto Error_0;
        }
        list = calloc( n, sizeof( uint64_t ));
        if( !list )
            goto Error_0;
        uint64_t n_ = n;
        if( !~syscall( 707, device_id, ~0UL, &n_, list ))
        {   perror( "4" );
            goto Error_0;
        }
        if( n_ != n )
        {   free(list);
            goto Retry_1;
        }
        uint64_t i;
        for( i = 0; i != n; i++ )
        {   uint64_t l = 0;
            char *name = 0;
            if( !~syscall( 709, device_id, list[i], &l, name ))
            {   perror( "5" );
                goto Error_0;
            }
            name = malloc(l);
            if( !name )
                goto Error_0;
            uint64_t l_ = l;
            if( !~syscall( 709, device_id, list[i], &l_, name ))
            {   perror( "6" );
                goto Error_0;
            }
            if( l_ != l )
            {   free(name);
                i--;
                continue;
            }
            if( !strcmp( name, "system" ))
            {   directory_uid = list[i];
                break;
            }
            free(name);
        }
        free(list);
        if( i == n )
            goto Error_0;
    }
    uint64_t file_uid;
    if( !~syscall( 705, device_id, directory_uid, "kernel", &file_uid ))
    {   if( errno != EEXIST )
        {   perror( "7" );
            goto Error_0;
        }
Retry_2:;
        uint64_t n = 0;
        uint64_t *list = 0;
        if( !~syscall( 708, device_id, directory_uid, &n, list ))
        {   perror( "8" );
            goto Error_0;
        }
        list = calloc( n, sizeof( uint64_t ));
        if( !list )
            goto Error_0;
        uint64_t n_ = n;
        if( !~syscall( 708, device_id, directory_uid, &n_, list ))
        {   perror( "9" );
            goto Error_0;
        }
        if( n_ != n )
        {   free(list);
            goto Retry_2;
        }
        uint64_t i;
        for( i = 0; i != n; i++ )
        {   uint64_t l = 0;
            char *name = 0;
            if( !~syscall( 711, device_id, list[i], &l, name ))
            {   perror( "10" );
                goto Error_0;
            }
            name = malloc(l);
            if( !name )
                goto Error_0;
            uint64_t l_ = l;
            if( !~syscall( 711, device_id, list[i], &l_, name ))
            {   perror( "11" );
                goto Error_0;
            }
            if( l_ != l )
            {   free(name);
                i--;
                continue;
            }
            if( !strcmp( name, "kernel" ))
            {   file_uid = list[i];
                break;
            }
            free(name);
        }
        free(list);
        if( i == n )
            goto Error_0;
    }
    if( !~syscall( 717, device_id, file_uid, LOCK_EX ))
    {   perror( "12" );
        goto Error_0;
    }
    if( !~syscall( 716, device_id, file_uid, 0UL ))
    {   perror( "13" );
        goto Error_1;
    }
    int src_fd = open( src_file, O_RDONLY );
    if( !~src_fd )
        goto Error_1;
    struct stat stat;
    if( !~fstat( src_fd, &stat )
    || !stat.st_size
    )
        goto Error_1;
    char *buffer = malloc( stat.st_size );
    if( !buffer )
        goto Error_1;
    ssize_t l = read( src_fd, buffer, stat.st_size );
    if( l != stat.st_size )
        goto Error_1;
    uint64_t l_ = stat.st_size;
    if( !~syscall( 719, device_id, file_uid, 0UL, &l_, buffer ))
    {   perror( "14" );
        goto Error_1;
    }
    if( l_ != stat.st_size )
        goto Error_1;
    if( !~syscall( 717, device_id, file_uid, LOCK_UN ))
    {   perror( "15" );
        return 1;
    }
    return !~syscall( 701, device_id ) ? 1 : 0;
Error_1:
    syscall( 717, device_id, file_uid, LOCK_UN );
Error_0:
    syscall( 701, device_id );
    return 1;
}
/******************************************************************************/
