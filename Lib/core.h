#ifndef CORE_H
#define CORE_H

#include <QDataStream>
#include <QVector>

/*!
 * \class SourceDetails
 * \brief Holds all details of a backup path.
 *
 * BackupType   Determines whether to everything under the directory
 *              as it is or use a selective approach.
 *
 *              all     take everything
 *              selective   use one or more criteria to decide which sub-directories
 *              to include in the bacup
 *
 *
 * \note The backup path itself is not included in the data structure.
 *
 * @
 */
class SourceDetails {
public:
    /*!
     * \enum BackupType
     * \brief Backup to approach. Pick all or some.
     *
     * Determines whether to take everything under the root path or select some of the
     * sub-directories using one or more criteria.
     *
     * \value   all
     *          Blind backup everything
     * \value   selective
     *          Apply a picking algorithm based on BackupDepth and Selection fields.
     */
    enum BackupType {
        all = 0,
        selective = 1
    } backupType; // how to backup each sub-dir entry

    enum BackupDepth {
        rootOnly = 0,
        directChildren = 1,
        recursive = 2
    } backupDepth;

    enum PredicateType {
        containsFilenameId = 0,
        nameMatchesId = 1
    } predicateType;

    enum ActionType {
        rsync = 0,
        gitBundle = 1
    } actionType;

    /*!
     * \variable SourceDetails::precicate
     * \brief Applies filtering criteria for subdir selection.
     *
     * Each sub-directory resulting from backupType and backupDepth will be run against
     * the _predicate_ to deterine if it will be included in the backup. Returns true to
     * mark inclusion and false otherwise.
     *
     * The predicate syntactically takes the form a bash command.
     */
    QString predicate;
    QString sourcePath;

    QString containsFilename; // match when _traversed_ directory contains a file like this (use bash shell patterns)
    QString nameMatches; // match when _traversed_ directory's name is like this (use bash shell pattern)

public:
    //SourceDetails(const BackupType psourceType = all, const QString& ppredicate = nullptr, const BackupDepth pbackupDepth = rootOnly);
    SourceDetails();


    ~SourceDetails();

    bool operator==(const SourceDetails& other) const {
        return (backupType == other.backupType) &&
                (backupDepth == other.backupDepth) &&
                (predicateType == other.predicateType) &&
                (actionType == other.actionType) &&
                (predicate == other.predicate) &&
                (sourcePath == other.sourcePath) &&
                (containsFilename == other.containsFilename) &&
                (nameMatches == other.nameMatches);
    }

    friend QDataStream& operator<<(QDataStream& s, const SourceDetails& item);
    friend QDataStream& operator>>(QDataStream& s, SourceDetails& item);
    //friend bool operator==(const SourceDetails& one, const SourceDetails& another);
};

// Model struct to keep everything related to a backup in a single data structure.
struct BackupDetails {

    struct Tmp { // temprary stuff not stored to disk but required as long as the applications runs. Per-task.
        QString name;
        QString taskFilepath;
    } tmp;

    QString systemdId; // identifier part for the systemd service name
    //QString backupName; // identifier for task resources. systemd service, backup bash script etc.
    QString friendlyName; // user friendly name of the task
    QString systemdMountUnit; // e.g.  media-username-label
    QString destinationBasePath; // root directory of the inserted medium when mounted
    QString destinationBaseSuffixPath; // together with 'destinationBasePath' forms the destination directory for the backup

    BackupDetails() {}

    BackupDetails& operator=(const BackupDetails& from) {
        systemdId = from.systemdId;
        //backupName = from.backupName;
        tmp = from.tmp;
        friendlyName = from.friendlyName;
        systemdMountUnit = from.systemdMountUnit;
        destinationBasePath = from.destinationBasePath;
        destinationBaseSuffixPath = from.destinationBaseSuffixPath;

        return *this;
    }

    bool operator==(const BackupDetails& other) const {
        return (systemdId == other.systemdId) &&
                (friendlyName == other.friendlyName) &&
                (systemdMountUnit == other.systemdMountUnit) &&
                (destinationBasePath == other.destinationBasePath) &&
                (destinationBaseSuffixPath == other.destinationBaseSuffixPath);
    }

    BackupDetails(const BackupDetails& from) {
        systemdId = from.systemdId;
        //backupName = from.backupName;
        tmp = from.tmp;
        friendlyName = from.friendlyName;
        systemdMountUnit = from.systemdMountUnit;
        destinationBasePath = from.destinationBasePath;
        destinationBaseSuffixPath = from.destinationBaseSuffixPath;
    }

public:
    const QString &getDestinationBasePath() const;

private:
    Q_PROPERTY(QString destinationBasePath READ getDestinationBasePath CONSTANT)
};

// Data model used for readind/writing to disk. It doesn't allocate memory.

class BackupModel {
public:
    enum ValueType {
        // TODO - populates with all different types of data the model can have like backupType, backupDepth etc.
        unset,
        backupType
    };


    BackupDetails backupDetails;
    QVector<SourceDetails> allSourceDetails;
    typedef int SourceDetailsIndex; // used for indexing allSourceDetails

    BackupModel() {
        allSourceDetails.reserve(10);
    };

    BackupModel& operator=(const BackupModel& other);
    bool operator==(const BackupModel& other) const;



    friend QDataStream& operator << (QDataStream& s, const BackupModel& pmodel);
    friend QDataStream& operator >> (QDataStream& s, BackupModel& pmodel);
};


// application state that deserves to be saved to disk to ease user experience. But he/she cn also do without it.
class Session {
public:
    QString defaultBrowseBackupDirectory; // that's the starting location when browsing fs for the directory to backup
    QVector<QString> recentBackupNames;

    friend QDataStream& operator << (QDataStream& s, const Session& o);
    friend QDataStream& operator >> (QDataStream& s, Session& o);
};

// application operation state. Things that we don't want to load and check again and again
class State {
public:
    bool backupNamed = false; // is the backup named ? If true, activeBackup->backupName is full and valid
    bool triggerExists; // is a systemd mount hook service file in place ?
    BackupModel modelCopy; // reflects the state of the application as it is persisted to storage. Helps to see if anything has changed.

    State() : triggerExists(false)
    {}
};

namespace Lb {

bool loadPersistedFile(const QString backupFilename, BackupModel& persisted);

}


#endif // CORE_H
