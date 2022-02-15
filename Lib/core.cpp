#include "core.h"

#include <QDebug>
#include <QFile>

SourceDetails::SourceDetails() :
    backupType(all),
    backupDepth(rootOnly),
    predicateType(containsFilenameId) {}

SourceDetails::~SourceDetails() {
}

bool BackupModel::operator==(const BackupModel& other) const {
    return (allSourceDetails == other.allSourceDetails) &&
            (backupDetails == other.backupDetails);

}

BackupModel& BackupModel::operator=(const BackupModel& other) {
    backupDetails = other.backupDetails;
    allSourceDetails = other.allSourceDetails;
    allSourceDetails.detach();
}

QDataStream& operator<<(QDataStream& s, const SourceDetails& item) {
    s << (qint32)item.backupType
      << (qint32)item.backupDepth
      << item.predicate
      << item.sourcePath
      << item.containsFilename
      << item.nameMatches
      << (qint32) item.predicateType
      << (qint32) item.actionType;
    return s;
}

QDataStream& operator>>(QDataStream& s, SourceDetails& item) {
    qint32 temp;
    s >> temp;
    item.backupType = (SourceDetails::BackupType)temp;
    s >> temp;
    item.backupDepth = (SourceDetails::BackupDepth)temp;
    s >> item.predicate;
    s >> item.sourcePath;
    s >> item.containsFilename;
    s >> item.nameMatches;
    s >> temp;
    item.predicateType = (SourceDetails::PredicateType) temp;
    s >> temp;
    item.actionType = (SourceDetails::ActionType) temp;
    return s;
}

QDataStream& operator << (QDataStream& s, const BackupDetails& backupDetails) {
    //s << backupDetails.backupName;  // skip this. We treat it separately
    s << backupDetails.friendlyName;
    s << backupDetails.systemdId;
    s << backupDetails.systemdMountUnit;
    s << backupDetails.destinationBasePath;
    s << backupDetails.destinationBaseSuffixPath;
    return s;
}

QDataStream& operator >> (QDataStream& s, BackupDetails& backupDetails) {
    //s >> backupDetails.backupName; // skip this. We treat it separately
    s >> backupDetails.friendlyName;
    s >> backupDetails.systemdId;
    s >> backupDetails.systemdMountUnit;
    s >> backupDetails.destinationBasePath;
    s >> backupDetails.destinationBaseSuffixPath;
    return s;
}

QDataStream& operator << (QDataStream& s, const BackupModel& pmodel) {
    s << pmodel.backupDetails;
    s << pmodel.allSourceDetails;
    return s;
}

QDataStream& operator >> (QDataStream& s, BackupModel& pmodel) {
    s >> pmodel.backupDetails;
    s >> pmodel.allSourceDetails;
    return s;
}

QDataStream& operator << (QDataStream& s, const Session& o) {
    s << o.defaultBrowseBackupDirectory;
    s << o.recentBackupNames;
    return s;
}

QDataStream& operator >> (QDataStream& s, Session& o) {
    s >> o.defaultBrowseBackupDirectory;
    s >> o.recentBackupNames;
    return s;
}

namespace Lb {

bool loadPersistedFile(const QString backupFilename, BackupModel& persisted) {
    qDebug() << "loading task file: " << backupFilename;
    QFile ifile(backupFilename);
    if (ifile.open(QIODevice::ReadOnly)) {
        QDataStream istream(&ifile);
        istream >> persisted;
        ifile.close();
        return true;
    }
    return false;
}

}
