#include "progresswidget.h"
#include "ui_progresswidget.h"

ProgressWidget::ProgressWidget( QWidget *parent ) :
    QWidget( parent ),
    ui( new Ui::ProgressWidget )
{
    ui->setupUi( this );

    setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint );

    ui->progressBar->setMinimum( 0 );
    ui->progressBar->setMaximum( 100 );
}

ProgressWidget::~ProgressWidget()
{
    delete ui;
}

void ProgressWidget::upd(double progress)
{
    int steps = int( progress * 100 );
    ui->progressBar->setValue( steps );
}
