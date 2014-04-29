#include <stdio.h>
#include <windows.h>

#define BUFFER_SIZE 100

int main( int argc, char **argv )
{
    // Получить хэндлы стандартных потоков для передачи их дочерним процессам
    HANDLE hStdInput = GetStdHandle( STD_INPUT_HANDLE );
    HANDLE hStdOuptut = GetStdHandle( STD_OUTPUT_HANDLE );
    HANDLE hStdError = GetStdHandle( STD_ERROR_HANDLE );

    // Разрешить дочерними процессам наследовать хэндлы стандартных потоков
    SetHandleInformation( hStdInput, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT );
    SetHandleInformation( hStdOuptut, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT );
    SetHandleInformation( hStdError, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT );

    // Заполнить основные поля структуры STARTUPINFO для дочерних процессов
    STARTUPINFO siChild;
    ZeroMemory( &siChild, sizeof( STARTUPINFO ) );
    siChild.cb = sizeof( STARTUPINFO );
    siChild.dwFlags |= STARTF_USESTDHANDLES;

    // Создать структуру SECURITY_ATTRIBUTES разрешающую наследование хэндла дочерними процессами
    SECURITY_ATTRIBUTES saInherit;
    saInherit.nLength = sizeof( SECURITY_ATTRIBUTES );
    saInherit.bInheritHandle = TRUE;
    saInherit.lpSecurityDescriptor = NULL;

    // Главный цикл
    while ( 1 ) {

        // Вывести приглашение
        printf( "%s", "> " );

        // Прочитать командную строку пользователя
        char input[ BUFFER_SIZE ] = "";
        fgets( input, BUFFER_SIZE, stdin );

        // Разбить введенную строку на отдельные команды, разделенные символом '|'
        char *commands[ BUFFER_SIZE ];
        commands[ 0 ] = strtok( input, "|" );
        int commandIndex = 0;
        while ( commands[ commandIndex ] != NULL ) {
            commandIndex++;
            commands[ commandIndex ] = strtok( NULL, "|" );
        }

        // Установить для дочернего процесса такие же хэндлы стандартных потоков ввода и ошибок,
        // как и у текущего процесса
        siChild.hStdInput = hStdInput;
        siChild.hStdError = hStdError;

        // Количество запущенных дочерних процессов и их хэндлы. Необходимо для ожидания завершения
        // всех дочерних процессов.
        int childProcessCount = 0;
        HANDLE hChildProcesses[ BUFFER_SIZE ];

        // Обработать каждую команду
        for ( commandIndex = 0; commands[ commandIndex ] != NULL; commandIndex++ ) {

            // Пробельные символы, которыми разделяются параметры
            char *spaces = " \t\n\r\v\f";

            // Командная строка текущей команды для передачи в CreateProcess
            // (без перенаправлений ввода/вывода)
            char commandLine[ BUFFER_SIZE ] = "";

            // Выделить имя команды (исполняемого файла)
           char *commandName = strtok( commands[ commandIndex ], spaces );

            // Если команда пустая, перейти к следующей
            if ( commandName == NULL ) {
                continue;
            }

            // Если получена команда 'exit', выйти (со статусом успеха)
            if ( strcmp( commandName, "exit" ) == 0 ) {
                return 0;
            }

            // Сохранить имя команды в командную строку
            strcat( commandLine, commandName );

            // Дескрипторы канала для передачи ввода/вывода между командами
            HANDLE hPipeReadEnd, hPipeWriteEnd;

            // Если текущая команда не последняя
            if ( commands[ commandIndex + 1 ] != NULL ) {

                // Создать канал для текущей и следующей команд
                CreatePipe( &hPipeReadEnd, &hPipeWriteEnd, NULL, 0 );

                // Разрешить разрешить дочернему процессу наследоват хэндл входа канала
                SetHandleInformation( hPipeWriteEnd, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT );

                // Установить хэндл входа канала в качестве хэндла
                // стандартного потока вывода для дочернего процесса
                siChild.hStdOutput = hPipeWriteEnd;

            // Если текущая команда последняя
            } else {

                // Установить для дочернего процесса такой же хэндлы стандартного потока вывода,
                // как и у текущего процесса
                siChild.hStdOutput = hStdOuptut;
            }

            // Имена файлов для перенаправления ввода/вывода.
            // NULL означает, что соответствующего перенаправления нет.
            char *inputFilename = NULL;
            char *outputFilename = NULL;

            // Извлечь параметры команды по одному
            char *parameter;
            while ( ( parameter = strtok( NULL, spaces ) ) != NULL ) {

                // Проверить, является ли данный параметр перенаправлением ввода/вывода
                switch ( parameter[ 0 ] ) {

                    // Если это перенаправление ввода
                    case '<':

                        // Сохранить имя файла исключая символ '<'
                        inputFilename = parameter + 1;
                        break;

                    // Если это перенаправление вывода
                    case '>':

                        // Сохранить имя файла исключая символ '>'
                        outputFilename = parameter + 1;
                        break;

                    // Если это не перенаправление (обычный параметр)
                    default:

                        // Добавить этот параметр в командную строку
                        strcat( commandLine, " " );
                        strcat( commandLine, parameter );
                }
            } // Конец цикла извлечения параметров

            // Если задано перенаправление ввода
            if ( inputFilename != NULL ) {

                // Если это не первая команда, закрыть первоначальный дескриптор выхода канала. Иначе далее единственная переменная, где этот
                // хэндл остался, будет перезаписана.
                if ( commandIndex > 0 ) {
                    CloseHandle( siChild.hStdInput );
                }

                // Открыть заданный файл на чтение и сохранить хэндл в качестве
                // хэндла стандартного потока ввода для дочернего канала
                siChild.hStdInput = CreateFile( inputFilename, GENERIC_READ, 0, &saInherit,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
            }

            // Если задано перенаправление вывода
            if ( outputFilename != NULL ) {

                // Создать/открыть заданный файл на запись и сохранить хэндл в качестве
                // хэндла стандартного потока вывода для дочернего канала
                siChild.hStdOutput = CreateFile( outputFilename, GENERIC_WRITE, 0, &saInherit,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
            }

            // Запустить команду в дочернем процессе
            PROCESS_INFORMATION piChild;
            BOOL isCreated = CreateProcess( NULL, commandLine, NULL, NULL, TRUE, 0, NULL, NULL,
                &siChild, &piChild );

            // Если команда успешно запущена
            if ( isCreated ) {

                // Сохранить хэндл дочернего процесса для дальнейшего ожидания его завершения
                hChildProcesses[ childProcessCount ] = piChild.hProcess;
                childProcessCount++;

                // Закрыть хэндл дочернего потока
                CloseHandle( piChild.hThread );

            // Если не удалось запустить команду
            } else {

                // Считаем, что была задана неверная команда, и выводим соответствующее сообщение.
                fprintf( stderr, "error: unknown command '%s'\n", commandName    );
            }

            // Если это не первая команда, закрыть первоначальный дескриптор выхода канала,
            // созданного на прошлой итерации
            if ( commandIndex > 0 ) {
                CloseHandle( siChild.hStdInput );
            }

            // Если задано перенаправление ввода, закрыть соответствующий хэндл
            if ( inputFilename != NULL ) {
                CloseHandle( siChild.hStdInput );
            }

            // Если задано перенаправление вывода, закрыть соответствующий хэндл
            if ( outputFilename != NULL ) {
                CloseHandle( siChild.hStdOutput );
            }

            // Если это была не последняя команда
            if ( commands[ commandIndex + 1 ] != NULL ) {

                // Закрыть первоначальный дескриптор входа канала. Иначе канал не достигнет EOF.
                CloseHandle( hPipeWriteEnd );

                // Разрешить дочернему процессу наследовать хэндл выхода канала
                SetHandleInformation( hPipeReadEnd, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT );

                // Установить хэндл выхода канала в качестве хэндла
                // стандартного потока ввода для дочернего процесса
                siChild.hStdInput = hPipeReadEnd;
            }

        } // Конец цикла команд

        // Дождаться завершения всех дочерних процессов
        WaitForMultipleObjects( childProcessCount, hChildProcesses, TRUE, INFINITE );

        // Закрыть хэндлы всех дочерних процессов (в обратном порядке - чтобы не заводить
        // лишнюю переменную-счетчик).
        for ( childProcessCount--; childProcessCount >= 0; childProcessCount-- ) {
            CloseHandle( hChildProcesses[ childProcessCount ] );
        }

    } // Конец главного цикла

    // Выйти (со статусом успеха)
    return 0;
}
