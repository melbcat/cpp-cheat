#include <cstdlib>
#include <iostream>

#include <QString>

#include <KApplication>
#include <KAboutData>
#include <KAction>
#include <KActionCollection>
#include <KCmdLineArgs>
#include <KConfigDialog>
#include <KConfigGroup>
#include <KLocale>
#include <KMessageBox>
#include <KMenuBar>
#include <KSharedConfigPtr>

#include "main.h"

#include <settings.h>
#include "ui_general.h"

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
{
    textArea = new KTextEdit;
    setCentralWidget(textArea);
    setupActions();
    loadSettingsXt();

    /*
    #setGUI

        Creates GUI elements from `ui.rc` files.
    */

        this->setupGUI(Default, "kdecheatui.rc");
}

/*
#SharedConfig

    Shares multiple KConf instances nicely amongst concurrent access.

    Better than using KConf directly.

    New alternative to this method; KConfig XT

    Configuration files are ini files kept under `~/.kde/share/config` in Linux.

#ConfigGroup

    Inside a given config file or ini section, which ini section we are looking for.

#writeEntry

    Writting to config file is immediate.
*/
void MainWindow::saveConfig()
{
    // Name of the config file is determined from the application name if not given.
    // as APPNAMErc.
    KSharedConfigPtr config = KSharedConfig::openConfig();

    KConfigGroup generalGroup(config, "general");
    generalGroup.writeEntry("string", textArea->toPlainText());
    std::cout << textArea->toPlainText().toStdString() << std::endl;

    // lessGeneral is a subgroup of general and appears as `[general][lessGeneral]`
    // in the `ini` file.
    KConfigGroup lessGeneralGroup(&generalGroup, "lessGeneral");

    // Any data type is ok:
    lessGeneralGroup.writeEntry("int", 1);
}

/*
#readEntry

    The second parameter is the default value.
*/
void MainWindow::viewConfig()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    // Name of the ini header is `[general]`.
    KConfigGroup generalGroup(config, "general");
    KConfigGroup lessGeneralGroup(config, "int");
    textArea->setText(
        "[general] string = " + generalGroup.readEntry("string", "abc") + "\n" +
        "[general][lessGeneral] int = " + QString::number(lessGeneralGroup.readEntry("int", 1)) + "\n" +
        "[configxt] serverName = " + serverName + "\n" +
        "[configxt] serverName = " + QString::number(port)
    );
}

/*
#Config XT

    Newer configuration method.

    Configuration is specified in an XML format.

    Configuration managed by Config XT is merged without problem to
    configuration managed via `KConfig` classes.
*/

/**
Load settings from disk into running instance.
*/
void MainWindow::loadSettingsXt() {
    serverName = Settings::serverName();
    port       = Settings::port();
}

/*
No explicit configuration save method such as this is needed:
that is done automatically by the code generated by kcfg.

This only shows that it is possible to save configurations "manually".
*/
void MainWindow::saveSettingsXt() {
    Settings::setServerName("www.kde.com");
    Settings::setPort(80);
    Settings::self()->writeConfig();
}


/*
#ConfigDialog

    A standard KDE config dialog. It contains:

    - "Ok", "Cancel", "Apply", "Defaults" buttons.

        Apply only become active when one settings has changed

        When the user hits Apply or Ok to save settings,
        if any settings have changed the `settingsChanged()` slot is activated.

    - side pages which can be added via the `addPage` method

    It is up to you do design each page, but KCOnfigDialog magically integrates with
    KConfig XT: all you have to do is to name input fields as:

        kcfg_NAME

    where `NAME` is the `name` attribute of the `kcfg` file!

    That's all! Not setting the input field values will automagically
    modify the configuration files as expected.
*/

void MainWindow::showSettingsDialog()
{
    // An instance of your dialog could be already created and could be
    // cached, in which case you want to display the cached dialog
    // instead of creating another one
    if (KConfigDialog::showDialog("settings"))
        return;

    // KConfigDialog didn't find an instance of this dialog, so lets
    // create it :
    KConfigDialog* dialog = new KConfigDialog(this, "settings",
                                            Settings::self());

    QWidget *widget = new QWidget;
    Ui::General ui;
    ui.setupUi(widget);
    dialog->addPage(widget, i18n("General"), "general");

    // User edited the configuration - update your local copies of the
    // configuration data
    connect(dialog, SIGNAL(settingsChanged(QString)),
            this, SLOT(loadSettingsXt()));

    dialog->show();
}

void MainWindow::setupActions()
{
    /*
    #StandardAction

        Offers standard actions.

        Those actions have standard names, icons and shortcuts.

        They are automatically put inside standard menus. For exampe, quit goes under "File".
    */
    {
        /* Adds "Quit" under file menu, default shortcut Ctrl + Q. */
        KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

        /* Adds "Appname Preferences" under "Settings" menu. */
        KStandardAction::preferences(this, SLOT(showSettingsDialog()), actionCollection());
    }

    // Sample KAction action with all possible action features.
    // Activates a slot directly on textArea.
    {
        KAction* action = new KAction(this);
        action->setText(i18n("&Clear"));
        action->setIcon(KIcon("document-new"));
        action->setShortcut(Qt::CTRL + Qt::Key_W);
        // The first argument determines the action name
        actionCollection()->addAction("clear", action);
        connect(action, SIGNAL(triggered(bool)),
                textArea, SLOT(clear()));
    }

    /*
    #XMLGUI

        GUI components that can be configured via *ui.rc XML files.

        Examples:

        - KXmlGuiWindow

        XML Schema: <http://www.kde.org/standards/kxmlgui/1.0/kxmlgui.xsd>

    #XmlGuiWindow

        Suitable for a main window.

        Is usually used as a base class.

        Provides:

        - main menu with:

            - Help
            - Settings with "Configure Shortcuts" and "Configure Toolbars"

                Configure Shortcuts" automatically contains any shortcuts added to actions,
                in addition to many standard shortcuts.

                TODO where is Configure Shortcuts config stored?

            - Show statusbar

        - status bar

        As the name suggests, this class is part of the XMLGUI KDE technology.

    #MainWindow

        Good possibility for main window class.

        Inherits KXmlGuiWindow, so supports XMLGUI.
    */

    // TODO how to fix all the properties of the action via XMLGUI?
    // For example, setText().

    // This kaction activates a slot on the main window.
    {
        KAction* action = new KAction(this);
        action->setText(i18n("View Configuration"));
        actionCollection()->addAction("viewConfig", action);
        connect(action, SIGNAL(triggered(bool)),
                this, SLOT(viewConfig()));
    }

    // saveConfig action
    {
        KAction* action = new KAction(this);
        action->setText(i18n("Save Conf"));
        actionCollection()->addAction("saveConfig", action);
        connect(action, SIGNAL(triggered(bool)),
                this, SLOT(saveConfig()));
    }

    {
        KAction* action = new KAction(this);
        action->setText(i18n("Save Settings XT"));
        actionCollection()->addAction("saveSettingsXt", action);
        connect(action, SIGNAL(triggered(bool)),
                this, SLOT(saveSettingsXt()));
    }

    {
        KAction* action = new KAction(this);
        action->setText(i18n("Load Settings XT"));
        actionCollection()->addAction("loadSettingsXt", action);
        connect(action, SIGNAL(triggered(bool)),
                this, SLOT(loadSettingsXt()));
    }
}

int main (int argc, char *argv[])
{
    // Most of the descriptive parameters show on the `Help > About` menu item.
    KAboutData aboutData(
        // The program name used internally.
        "kdecheat",
        // The message catalog name
        // If null, program name is used instead.
        0,
        // A displayable program name string.
        ki18n("KDE Cheat"),
        // The program version string.
        "1.0",
        // Short description of what the app does.
        ki18n("Short description."),
        // The license this code is released under
        KAboutData::License_GPL,
        // Copyright Statement
        ki18n("(c) 2007"),
        // Optional text shown in the About box.
        // Can contain any information desired.
        ki18n("Shown in the about box."),
        // The program homepage string.
        "http://example.com/",
        // The bug report email address
        "submit@bugs.kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KApplication app;

    MainWindow* window = new MainWindow();
    window->show();

    //#MessageBox

        /*
        KGuiItem yesButton( i18n( "Hello" ),
                            QString(),
                            i18n( "This is a tooltip" ),
                            i18n( "This is a WhatsThis help text." ) );

        KMessageBox::questionYesNo(0, i18n( "Hello World" ), i18n( "Hello" ), yesButton )
            == KMessageBox::Yes ? EXIT_SUCCESS : EXIT_FAILURE;
        */

    return app.exec();
}
