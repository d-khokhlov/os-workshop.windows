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

    printf( "Создадим директорию '%s'.\n", dirname );
    mkdir( dirname, 0777 );

    printf( "Вот, что получилось:\n" );
    ls( );

    printf( "Теперь удалим эту директорию.\n" );
    rmdir( dirname );

    printf( "Вот, что получилось теперь:\n" );
    ls( );

    char *filename = "some-test-file";
    char *linkname = "some-test-link";

    printf( "Теперь создадим файл '%s'.\n", filename );
    close( open( filename, O_CREAT | O_TRUNC | O_WRONLY, 0666 ) );

    printf( "И создадим ссылку на него с именем '%s'.\n", linkname );
    link( filename, linkname );

    printf( "Получили следующее (обратим внимание на второй столбец):\n" );
    ls( );

    printf( "Сначала удалим (unlink) исходное имя файла:\n" );
    unlink( filename );
    ls( );

    printf( "А теперь удалим второе имя файла (ссылку):\n" );
    unlink( linkname );
    ls( );
*/}

void testOtherCalls( )
{
    /*
    char *filename = "temp-file-for-chmod-test";

    printf( "Перейдем в '/tmp':\n" );
    chdir( "/tmp" );
    ls( );

    printf( "Создадим здесь файл '%s' с минимальным уровнем доступа:\n", filename );
    close( open( filename, O_CREAT | O_TRUNC | O_WRONLY, 0400 ) );
    ls( );

    printf( "Теперь расширим уровень доступа для этого файла:\n" );
    chmod( filename, 0777 );
    ls( );

    unlink( filename );

    printf( "Теперь создадим (fork) дочерний процесс,\nкоторый каждую секунду будет выводить текущую метку времени.\n" );
    printf( "Нажмите ENTER, когда будете готовы.\nЧтобы завершить этот дочерний процесс, нажмите ENTER еще раз.\n" );
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
    printf( "Дочерний процесс завершен.\n" );
*/}

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
