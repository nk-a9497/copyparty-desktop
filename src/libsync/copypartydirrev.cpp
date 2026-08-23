/*
 * copyparty per-directory revision cache.
 *
 * Stores the last-seen dirrev value for each top-level directory so the discovery can
 * skip unchanged subtrees (see /.cpr/sync/dirrev). Values are opaque monotonic integers;
 * only equality is meaningful. Persisted as JSON next to the sync journal.
 */

#include "libsync/copypartydirrev.h"

#include "libsync/configfile.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QStandardPaths>

Q_LOGGING_CATEGORY(lcCopypartyDirRev, "sync.copyparty.dirrev", QtInfoMsg)

namespace OCC {

CopypartyDirRevCache::CopypartyDirRevCache(const QString &accountUuid)
{
    _filePath = ConfigFile::configPath() + QStringLiteral("/copyparty-dirrev-") + accountUuid + QStringLiteral(".json");
}

QString CopypartyDirRevCache::baseline(const QString &path) const
{
    return _baseline.value(path);
}

void CopypartyDirRevCache::setBaseline(const QString &path, const QString &revision)
{
    _baseline[path] = revision;
}

void CopypartyDirRevCache::load()
{
    QFile f(_filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    const auto doc = QJsonDocument::fromJson(f.readAll());
    const auto obj = doc.object();
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        _baseline[it.key()] = it.value().toString();
    }
}

void CopypartyDirRevCache::save()
{
    QJsonObject obj;
    for (auto it = _baseline.constBegin(); it != _baseline.constEnd(); ++it) {
        obj[it.key()] = it.value();
    }
    QFile f(_filePath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    }
}

} // namespace OCC
