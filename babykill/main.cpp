#include <stdio.h>
#include <windows.h>

int main( int argc, char **argv )
{
    int pid;

    // Если не получено ни одного параметра или
    // из первого параметра не получается извлечь PID (целое число)
    if ( argc < 2 || sscanf( argv[ 1 ], "%i", &pid ) != 1 ) {

        puts( "Usage: babykill <pid>" );

        // Выйти со статусом ошибки
        return 1;
    }

    // Открыть процесс, чтобы получить его хэндл
    HANDLE ph = OpenProcess( PROCESS_TERMINATE, FALSE, pid );

    // Попробовать завершить заданный процесс
    TerminateProcess( ph, 0 );

    // Выйти со статусом успеха
    return 0;
}
