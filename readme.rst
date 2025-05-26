nmcli connection add con-name test3 type 802-3-ethernet ipv4.method manual ipv4.addresses 192.168.1.100/24

QT_LOGGING_RULES="*.info=true" ./build/Desktop-Debug/nmconeditor
