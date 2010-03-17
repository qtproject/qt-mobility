/* This file is a workaround for Maemo5 PR 1.1
 * This file should be deleted in PR 1.2 and
 * replaced by <libosso-abook/osso-abook.h>
 */
#ifndef OSSO_ABOOK_WORKAROUND_H
#define OSSO_ABOOK_WORKAROUND_H

#include <libmcclient/mc-profile.h>
#include <libmcclient/mc-account.h>

#include <libosso-abook/osso-abook-init.h>
#include <libosso-abook/osso-abook-types.h>
#include <libosso-abook/osso-abook-waitable.h>
#include <libosso-abook/osso-abook-presence.h>
#include <libosso-abook/osso-abook-avatar.h>

//### Remove in PR 1.2
extern "C" {
        typedef void GtkWindow;
        struct _OssoABookContact {
          EContact parent;
        };

        OssoABookRoster*    osso_abook_roster_new               (const char *name,
                                                                 EBookView *book_view,
                                                                 const char *vcard_field);
        gboolean            osso_abook_roster_is_running        (OssoABookRoster *roster);
        void                osso_abook_roster_start             (OssoABookRoster *roster);

        gboolean            osso_abook_contact_has_valid_name   (OssoABookContact *contact);
        gboolean            osso_abook_contact_is_roster_contact(OssoABookContact *contact);

        OssoABookRoster*    osso_abook_aggregator_get_default   (GError **error);
        GList*              osso_abook_aggregator_list_master_contacts
                                                                (OssoABookAggregator *aggregator);
        unsigned            osso_abook_aggregator_get_master_contact_count
                                                                (OssoABookAggregator *aggregator);
        GList*              osso_abook_aggregator_lookup        (OssoABookAggregator *aggregator,
                                                                 const char *uid);
        const char*         osso_abook_contact_get_uid          (OssoABookContact *contact); 
        EBook*              osso_abook_roster_get_book          (OssoABookRoster *roster);
        char*               osso_abook_contact_to_string        (OssoABookContact *contact,
                                                                 EVCardFormat format,
                                                                 gboolean inline_avatar);
        char*               osso_abook_contact_get_value        (EContact *contact,
                                                                 const char *attr_name);
        GList*              osso_abook_aggregator_find_contacts (OssoABookAggregator *aggregator,
                                                                 EBookQuery *query);
        GList*              osso_abook_contact_get_values       (EContact *contact,
                                                                 const char *attr_name);
        GList*              osso_abook_contact_get_attributes   (EContact *contact,
                                                                 const char *attr_name);
        GList*              osso_abook_contact_get_roster_contacts
                                                                (OssoABookContact *master_contact);
        McProfile*          osso_abook_contact_get_profile      (OssoABookContact *contact);
        McAccount*          osso_abook_contact_get_account      (OssoABookContact *contact);
        gboolean            osso_abook_contact_delete           (OssoABookContact *contact,
                                                                 EBook *book,
                                                                 GtkWindow *window);
        GList*              osso_abook_aggregator_find_contacts (OssoABookAggregator *aggregator,
                                                                 EBookQuery *query);
        const char*         osso_abook_contact_get_display_name (OssoABookContact *contact);
        GdkPixbuf*          osso_abook_avatar_get_image_rounded (OssoABookAvatar *avatar,
                                                                 int width,
                                                                 int height,
                                                                 gboolean crop,
                                                                 int radius,
                                                                 const guint8 border_color[4]);
        OssoABookContact*   osso_abook_contact_new              (void);
        guint               osso_abook_contact_async_add        (OssoABookContact *contact,
                                                                 EBook *book,
                                                                 EBookIdCallback callback,
                                                                 gpointer user_data);
        guint               osso_abook_contact_async_commit     (OssoABookContact *contact,
                                                                 EBook *book,
                                                                 EBookCallback callback,
                                                                 gpointer user_data);
        gboolean            osso_abook_contact_add_value        (EContact *contact,
                                                                 const char *attr_name,
                                                                 GCompareFunc value_check,
                                                                 const char *value);
        void                osso_abook_contact_set_pixbuf       (OssoABookContact *contact,
                                                                 GdkPixbuf *pixbuf,
                                                                 EBook *book,
                                                                 GtkWindow *window);
        // osso-abook-account-manager.h
        const GList*        osso_abook_account_manager_get_primary_vcard_fields
                                                                 (OssoABookAccountManager *manager);
        OssoABookAccountManager* osso_abook_account_manager_get_default(void);
        const char*         osso_abook_account_manager_get_vcard_field
                                                                (OssoABookAccountManager *manager,
                                                                 const char *account_name);
        // osso-abook-caps.h
        typedef enum {
          OSSO_ABOOK_CAPS_NONE             = (0),
          OSSO_ABOOK_CAPS_EMAIL            = (1 << 0),
          OSSO_ABOOK_CAPS_CHAT             = (1 << 1),
          OSSO_ABOOK_CAPS_CHAT_ADDITIONAL  = (1 << 2),
          OSSO_ABOOK_CAPS_VOICE            = (1 << 3),
          OSSO_ABOOK_CAPS_VOICE_ADDITIONAL = (1 << 4),
          OSSO_ABOOK_CAPS_VIDEO            = (1 << 5),
          OSSO_ABOOK_CAPS_PHONE            = (1 << 6),
          OSSO_ABOOK_CAPS_ADDRESSBOOK      = (1 << 7),
          OSSO_ABOOK_CAPS_IMMUTABLE_STREAMS= (1 << 8),
          OSSO_ABOOK_CAPS_SMS              = (1 << 9),

          OSSO_ABOOK_CAPS_ALL = (OSSO_ABOOK_CAPS_EMAIL | OSSO_ABOOK_CAPS_CHAT |
                                 OSSO_ABOOK_CAPS_VOICE | OSSO_ABOOK_CAPS_VIDEO |
                                 OSSO_ABOOK_CAPS_PHONE | OSSO_ABOOK_CAPS_SMS)
        } OssoABookCapsFlags;
        GType osso_abook_caps_get_type (void) G_GNUC_CONST;

        typedef struct _OssoABookCaps OssoABookCaps; 
        #define OSSO_ABOOK_TYPE_CAPS \
                (osso_abook_caps_get_type ())

        #define OSSO_ABOOK_CAPS(obj) \
                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                 OSSO_ABOOK_TYPE_CAPS, \
                 OssoABookCaps))

        OssoABookCapsFlags  osso_abook_caps_get_capabilities    (OssoABookCaps *caps);
}

#endif
