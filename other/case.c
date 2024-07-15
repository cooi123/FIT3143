#include <stdio.h>
enum week
{
    Mon,
    Tue,
    Wed,
    Thur,
    Fri,
    Sat,
    Sun
};

int main()
{
    enum week day = Tue;

    switch (day)
    {
    case Wed:
        printf("wed");
        break;
    case Tue:
        printf("Tue");
        break;

    default:
        return 0;
    }
    return 0;
}