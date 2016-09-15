/*
 * Copyright (C) 2016 Jolla Ltd.
 * Contact: Martin Kampas <martin.kampas@jolla.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQuickView>

#include <sailfishapp.h>

#include <livenodeengine.h>
#include <remotereceiver.h>

namespace QmlLiveSailfishPriv {

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
# define QtInfoMsg QtWarningMsg
# define qCInfo qCWarning
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
Q_LOGGING_CATEGORY(preloadLog, "qmllive-sailfish.preload")
#else
Q_LOGGING_CATEGORY(preloadLog, "qmllive-sailfish.preload", QtInfoMsg)
#endif

const char *const IPC_PORT_ENVVAR = "QMLLIVERUNTIME_SAILFISH_IPC_PORT";
const char *const WORKSPACE_ENVVAR = "QMLLIVERUNTIME_SAILFISH_WORKSPACE";
const char *const UPDATE_ON_CONNECT_ENVVAR = "QMLLIVERUNTIME_SAILFISH_UPDATE_ON_CONNECT";
const char *const NO_UPDATES_AS_OVERLAY_ENVVAR = "QMLLIVERUNTIME_SAILFISH_NO_UPDATES_AS_OVERLAY";
const char *const LOAD_DUMMY_DATA_ENVVAR = "QMLLIVERUNTIME_SAILFISH_LOAD_DUMMY_DATA";
const int DEFAULT_IPC_PORT = 10234;

struct Args
{
    Args();

    int ipcPort = DEFAULT_IPC_PORT;
    QString workspace = SailfishApp::pathTo(QStringLiteral(".")).toLocalFile();
    LiveNodeEngine::WorkspaceOptions workspaceOptions = LiveNodeEngine::AllowUpdates | LiveNodeEngine::UpdatesAsOverlay;
    RemoteReceiver::ConnectionOptions connectionOptions;
};

Args::Args()
{
    if (qEnvironmentVariableIsSet(IPC_PORT_ENVVAR)) {
        const QByteArray ipcPortArg = qgetenv(IPC_PORT_ENVVAR);
        bool ok;
        const int ipcPortArg_ = ipcPortArg.toInt(&ok);
        if (ok) {
            ipcPort = ipcPortArg_;
        } else {
            qCritical() << "Invalid value of " << IPC_PORT_ENVVAR << " environment variable. "
                "Cannot convert to int: " << ipcPortArg;
        }
    }

    if (qEnvironmentVariableIsSet(WORKSPACE_ENVVAR)) {
        const QString workspaceArg = QString::fromLocal8Bit(qgetenv(WORKSPACE_ENVVAR));
        const QString resolved = QDir::isAbsolutePath(workspaceArg)
            ? workspaceArg
            : SailfishApp::pathTo(workspaceArg).toLocalFile();
        if (QFileInfo(resolved).isDir()) {
            workspace = resolved;
        } else {
            qCritical() << "Invalid value of " << WORKSPACE_ENVVAR << " environment variable. "
                "No such directory: " << workspaceArg;
            if (QDir::isRelativePath(workspaceArg))
                qCritical() << "    (relative paths are resolved relative to application directory)";
        }
    }

    if (qEnvironmentVariableIsSet(UPDATE_ON_CONNECT_ENVVAR)) {
        connectionOptions |= RemoteReceiver::UpdateDocumentsOnConnect | RemoteReceiver::BlockingConnect;
    }

    if (qEnvironmentVariableIsSet(NO_UPDATES_AS_OVERLAY_ENVVAR)) {
        workspaceOptions &= ~LiveNodeEngine::UpdatesAsOverlay;
    }

    if (qEnvironmentVariableIsSet(LOAD_DUMMY_DATA_ENVVAR)) {
        workspaceOptions |= LiveNodeEngine::LoadDummyData;
    }
}

void init(QQuickView *view)
{
    Args args;

    qCDebug(preloadLog) << "Initializing with view" << view << "[[";
    qCDebug(preloadLog) << "\tipcPort:" << args.ipcPort;
    qCDebug(preloadLog) << "\tworkspace:" << args.workspace;
    qCDebug(preloadLog) << "\tworkspaceOptions:" << args.workspaceOptions;
    qCDebug(preloadLog) << "\tconnectionOptions:" << args.connectionOptions << "]]";
    qCDebug(preloadLog) << "]]";

    LiveNodeEngine *node = new LiveNodeEngine(qApp);
    node->setQmlEngine(view->engine());
    node->setFallbackView(view);
    node->setWorkspace(args.workspace, args.workspaceOptions);

    RemoteReceiver *receiver = new RemoteReceiver(qApp);
    receiver->registerNode(node);

    // attention: sailfish-qtcreator relies on this!
    qCInfo(preloadLog) << "qmlliveruntime-sailfish initialized";

    receiver->listen(args.ipcPort, args.connectionOptions);
}

} // namespace QmlLiveSailfishPriv

namespace SailfishAppPriv {
void Q_DECL_EXPORT _PrivateAPI_DoNotUse_onViewCreated(QQuickView *view)
{
    QmlLiveSailfishPriv::init(view);
}
}
