/*
 * copyparty change-notification watcher.
 *
 * Watches copyparty's Tier-3 long-poll endpoint (/.cpr/sync/watch) for
 * server-side file changes and triggers a re-sync of the account's folders.
 * Falls back to polling the Tier-2 delta endpoint (/.cpr/sync/delta) if the
 * long-poll cannot be kept alive (e.g. a proxy that drops idle connections).
 */

#pragma once

#include "accountfwd.h"

#include <QObject>
#include <QTimer>

class QJsonObject;

namespace OCC {

class CopypartyChangesWatcher : public QObject
{
    Q_OBJECT
public:
    explicit CopypartyChangesWatcher(AccountStatePtr accountState, QObject *parent = nullptr);

private:
    void poll();
    void handleReply(const QJsonObject &data);
    void triggerSync();

    AccountStatePtr _accountState;
    QTimer _timer;
    QString _cursor;
    bool _polling = false;
    bool _useWatch = true;
};

} // namespace OCC
