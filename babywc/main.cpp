#include <stdio.h>
#include <windows.h>

void printUsageAndExit()
{

    puts( "Usage: babywc [-l|-c] <filename>" );

    // ����� �� �������� ������
    ExitProcess( 1 );
}

int main( int argc, char **argv )
{
    char *filename = NULL;

    // ����� ��������: 'l' - ������, 'c' - �����
    char mode = 'l';

    // ��� ������� ��������� ��������� ������
    // (����� ��������, ������� �������� ��� ���������)
    int argIndex;
    for ( argIndex = 1; argIndex < argc; argIndex++ ) {

        // ���� �������� �������� ������� ������ (���������� � '-')
        if ( argv[ argIndex ][ 0 ] == '-' ) {

            // ��� ������� ������� � ���� ���������
            // (����� ��������, ������� �������� '-')
            int optionIndex = 1;
            while ( argv[ argIndex ][ optionIndex ] != 0 ) {

                // ���� ������ - ���� �� ������ ������, ��������� ���,
                // ����� - ������� usage � �����
                switch ( argv[ argIndex ][ optionIndex ] ) {
                    case 'c':
                        mode = 'c';
                        break;
                    case 'l':
                        mode = 'l';
                        break;
                    default:
                        printUsageAndExit();
                }

                // ������� � ���������� ������� ��� ��������� ��������
                optionIndex++;
            }

            // ���� �������� ���������� �� � '-', ������ ��� ��� �����
        } else {

            // ��������� ��� � ��������� ������ ����������
            filename = argv[ argIndex ];
            break;
        }
    } // ����� ����� ����������

    // ���� ��� ����� �� ���� ��������, ������� usage � �����
    if ( filename == NULL ) {
        printUsageAndExit();
    }

    // ������� �������� ���� �� ������
    HANDLE fh = CreateFile( filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    int count = 0;

    // ������ �� ������ ������� �� �����, ���� ���� �� ��������
    // (��� ���� ReadFile ������ ���������� ����������� ���� ������ 0)
    char input;
    DWORD readCount;
    while ( ReadFile( fh, &input, 1, &readCount, NULL ) && readCount == 1 ) {

        // ��������� �������, ���� �� ������� ����� ����
        // ���� �� ������� ������ � �������� ������ ����� ������
        if ( mode == 'c' || ( mode == 'l' && input == '\n' ) ) {
            count++;
        }
    }

    // ������� ����
    CloseHandle( fh );

    // ������� ������������ ����������
    printf( "%i\n", count );

    // ����� �� �������� ������
    return 0;
}
