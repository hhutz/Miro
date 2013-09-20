#ifndef miro_qt_compatibility_h
#define miro_qt_compatibility_h

// temporary compatibility file for porting to Qt4
// the Qt3Support library from trolltech is heavyweight
// and has some problems...

#if QT_VERSION >= 0x040000

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QString& str) {	
   return os << qPrintable(str);
}

#else // QT_VERSION >= 0x040000

#define qPrintable( qstring ) qstring.latin1()

#endif // QT_VERSION >= 0x040000

#endif // miro_qt_compatibility_h
