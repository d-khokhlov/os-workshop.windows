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

    puts( "Введите строку:" );

    char input[ BUFFER_SIZE ];
    fgets( input, BUFFER_SIZE, stdin );

    HANDLE fh = CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    WriteFile( fh, input, strlen( input ), NULL, NULL );
    CloseHandle( fh );

    printf( "Ваша строка сохранена в файл '%s'.\n", filename );

    char output[ BUFFER_SIZE ];
    fh = CreateFile( filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    DWORD length;
    ReadFile( fh, output, BUFFER_SIZE, &length, NULL );

    puts( "Вот что удалось прочитать из этого файла (должна быть та же строка):" );
    output[ length ] = 0;
    fputs( output, stdout );

    puts( "Попробуем вернуться в начало файла и снова прочитать первый символ:" );
    SetFilePointer( fh, 0, 0, FILE_BEGIN );
    ReadFile( fh, output, 1, NULL, NULL );
    output[ 1 ] = 0;
    puts( output );

    CloseHandle( fh );

    WIN32_FILE_ATTRIBUTE_DATA info;
    GetFileAttributesEx( filename, GetFileExInfoStandard, &info );

    printf( "Атрибуты данного файла: 0x%08lx. Размер в байтах: %li.\n", info.dwFileAttributes, info.nFileSizeLow );
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

    printf( "Создадим директорию '%s'.\n", dirname );
    CreateDirectory( dirname, NULL );

    printf( "Вот, что получилось:\n" );
    dir( );

    printf( "Теперь удалим эту директорию.\n" );
    RemoveDirectory( dirname );

    printf( "Вот, что получилось теперь:\n" );
    dir( );

    char *filename = "some-test-file";
    char *linkname = "some-test-link";

    printf( "Теперь создадим файл '%s'.\n", filename );
    CloseHandle( CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) );

    printf( "И создадим ссылку на него с именем '%s'.\n", linkname );
    CreateHardLink( linkname, filename, NULL );

    printf( "Получили следующее:\n" );
    dir( );

    printf( "Сначала удалим исходное имя файла:\n" );
    DeleteFile( filename );
    dir( );

    printf( "А теперь удалим второе имя файла (ссылку):\n" );
    DeleteFile( linkname );
    dir( );
}

void testOtherCalls( )
{
    char *filename = "temp-file-for-chmod-test";

    printf( "Перейдем в директорию для временных файлов:\n" );
    char homePath[ BUFFER_SIZE ];
    GetEnvironmentVariable( "USERPROFILE", homePath, BUFFER_SIZE );
    strcat( homePath, "\\" );
    SetCurrentDirectory( homePath );
    dir( );

    printf( "Создадим здесь файл '%s' с атрибутом 'только для чтения':\n", filename );
    CloseHandle( CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_READONLY, NULL ) );
    attrib( filename );

    printf( "Теперь снимем атрибут 'только для чтения' с этого файла:\n" );
    SetFileAttributes( filename, FILE_ATTRIBUTE_NORMAL );
    attrib( filename );

    DeleteFile( filename );

    SYSTEMTIME time;
    GetLocalTime( &time );
    printf( "Текущие дата и время: %02hu.%02hu.%04hu %02hu:%02hu:%02hu\n\n", time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute, time.wSecond );

    char *command = "ping -t google.com";
    printf( "Теперь создадим дочерний процесс '%s'.\n", command );
    printf( "Нажмите ENTER, когда будете готовы.\nЧтобы завершить этот дочерний процесс, нажмите ENTER еще раз.\n" );
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

        printf( "Дочерний процесс завершен.\n" );
    }
}

int main( int argc, char **argv )
{
    setlocale( LC_ALL, "Russian" );

    printf( "\nДемонстрация системных вызовов управления процессами\n\n" );
    testProcessCalls( );
    printf( "\n(для продолжения нажмите ENTER)\n" );
    getchar( );

    printf( "\nДемонстрация системных вызовов файлового ввода/вывода\n\n" );
    testFileCalls( );
    printf( "\n(для продолжения нажмите ENTER)\n" );
    getchar( );

    printf( "\nДемонстрация системных вызовов управления файловой системой\n\n" );
    testFsCalls( );
    printf( "(для продолжения нажмите ENTER)\n" );
    getchar( );

    printf( "\nДемонстрация прочих системных вызовов\n\n" );
    testOtherCalls( );

    return 0;
}
