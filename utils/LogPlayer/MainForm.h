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
#ifndef Mainform_h
#define MainForm_h

#include <ace/Time_Value.h>

#include <q3mainwindow.h>
#include <qstring.h>

#include <Q3PopupMenu>

#include <map>

// forward declarations
class FileSet;
class FileListDialog;
class EventView;

class QApplication;
class Q3PopupMenu;
class QTimer;
class QSlider;
class QLCDNumber;
class QDial;

typedef std::map<int, QString> IntQStrMap;
typedef std::map<QObject *, IntQStrMap> QObjIntQStrMap;


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

class MainForm : public Q3MainWindow
{
  Q_OBJECT

  typedef Q3MainWindow Super;
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

  void toggleExcludeEvent(QObject*, int);

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

  Q3PopupMenu *     eventMenu_;
  Q3PopupMenu *     toolsMenu_;

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
  int eventViewId_;

  bool  exitOnReplayEnd_;
  int   numConsumers_;

  QObjIntQStrMap eventMenuTypesMap_;

  static ACE_Time_Value const MIN_TIME;

  float  m_opacity;
  bool   m_loopPlay;
};

#endif
