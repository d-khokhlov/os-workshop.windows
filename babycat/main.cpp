#include <stdio.h>
#include <windows.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <stdlib.h>

#define BUFFER_SIZE 100

void printUsageAndExit()
{
    puts( "Usage: babycat <in_filename1> [<in_filename2> ...] <out_filename>" );

    // ����� �� �������� ������
    ExitProcess( 1 );
}

int main( int argc, char **argv )
{
    // ���� �������� ���� ���������� (���������� ������ ����:
    // ��� ���������, ������� ���� � �������� ����)
    if ( argc < 3 ) {

        // ������� usage � �����
        printUsageAndExit();
    }

    // ��� �������� ��������� ���������� ��������� (��� ��������� �����)
    // �� ��������� ���������� (���� ������� ������), ��������� ����������
    // ���������� ����� ����
    argc--;

    // �������/������� �� ������ �������� ����
    HANDLE outFh = CreateFile( argv[ argc ], GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

    // ��� ������� �������� ����� (������� �������� ����������,
    // �.�. ��� ��� ���������, � �� �������� �����)
    int i;
    for ( i = 1; i < argc; i++ ) {

        // ����� ��� ��������� ������ ����� �������
        char *buffer[ BUFFER_SIZE ];

        // ������� ������� ���� �� ������
        HANDLE inFh = CreateFile( argv[ i ], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

        // ���������� ���������� ����������� ���� (��� ����� ���� ������,
        // ��� ����������� ����������, ���� ��������� ����� �����)
        DWORD count;

        do {
            // ��������� ���� ������ �� �������� �����
            ReadFile( inFh, buffer, BUFFER_SIZE, &count, NULL );

            // �������� ����������� ���� � �������� ����
            WriteFile( outFh, buffer, count, NULL, NULL );

        // ���������, ���� ���������� ����������� ���� ����� ���������� �����������.
        // ����� ��������� ��������� ���� ��� ����������� ������ ������.
        } while ( count == BUFFER_SIZE );

        // ������� ������� ����
        CloseHandle( inFh );
    }

    // ������� �������� ����
    CloseHandle( outFh );

    // ����� �� �������� ������
    return 0;
}
