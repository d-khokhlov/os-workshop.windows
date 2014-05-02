#include <stdio.h>
#include <windows.h>

#define BUFFER_SIZE 100

void printUsageAndExit()
{

    puts( "Usage: babytail [-c <bytes_count>] <filename>" );

    // Выйти со статусом ошибки
    ExitProcess( 1 );
}

int main( int argc, char **argv )
{
    // Если получено мало параметров (минимально должно быть:
    // имя программы и имя файла)
    if ( argc < 2 ) {

        // Вывести usage и выйти
        printUsageAndExit();
    }

    char *filename;

    // Количество выводимых байт (по умолчанию 10)
    int count = 10;

    // Если количество выводимых байт передано в параметрах
    if ( strcmp( argv[ 1 ], "-c" ) == 0 ) {

        // В этом случае минимальное количество параметров
        // увеличивается на 2. Вывести usage и выйти, если передано меньше
        if ( argc < 4 ) {
            printUsageAndExit();
        }

        // Получить требуемое количество байт из второго параметра.
        // Если не удалось, вывести usage и выйти.
        if ( sscanf( argv[ 2 ], "%i", &count ) != 1 ) {
            printUsageAndExit();
        }

        filename = argv[ 3 ];

        // Если нет параметров с требуемым количеством байт
    } else {
        filename = argv[ 1 ];
    }

    // Открыть заданный файл на чтение
    HANDLE fh = CreateFile( filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    // Перейти в позицию, отстоящую от конца файла на требуемое количество байт
    SetFilePointer( fh, -count, NULL, FILE_END );

    // Прочитать требуемое количество байт, сохранив
    // фактически прочитанное количество, т.к. оно может отличаться
    char buffer[ BUFFER_SIZE ];
    DWORD readCount;
    ReadFile( fh, buffer, BUFFER_SIZE, &readCount, NULL );

    // Закрыть файл
    CloseHandle( fh );

    // Вывести прочитанные байты
    WriteFile( GetStdHandle( STD_OUTPUT_HANDLE ), buffer, readCount, NULL, NULL );

    // Выйти со статусом успеха
    return 0;
}
