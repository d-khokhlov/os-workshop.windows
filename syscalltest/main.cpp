#include <stdio.h>
#include <windows.h>
#include <locale.h>

#define BUFFER_SIZE 100

void testProcessCalls( )
{
    printf( "parent: started\n" );

    int i;
    for ( i = 0; i < 3; i++ ) {

        char command[ BUFFER_SIZE ];
        sprintf( command, "cmd /c echo child %i: echo: executed", i );

        STARTUPINFO siChild;
        ZeroMemory( &siChild, sizeof( STARTUPINFO ) );
        siChild.cb = sizeof( STARTUPINFO );

        PROCESS_INFORMATION piChild;

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
    char *filename = "input.txt";

    puts( "������� ������:" );

    char input[ BUFFER_SIZE ];
    fgets( input, BUFFER_SIZE, stdin );

    HANDLE fh = CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    WriteFile( fh, input, strlen( input ), NULL, NULL );
    CloseHandle( fh );

    printf( "���� ������ ��������� � ���� '%s'.\n", filename );

    char output[ BUFFER_SIZE ];
    fh = CreateFile( filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    DWORD length;
    ReadFile( fh, output, BUFFER_SIZE, &length, NULL );

    puts( "��� ��� ������� ��������� �� ����� ����� (������ ���� �� �� ������):" );
    output[ length ] = 0;
    fputs( output, stdout );

    puts( "��������� ��������� � ������ ����� � ����� ��������� ������ ������:" );
    SetFilePointer( fh, 0, 0, FILE_BEGIN );
    ReadFile( fh, output, 1, NULL, NULL );
    output[ 1 ] = 0;
    puts( output );

    CloseHandle( fh );

    WIN32_FILE_ATTRIBUTE_DATA info;
    GetFileAttributesEx( filename, GetFileExInfoStandard, &info );

    printf( "�������� ������� �����: 0x%08lx. ������ � ������: %li.\n", info.dwFileAttributes, info.nFileSizeLow );
}

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
