#include <stdio.h>
#include <windows.h>

#define BUFFER_SIZE 100

int main( int argc, char **argv )
{
    // �������� ������ ����������� ������� ��� �������� �� �������� ���������
    HANDLE hStdInput = GetStdHandle( STD_INPUT_HANDLE );
    HANDLE hStdOuptut = GetStdHandle( STD_OUTPUT_HANDLE );
    HANDLE hStdError = GetStdHandle( STD_ERROR_HANDLE );

    // ��������� ��������� ��������� ����������� ������ ����������� �������
    SetHandleInformation( hStdInput, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT );
    SetHandleInformation( hStdOuptut, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT );
    SetHandleInformation( hStdError, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT );

    // ��������� �������� ���� ��������� STARTUPINFO ��� �������� ���������
    STARTUPINFO siChild;
    ZeroMemory( &siChild, sizeof( STARTUPINFO ) );
    siChild.cb = sizeof( STARTUPINFO );
    siChild.dwFlags |= STARTF_USESTDHANDLES;

    // ������� ��������� SECURITY_ATTRIBUTES ����������� ������������ ������ ��������� ����������
    SECURITY_ATTRIBUTES saInherit;
    saInherit.nLength = sizeof( SECURITY_ATTRIBUTES );
    saInherit.bInheritHandle = TRUE;
    saInherit.lpSecurityDescriptor = NULL;

    // ������� ����
    while ( 1 ) {

        // ������� �����������
        printf( "%s", "> " );

        // ��������� ��������� ������ ������������
        char input[ BUFFER_SIZE ] = "";
        fgets( input, BUFFER_SIZE, stdin );

        // ������� ��������� ������ �� ��������� �������, ����������� �������� '|'
        char *commands[ BUFFER_SIZE ];
        commands[ 0 ] = strtok( input, "|" );
        int commandIndex = 0;
        while ( commands[ commandIndex ] != NULL ) {
            commandIndex++;
            commands[ commandIndex ] = strtok( NULL, "|" );
        }

        // ���������� ��� ��������� �������� ����� �� ������ ����������� ������� ����� � ������,
        // ��� � � �������� ��������
        siChild.hStdInput = hStdInput;
        siChild.hStdError = hStdError;

        // ���������� ���������� �������� ��������� � �� ������. ���������� ��� �������� ����������
        // ���� �������� ���������.
        int childProcessCount = 0;
        HANDLE hChildProcesses[ BUFFER_SIZE ];

        // ���������� ������ �������
        for ( commandIndex = 0; commands[ commandIndex ] != NULL; commandIndex++ ) {

            // ���������� �������, �������� ����������� ���������
            char *spaces = " \t\n\r\v\f";

            // ��������� ������ ������� ������� ��� �������� � CreateProcess
            // (��� ��������������� �����/������)
            char commandLine[ BUFFER_SIZE ] = "";

            // �������� ��� ������� (������������ �����)
           char *commandName = strtok( commands[ commandIndex ], spaces );

            // ���� ������� ������, ������� � ���������
            if ( commandName == NULL ) {
                continue;
            }

            // ���� �������� ������� 'exit', ����� (�� �������� ������)
            if ( strcmp( commandName, "exit" ) == 0 ) {
                return 0;
            }

            // ��������� ��� ������� � ��������� ������
            strcat( commandLine, commandName );

            // ����������� ������ ��� �������� �����/������ ����� ���������
            HANDLE hPipeReadEnd, hPipeWriteEnd;

            // ���� ������� ������� �� ���������
            if ( commands[ commandIndex + 1 ] != NULL ) {

                // ������� ����� ��� ������� � ��������� ������
                CreatePipe( &hPipeReadEnd, &hPipeWriteEnd, NULL, 0 );

                // ��������� ��������� ��������� �������� ���������� ����� ����� ������
                SetHandleInformation( hPipeWriteEnd, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT );

                // ���������� ����� ����� ������ � �������� ������
                // ������������ ������ ������ ��� ��������� ��������
                siChild.hStdOutput = hPipeWriteEnd;

            // ���� ������� ������� ���������
            } else {

                // ���������� ��� ��������� �������� ����� �� ������ ������������ ������ ������,
                // ��� � � �������� ��������
                siChild.hStdOutput = hStdOuptut;
            }

            // ����� ������ ��� ��������������� �����/������.
            // NULL ��������, ��� ���������������� ��������������� ���.
            char *inputFilename = NULL;
            char *outputFilename = NULL;

            // ������� ��������� ������� �� ������
            char *parameter;
            while ( ( parameter = strtok( NULL, spaces ) ) != NULL ) {

                // ���������, �������� �� ������ �������� ���������������� �����/������
                switch ( parameter[ 0 ] ) {

                    // ���� ��� ��������������� �����
                    case '<':

                        // ��������� ��� ����� �������� ������ '<'
                        inputFilename = parameter + 1;
                        break;

                    // ���� ��� ��������������� ������
                    case '>':

                        // ��������� ��� ����� �������� ������ '>'
                        outputFilename = parameter + 1;
                        break;

                    // ���� ��� �� ��������������� (������� ��������)
                    default:

                        // �������� ���� �������� � ��������� ������
                        strcat( commandLine, " " );
                        strcat( commandLine, parameter );
                }
            } // ����� ����� ���������� ����������

            // ���� ������ ��������������� �����
            if ( inputFilename != NULL ) {

                // ���� ��� �� ������ �������, ������� �������������� ���������� ������ ������. ����� ����� ������������ ����������, ��� ����
                // ����� �������, ����� ������������.
                if ( commandIndex > 0 ) {
                    CloseHandle( siChild.hStdInput );
                }

                // ������� �������� ���� �� ������ � ��������� ����� � ��������
                // ������ ������������ ������ ����� ��� ��������� ������
                siChild.hStdInput = CreateFile( inputFilename, GENERIC_READ, 0, &saInherit,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
            }

            // ���� ������ ��������������� ������
            if ( outputFilename != NULL ) {

                // �������/������� �������� ���� �� ������ � ��������� ����� � ��������
                // ������ ������������ ������ ������ ��� ��������� ������
                siChild.hStdOutput = CreateFile( outputFilename, GENERIC_WRITE, 0, &saInherit,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
            }

            // ��������� ������� � �������� ��������
            PROCESS_INFORMATION piChild;
            BOOL isCreated = CreateProcess( NULL, commandLine, NULL, NULL, TRUE, 0, NULL, NULL,
                &siChild, &piChild );

            // ���� ������� ������� ��������
            if ( isCreated ) {

                // ��������� ����� ��������� �������� ��� ����������� �������� ��� ����������
                hChildProcesses[ childProcessCount ] = piChild.hProcess;
                childProcessCount++;

                // ������� ����� ��������� ������
                CloseHandle( piChild.hThread );

            // ���� �� ������� ��������� �������
            } else {

                // �������, ��� ���� ������ �������� �������, � ������� ��������������� ���������.
                fprintf( stderr, "error: unknown command '%s'\n", commandName    );
            }

            // ���� ��� �� ������ �������, ������� �������������� ���������� ������ ������,
            // ���������� �� ������� ��������
            if ( commandIndex > 0 ) {
                CloseHandle( siChild.hStdInput );
            }

            // ���� ������ ��������������� �����, ������� ��������������� �����
            if ( inputFilename != NULL ) {
                CloseHandle( siChild.hStdInput );
            }

            // ���� ������ ��������������� ������, ������� ��������������� �����
            if ( outputFilename != NULL ) {
                CloseHandle( siChild.hStdOutput );
            }

            // ���� ��� ���� �� ��������� �������
            if ( commands[ commandIndex + 1 ] != NULL ) {

                // ������� �������������� ���������� ����� ������. ����� ����� �� ��������� EOF.
                CloseHandle( hPipeWriteEnd );

                // ��������� ��������� �������� ����������� ����� ������ ������
                SetHandleInformation( hPipeReadEnd, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT );

                // ���������� ����� ������ ������ � �������� ������
                // ������������ ������ ����� ��� ��������� ��������
                siChild.hStdInput = hPipeReadEnd;
            }

        } // ����� ����� ������

        // ��������� ���������� ���� �������� ���������
        WaitForMultipleObjects( childProcessCount, hChildProcesses, TRUE, INFINITE );

        // ������� ������ ���� �������� ��������� (� �������� ������� - ����� �� ��������
        // ������ ����������-�������).
        for ( childProcessCount--; childProcessCount >= 0; childProcessCount-- ) {
            CloseHandle( hChildProcesses[ childProcessCount ] );
        }

    } // ����� �������� �����

    // ����� (�� �������� ������)
    return 0;
}
