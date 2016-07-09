cmake_minimum_required( VERSION 2.8.12 )
project( QScintilla2 )
set( QT_VERSION_REQ "5.2" )

if( NOT CMAKE_PREFIX_PATH )
    # Edit this path to point to your prefix of Qt installation
    # directory. Prefix means: path up to and not including "bin"
    set( CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.7/clang_64 )
    #set( CMAKE_PREFIX_PATH /usr/local/Cellar/qt5/5.6.1 )
endif()

function( QUERY_QMAKE VAR RESULT )
    get_target_property( QMAKE_EXEC Qt5::qmake LOCATION )
    exec_program( "${QMAKE_EXEC}" ARGS "-query ${VAR}" RETURN_VALUE retval OUTPUT_VARIABLE out )
    if( retval EQUAL 0 )
        file( TO_CMAKE_PATH "${out}" out )
        set( ${RESULT} ${out} PARENT_SCOPE )
    endif()
endfunction()

SET( HDRS
    ./Qsci/qsciglobal.h
    ./Qsci/qsciscintilla.h
    ./Qsci/qsciscintillabase.h
    ./Qsci/qsciabstractapis.h
    ./Qsci/qsciapis.h
    ./Qsci/qscicommand.h
    ./Qsci/qscicommandset.h
    ./Qsci/qscidocument.h
    ./Qsci/qscilexer.h
    ./Qsci/qscilexeravs.h
    ./Qsci/qscilexerbash.h
    ./Qsci/qscilexerbatch.h
    ./Qsci/qscilexercmake.h
    ./Qsci/qscilexercoffeescript.h
    ./Qsci/qscilexercpp.h
    ./Qsci/qscilexercsharp.h
    ./Qsci/qscilexercss.h
    ./Qsci/qscilexercustom.h
    ./Qsci/qscilexerd.h
    ./Qsci/qscilexerdiff.h
    ./Qsci/qscilexerfortran.h
    ./Qsci/qscilexerfortran77.h
    ./Qsci/qscilexerhtml.h
    ./Qsci/qscilexeridl.h
    ./Qsci/qscilexerjava.h
    ./Qsci/qscilexerjavascript.h
    ./Qsci/qscilexerlua.h
    ./Qsci/qscilexermakefile.h
    ./Qsci/qscilexermatlab.h
    ./Qsci/qscilexeroctave.h
    ./Qsci/qscilexerpascal.h
    ./Qsci/qscilexerperl.h
    ./Qsci/qscilexerpostscript.h
    ./Qsci/qscilexerpo.h
    ./Qsci/qscilexerpov.h
    ./Qsci/qscilexerproperties.h
    ./Qsci/qscilexerpython.h
    ./Qsci/qscilexerruby.h
    ./Qsci/qscilexerspice.h
    ./Qsci/qscilexersql.h
    ./Qsci/qscilexertcl.h
    ./Qsci/qscilexertex.h
    ./Qsci/qscilexerverilog.h
    ./Qsci/qscilexervhdl.h
    ./Qsci/qscilexerxml.h
    ./Qsci/qscilexeryaml.h
    ./Qsci/qscimacro.h
    ./Qsci/qsciprinter.h
    ./Qsci/qscistyle.h
    ./Qsci/qscistyledtext.h
    ListBoxQt.h
    SciClasses.h
    SciNamespace.h
    ScintillaQt.h
    ../include/ILexer.h
    ../include/Platform.h
    ../include/SciLexer.h
    ../include/Scintilla.h
    ../include/ScintillaWidget.h
    ../lexlib/Accessor.h
    ../lexlib/CharacterCategory.h
    ../lexlib/CharacterSet.h
    ../lexlib/LexAccessor.h
    ../lexlib/LexerBase.h
    ../lexlib/LexerModule.h
    ../lexlib/LexerNoExceptions.h
    ../lexlib/LexerSimple.h
    ../lexlib/OptionSet.h
    ../lexlib/PropSetSimple.h
    ../lexlib/StringCopy.h
    ../lexlib/StyleContext.h
    ../lexlib/SubStyles.h
    ../lexlib/WordList.h
    ../src/AutoComplete.h
    ../src/CallTip.h
    ../src/CaseConvert.h
    ../src/CaseFolder.h
    ../src/Catalogue.h
    ../src/CellBuffer.h
    ../src/CharClassify.h
    ../src/ContractionState.h
    ../src/Decoration.h
    ../src/Document.h
    ../src/EditModel.h
    ../src/Editor.h
    ../src/EditView.h
    ../src/ExternalLexer.h
    ../src/FontQuality.h
    ../src/Indicator.h
    ../src/KeyMap.h
    ../src/LineMarker.h
    ../src/MarginView.h
    ../src/Partitioning.h
    ../src/PerLine.h
    ../src/PositionCache.h
    ../src/RESearch.h
    ../src/RunStyles.h
    ../src/ScintillaBase.h
    ../src/Selection.h
    ../src/SplitVector.h
    ../src/Style.h
    ../src/UnicodeFromUTF8.h
    ../src/UniConversion.h
    ../src/ViewStyle.h
    ../src/XPM.h
)

SET( SRCS
    qsciscintilla.cpp
    qsciscintillabase.cpp
    qsciabstractapis.cpp
    qsciapis.cpp
    qscicommand.cpp
    qscicommandset.cpp
    qscidocument.cpp
    qscilexer.cpp
    qscilexeravs.cpp
    qscilexerbash.cpp
    qscilexerbatch.cpp
    qscilexercmake.cpp
    qscilexercoffeescript.cpp
    qscilexercpp.cpp
    qscilexercsharp.cpp
    qscilexercss.cpp
    qscilexercustom.cpp
    qscilexerd.cpp
    qscilexerdiff.cpp
    qscilexerfortran.cpp
    qscilexerfortran77.cpp
    qscilexerhtml.cpp
    qscilexeridl.cpp
    qscilexerjava.cpp
    qscilexerjavascript.cpp
    qscilexerlua.cpp
    qscilexermakefile.cpp
    qscilexermatlab.cpp
    qscilexeroctave.cpp
    qscilexerpascal.cpp
    qscilexerperl.cpp
    qscilexerpostscript.cpp
    qscilexerpo.cpp
    qscilexerpov.cpp
    qscilexerproperties.cpp
    qscilexerpython.cpp
    qscilexerruby.cpp
    qscilexerspice.cpp
    qscilexersql.cpp
    qscilexertcl.cpp
    qscilexertex.cpp
    qscilexerverilog.cpp
    qscilexervhdl.cpp
    qscilexerxml.cpp
    qscilexeryaml.cpp
    qscimacro.cpp
    qsciprinter.cpp
    qscistyle.cpp
    qscistyledtext.cpp
    MacPasteboardMime.cpp
    InputMethod.cpp
    SciClasses.cpp
    ListBoxQt.cpp
    PlatQt.cpp
    ScintillaQt.cpp
    ../lexers/LexA68k.cpp
    ../lexers/LexAbaqus.cpp
    ../lexers/LexAda.cpp
    ../lexers/LexAPDL.cpp
    ../lexers/LexAsm.cpp
    ../lexers/LexAsn1.cpp
    ../lexers/LexASY.cpp
    ../lexers/LexAU3.cpp
    ../lexers/LexAVE.cpp
    ../lexers/LexAVS.cpp
    ../lexers/LexBaan.cpp
    ../lexers/LexBash.cpp
    ../lexers/LexBasic.cpp
    ../lexers/LexBibTex.cpp
    ../lexers/LexBullant.cpp
    ../lexers/LexCaml.cpp
    ../lexers/LexCLW.cpp
    ../lexers/LexCmake.cpp
    ../lexers/LexCOBOL.cpp
    ../lexers/LexCoffeeScript.cpp
    ../lexers/LexConf.cpp
    ../lexers/LexCPP.cpp
    ../lexers/LexCrontab.cpp
    ../lexers/LexCsound.cpp
    ../lexers/LexCSS.cpp
    ../lexers/LexD.cpp
    ../lexers/LexDMAP.cpp
    ../lexers/LexDMIS.cpp
    ../lexers/LexECL.cpp
    ../lexers/LexEiffel.cpp
    ../lexers/LexErlang.cpp
    ../lexers/LexEScript.cpp
    ../lexers/LexFlagship.cpp
    ../lexers/LexForth.cpp
    ../lexers/LexFortran.cpp
    ../lexers/LexGAP.cpp
    ../lexers/LexGui4Cli.cpp
    ../lexers/LexHaskell.cpp
    ../lexers/LexHex.cpp
    ../lexers/LexHTML.cpp
    ../lexers/LexInno.cpp
    ../lexers/LexKix.cpp
    ../lexers/LexKVIrc.cpp
    ../lexers/LexLaTex.cpp
    ../lexers/LexLisp.cpp
    ../lexers/LexLout.cpp
    ../lexers/LexLua.cpp
    ../lexers/LexMagik.cpp
    ../lexers/LexMarkdown.cpp
    ../lexers/LexMatlab.cpp
    ../lexers/LexMetapost.cpp
    ../lexers/LexMMIXAL.cpp
    ../lexers/LexModula.cpp
    ../lexers/LexMPT.cpp
    ../lexers/LexMSSQL.cpp
    ../lexers/LexMySQL.cpp
    ../lexers/LexNimrod.cpp
    ../lexers/LexNsis.cpp
    ../lexers/LexOpal.cpp
    ../lexers/LexOScript.cpp
    ../lexers/LexOthers.cpp
    ../lexers/LexPascal.cpp
    ../lexers/LexPB.cpp
    ../lexers/LexPerl.cpp
    ../lexers/LexPLM.cpp
    ../lexers/LexPO.cpp
    ../lexers/LexPOV.cpp
    ../lexers/LexPowerPro.cpp
    ../lexers/LexPowerShell.cpp
    ../lexers/LexProgress.cpp
    ../lexers/LexPS.cpp
    ../lexers/LexPython.cpp
    ../lexers/LexR.cpp
    ../lexers/LexRebol.cpp
    ../lexers/LexRegistry.cpp
    ../lexers/LexRuby.cpp
    ../lexers/LexRust.cpp
    ../lexers/LexScriptol.cpp
    ../lexers/LexSmalltalk.cpp
    ../lexers/LexSML.cpp
    ../lexers/LexSorcus.cpp
    ../lexers/LexSpecman.cpp
    ../lexers/LexSpice.cpp
    ../lexers/LexSQL.cpp
    ../lexers/LexSTTXT.cpp
    ../lexers/LexTACL.cpp
    ../lexers/LexTADS3.cpp
    ../lexers/LexTAL.cpp
    ../lexers/LexTCL.cpp
    ../lexers/LexTCMD.cpp
    ../lexers/LexTeX.cpp
    ../lexers/LexTxt2tags.cpp
    ../lexers/LexVB.cpp
    ../lexers/LexVerilog.cpp
    ../lexers/LexVHDL.cpp
    ../lexers/LexVisualProlog.cpp
    ../lexers/LexYAML.cpp
    ../lexlib/Accessor.cpp
    ../lexlib/CharacterCategory.cpp
    ../lexlib/CharacterSet.cpp
    ../lexlib/LexerBase.cpp
    ../lexlib/LexerModule.cpp
    ../lexlib/LexerNoExceptions.cpp
    ../lexlib/LexerSimple.cpp
    ../lexlib/PropSetSimple.cpp
    ../lexlib/StyleContext.cpp
    ../lexlib/WordList.cpp
    ../src/AutoComplete.cpp
    ../src/CallTip.cpp
    ../src/CaseConvert.cpp
    ../src/CaseFolder.cpp
    ../src/Catalogue.cpp
    ../src/CellBuffer.cpp
    ../src/CharClassify.cpp
    ../src/ContractionState.cpp
    ../src/Decoration.cpp
    ../src/Document.cpp
    ../src/EditModel.cpp
    ../src/Editor.cpp
    ../src/EditView.cpp
    ../src/ExternalLexer.cpp
    ../src/Indicator.cpp
    ../src/KeyMap.cpp
    ../src/LineMarker.cpp
    ../src/MarginView.cpp
    ../src/PerLine.cpp
    ../src/PositionCache.cpp
    ../src/RESearch.cpp
    ../src/RunStyles.cpp
    ../src/ScintillaBase.cpp
    ../src/Selection.cpp
    ../src/Style.cpp
    ../src/UniConversion.cpp
    ../src/ViewStyle.cpp
    ../src/XPM.cpp
)

SET( TRANSLATIONS
    qscintilla_cs.ts
    qscintilla_de.ts
    qscintilla_es.ts
    qscintilla_fr.ts
    qscintilla_pt_br.ts
)

# Find includes in corresponding build directories
set( CMAKE_INCLUDE_CURRENT_DIR ON )

# Run moc automatically when required
set( CMAKE_AUTOMOC ON )

# Widgets finds its own dependencies (QtGui and QtCore).
find_package( Qt5Widgets ${QT_VERSION_REQ} REQUIRED )
find_package( Qt5PrintSupport ${QT_VERSION_REQ} REQUIRED )
find_package( Qt5LinguistTools ${QT_VERSION_REQ} REQUIRED )
if( APPLE )
    find_package( Qt5MacExtras ${QT_VERSION_REQ} REQUIRED )
endif()

message("= QT is installed at: ${_qt5Widgets_install_prefix} =")

#set( DEFINES ${DEFINES} -DQSCINTILLA_MAKE_DLL )
#set( DEFINES ${DEFINES} -DSCI_NAMESPACE )
set( DEFINES ${DEFINES} -DSCINTILLA_QT -DSCI_LEXER )

if (";${DEFINES};" MATCHES ";-DQSCINTILLA_MAKE_DLL;")
    message( "= Dynamic build =" )
else()
    message( "= Static build =" )
endif()

QT5_ADD_TRANSLATION( QM_FILES ${TRANSLATIONS} )
ADD_CUSTOM_TARGET( trans ALL DEPENDS ${QM_FILES} )

# Qt5Widgets also includes directories for dependencies QtCore and QtGui
include_directories( ${Qt5Widgets_INCLUDE_DIRS} ${Qt5PrintSupport_INCLUDE_DIRS} 
                     ${Qt5LinguistTools_INCLUDE_DIRS} ${Qt5MacExtras_INCLUDE_DIRS}
                     . ../include ../lexlib ../src )

# Needed -DQT_WIDGETS_LIB for Qt5, and others
add_definitions( ${DEFINES} ${Qt5Widgets_DEFINITIONS} ${Qt5PrintSupport_DEFINITIONS} 
                 ${Qt5LinguistTools_DEFINITIONS} ${Qt5MacExtras_DEFINITIONS} )

# Executables fail to build with Qt5 without -fPIE
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

if (";${DEFINES};" MATCHES ";-DQSCINTILLA_MAKE_DLL;")
    add_library( ${PROJECT_NAME} SHARED ${SRCS} ${HDRS} )
else()
    add_library( ${PROJECT_NAME} STATIC ${SRCS} ${HDRS} )
endif()

if( APPLE )
    target_link_libraries( ${PROJECT_NAME} Qt5::Widgets Qt5::PrintSupport Qt5::MacExtras )
else()
    target_link_libraries( ${PROJECT_NAME} Qt5::Widgets Qt5::PrintSupport )
endif()

target_include_directories( ${PROJECT_NAME} INTERFACE "." )
