#include <stdio.h>
#include <windows.h>

#define BUFFER_SIZE 100

void printUsageAndExit()
{
    puts( "Usage: babycat <in_filename1> [<in_filename2> ...] <out_filename>" );

    // Выйти со статусом ошибки
    ExitProcess( 1 );
}

int main( int argc, char **argv )
{
    // Если получено мало параметров (минимально должно быть:
    // имя программы, входной файл и выходной файл)
    if ( argc < 3 ) {

        // Вывести usage и выйти
        printUsageAndExit();
    }

    // Для удобства отделения последнего параметра (имя выходного файла)
    // от остальных параметров (имен входных файлов), вычислить количество
    // параметров минус один
    argc--;

    // Создать/открыть на запись выходной файл
    HANDLE outFh = CreateFile( argv[ argc ], GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

    // Для каждого входного файла (нулевой параметр пропускаем,
    // т.к. это имя программы, а не входного файла)
    int i;
    for ( i = 1; i < argc; i++ ) {

        // Буфер для пересылки данных между файлами
        char *buffer[ BUFFER_SIZE ];

        // Открыть входной файл на чтение
        HANDLE inFh = CreateFile( argv[ i ], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

        // Количество фактически прочитанных байт (оно может быть меньше,
        // чем запрошенное количество, если достигнут конец файла)
        DWORD count;

        do {
            // Прочитать блок данных из входного файла
            ReadFile( inFh, buffer, BUFFER_SIZE, &count, NULL );

            // Записать прочитанный блок в выходной файл
            WriteFile( outFh, buffer, count, NULL, NULL );

        // Выполнять, пока количество прочитанных байт равно количеству запрошенных.
        // Когда прочитаем последний блок это соотношение станет меньше.
        } while ( count == BUFFER_SIZE );

        // Закрыть входной файл
        CloseHandle( inFh );
    }

    // Закрыть выходной файл
    CloseHandle( outFh );

    // Выйти со статусом успеха
    return 0;
}
