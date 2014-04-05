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
// #define LSB_Q3MAINWINDOW
// #define LSB_Q3POPUPMENU

#ifndef Mainform_h
#define MainForm_h

#include <ace/Time_Value.h>

#ifdef LSB_Q3MAINWINDOW
#include <QMainWindow>
#else
#include <q3mainwindow.h>
#endif
#include <qstring.h>

#ifdef LSB_Q3POPUPMENU
#else
#include <Q3PopupMenu>
#endif

#include <map>

// forward declarations
class FileSet;
class FileListDialog;
class EventView;

class QApplication;
#ifdef LSB_Q3POPUPMENU
class QAction;
class QMenu;
#else
class Q3PopupMenu;
#endif
class QTimer;
class QSlider;
class QLCDNumber;
class QDial;

#ifdef LSB_Q3POPUPMENU
typedef std::map<QAction*, QString> QActionQStrMap;
typedef std::map<QObject*, QActionQStrMap> QObjQActionQStrMap;
#else
typedef std::map<int, QString> IntQStrMap;
typedef std::map<QObject *, IntQStrMap> QObjIntQStrMap;
#endif


class SubmenuEvent : public QObject
{
  Q_OBJECT

private:
  typedef QObject Super;

public:
  SubmenuEvent(QObject * parent, char const * name);

public slots:
  void action(int);

signals:
  void activated(QObject *, int);
};

class MainForm : public
#ifdef LSB_Q3MAINWINDOW
QMainWindow
#else
Q3MainWindow
#endif
{
  Q_OBJECT

#ifdef LSB_Q3MAINWINDOW
  typedef QMainWindow Super;
#else
  typedef Q3MainWindow Super;
#endif

public:
  MainForm(QApplication& _app, FileSet& _fileSet,
           QWidget * parent = 0, const char * name = 0 );

  void loadFile(QString const & _name);
  void calcStartTime();

  void addExclude(QString const & _eventName);

  void exitOnReplayEnd(bool flag);
  void setSpeed(int);
  void setStartOnConsumers(int _numConsumers);

  void enterEvent(QEvent*);
  void leaveEvent(QEvent*);

public slots:
  void pause();
  void play();
  void stop();
  void next();
  void prev();
  void files();
  void changed( int );
  void step();
  void beginAction();
  void endAction();
  void timeAction( int );
  void speed( int );

  void scaleSlider();
  void setSlider();
  void setHistory();

#ifdef LSB_Q3POPUPMENU
  void toggleExcludeEvent(QObject * const obj, QAction * const pQAction);
#else
  void toggleExcludeEvent(QObject*, int);
#endif

  void toggleEventView();
  void eventViewClosed();

  void saveAs();

  void cutFront();
  void cutBack();
  void cutUndo();

  void startOnConsumers();

  void enableTransparency(bool enabled);
  void enableLoopPlayback(bool enabled);

signals:
  void excludeEvent(const QString &, const QString&);
  void includeEvent(const QString &, const QString&);

protected:
  void enableButtons(bool _flag);
  void createEventMenu();

  QApplication&     app_;
  FileSet&          fileSet_;
  FileListDialog *  fileListDialog_;
  EventView *       eventView_;
  QTimer *          timer_;
  QTimer *          autoStartTimer_;

#ifdef LSB_Q3POPUPMENU
  QMenu *     eventMenu_;
  QMenu *     toolsMenu_;
#else
  Q3PopupMenu *     eventMenu_;
  Q3PopupMenu *     toolsMenu_;
#endif

  QWidget *     playButton;
  QWidget *     stopButton;
  QWidget *     pauseButton;
  QWidget *     nextButton;
  QWidget *     prevButton;
  QSlider *     timeSlider;
  QLCDNumber *  centiSecLabel;
  QLCDNumber *  secLabel;
  QLCDNumber *  minLabel;
  QDial *       speedDial;

  bool  action_;
  int   speed_;
  int   history_;
  ACE_Time_Value timeBase_;
  ACE_Time_Value timeCBase_;

  int domainNameMenuId_;

#ifdef LSB_Q3POPUPMENU
  QAction * m_pToggleEventViewAction;
#else
  int eventViewId_;
#endif

  bool  exitOnReplayEnd_;
  int   numConsumers_;

#ifdef LSB_Q3POPUPMENU
  // Maps a QObject* onto a map from QAction* to QString
  QObjQActionQStrMap eventMenuTypesMap_;
#else
  QObjIntQStrMap eventMenuTypesMap_;
#endif

  static ACE_Time_Value const MIN_TIME;

  float  m_opacity;
  bool   m_loopPlay;
};

#endif
