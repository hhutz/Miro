// -*- c++ -*- ///////////////////////////////////////////////////////////////
//
// This file is part of Miro (The Middleware for Robots)
// Copyright (C) 1999-2013
// Department of Neural Information Processing, University of Ulm
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

// Enable migration from Qt v3 to Qt v4
#define LSB_Q3GRIDLAYOUT
#define LSB_Q3HBOXLAYOUT
#define LSB_Q3PROGRESSDIALOG
#define LSB_Q3VBOXLAYOUT

#include "MainForm.h"
#include "LogFile.h"
#include "FileSet.h"
#include "EventView.h"
#include "ChannelManager.h"

#include "miroWidgets/FileListDialog.h"

#include "miro/Exception.h"
#include "miro/TimeHelper.h"
#include "miro/Log.h"

#include <ace/OS_NS_sys_time.h>

#define INCLUDE_MENUITEM_DEF
#include <qapplication.h>
#ifdef LSB_Q3POPUPMENU
#include <QMenu>
#else
#include <q3popupmenu.h>
#endif
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qlcdnumber.h>
#include <qtimer.h>
#include <qdial.h>
#ifdef LSB_Q3PROGRESSDIALOG
#include <QProgressDialog>
#else
#include <q3progressdialog.h>
#endif
#include <qmessagebox.h>
#include <qinputdialog.h>
#ifdef LSB_Q3FILEDIALOG
// Unused
#else
#include <q3filedialog.h>
#endif
#include <qobject.h>

#ifdef LSB_Q3HBOXLAYOUT
#include <QHBoxLayout>
#else
#include <Q3HBoxLayout>
#endif
#ifdef LSB_Q3GRIDLAYOUT
#include <QGridLayout>
#else
#include <Q3GridLayout>
#endif
#ifdef LSB_Q3VBOXLAYOUT
#include <QVBoxLayout>
#else
#include <Q3VBoxLayout>
#endif
#include <QFileDialog>
#include <QMenuItem>

#include <cstring>

using namespace std;

namespace
{
  char const * filter[] = {
    "log files (*.mlog)",
    "old log files (*.log*)",
    "all files (*)",
    NULL
  };
  const QString logExt(".mlog");
  const QString logFilter("log files (*"+logExt+");;old log files (*.log*);;all files (*)");
};

SubmenuEvent::SubmenuEvent(QObject * parent, char const * name) :
    Super(parent, name)
{}

void
SubmenuEvent::action(int _id)
{
  emit activated(parent(), _id);
}


ACE_Time_Value const MainForm::MIN_TIME(0, 20000);

MainForm::MainForm(QApplication& _app, FileSet& _fileSet,
                   QWidget *parent, const char *name ) :
    Super(parent, name),
    app_(_app),
    fileSet_(_fileSet),
    fileListDialog_(new FileListDialog(this, "File list dialog", "Log files", filter)),
    eventView_(NULL),
    timer_(new QTimer(this)),
    autoStartTimer_(new QTimer(this)),
#ifdef LSB_Q3POPUPMENU
    eventMenu_(new QMenu(this)),
#else
    eventMenu_(new Q3PopupMenu( this )),
#endif
    action_(false),
    speed_(1),
    history_(100),
    domainNameMenuId_(0),
    exitOnReplayEnd_(false),
    numConsumers_(0),
    m_opacity(1),
    m_loopPlay(false)
{
#ifdef LSB_Q3POPUPMENU
  // Use my name instead
#else
  QAction* action;
#endif
  // the menu
#ifdef LSB_Q3POPUPMENU
  QMenu * const fileMenu = menuBar()->addMenu(tr("&File"));
  QAction * pAction = NULL;
  
  pAction = new QAction(tr("Open/Close..."), this);
  connect(pAction, SIGNAL(triggered), this, SLOT(files()));
  fileMenu->addAction(pAction);

  pAction = new QAction(tr("Save &as..."), this);
  connect(pAction, SIGNAL(triggered), this, SLOT(saveAs()));
  fileMenu->addAction(pAction);

  fileMenu->addSeparator();

  pAction = new QAction(tr("&Quit"), qApp);
  connect(pAction, SIGNAL(triggered), SLOT(quit));
  fileMenu->addAction(pAction);
#else
  Q3PopupMenu *fileMenu = new Q3PopupMenu( this );
  fileMenu->insertItem( "&Open/Close...", this, SLOT( files() ) );
  fileMenu->insertItem("Save &as...", this, SLOT(saveAs()));
  fileMenu->insertSeparator();
  fileMenu->insertItem( "&Quit", qApp, SLOT( quit() ) );
#endif

#ifdef LSB_Q3POPUPMENU
  QMenu * const editMenu = menuBar()->addMenu(tr("&Edit"));

  pAction = new QAction(tr("Cut Front"), this);
  connect(pAction, SIGNAL(triggered), SLOT(cutFront()));
  editMenu->addAction(pAction);

  pAction = new QAction(tr("Cut Back"), this);
  connect(pAction, SIGNAL(triggered), SLOT(cutBack()));
  editMenu->addAction(pAction);

  pAction = new QAction(tr("Undo all"), this);
  connect(pAction, SIGNAL(triggered), SLOT(cutUndo()));
  editMenu->addAction(pAction);
#else
  Q3PopupMenu * editMenu = new Q3PopupMenu(this);
  editMenu->insertItem("Cut Front", this, SLOT(cutFront()));
  editMenu->insertItem("Cut Back", this, SLOT(cutBack()));
  editMenu->insertItem("Undo all", this, SLOT(cutUndo()));
#endif

#ifdef LSB_Q3POPUPMENU
  QMenu * const toolsMenu = menuBar()->addMenu("&Tools");
  /// @todo What is the counterpart of saving the return value from 
  /// Q3PopupMenu::insertItem()?
  pAction = new QAction(tr("&Event View"), this);
  pAction->setCheckable(true);
  connect(pAction, SIGNAL(triggered), SLOT(toggleEventView()));
  toolsMenu->addAction(pAction);
  // Store the Action for later toggling
  m_pToggleEventViewAction = pAction;
#else
  toolsMenu_ = new Q3PopupMenu(this);
  toolsMenu_->setCheckable(true);
  eventViewId_ = toolsMenu_->insertItem("&Event View", this, SLOT(toggleEventView()));
#endif

#ifdef LSB_Q3POPUPMENU
  QMenu * const settingsMenu = menuBar()->addMenu("&Settings");

  pAction = new QAction(tr("&History"), this);
  connect(pAction, SIGNAL(triggered), SLOT(setHistory()));
  settingsMenu->addAction(pAction);

  pAction = new QAction(tr("&Loop Playback"), this);
  pAction->setCheckable(true);
  pAction->setChecked(m_loopPlay);
  connect(pAction, SIGNAL(toggled(bool)), SLOT(enableLoopPlayback(bool)));
  settingsMenu->addAction(pAction);

  pAction = new QAction(tr("Always on &Top"), this);
  pAction->setCheckable(true);
  pAction->setChecked(false);
  connect(pAction, SIGNAL(toggled(bool)), SLOT(eanbleTransparency(bool)));
  settingsMenu->addAction(pAction);
#else
  Q3PopupMenu * settingsMenu = new Q3PopupMenu(this);
  settingsMenu->insertItem("&History", this, SLOT(setHistory()));
  action = settingsMenu->addAction("&Loop Playback");
  action->setCheckable(true);
  QObject::connect(action, SIGNAL(toggled(bool)), this, SLOT(enableLoopPlayback(bool)) );
  action->setChecked(m_loopPlay);
  action = settingsMenu->addAction("Always on &Top / Transparent");
  action->setCheckable(true);
  QObject::connect(action, SIGNAL(toggled(bool)), this, SLOT(enableTransparency(bool)) );
  action->setChecked(false);
#endif

#ifdef LSB_Q3POPUPMENU
  // Done above
#else
  menuBar()->insertItem("&File", fileMenu);
  menuBar()->insertItem("&Edit", editMenu);
  menuBar()->insertItem("E&vents", eventMenu_ );
  menuBar()->insertItem("&Tools", toolsMenu_);
  menuBar()->insertItem("&Settings", settingsMenu);
#endif

  // the widgets
  QWidget * cw = new QWidget(this, "central widget");

  playButton = new QPushButton( "Play", cw, "play" );
  playButton->setMinimumSize(90, 30);

  stopButton = new QPushButton( "Stop", cw, "stop" );
  stopButton->setMinimumSize(90, 30);

  pauseButton = new QPushButton( "Pause", cw, "pause" );
  pauseButton->setMinimumSize(90, 30);

  nextButton = new QPushButton( "Next", cw, "next" );
  nextButton->setMinimumSize(90, 30);

  prevButton = new QPushButton( "Prev", cw, "prev" );
  prevButton->setMinimumSize(90, 30);

  minLabel = new QLCDNumber(2 , cw, "min");
  secLabel = new QLCDNumber(2, cw, "sec" );
  centiSecLabel = new QLCDNumber( 2, cw, "csec" );

  timeSlider = new QSlider( Qt::Horizontal, cw, "time" );
  timeSlider->setRange(0, 1000);
  timeSlider->setValue(0);

  speedDial = new QDial( -20, 20, 1, 0, cw, "speed" );
  speedDial->setWrapping(false);
  speedDial->setNotchesVisible(true);
  speedDial->setLineStep(2);
  speedDial->setPageStep(5);

  // ... add to the layout
#ifdef LSB_Q3HBOXLAYOUT
  QWidget * const topLayoutParent = cw;
  QHBoxLayout * const topLayout = new QHBoxLayout(topLayoutParent);
  assert(topLayout != NULL);
  {
    const int margin = 5;
    topLayout->setContentsMargins(margin, margin, margin, margin);
  }
#else
  Q3BoxLayout *topLayout = new Q3HBoxLayout(cw, 5);
#endif
#ifdef LSB_Q3VBOXLAYOUT
  QHBoxLayout * const layout1Parent = topLayout;
  QVBoxLayout * layout1 = new QVBoxLayout(layout1Parent);
  assert(layout1 != NULL);
  {
    const int margin = 5;
    layout1->setContentsMargins(margin, margin, margin, margin);
  }
#else
  Q3BoxLayout *layout1 = new Q3VBoxLayout(topLayout, 5);
#endif
#ifdef LSB_Q3GRIDLAYOUT
  // A layout is not a widget, and the parent is a layout
  QWidget * const pLayout2Parent = 0;
  QGridLayout * const layout2 = new QGridLayout(pLayout2Parent);
  assert(layout2 != NULL);
  // The grid that contains the buttons is the first of two child widgets
  // Add it to the parent before doing anything to it
  layout1->addLayout(layout2);
  const int margin = 5;
  layout2->setContentsMargins(margin, margin, margin, margin);
#else
  Q3GridLayout *layout2 = new Q3GridLayout(layout1, 2, 3, 5);
#endif
#ifdef LSB_Q3HBOXLAYOUT
  QWidget * const layout3Parent = NULL;
  QBoxLayout * const layout3 = new QHBoxLayout(layout3Parent);
  assert(layout3 != NULL);
  const int spacing = -1;
  layout3->setSpacing(spacing);
#else
  Q3BoxLayout *layout3 = new Q3HBoxLayout(-1, "time layout");
#endif

  layout2->addWidget(playButton, 0, 0);
  layout2->addWidget(stopButton, 0, 1);
  layout2->addWidget(pauseButton, 0, 2);
  layout2->addWidget(prevButton, 1, 0);
  layout2->addLayout(layout3, 1, 1);
  layout2->addWidget(nextButton, 1, 2);

  layout3->addWidget(minLabel);
  layout3->addWidget(secLabel);
  layout3->addWidget(centiSecLabel);

  layout1->addSpacing(10);
  layout1->addWidget(timeSlider);
  topLayout->addWidget(speedDial);

  statusBar();
  setCentralWidget(cw);

  // signals n slots
  connect( timer_, SIGNAL( timeout() ),
           (MainForm *)this, SLOT( step() ) );

  connect( autoStartTimer_, SIGNAL( timeout() ),
           (MainForm *)this, SLOT( startOnConsumers() ) );

  connect( timeSlider, SIGNAL( valueChanged(int) ),
           this, SLOT( changed(int) ) );
  connect( timeSlider, SIGNAL( sliderPressed() ),
           (MainForm *)this, SLOT( beginAction() ) );
  connect( timeSlider, SIGNAL( sliderReleased() ),
           (MainForm *)this, SLOT( endAction() ) );
  connect( timeSlider, SIGNAL( sliderMoved( int ) ),
           (MainForm *)this, SLOT( timeAction( int ) ) );

  connect( &fileSet_, SIGNAL( intervalChange() ),
           (MainForm *)this, SLOT( scaleSlider() ));
  connect( &fileSet_, SIGNAL( coursorChange() ),
           (MainForm *)this, SLOT( setSlider() ));

  connect( playButton, SIGNAL( clicked() ),
           (MainForm *)this, SLOT( play() ) );
  connect( stopButton, SIGNAL( clicked() ),
           (MainForm *)this, SLOT( stop() ) );
  connect( pauseButton, SIGNAL( clicked() ),
           (MainForm *)this, SLOT( pause() ) );
  connect( prevButton, SIGNAL( clicked() ),
           (MainForm *)this, SLOT( prev() ) );
  connect( nextButton, SIGNAL( clicked() ),
           (MainForm *)this, SLOT( next() ) );

  connect( speedDial, SIGNAL( valueChanged( int ) ),
           (MainForm *)this, SLOT( speed( int ) ) );
  connect( speedDial, SIGNAL( dialPressed() ),
           (MainForm *)this, SLOT( beginAction() ) );
  connect( speedDial, SIGNAL( dialReleased() ),
           (MainForm *)this, SLOT( endAction() ) );


  connect (this, SIGNAL(excludeEvent(const QString&, const QString&)),
           &fileSet_, SLOT(addExclude(const QString&, const QString&)));
  connect (this, SIGNAL(includeEvent(const QString&, const QString&)),
           &fileSet_, SLOT(delExclude(const QString&, const QString&)));

  enableButtons(fileSet_.size() != 0);
}

void MainForm::enableTransparency(bool enabled)
{
  Qt::WindowFlags wFlags = this->windowFlags();
  if (enabled) {
    m_opacity = 0.5f;
    setWindowFlags(wFlags|Qt::WindowStaysOnTopHint);
  }
  else {
    m_opacity = 1;
    setWindowOpacity(m_opacity);
    setWindowFlags(wFlags^Qt::WindowStaysOnTopHint);
  }
  this->show();
}

void MainForm::enterEvent(QEvent*)
{
  if (windowOpacity() < 0.99)
    setWindowOpacity(1);
}
void MainForm::leaveEvent(QEvent*)
{
  if (m_opacity < 0.99)
    setWindowOpacity(m_opacity);
}

void
MainForm::addExclude(QString const& _eventName)
{
#ifdef LSB_Q3POPUPMENU
  QObjQActionQStrMap::const_iterator first, last = eventMenuTypesMap_.end();
#else
  QObjIntQStrMap::const_iterator first, last = eventMenuTypesMap_.end();
#endif
  for (first = eventMenuTypesMap_.begin(); first != last; ++first) {

    QObject * obj = first->first;

#ifdef LSB_Q3POPUPMENU
    QActionQStrMap::const_iterator f, l = first->second.end();
    QActionQStrMap::const_iterator typeIt = l;
#else
    IntQStrMap::const_iterator f, l = first->second.end();
    IntQStrMap::const_iterator typeIt = l;
#endif
    for (f = first->second.begin(); f != l; ++f) {
      if (f->second == _eventName) {
        typeIt = f;
      }
    }

    if (typeIt != l) {
#ifdef LSB_Q3POPUPMENU
      QAction * const pQAction = typeIt->first;
#else
      int eventTypeId = typeIt->first;
#endif
      QString domainName = obj->objectName();
#ifdef LSB_Q3POPUPMENU
      pQAction->setChecked(false);
#else
      Q3PopupMenu * domainNameMenu = dynamic_cast<Q3PopupMenu *>(obj);

      domainNameMenu->setItemChecked(eventTypeId, false);
#endif
      // enable event
      //      cout << "disable event: " << domainName.latin1() << "  " << _eventName.latin1() << endl;
      emit excludeEvent(domainName, _eventName);
    }
  }
}

void
MainForm::enableButtons(bool _flag)
{
  playButton->setEnabled(_flag);
  stopButton->setEnabled(_flag);
  pauseButton->setEnabled(_flag);
  nextButton->setEnabled(_flag);
  prevButton->setEnabled(_flag);
  timeSlider->setEnabled(_flag);
  speedDial->setEnabled(_flag);

  if (!_flag) {
    timeSlider->setValue(0);
    centiSecLabel->display(0);
    secLabel->display(0);
    minLabel->display(0);
    speedDial->setValue(0);
  }

  eventMenu_->setEnabled(eventMenu_->count() != 0);
}

void
MainForm::scaleSlider()
{
  ACE_Time_Value d = fileSet_.cutEndTime() - fileSet_.cutStartTime();
  timeSlider->setMinValue(0);
  timeSlider->setMaxValue(d.sec() * 100 + d.usec() / 10000);
  setSlider();
}

void
MainForm::setSlider()
{
  ACE_Time_Value d = fileSet_.coursorTime() - fileSet_.cutStartTime();
  timeSlider->setValue(d.sec() * 100 + d.usec() / 10000);
}


void
MainForm::files()
{
  QStringList oldFiles = fileSet_.files();
  fileListDialog_->init(oldFiles);
  if (fileListDialog_->exec() &&
              fileListDialog_->modified()) {
    QStringList newFiles = fileListDialog_->result();

    QStringList::Iterator first, last = oldFiles.end();
    for (first = oldFiles.begin(); first != last; ++first) {
      if (newFiles.find(*first) == newFiles.end())
        fileSet_.delFile(*first);
    }

    last = newFiles.end();
    for (first = newFiles.begin(); first != last; ++first) {
      if (oldFiles.find(*first) == oldFiles.end())
        loadFile(*first);
    }
    enableButtons(fileSet_.size() != 0);
  }
}

void
MainForm::calcStartTime()
{
  timeBase_ = ACE_OS::gettimeofday();
  timeCBase_ = fileSet_.coursorTime();
}

void
MainForm::changed( int )
{
  ACE_Time_Value t = fileSet_.coursorTime() - fileSet_.cutStartTime();

  minLabel->display( int(t.sec() / 60) );
  secLabel->display( int(t.sec() % 60) );
  centiSecLabel->display( int(t.usec() / 10000 ) );
}

void
MainForm::play()
{
  calcStartTime();
  timer_->start(0);
}

void
MainForm::stop()
{
  timer_->stop();
  fileSet_.coursorTime(fileSet_.cutStartTime());
}

void MainForm::pause()
{
  timer_->stop();
}

void
MainForm::next()
{
  if (!timer_->isActive()) {
    fileSet_.playEvents(fileSet_.coursorTime());
  }
}

void
MainForm::prev()
{
  if (!timer_->isActive()) {
    fileSet_.playBackwards();

  }
}

void
MainForm::step()
{
  if (!action_) {

    ACE_Time_Value destTime = ACE_OS::gettimeofday() + MIN_TIME - timeBase_;
    destTime *= (speed_ > 0)? (double)speed_ : (1. / (double)-speed_);
    destTime += timeCBase_;
    destTime = std::min(destTime, fileSet_.coursorTime() + ACE_Time_Value(0, 200000));
    destTime = std::min(destTime, fileSet_.cutEndTime());

    fileSet_.playEvents(destTime);

    if (fileSet_.coursorTime() < fileSet_.cutEndTime()) {
      // rescedule timer
      ACE_Time_Value nextTime = ACE_OS::gettimeofday() - timeBase_;
      nextTime *= (speed_ > 0)? (double)speed_ : (1. / (double)-speed_);
      nextTime += timeCBase_;

      int interval = (fileSet_.coursorTime() <= nextTime || speed_ == 20)?
                     0 :
                     (fileSet_.coursorTime() - nextTime).msec();
      timer_->changeInterval(interval);
    }
    else {
      if (exitOnReplayEnd_) {
        timer_->stop();
        emit qApp->quit();
      }
      else {
        // reset cursor to beginning of file
        beginAction();
        timeAction(0);
        endAction();
        if(!m_loopPlay) { // stop timer if we're not looping
          timer_->stop();
        }
      }
    }
  }
}

void
MainForm::beginAction()
{
  action_ = true;
}

void
MainForm::endAction()
{
  calcStartTime();
  action_ = false;
}

void
MainForm::setStartOnConsumers(int _numConsumers)
{
  numConsumers_ = _numConsumers;
  autoStartTimer_->start(100);
}


void
MainForm::startOnConsumers()
{
  ChannelManager::ChannelMap::const_iterator first, last = fileSet_.channelManager()->channelMap().end();
  int sumConsumers = 0;
  for (first = fileSet_.channelManager()->channelMap().begin(); first != last; ++first) {
    CosNotifyChannelAdmin::AdminIDSeq_var admins =
      first->second->get_all_consumeradmins();
    sumConsumers += admins->length();
  }

  if (sumConsumers >= numConsumers_) {
    play();
    autoStartTimer_->stop();
  }
}

void
MainForm::timeAction(int hsec )
{
  ACE_Time_Value t(hsec / 100, (hsec % 100) * 10000);
  fileSet_.coursorTime(fileSet_.cutStartTime() + t);
}

void
MainForm::speed( int _speed)
{
  speed_ = _speed;
  if (speed_ == 0)
    speed_ = 1;
}

void
MainForm::setSpeed(int _speed)
{
  speed_ = _speed;
  if (speed_ == 0)
    speed_ = 1;

  speedDial->setValue((speed_ == 1)? 0 : speed_);
}

void
MainForm::loadFile(QString const & _name )
{
  try {
    LogFile * file = fileSet_.addFile(_name);
    try {
#ifdef LSB_Q3PROGRESSDIALOG
      const QString labelText("Parsing log file " + _name);
      const QString cancelButtonText("Cancel");
      const int minimum = 0;
      const int maximum = 100;
      QWidget * const parent = this;
      QProgressDialog progress(labelText, cancelButtonText, minimum, maximum,
			       parent);
      progress.setValue(0);
#else
      Q3ProgressDialog progress("Parsing log file " + _name, "Cancel", 100,
                                this, "progress", TRUE);
      progress.setProgress(0);
#endif
      int percent;

      while ((percent = file->parse()) != 100) {
#ifdef LSB_Q3PROGRESSDIALOG
	progress.setValue(percent);
#else
        progress.setProgress(percent);
#endif
        qApp->processEvents();
#ifdef LSB_Q3PROGRESSDIALOG
	if (progress.wasCanceled())
#else
        if (progress.wasCancelled())
#endif
          throw Miro::Exception("canceled");
      }
#ifdef LSB_Q3PROGRESSDIALOG
      progress.setValue(100);
#endif

      if (file->endTime() > ACE_OS::gettimeofday())
        throw Miro::Exception("Clock screw detected:\nEnd time of file lies in the future.");

      statusBar()->message(QString("loaded file: ") + _name, 5000);
      fileSet_.calcStartEndTime();
      fileSet_.coursorTime(fileSet_.cutStartTime());

      connect(file, SIGNAL(notifyEvent(const QString&)),
              statusBar(), SLOT(message(const QString&)));
      if (eventView_) {
        connect(this, SIGNAL(excludeEvent(const QString&, const QString&)),
                eventView_, SLOT(excludeEvent(const QString&, const QString&)));
        connect(this, SIGNAL(includeEvent(const QString&, const QString&)),
                eventView_, SLOT(includeEvent(const QString&, const QString&)));
        connect(file, SIGNAL(newEvent(const QString&,const QString&,const QString&,const QString&)),
                eventView_, SLOT(insertEvent(const QString&,const QString&,const QString&,const QString&)));
      }
    }
    catch (Miro::Exception const& e) {
      fileSet_.delFile(_name);
      if (strcmp("canceled", e.what()) == 0)
        statusBar()->message("loading file canceled.", 5000);
      else
        QMessageBox::warning(this, "Error parsing file:",
                             QString("File ") + _name + QString(":\n") +
                             QString(e.what()));
    }
  }
  catch (Miro::CException const& e) {
    QMessageBox::warning(this, "Error loading file:",
                         QString("File ") + _name + QString(":\n") +
                         QString(e.what()));
  }
  catch (Miro::Exception const& e) {
    QMessageBox::warning(this, "Error parsing file:",
                         QString("File ") + _name + QString(":\n") +
                         QString(e.what()));
  }
  createEventMenu();
  enableButtons(fileSet_.size() != 0);
}

void
MainForm::createEventMenu()
{
  FileSet::DNETMap m = fileSet_.typeNames();

  fileSet_.clearExclude();
  eventMenu_->clear();
  eventMenuTypesMap_.clear();

  FileSet::DNETMap::const_iterator first, last = m.end();
  for (first = m.begin(); first != last; ++first) {
#ifdef LSB_Q3POPUPMENU
    const QString& menuName = first->first;
    QMenu * typeNames = eventMenu_->addMenu(tr(menuName)); 
#else
    Q3PopupMenu *typeNames = new Q3PopupMenu(eventMenu_);
#endif
    typeNames->setObjectName(first->first);
    SubmenuEvent *submenuEvent = new SubmenuEvent(typeNames, "subEvent");

    connect(typeNames, SIGNAL(activated(int)), submenuEvent, SLOT(action(int)));
    connect(submenuEvent, SIGNAL(activated(QObject*, int)), this, SLOT(toggleExcludeEvent(QObject*, int)));

#ifdef LSB_Q3POPUPMENU
    // Build this map and save it in a member variable
    QActionQStrMap events;
    LogFile::CStringSet::const_iterator f, l = first->second.end();
    for (f = first->second.begin(); f != l; ++f) {
      // The menu item text
      const QString text = *f;
      /// @todo Is this->eventMenu_ the parent? If not, what is?
      QAction * const pAction  = new QAction(text, eventMenu_);
      /// @todo Does the Action get connected to a slot? If so, which one?
      pAction->setCheckable(true);
      pAction->setChecked(true);
      typeNames->addAction(pAction);
      // Map the QAction to the QString
      events[pAction] = QString(*f);
    }

    // Save the map in a member variable
    eventMenuTypesMap_[typeNames] = events;

#else
    typeNames->setCheckable(true);
    LogFile::CStringSet::const_iterator f, l = first->second.end();
    IntQStrMap events;
    for (f = first->second.begin(); f != l; ++f) {
      int id = typeNames->insertItem(QString(*f));
      typeNames->setItemChecked(id, true);
      events[id] = QString(*f);
    }

    eventMenu_->insertItem(first->first, typeNames);
    eventMenuTypesMap_[typeNames] = events;
#endif
  }
}

void
#ifdef LSB_Q3POPUPMENU
MainForm::toggleExcludeEvent(QObject* const obj, QAction * pQAction)
#else
MainForm::toggleExcludeEvent(QObject* obj, int _eventTypeId)
#endif
{
  QString domainName = obj->objectName();
#ifdef LSB_Q3POPUPMENU
  const QString& s = eventMenuTypesMap_[obj][pQAction];
  if (pQAction->isChecked()) { emit includeEvent(domainName, s); } // enable
  else                       { emit excludeEvent(domainName, s); } // disable
#else
  Q3PopupMenu * domainNameMenu = dynamic_cast<Q3PopupMenu *>(obj);

  // enable event
  if (domainNameMenu->isItemChecked(_eventTypeId)) {
    //cout << "enable event: " << domainName.latin1() << ", " << eventMenuTypesMap_[obj][_eventTypeId].latin1() << endl;
    emit includeEvent(domainName, eventMenuTypesMap_[obj][_eventTypeId]);
  }
  // disable event
  else {
    //cout << "disable event: " << domainName.latin1() << "  " << eventMenuTypesMap_[obj][_eventTypeId].latin1() << endl;
    emit excludeEvent(domainName, eventMenuTypesMap_[obj][_eventTypeId]);
  }
#endif
}

void
MainForm::toggleEventView()
{
  if (eventView_ == NULL) {
    eventView_ = new EventView(&fileSet_, history_, "event view");

#ifdef LSB_Q3POPUPMENU
    assert(m_pToggleEventViewAction != NULL);
    m_pToggleEventViewAction->setChecked(true);
#else
    toolsMenu_->setItemChecked(eventViewId_, true);
#endif

    connect (this, SIGNAL(excludeEvent(const QString&, const QString&)),
             eventView_, SLOT(excludeEvent(const QString&, const QString&)));
    connect (this, SIGNAL(includeEvent(const QString&, const QString&)),
             eventView_, SLOT(includeEvent(const QString&, const QString&)));
    connect(eventView_, SIGNAL(destroyed()),
            this, SLOT( eventViewClosed()));
  }
  else {
    delete eventView_;
  }
}

void
MainForm::eventViewClosed()
{
  eventView_ = NULL;
#ifdef LSB_Q3POPUPMENU
  assert(m_pToggleEventViewAction != NULL);
  m_pToggleEventViewAction->setChecked(false);
#else
  toolsMenu_->setItemChecked(eventViewId_, false);
#endif
}

void
MainForm::setHistory()
{
  bool ok = FALSE;
  int res = QInputDialog::getInteger(
              tr( "LogPlayer" ),
              tr( "History Size" ), history_, 1, 10000, 10, &ok, this );
  if ( ok ) {
    history_ = res; // user entered something and pressed OK
    if (eventView_) {
      eventView_->setHistory(history_);
    }
  }
}

void
MainForm::saveAs()
{
  static QString filename = ".";
  if (filename.isEmpty()) {
    filename = ".";
  }

  // show file dialog
  filename = QFileDialog::getSaveFileName(this, "Save Log File",
                                          filename,
                                          logFilter);
  if (!filename.isEmpty()) {
    // append file extension if none is given
    QFileInfo file(filename);
    if (file.suffix().isEmpty()) {
      filename.append(".mlog");
      file.setFile(filename);
    }
    // save the file
    fileSet_.saveCut(filename);
  }
}

void
MainForm::cutFront()
{
  fileSet_.cutStart();
}

void
MainForm::cutBack()
{
  fileSet_.cutEnd();
}

void
MainForm::cutUndo()
{
  fileSet_.cutUndo();
}

void
MainForm::exitOnReplayEnd(bool flag)
{
  exitOnReplayEnd_ = flag;
}

void MainForm::enableLoopPlayback(bool enabled)
{
  m_loopPlay = enabled;
}
