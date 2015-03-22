#include "stdafx.h"

QString qtToUnicode( const char* str )
{
	QTextCodec *codec = QTextCodec::codecForLocale();
	return codec->toUnicode ( str );
}
