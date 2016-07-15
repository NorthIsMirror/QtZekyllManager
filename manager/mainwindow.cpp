#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "zmeditor.h"
#include "math_functions.h"
#include "coding_functions.h"
#include "script_functions.h"
#include "singleton.h"
#include "messages.h"
#include <QDebug>
#include <QRegExp>
#include <QCheckBox>
#include <QDir>
#include <QFileDialog>

#define MessagesI Singleton<Messages>::instance()

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    isConsistent_(true),
    is_loading_(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->saveTheColor();
    ui->currentIndex->setText(tr("1"));
    current_index_ = 1;

    QStringList labels;
    labels << tr("Id") << tr("Zekyll") << tr("Use") << tr("Section") << tr("Description");
    ui->tableWidget->setHorizontalHeaderLabels(labels);
    ui->tableWidget->horizontalHeader()->setStretchLastSection( true );
    ui->tableWidget->setColumnHidden(0, true);
    ui->tableWidget_2->setHorizontalHeaderLabels(labels);
    ui->tableWidget_2->horizontalHeader()->setStretchLastSection( true );
    ui->tableWidget_2->setColumnHidden(0, true);
    labels.clear();
    labels << tr("Id") << tr("Zekyll") << tr("Section") << tr("Description") << tr("Error");
    ui->tableWidget_3->setHorizontalHeaderLabels(labels);
    ui->tableWidget_3->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableWidget_3->horizontalHeader()->setStretchLastSection( true );
    ui->tableWidget_3->setColumnHidden(0, true);

    QString PATH = QString::fromLocal8Bit( qgetenv("PATH") );
    if( PATH.indexOf( ":" ) != -1 ) {
        PATH += ":" + QString::fromLocal8Bit( qgetenv("HOME") ) + "/.zekyll/zekyll";
    }
    qputenv( "PATH", PATH.toLocal8Bit() );

    zkiresize_ = new ZkIResize();

    // Connect ZkIResize to obtain list of zekylls
    QObject::connect(this->zkiresize_,
                     SIGNAL(result_list(int, QStringList)),
                     this,
                     SLOT(handle_zkiresize_list(int, QStringList)));

    // Connect ZkIResize to obtain list of inconsistent zekylls
    QObject::connect(this->zkiresize_,
                     SIGNAL(result_consistent(int, QStringList)),
                     this,
                     SLOT(handle_zkiresize_consistent(int, QStringList)));

    // Connect ZkIResize to be able to resize index
    QObject::connect(this->zkiresize_,
                     SIGNAL(result_resize(int, QStringList)),
                     this,
                     SLOT(handle_zkiresize_resize(int, QStringList)));

    zkrewrite_ = new ZkRewrite();

    // Connect ZkRewrite
    QObject::connect(this->zkrewrite_,
                     SIGNAL(result(int, QStringList)),
                     this,
                     SLOT(handle_zkrewrite(int, QStringList)));

    git_ = new Git();

    QObject::connect(this->git_,
                     SIGNAL(result_git_rm(int, QStringList)),
                     this,
                     SLOT(handle_git_rm(int, QStringList)));

    QObject::connect(this->git_,
                     SIGNAL(result_git_mv(int, QStringList)),
                     this,
                     SLOT(handle_git_mv(int, QStringList)));

    connect(ui->curRepoButton, &QAbstractButton::clicked, this, &MainWindow::browse);
    connect(this, &MainWindow::repositoryChanged, this, &MainWindow::reloadRepository);
    QObject::connect(&MessagesI, SIGNAL(messagesChanged(const QStringList&)), this, SLOT(updateMessages(const QStringList&)) );
    QObject::connect(this->ui->tabWidget, SIGNAL(thirdTabMarkingEnough()), this, SLOT(stopThirdTabMarking()));

    create_codes_map();
    create_rcodes_map();

    zkiresize_->setIndex( current_index_ );
    reloadRepository();

    tuple< vector<string>, int > set_index_result = setIndex( current_index_ );
    ZKL_INDEX_ZEKYLLS_ = get<0>(set_index_result);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete zkiresize_;
}

void MainWindow::handle_zkiresize_list(int exitCode, QStringList entries) {
    current_size_of_index_ = 0;
    this->ui->indexSize->setText( QString("%1").arg( current_size_of_index_ ) );

    if( exitCode != 0 && exitCode != 12 ) {
        QString error_decode = decode_zkiresize_exit_code(exitCode);
        if( error_decode != "" ) {
            MessagesI.AppendMessageT( tr("<font color=green>Message from the Zekyll backend (1):</font> ") + error_decode );
        }
        is_loading_ = false;
        return;
    }

    isConsistent_ = (exitCode == 0);

    QRegExp rx("([a-z0-9][a-z0-9][a-z0-9])\\.([A-Z])--(.*)");
    rx.setCaseSensitivity(Qt::CaseSensitive);
    int counter = 0;
    foreach (const QString &str, entries) {
        if (rx.indexIn(str) != -1) {
            counter ++;
            lzcsde_list_.insertFromListing(counter, str);
            insertLZCSDTableRow(ui->tableWidget, counter, rx.cap(1), true, rx.cap(2), rx.cap(3));
        }
    }

    lzcsde_initial_.clear();
    lzcsde_initial_ = lzcsde_list_;

    current_size_of_index_ = lzcsde_list_.count();
    this->ui->indexSize->setText( QString("%1").arg( current_size_of_index_ ) );

    if( entries.count() == 0 ) {
        MessagesI.AppendMessageT(tr("Index ") + QString("%1") . arg(current_index_) + tr(" is empty (go ahead and resize it), or selected path isn't a Zekyll repository"));
    }

    // If exitCode is 12, there was inconsistent read
    // started, and whole listing actually stops there
    if( exitCode != 12 ) {
        is_loading_ = false;
    }
}

void MainWindow::handle_zkiresize_consistent(int exitCode, QStringList entries) {
    if( exitCode != 11 ) {
        isConsistent2_ = (exitCode == 0);
        QString error_decode = decode_zkiresize_exit_code(exitCode);
        if( error_decode != "" ) {
            MessagesI.AppendMessageT( tr("<font color=green>Message from the Zekyll backend (2):</font> ") + error_decode );
        }
        is_loading_ = false;
        return;
    }
    isConsistent2_ = false;

    QRegExp rx1("([a-z0-9][a-z0-9][a-z0-9])\\.([A-Z])--(.*) >>(.*)<<");
    QRegExp rx2("([a-z0-9][a-z0-9][a-z0-9]) >>(.*)<<");
    rx1.setCaseSensitivity(Qt::CaseSensitive);
    rx2.setCaseSensitivity(Qt::CaseSensitive);
    int counter = 0;
    foreach (const QString &str, entries) {
        if( str == "consistent" || str == "inconsistent" ) {
            continue;
        }

        if (rx1.indexIn(str) != -1) {
            counter ++;
            lzcsde_consistent_.insertFromListing(counter, str);
            insertLZSDETableRow(ui->tableWidget_3, counter, rx1.cap(1), rx1.cap(2), rx1.cap(3), rx1.cap(4));
        } else if (rx2.indexIn(str) != -1 ) {
            counter ++;
            lzcsde_consistent_.insertFromListing(counter, str);
            insertLZSDETableRow(ui->tableWidget_3, counter, rx2.cap(1), "", "", rx2.cap(2));
        } else {
            // TODO report inproper input
        }
    }

    MessagesI.AppendMessageT( tr("<font color=red>Index ") + QString("%1") . arg(current_index_) + tr(" is inconsistent</font>") );

    if(!timer_.isActive()) {
        timer_.setInterval(500);
        timer_.setSingleShot(false);
        connect(&timer_, SIGNAL(timeout()), ui->tabWidget, SLOT(toggleThirdTabMark()));
        timer_.start();
    }

    is_loading_ = false;
}

void MainWindow::insertLZCSDTableRow(QTableWidget * tableWidget, int id, const QString & zekyll, bool checked, const QString & section, const QString & description) {
    QTableWidgetItem *idItem = new QTableWidgetItem(QString("%1").arg(id));
    idItem->setFlags(idItem->flags() ^ Qt::ItemIsEditable);
    idItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    QTableWidgetItem *zekyllItem = new QTableWidgetItem(zekyll);
    zekyllItem->setFlags(zekyllItem->flags() ^ Qt::ItemIsEditable);
    zekyllItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    QTableWidgetItem *checkItem = new QTableWidgetItem(tr(""));
    checkItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    QCheckBox *checkBox = new QCheckBox();
    if( checked ) {
        checkBox->setCheckState(Qt::Checked);
    } else {
        checkBox->setCheckState(Qt::Unchecked);
    }

    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(checkBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0,0,0,0);
    widget->setLayout(layout);

    QTableWidgetItem *sectionItem = new QTableWidgetItem(section);
    sectionItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    QTableWidgetItem *descriptionItem = new QTableWidgetItem(description);
    descriptionItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);

    tableWidget->setItem(row, 0, idItem);
    tableWidget->setItem(row, 1, zekyllItem);
    tableWidget->setItem(row, 2, checkItem);
    tableWidget->setCellWidget(row, 2, widget);
    tableWidget->setItem(row, 3, sectionItem);
    tableWidget->setItem(row, 4, descriptionItem);
}

void MainWindow::insertLZSDETableRow(QTableWidget * tableWidget, int id, const QString & zekyll, const QString & section,
                                        const QString & description, const QString & error)
{
    QTableWidgetItem *idItem = new QTableWidgetItem(QString("%1").arg(id));
    idItem->setFlags(idItem->flags() ^ Qt::ItemIsEditable);
    idItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    QTableWidgetItem *zekyllItem = new QTableWidgetItem(zekyll);
    zekyllItem->setFlags(zekyllItem->flags() ^ Qt::ItemIsEditable);
    zekyllItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    QTableWidgetItem *sectionItem = new QTableWidgetItem(section);
    sectionItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    QTableWidgetItem *descriptionItem = new QTableWidgetItem(description);
    descriptionItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QTableWidgetItem *errorItem = new QTableWidgetItem(error);
    errorItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);

    tableWidget->setItem(row, 0, idItem);
    tableWidget->setItem(row, 1, zekyllItem);
    tableWidget->setItem(row, 2, sectionItem);
    tableWidget->setItem(row, 3, descriptionItem);
    tableWidget->setItem(row, 4, errorItem);
}

void MainWindow::browse()
{
    QString msg_incorrect = tr("Incorrect path selected");
    QString directory = QFileDialog::getExistingDirectory(this,
                               tr("Select repository"), QDir::homePath() + "/.zekyll/repos");

    if (!directory.isEmpty()) {
        QString repo, path, selected;
        int error;
        tie(repo, path, error) = getRepoFromPath( directory );

        if( error == 0 ) {
            current_repo_ = repo;
            current_path_ = path;
            selected = repo;
        } else if ( error == 2 ) {
            current_repo_ = "";
            current_path_ = path;
            selected = path;
        } else if ( error == 1 ) {
            selected = msg_incorrect;
        }

        // Remove any previous "Incorrect path selected"
        if( error == 0 || error == 2 ) {
            int idx = ui->curRepoCombo->findText(msg_incorrect);
            if( idx != -1) {
                ui->curRepoCombo->removeItem( idx );
            }
        }

        // Add item if new
        if (ui->curRepoCombo->findText(selected) == -1) {
            ui->curRepoCombo->addItem(selected);
        }

        ui->curRepoCombo->setCurrentIndex(ui->curRepoCombo->findText(selected));

        if( error == 0 || error == 2 ) {
            emit repositoryChanged();
        }
    }
}

void MainWindow::reloadRepository() {
    is_loading_ = true;
    ui->tableWidget->setRowCount(0);
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_3->setRowCount(0);

    lzcsde_list_.clear();
    lzcsde_initial_.clear();
    lzcsde_consistent_.clear();
    lzcsde_renamed_from_to_.first.clear();
    lzcsde_renamed_from_to_.second.clear();
    lzcsde_deleted_.clear();

    git_->setRepoPath( current_path_ );

    zkrewrite_->setRepoPath( current_path_ );

    zkiresize_->setRepoPath( current_path_ );
    zkiresize_->setIndex( current_index_ );
    zkiresize_->list();
}

 void MainWindow::updateMessages( const QStringList & messages ) {
     ui->messages->clear();
     int counter = messages.count();
     QStringList::const_iterator it = messages.constEnd();
     while( it != messages.constBegin() ) {
         -- it;
         QString str2 = *it;
         str2.replace( QRegExp("[\r\n]+"), QString("<br/>") );
         str2 = QString("<font color=magenta>{<b>%1</b>}</font> ").arg( counter ) + str2;
         ui->messages->appendHtml( str2 );
         -- counter;
     }
     ui->messages->moveCursor (QTextCursor::Start);
     ui->messages->ensureCursorVisible();
 }

 void MainWindow::stopThirdTabMarking() {
     timer_.stop();
 }

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    if( column == 1 ) {
        QTableWidgetItem *item = ui->tableWidget->item( row, 0 );
        QString text = item->text();
        LZCSDE_Entry entry = lzcsde_list_.getId( text );
        if( entry.id() != -1 ) {
            ZMEditor *editor = new ZMEditor(this);
            editor->setFile( current_path_ + "/" + entry.listing() );
            editor->show();
        } else {
            MessagesI.AppendMessageT("<font color=red>Could not find data associated with the clicked cell</font>");
        }
    }
}

void MainWindow::on_up_clicked()
{
    if(ui->tabWidget->currentIndex() == 0) {
        int sourceId, destId;
        tie(sourceId, destId) = ui->tableWidget->move(true);
        if( sourceId == -2 || destId == -2 ) {
            MessagesI.AppendMessageT("<font color=red>Internal error during element move (1)</font>");
        } else if ( sourceId > 0 && destId > 0 ) {
            lzcsde_list_.move( sourceId, destId );
        }
    }
}

void MainWindow::on_down_clicked()
{
    if(ui->tabWidget->currentIndex() == 0) {
        int sourceId, destId;
        tie(sourceId, destId) = ui->tableWidget->move(false);
        if( sourceId == -2 || destId == -2 ) {
            MessagesI.AppendMessageT("<font color=red>Internal error during element move (2)</font>");
        } else if ( sourceId > 0 && destId > 0 ) {
            lzcsde_list_.move( sourceId, destId );
        }
    }
}

void MainWindow::on_minus_clicked()
{
    if(ui->tabWidget->currentIndex() == 0) {
        if( ui->tableWidget->selectedItems().count() > 0)
        {
            int row = ui->tableWidget->currentRow();
            QTableWidgetItem *item = ui->tableWidget->item(row, 0);
            QString id = item->text();
            if( !lzcsde_list_.removeId(id) ) {
                MessagesI.AppendMessageT("<font color=red>Internal error during element removal</font>");
            }
            ui->tableWidget->removeRow( row );
        }
    }
}

void MainWindow::on_save_clicked()
{
    const QVector<LZCSDE_Entry> & initial_entries = lzcsde_initial_.entries();
    int size = initial_entries.count();

    // Gather renames
    int changed;
    for( int i=0; i<size; i++ ) {
        int idx = lzcsde_list_.findIdxOfId( initial_entries[i].id() );
        if( idx != -1 ) {
            const LZCSDE_Entry & initial = initial_entries[i];
            const LZCSDE_Entry & current = lzcsde_list_.entries()[idx];
            changed = 0;

            // Section changed?
            if( initial.section() != current.section() ) {
                changed = 1;
            }

            // Description changed?
            if( initial.description() != current.description() ) {
                changed += 2;
            }

            if( changed > 0 ) {
                lzcsde_renamed_from_to_.first.appendEntry( initial );
                lzcsde_renamed_from_to_.second.appendEntry( current );
            }
        }
    }

    // Gather deletions
    for( int i=0; i<size; i++ ) {
        int idx = lzcsde_list_.findIdxOfId( initial_entries[i].id() );
        if( idx == -1 ) {
            lzcsde_deleted_.appendEntry( initial_entries[i] );
        }
    }

    // Establish new sequence of zekylls
    QStringList current_zekylls = lzcsde_list_.getZekylls();
    QStringList newer_zekylls;
    size = current_zekylls.size();
    for( int i=0; i<size; i++ ) {
        newer_zekylls << QString( ZKL_INDEX_ZEKYLLS_[i].c_str() );
    }

    // Establish new index size
    bool ok = false;
    new_size_of_index_ = ui->indexSize->text().toInt( &ok );
    if( ok ) {
        zkiresize_->resize( current_size_of_index_, new_size_of_index_ );
        zkiresize_->waitForFinishedResize();
    } else {
        MessagesI.AppendMessageT("<font color=red>Incorrect index size</font>");
    }

    git_->rename_lzcsde_to_lzcsde( lzcsde_renamed_from_to_ );
    git_->waitForFinishedRename();

    git_->remove_lzcsde( lzcsde_deleted_ );
    git_->waitForFinishedRemove();

    zkrewrite_->setInZekylls( current_zekylls.join("") );
    zkrewrite_->setOutZekylls( newer_zekylls.join("") );
    zkrewrite_->rewrite();
    zkrewrite_->waitForFinishedRewrite();

    reloadRepository();
}

void MainWindow::handle_zkrewrite( int exitCode, QStringList entries ) {
    if( exitCode == 0 ) {
        return;
    }

    QString error_decode = decode_zkrewrite_exit_code( exitCode );
    MessagesI.AppendMessageT( QString("[Exit code: %1] ").arg(exitCode) + error_decode );
}

void MainWindow::handle_git_rm( int exitCode, QStringList entries ) {
    if( exitCode == 0 ) {
        return;
    }
    MessagesI.AppendMessageT( QString("[Exit code: %1] ").arg(exitCode) + "<font color=red>Git ran with errors</font>", entries );
}

void MainWindow::handle_git_mv( int exitCode, QStringList entries ) {
    if( exitCode == 0 ) {
        return;
    }
    MessagesI.AppendMessageT( QString("[Exit code: %1] ").arg(exitCode) + "<font color=red>Git ran with errors</font>", entries );
}

void MainWindow::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    if( is_loading_ ) {
        return;
    }

    int row = item->row();
    int column = item->column();
    if( row == -1 || column == -1 ) {
        return;
    }

    QTableWidgetItem *id_item = ui->tableWidget->item(row, 0);
    QString id = id_item->text();
    if( column == 3 ) {
        QString new_section = item->text();
        lzcsde_list_.updateSectionOfId( id, new_section );
    } else if( column == 4 ) {
        QString new_description = item->text();
        lzcsde_list_.updateDescriptionOfId( id, new_description );
    }
}

void MainWindow::handle_zkiresize_resize( int exitCode, QStringList entries ) {
    if( exitCode == 0 ) {
        return;
    }

    QString error_decode = decode_zkiresize_exit_code(exitCode);
    if( error_decode != "" ) {
        MessagesI.AppendMessageT( tr("<font color=green>Message from the Zekyll backend (1):</font> ") + error_decode );
    }
}
