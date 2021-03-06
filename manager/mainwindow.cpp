/*
 Copyright 2016 Sebastian Gniazdowski

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "singleton.h"
#include "messages.h"
#include "math_functions.h"
#include "script_functions.h"
#include "coding_functions.h"
#include "external_functions.h"
#include "commitdialog.h"
#include "zmeditor.h"
#include "pulldialog.h"
#include "checkoutdialog.h"
#include "statusdialog.h"
#include "newbranchdialog.h"
#include "deletebranchtagdialog.h"

#include <QMap>
#include <QDir>
#include <QRegExp>
#include <QSettings>
#include <QFileDialog>
#include <QCheckBox>
#include <QScrollBar>
#include <QTableWidget>
#include <QDebug>

#define MessagesI Singleton<Messages>::instance()

static QDebug operator<<(QDebug out, const std::string & str)
{
    out << QString::fromStdString(str);
    return out;
}

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    isConsistent_(true),
    is_loading_(false),
    isDeferredApplyPrepared_(false),
    combo_box_reactions_limited_(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->messages->setReadOnly( true );

    readSettings();

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

    zkiresize_ = new ZkIResize( this );

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

    zkrewrite_ = new ZkRewrite( this );

    // Connect ZkRewrite
    QObject::connect(this->zkrewrite_,
                     SIGNAL(result(int, QStringList)),
                     this,
                     SLOT(handle_zkrewrite(int, QStringList)));

    git_ = new Git( this );
    lgit_ = new lgit( this );

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
    create_sites_maps();
    create_helper_maps();

    // Remember scroll bars' initial positions
    rememberScrollBars();

    // Prepare default repositories directory
    home_path_ = qgetenv("HOME");
    if( home_path_.size() < 1 ) {
        home_path_.setRawData("~", 1);
    }
    repos_paths_.push_back( QString::fromLocal8Bit(home_path_) + "/.zekyll/repos" );

    // Try to load default repository
    QByteArray default_repo = qgetenv( "ZEKYLL_DEFAULT_REPO" );
    if( default_repo.size() > 0 ) {
        ZEKYLL_DEFAULT_REPO_ = QString::fromLocal8Bit( default_repo );
        loadDefaultRepository();
    }

    lgit_->setOpTracker( &op_tracker_ );

    editAct = new QAction( tr("Edit zekyll value"), this);
    editAct->setStatusTip( tr("Edit zekyll value") );
    connect( editAct, &QAction::triggered, this, &MainWindow::edit_zekyll );
}

MainWindow::~MainWindow()
{
    delete editAct;
    delete lgit_;
    delete git_;
    delete zkrewrite_;
    delete zkiresize_;
    delete ui;
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
        // Expecting incorrect behavior, thus silent (i.e. the true)
        applyDeferredCodeSelectors( true );
        recomputeZcode();
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
            insertLZCSDTableRow( "list", ui->tableWidget, counter, rx.cap(1), true, rx.cap(2), rx.cap(3) );

            // Put this zekyll into SelectedZekylls data structure,
            // that maps zekylls to all IDs they occupy (with no
            // inconsistencies this will be single ID) and to
            // selection state
            // This structure holds one more information ??? by
            // containing given zekyll it denotes that the zekyll
            // was in listing
            std::string zekyll = rx.cap(1).toStdString();
            SelectedZekylls::iterator it = sel_zekylls_initial_.find( zekyll );
            if( it != sel_zekylls_initial_.end() ) {
                IDsVec & ids = it->second.first;
                ids.push_back( counter );
                it->second.second = true;
            } else {
                sel_zekylls_initial_[ zekyll ] = IDSelection( IDsVec( 1, counter ), true );
            }

            ZekyllIDs::iterator it2 = zekyllIDs_.find( zekyll );
            if( it2 != zekyllIDs_.end() ) {
                IDsVec & ids2 = it2->second;
                ids2.push_back( counter );
            } else {
                zekyllIDs_[ zekyll ] = IDsVec( 1, counter );
            }
        }
    }

    sel_zekylls_current_.clear();
    sel_zekylls_current_ = sel_zekylls_initial_;
    lzcsde_initial_.clear();
    lzcsde_initial_ = lzcsde_list_;
    lzcsde_section_.clear();
    lzcsde_section_ = lzcsde_list_;

    if( sectionOrderOnLZCSDE( lzcsde_section_ ) ) {
        MessagesI.AppendMessageT("Warning: Problems with data (16)");
    }
    foreach( const LZCSDE_Entry & entry, lzcsde_section_.entries() ) {
        insertLZCSDTableRow( "section", ui->tableWidget_2, entry.id(), entry.zekyll(), entry.checked(), entry.section(), entry.description() );
    }

    current_size_of_index_ = lzcsde_list_.count();
    this->ui->indexSize->setText( QString("%1").arg( current_size_of_index_ ) );

    if( entries.count() == 0 ) {
        MessagesI.AppendMessageT(tr("Index ") + QString("%1") . arg(current_index_) + tr(" is empty (go ahead and resize it), or selected path isn't a Zekyll repository"));
    }

    // This method creates lzcsde_list_ and lzcsde_consistent_,
    // thus two QScrollBar::setValue calls

    // RESTORE SCROLL of Zekyll order list
    ui->tableWidget->verticalScrollBar()->setMaximum( vscroll_bar_value_[0] * 2 );
    ui->tableWidget->verticalScrollBar()->setValue( vscroll_bar_value_.size() >= 1 ? vscroll_bar_value_[0] : 0 );
    // RESTORE SCROLL of Section order list
    ui->tableWidget_2->verticalScrollBar()->setMaximum( vscroll_bar_value_[1] * 2 );
    ui->tableWidget_2->verticalScrollBar()->setValue( vscroll_bar_value_.size() >= 2 ? vscroll_bar_value_[1] : 0 );

    // If exitCode is 12, there was inconsistent read
    // started, and whole listing actually stops there
    if( exitCode != 12 ) {
        is_loading_ = false;
        // Expecting correct behavior, thus not silent (i.e. the false)
        applyDeferredCodeSelectors( false );
        recomputeZcode();
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
        // Expecting correct behavior (list should went well), thus not silent (i.e. the false)
        applyDeferredCodeSelectors( false );
        recomputeZcode();
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
            insertLZSDETableRow( "consistent", ui->tableWidget_3, counter, rx1.cap(1), rx1.cap(2), rx1.cap(3), rx1.cap(4) );
        } else if (rx2.indexIn(str) != -1 ) {
            counter ++;
            lzcsde_consistent_.insertFromListing(counter, str);
            insertLZSDETableRow( "consistent", ui->tableWidget_3, counter, rx2.cap(1), "", "", rx2.cap(2) );
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
    // Expecting correct behavior, thus not silent (i.e. the false)
    applyDeferredCodeSelectors( false );
    recomputeZcode();

    // RESTORE SCROLL of inconsistent list
    // Single table created by this method -> single QScrollBar::setValue
    ui->tableWidget_3->verticalScrollBar()->setMaximum( vscroll_bar_value_[2] * 2 );
    ui->tableWidget_3->verticalScrollBar()->setValue( vscroll_bar_value_.size() >= 3 ? vscroll_bar_value_[2] : 0 );
}

void MainWindow::insertLZCSDTableRow(const QString & lzcsde, QTableWidget * tableWidget, int id, const QString & zekyll, bool checked, const QString & section, const QString & description) {
    QTableWidgetItem *idItem = new QTableWidgetItem(QString("%1").arg(id));
    idItem->setFlags(idItem->flags() ^ Qt::ItemIsEditable);
    idItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    QTableWidgetItem *zekyllItem = new QTableWidgetItem(zekyll);
    //zekyllItem->setFlags(zekyllItem->flags() ^ Qt::ItemIsEditable);
    zekyllItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    QTableWidgetItem *checkItem = new QTableWidgetItem(tr(""));
    checkItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    QCheckBox *checkBox = new QCheckBox();
    if( checked ) {
        checkBox->setCheckState(Qt::Checked);
    } else {
        checkBox->setCheckState(Qt::Unchecked);
    }
    checkBox->setProperty( "id", id );
    checkBox->setProperty( "zekyll", zekyll );
    if( lzcsde == "list" ) {
        checkBox->setProperty( "lzcsde", "list" );
    } else if( lzcsde == "section" ) {
        checkBox->setProperty( "lzcsde", "section" );
    } else {
        checkBox->setProperty( "lzcsde", "unknown" );
    }
    connect( checkBox, SIGNAL(clicked(bool)), this, SLOT(checkBoxClicked(bool)) );

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

void MainWindow::insertLZSDETableRow(const QString & lzsde, QTableWidget * tableWidget, int id, const QString & zekyll, const QString & section,
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

    Q_UNUSED( lzsde );
}

int MainWindow::applyDeferredCodeSelectors( bool silent )
{
    if( isDeferredApplyPrepared_ ) {
        isDeferredApplyPrepared_ = false;
        applyCodeSelectors( deferredCodeSelectors_, silent );
        deferredCodeSelectors_ = std::vector<int>();
    }

    return 0;
}

int MainWindow::setCheckedInTable( bool selected, QTableWidget *table, int row, bool silent )
{
    int retval = 0;

    QWidget *awidget = table->cellWidget( row, 2 );
    QWidget *widget = qobject_cast<QWidget*>( awidget );
    if(!widget) {
        retval += 161;
        if( !silent ) {
            MessagesI.AppendMessageT( "Warning: Problems with data (9)" );
        }
        return retval;
    }

    QLayout *layout_general = widget->layout();
    QHBoxLayout *layout = qobject_cast<QHBoxLayout *>( layout_general );
    if(!layout) {
        retval += 162;
        if( !silent ) {
            MessagesI.AppendMessageT( "Warning: Problems with data (10)" );
        }
        return retval;
    }

    int count = layout->count();
    for( int i = 0; i < count; i ++ ) {
        QLayoutItem *l_item = layout->itemAt( i );
        QCheckBox *checkBox = qobject_cast<QCheckBox*>( l_item->widget() );
        if( checkBox ) {
            if( selected ) {
                checkBox->setCheckState( Qt::Checked );
            } else {
                checkBox->setCheckState( Qt::Unchecked );
            }
        } else {
            retval += 164;
            if( !silent ) {
                MessagesI.AppendMessageT( "Warning: Problems with data (12)" );
            }
        }
    }

    return retval;
}

std::tuple<int,int> MainWindow::setCheckedViaTableInLZCSDE( bool selected, QTableWidget *table, LZCSDE & lzcsde, int row, bool silent )
{
    int retval = 0, retid = -1;
    QTableWidgetItem *id_item = table->item( row, 0 );
    if( id_item ) {
        QString id = id_item->text();
        bool ok = false;
        int intid = id.toInt( &ok );
        if( ok ) {
            int idx = lzcsde.findIdxOfId( intid );
            if( idx != -1 ) {
                lzcsde.entries()[idx].setChecked( selected );
                retid = intid;
            } else {
                retval += 173;
                if( !silent ) {
                    MessagesI.AppendMessageT( "Warning: Problems with data (21)" );
                }
            }
        } else {
            retval += 163;
            if( !silent ) {
                MessagesI.AppendMessageT( "Warning: Problems with data (11)" );
            }
        }
    } else {
        retval += 167;
        if( !silent ) {
            MessagesI.AppendMessageT( "Warning: Problems with data (20)" );
        }
    }

    return std::make_tuple( retval, retid );
}

bool MainWindow::errorOnDisallowedChars(const QString &type, const QStringList &invalidChars)
{
    if( !invalidChars.empty() ) {
        QString name = getNames()[ type ];
        if( name.count() > 0 ) {
            MessagesI.AppendMessageT("Invalid characters in " + name + ": <b>" + invalidChars.join("</b>, <b>") + "</b>, they are skipped");
        } else {
            MessagesI.AppendMessageT("Invalid characters used: <b>" + invalidChars.join("</b>, <b>") + "</b>, they are skipped");
        }
        return true;
    }
    return false;
}

// bool - if there is meaningful data, QString - the data, int - index extracted, QString - code extracted
std::tuple<bool, QString, int, QString> MainWindow::getProcessedZcodeInput()
{
    QString input = ui->zcode->text().trimmed();
    if( input == "" ) {
        input = QString( "%1/" ).arg( current_index_ );
        return std::make_tuple( false, input, current_index_, QString("") );
    }

    QStringList parts = input.split("/", QString::KeepEmptyParts );
    if( parts.count() != 2 ) {
        if( parts.count() == 1 ) {
            parts.prepend( QString( "%1" ).arg( current_index_ ) );
            input = parts.join( "/" );
        } else {
            MessagesI.AppendMessageT( "Warning: incorrect Zcode enterred" );
            int index;
            if( !to_int( parts.first(), &index ) ) {
                index = -1;
            } else {
                parts.pop_front();
            }
            return std::make_tuple( false, input, index, parts.join("/") );
        }
    } else {
        if( parts.first() == "" ) {
            parts.first() = QString( "%1" ).arg( current_index_ );
            input = parts.join( "/" );
        }

        if( parts.last() == "" ) {
            int index;
            if( !to_int( parts.first(), &index ) ) {
                index = -1;
            }
            return std::make_tuple( false, input, index, QString("") );
        }
    }

    int index;
    if( !to_int( parts.first(), &index ) ) {
        index = -1;
    } else {
        parts.pop_front();
    }
    return std::make_tuple( true, input, index, parts.join("/") );
}

std::tuple< std::vector<int>, int > MainWindow::gatherCodeSelectors()
{
    int retval = 0;
    if( int( lzcsde_list_.count() ) != ui->tableWidget->rowCount() ) {
        retval += 140;
        MessagesI.AppendMessageT( QString( "Warning: Problems with data (%1/%2)" ).arg( lzcsde_list_.count() ).arg( ui->tableWidget->rowCount() ) );
    }

    int _rows = ui->tableWidget->rowCount();
    unsigned int rows = (unsigned int) ( _rows >= 0 ? _rows : 0 );

    // First find maximal zekyll
    std::string max_zekyll;
    for( unsigned int row = 0; row < rows; row ++ ) {
        QTableWidgetItem *item = ui->tableWidget->item( row, 1 );
        if( !item ) {
            retval += 145;
            MessagesI.AppendMessageT( "Warning: Problems with data (26)" );
            continue;
        }
        QString qzekyll = item->text();
        std::string zekyll = qzekyll.toStdString();
        if( max_zekyll.size() == 0 || zekyll.compare(max_zekyll) > 0 ) {
            max_zekyll = zekyll;
        }
    }

    // Maps zekylls to their state
    SelectorsMap selectorsMap;

    for( unsigned int row = 0; row < rows; row ++ ) {
        QWidget *sel_widget = ui->tableWidget->cellWidget( row, 2 );
        QWidget *widget = qobject_cast<QWidget*>( sel_widget );
        if(!widget) {
            retval += 141;
            MessagesI.AppendMessageT( "Warning: Problems with data (2)" );
            continue;
        }

        QLayout *layout_general = widget->layout();
        QHBoxLayout *layout = qobject_cast<QHBoxLayout *>( layout_general );
        if(!layout) {
            retval += 142;
            MessagesI.AppendMessageT( "Warning: Problems with data (3)" );
            continue;
        }

        QTableWidgetItem *id_item = ui->tableWidget->item( row, 0 );

        int count = layout->count();
        for( int i = 0; i < count; i ++ ) {
            QLayoutItem *l_item = layout->itemAt( i );
            QCheckBox *checkBox = qobject_cast<QCheckBox*>( l_item->widget() );
            if( checkBox && id_item ) {
                QString id = id_item->text();
                bool ok = false;
                int intid = id.toInt( &ok );
                if( ok ) {
                    int idx = lzcsde_list_.findIdxOfId( intid );
                    bool selected1 = checkBox->checkState() == Qt::Checked;
                    bool selected2 = lzcsde_list_.entries()[idx].checked();

                    if( selected1 != selected2 ) {
                        lzcsde_list_.entries()[idx].setChecked( selected1 );
                        MessagesI.AppendMessageT( "Warning: Problems with data (4)" );
                    }

                    // Apply the state on selectorsMap, first finding zekyll
                    QTableWidgetItem *zekyll_item = ui->tableWidget->item( row, 1 );
                    if( !zekyll_item ) {
                        retval += 145;
                        MessagesI.AppendMessageT( "Warning: Problems with data (27)" );
                    } else {
                        QString qzekyll = zekyll_item->text().trimmed();
                        std::string zekyll = qzekyll.toStdString();
                        if( selectorsMap.count( zekyll ) > 0 ) {
                            MessagesI.AppendMessageT( QString( "Warning: duplicate zekyll found (" + qzekyll + ") in row %1" ) .arg( row + 1 ) +
                                                      ", it will overwrite preceding zekyll's Zcode data" );
                        }
                        selectorsMap[ zekyll ] = selected1;
                    }
                } else {
                    retval += 143;
                    MessagesI.AppendMessageT( "Warning: Problems with data (5)" );
                }
            } else {
                retval += 144;
                MessagesI.AppendMessageT( "Warning: Problems with data (6)" );
            }
        }
    }

    // Last stage ??? read selectorsMap in order of ZKL_INDEX_ZEKYLLS_
    vector<int> selectors;
    if( max_zekyll == "" ) {
        return std::make_tuple( selectors, retval );
    }

    unsigned int size = ZKL_INDEX_ZEKYLLS_.size();
    _rows = ui->tableWidget->rowCount();
    rows = (unsigned int) ( _rows >= 0 ? _rows : 0 );
    unsigned int row;
    unsigned int i;
    for( i = 0, row = 0; i < size && row < rows; i ++, row ++ ) {
        // Prepare common data: zekyll, table_zekyll, selectorsMap pointer (sub_it)
        // Also perform some assert-like checks
        const std::string & zekyll = ZKL_INDEX_ZEKYLLS_[ i ];
        QTableWidgetItem* item = ui->tableWidget->item( row, 1 );
        std::string table_zekyll;
        SelectorsMap::iterator sub_it;
        if( item ) {
            table_zekyll = item->text().trimmed().toStdString();
            sub_it = selectorsMap.find( table_zekyll );
            if( sub_it == selectorsMap.end() ) {
                retval += 146;
                MessagesI.AppendMessageT( "Warning: Problems with data (28)" );
            }
        } else {
            retval += 147;
            MessagesI.AppendMessageT( "Warning: Problems with data (29)" );
        }

        SelectorsMap::iterator it = selectorsMap.find( zekyll );
        bool selected = false;
        if( it != selectorsMap.end() ) {
            // Expected zekyll exists in system, thus
            // there is no hole and we should just
            // read current table row's check box
            // (selectorsMap has this value, filled
            // above, but doesn't support duplicate
            // zekylls ??? TODO) ??? by doing such read
            // we follow any future order (i.e. zekyll
            // assignment to entries) of index, behave
            // like if rewrite was already performed
            if( item ) {
                if( sub_it != selectorsMap.end() ) {
                    selected = sub_it->second;
                }
            }
        } else {
            // Has been the element removed?
            ZekyllIDs::iterator it2 = zekyllIDs_.find( zekyll );
            bool existed = false, was_removed = false;
            if( it2 != zekyllIDs_.end() && it2->second.size() != 0 ) {
                existed = true;
            }

            if( existed ) {
                const IDsVec & ids = it2->second;

                for( IDsVec::const_iterator it3 = ids.begin(); it3 != ids.end(); ++ it3 ) {
                    // Was the element at our current position?
                    if( *it3 == int(row+1) ) {
                        if( lzcsde_initial_.findIdxOfId( row+1 /* ID is defined as row number + 1 when LZCSDE is unmodified */ ) != -1 ) {
                            was_removed = true;
                            // The element has been removed, there
                            // is no hole an we should just read
                            // current table row's checkbox
                            if( item ) {
                                if( sub_it != selectorsMap.end() ) {
                                    selected = sub_it->second;
                                }
                            }
                        } else {
                            was_removed = false;
                            // Element doesn't exist and wasn't removed
                            // ??? this means inconsistency in index and
                            // we should increment i, but hold back with row
                            // until we encounter ZKL_INDEX_ZEKYLLS_ zekyll
                            // that exists in system
                        }
                        break;
                    }
                }

                if( existed && !was_removed ) {
                    retval += 148;
                    MessagesI.AppendMessageT( "Warning: problems with data (30)" );
                }

                if( !was_removed ) {
                    // There's a hole in zekyll listing, advance on
                    // ZKL_INDEX_ZEKYLLS_, but not on table
                    -- row;
                }
            } else {
                -- row;
            }
        }

        selectors.push_back( selected );

        if( max_zekyll == ZKL_INDEX_ZEKYLLS_[i] ) {
            break;
        }
    }

    return std::make_tuple( selectors, retval );
}

void MainWindow::on_actionQuit_triggered() {
    writeSettings();
    QApplication::quit();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    writeSettings();
    event->accept();
}

void MainWindow::browse()
{
    QString directory = QFileDialog::getExistingDirectory( this, tr("Select repository"), repos_paths_[0] );

    if (!directory.isEmpty()) {
        QString selected;
        int error;
        std::tie( selected, error ) = SetFromRepoOrPathGetSelection( directory, false /* isRepo */ );
        processCurRepoCombo( selected, error );

        if( error == MY_REPO_AND_PATH || error == MY_ONLY_PATH ) {
            emit repositoryChanged();
        }
    }
}

void MainWindow::processCurRepoCombo(const QString &selected, int error)
{
    // We, not the user, are changing the combo box
    limitComboBoxReactions( true );

    // Remove any previous "Incorrect path selected"
    if( error == MY_ONLY_PATH || error == MY_REPO_AND_PATH ) {
        int idx = ui->curRepoCombo->findText(msg_incorrect_);
        if( idx != -1) {
            ui->curRepoCombo->removeItem( idx );
        }
    }

    // Add item if new
    if (ui->curRepoCombo->findText(selected) == -1) {
        ui->curRepoCombo->addItem(selected);
    }

    ui->curRepoCombo->setCurrentIndex(ui->curRepoCombo->findText(selected));

    // We, not the user, are changing the combo box
    limitComboBoxReactions( false );
}

void MainWindow::reloadRepository() {
    // Maintain current code selectors
    if( !isDeferredApplyPrepared_ ) {
        std::vector<int> code_selectors;
        int error;
        std::tie( code_selectors, error ) = gatherCodeSelectors();
        if( code_selectors.size() > 0 ) {
            while( code_selectors.size() > 0 && code_selectors.back() == 0 ) {
                code_selectors.pop_back();
            }
            std::reverse( code_selectors.begin(), code_selectors.end() );
            setupDeferredApplyOfCodeSelectors( code_selectors );
        }
    }

    is_loading_ = true;

    // Remember scroll bars positions
    rememberScrollBars();

    ui->tableWidget->setRowCount(0);
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_3->setRowCount(0);

    lzcsde_list_.clear();
    lzcsde_initial_.clear();
    lzcsde_consistent_.clear();
    lzcsde_renamed_from_to_.first.clear();
    lzcsde_renamed_from_to_.second.clear();
    lzcsde_deleted_.clear();
    sel_zekylls_initial_.clear();
    sel_zekylls_current_.clear();
    zekyllIDs_.clear();

    int error;
    std::tie( ZKL_INDEX_ZEKYLLS_, error ) = setIndex( current_index_ );
    if( error != 0 ) {
        MessagesI.AppendMessageT( QString( "Warning: index generation caused errors: %1" ).arg( error ) );
    }

    lgit_->setRepoPath( current_path_ );

    git_->setRepoPath( current_path_ );

    zkrewrite_->setRepoPath( current_path_ );

    zkiresize_->setRepoPath( current_path_ );
    zkiresize_->setIndex( current_index_ );
    zkiresize_->list();
}

void MainWindow::checkBoxClicked( bool checked )
{
    QCheckBox * box = qobject_cast< QCheckBox * >( sender() );
    if( !box ) {
        MessagesI.AppendMessageT("Warning: Problems with data (13)");
        return;
    }

    // Reflect the toggle in SelectedZekylls structure
    std::string zekyll = box->property("zekyll").toString().toStdString();
    sel_zekylls_current_[ zekyll ].second = checked;

    // Reflect the toggle in LZCSDE structure
    bool ok = false;
    int id = box->property("id").toInt( &ok );
    if( !ok ) {
        MessagesI.AppendMessageT("Warning: Problems with data (14)");
        return;
    }

    LZCSDE_Entry & entry = lzcsde_list_.getId( id );
    if( entry.id() == -1 ) {
        MessagesI.AppendMessageT("Warning: Problems with data (15)");
        return;
    }

    entry.setChecked( checked );

    //
    // Now reflect the toggle also on accompanying checkBox
    //

    QTableWidget *table;
    if( box->property("lzcsde") == "list" ) {
        table = ui->tableWidget_2;
    } else if( box->property("lzcsde") == "section" ) {
        table = ui->tableWidget;
    } else {
        recomputeZcode();
        return;
    }

    QString strId = box->property("id").toString();
    int rows = table->rowCount();
    for( int row = 0; row < rows; row ++ ) {
        QTableWidgetItem *item = table->item( row, 0 );
        if( !item || item->text() != strId ) {
            continue;
        }

        setCheckedInTable( checked, table, row, false );

        // Single find is the correct situation
        break;
    }

    recomputeZcode();
}

 void MainWindow::updateMessages( const QStringList & messages ) {
     ui->messages->clear();
     int counter = messages.count();
     QString str_prev, str_accum;
     bool first = true;
     QStringList::const_iterator it = messages.constEnd();
     while( it != messages.constBegin() ) {
         -- it;

         QString str = *it;
         str.replace( QRegExp("[\r\n]+"), QString("<br/>") );

         if( first ) {
             first = false;
             str_prev = str;
             str_accum = str;
         } else {
             int distance = levenshtein( str.toUtf8().constData(), str_prev.toUtf8().constData(), 1, 1, 1, 1 );
             if( distance > 5 ) {
                 QString msg = QString("<font color=magenta>{<b>%1</b>}</font> ").arg( counter + 1 ) + str_accum;
                 ui->messages->appendHtml( msg );
                 str_accum = str;
             } else {
                 str_accum += "<font color=red><b> ???</b></font>" + str;
             }

             str_prev = str;
         }

         -- counter;
     }
     ui->messages->appendHtml( QString("<font color=magenta>{<b>1</b>}</font> ") + str_accum );
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

            // Zekyll changed? A special case, needed for duplicate resolution
            if( initial.zekyll() != current.zekyll() ) {
                changed = 1;
            }

            // Section changed?
            if( initial.section() != current.section() ) {
                changed += 2;
            }

            // Description changed?
            if( initial.description() != current.description() ) {
                changed += 4;
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

void MainWindow::on_tableWidget_itemChanged( QTableWidgetItem *item )
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
    if( column == 1 ) {
        QString new_zekyll = item->text();
        lzcsde_list_.updateZekyllOfId( id, new_zekyll );
    } else if( column == 3 ) {
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
        MessagesI.AppendMessageT( tr("<font color=green>Message from Zekyll backend (1):</font> ") + error_decode );
    }
}

void MainWindow::on_zcode_editingFinished()
{
    bool has_correct_data;
    QString input, code;
    int index;
    std::tie( has_correct_data, input, index, code ) = getProcessedZcodeInput();

    ui->zcode->setText( input );

    if( !has_correct_data ) {
        return;
    }

    vector<int> bits;
    int error;
    tie( bits, error ) = decode_zcode( input.toStdString() );
    if( error != 0 ) {
        MessagesI.AppendMessageT( QString("<font color=red>Error %1 during decoding the entered Zcode</font>").arg( error ) );
    }

    int to_skip;
    QMap< QString, QString > decoded;
    tie( to_skip, decoded, error ) = process_meta_data( bits );

    this->ui->file->setText( decoded["file"] );
    this->ui->rev->setText( decoded["rev"] );
    this->ui->userRepo->setText( decoded["repo"] );
    if( decoded["site"] == "" ) {
        this->ui->site->setCurrentIndex( 0 );
    } else {
        QString strIdx = getSites()[ decoded["site"] ];
        if( strIdx == "" ) {
            MessagesI.AppendMessageT( "Warning: decoded incorrect site, falling back to GH" );
            this->ui->site->setCurrentIndex( 0 );
        } else {
            bool ok = false;
            int idx = strIdx.toInt( &ok );
            if( !ok ) {
                MessagesI.AppendMessageT( "Warning: decoded incorrect site, falling back to GH" );
                this->ui->site->setCurrentIndex( 0 );
            }
            this->ui->site->setCurrentIndex( idx - 1 );
        }
    }

    if( to_skip >= 0 ) {
        if( ((unsigned int) to_skip) > bits.size() ) {
            MessagesI.AppendMessageT("Warning: Problems with data (7)");
        } else {
            // Skip computed number of bits
            bits.erase( bits.end() - to_skip, bits.end() );
        }
    }

    if( 2 > bits.size() ) {
        MessagesI.AppendMessageT("Warning: Problems with data (8)");
    } else {
        // Two last bits are version
        bits.erase( bits.end() - 2, bits.end() );
    }

    if( index != current_index_ ) {
        current_index_ = index;
        setupDeferredApplyOfCodeSelectors( bits );
        reloadRepository();
    } else {
        applyCodeSelectors( bits );
    }
}

int MainWindow::applyCodeSelectors( const std::vector<int> & bits_, bool silent ) {
    // If there is no code then limit operation
    if( ui->zcode->text().trimmed().count() == 0 ) {
        silent = true;
    }

    // Have upper zekylls first
    std::vector<int> bits = bits_;
    std::reverse( bits.begin(), bits.end() );

    bool recompute_zcode = false;

    int retval = 0;
    if( lzcsde_list_.count() < bits.size() ) {
        retval += 160;
        if( !silent ) {
            recompute_zcode = true;
        }
    }

    // Will rebuild selectedZekylls from bits_
    // and ZKL_INDEX_ZEKYLLS
    sel_zekylls_current_.clear();

    bool selected;

    unsigned int size = ZKL_INDEX_ZEKYLLS_.size();
    for( unsigned int row = 0; row < size; row ++ ) {
        if( bits.size() < row + 1 ) {
            selected  = false;
        } else {
            if( bits[ row ] == 0 ) {
                selected = false;
            } else {
                selected = true;
            }
        }

        std::string zekyll = ZKL_INDEX_ZEKYLLS_[ row ];
        IDsVec ids;
        ZekyllIDs::iterator it = zekyllIDs_.find( zekyll );
        if( it != zekyllIDs_.end() ) {
            ids = it->second;
        }
        sel_zekylls_current_[ zekyll ] = IDSelection( ids, selected );
    }

    int _rows = ui->tableWidget->rowCount();
    unsigned int rows = (unsigned int) ( _rows >= 0 ? _rows : 0 );
    for( unsigned int row = 0; row < rows; row ++ ) {
        // Establish zekyll
        QTableWidgetItem *item = ui->tableWidget->item( row, 1 );
        if( item ) {
            QString qzekyll = item->text();
            std::string zekyll = qzekyll.toStdString();
            SelectedZekylls::iterator it = sel_zekylls_current_.find( zekyll );
            if( it != sel_zekylls_current_.end() ) {
                selected = it->second.second;
            } else {
                selected = false;
                MessagesI.AppendMessageT( QString( "Warning: Problems with data (24/" + qzekyll + ")" ) );
            }
        } else {
            selected = false;
            MessagesI.AppendMessageT( QString( "Warning: Problems with data (25/%1)" ).arg( row ) );
        }

        // Set CheckBox
        int retval2 = setCheckedInTable( selected, ui->tableWidget, row, silent );
        retval += retval2;

        // Set LZCSDE entry
        if( retval2 == 0 ) {
            int id;
            std::tie( retval2, id ) = setCheckedViaTableInLZCSDE( selected, ui->tableWidget, lzcsde_list_, row, silent );
            retval += retval2;

            // Set CheckBox in section order list
            if( retval2 == 0 && id != -1 ) {
                QString strId = QString( "%1" ).arg( id );
                int rows2 = ui->tableWidget_2->rowCount();
                for( int row2 = 0; row2 < rows2; row2 ++ ) {
                    QTableWidgetItem *item = ui->tableWidget_2->item( row2, 0 );
                    if( !item || item->text() != strId ) {
                        continue;
                    }

                    // Found the ID, meaning row is also known
                    retval2 = setCheckedInTable( selected, ui->tableWidget_2, row2, silent );
                    retval += retval2 ? retval2 + 2110000 : 0;
                    if( retval2 != 0 ) {
                        MessagesI.AppendMessageT( QString( "Warning: Problems with data (23/%1)" ).arg( retval2 ) );
                    }
                    break;
                }
            } else {
                retval += 179;
                if( !silent ) {
                    MessagesI.AppendMessageT( "Warning: Problems with data (22)" );
                }
            }
        }
    }

    if( recompute_zcode ) {
        if(!recomputeZcode()) {
            retval += 2230000;
        }
        MessagesI.AppendMessageT(
                    QString( "Warning: Zcode is for index of size at least <b>%1</b> (current size: <b>%2</b>) ??? truncated the Zcode: ")
                    .arg( bits.size() ).arg( lzcsde_list_.count() ) + "<u><b>" + ui->zcode->text().trimmed() + "</b></u>"
        );
    }

    return retval;
}

bool MainWindow::recomputeZcode()
{
    vector<int> bits;

    // Version
    bits.push_back( 0 );
    bits.push_back( 0 );

    vector<int> selectors;
    int error;
    std::tie( selectors, error ) = gatherCodeSelectors();
    if( error ) {
        MessagesI.AppendMessageT("Warning: Gathering data for Zcode wasn't without problems");
    }

    // The selectors of zekylls
    // Reverse them, so that selectors for later zekylls
    // will be simply skipped if they are zero ??? when the
    // bits sequence will be divided by 36 when converting
    // to radix-36
    bits.insert( bits.end(), selectors.begin(), selectors.end() );
    std::reverse( bits.begin(), bits.end() );

    std::vector<int> appendix;
    int newerror;
    QStringList invalidChars;

    // Meta-data start
    error += BitsStart( appendix );

    // Revision
    std::tie( newerror, invalidChars ) = BitsWithPreamble( appendix, "rev",  ui->rev->text() );
    error += newerror;
    errorOnDisallowedChars( "rev", invalidChars );

    // File name
    std::tie( newerror, invalidChars ) = BitsWithPreamble( appendix, "file", ui->file->text() );
    error += newerror;
    errorOnDisallowedChars( "file", invalidChars );

    // User/repo
    std::tie( newerror, invalidChars ) = BitsWithPreamble( appendix, "repo", ui->userRepo->text() );
    error += newerror;
    errorOnDisallowedChars( "repo", invalidChars );

    // Site
    int siteId = ui->site->currentIndex() + 1;
    // Github is the default
    if( siteId != 1 ) {
        QString strSiteId = QString("%1").arg( siteId );
        std::tie( newerror, invalidChars ) = BitsWithPreamble( appendix, "site", strSiteId );
        error += newerror;
        errorOnDisallowedChars( "site", invalidChars );
    }

    // Meta-data end
    error += BitsStop( appendix );

    // Empty meta-data?
    error += BitsRemoveIfStartStop( appendix );

    if( appendix.size() == 0 ) {
        QString str_bits = getCodes()[ "ss" ].trimmed();
        QString rev_str_bits = reverseQString( str_bits );

        // This cannot happen with current version bits
        bool result;
        std::tie( result, newerror ) = BitsCompareSuffix( bits, rev_str_bits );
        error += newerror;
        if( result ) {
            // No metadata but 'ss' at end ??? add another one to
            // mark that the 'ss' is a real data
            error += insertBitsFromStrBits( bits, rev_str_bits );
        }
    } else {
        // Append meta-data bits
        // They go to end, and are reversed, so to decode, one can
        // first reverse whole sequence, to have meta-data waiting
        // in the beginning, in order
        std::reverse( appendix.begin(), appendix.end() );
        bits.insert( bits.end(), appendix.begin(), appendix.end() );
    }

    // Create Zcode
    std::vector<char> zcode;
    std::vector<int> numbers;
    int error2;
    std::tie( zcode, numbers, error2 ) = encode_zcode_arr01( bits );
    error += error2;

    // Convert Zcode to QString
    std::string zcode2( zcode.begin(), zcode.end() );
    QString zcode3 = QString::fromStdString( zcode2 );

    ui->zcode->setText( QString( "%1/" ).arg( current_index_ ) + zcode3 );

    if( error ) {
        int exam = error - 1630000;
        // Display if there is other error besides use of invalid characters
        if( exam % 163 ) {
            MessagesI.AppendMessageT( QString("Warning: Computation ended with code %1").arg( error ) );
        } else {
            MessagesI.AppendMessageT( "Allowed characters are: <b>a-z</b>, <b>A-Z</b>, <b>0-9</b>, <b>/</b>, <b>~</b>, <b>-</b>, <b>_</b>, <b>.</b>, <b>space</b>" );
        }

        return false;
    } else {
        return true;
    }
}

MainWindow* MainWindow::ptr()
{
    MainWindow *mainWindow;
    bool found = false;
    foreach(QWidget *widget, QApplication::topLevelWidgets()) {
      if( widget->objectName() == "MainWindow" ) {
          mainWindow = qobject_cast< MainWindow * >( widget );
          if( mainWindow ) {
              found = true;
              break;
          }
      }
    }

    if( found ) {
        return mainWindow;
    } else {
        return 0;
    }
}

// Sets current_repo_ and current_path_ from repo string or path.
// Returns: repo string or path. Repo string when input was repo
// or if given path pointed to well formated directory name. Path
// when input was path with not well formated directory name. In
// both cases the pointed directory must exist, otherwise
// MY_GENERAL_ERROR is returned
std::tuple<QString, int> MainWindow::SetFromRepoOrPathGetSelection( QString repoOrPath, bool isRepo ) {
    QString repo, path, selected;
    int error;
    if( isRepo ) {
        tie( repo, path, error ) = getPathFromRepo( repos_paths_[0], repoOrPath );
    } else {
        tie( repo, path, error ) = getRepoFromPath( repoOrPath );
    }

    // Error 0 ??? we've gotten repo, and decoded a path ??? all as expected
    if( error == MY_REPO_AND_PATH ) {
        current_repo_ = repo;
        current_path_ = path;
        selected = repo;
        // Error 2 ??? things are little unexpected, we've directly gotten path
    } else if ( error == MY_ONLY_PATH ) {
        current_repo_ = "";
        current_path_ = path;
        selected = path;
        // Error 1 ??? true error, incorrect selection
    } else if ( error == MY_GENERAL_ERROR ) {
        selected = msg_incorrect_;
    }

    return std::make_tuple( selected, error );
}

std::tuple<QString, int> MainWindow::loadDefaultRepository()
{
    if( ZEKYLL_DEFAULT_REPO_.count() == 0 ) {
        MessagesI.AppendMessageT( "Environment variable $ZEKYLL_DEFAULT_REPO is not set. You can set it to repo string." );
        return std::make_tuple( QString(""), MY_GENERAL_ERROR );
    }

    QString selected;
    int error;
    std::tie( selected, error ) = SetFromRepoOrPathGetSelection( ZEKYLL_DEFAULT_REPO_, true /* isRepo */ );
    if( error == MY_REPO_AND_PATH || error == MY_ONLY_PATH ) {
        // Setup combo box only when a path is correct
        processCurRepoCombo( selected, error );
        MessagesI.AppendMessageT( "Loading default repository ($ZEKYLL_DEFAULT_REPO): <b>" + ZEKYLL_DEFAULT_REPO_ + "</b>" );
        reloadRepository();
    } else {
        MessagesI.AppendMessageT( "Default repository doesn't exist ($ZEKYLL_DEFAULT_REPO is \"<b>" + ZEKYLL_DEFAULT_REPO_ + "</b>\")" );
    }

    return std::make_tuple( selected, error );
}

std::tuple<bool, int> MainWindow::stepIntegerQLineEdit( QLineEdit *lineEdit, int min, int max, const QString &msg, bool subtract )
{
    int integer = 0, integer_initial = 0;
    QString str_integer = lineEdit->text().trimmed();
    if( to_int( str_integer, &integer, &integer_initial ) ) {
        integer = subtract? integer -1 : integer + 1;
        integer = integer < min ? min : integer;
        integer = integer > max ? max : integer;
        if( integer != integer_initial ) {
            lineEdit->setText( QString("%1").arg( integer ) );
            return std::make_tuple( true, integer );
        }
    } else {
        MessagesI.AppendMessageT( msg );
        QRegExp rx( "[0-9]+" );
        if( rx.indexIn( str_integer ) != -1 ) {
            lineEdit->setText( rx.cap(0) );
        }
    }

    return std::make_tuple( false, integer );
}

int MainWindow::rememberScrollBars()
{
    vscroll_bar_value_.clear();
    vscroll_bar_value_.push_back( ui->tableWidget->verticalScrollBar()->value() );
    vscroll_bar_value_.push_back( ui->tableWidget_2->verticalScrollBar()->value() );
    vscroll_bar_value_.push_back( ui->tableWidget_3->verticalScrollBar()->value() );

    return 0;
}

bool MainWindow::setCurrentIndexInZcode()
{
    std::tuple<bool, QString, int, QString> result = getProcessedZcodeInput();

    QStringList with_index( std::get<3>( result ) );
    with_index.prepend( QString( "%1" ).arg( current_index_ )  );
    ui->zcode->setText( with_index.join( "/" ) );
    return true;
}

void MainWindow::writeSettings()
{
    QSettings settings( "qtzekyllmgr.ini", QSettings::IniFormat );

    settings.beginGroup( "Main" );
    settings.setValue( "size", size() );
    settings.setValue( "pos", pos() );
    settings.endGroup();
    settings.sync();

    qDebug() << "Wrote settings INI (" << settings.fileName() << ")";
}

void MainWindow::readSettings()
{
    QSettings settings( "qtzekyllmgr.ini", QSettings::IniFormat );

    settings.beginGroup("Main");
    resize( settings.value( "size", QSize(800, 700) ).toSize() );
    move( settings.value( "pos", QPoint(400, 170) ).toPoint() );
    settings.endGroup();
}

void MainWindow::on_rev_editingFinished()
{
    recomputeZcode();
}

void MainWindow::on_file_editingFinished()
{
    recomputeZcode();
}

void MainWindow::on_userRepo_editingFinished()
{
    recomputeZcode();
}

void MainWindow::on_site_currentIndexChanged(int index)
{
    Q_UNUSED( index );
    recomputeZcode();
}

void MainWindow::on_curRepoCombo_activated(int index)
{
    // This will not happen (would for currentIndexChanged)
    if( combo_box_reactions_limited_ ) {
        return;
    }

    QString selected = ui->curRepoCombo->itemText( index );
    int error;
    std::tie( selected, error ) = SetFromRepoOrPathGetSelection( selected, true /* isRepo */ );
    processCurRepoCombo( selected, error );

    if( error == MY_REPO_AND_PATH || error == MY_ONLY_PATH ) {
        emit repositoryChanged();
    }
}

void MainWindow::on_zcode_returnPressed()
{
    ui->zcode->blockSignals(true);
    ui->zcode->clearFocus();
    ui->zcode->blockSignals(false);
}

void MainWindow::on_rev_returnPressed()
{
    ui->rev->blockSignals(true);
    ui->rev->clearFocus();
    ui->rev->blockSignals(false);
}

void MainWindow::on_file_returnPressed()
{
    ui->file->blockSignals(true);
    ui->file->clearFocus();
    ui->file->blockSignals(false);
}

void MainWindow::on_userRepo_returnPressed()
{
    ui->userRepo->blockSignals(true);
    ui->userRepo->clearFocus();
    ui->userRepo->blockSignals(false);
}

void MainWindow::on_indexSize_returnPressed()
{
    ui->indexSize->blockSignals(true);
    ui->indexSize->clearFocus();
    ui->indexSize->blockSignals(false);
}

void MainWindow::on_currentIndex_returnPressed()
{
    ui->currentIndex->blockSignals(true);
    ui->currentIndex->clearFocus();
    ui->currentIndex->blockSignals(false);
}

void MainWindow::on_defaultRepoButton_clicked()
{
    loadDefaultRepository();
}

void MainWindow::on_currentIndex_editingFinished()
{
    int index;
    QString str_index = ui->currentIndex->text().trimmed();
    if( to_int( str_index, &index ) ) {
        current_index_ = index;
        reloadRepository();
    } else {
        MessagesI.AppendMessageT( "Enterred index number incorrect");
        QRegExp rx( "[0-9]+" );
        if( rx.indexIn( str_index ) != -1 ) {
            ui->currentIndex->setText( rx.cap(0) );
        }
    }
}

void MainWindow::on_sizeLeft_clicked()
{
    stepIntegerQLineEdit( ui->indexSize, 0, 150, "Incorrect index size", true /* subtract */ );
}

void MainWindow::on_sizeRight_clicked()
{
    stepIntegerQLineEdit( ui->indexSize, 0, 150, "Incorrect index size" );
}

void MainWindow::on_indexLeft_clicked()
{
    std::tuple<bool, int> result = stepIntegerQLineEdit( ui->currentIndex, 1, 311, "Incorrect current index", true /* subtract */ );
    if( std::get<0>( result ) ) {
        current_index_ = std::get<1>( result );
        setCurrentIndexInZcode();
        reloadRepository();
    }
}

void MainWindow::on_indexRight_clicked()
{
    std::tuple<bool, int> result = stepIntegerQLineEdit( ui->currentIndex, 1, 311, "Incorrect current index" );
    if( std::get<0>( result ) ) {
        current_index_ = std::get<1>( result );
        setCurrentIndexInZcode();
        reloadRepository();
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch( index ) {
    case 0:
        ui->up->setEnabled( true );
        ui->down->setEnabled( true );
        break;
    case 1:
    case 2:
        ui->up->setEnabled( false );
        ui->down->setEnabled( false );
        break;
    default:
        MessagesI.AppendMessageT( "Warning: Qt internal error (1)" );
        break;
    }
}

void MainWindow::on_gitReset_clicked()
{
    int error = lgit_->hardReset();
    if( !error ) {
        MessagesI.AppendMessageT( QString("Successfully performed hard reset on repository") + " <b>" + current_repo_ + "</b>" );
        reloadRepository();
    } else {
        MessagesI.AppendMessageT( QString("Exit code of git reset: %1").arg( error ) );
    }
}

void MainWindow::on_gitCommit_clicked()
{
    CommitDialog *dialog = new CommitDialog(this);
    if(!dialog) {
        MessagesI.AppendMessageT( "Serious error: new failed (C++)" );
        return;
    }

    std::tuple< QString, QString, git_time, int > result = lgit_->testDefaultSignature();
    int error = std::get<3>( result );
    if( error > 0 ) {
        int git_error = ( error % 1000000 ) / 10000 * -1;
        if( git_error == GIT_ENOTFOUND ) {
            if( error / 1000000 == 23 ) {
                MessagesI.AppendMessageT( "Error: path does not point to Git repository" );
                delete dialog;
                return;
            } else {
                MessagesI.AppendMessageT( "user.name and user.email are not set in a gitconfig file, please enter the values in commit window" );
            }
        } else {
            if( error / 1000000 == 23 ) {
                MessagesI.AppendMessageT( QString( "Error: could not open provided repository, error code: %1" ).arg( error ) );
                delete dialog;
                return;
            } else {
                MessagesI.AppendMessageT( QString( "Could not get default signature (author and commiter), error code: %1" ).arg( error ) );
                MessagesI.AppendMessageT( "Please provide name and email in commit window" );
            }
        }
    } else {
        if( lgit_->backendStatus() == INITIALIZED ) {
            dialog->setName( std::get<0>( result ) );
            dialog->setEmail( std::get<1>( result ) );
        } else {
            MessagesI.AppendMessageT( "Could not initialize Git backend" );
            delete dialog;
            return;
        }
    }

    std::vector< std::string > merge_heads;
    if ( ( error = lgit_->listMergeHeads( merge_heads ) ) < 0 ) {
        MessagesI.AppendMessageT( "Could not read merge state" );
    } else if( merge_heads.size() > 0 ) {
        std::string head = merge_heads.front();
        if ( ( error = lgit_->loadBranches( BRANCH_ALL ) ) < 0 ) {
            MessagesI.AppendMessageT( "Could not load branch list" );
        } else {
            const mybranch & fhbranch = lgit_->branches().findFetchHeadSha( head.c_str() );
            if( fhbranch.invalid != INVALID_DUMMY ) {
                QString msg = "Merge branch '";
                msg += QString::fromStdString( fhbranch.name ) + "' of ";
                msg += QString::fromStdString( fhbranch.fetch_head_remote_url );

                dialog->setCommitMessage( msg );
            }
        }
    }

    if( dialog->exec() == QDialog::Rejected ) {
        MessagesI.AppendMessageT( "<font color=green>Commit has been stopped</font>" );
        delete dialog;
        return;
    }

    if ( dialog->name().count() > 0 ) {
        lgit_->setName( dialog->name() );
    }
    if ( dialog->email().count() > 0 ) {
        lgit_->setEmail( dialog->email() );
    }

    QString message = dialog->commitMessage();
    delete dialog;

    error = lgit_->commit( message );

    if( error == 0 ) {
        MessagesI.AppendMessageT( QString("Successfully performed git commit on repository") + " <b>" + current_repo_ + "</b>" );
        reloadRepository();
    } else {
        MessagesI.AppendMessageT( QString("Exit code of git commit: %1").arg( error ) );
    }
}

void MainWindow::on_gitPull_clicked()
{
    int error;

    PullDialog *pullDialog = new PullDialog( this );
    pullDialog->setLGit( lgit_ );
    op_tracker_.setPullDialog( pullDialog );
    error = pullDialog->reset();
    pullDialog->exec();
    delete pullDialog;
}

void MainWindow::edit_zekyll()
{
    if(ui->tabWidget->currentIndex() == 0) {
        if( ui->tableWidget->selectedItems().count() > 0)
        {
            int row = ui->tableWidget->currentRow();
            QModelIndex index = ui->tableWidget->model()->index( row, 1 );
            ui->tableWidget->edit( index );
        }
    }
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent( QContextMenuEvent *event )
{
    if ( ui->tabWidget->currentIndex() != 0 ) {
        return;
    }

    if ( ui->tableWidget->rect().contains( ui->tableWidget->mapFromGlobal( event->globalPos() ) ) ) {
        QMenu menu( this );
        menu.addAction( editAct );
        menu.exec( event->globalPos() );
    }
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::on_gitCheckout_clicked()
{
    CheckoutDialog *dialog = new CheckoutDialog( this );
    if( !dialog ) {
        MessagesI.AppendMessageT( "Internal error occured (C++ new failed)" );
        return;
    }

    op_tracker_.setCheckoutDialog( dialog );

    dialog->setLGit( lgit_ );
    lgit_->loadBranches( BRANCH_LOCAL );
    lgit_->loadTags();
    lgit_->establishCurrent();

    switch( lgit_->current().type() ) {
    case CURRENT_TYPE_BRANCH:
        dialog->setCurrent( QString::fromStdString( lgit_->current().branch() ) );
        break;
    case CURRENT_TYPE_TAG:
        dialog->setCurrent( QString::fromStdString( lgit_->current().tag() ) );
        break;
    case CURRENT_TYPE_OID:
        dialog->setCurrent( QString::fromStdString( lgit_->current().oid() ).left( 7 ) );
        break;
    default:
        break;
    }

    for ( std::vector< mybranch >::const_iterator it = lgit_->raw_branches().begin(); it != lgit_->raw_branches().end(); ++ it ) {
        if( it->is_in_fetch_head ) {
            continue;
        }
        dialog->addBranch( QString::fromStdString( it->name ), QString::fromStdString( it->tip_sha ) );
    }

    for ( std::vector< mytag >::const_iterator it = lgit_->raw_tags().begin(); it != lgit_->raw_tags().end(); ++ it ) {
        dialog->addTag( QString::fromStdString( it->name ), QString::fromStdString( it->targetSha ) );
    }

    dialog->exec();

    delete dialog;
}

void MainWindow::on_gitStatus_clicked()
{
    StatusDialog *sdialog = new StatusDialog( this );
    lgit_->doStatus();
    sdialog->setText( QString::fromStdString( lgit_->status_summary() ) );
    sdialog->exec();
    sdialog->deleteLater();
}

void MainWindow::on_git2NewBranch_clicked()
{
    int error;

    NewBranchDialog *nbdialog = new NewBranchDialog( this );
    nbdialog->setLGit( lgit_ );
    if ( nbdialog->exec() == QDialog::Accepted ) {
        if ( !nbdialog->isUnclear() ) {
            if ( nbdialog->useHead() ) {
                error = lgit_->createBranch( nbdialog->branchName().toStdString() );
            } else {
                error = lgit_->createBranch( nbdialog->branchName().toStdString(), nbdialog->commitSha().toStdString() );
            }

            if ( 0 == error ) {
                MessagesI.AppendMessageT( tr( "Successfully created branch <b>" ) + nbdialog->branchName() + "</b>" );
            }

            if ( nbdialog->doCheckout() ) {
                if ( 0 == error ) {
                    // Refresh branches list
                    lgit_->loadBranches( BRANCH_LOCAL );

                    // Invoke checkout
                    if ( ( error = lgit_->checkout( nbdialog->branchName().toStdString(), "", true, false ) ) > 0 ) {
                        MessagesI.AppendMessageT( QString( "Checkout of newly created branch <b>" ) + nbdialog->branchName() +
                                                  QString( "</b> failed (exit code: %1)" ) . arg( error ) );
                    } else {
                        MessagesI.AppendMessageT( QString( "Successful checkout of newly created branch <b>" ) + nbdialog->branchName() + "</b>");
                    }
                } else {
                    MessagesI.AppendMessageT( "Not doing checkout, branch creation not successful" );
                }
            }

        }
    }
    nbdialog->deleteLater();
}

void MainWindow::on_git2DeleteBranch_clicked()
{
    int error;

    DeleteBranchTagDialog *dbtdialog = new DeleteBranchTagDialog( this );

    lgit_->loadBranches( BRANCH_LOCAL );

    for ( std::vector< mybranch >::const_iterator it = lgit_->raw_branches().begin(); it != lgit_->raw_branches().end(); ++ it ) {
        if ( it->is_in_fetch_head ) {
            continue;
        }
        if ( it->name == "master" ) {
            continue;
        }
        dbtdialog->addBranch( QString::fromStdString( it->name ), QString::fromStdString( it->tip_sha ) );
    }

    dbtdialog->endAdding();

    if ( dbtdialog->exec() == QDialog::Accepted ) {
        QString selectedBranch = dbtdialog->selectedRef().trimmed();
        if ( !selectedBranch.isEmpty() && selectedBranch != tr( "No branches or tags in repository" ) ) {
            error = lgit_->deleteBranch( selectedBranch.toStdString(), true );
            if ( 0 == error ) {
                MessagesI.AppendMessageT( tr( "Successful deletion of branch <b>" ) + selectedBranch + "</b>" );
                if ( ( error = lgit_->loadBranches( BRANCH_LOCAL ) ) > 0 ) {
                    MessagesI.AppendMessageT( tr( "Could not reload branches, error code: %1" ).arg( error ) );
                }
            } else {
                MessagesI.AppendMessageT( tr( "Cannot delete branch <b>%1</b>, error code: %2").arg( selectedBranch ).arg( error ) );
            }
        }
    } else {
        MessagesI.AppendMessageT( tr( "<font color='green'>Branch deletion aborted</font>" ) );
    }

    dbtdialog->deleteLater();
}

void MainWindow::on_git2DeleteTag_clicked()
{
    int error;

    DeleteBranchTagDialog *dbtdialog = new DeleteBranchTagDialog( this );

    lgit_->loadTags();

    for ( std::vector< mytag >::const_iterator it = lgit_->raw_tags().begin(); it != lgit_->raw_tags().end(); ++ it ) {
        dbtdialog->addTag( QString::fromStdString( it->name ), QString::fromStdString( it->targetSha ) );
    }

    dbtdialog->endAdding();

    if ( dbtdialog->exec() == QDialog::Accepted ) {
        QString selectedTag = dbtdialog->selectedRef().trimmed();
        if ( !selectedTag.isEmpty() && selectedTag != tr( "No branches or tags in repository" ) ) {
            error = lgit_->deleteTag( selectedTag.toStdString() );
            if ( 0 == error ) {
                MessagesI.AppendMessageT( tr( "Successful deletion of tag <b>" ) + selectedTag + "</b>" );
                if ( ( error = lgit_->loadTags() ) > 0 ) {
                    MessagesI.AppendMessageT( tr( "Could not reload tags, error code: %1" ).arg( error ) );
                }
            } else {
                MessagesI.AppendMessageT( tr( "Cannot delete tag <b>%1</b>, error code: %2").arg( selectedTag ).arg( error ) );
            }
        }
    } else {
        MessagesI.AppendMessageT( tr( "<font color='green'>Tag deletion aborted</font>" ) );
    }

}
