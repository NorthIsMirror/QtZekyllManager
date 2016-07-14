#include "mycodeeditor.h"
#include "editor/TextDocument.h"
#include "margin/MarginStacker.h"
#include "syntax/Highlighter.h"
#include "syntax/Model.h"
#include "syntax/Document.h"

#include <QtWidgets>

MyCodeEditor::MyCodeEditor( QWidget* parent ) : CodeEditor( parent )
{
    setCaretLineBackground( QColor( 150, 150, 150, 150 ) );

    MarginStacker* margins = new MarginStacker( this );
    margins->setVisible( QodeEdit::BookmarkMargin, true );
    margins->setVisible( QodeEdit::NumberMargin, true );
    margins->setVisible( QodeEdit::RevisionMargin, true );
    margins->setVisible( QodeEdit::FoldMargin, true );
    margins->setVisible( QodeEdit::SpaceMargin, true );

    setCaretLineBackground(paper());

    // fake save document shortcut
    new QShortcut( QKeySequence::Save, this, SLOT( save() ) );

    new QShortcut( QKeySequence("Ctrl+Shift+D"), this, SLOT(duplicateLine()) );

    new QShortcut( QKeySequence("Ctrl+Shift+Up"), this, SLOT(swapLineUp()) );
    new QShortcut( QKeySequence("Ctrl+Shift+Down"), this, SLOT(swapLineDown()) );

    new QShortcut( QKeySequence("Ctrl+]"), this, SLOT(indent()) );
    new QShortcut( QKeySequence("Ctrl+["), this, SLOT(unindent()) );

    new QShortcut( QKeySequence("Ctrl+J"), this, SLOT(joinLines()) );

    new QShortcut( QKeySequence("Ctrl+Shift+L"), this, SLOT(expandSelectionToLine()) );
    new QShortcut( QKeySequence("Ctrl+Shift+W"), this, SLOT(expandSelectionToWord()) );
}

MyCodeEditor::~MyCodeEditor()
{

}
