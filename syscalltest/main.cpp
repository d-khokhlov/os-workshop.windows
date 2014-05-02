#include <stdio.h>
#include <windows.h>
#include <locale.h>

#define BUFFER_SIZE 100

void testProcessCalls( )
{

    printf( "parent: started\n" );

    STARTUPINFO siChild;
    ZeroMemory( &siChild, sizeof( STARTUPINFO ) );
    siChild.cb = sizeof( STARTUPINFO );

    int i;
    for ( i = 0; i < 3; i++ ) {

        PROCESS_INFORMATION piChild;
        char command[ BUFFER_SIZE ];
        sprintf( command, "cmd /c echo child %i: echo: executed", i );
        BOOL isCreated = CreateProcess( NULL, command, NULL, NULL, TRUE, 0, NULL, NULL,
            &siChild, &piChild );
        if ( isCreated ) {

            CloseHandle( piChild.hThread );

            printf( "parent: child %i created with PID %lu\n", i, piChild.dwProcessId );

            WaitForSingleObject( piChild.hProcess, INFINITE );

            DWORD status;
            GetExitCodeProcess( piChild.hProcess, &status );
            printf( "parent: child %i exited with status %lu\n", i, status );

            CloseHandle( piChild.hProcess );

        } else {
            printf( "parent: failed to create child %i\n" );
        }
    }

    printf( "parent: exiting\n" );
}

void testFileCalls( )
{
    /*
    char *filename = "input.txt";

    puts( "������� ������:" );

    char input[ BUFFER_SIZE ];
    fgets( input, BUFFER_SIZE, stdin );

    int fd = open( filename, O_CREAT | O_TRUNC | O_WRONLY, 0666 );
    write( fd, input, strlen( input ) );
    close( fd );

    printf( "���� ������ ��������� � ���� '%s'.\n", filename );

    char output[ BUFFER_SIZE ];
    fd = open( filename, O_RDONLY );
    int length = read( fd, output, BUFFER_SIZE );

    puts( "��� ��� ������� ��������� �� ����� ����� (������ ���� �� �� ������):" );
    output[ length ] = 0;
    fputs( output, stdout );

    puts( "��������� ��������� � ������ ����� � ����� ��������� ������ ������:" );
    lseek( fd, 0, SEEK_SET );
    read( fd, output, 1 );
    output[ 1 ] = 0;
    puts( output );

    close( fd );

    struct stat info;
    stat( filename, &info );

    printf( "������ ���� ����� UID %u, GID %u. ��� ������ � ������: %li.\n", info.st_uid, info.st_gid, info.st_size );
*/}

void ls( )
{/*
    if ( fork( ) == 0 ) {
        execlp( "ls", "ls", "-lAh", NULL );
        exit( 1 );
    }
    wait( NULL );
    printf( "\n" );
*/}

void testFsCalls( )
{
    /*
    char *dirname = "some-test-dir";

    printf( "�������� ���������� '%s'.\n", dirname );
    mkdir( dirname, 0777 );

    printf( "���, ��� ����������:\n" );
    ls( );

    printf( "������ ������ ��� ����������.\n" );
    rmdir( dirname );

    printf( "���, ��� ���������� ������:\n" );
    ls( );

    char *filename = "some-test-file";
    char *linkname = "some-test-link";

    printf( "������ �������� ���� '%s'.\n", filename );
    close( open( filename, O_CREAT | O_TRUNC | O_WRONLY, 0666 ) );

    printf( "� �������� ������ �� ���� � ������ '%s'.\n", linkname );
    link( filename, linkname );

    printf( "�������� ��������� (������� �������� �� ������ �������):\n" );
    ls( );

    printf( "������� ������ (unlink) �������� ��� �����:\n" );
    unlink( filename );
    ls( );

    printf( "� ������ ������ ������ ��� ����� (������):\n" );
    unlink( linkname );
    ls( );
*/}

void testOtherCalls( )
{
    /*
    char *filename = "temp-file-for-chmod-test";

    printf( "�������� � '/tmp':\n" );
    chdir( "/tmp" );
    ls( );

    printf( "�������� ����� ���� '%s' � ����������� ������� �������:\n", filename );
    close( open( filename, O_CREAT | O_TRUNC | O_WRONLY, 0400 ) );
    ls( );

    printf( "������ �������� ������� ������� ��� ����� �����:\n" );
    chmod( filename, 0777 );
    ls( );

    unlink( filename );

    printf( "������ �������� (fork) �������� �������,\n������� ������ ������� ����� �������� ������� ����� �������.\n" );
    printf( "������� ENTER, ����� ������ ������.\n����� ��������� ���� �������� �������, ������� ENTER ��� ���.\n" );
    getchar( );

    int pid = fork( );
    if ( pid == 0 ) {
        while ( 1 ) {
            printf( "%li\n", time( NULL ) );
            sleep( 1 );
        }
        exit( 0 );
    }

    getchar( );
    kill( pid, SIGKILL );
    printf( "�������� ������� ��������.\n" );
*/}

int main( int argc, char **argv )
{
    setlocale( LC_ALL, "Russian" );

    printf( "\n������������ ��������� ������� ���������� ����������\n\n" );
    testProcessCalls( );
    printf( "\n(��� ����������� ������� ENTER)\n" );
    getchar( );

    printf( "\n������������ ��������� ������� ��������� �����/������\n\n" );
    testFileCalls( );
    printf( "\n(��� ����������� ������� ENTER)\n" );
    getchar( );

    printf( "\n������������ ��������� ������� ���������� �������� ��������\n\n" );
    testFsCalls( );
    printf( "(��� ����������� ������� ENTER)\n" );
    getchar( );

    printf( "\n������������ ������ ��������� �������\n\n" );
    testOtherCalls( );

    return 0;
}
