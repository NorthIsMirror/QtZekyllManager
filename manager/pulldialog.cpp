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

#include "pulldialog.h"
#include "ui_pulldialog.h"

#include "singleton.h"
#include "messages.h"
#include "commitdialog.h"

#include <cmath>

#include <QVariant>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QCoreApplication>
#include <QDebug>

#define MessagesI Singleton<Messages>::instance()

static QDebug operator<< ( QDebug out, const std::string & str )
{
    out << QString::fromStdString( str );
    return out;
}

PullDialog::PullDialog( QWidget *parent ) :
    QDialog( parent ),
    table_has_notifications_( false ),
    progressShown_( false ),
    pwidget_( 0 ),
    ui( new Ui::PullDialog )
{
    ui->setupUi(this);
    ui->gridLayout->setSpacing( 10 );
    ui->gridLayout->setAlignment( Qt::AlignTop );
    ui->gridLayout_2->setSpacing( 10 );
    ui->gridLayout_2->setAlignment( Qt::AlignTop );
    ui->horizontalLayout_4->setSpacing( 10 );
    ui->horizontalLayout_4->setAlignment( Qt::AlignTop );
    ui->horizontalLayout_4->setContentsMargins( -1, -1, -1, 8 );
    ui->horizontalLayout_5->setSpacing( 10 );
    ui->horizontalLayout_5->setAlignment( Qt::AlignTop );

    ui->checkoutLabel->setContentsMargins( -1, -1, -1, 3 );

    ui->verticalLayout->setAlignment( Qt::AlignTop );
    ui->verticalLayout->setSpacing( 0 );

    pwidget_ = new ProgressWidget( this );

    table_has_notifications_ = true;
    switchTableToLog();
}

PullDialog::~PullDialog()
{
    delete ui;
}

int PullDialog::reset()
{
    int error, retval = 0;

    error = lgit_->loadBranches( BRANCH_ALL );
    error = lgit_->establishCurrent();
    error = lgit_->listRemotes();

    // What's currently checked out
    if ( lgit_->current().type() == CURRENT_TYPE_BRANCH ) {
        ui->checkoutLabel->setText( tr("Currently checked out: ") + QString::fromStdString( lgit_->current().branch() ) + tr(" (branch)") );
    } else if ( lgit_->current().type() == CURRENT_TYPE_TAG ) {
        ui->checkoutLabel->setText( tr("Currently checked out: ") + QString::fromStdString( lgit_->current().tag() ) + tr(" (tag)") );
    } else if ( lgit_->current().type() == CURRENT_TYPE_OID ) {
        ui->checkoutLabel->setText( tr("Detached HEAD points to: ") + QString::fromStdString( lgit_->current().oid() ) );
    }

    //
    // List of branches that can be fetched
    //

    QString selectedBranch;
    if( ui->branchCombo->count() > 0 ) {
        selectedBranch = ui->branchCombo->currentText();
    }

    ui->branchCombo->clear();
    int count = lgit_->branches().count();
    int remember_idx = 0;
    for ( int idx = 0; idx < count; idx ++ ) {
        const mybranch & b = lgit_->branches()[idx];
        if ( !b.is_in_fetch_head && b.type == BRANCH_LOCAL ) {
            ui->branchCombo->addItem( QString::fromStdString( b.name ), QVariant( idx ) );

            if( b.tip_sha == lgit_->current().oid() ) {
                remember_idx = idx;
            }
        }
    }
    ui->branchCombo->setCurrentIndex( remember_idx );

    if( !selectedBranch.isEmpty() ) {
        int idx = ui->branchCombo->findText( selectedBranch );
        if( idx != -1 ) {
            ui->branchCombo->setCurrentIndex( idx );
        }
    }

    //
    // List available remotes
    //

    QString selectedRemote;
    if( ui->remotesCombo->count() > 0 ) {
        selectedRemote = ui->remotesCombo->currentText();
    }

    ui->remotesCombo->clear();
    int count2 = lgit_->remotes().count();
    for ( int idx = 0; idx < count2; idx ++ ) {
        std::string remote = lgit_->remotes().entry(idx).name;
        ui->remotesCombo->addItem( QString::fromStdString( remote ), QVariant( idx ) );
    }

    if( !selectedRemote.isEmpty() ) {
        int idx = ui->remotesCombo->findText( selectedRemote );
        if( idx != -1 ) {
            ui->remotesCombo->setCurrentIndex( idx );
        }
    }

    //
    // List available FETCH_HEAD tips
    //

    QString selectedFetchHeadTip;
    if( ui->fetchHeadCombo->count() > 0 ) {
        selectedFetchHeadTip = ui->fetchHeadCombo->currentText();
    }

    populateFetchHead();

    if( !selectedFetchHeadTip.isEmpty() ) {
        int idx = ui->fetchHeadCombo->findText( selectedFetchHeadTip );
        if( idx != -1 ) {
            ui->fetchHeadCombo->setCurrentIndex( idx );
        }
    }

    //
    // List log of selected FETCH_HEAD tip
    //

    if( ui->fetchHeadCombo->count() > 0 ) {
        int idx = ui->fetchHeadCombo->currentData().toInt();
        const mybranch & b = lgit_->branches()[ idx ];
        logOfTip( QString::fromStdString( b.tip_sha ), QString::fromStdString( b.name ) );
    }

    //
    // Update merge analysis message
    //

    updateMergeAnalysis();

    return retval;
}

int PullDialog::populateFetchHead()
{
    ui->fetchHeadCombo->clear();
    int count = lgit_->branches().count();

    // List of FETCH_HEAD revisions
    for ( int idx = 0; idx < count; idx ++ ) {
        const mybranch & b = lgit_->branches()[ idx ];
        if ( b.is_in_fetch_head ) {
            QString name = QString::fromStdString( b.name );
            QString remoteUrl = QString::fromStdString( b.fetch_head_remote_url );

            myremote myremote = lgit_->remotes().findRemoteByUrl( remoteUrl );
            if( myremote.push_url != "-" ) {
                name = name + " [" + QString::fromStdString( myremote.name ) + "]";
            }
            ui->fetchHeadCombo->addItem( name, QVariant( idx ) );
        }
    }

    return 0;
}

int PullDialog::updateMergeAnalysis()
{
    int error;

    // Merge analysis, of currently checked out branch,
    // and corresponding branch from FETCH_HEAD
    if ( lgit_->current().type() == CURRENT_TYPE_TAG ) {
        ui->mergeTypeLabel->setText( tr("No merge possible – HEAD is detached at a tag (") + QString::fromStdString( lgit_->current().tag() ) + ")" );
    } else if ( lgit_->current().type() == CURRENT_TYPE_OID ){
        ui->mergeTypeLabel->setText( tr("No merge possible – HEAD is detached") );
    } else if ( lgit_->current().type() == CURRENT_TYPE_BRANCH ) {
        std::string branch_name = lgit_->current().branch();
        std::string head_oid = lgit_->current().oid();

        if ( ui->fetchHeadCombo->count() > 0 ) {
            int idx = ui->fetchHeadCombo->currentData().toInt();
            const mybranch & b2 = lgit_->branches()[ idx ];

            if( b2.name != branch_name ) {
                ui->mergeTypeLabel->setText( tr( "Selected commits are for branch `%1', to merge checkout it first" )
                                             .arg( QString::fromStdString(b2.name) ) );
            } else {
                // Additional consistency check
                const mybranch & b1 = lgit_->branches().findNameWithType( branch_name.c_str(), FIND_BRANCH_FETCH_HEAD );
                if( b1.tip_sha != b2.tip_sha ) {
                    MessagesI.AppendMessageT( tr( "Slight internal problem: SHA mismatch detected (%1/%2)" )
                                              .arg( QString::fromStdString(head_oid) ).arg( QString::fromStdString(b2.tip_sha) ) );
                }

                if( ( error = lgit_->analyzeMerge( branch_name, b2.tip_sha ) ) > 0 ) {
                    ui->mergeTypeLabel->setText( tr( "Error during merge analysis, cannot merge" ) );
                    return 241 + error;
                }

                if ( lgit_->analysisResult() == ANALYSIS_UNSET || lgit_->analysisResult() == ANALYSIS_ERROR ) {
                    ui->mergeTypeLabel->setText( tr( "Error during merge analysis, cannot merge" ) );
                }

                if ( lgit_->analysisResult() == ANALYSIS_NONE ) {
                    ui->mergeTypeLabel->setText( tr( "libgit2: no merge is possible" ) );
                }

                if ( lgit_->analysisResult() & ANALYSIS_FASTFORWARD ) {
                    ui->mergeTypeLabel->setText( tr( "Merge type available: fast-forward" ) );
                } else if ( lgit_->analysisResult() & ANALYSIS_NORMAL ) {
                    ui->mergeTypeLabel->setText( tr( "Merge type available: normal, possibly with conflicts" ) );
                } else if ( lgit_->analysisResult() & ANALYSIS_UNBORN ) {
                    ui->mergeTypeLabel->setText( tr( "Merge type available: fast-forward" ) );
                } else if ( lgit_->analysisResult() & ANALYSIS_UP_TO_DATE ) {
                    ui->mergeTypeLabel->setText( tr( "No merge is needed" ) );
                } else {
                    ui->mergeTypeLabel->setText( tr( "internal error") );
                }
            }

        } else {
            ui->mergeTypeLabel->setText( tr("No fetched commits") );
        }
    }

    return 0;
}

int PullDialog::switchTableToLog()
{
    // Already switched?
    if( !table_has_notifications_ ) {
        return 1;
    }

    table_has_notifications_ = false;

    ui->tableWidget->setRowCount( 0 );
    ui->tableWidget->setColumnCount( 3 );
    ui->tableWidget->verticalHeader()->setDefaultSectionSize( 50 );
    QStringList labels;
    labels << tr( "ID" ) << tr( "Author" ) << tr( "Message" );
    ui->tableWidget->horizontalHeader()->setVisible( true );
    ui->tableWidget->setHorizontalHeaderLabels( labels );

    return 0;
}

int PullDialog::switchTableToNotifications()
{
    // Already switched?
    if( table_has_notifications_ ) {
        return 1;
    }

    table_has_notifications_ = true;

    ui->tableWidget->setRowCount( 0 );
    ui->tableWidget->setColumnCount( 2 );
    ui->tableWidget->verticalHeader()->setDefaultSectionSize( 32 );
    ui->tableWidget->horizontalHeader()->setVisible( false );

    return 0;
}

int PullDialog::logOfTip( QString sha, QString hide )
{
    switchTableToLog();

    const mybranch & b = lgit_->branches().findNameWithType( hide.trimmed().toUtf8().constData(), FIND_BRANCH_LOCAL );

    if( b.invalid == INVALID_DUMMY ) {
        lgit_->readLog( sha, "" );
    } else {
        lgit_->readLog( sha, hide );
    }
    int count = lgit_->log().count();

    ui->tableWidget->setRowCount( 0 );
    for( int idx = 0; idx < count; idx ++ ) {
        log_entry entry = lgit_->log().entry( idx );
        QTableWidgetItem *item1 = new QTableWidgetItem( QString::fromStdString( entry.sha ) );
        QTableWidgetItem *item2 = new QTableWidgetItem( QString::fromStdString( entry.author ) );
        QTableWidgetItem *item3 = new QTableWidgetItem( QString::fromStdString( entry.message ) );

        item1->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
        item2->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
        item3->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

        item1->setToolTip( QString::fromStdString( entry.diff_data_summary_html() ) );
        item2->setToolTip( QString::fromStdString( entry.diff_data_summary_html() ) );
        item3->setToolTip( QString::fromStdString( entry.diff_data_summary_html() ) );


        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem( row, 0, item1 );
        ui->tableWidget->setItem( row, 1, item2 );
        ui->tableWidget->setItem( row, 2, item3 );
    }

    return 0;
}

void PullDialog::runCommitDialog( const std::string & msg, bool usem, const std::string & parent1, bool use1, const std::string & parent2, bool use2 )
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

    if( usem ) {
        dialog->setCommitMessage( QString::fromStdString( msg ) );
    }

    if( use1 ) {
        if( ! dialog->addParent( parent1 ) ) {
            MessagesI.AppendMessageT( "Warning: Duplicate commit parent (1)" );
        }
    }

    if( use2 ) {
        if( ! dialog->addParent( parent2 ) ) {
            MessagesI.AppendMessageT( "Warning: Duplicate commit parent (2)" );
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

    error = lgit_->commit( dialog->commitMessage(), dialog->parents() );

    delete dialog;

    if( error == 0 ) {
        MessagesI.AppendMessageT( QString("Successfully performed merge commit") );
    } else {
        MessagesI.AppendMessageT( QString("Exit code of git commit: %1").arg( error ) );
    }
}

void PullDialog::addNotification( git_checkout_notify_t why, const QString &path )
{
    switchTableToNotifications();

    QTableWidgetItem *item1;

    if ( why == GIT_CHECKOUT_NOTIFY_NONE ) {
            return;
    }

    switch( why ) {
    case GIT_CHECKOUT_NOTIFY_CONFLICT:
        item1 = new QTableWidgetItem( "Conflict" );
        item1->setBackground( Qt::yellow );
        break;
    case GIT_CHECKOUT_NOTIFY_DIRTY:
        item1 = new QTableWidgetItem( "Dirty" );
        item1->setBackground( Qt::red );
        break;
    case GIT_CHECKOUT_NOTIFY_UPDATED:
        item1 = new QTableWidgetItem( "Updated" );
        item1->setBackground( Qt::green );
        break;
    case GIT_CHECKOUT_NOTIFY_UNTRACKED:
        item1 = new QTableWidgetItem( "Untracked" );
        break;
    case GIT_CHECKOUT_NOTIFY_IGNORED:
        item1 = new QTableWidgetItem( "Ignored" );
        item1->setForeground( Qt::blue );
        break;
    default:
        item1 = new QTableWidgetItem( "" );
        break;
    }

    QTableWidgetItem *item2 = new QTableWidgetItem( path );

    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    ui->tableWidget->setItem( row, 0, item1 );
    ui->tableWidget->setItem( row, 1, item2 );
}

void PullDialog::updateProgress( double progress )
{
    pwidget_->upd( progress );

    if( !progressShown_ ) {
        pwidget_->show();
        pwidget_->raise();
        progressShown_ = true;
    }

    if( fabs( progress - 1.0 ) < 0.05 ) {
        pwidget_->hide();
        progressShown_ = false;
    }

    QCoreApplication::processEvents();
}

void PullDialog::on_fetchHeadCombo_activated( int index )
{
    reset();
}

void PullDialog::on_fetchBranch_clicked()
{
    if ( ui->branchCombo->count() <= 0 ) {
        return;
    }

    int idx = ui->branchCombo->currentData().toInt();
    const mybranch & b = lgit_->branches()[ idx ];

    int idx2 = ui->remotesCombo->currentData().toInt();
    const myremote & r = lgit_->remotes().entry( idx2 );

    QString branchArg = QString::fromStdString( b.name );
    QString remoteArg = QString::fromStdString( r.name );

    updateProgress( 0.05 );

    int error = lgit_->fetchBranch( branchArg, remoteArg );

    updateProgress( 1.0 );

    reset();
}

void PullDialog::on_fetchAll_clicked()
{
    // Empty branch means follow config, fetch multiple branches
    if( ui->remotesCombo->count() <= 0 ) {
        MessagesI.AppendMessageT( "No configured remotes (in git config)" );
        return;
    }

    int idx = ui->remotesCombo->currentData().toInt();
    const myremote & remote = lgit_->remotes().entry( idx );

    if( remote.name != "-" ) {
        updateProgress( 0.05 );
        int error = lgit_->fetchBranch( "", QString::fromStdString( remote.name ) );
        updateProgress( 1.0 );
    } else {
        MessagesI.AppendMessageT( "Could not find selected remote, cannot fetch" );
        return;
    }

    reset();
}

void PullDialog::on_fetchURL_clicked()
{
    QString url = ui->url->text().trimmed();
    QString branch = ui->branch->text().trimmed();
    if( url.isEmpty() ) {
        MessagesI.AppendMessageT( "No URL provided, cannot fetch" );
        return;
    }

    if( branch.isEmpty() ) {
        MessagesI.AppendMessageT( "No branch provided, assuming 'master'");
        branch = "master";
    }

    updateProgress( 0.05 );

    int error = lgit_->fetchBranch( branch, url );

    updateProgress( 1.0 );

    reset();
}

void PullDialog::on_merge_clicked()
{
    int error;

    if ( ui->fetchHeadCombo->count() <= 0 ) {
        return;
    }

    int idx = ui->fetchHeadCombo->currentData().toInt();
    const mybranch & selected_branch = lgit_->branches()[ idx ];

    if ( selected_branch.name != lgit_->current().branch() ) {
        QMessageBox::warning( this, tr( "Cannot merge" ), tr( "Selected commits are for branch `%1', while currently checked out branch is `%2'. Please checkout the branch you want to merge." )
                              .arg( QString::fromStdString( selected_branch.name ) )
                              .arg( QString::fromStdString( lgit_->current().branch() ) ) );
        return;
    }

    if ( lgit_->analysisResult() & ANALYSIS_FASTFORWARD || lgit_->analysisResult() & ANALYSIS_UNBORN ) {
        error = lgit_->fastForwardSha( selected_branch.name, selected_branch.tip_sha, CHECKOUT_MERGE );
        if( error == 0 ) {
            updateMergeAnalysis();
            ui->mergeTypeLabel->setText( tr( "Fast-forward successful" ) );
        }
    } else if ( lgit_->analysisResult() & ANALYSIS_UP_TO_DATE ) {
        QMessageBox::information( this, "Information", "No merge is needed" );
    } else if ( lgit_->analysisResult() & ANALYSIS_NORMAL ) {
        error = lgit_->mergeBranch( lgit_->current().branch(), lgit_->current().oid(), selected_branch );
        if( error == 0 ) {
            bool result = false;
            if( ( error = lgit_->indexHasConflicts( &result ) ) < 0 ) {
                MessagesI.AppendMessageT( tr( "Merge appeared a success, however could not check if index has conflicts (error: %1)" ).arg( error ) );
            } else {
                if( !result ) {
                    QString msg = "Merge branch '";
                    msg += QString::fromStdString( selected_branch.name ) + "' of ";
                    msg += QString::fromStdString( selected_branch.fetch_head_remote_url );
                    runCommitDialog( msg.toStdString(), true, lgit_->current().oid(), true, selected_branch.tip_sha, true );
                } else {
                    QMessageBox::warning( this, "Conflicts", "There are merge conflicts. You can commit and conclude"
                                                             " the merge after resolving them." );
                }
            }
        } else if( error / 10000 == ( GIT_ECONFLICT * -1 ) ) {
            QMessageBox::warning( this, "Error", "There are uncommited changes that conflict with the fetched commits. "
                                                 "Please clean the work dir before doing the merge." );
        } else {
            MessagesI.AppendMessageT( tr( "Merge stopped because of general errors (exit code: %1)" ).arg( error ) );
        }
    }
}
