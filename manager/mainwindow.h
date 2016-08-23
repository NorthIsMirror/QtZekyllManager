#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "selectedzekylls.h"
#include "lzcsde.h"
#include "zkiresize.h"
#include "zkrewrite.h"
#include "git.h"
#include "lgit.h"
#include "gitoperationtracker.h"

#include <tuple>
#include <vector>
#include <string>

#include <QByteArray>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QTableWidget>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QString current_repo_;
    QString current_path_;
    int current_index_;
    int current_size_of_index_;
    bool isConsistent_;
    bool isConsistent2_;

    QTimer timer_;
    bool is_loading_;

    std::vector<std::string> ZKL_INDEX_ZEKYLLS_;
    ZkIResize *zkiresize_;
    ZkRewrite *zkrewrite_;
    Git *git_;
    lgit *lgit_;

    LZCSDE lzcsde_list_;
    LZCSDE lzcsde_section_;
    LZCSDE lzcsde_consistent_;
    SelectedZekylls sel_zekylls_initial_;
    SelectedZekylls sel_zekylls_current_;
    ZekyllIDs zekyllIDs_;

    LZCSDE lzcsde_initial_;
    std::pair<LZCSDE, LZCSDE> lzcsde_renamed_from_to_;
    LZCSDE lzcsde_deleted_;
    int new_size_of_index_;

    bool isDeferredApplyPrepared_;
    std::vector<int> deferredCodeSelectors_;
    bool combo_box_reactions_limited_;

    QString msg_incorrect_ = tr("Incorrect path selected");

    QByteArray home_path_;
    QVector<QString> repos_paths_;
    QString ZEKYLL_DEFAULT_REPO_;

    QVector<int> vscroll_bar_value_;

    GitOperationTracker op_tracker_;

    Ui::MainWindow *ui;

    void insertLZCSDTableRow(const QString & lzcsde, QTableWidget * tableWidget, int id, const QString & zekyll, bool checked, const QString & section, const QString & description);
    void insertLZSDETableRow(const QString & lzcsde, QTableWidget * tableWidget, int id, const QString & zekyll, const QString & section,
                                const QString & description, const QString & error);

    void closeEvent(QCloseEvent *event);
    void processCurRepoCombo( const QString & selected, int error );
    void setupDeferredApplyOfCodeSelectors( const std::vector<int> & bits ) { isDeferredApplyPrepared_ = true; deferredCodeSelectors_ = bits; }
    int applyDeferredCodeSelectors( bool silent );
    void limitComboBoxReactions( bool should_limit ) { combo_box_reactions_limited_ = should_limit; }
    int setCheckedInTable( bool selected, QTableWidget *table, int row, bool silent );
    std::tuple<int,int> setCheckedViaTableInLZCSDE( bool selected, QTableWidget *table, LZCSDE & lzcsde, int row, bool silent );
    bool errorOnDisallowedChars( const QString & type, const QStringList & invalidChars );
    std::tuple< std::vector<int>, int > gatherCodeSelectors();
    int applyCodeSelectors( const std::vector<int> & bits_, bool silent = false );
    std::tuple<QString, int> SetFromRepoOrPathGetSelection( QString repoOrPath, bool isRepo );
    std::tuple<QString, int> loadDefaultRepository();
    std::tuple<bool, int> stepIntegerQLineEdit( QLineEdit *lineEdit, int min, int max, const QString & msg, bool subtract = false );
    int rememberScrollBars();
    bool setCurrentIndexInZcode();
    void writeSettings();
    void readSettings();

signals:
    void repositoryChanged();

public slots:
    void handle_zkiresize_list(int exitCode, QStringList entries);
    void handle_zkiresize_consistent(int exitCode, QStringList entries);
    void handle_zkiresize_resize(int exitCode, QStringList entries);
    void handle_zkrewrite(int exitCode, QStringList entries);
    void handle_git_rm(int exitCode, QStringList entries);
    void handle_git_mv(int exitCode, QStringList entries);
    void updateMessages( const QStringList & messages );
    void stopThirdTabMarking();

private slots:
    void on_actionQuit_triggered();

    void browse();

    void reloadRepository();

    void checkBoxClicked( bool checked );

    void on_tableWidget_cellDoubleClicked( int row, int column );

    void on_up_clicked();

    void on_down_clicked();

    void on_minus_clicked();

    void on_save_clicked();

    void on_tableWidget_itemChanged(QTableWidgetItem *item);

    void on_zcode_editingFinished();

    void on_rev_editingFinished();

    void on_file_editingFinished();

    void on_userRepo_editingFinished();

    void on_site_currentIndexChanged(int index);

    void on_curRepoCombo_activated(int index);

    void on_zcode_returnPressed();

    void on_rev_returnPressed();

    void on_file_returnPressed();

    void on_userRepo_returnPressed();

    void on_indexSize_returnPressed();

    void on_currentIndex_returnPressed();

    void on_defaultRepoButton_clicked();

    void on_currentIndex_editingFinished();

    void on_sizeLeft_clicked();

    void on_sizeRight_clicked();

    void on_indexLeft_clicked();

    void on_indexRight_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_gitReset_clicked();

    void on_gitCommit_clicked();

    void on_gitPull_clicked();

    void on_e_clicked();

public:
    int currentIndex() const { return current_index_; }
    std::tuple<bool, QString, int, QString> getProcessedZcodeInput();
    bool recomputeZcode();
    static MainWindow* ptr();
};

#endif // MAINWINDOW_H
