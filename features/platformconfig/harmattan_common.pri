#Harmattan specific options common to both x86 and arm go here
#A not existing option is interpreted as opt-out/no.
#for example: bluez_enabled = yes|no

QMAKE_CFLAGS += -g1 -O2

# Specify the config test that needs to be enabled/disabled 
qmf_enabled = yes
networkmanager_enabled = no
corewlan_enabled = no
maemo-icd_enabled = yes
maemo-icd-network-wlan_enabled = yes
maemo5-contacts_enabled = no
maemo5-calendar_enabled = no
maemo6-landmarks_enabled = yes
bluez_enabled = yes
proj_enabled = no
gstreamer-photography_enabled = yes
blkid_enabled = yes
pulseaudio_enabled = yes
udev_enabled = yes
iphb_enabled = yes
nfc_enabled = no
sensord_enabled = yes

