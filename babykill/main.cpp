#include <stdio.h>
#include <windows.h>

int main( int argc, char **argv )
{
    int pid;

    // ���� �� �������� �� ������ ��������� ���
    // �� ������� ��������� �� ���������� ������� PID (����� �����)
    if ( argc < 2 || sscanf( argv[ 1 ], "%i", &pid ) != 1 ) {

        puts( "Usage: babykill <pid>" );

        // ����� �� �������� ������
        return 1;
    }

    // ������� �������, ����� �������� ��� �����
    HANDLE ph = OpenProcess( PROCESS_TERMINATE, FALSE, pid );

    // ����������� ��������� �������� �������
    TerminateProcess( ph, 0 );

    // ����� �� �������� ������
    return 0;
}
