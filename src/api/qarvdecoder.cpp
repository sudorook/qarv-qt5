/*
    qarv, a Qt interface to aravis.
    Copyright (C) 2012  Jure Varlec <jure.varlec@ad-vega.si>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "api/qarvdecoder.h"
#include <QPluginLoader>

QList< QArvPixelFormat* > QArvPixelFormat::supportedFormats() {
  auto plugins = QPluginLoader::staticInstances();
  QList<QArvPixelFormat*> list;
  foreach (auto plugin, plugins) {
    auto fmt = qobject_cast<QArvPixelFormat*>(plugin);
    if (fmt != NULL) list << fmt;
  }
  return list;
}

/*!
 * Returns NULL if the format is not supported.
 */
QArvDecoder* QArvPixelFormat::makeDecoder(QString format, QSize size) {
  auto list = supportedFormats();
  foreach (auto fmt, list) {
    if (fmt->pixelFormat() == format) return fmt->makeDecoder(size);
  }
  return NULL;
}