# Gutenberg

This is a small program that runs on Windows and fetchs a remote text file and prints it
to a thermal printer. It is used with our SaaS clients for Bill Gastor/Salor Hospitality.

This is a windows only program. To print remote content on Linux, Salor Bin is suggested.

To compile: Download and install Qt Creator for Windows, then open the project
and click build, or switch to release mode.

The exe file will need to be in a directory along with:

    gutenberg.ini
    libgcc_s_dw2-1.dll
    mingwm10.dll
    QtCore4.dll
    QtGui4.dll
    QtNetwork4.dll

All of which can be found from your QtSDK install location.

Make sure that you build for target Qt 4.8.1 and that you grab
the correct DLLs for 4.8.1.

