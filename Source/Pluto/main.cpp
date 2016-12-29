#include <Core/Common.h>
#include <QSplashScreen>

#include "Console/ConsoleModule.h"
#include "MainWindow.h"
#include "PlutoApplication.h"


#if defined(PLUTO_BUILD_DEBUG) || !defined(PLUTO_PLATFORM_WINDOWS)
int main(int argc, char *argv[])
#else
int WinMain(HINSTANCE /*hInInstance*/, HINSTANCE /*hPrevInstance*/, char*, int /*nCmdShow*/)
#endif
{
    memory::initialize();
    int ret = 0;
    {
        ConsoleModule::create();

#if defined(PLUTO_BUILD_DEBUG) || !defined(PLUTO_PLATFORM_WINDOWS)
        PlutoApplication app(argc, argv);
#else
        PlutoApplication app(__argc, __argv);
#endif

        QSplashScreen* splash = new QSplashScreen();
        splash->setPixmap(QPixmap(":/res/splash.png"));
        splash->show();

        app.init();

        MainWindow w;

        w.show();
        splash->finish(&w);

        app.start_kernel();
        ret = app.exec();
    }
    memory::shutdown();
    return ret;
}
