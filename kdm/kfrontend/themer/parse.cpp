/*
 *  Copyright (C) 2003 by Unai Garro <ugarro@users.sourceforge.net>
 *  Copyright (C) 2004 by Enrico Ros <rosenric@dei.unipd.it>
 *  Copyright (C) 2004 by Stephan Kulow <coolo@kde.org>
 *  Copyright (C) 2004,2006 by Oswald Buddenhagen <ossi@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "parse.h"

#include <QString>
#include <QStringList>
#include <QFont>
#include <QColor>

void
parseSize( const QString &s, DataPoint &pt )
{
	if (s.isEmpty())
		return;

	int p;
	if (s == "box") {	// box value
		pt.type = DTbox;
		pt.val = 0;
	} else if (s == "scale") {
		pt.type = DTscale;
		pt.val = 0;
	} else if ((p = s.indexOf( '%' )) >= 0) {	// percent value
		pt.type = DTpercent;
		QString sCopy = s;
		sCopy.remove( p, 1 );
		pt.levels = 0;
		while ((p = sCopy.indexOf( '^' )) >= 0) {
			sCopy.remove( p, 1 );
			pt.levels++;
		}
		sCopy.replace( ',', '.' );
		pt.val = (int)sCopy.toDouble();
	} else {		// int value
		pt.type = DTpixel;
		QString sCopy = s;
		if (sCopy.at( 0 ) == '-') {
			sCopy.remove( 0, 1 );
			pt.type = DTnpixel;
		}
		sCopy.replace( ',', '.' );
		pt.val = (int)sCopy.toDouble();
	}
}


static QString
getword( QString &rs )
{
	int splitAt = rs.lastIndexOf( ' ' ) + 1;
	QString s( rs.mid( splitAt ) );
	rs.truncate( splitAt - 1 );
	return s;
}

void
parseFont( const QString &is, QFont &font )
{
	QString rs( is.simplified() );
	QString s( getword( rs ) );
	bool ok;
	if (s.endsWith( "px" )) {
		int ps = s.left( s.length() - 2 ).toInt( &ok );
		if (ok) {
			font.setPixelSize( ps );
			s = getword( rs );
		}
	} else {
		double ps = s.toDouble( &ok );
		if (ok) {
			font.setPointSizeF( ps );
			s = getword( rs );
		}
	}
	forever {
		QString ss( s.toLower() );
		if (ss == "oblique")
			font.setStyle( QFont::StyleOblique );
		else if (ss == "italic")
			font.setStyle( QFont::StyleItalic );
		else if (ss == "ultra-light")
			font.setWeight( 13 );
		else if (ss == "light")
			font.setWeight( QFont::Light );
		else if (ss == "medium")
			font.setWeight( 50 );
		else if (ss == "semi-bold")
			font.setWeight( QFont::DemiBold );
		else if (ss == "bold")
			font.setWeight( QFont::Bold );
		else if (ss == "ultra-bold")
			font.setWeight( QFont::Black );
		else if (ss == "heavy")
			font.setWeight( 99 );
		else if (ss == "ultra-condensed")
			font.setStretch( QFont::UltraCondensed );
		else if (ss == "extra-condensed")
			font.setStretch( QFont::ExtraCondensed );
		else if (ss == "condensed")
			font.setStretch( QFont::Condensed );
		else if (ss == "semi-condensed")
			font.setStretch( QFont::SemiCondensed );
		else if (ss == "semi-expanded")
			font.setStretch( QFont::SemiExpanded );
		else if (ss == "expanded")
			font.setStretch( QFont::Expanded );
		else if (ss == "extra-expanded")
			font.setStretch( QFont::ExtraExpanded );
		else if (ss == "ultra-expanded")
			font.setStretch( QFont::UltraExpanded );
		else if (ss == "normal" || // no-op
		         ss == "small-caps" || // this and following ignored
		         ss == "not-rotated" || ss == "south" || ss == "upside-down" ||
		         ss == "north" ||
		         ss == "rotated-left" || ss == "east" ||
		         ss == "rotated-right" || ss == "west")
		{
		} else
			break;
		s = getword( rs );
	}
	if (!rs.isEmpty())
		rs.append( ' ' ).append( s );
	else
		rs = s;
	QStringList ffs = rs.split( QRegExp( " ?, ?" ), QString::SkipEmptyParts );
	if (!ffs.isEmpty()) {
		foreach (QString ff, ffs) {
			font.setFamily( ff );
			if (font.exactMatch())
				return;
		}
		font.setFamily( ffs.first() );
	}
}


void
parseColor( const QString &s, const QString &a, QColor &color )
{
	if (!s.length() || s.at( 0 ) != '#')
		return;
	bool ok;
	QString sCopy = s;
	uint hexColor = sCopy.remove( 0, 1 ).toUInt( &ok, 16 );
	if (ok) {
		if (sCopy.length() == 8)
			color.setRgba( hexColor );
		else {
			color.setRgb( hexColor );
			if (!a.isNull())
				color.setAlpha( int(a.toFloat() * 255) );
		}
	}
}
