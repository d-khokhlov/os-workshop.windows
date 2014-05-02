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

void dir( )
{
    STARTUPINFO siChild;
    ZeroMemory( &siChild, sizeof( STARTUPINFO ) );
    siChild.cb = sizeof( STARTUPINFO );

    PROCESS_INFORMATION piChild;

    BOOL isCreated = CreateProcess( NULL, "cmd /c dir", NULL, NULL, TRUE, 0, NULL, NULL,
        &siChild, &piChild );

    if ( isCreated ) {
        CloseHandle( piChild.hThread );
        WaitForSingleObject( piChild.hProcess, INFINITE );
        CloseHandle( piChild.hProcess );
    }
    printf( "\n" );
}

void attrib( char *filename )
{
    char command[ BUFFER_SIZE ] = "attrib ";
    strcat( command, filename );

    STARTUPINFO siChild;
    ZeroMemory( &siChild, sizeof( STARTUPINFO ) );
    siChild.cb = sizeof( STARTUPINFO );

    PROCESS_INFORMATION piChild;

    BOOL isCreated = CreateProcess( NULL, command, NULL, NULL, TRUE, 0, NULL, NULL,
        &siChild, &piChild );

    if ( isCreated ) {
        CloseHandle( piChild.hThread );
        WaitForSingleObject( piChild.hProcess, INFINITE );
        CloseHandle( piChild.hProcess );
    }
    printf( "\n" );
}

void testFsCalls( )
{
    char *dirname = "some-test-dir";

    printf( "�������� ���������� '%s'.\n", dirname );
    CreateDirectory( dirname, NULL );

    printf( "���, ��� ����������:\n" );
    dir( );

    printf( "������ ������ ��� ����������.\n" );
    RemoveDirectory( dirname );

    printf( "���, ��� ���������� ������:\n" );
    dir( );

    char *filename = "some-test-file";
    char *linkname = "some-test-link";

    printf( "������ �������� ���� '%s'.\n", filename );
    CloseHandle( CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) );

    printf( "� �������� ������ �� ���� � ������ '%s'.\n", linkname );
    CreateHardLink( linkname, filename, NULL );

    printf( "�������� ���������:\n" );
    dir( );

    printf( "������� ������ �������� ��� �����:\n" );
    DeleteFile( filename );
    dir( );

    printf( "� ������ ������ ������ ��� ����� (������):\n" );
    DeleteFile( linkname );
    dir( );
}

void testOtherCalls( )
{
    char *filename = "temp-file-for-chmod-test";

    printf( "�������� � ���������� ��� ��������� ������:\n" );
    char homePath[ BUFFER_SIZE ];
    GetEnvironmentVariable( "USERPROFILE", homePath, BUFFER_SIZE );
    strcat( homePath, "\\" );
    SetCurrentDirectory( homePath );
    dir( );

    printf( "�������� ����� ���� '%s' � ��������� '������ ��� ������':\n", filename );
    CloseHandle( CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_READONLY, NULL ) );
    attrib( filename );

    printf( "������ ������ ������� '������ ��� ������' � ����� �����:\n" );
    SetFileAttributes( filename, FILE_ATTRIBUTE_NORMAL );
    attrib( filename );

    DeleteFile( filename );

    SYSTEMTIME time;
    GetLocalTime( &time );
    printf( "������� ���� � �����: %02hu.%02hu.%04hu %02hu:%02hu:%02hu\n\n", time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute, time.wSecond );

    char *command = "ping -t google.com";
    printf( "������ �������� �������� ������� '%s'.\n", command );
    printf( "������� ENTER, ����� ������ ������.\n����� ��������� ���� �������� �������, ������� ENTER ��� ���.\n" );
    getchar( );

    STARTUPINFO siChild;
    ZeroMemory( &siChild, sizeof( STARTUPINFO ) );
    siChild.cb = sizeof( STARTUPINFO );

    PROCESS_INFORMATION piChild;

    BOOL isCreated = CreateProcess( NULL, command, NULL, NULL, TRUE, 0, NULL, NULL,
        &siChild, &piChild );

    if ( isCreated ) {

        CloseHandle( piChild.hThread );

        getchar( );
        TerminateProcess( piChild.hProcess, 0 );

        CloseHandle( piChild.hProcess );

        printf( "�������� ������� ��������.\n" );
    }
}

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
