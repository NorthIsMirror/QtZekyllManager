#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "editor.h"
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

    zkiresize_ = new ZkIResize();

    // Connect ZkIResize to obtain list of zekylls
    QObject::connect(this->zkiresize_,
                     SIGNAL(result_list(int, QStringList)),
                     this,
                     SLOT(handle_zkiresize_list(int, QStringList)));

    // Connect ZkIResize to obtain list of zekylls
    QObject::connect(this->zkiresize_,
                     SIGNAL(result_consistent(int, QStringList)),
                     this,
                     SLOT(handle_zkiresize_consistent(int, QStringList)));

    connect(ui->curRepoButton, &QAbstractButton::clicked, this, &MainWindow::browse);
    connect(this, &MainWindow::repositoryChanged, this, &MainWindow::reloadRepository);
    QObject::connect(&MessagesI, SIGNAL(messagesChanged(const QStringList&)), this, SLOT(updateMessages(const QStringList&)) );
    QObject::connect(this->ui->tabWidget, SIGNAL(thirdTabMarkingEnough()), this, SLOT(stopThirdTabMarking()));

    zkiresize_->setIndex( current_index_ );
    zkiresize_->list();

    tuple< vector<string>, int > set_index_result = setIndex( current_index_ );
    ZKL_INDEX_ZEKYLLS_ = get<0>(set_index_result);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete zkiresize_;
}

void MainWindow::handle_zkiresize_list(int exitCode, QStringList entries) {
    if( exitCode != 0 && exitCode != 12 ) {
        QString error_decode = decode_zkiresize_exit_code(exitCode);
        if( error_decode != "" ) {
            MessagesI.AppendMessageT( tr("<font color=green>Message from the Zekyll backend (1):</font> ") + error_decode );
        }
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

    if( entries.count() == 0 ) {
        MessagesI.AppendMessageT(tr("Index ") + QString("%1") . arg(current_index_) + tr(" is empty (go ahead and resize it), or selected path isn't a Zekyll repository"));
    }
}

void MainWindow::handle_zkiresize_consistent(int exitCode, QStringList entries) {
    if( exitCode != 11 ) {
        isConsistent2_ = (exitCode == 0);
        QString error_decode = decode_zkiresize_exit_code(exitCode);
        if( error_decode != "" ) {
            MessagesI.AppendMessageT( tr("<font color=green>Message from the Zekyll backend (2):</font> ") + error_decode );
        }
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
    ui->tableWidget->setRowCount(0);
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_3->setRowCount(0);
    lzcsde_list_.clear();
    lzcsde_consistent_.clear();

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
            Editor *editor = new Editor(this);
            editor->setFile( current_path_ + "/" + entry.listing() );
            editor->show();
        } else {
            MessagesI.AppendMessageT("<font color=red>Could not find data associated with the clicked cell</font>");
        }
    }
}
