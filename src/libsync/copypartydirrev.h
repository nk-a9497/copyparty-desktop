/*
 * copyparty per-directory revision cache.
 *
 * Stores the last-seen dirrev value for each top-level directory so the discovery can
 * skip unchanged subtrees (see /.cpr/sync/dirrev). Values are opaque monotonic integers;
 * only equality is meaningful. Persisted as JSON next to the sync journal.
 *
 * Kept as a plain data holder (no QObject) so it is trivially copyable and can be
 * captured by value in a std::function callback without pulling in Qt's object system.
 */

#pragma once

#include "opencloudsynclib.h"

#include <QHash>
#include <QSet>
#include <QString>

namespace OCC {

class OPENCLOUD_SYNC_EXPORT CopypartyDirRevCache
{
public:
    explicit CopypartyDirRevCache(const QString &accountUuid);

    /// Last-seen revision for a remote path ("" for the root), or empty if unknown.
    QString baseline(const QString &path) const;
    void setBaseline(const QString &path, const QString &revision);

    /// Directories found unchanged by the latest dirrev scan (this sync).
    const QSet<QString> &unchangedDirs() const { return _unchanged; }
    void clearUnchanged() { _unchanged.clear(); }
    void addUnchanged(const QString &path) { _unchanged.insert(path); }
    bool isUnchanged(const QString &path) const { return _unchanged.contains(path); }

    void load();
    void save();

private:
    QString _filePath;
    QHash<QString, QString> _baseline;
    QSet<QString> _unchanged;
};

} // namespace OCC
