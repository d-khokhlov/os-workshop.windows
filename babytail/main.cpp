#include <stdio.h>
#include <windows.h>

#define BUFFER_SIZE 100

void printUsageAndExit( )
{

    puts( "Usage: babytail [-c <bytes_count>] <filename>" );

    // ����� �� �������� ������
    ExitProcess( 1 );
}

int main( int argc, char **argv )
{
    // ���� �������� ���� ���������� (���������� ������ ����:
    // ��� ��������� � ��� �����)
    if ( argc < 2 ) {

        // ������� usage � �����
        printUsageAndExit( );
    }

    char *filename;

    // ���������� ��������� ���� (�� ��������� 10)
    int count = 10;

    // ���� ���������� ��������� ���� �������� � ����������
    if ( strcmp( argv[ 1 ], "-c" ) == 0 ) {

        // � ���� ������ ����������� ���������� ����������
        // ������������� �� 2. ������� usage � �����, ���� �������� ������
        if ( argc < 4 ) {
            printUsageAndExit( );
        }

        // �������� ��������� ���������� ���� �� ������� ���������.
        // ���� �� �������, ������� usage � �����.
        if ( sscanf( argv[ 2 ], "%i", &count ) != 1 ) {
            printUsageAndExit( );
        }

        filename = argv[ 3 ];

        // ���� ��� ���������� � ��������� ����������� ����
    } else {
        filename = argv[ 1 ];
    }

    // ������� �������� ���� �� ������
    HANDLE fh = CreateFile( filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    // ������� � �������, ��������� �� ����� ����� �� ��������� ���������� ����
    SetFilePointer( fh, -count, NULL, FILE_END );

    // ��������� ��������� ���������� ����, ��������
    // ���������� ����������� ����������, �.�. ��� ����� ����������
    char buffer[ BUFFER_SIZE ];
    DWORD readCount;
    ReadFile( fh, buffer, BUFFER_SIZE, &readCount, NULL );

    // ������� ����
    CloseHandle( fh );

    // ������� ����������� �����
    WriteFile( GetStdHandle( STD_OUTPUT_HANDLE ), buffer, readCount, NULL, NULL );

    // ����� �� �������� ������
    return 0;
}
