#include <shutdown.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kapplication.h>

int
main(int argc, char *argv[])
{
   KAboutData about("kapptest", "kapptest", "version");
   KCmdLineArgs::init(argc, argv, &about);

   KApplication a;
   KSMShutdownFeedback::start();
   QObject::connect( KSMShutdownFeedback::self(), SIGNAL( aborted() ), &a, SLOT( quit() ) );

   bool saveSession;
   KApplication::ShutdownType sdtype = KApplication::ShutdownTypeNone;
   KApplication::ShutdownMode sdmode = KApplication::ShutdownModeDefault;
   (void)KSMShutdownDlg::confirmShutdown( saveSession, true, true,
                                          sdtype, sdmode );
   KSMShutdownFeedback::stop();
}
