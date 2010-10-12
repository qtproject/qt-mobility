#include <geoclue/geoclue-master.h>
#include <gconf/gconf-client.h>

int main(int, char**)
{
    GeoclueMaster *master(0);
    GConfClient* client(0);
    GError *error(0);
    g_type_init ();
    return 0;
}
