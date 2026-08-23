/*
 * copyparty change-notification watcher.
 *
 * Watches copyparty's Tier-3 long-poll endpoint (/.cpr/sync/watch) for
 * server-side file changes and triggers a re-sync of the account's folders.
 * Falls back to polling the Tier-2 delta endpoint (/.cpr/sync/delta) if the
 * long-poll cannot be kept alive (e.g. a proxy that drops idle connections).
 */

#include "gui/copypartychangeswatcher.h"

#include "account.h"
#include "accountstate.h"
#include "folderman.h"
#include "libsync/copyparty.h"
#include "libsync/configfile.h"
#include "libsync/copypartydirrev.h"
#include "libsync/networkjobs/jsonjob.h"
#include "scheduling/syncscheduler.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLoggingCategory>
#include <QNetworkRequest>
#include <QTimer>

using namespace std::chrono_literals;

Q_LOGGING_CATEGORY(lcCopypartyChanges, "gui.copyparty.changes", QtInfoMsg)

namespace OCC {

CopypartyChangesWatcher::CopypartyChangesWatcher(AccountStatePtr accountState, QObject *parent)
    : QObject(parent)
    , _accountState(accountState)
{
    _cursor = ConfigFile().copypartyChangesCursor(accountState->account()->uuid());

    _timer.setInterval(30s);
    connect(&_timer, &QTimer::timeout, this, &CopypartyChangesWatcher::poll);

    connect(accountState.data(), &AccountState::isConnectedChanged, this, [this] {
        if (_accountState && _accountState->isConnected()) {
            poll();
            checkDirRev();
        }
    });

    if (accountState->isConnected()) {
        poll();
        checkDirRev();
    }
    _timer.start();
}

void CopypartyChangesWatcher::poll()
{
    if (_polling || !Copyparty::isEnabled()) {
        return;
    }
    if (!_accountState || !_accountState->isConnected()) {
        return;
    }

    _polling = true;
    auto *job = new JsonJob(_accountState->account(), _accountState->account()->url(),
        QStringLiteral("/.cpr/sync/") + (_useWatch ? QStringLiteral("watch") : QStringLiteral("delta")), "GET",
        SimpleNetworkJob::UrlQuery{{QStringLiteral("since"), _cursor.isEmpty() ? QStringLiteral("0") : _cursor}}, QNetworkRequest{}, this);
    job->setAuthenticationJob(true);
    // the watch endpoint holds the connection open ~25s; give it plenty of headroom.
    // the delta fallback only needs a short timeout.
    job->setTimeout(_useWatch ? std::chrono::seconds(40) : std::chrono::seconds(30));

    connect(job, &JsonJob::finishedSignal, this, [this, job] {
        _polling = false;
        const auto data = job->data();
        // A watch timeout reply is {"ok": true, "timeout": true} (no cursor/events),
        // which is still a successful long-poll. Only a parse/HTTP failure counts as broken.
        const bool failed = job->parseError().error != QJsonParseError::NoError || !data.value(QStringLiteral("ok")).toBool();

        if (_useWatch && failed) {
            // long-poll broken (e.g. proxy drops idle connections) - fall back to periodic delta polling
            qCInfo(lcCopypartyChanges) << u"copyparty watch failed, falling back to delta polling";
            _useWatch = false;
            _timer.start();
        }
        if (!failed) {
            handleReply(data);
        }
        job->deleteLater();

        if (_useWatch) {
            // keep the long-poll alive: re-issue it immediately
            QTimer::singleShot(0, this, &CopypartyChangesWatcher::poll);
        }
    });
    job->start();
}

void CopypartyChangesWatcher::checkDirRev()
{
    if (!_accountState || !_accountState->isConnected()) {
        return;
    }
    auto *job = new JsonJob(_accountState->account(), _accountState->account()->url(), QStringLiteral("/.cpr/sync/dirrev"), "GET",
        SimpleNetworkJob::UrlQuery{{QStringLiteral("path"), QStringLiteral("/")}}, QNetworkRequest{}, this);
    job->setAuthenticationJob(true);
    job->setTimeout(std::chrono::seconds(15));
    connect(job, &JsonJob::finishedSignal, this, [this, job] {
        const auto data = job->data();
        const bool ok = job->parseError().error == QJsonParseError::NoError && data.value(QStringLiteral("ok")).toBool();
        job->deleteLater();
        if (!ok) {
            return;
        }
        // Compare each top-level directory's recursive hash to the hash we last synced.
        // If any differ (or we have no cached hash - e.g. the last sync never completed),
        // the local state is out of date/broken, so trigger a sync to repair it.
        CopypartyDirRevCache cache(_accountState->account()->uuid().toString(QUuid::WithoutBraces));
        cache.load();
        bool changed = false;
        const auto dirs = data.value(QStringLiteral("dirs")).toObject();
        for (auto it = dirs.constBegin(); it != dirs.constEnd(); ++it) {
            const QString h = it.value().toObject().value(QStringLiteral("hash")).toVariant().toString();
            const QString base = cache.baseline(it.key());
            if (base.isEmpty() || base != h) {
                changed = true;
                break;
            }
        }
        if (changed) {
            qCInfo(lcCopypartyChanges) << u"copyparty dirrev hash differs from local, triggering re-sync";
            triggerSync();
        }
    });
    job->start();
}

void CopypartyChangesWatcher::handleReply(const QJsonObject &data)
{
    if (!data.contains(QStringLiteral("cursor"))) {
        return;
    }

    const auto cursor = data.value(QStringLiteral("cursor")).toVariant().toString();
    const bool truncated = data.value(QStringLiteral("truncated")).toBool();
    const bool timedOut = data.value(QStringLiteral("timeout")).toBool();
    const auto events = data.value(QStringLiteral("events")).toArray();

    if (truncated || (!timedOut && !events.isEmpty())) {
        qCInfo(lcCopypartyChanges) << u"copyparty server-side changes detected, triggering re-sync"
                                   << (truncated ? u"(truncated, full re-sync)" : u"") << u"cursor" << cursor;
        triggerSync();
    }

    if (!cursor.isEmpty() && cursor != _cursor) {
        _cursor = cursor;
        ConfigFile().setCopypartyChangesCursor(_accountState->account()->uuid(), cursor);
    }
}

void CopypartyChangesWatcher::triggerSync()
{
    for (auto *folder : FolderMan::instance()->folders()) {
        if (folder->accountState() && folder->accountState()->account() == _accountState->account()) {
            FolderMan::instance()->scheduler()->enqueueFolder(folder, SyncScheduler::Priority::Medium);
        }
    }
}

} // namespace OCC

#include "copypartychangeswatcher.moc"
