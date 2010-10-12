#include <gypsy/gypsy-satellite.h>
#include <gypsy/gypsy-control.h>
#include <gconf/gconf-client.h>

int main(int, char**)
{
    GConfClient* client(0);
    GypsyControl *control(0);
    GypsyDevice *device(0);
    g_type_init ();
    return 0;
}
