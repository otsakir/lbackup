#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDataWidgetMapper>
#include <QItemSelection>
#include <QMainWindow>
#include <QStandardItemModel>
#include <systemdunitdialog.h>

#include <QProcess>
#include <core.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Forward (abstract) type declarations
struct BackupDetails;
class BackupModel;
class SourceDetails;
class Session;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void methodControlChanged(int methodIndex); // raised when the backup method UI control is updated. Helps to chain actions to update the ui state (hide/show other controls etc.)
    void actionChanged(SourceDetails::ActionType action);
    //void backupNameChanged(QString backupName); // signal raised when the backup name is changed. Be it set to another value or cleared altogether.
    void newBackupName(QString backupName); // there is a new backup name established!
    void PleaseQuit();
    void friendlyNameEdited(); // there is new content in activeBackup.backupDetails.friendlyName
    void systemdUnitChanged(QString unitName);
    void modelUpdated(BackupModel::ValueType valueType = BackupModel::unset);

private slots:

    // custom slots
    void on_actionChanged(SourceDetails::ActionType action);
    void onNewBackupName(QString backupName);
    void onSystemdUnitChanged(QString newUnitName);
    void onModelUpdated(BackupModel::ValueType valueType);


    void on_pushButton_clicked();

    void on_updateSelection(const QItemSelection &selected, const QItemSelection &deselected);

    void on_currentChanged(const QModelIndex &current, const QModelIndex &previous);
    //void on_pushButton_2_clicked();

    void updateSourceDetailControls(const QModelIndex& current);

    SourceDetails* getSelectedSourceDetails();

    void on_removeSourceButton_clicked();

    void on_pushButtonSelectDevice_clicked();

    void on_comboBoxDepth_currentIndexChanged(int index);

    void on_radioButtonAll_toggled(bool checked);

    void on_radioButtonSelective_toggled(bool checked);

    void on_lineEditContainsFilename_editingFinished();

    void on_lineEditNameMatches_editingFinished();

    void updatePredicateTypeIndex(int index);

    void on_lineEditSystemdUnit_textChanged(const QString &arg1);

    void on_lineEditDestinationSuffixPath_textChanged(const QString &arg1);

    void on_lineEditDestinationSuffixPath_editingFinished();

    void on_pushButtonChooseDestinationSubdir_clicked();

    void on_activeBackupMethodChanged(int backupType);

    void on_action_New_triggered();

    void on_ButtonApply_clicked();

    void on_action_Open_triggered();

    void on_pushButtonRefreshBasePaths_clicked();

    void on_comboBoxBasePath_currentIndexChanged(const QString &arg1);

    void on_action_Save_triggered();

    void on_pushButtonInstallTrigger_clicked();

    void on_pushButtonRemoveTrigger_clicked();

    void on_actionDelete_triggered();

    void newBackupTaskFromDialog(qint32 dialogMode);

    void on_pushButtonEditFriendlyName_toggled(bool checked);

    void on_lineEditFriendlyName_returnPressed();

    void onFriendlyNameEdited(); // handler for custom signal 'friendlyNameEdited

    void on_radioButtonRsync_toggled(bool checked);

    void on_radioButtonGitBundle_toggled(bool checked);

    void onListViewCurrentChanged(const QModelIndex& current, const QModelIndex& previous);

    void on_actionAbout_triggered();

    void on_toolButtonRun_triggered(QAction *arg1);

    void on_toolButtonRun_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;

    QStandardItemModel* sourcesModel;
    QDataWidgetMapper* sourcesDataMapper;
    BackupModel* activeBackup; // contains additional info about a backup except source stuff (i.e.like path, predicate, type etc.)
    Session session;
    State state; // generic application state. Not part of a backup.
    QProcess consoleProcess;

    void loadTask(QString taskId);
    bool loadPersisted(QString backupName, BackupModel& persisted);
    QStandardItem* appendSource(BackupModel::SourceDetailsIndex sourceDetails);
    void collectUIControls(BackupModel& persisted);
    void initUIControls(BackupModel& persisted);

    void applyChanges();
    void refreshBasePaths(QString current);
    //void enableMostUI(bool enable);
    void setupTriggerButtons(const QString& backupName);

    void consoleProcessStarted();
    void consoleProcessDataAvail();
    void consoleProcessFinished(int exitCode);
};
#endif // MAINWINDOW_H
